#!/usr/bin/env python3

###
# Generates build files for the project.
# This file also includes the project configuration,
# such as compiler flags and the object matching status.
#
# Usage:
#   python3 configure.py
#   ninja
#
# Append --help to see available options.
###

import argparse
import sys
from pathlib import Path
from typing import List

from tools.project import (
    Object,
    ProgressCategory,
    ProjectConfig,
    calculate_progress,
    generate_build,
    is_windows,
)

# Game versions
DEFAULT_VERSION = 0
VERSIONS = [
    "R4QE01",  # 0: USA, Rev 1
]

parser = argparse.ArgumentParser()
parser.add_argument(
    "mode",
    choices=["configure", "progress"],
    default="configure",
    help="script mode (default: configure)",
    nargs="?",
)
parser.add_argument(
    "-v",
    "--version",
    choices=VERSIONS,
    type=str.upper,
    default=VERSIONS[DEFAULT_VERSION],
    help="version to build",
)
parser.add_argument(
    "--build-dir",
    metavar="DIR",
    type=Path,
    default=Path("build"),
    help="base build directory (default: build)",
)
parser.add_argument(
    "--binutils",
    metavar="BINARY",
    type=Path,
    help="path to binutils (optional)",
)
parser.add_argument(
    "--compilers",
    metavar="DIR",
    type=Path,
    help="path to compilers (optional)",
)
parser.add_argument(
    "--map",
    action="store_true",
    help="generate map file(s)",
)
parser.add_argument(
    "--debug",
    action="store_true",
    help="build with debug info (non-matching)",
)
if not is_windows():
    parser.add_argument(
        "--wrapper",
        metavar="BINARY",
        type=Path,
        help="path to wibo or wine (optional)",
    )
parser.add_argument(
    "--dtk",
    metavar="BINARY | DIR",
    type=Path,
    help="path to decomp-toolkit binary or source (optional)",
)
parser.add_argument(
    "--objdiff",
    metavar="BINARY | DIR",
    type=Path,
    help="path to objdiff-cli binary or source (optional)",
)
parser.add_argument(
    "--sjiswrap",
    metavar="EXE",
    type=Path,
    help="path to sjiswrap.exe (optional)",
)
parser.add_argument(
    "--ninja",
    metavar="BINARY",
    type=Path,
    help="path to ninja binary (optional)",
)
parser.add_argument(
    "--verbose",
    action="store_true",
    help="print verbose output",
)
parser.add_argument(
    "--non-matching",
    dest="non_matching",
    action="store_true",
    help="builds equivalent (but non-matching) or modded objects",
)
parser.add_argument(
    "--warn",
    dest="warn",
    type=str,
    choices=["all", "off", "error"],
    help="how to handle warnings",
)
parser.add_argument(
    "--no-progress",
    dest="progress",
    action="store_false",
    help="disable progress calculation",
)
args = parser.parse_args()

config = ProjectConfig()
config.version = str(args.version)
version_num = VERSIONS.index(config.version)

# Apply arguments
config.build_dir = args.build_dir
config.dtk_path = args.dtk
config.objdiff_path = args.objdiff
config.binutils_path = args.binutils
config.compilers_path = args.compilers
config.generate_map = args.map
config.non_matching = args.non_matching
config.sjiswrap_path = args.sjiswrap
config.ninja_path = args.ninja
config.progress = args.progress
if not is_windows():
    config.wrapper = args.wrapper
# Don't build asm unless we're --non-matching
if not config.non_matching:
    config.asm_dir = None

# Tool versions
config.binutils_tag = "2.42-2"
config.compilers_tag = "20251118"
config.dtk_tag = "v1.8.3"
config.objdiff_tag = "v3.6.1"
config.sjiswrap_tag = "v1.2.2"
config.wibo_tag = "1.0.3"

# Project
config.config_path = Path("config") / config.version / "config.yml"
config.check_sha_path = Path("config") / config.version / "build.sha1"
config.asflags = [
    "-mgekko",
    "--strip-local-absolute",
    "-I include",
    f"-I build/{config.version}/include",
    f"--defsym BUILD_VERSION={version_num}",
]
config.ldflags = [
    "-fp hardware",
    "-nodefaults",
]
if args.debug:
    config.ldflags.append("-g")  # Or -gdwarf-2 for Wii linkers
if args.map:
    config.ldflags.append("-mapunused")
    # config.ldflags.append("-listclosure") # For Wii linkers

# Additional files that affect the split and generated build configuration
config.reconfig_deps = [
    config.config_path.parent / "splits.txt",
    config.config_path.parent / "symbols.txt",
]

# Optional numeric ID for decomp.me preset
# Can be overridden in libraries or objects
config.scratch_preset_id = None

# Base flags, common to most GC/Wii games.
# Generally leave untouched, with overrides added below.
cflags_base = [
    "-nodefaults",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hardware",
    "-Cpp_exceptions off",
    "-O4,p",
    '-pragma "cats off"',
    '-pragma "warn_notinlined off"',
    "-maxerrors 1",
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-str reuse",
    "-enc SJIS",
    "-i include",
    f"-i build/{config.version}/include",
    "-i libs/Runtime/include",
    "-i libs/MSL_C/include",
    "-i libs/RVL_SDK/include",
    "-i libs/MetroTRK/include",
    "-i src/ode",
    f"-DBUILD_VERSION={version_num}",
    f"-DVERSION_{config.version}",
]

# Debug flags
if args.debug:
    # Or -sym dwarf-2 for Wii compilers
    cflags_base.extend(["-sym on", "-DDEBUG=1"])
else:
    cflags_base.append("-DNDEBUG=1")

# Warning flags
if args.warn == "all":
    cflags_base.append("-W all")
elif args.warn == "off":
    cflags_base.append("-W off")
elif args.warn == "error":
    cflags_base.append("-W error")

# Metrowerks library flags
# The game library is built without data pooling. NL/nlStringSupport.cpp is the
# discriminating unit: with pooling on, CodeWarrior anchors the translation
# unit's .bss objects on a shared section base register and defers the two
# global allocator objects behind their destructor-chain records. R4QE01
# addresses every .bss object through its own symbol and places each record
# immediately before the object it registers.
cflags_game_common = [
    *cflags_base,
    "-use_lmw_stmw on",
    "-pool off",
    "-DdNODEBUG=1",
    "-DdSINGLE=1",
]

cflags_game = [
    *cflags_game_common,
    "-inline auto",
]

cflags_game_deferred = [
    *cflags_game_common,
    "-inline auto,deferred",
]

cflags_runtime = [
    *cflags_base,
    "-use_lmw_stmw on",
    "-str reuse,pool,readonly",
    "-gccinc",
    "-common off",
    "-inline auto",
]

# Revolution SDK library flags.
cflags_rvl_sdk = [
    *cflags_base,
    "-inline auto",
    "-ipa file",
    "-fp_contract off",
]

# Home Button Menu library flags. R4QE01 links the HBM build dated
# Dec  7 2006, whose nw4hbm assertions are compiled in: every retained
# ut/lyt/snd routine still calls nw4hbm::db::Panic with its source file name
# and an explicit line number.
cflags_rvl_hbm = [
    *cflags_rvl_sdk,
    "-DHBM_ASSERT",
]

# Open Dynamics Engine flags. R4QE01 uses the single-precision, assertions-off
# configuration retained from the GameCube predecessor. obstack.cpp excludes
# GC/2.7; all tested GC/3.0 revisions through 3.0a5.2 emit the same code.
cflags_ode = [
    *cflags_base,
    "-inline auto",
    "-char signed",
    "-use_lmw_stmw on",
    "-common off",
    "-DdNODEBUG=1",
    "-DdSINGLE=1",
    "-DdTHREADING_INTF_DISABLED",
    "-DHAVE_MALLOC_H=1",
]

config.linker_version = "GC/3.0a5"


Matching = True                   # Object matches and should be linked
NonMatching = False               # Object does not match and should not be linked
Equivalent = config.non_matching  # Object should be linked when configured with --non-matching


# Object is only matching for specific versions
def MatchingFor(*versions):
    return config.version in versions


config.warn_missing_config = True
config.warn_missing_source = False
config.libs = [
    {
        "lib": "Game",
        "mw_version": config.linker_version,
        "cflags": cflags_game,
        "progress_category": "game",
        "objects": [
            Object(Matching, "Game/AI/AiUtil.cpp"),
            Object(Matching, "Game/AnimInventory.cpp", extra_cflags=["-ipa file"]),
            Object(NonMatching, "Game/Character.cpp", extra_cflags=["-inline deferred"]),
            Object(NonMatching, "Game/AI/Fielder.cpp"),
            Object(NonMatching, "Game/Goalie.cpp", cflags=cflags_game_deferred),
            Object(NonMatching, "Game/AI/GoalieActions.cpp", cflags=cflags_game_deferred),
            Object(Matching, "Game/GoalieFatigue.cpp"),
            Object(NonMatching, "Game/AI/GoalieLooseBall.cpp", cflags=cflags_game_deferred),
            Object(NonMatching, "Game/AI/GoalieSave.cpp", cflags=cflags_game_deferred),
            Object(NonMatching, "Game/AI/Powerups.cpp"),
            Object(NonMatching, "Game/AI/Desire.cpp"),
            Object(Matching, "Game/AI/Fuzzy.cpp"),
            Object(Equivalent, "Game/AI/Variant.cpp", cflags=[*cflags_game_deferred, "-char signed"]),
            Object(Matching, "Game/DB/BasicGameInfo.cpp", extra_cflags=["-ipa file"]),
            Object(NonMatching, "Game/GameInfo.cpp", extra_cflags=["-ipa file"]),
            Object(NonMatching, "Game/NisPlayer.cpp", extra_cflags=["-inline deferred"]),
            Object(NonMatching, "Game/Render/Nis.cpp", extra_cflags=["-inline deferred"]),
            Object(Matching, "Game/AnimProps/globalanimproperties.cpp"),
            Object(Matching, "Game/AnimProps/goalieanimproperties.cpp"),
            Object(Matching, "NL/nlAVLTree.cpp"),
            Object(Matching, "NL/nlMath.cpp"),
            Object(Matching, "NL/nlMemory.cpp"),
            Object(Matching, "NL/nlPrint.cpp"),
            Object(Matching, "NL/nlTicker.cpp"),
            Object(Matching, "NL/nlString.cpp"),
            Object(Matching, "Game/Sys/simpleparser.cpp"),
            Object(NonMatching, "NL/MemAlloc.cpp"),
            Object(NonMatching, "NL/nlConfig.cpp"),
            Object(Matching, "NL/nlSlotPool.cpp"),
            Object(Matching, "NL/nlStringSupport.cpp", extra_cflags=["-ipa file"]),
            Object(Matching, "NL/nlTask.cpp"),
            Object(Matching, "NL/nlLocalization.cpp"),
            Object(Matching, "NL/nlBundleFile.cpp"),
            Object(NonMatching, "NL/nlTextBox.cpp"),
            Object(Matching, "Game/Sys/clock.cpp"),
            Object(Matching, "NL/nlTimer.cpp"),
            Object(Matching, "NL/nlEndian.cpp"),
            Object(Matching, "NL/gl/glStat.cpp"),
            Object(Matching, "NL/gl/glState.cpp"),
            Object(Matching, "NL/gl/glStruct.cpp"),
            Object(Matching, "NL/glx/glxGX.cpp"),
            Object(Matching, "NL/glx/glxSwap.cpp", extra_cflags=["-inline noauto"]),
            Object(Matching, "Game/Drawable/DrawableNetMesh.cpp"),
            Object(NonMatching, "Game/Drawable/DrawableCharacter.cpp"),
            Object(Matching, "Game/Drawable/DrawableBall.cpp"),
            Object(NonMatching, "Game/Drawable/DrawableBulletBill.cpp"),
            Object(NonMatching, "Game/Drawable/DrawableBirdoEgg.cpp"),
            Object(NonMatching, "Game/Drawable/DrawableYoshiEgg.cpp"),
            Object(NonMatching, "Game/Drawable/DrawableFlyingCamera.cpp"),
            Object(NonMatching, "Game/Drawable/DrawableKoopaShell.cpp"),
            Object(Matching, "Game/Drawable/DrawablePowerup.cpp"),
            Object(NonMatching, "Game/Drawable/DrawableThwomp.cpp"),
            Object(Matching, "Game/Task/WorldUpdateTask.cpp"),
            Object(Matching, "Game/Task/EndFrameTask.cpp"),
            Object(Matching, "Game/Task/NetworkUpdateTask.cpp"),
            Object(Matching, "Game/Task/PlatPadUpdateTask.cpp"),
            Object(Matching, "Game/Task/DispatchEventsTask.cpp"),
            Object(NonMatching, "Game/Task/TransitionTask.cpp"),
            Object(Matching, "Game/Task/LoadingTask.cpp"),
            Object(Matching, "Game/Task/SmokeTestUpdateTask.cpp"),
            Object(Matching, "Game/Task/TextWindowTask.cpp"),
            Object(Matching, "Game/Task/TweakerTask.cpp"),
            Object(Matching, "Game/Task/ProfilerTask.cpp"),
            Object(Matching, "Game/Task/ParticleUpdateTask.cpp"),
            Object(Matching, "Game/FE/GameSceneManager.cpp"),
            Object(Matching, "Game/FE/BaseSceneHandler.cpp"),
            Object(Matching, "Game/SAnim/pnSAnimController.cpp", extra_cflags=["-inline deferred"]),
            Object(Matching, "Game/SAnim/pnBlender.cpp"),
            Object(Matching, "Game/SAnim/pnSingleAxisBlender.cpp"),
            Object(Matching, "Game/SAnim/pnFeather.cpp", extra_cflags=["-inline auto,depth=3"]),
            Object(Matching, "Game/Physics/CollisionSpace.cpp"),
            Object(Matching, "Game/Physics/PhysicsBox.cpp"),
            Object(Matching, "Game/Physics/PhysicsCapsule.cpp"),
            Object(NonMatching, "Game/Physics/PhysicsCharacter.cpp"),
            Object(Matching, "Game/Physics/PhysicsCharacterBase.cpp"),
            Object(Matching, "Game/Physics/PhysicsCharacterBaseData.cpp"),
            Object(Matching, "Game/Physics/PhysicsColumn.cpp"),
            Object(Matching, "Game/Physics/PhysicsCompositeObject.cpp", mw_version="GC/3.0a3", extra_cflags=["-ipa file"]),
            Object(Matching, "Game/Physics/PhysicsCylinder.cpp"),
            Object(Matching, "Game/Physics/PhysicsFinitePlane.cpp"),
            Object(NonMatching, "Game/Physics/PhysicsNet.cpp"),
            Object(Matching, "Game/Physics/PhysicsObject.cpp", extra_cflags=["-opt nolifetimes"]),
            Object(Matching, "Game/Physics/PhysicsPlane.cpp"),
            Object(Matching, "Game/Physics/PhysicsRoundedCorner.cpp"),
            Object(Matching, "Game/Physics/PhysicsSphere.cpp"),
            Object(Matching, "Game/Physics/PhysicsTransform.cpp"),
            Object(Matching, "Game/Physics/PhysicsWorld.cpp"),
        ],
    },
    {
        "lib": "Open Dynamics Engine (ODE)",
        "mw_version": config.linker_version,
        "cflags": cflags_ode,
        "progress_category": "game",
        "objects": [
            Object(Matching, "ode/collision_kernel.cpp"),
            Object(Matching, "ode/collision_space.cpp"),
            Object(NonMatching, "ode/collision_std.cpp"),
            Object(Matching, "ode/collision_transform.cpp"),
            Object(Matching, "ode/collision_util.cpp"),
            Object(NonMatching, "ode/dCylinder.cpp"),
            Object(Matching, "ode/ext/dColumn.cpp"),
            Object(Matching, "ode/ext/dFinitePlane.cpp"),
            Object(Matching, "ode/ext/dRoundedCorner.cpp"),
            Object(Matching, "ode/error.cpp"),
            Object(Matching, "ode/joint.cpp"),
            Object(Matching, "ode/matrix.cpp"),
            Object(Matching, "ode/memory.cpp", extra_cflags=["-inline deferred"]),
            Object(Matching, "ode/mass.cpp", extra_cflags=["-inline deferred"]),
            Object(Matching, "ode/obstack.cpp", extra_cflags=["-inline deferred"]),
            Object(Matching, "ode/odemath.cpp"),
            Object(Matching, "ode/ode.cpp"),
            Object(NonMatching, "ode/quickstep.cpp"),
            Object(Matching, "ode/rotation.cpp", extra_cflags=["-inline deferred"]),
            Object(Matching, "ode/util.cpp"),
            Object(NonMatching, "ode/body_debug.cpp"),
            Object(Matching, "ode/NLGAdditions.cpp"),
        ],
    },
    {
        "lib": "MSL_C",
        "mw_version": config.linker_version,
        "cflags": cflags_rvl_sdk,
        "progress_category": "sdk",
        "objects": [
            Object(Matching, "MSL/math_sun.c", cflags=cflags_runtime, extra_cflags=["-Cpp_exceptions on"]),
            Object(Matching, "MSL/float.c"),
            Object(Matching, "MSL/e_acos.c"),
            Object(Matching, "MSL/e_pow.c"),
            Object(Matching, "MSL/k_cos.c"),
            Object(Matching, "MSL/k_sin.c"),
            Object(Matching, "MSL/s_atan.c"),
            Object(Matching, "MSL/s_ceil.c"),
            Object(Matching, "MSL/s_copysign.c"),
            Object(Matching, "MSL/s_cos.c"),
            Object(Matching, "MSL/s_floor.c"),
            Object(Matching, "MSL/s_frexp.c"),
            Object(Matching, "MSL/s_sin.c"),
            Object(Matching, "MSL/s_tan.c"),
            Object(Matching, "MSL/w_acos.c", extra_cflags=["-D_IEEE_LIBM"]),
            Object(Matching, "MSL/w_asin.c", extra_cflags=["-D_IEEE_LIBM"]),
            Object(Matching, "MSL/w_pow.c", extra_cflags=["-D_IEEE_LIBM"]),
            Object(Matching, "MSL/e_sqrt.c"),
            Object(Matching, "MSL/math_ppc.c"),
            Object(Matching, "MSL/w_sqrt.c", extra_cflags=["-D_IEEE_LIBM"]),
        ],
    },
    {
        "lib": "Runtime.PPCEABI.H",
        "mw_version": config.linker_version,
        "cflags": cflags_runtime,
        "progress_category": "sdk",  # str | List[str]
        "objects": [
            # GC/3.0a3, GC/3.0a5, and GC/3.0a5.2 all reproduce these units.
            Object(Matching, "Runtime/__mem.c"),
            Object(Matching, "Runtime/__va_arg.c"),
            Object(Matching, "Runtime/global_destructor_chain.c"),
            Object(Matching, "Runtime/NMWException.cpp", extra_cflags=["-Cpp_exceptions on"]),
            Object(Matching, "Runtime/ptmf.c"),
            Object(Matching, "Runtime/runtime.c"),
            Object(Matching, "Runtime/__init_cpp_exceptions.cpp"),
            Object(Matching, "Runtime/Gecko_ExceptionPPC.cpp"),
            Object(Matching, "Runtime/GCN_mem_alloc.c"),
        ],
    },
    {
        "lib": "RVL_SDK",
        "mw_version": config.linker_version,
        "cflags": cflags_rvl_sdk,
        "progress_category": "sdk",
        "objects": [
            Object(Matching, "RVL_SDK/arc/arc.c"),
            Object(Matching, "RVL_SDK/ai/ai.c"),
            Object(Matching, "RVL_SDK/ax/AX.c"),
            Object(Matching, "RVL_SDK/ax/AXAlloc.c"),
            Object(Matching, "RVL_SDK/ax/AXAux.c"),
            Object(Matching, "RVL_SDK/ax/AXCL.c"),
            Object(Matching, "RVL_SDK/ax/AXOut.c"),
            Object(Matching, "RVL_SDK/ax/AXSPB.c"),
            Object(Matching, "RVL_SDK/ax/AXVPB.c"),
            Object(Matching, "RVL_SDK/ax/AXProf.c"),
            Object(Matching, "RVL_SDK/ax/AXComp.c"),
            Object(Matching, "RVL_SDK/ax/DSPCode.c"),
            Object(Matching, "RVL_SDK/dsp/dsp.c"),
            Object(Matching, "RVL_SDK/dsp/dsp_debug.c"),
            Object(Matching, "RVL_SDK/dsp/dsp_task.c"),
            Object(Matching, "RVL_SDK/dvd/dvdfs.c"),
            Object(Matching, "RVL_SDK/dvd/dvd.c"),
            Object(NonMatching, "RVL_SDK/dvd/dvd_broadway.c"),
            Object(Matching, "RVL_SDK/dvd/dvdqueue.c"),
            Object(Matching, "RVL_SDK/dvd/dvderror.c"),
            Object(Matching, "RVL_SDK/dvd/dvdidutils.c"),
            Object(Matching, "RVL_SDK/dvd/dvdFatal.c"),
            Object(Matching, "RVL_SDK/vf/pf_path.c", mw_version="GC/3.0a5.2"),
            Object(Matching, "RVL_SDK/vf/pf_driver.c"),
            Object(Matching, "RVL_SDK/vf/pdm_bpb.c"),
            Object(Matching, "RVL_SDK/vf/pdm_disk.c"),
            Object(Matching, "RVL_SDK/vf/pdm_partition.c"),
            Object(Matching, "RVL_SDK/vf/pdm_dskmng.c"),
            Object(Matching, "RVL_SDK/vf/pf_cache.c"),
            Object(Matching, "RVL_SDK/vf/d_vf.c"),
            Object(Matching, "RVL_SDK/vf/d_common.c"),
            Object(Matching, "RVL_SDK/vf/d_vf_sys.c"),
            Object(Matching, "RVL_SDK/vf/d_hash.c"),
            Object(Matching, "RVL_SDK/vf/d_time.c"),
            Object(Matching, "RVL_SDK/vf/pf_api_util.c"),
            Object(Matching, "RVL_SDK/vf/pf_fatfs.c"),
            Object(Matching, "RVL_SDK/vf/sd_drv.c"),
            Object(Matching, "RVL_SDK/vf/pf_clib.c"),
            Object(Matching, "RVL_SDK/vf/pf_code.c"),
            Object(Matching, "RVL_SDK/vf/pf_service.c"),
            Object(Matching, "RVL_SDK/vf/pf_str.c"),
            Object(Matching, "RVL_SDK/vf/pf_w_clib.c"),
            Object(Matching, "RVL_SDK/vf/pdm_mbr.c"),
            Object(Matching, "RVL_SDK/vf/pf_dir.c"),
            Object(Matching, "RVL_SDK/vf/pf_errnum.c"),
            Object(Matching, "RVL_SDK/vf/pf_fclose.c"),
            Object(Matching, "RVL_SDK/vf/pf_finfo.c"),
            Object(Matching, "RVL_SDK/vf/pf_fread.c"),
            Object(Matching, "RVL_SDK/vf/pf_fseek.c"),
            Object(Matching, "RVL_SDK/vf/pf_fwrite.c"),
            Object(Matching, "RVL_SDK/vf/pf_fopen.c"),
            Object(Matching, "RVL_SDK/vf/pf_init_prfile2.c"),
            Object(Matching, "RVL_SDK/vf/pf_cluster.c"),
            Object(Matching, "RVL_SDK/vf/pf_entry.c"),
            Object(Matching, "RVL_SDK/vf/pf_entry_iterator.c"),
            Object(Matching, "RVL_SDK/vf/pf_file.c"),
            Object(Matching, "RVL_SDK/vf/pf_volume.c"),
            Object(Matching, "RVL_SDK/vf/pf_fat.c"),
            Object(Matching, "RVL_SDK/vf/pf_fat12.c"),
            Object(Matching, "RVL_SDK/vf/pf_fat16.c"),
            Object(Matching, "RVL_SDK/vf/pf_fat32.c"),
            Object(Matching, "RVL_SDK/vf/pf_sector.c"),
            Object(Matching, "RVL_SDK/vf/pf_cp932.c"),
            Object(Matching, "RVL_SDK/vf/pf_filelock.c"),
            Object(Matching, "RVL_SDK/vf/pf_system.c"),
            Object(Matching, "RVL_SDK/fs/fs.c"),
            Object(Matching, "RVL_SDK/gx/GXAttr.c"),
            Object(Matching, "RVL_SDK/gx/GXGeometry.c"),
            Object(Matching, "RVL_SDK/gx/GXLight.c"),
            Object(Matching, "RVL_SDK/gx/GXBump.c"),
            Object(Matching, "RVL_SDK/gx/GXDisplayList.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXReverbHi.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXReverbHiDpl2.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXReverbHiExp.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXReverbHiExpDpl2.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXDelay.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXHooks.c"),
            Object(Matching, "RVL_SDK/nand/nand.c"),
            Object(Matching, "RVL_SDK/nand/NANDOpenClose.c"),
            Object(Matching, "RVL_SDK/nand/NANDCore.c"),
            Object(Matching, "RVL_SDK/nand/NANDCheck.c"),
            Object(Matching, "RVL_SDK/nwc24/NWC24Download.c"),
            Object(Matching, "RVL_SDK/nwc24/NWC24Config.c"),
            Object(Matching, "RVL_SDK/nwc24/NWC24FriendList.c"),
            Object(Matching, "RVL_SDK/nwc24/NWC24SecretFList.c"),
            Object(Matching, "RVL_SDK/nwc24/NWC24Manage.c"),
            Object(Matching, "RVL_SDK/nwc24/NWC24System.c"),
            Object(Matching, "RVL_SDK/nwc24/NWC24Schedule.c"),
            Object(Matching, "RVL_SDK/nwc24/NWC24MBoxCtrl.c"),
            Object(Matching, "RVL_SDK/nwc24/NWC24Mime.c"),
            Object(Matching, "RVL_SDK/nwc24/NWC24Time.c"),
            Object(NonMatching, "RVL_SDK/ncd/ncdsystem.c"),
            Object(Matching, "RVL_SDK/nwc24/NWC24StdApi.c"),
            Object(Matching, "RVL_SDK/nwc24/NWC24FileApi.c"),
            Object(Matching, "RVL_SDK/nwc24/NWC24Ipc.c"),
            Object(Matching, "RVL_SDK/thp/THPDec.c"),
            Object(Matching, "RVL_SDK/thp/THPAudio.c"),
            Object(Matching, "RVL_SDK/base/PPCArch.c"),
            Object(Matching, "RVL_SDK/db/db.c"),
            Object(Matching, "RVL_SDK/euart/euart.c"),
            Object(Matching, "RVL_SDK/exi/EXIBios.c", extra_cflags=["-schedule off"]),
            Object(Matching, "RVL_SDK/exi/EXIUart.c"),
            Object(Matching, "RVL_SDK/exi/EXICommon.c"),
            Object(Matching, "RVL_SDK/ipc/ipcMain.c"),
            Object(NonMatching, "RVL_SDK/ipc/ipcclt.c"),
            Object(Matching, "RVL_SDK/ipc/memory.c"),
            Object(Matching, "RVL_SDK/ipc/ipcProfile.c"),
            Object(NonMatching, "RVL_SDK/dwc/dwc_error.c"),
            Object(NonMatching, "RVL_SDK/kpad/KPAD.c"),
            Object(Matching, "RVL_SDK/mem/mem_heapCommon.c"),
            Object(Matching, "RVL_SDK/mem/mem_expHeap.c"),
            Object(Matching, "RVL_SDK/mem/mem_frameHeap.c"),
            Object(Matching, "RVL_SDK/mem/mem_unitHeap.c"),
            Object(Matching, "RVL_SDK/mem/mem_allocator.c"),
            Object(Matching, "RVL_SDK/mem/mem_list.c"),
            Object(Matching, "RVL_SDK/mtx/mtx.c"),
            Object(Matching, "RVL_SDK/mtx/mtxvec.c"),
            Object(Matching, "RVL_SDK/mtx/mtx44.c"),
            Object(Matching, "RVL_SDK/mtx/vec.c"),
            Object(Matching, "RVL_SDK/mtx/quat.c"),
            Object(Matching, "RVL_SDK/os/OS.c"),
            Object(Matching, "RVL_SDK/os/OSAlarm.c"),
            Object(Matching, "RVL_SDK/os/OSAlloc.c"),
            Object(Matching, "RVL_SDK/os/OSArena.c"),
            Object(Matching, "RVL_SDK/os/OSAudioSystem.c"),
            Object(Matching, "RVL_SDK/os/OSCache.c"),
            Object(Matching, "RVL_SDK/os/OSContext.c"),
            Object(Matching, "RVL_SDK/os/OSError.c"),
            Object(Matching, "RVL_SDK/os/OSFatal.c"),
            Object(Matching, "RVL_SDK/os/OSFont.c"),
            Object(Matching, "RVL_SDK/os/OSInterrupt.c"),
            Object(Matching, "RVL_SDK/os/OSLink.c"),
            Object(Matching, "RVL_SDK/os/OSMessage.c"),
            Object(Matching, "RVL_SDK/os/OSMemory.c"),
            Object(Matching, "RVL_SDK/os/OSMutex.c"),
            Object(Matching, "RVL_SDK/os/OSReboot.c"),
            Object(Matching, "RVL_SDK/os/OSReset.c"),
            Object(Matching, "RVL_SDK/os/OSRtc.c"),
            Object(Matching, "RVL_SDK/os/OSSync.c"),
            Object(Matching, "RVL_SDK/os/OSThread.c"),
            Object(Matching, "RVL_SDK/os/OSTime.c"),
            Object(Matching, "RVL_SDK/os/OSUtf.c"),
            Object(Matching, "RVL_SDK/os/OSIpc.c"),
            Object(Matching, "RVL_SDK/os/__start.c"),
            Object(Matching, "RVL_SDK/os/__ppc_eabi_init.c"),
            Object(Matching, "RVL_SDK/os/OSNet.c"),
            Object(Matching, "RVL_SDK/os/OSStateTM.c"),
            Object(Matching, "RVL_SDK/os/OSPlayRecord.c"),
            Object(Matching, "RVL_SDK/os/OSStateFlags.c"),
            Object(Matching, "RVL_SDK/os/OSNandbootInfo.c"),
            Object(NonMatching, "RVL_SDK/vi/vi.c"),
            Object(Matching, "RVL_SDK/vi/vi3in1.c"),
            Object(NonMatching, "RVL_SDK/wpad/WPAD.c"),
            Object(Matching, "RVL_SDK/so/soCommon.c", mw_version="GC/3.0a5.2"),
            Object(Matching, "RVL_SDK/so/soBasic.c"),
            Object(Matching, "RVL_SDK/si/SIBios.c"),
            Object(Matching, "RVL_SDK/si/SISamplingRate.c"),
            Object(Matching, "RVL_SDK/usb/usb.c"),
            Object(Matching, "RVL_SDK/sc/scsystem.c"),
            Object(Matching, "RVL_SDK/sc/scapi.c"),
            Object(Matching, "RVL_SDK/sc/scapi_prdinfo.c"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_Lfo.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_resourceAccessor.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_drawInfo.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_bounding.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_picture.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_animation.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_common.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_window.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_TagProcessorBase.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_ResFontBase.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_Util.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundHandle.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_MmlSeqTrackAllocator.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundArchiveLoader.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_WsdTrack.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/snd/snd_WaveSound.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_BasicSound.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_Bank.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_WaveFile.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_StrmSound.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_TaskManager.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundHeap.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/snd/snd_MmlSeqTrack.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_Channel.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_StrmPlayer.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_StrmSoundHandle.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_WaveSoundHandle.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/ut/ut_CharWriter.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_group.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_FileStream.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_IOStream.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_LinkList.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_list.cpp", cflags=cflags_rvl_hbm),
        ],
    },
]


# Optional callback to adjust link order. This can be used to add, remove, or reorder objects.
# This is called once per module, with the module ID and the current link order.
#
# For example, this adds "dummy.c" to the end of the DOL link order if configured with --non-matching.
# "dummy.c" *must* be configured as a Matching (or Equivalent) object in order to be linked.
def link_order_callback(module_id: int, objects: List[str]) -> List[str]:
    # Don't modify the link order for matching builds
    if not config.non_matching:
        return objects
    if module_id == 0:  # DOL
        return objects + ["dummy.c"]
    return objects


# Uncomment to enable the link order callback.
# config.link_order_callback = link_order_callback


# Optional extra categories for progress tracking
# Adjust as desired for your project
config.progress_categories = [
    ProgressCategory("game", "Game Code"),
    ProgressCategory("sdk", "SDK Code"),
]
config.progress_each_module = args.verbose
# Optional extra arguments to `objdiff-cli report generate`
config.progress_report_args = [
    # Marks relocations as mismatching if the target value is different
    # Default is "functionRelocDiffs=none", which is most lenient
    # "--config functionRelocDiffs=data_value",
]

if args.mode == "configure":
    # Write build.ninja and objdiff.json
    generate_build(config)
elif args.mode == "progress":
    # Print progress information
    calculate_progress(config)
else:
    sys.exit("Unknown mode: " + args.mode)
