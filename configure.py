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

# Use for any additional files that should cause a re-configure when modified
config.reconfig_deps = []

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
cflags_game_common = [
    *cflags_base,
    "-use_lmw_stmw on",
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
    "-i libs/Runtime/include",
    "-i libs/MSL_C/include",
    "-use_lmw_stmw on",
    "-str reuse,pool,readonly",
    "-gccinc",
    "-common off",
    "-inline auto",
]

# Revolution SDK library flags.
cflags_rvl_sdk = [
    *cflags_base,
    "-i libs/RVL_SDK/include",
    "-i libs/MSL_C/include",
    "-inline auto",
    "-ipa file",
    "-fp_contract off",
]

# Open Dynamics Engine flags. R4QE01 uses the single-precision, assertions-off
# configuration retained from the GameCube predecessor. obstack.cpp excludes
# GC/2.7; all tested GC/3.0 revisions through 3.0a5.2 emit the same code.
cflags_ode = [
    *cflags_base,
    "-i src/ode",
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
            Object(
                Matching,
                "Game/AI/AiUtil.cpp",
                extra_cflags=["-i libs/MSL_C/include"],
            ),
            Object(Matching, "Game/AI/Fuzzy.cpp"),
            Object(
                Equivalent,
                "Game/AI/Variant.cpp",
                cflags=[
                    *cflags_game_deferred,
                    "-i libs/Runtime/include",
                    "-char signed",
                ],
            ),
            Object(Matching, "NL/nlMath.cpp"),
            Object(Matching, "NL/nlTask.cpp"),
            Object(Matching, "NL/gl/glStat.cpp"),
            Object(Matching, "NL/gl/glState.cpp"),
            Object(Matching, "NL/gl/glStruct.cpp"),
            Object(Matching, "Game/Drawable/DrawableNetMesh.cpp"),
            Object(NonMatching, "Game/Drawable/DrawableCharacter.cpp"),
            Object(Matching, "Game/Drawable/DrawableBall.cpp"),
            Object(Matching, "Game/Drawable/DrawablePowerup.cpp"),
            Object(Matching, "Game/Task/WorldUpdateTask.cpp"),
            Object(Matching, "Game/Task/EndFrameTask.cpp"),
            Object(Matching, "Game/Task/NetworkUpdateTask.cpp"),
            Object(Matching, "Game/Task/PlatPadUpdateTask.cpp"),
            Object(Matching, "Game/Task/DispatchEventsTask.cpp"),
            Object(Matching, "Game/Task/SmokeTestUpdateTask.cpp"),
            Object(Matching, "Game/Task/TextWindowTask.cpp"),
            Object(Matching, "Game/Task/TweakerTask.cpp"),
            Object(Matching, "Game/Task/ProfilerTask.cpp"),
            Object(Matching, "Game/Task/ParticleUpdateTask.cpp"),
            Object(NonMatching, "Game/FE/GameSceneManager.cpp"),
            Object(Matching, "Game/FE/BaseSceneHandler.cpp"),
            Object(Matching, "Game/Physics/PhysicsPlane.cpp"),
            Object(Matching, "Game/Physics/PhysicsRoundedCorner.cpp"),
            Object(Matching, "Game/Physics/PhysicsSphere.cpp"),
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
            Object(Matching, "ode/collision_transform.cpp"),
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
            Object(Matching, "ode/rotation.cpp", extra_cflags=["-inline deferred"]),
            Object(Matching, "ode/util.cpp"),
            Object(Matching, "ode/NLGAdditions.cpp"),
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
            Object(Matching, "RVL_SDK/ipc/ipcMain.c"),
            Object(Matching, "RVL_SDK/ipc/memory.c"),
            Object(Matching, "RVL_SDK/ipc/ipcProfile.c"),
            Object(Matching, "RVL_SDK/mem/mem_heapCommon.c"),
            Object(Matching, "RVL_SDK/mem/mem_expHeap.c"),
            Object(Matching, "RVL_SDK/mem/mem_frameHeap.c"),
            Object(Matching, "RVL_SDK/mem/mem_unitHeap.c"),
            Object(Matching, "RVL_SDK/mem/mem_allocator.c"),
            Object(Matching, "RVL_SDK/mem/mem_list.c"),
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
            Object(Matching, "RVL_SDK/os/OSStateTM.c"),
            Object(Matching, "RVL_SDK/os/OSPlayRecord.c"),
            Object(Matching, "RVL_SDK/os/OSStateFlags.c"),
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
