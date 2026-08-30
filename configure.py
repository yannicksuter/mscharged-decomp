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
config.objdiff_tag = "v3.7.3"
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

cflags_zlib = [
    *cflags_game,
    "-i src/zlib",
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

# MetroTRK uses its own debugger-runtime optimization model.
cflags_metrotrk = [
    *(flag for flag in cflags_base if flag != "-enc SJIS"),
    "-str reuse,readonly",
    "-use_lmw_stmw on",
    "-inline on,deferred",
    "-func_align 4",
    "-sdata 0",
    "-sdata2 0",
]

cflags_metrotrk_deferred_auto = [
    *(flag for flag in cflags_metrotrk if not flag.startswith("-inline ")),
    "-inline deferred,auto",
]

# DWC consumes the vendored GameSpy interfaces below the RVL SDK source root.
cflags_rvl_dwc = [
    *cflags_rvl_sdk,
    "-i src/RVL_SDK",
    "-i src/RVL_SDK/gamespy/common",
    "-i src/RVL_SDK/gamespy/gstats",
]

# Home Button Menu library flags. R4QE01 links the HBM build dated
# Dec  7 2006, whose nw4hbm assertions are compiled in: every retained
# ut/lyt/snd routine still calls nw4hbm::db::Panic with its source file name
# and an explicit line number.
cflags_rvl_hbm = [
    *cflags_rvl_sdk,
    "-DHBM_ASSERT",
]

# WPAD's retail object contains no floating-point instructions and copies
# WPADCommand structures through GPR pairs, which is the -fp off block-move
# form; the library runs in interrupt context where FPRs are not saved.
cflags_rvl_wpad = [
    *(flag for flag in cflags_rvl_sdk if flag != "-fp hardware"),
    "-fp off",
]

# BTE is vendored Broadcom middleware and includes its own private headers by
# bare name, the way its own build does.
# WUD reaches BTE's private headers and its own, the way its own build does.
cflags_rvl_wud = [
    *cflags_rvl_sdk,
    "-i libs/RVL_SDK/include/private",
    "-i libs/RVL_SDK/include/private/bte",
]

cflags_rvl_bte = [
    *cflags_rvl_sdk,
    "-i libs/RVL_SDK/include/private/bte",
]

# GameSpy is vendored middleware whose headers sit beside its sources and are
# included by bare name, so every subsystem directory is on the include path
# the way its own build has them.
cflags_rvl_spy = [
    *cflags_rvl_sdk,
    "-w nounusedexpr",
    "-w nounusedarg",
    "-i src/RVL_SDK/gamespy",
    "-i src/RVL_SDK/gamespy/common",
    "-i src/RVL_SDK/gamespy/common/revolution",
    "-i src/RVL_SDK/gamespy/GP",
    "-i src/RVL_SDK/gamespy/ghttp",
    "-i src/RVL_SDK/gamespy/gstats",
    "-i src/RVL_SDK/gamespy/gt2",
    "-i src/RVL_SDK/gamespy/natneg",
    "-i src/RVL_SDK/gamespy/qr2",
    "-i src/RVL_SDK/gamespy/sake",
    "-i src/RVL_SDK/gamespy/serverbrowsing",
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
            Object(NonMatching, "unclassified/tu_80007400.cpp"),
            Object(Matching, "Game/AI/AiUtil.cpp"),
            Object(Matching, "Game/AnimInventory.cpp", extra_cflags=["-ipa file"]),
            Object(NonMatching, "Game/Ball.cpp"),
            Object(NonMatching, "Game/Character.cpp", extra_cflags=["-inline deferred"]),
            Object(NonMatching, "Game/Field.cpp"),
            Object(NonMatching, "Game/AI/Fielder.cpp"),
            Object(NonMatching, "Game/AI/FielderActions.cpp", extra_cflags=["-ipa file"]),
            Object(NonMatching, "Game/AI/FielderAbility.cpp"),
            Object(Matching, "Game/AI/FilteredRandom.cpp", cflags=cflags_game_deferred),
            Object(NonMatching, "Game/Formation.cpp", cflags=cflags_game),
            Object(NonMatching, "Game/FormationDefines.cpp", cflags=cflags_game),
            Object(NonMatching, "Game/Game.cpp"),
            Object(NonMatching, "unclassified/tu_80073898.cpp"),
            Object(NonMatching, "Game/GameTweaks.cpp"),
            Object(NonMatching, "Game/Goalie.cpp", cflags=cflags_game_deferred),
            Object(NonMatching, "Game/AI/GoalieActions.cpp", cflags=cflags_game_deferred),
            Object(Matching, "Game/GoalieFatigue.cpp"),
            Object(NonMatching, "Game/AI/GoalieLooseBall.cpp", cflags=cflags_game_deferred),
            Object(NonMatching, "Game/AI/GoalieSave.cpp", cflags=cflags_game_deferred),
            Object(NonMatching, "Game/AI/HeadTrack.cpp"),
            Object(NonMatching, "Game/Net.cpp"),
            Object(NonMatching, "Game/AI/Powerups.cpp"),
            Object(NonMatching, "Game/AI/ShotMeter.cpp", cflags=cflags_game_deferred),
            Object(NonMatching, "unclassified/tu_800A9140.cpp"),
            Object(NonMatching, "unclassified/tu_800A9588.cpp"),
            Object(NonMatching, "Game/AI/Desire.cpp"),
            Object(Matching, "Game/TweaksBase.cpp"),
            Object(Matching, "Game/AI/Scripts/ScriptDefines.cpp", cflags=cflags_game_deferred),
            Object(Matching, "Game/ScriptTuning.cpp"),
            Object(Matching, "Game/AI/Fuzzy.cpp"),
            Object(NonMatching, "Game/AI/Scripts/FuzzyAIRuntime.cpp", cflags=[*cflags_game_deferred, "-char signed"]),
            Object(NonMatching, "Game/AI/FuzzyVariant.cpp", mw_version="GC/3.0a3", cflags=[*cflags_game_deferred, "-char signed"]),
            Object(Matching, "Game/AI/TeamDesire.cpp"),
            Object(Matching, "Game/AI/TeamPlayMachine.cpp"),
            Object(Matching, "Game/AI/TutorialMegastrikeDesire.cpp"),
            Object(NonMatching, "unclassified/tu_80139B18.cpp"),
            Object(Equivalent, "Game/AI/Variant.cpp", cflags=[*cflags_game_deferred, "-char signed"]),
            Object(Matching, "Game/Camera/TopDownCamera.cpp", cflags=cflags_game_deferred),
            Object(Matching, "Game/Camera/kickoffcam.cpp", cflags=cflags_game_deferred),
            Object(NonMatching, "Game/Camera/animcam.cpp", cflags=cflags_game),
            Object(NonMatching, "Game/Camera/CameraMan.cpp", cflags=cflags_game_deferred),
            Object(NonMatching, "Game/Camera/FaceCam.cpp", cflags=cflags_game_deferred),
            Object(Matching, "Game/Camera/GoalCam.cpp", cflags=cflags_game_deferred),
            Object(NonMatching, "Game/Camera/MatrixEffectCam.cpp", cflags=cflags_game_deferred),
            Object(Matching, "Game/Camera/AnimViewerCam.cpp", cflags=cflags_game_deferred),
            Object(Matching, "Game/Camera/ShootToScoreCam.cpp", cflags=cflags_game_deferred),
            Object(Matching, "Game/DB/BasicGameInfo.cpp", extra_cflags=["-ipa file"]),
            Object(NonMatching, "Game/GameInfo.cpp", extra_cflags=["-ipa file"]),
            Object(Matching, "Game/DB/UserOptions.cpp"),
            Object(NonMatching, "Game/InterpreterCore.cpp", cflags=cflags_game),
            Object(NonMatching, "Game/NisPlayer.cpp", extra_cflags=["-inline deferred"]),
            Object(Matching, "Game/Render/RLView.cpp"),
            Object(NonMatching, "Game/Render/RLViewLayers.cpp"),
            Object(NonMatching, "Game/Render/RLViewData.cpp"),
            Object(NonMatching, "Game/Render/Nis.cpp", extra_cflags=["-inline deferred"]),
            Object(Matching, "Game/AnimProps/globalanimproperties.cpp"),
            Object(Matching, "Game/AnimProps/goalieanimproperties.cpp"),
            Object(Matching, "NL/nlAVLTree.cpp"),
            Object(Matching, "NL/nlDebug.cpp"),
            Object(NonMatching, "unclassified/tu_802A7C90.cpp"),
            Object(NonMatching, "unclassified/tu_802A7F80.cpp"),
            Object(NonMatching, "unclassified/tu_802A8508.cpp"),
            Object(NonMatching, "unclassified/tu_802A87F8.cpp"),
            Object(NonMatching, "unclassified/tu_802A8A90.cpp"),
            Object(Matching, "NL/nlMath.cpp"),
            Object(Matching, "NL/nlMemory.cpp"),
            Object(Matching, "NL/nlPrint.cpp"),
            Object(Matching, "NL/nlTicker.cpp"),
            Object(Matching, "NL/nlString.cpp"),
            Object(Matching, "NL/nlMain.cpp", cflags=cflags_game_deferred),
            Object(Matching, "Game/Sys/simpleparser.cpp"),
            Object(Matching, "zlib/adler32.c", cflags=cflags_zlib, progress_category="sdk"),
            Object(Matching, "zlib/crc32.c", cflags=cflags_zlib, progress_category="sdk"),
            Object(Matching, "zlib/inffast.c", cflags=cflags_zlib, progress_category="sdk"),
            Object(Matching, "zlib/inflate.c", cflags=cflags_zlib, progress_category="sdk"),
            Object(Matching, "zlib/inftrees.c", cflags=cflags_zlib, progress_category="sdk"),
            Object(Matching, "zlib/zutil.c", cflags=cflags_zlib, progress_category="sdk"),
            Object(NonMatching, "NL/MemAlloc.cpp"),
            Object(NonMatching, "NL/nlConfig.cpp"),
            Object(Matching, "NL/nlDebugFile.cpp"),
            Object(Matching, "NL/nlFile.cpp"),
            Object(NonMatching, "NL/nlFileGC.cpp"),
            Object(Matching, "NL/nlSlotPool.cpp"),
            Object(Matching, "NL/nlStringSupport.cpp", extra_cflags=["-ipa file"]),
            Object(Matching, "NL/nlTask.cpp"),
            Object(NonMatching, "NL/math.cpp", cflags=cflags_game_deferred),
            Object(NonMatching, "NL/utility.cpp", cflags=cflags_game_deferred),
            Object(Matching, "Game/Core/mtRandom.cpp"),
            Object(Matching, "NL/nlLocalization.cpp"),
            Object(Matching, "NL/nlBundleFile.cpp"),
            Object(NonMatching, "NL/nlFont.cpp", cflags=cflags_game_deferred),
            Object(NonMatching, "NL/nlTextBox.cpp"),
            Object(Matching, "NL/nlTextEscape.cpp", cflags=[*cflags_game_deferred, "-sym on"]),
            Object(Matching, "Game/Sys/clock.cpp"),
            Object(Matching, "NL/nlTimer.cpp"),
            Object(NonMatching, "Game/Debug/FrameCounter.cpp"),
            Object(Matching, "NL/nlEndian.cpp"),
            Object(NonMatching, "NL/gl/glDraw2.cpp"),
            Object(Matching, "NL/gl/glDraw3.cpp"),
            Object(Matching, "NL/gl/glFont.cpp"),
            Object(Matching, "NL/gl/glMatrix.cpp"),
            Object(Matching, "NL/gl/glStat.cpp"),
            Object(Matching, "NL/gl/glState.cpp"),
            Object(Matching, "NL/gl/glStruct.cpp"),
            Object(NonMatching, "NL/gl/glTarget.cpp"),
            Object(Matching, "NL/gl/glTexture.cpp"),
            Object(NonMatching, "NL/gl/glView.cpp"),
            Object(Matching, "NL/gl/gl.cpp"),
            Object(Matching, "NL/gl/glModel.cpp"),
            Object(NonMatching, "NL/plat/platqmath.cpp"),
            Object(Matching, "NL/plat/platvmath.cpp"),
            Object(Matching, "NL/gc/gcSwizzler.cpp"),
            Object(NonMatching, "NL/gl/glPlat.cpp"),
            Object(NonMatching, "Game/GL/GLInventory.cpp", extra_cflags=["-inline depth=3"]),
            Object(NonMatching, "NL/glx/glxLoadModel.cpp"),
            Object(NonMatching, "NL/glx/glxTexture.cpp", extra_cflags=["-inline deferred"]),
            Object(Matching, "NL/glx/glxGX.cpp"),
            Object(Matching, "NL/glx/glxSwap.cpp", extra_cflags=["-inline noauto"]),
            Object(Matching, "unclassified/tu_8036D6F8.cpp"),
            Object(NonMatching, "NL/glx/glxMatrix.cpp"),
            Object(Matching, "unclassified/tu_8036E430.cpp"),
            Object(Matching, "NL/glx/glxFont.cpp"),
            Object(NonMatching, "NL/glx/glxSend.cpp"),
            Object(Matching, "NL/glx/glxDisplayList.cpp"),
            Object(NonMatching, "Game/GL/ShaderSkinMesh.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgram_802981F0.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgram_80298478.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgram_802987A0.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgram_80298B18.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgram_80298EE0.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgram_802991B8.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgram_80299490.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgram_802997B8.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgram_80299A90.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_80299CA0.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_8029A4A0.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_8029AB0C.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_8029AFC4.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_8029B434.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgram_8029BA04.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_8029BC9C.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_8029C2F8.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_8029C9F0.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_8029D0E8.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_8029D7E0.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_8029DE3C.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_8029E338.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_8029E8F8.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_8029EF54.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_8029F5B0.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_8029FC0C.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_802A01CC.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_802A05A4.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_802A3EF0.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_802A4360.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_802A4744.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_802A4B28.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_802A4F0C.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_802A53B4.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_802A58E8.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_802A5D58.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgram_802A61C8.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_802A63D8.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_802A6848.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_802A6B6C.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_802A6FDC.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_802A73B0.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgram_802A7820.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgramRegistry_802A0A14.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_8028A98C.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_8028B1FC.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_8028BBF0.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_8028C66C.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_8028D230.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_8028D990.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_8028E318.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_8028ED18.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgramSupport_8028F79C.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_8028F9AC.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgramSupport_8029003C.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgramSupport_80290250.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgramSupport_802904D8.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgramSupport_802906DC.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_80290914.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_802913A4.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_80291B14.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_80292440.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_80293154.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_802938B8.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_80294404.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_80294B28.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_802951A4.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_80295E00.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_802963B4.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_80296BAC.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_802977EC.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgramSupport_80298074.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgramSupport_802A1EE4.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgramSupport_802A2054.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgramSupport_802A2220.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgramSupport_802A23F4.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgramSupport_802A253C.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_802A2778.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgramSupport_802A2A94.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgramSupport_802A2CFC.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgramSupport_802A2EDC.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgramSupport_802A30E4.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgramSupport_802A3254.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgramSupport_802A33E0.cpp"),
            Object(Matching, "NL/glx/GXMaterialProgramSupport_802A362C.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_802A3738.cpp"),
            Object(NonMatching, "NL/glx/GXMaterialProgramSupport_802A3D9C.cpp"),
            Object(Matching, "Game/CharacterEffects.cpp"),
            Object(Matching, "Game/Drawable/DrawableNetMesh.cpp"),
            Object(NonMatching, "Game/Drawable/DrawableCharacter.cpp"),
            Object(Matching, "Game/Drawable/DrawableBall.cpp"),
            Object(NonMatching, "Game/Drawable/DrawableBulletBill.cpp"),
            Object(NonMatching, "Game/Drawable/DrawableBirdoEgg.cpp"),
            Object(NonMatching, "Game/Drawable/DrawableYoshiEgg.cpp"),
            Object(NonMatching, "Game/Drawable/DrawableFlyingCamera.cpp"),
            Object(NonMatching, "Game/Drawable/DrawableKoopaShell.cpp"),
            Object(Matching, "Game/Drawable/DrawablePowerup.cpp"),
            Object(NonMatching, "unclassified/tu_80199E84.cpp"),
            Object(NonMatching, "unclassified/tu_8019A710.cpp"),
            Object(NonMatching, "Game/Render/depthoffield.cpp"),
            Object(NonMatching, "unclassified/tu_801AC30C.cpp"),
            Object(NonMatching, "unclassified/tu_801B369C.cpp"),
            Object(NonMatching, "Game/Render/Presentation.cpp"),
            Object(NonMatching, "Game/Drawable/DrawableThwomp.cpp"),
            Object(NonMatching, "Game/Blinker.cpp", cflags=cflags_game_deferred),
            Object(NonMatching, "Game/Render/ChainChomp.cpp"),
            Object(NonMatching, "Game/Render/NetMesh.cpp"),
            Object(Matching, "Game/NetMeshEdge.cpp"),
            Object(NonMatching, "Game/NetMeshModelLoader.cpp"),
            Object(NonMatching, "Game/Physics.cpp"),
            Object(Equivalent, "Game/Physics/PhysicsBanana.cpp"),
            Object(NonMatching, "unclassified/tu_80141AB0.cpp"),
            Object(NonMatching, "unclassified/tu_8016F34C.cpp"),
            Object(Matching, "Game/Physics/PhysicsGroundPlane.cpp"),
            Object(NonMatching, "unclassified/tu_801700D8.cpp"),
            Object(NonMatching, "unclassified/tu_801709D0.cpp"),
            Object(NonMatching, "unclassified/tu_80175F8C.cpp"),
            Object(NonMatching, "unclassified/tu_80176EF4.cpp"),
            Object(NonMatching, "unclassified/tu_80179858.cpp"),
            Object(NonMatching, "unclassified/tu_801798A8.cpp"),
            Object(NonMatching, "Game/FixedUpdateTask.cpp"),
            Object(Matching, "Game/Task/WorldUpdateTask.cpp"),
            Object(Matching, "Game/Task/EndFrameTask.cpp"),
            Object(NonMatching, "unclassified/tu_80112E0C.cpp"),
            Object(NonMatching, "Game/FrontEndTask.cpp"),
            Object(Matching, "Game/Task/NetworkUpdateTask.cpp"),
            Object(Matching, "Game/Task/PlatPadUpdateTask.cpp"),
            Object(NonMatching, "Game/Task/GameRenderTask.cpp"),
            Object(Matching, "Game/Task/DispatchEventsTask.cpp"),
            Object(NonMatching, "Game/Task/TransitionTask.cpp"),
            Object(Matching, "Game/Task/LoadingTask.cpp"),
            Object(Matching, "Game/ResetTask.cpp"),
            Object(Matching, "Game/Task/SmokeTestUpdateTask.cpp"),
            Object(Matching, "Game/Task/TextWindowTask.cpp"),
            Object(Matching, "Game/Task/TweakerTask.cpp"),
            Object(Matching, "Game/Task/ProfilerTask.cpp"),
            Object(Matching, "Game/Task/ParticleUpdateTask.cpp"),
            Object(Matching, "Game/Effects/PhotoFlashEffect.cpp"),
            Object(NonMatching, "unclassified/tu_802A8218.cpp"),
            Object(NonMatching, "unclassified/tu_802E3D4C.cpp"),
            Object(NonMatching, "Game/Effects/EmissionController.cpp"),
            Object(Equivalent, "Game/Audio/AudioResourceLoadOwner_802ED144.cpp"),
            Object(Equivalent, "Game/Audio/XSoundHandle_802ED74C.cpp"),
            Object(Matching, "Game/Audio/AudioResourceLoader_802EDA38.cpp"),
            Object(NonMatching, "Game/Audio/AudioBundleManager_802EDA7C.cpp"),
            Object(NonMatching, "Game/Audio/SoundMap.cpp"),
            Object(NonMatching, "unclassified/tu_802EE964.cpp"),
            Object(NonMatching, "unclassified/tu_802EFB70.cpp"),
            Object(NonMatching, "unclassified/tu_802F076C.cpp"),
            Object(NonMatching, "unclassified/tu_802F1758.cpp"),
            Object(NonMatching, "unclassified/tu_802F2110.cpp"),
            Object(NonMatching, "unclassified/tu_802F2C3C.cpp"),
            Object(NonMatching, "Game/Audio/AudioResourcePlatform_8035DE28.cpp"),
            Object(NonMatching, "Game/FE/feManager.cpp"),
            Object(Matching, "Game/FE/GameSceneManager.cpp"),
            Object(Matching, "Game/FE/Overlay/OverlayHandlerDemo.cpp"),
            Object(Matching, "Game/FE/BaseSceneHandler.cpp"),
            Object(NonMatching, "Game/FE/feAnimation.cpp"),
            Object(Matching, "Game/FE/feButtonComponent.cpp"),
            Object(Matching, "Game/FE/feFontResource.cpp"),
            Object(NonMatching, "Game/FE/feInput.cpp"),
            Object(Matching, "Game/FE/fePackage.cpp"),
            Object(NonMatching, "Game/FE/fePresentation.cpp", extra_cflags=["-ipa file"]),
            Object(NonMatching, "Game/FE/feRender.cpp", cflags=cflags_game_deferred),
            Object(NonMatching, "Game/FE/feResourceManager.cpp", cflags=cflags_game),
            Object(NonMatching, "Game/FE/feScene.cpp"),
            Object(NonMatching, "Game/FE/feSceneManager.cpp"),
            Object(Matching, "Game/FE/feSceneResource.cpp"),
            Object(NonMatching, "Game/FE/tlSlide.cpp"),
            Object(Matching, "Game/FE/feTextureResource.cpp"),
            Object(NonMatching, "unclassified/tu_80209584.cpp"),
            Object(Matching, "unclassified/tu_80219248.cpp"),
            Object(NonMatching, "unclassified/tu_80300104.cpp"),
            Object(NonMatching, "Game/FE/tlComponent.cpp"),
            Object(Matching, "Game/FE/feLibObject.cpp"),
            Object(Matching, "Game/FE/tlComponentInstance.cpp"),
            Object(NonMatching, "Game/FE/tlInstance.cpp"),
            Object(NonMatching, "Game/FE/tlTextInstance.cpp"),
            Object(NonMatching, "Game/FE/tlTextInstance_runtime.cpp"),
            Object(NonMatching, "Game/Font/fontmanager.cpp"),
            Object(NonMatching, "Game/SAnim.cpp", extra_cflags=["-inline deferred"]),
            Object(Matching, "Game/SAnim/pnSAnimController.cpp", extra_cflags=["-inline deferred"]),
            Object(NonMatching, "Game/SHierarchy.cpp", extra_cflags=["-inline deferred"]),
            Object(Matching, "Game/SAnim/pnBlender.cpp"),
            Object(Matching, "Game/SAnim/pnSingleAxisBlender.cpp"),
            Object(Matching, "Game/SAnim/pnFeather.cpp", extra_cflags=["-inline auto,depth=3"]),
            Object(NonMatching, "Game/SAnim/AnimRetargeter.cpp"),
            Object(Matching, "Game/Physics/CollisionSpace.cpp"),
            Object(Matching, "Game/Physics/PhysicsBox.cpp"),
            Object(Matching, "Game/Physics/PhysicsCapsule.cpp"),
            Object(NonMatching, "Game/Physics/PhysicsCharacter.cpp"),
            Object(NonMatching, "Game/Physics/PhysicsEventQueue.cpp"),
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
        "lib": "MetroTRK",
        "mw_version": "GC/2.7",
        "cflags": cflags_metrotrk,
        "progress_category": "sdk",
        "objects": [
            Object(Matching, "MetroTRK/mainloop.c", cflags=cflags_metrotrk_deferred_auto),
            Object(Matching, "MetroTRK/mutex_TRK.c"),
            Object(Matching, "MetroTRK/nubevent.c"),
            Object(Matching, "MetroTRK/nubinit.c", cflags=cflags_metrotrk_deferred_auto),
            Object(Matching, "MetroTRK/msg.c"),
            Object(Matching, "MetroTRK/msgbuf.c", cflags=cflags_metrotrk_deferred_auto),
            Object(Matching, "MetroTRK/serpoll.c", cflags=cflags_metrotrk_deferred_auto, extra_cflags=["-sdata 8"]),
            Object(Matching, "MetroTRK/usr_put.c"),
            Object(Matching, "MetroTRK/dispatch.c"),
            Object(Matching, "MetroTRK/msghndlr.c", cflags=cflags_metrotrk_deferred_auto),
            Object(Matching, "MetroTRK/support.c", cflags=cflags_metrotrk_deferred_auto),
            Object(Matching, "MetroTRK/notify.c", cflags=cflags_metrotrk_deferred_auto),
            Object(Matching, "MetroTRK/flush_cache.c"),
            Object(Matching, "MetroTRK/mem_TRK.c"),
            Object(Matching, "MetroTRK/string_TRK.c"),
            Object(Matching, "MetroTRK/targimpl.c", cflags=cflags_metrotrk_deferred_auto),
            Object(Matching, "MetroTRK/targsupp.c"),
            Object(Matching, "MetroTRK/mpc_7xx_603e.c"),
            Object(Matching, "MetroTRK/mslsupp.c"),
            Object(Matching, "MetroTRK/__exception.s"),
            Object(Matching, "MetroTRK/dolphin_trk.c", cflags=cflags_metrotrk_deferred_auto),
            Object(Matching, "MetroTRK/main_TRK.c"),
            Object(Matching, "MetroTRK/dolphin_trk_glue.c"),
            Object(Matching, "MetroTRK/targcont.c"),
            Object(Matching, "MetroTRK/target_options.c"),
            Object(Matching, "MetroTRK/UDP_Stubs.c"),
            Object(Matching, "MetroTRK/cc_gdev.c", extra_cflags=["-sdata 8"]),
            Object(Matching, "MetroTRK/CircleBuffer.c"),
            Object(Matching, "MetroTRK/MWCriticalSection_gc.c"),
        ],
    },
    {
        "lib": "MSL_C",
        "mw_version": config.linker_version,
        "cflags": cflags_rvl_sdk,
        "progress_category": "sdk",
        "objects": [
            Object(Matching, "MSL/qsort.c", extra_cflags=["-Cpp_exceptions on", "-use_lmw_stmw on"]),
            Object(Matching, "MSL/rand.c"),
            Object(Matching, "MSL/string.c", extra_cflags=["-Cpp_exceptions on", "-str reuse,pool,readonly"]),
            Object(Matching, "MSL/strtold.c", extra_cflags=["-Cpp_exceptions on", "-str reuse,pool,readonly", "-use_lmw_stmw on"]),
            Object(Matching, "MSL/strtoul.c", extra_cflags=["-Cpp_exceptions on", "-use_lmw_stmw on"]),
            Object(Matching, "MSL/wmem.c"),
            Object(Matching, "MSL/wprintf.c", extra_cflags=["-Cpp_exceptions on", "-str reuse,pool,readonly", "-use_lmw_stmw on"], mw_version="GC/3.0a5.2"),
            Object(Matching, "MSL/math_sun.c", cflags=cflags_runtime, extra_cflags=["-Cpp_exceptions on"]),
            Object(Matching, "MSL/float.c"),
            Object(Matching, "MSL/e_acos.c"),
            Object(Matching, "MSL/e_pow.c"),
            Object(Matching, "MSL/k_cos.c"),
            Object(Matching, "MSL/k_sin.c"),
            Object(Matching, "MSL/k_tan.c"),
            Object(Matching, "MSL/s_atan.c"),
            Object(Matching, "MSL/s_ceil.c"),
            Object(Matching, "MSL/s_copysign.c"),
            Object(Matching, "MSL/s_cos.c"),
            Object(Matching, "MSL/s_floor.c"),
            Object(Matching, "MSL/s_frexp.c"),
            Object(Matching, "MSL/s_sin.c"),
            Object(Matching, "MSL/s_tan.c"),
            Object(Matching, "MSL/w_acos.c", extra_cflags=["-D_IEEE_LIBM"]),
            Object(Matching, "MSL/w_atan2.c", extra_cflags=["-D_IEEE_LIBM"]),
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
            Object(NonMatching, "RVL_SDK/vf/nand_drv.c"),
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
            Object(NonMatching, "RVL_SDK/net/md5.c"),
            Object(NonMatching, "RVL_SDK/net/hmac.c"),
            Object(NonMatching, "RVL_SDK/net/neterrorcode.c"),
            Object(NonMatching, "RVL_SDK/net/NETVersion.c"),
            Object(Matching, "RVL_SDK/net/wireless_macaddr.c"),
            Object(Matching, "RVL_SDK/fs/fs.c"),
            Object(Matching, "RVL_SDK/gx/GXInit.c"),
            Object(Matching, "RVL_SDK/gx/GXFifo.c"),
            Object(Matching, "RVL_SDK/gx/GXAttr.c"),
            Object(Matching, "RVL_SDK/gx/GXGeometry.c"),
            Object(Matching, "RVL_SDK/gx/GXFrameBuf.c"),
            Object(Matching, "RVL_SDK/gx/GXLight.c"),
            Object(Matching, "RVL_SDK/gx/GXTexture.c"),
            Object(Matching, "RVL_SDK/gx/GXBump.c"),
            Object(Matching, "RVL_SDK/gx/GXTev.c"),
            Object(NonMatching, "RVL_SDK/gx/GXPixel.c"),
            Object(Matching, "RVL_SDK/gx/GXTransform.c"),
            Object(Matching, "RVL_SDK/gx/GXDisplayList.c"),
            Object(Matching, "RVL_SDK/gx/GXPerf.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXReverbHi.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXReverbHiDpl2.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXReverbHiExp.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXReverbHiExpDpl2.c"),
            Object(Matching, "RVL_SDK/axfx/AXFXDelay.c"),
            Object(NonMatching, "RVL_SDK/axfx/AXFXDelayExpDpl2.c"),
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
            Object(Matching, "RVL_SDK/ncd/ncdsystem.c"),
            Object(Matching, "RVL_SDK/nwc24/NWC24StdApi.c"),
            Object(Matching, "RVL_SDK/nwc24/NWC24FileApi.c"),
            Object(Matching, "RVL_SDK/nwc24/NWC24Ipc.c"),
            Object(NonMatching, "RVL_SDK/thp/THPSimple.cpp", cflags=cflags_game),
            Object(Matching, "RVL_SDK/thp/THPDec.c"),
            Object(Matching, "RVL_SDK/thp/THPAudio.c"),
            Object(Matching, "RVL_SDK/tpl/TPL.c"),
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
            Object(Matching, "RVL_SDK/dwc/dwc_base64.c"),
            Object(Matching, "RVL_SDK/dwc/dwc_error.c"),
            Object(Matching, "RVL_SDK/dwc/dwc_init.c", cflags=cflags_rvl_dwc),
            Object(Matching, "RVL_SDK/dwc/dwc_memfunc.c"),
            Object(Matching, "RVL_SDK/dwc/dwc_report.c"),
            Object(NonMatching, "RVL_SDK/dwc/dwc_ghttp.c", cflags=cflags_rvl_dwc),
            Object(NonMatching, "RVL_SDK/dwc/dwc_common.c", cflags=cflags_rvl_dwc),
            Object(NonMatching, "RVL_SDK/dwc/dwc_friend.c", cflags=cflags_rvl_dwc),
            Object(NonMatching, "RVL_SDK/dwc/dwc_login.c", cflags=cflags_rvl_dwc),
            Object(NonMatching, "RVL_SDK/dwc/dwc_main.c", cflags=cflags_rvl_dwc),
            Object(NonMatching, "RVL_SDK/dwc/dwc_transport.c", cflags=cflags_rvl_dwc),
            Object(Matching, "RVL_SDK/dwc/dwc_nonport.c", cflags=cflags_rvl_dwc),
            Object(NonMatching, "RVL_SDK/dwc/dwci_np_math.c", cflags=cflags_rvl_dwc),
            Object(Matching, "RVL_SDK/dwc/dwc_ranking.c", cflags=cflags_rvl_dwc),
            Object(NonMatching, "RVL_SDK/dwc/dwc_account.c", cflags=cflags_rvl_dwc),
            Object(NonMatching, "RVL_SDK/dwc/dwc_auth_interface.c", cflags=cflags_rvl_dwc),
            Object(Matching, "RVL_SDK/dwc/dwc_nastime.c", cflags=cflags_rvl_dwc),
            Object(NonMatching, "RVL_SDK/dwc/dwc_ranksession.c", cflags=cflags_rvl_dwc),
            Object(NonMatching, "RVL_SDK/dwc/unknown_804A2068.c", cflags=cflags_rvl_dwc),
            Object(Matching, "RVL_SDK/gamespy/darray.c", cflags=cflags_rvl_spy),
            Object(Matching, "RVL_SDK/gamespy/hashtable.c", cflags=cflags_rvl_spy),
            Object(Matching, "RVL_SDK/gamespy/md5c.c", cflags=cflags_rvl_spy),
            Object(NonMatching, "RVL_SDK/gamespy/common/gsPlatformSocket.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(NonMatching, "RVL_SDK/gamespy/common/gsPlatformUtil.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(NonMatching, "RVL_SDK/gamespy/common/gsLargeInt.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(NonMatching, "RVL_SDK/gamespy/gstats/gstats.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(Matching, "RVL_SDK/gamespy/common/gsMemory.c", cflags=cflags_rvl_spy),
            Object(Matching, "RVL_SDK/gamespy/common/gsAvailable.c", cflags=cflags_rvl_spy),
            Object(Matching, "RVL_SDK/gamespy/common/gsSHA1.c", cflags=cflags_rvl_spy),
            Object(Matching, "RVL_SDK/gamespy/gt2/gt2Utility.c", cflags=cflags_rvl_spy),
            Object(Matching, "RVL_SDK/gamespy/gt2/gt2Main.c", cflags=cflags_rvl_spy),
            Object(NonMatching, "RVL_SDK/gamespy/qr2/qr2.c", cflags=cflags_rvl_spy),
            Object(NonMatching, "RVL_SDK/gamespy/qr2/qr2regkeys.c", cflags=cflags_rvl_spy),
            Object(Matching, "RVL_SDK/gamespy/natneg/NATify.c", cflags=cflags_rvl_spy),
            Object(Matching, "RVL_SDK/gamespy/natneg/natneg.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(Matching, "RVL_SDK/gamespy/gstats/gbucket.c", cflags=cflags_rvl_spy),
            Object(Matching, "RVL_SDK/gamespy/ghttp/ghttpMain.c", cflags=cflags_rvl_spy),
            Object(Matching, "RVL_SDK/gamespy/serverbrowsing/sb_crypt.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(Matching, "RVL_SDK/gamespy/serverbrowsing/sb_queryengine.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(Matching, "RVL_SDK/gamespy/serverbrowsing/sb_server.c", cflags=cflags_rvl_spy),
            Object(Matching, "RVL_SDK/gamespy/serverbrowsing/sb_serverbrowsing.c", cflags=cflags_rvl_spy),
            Object(NonMatching, "RVL_SDK/gamespy/serverbrowsing/sb_serverlist.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(Matching, "RVL_SDK/gamespy/gt2/gt2Auth.c", cflags=cflags_rvl_spy),
            Object(Matching, "RVL_SDK/gamespy/gt2/gt2Buffer.c", cflags=cflags_rvl_spy),
            Object(Matching, "RVL_SDK/gamespy/gt2/gt2Callback.c", cflags=cflags_rvl_spy),
            Object(Matching, "RVL_SDK/gamespy/gt2/gt2Connection.c", cflags=cflags_rvl_spy),
            Object(Matching, "RVL_SDK/gamespy/gt2/gt2Socket.c", cflags=cflags_rvl_spy),
            Object(Matching, "RVL_SDK/gamespy/ghttp/ghttpCallbacks.c", cflags=cflags_rvl_spy),
            Object(Matching, "RVL_SDK/gamespy/ghttp/ghttpBuffer.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(NonMatching, "RVL_SDK/gamespy/ghttp/ghttpCommon.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(NonMatching, "RVL_SDK/gamespy/ghttp/ghttpPost.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(NonMatching, "RVL_SDK/gamespy/ghttp/ghttpProcess.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(Matching, "RVL_SDK/gamespy/ghttp/ghttpConnection.c", cflags=cflags_rvl_spy),
            Object(Matching, "RVL_SDK/gamespy/GP/gpiUtility.c", cflags=cflags_rvl_spy),
            Object(Matching, "RVL_SDK/gamespy/GP/gp.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(Matching, "RVL_SDK/gamespy/GP/gpi.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(Matching, "RVL_SDK/gamespy/GP/gpiBuddy.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(Matching, "RVL_SDK/gamespy/GP/gpiBuffer.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(Matching, "RVL_SDK/gamespy/GP/gpiCallback.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(Matching, "RVL_SDK/gamespy/GP/gpiConnect.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(Matching, "RVL_SDK/gamespy/GP/gpiInfo.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(Matching, "RVL_SDK/gamespy/GP/gpiOperation.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(NonMatching, "RVL_SDK/gamespy/GP/gpiPeer.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(Matching, "RVL_SDK/gamespy/GP/gpiProfile.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(NonMatching, "RVL_SDK/gamespy/GP/gpiSearch.c", cflags=cflags_rvl_spy, mw_version="GC/3.0a5.2"),
            Object(Matching, "RVL_SDK/rfl/RFL_HiddenDatabase.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(Matching, "RVL_SDK/rfl/RFL_Database.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(Matching, "RVL_SDK/rfl/RFL_Controller.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(Matching, "RVL_SDK/rfl/RFL_MiddleDatabase.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(Matching, "RVL_SDK/rfl/RFL_System.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(Matching, "RVL_SDK/rfl/RFL_NANDLoader.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(Matching, "RVL_SDK/rfl/RFL_NANDAccess.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(Matching, "RVL_SDK/rfl/RFL_Model.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(Matching, "RVL_SDK/rfl/RFL_MakeTex.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(Matching, "RVL_SDK/rfl/RFL_Icon.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(Matching, "RVL_SDK/rfl/RFL_MakeRandomFace.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(Matching, "RVL_SDK/rfl/RFL_DefaultDatabase.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(Matching, "RVL_SDK/rfl/RFL_DataUtility.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(Matching, "RVL_SDK/rfl/RFL_Format.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(NonMatching, "RVL_SDK/ndev/DebuggerDriver.c"),
            Object(Matching, "RVL_SDK/ndev/exi2.c"),
            Object(NonMatching, "RVL_SDK/kpad/KPAD.c"),
            Object(Matching, "RVL_SDK/mem/mem_heapCommon.c"),
            Object(Matching, "RVL_SDK/mem/mem_expHeap.c"),
            Object(Matching, "RVL_SDK/mem/mem_frameHeap.c"),
            Object(Matching, "RVL_SDK/mem/mem_unitHeap.c"),
            Object(Matching, "RVL_SDK/mem/mem_allocator.c"),
            Object(Matching, "RVL_SDK/mem/mem_list.c"),
            Object(Matching, "RVL_SDK/mix/mix.c"),
            Object(Matching, "RVL_SDK/mix/remote.c"),
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
            Object(NonMatching, "RVL_SDK/os/OSExec.c", cflags=[flag for flag in cflags_rvl_sdk if flag != "-ipa file"]),
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
            Object(Matching, "RVL_SDK/vi/vi.c", mw_version="GC/3.0a5.2"),
            Object(Matching, "RVL_SDK/vi/i2c.c"),
            Object(Matching, "RVL_SDK/vi/vi3in1.c"),
            Object(Matching, "RVL_SDK/wenc/wenc.c", mw_version="GC/3.0a5.2"),
            Object(NonMatching, "RVL_SDK/wpad/WPAD.c", cflags=cflags_rvl_wpad),
            Object(Matching, "RVL_SDK/wpad/WPADHIDParser.c"),
            Object(Matching, "RVL_SDK/wpad/WPADEncrypt.c"),
            Object(Matching, "RVL_SDK/wpad/debug_msg.c"),
            Object(Matching, "RVL_SDK/so/soCommon.c", mw_version="GC/3.0a5.2"),
            Object(NonMatching, "RVL_SDK/so/soBasic.c"),
            Object(NonMatching, "RVL_SDK/so/SOInformation.c"),
            Object(Equivalent, "RVL_SDK/so/SOOption.c"),
            Object(Matching, "RVL_SDK/nhttp/NHTTP_bgnend.c"),
            Object(NonMatching, "RVL_SDK/nhttp/NHTTP_control.c"),
            Object(Matching, "RVL_SDK/nhttp/NHTTP_list.c"),
            Object(Matching, "RVL_SDK/nhttp/NHTTP_os_RVL.c"),
            Object(NonMatching, "RVL_SDK/nhttp/NHTTP_recvbuf.c"),
            Object(NonMatching, "RVL_SDK/nhttp/NHTTP_request.c"),
            Object(NonMatching, "RVL_SDK/nhttp/NHTTP_response.c"),
            Object(NonMatching, "RVL_SDK/nhttp/NHTTP_socket_RVL.c"),
            Object(NonMatching, "RVL_SDK/nhttp/NHTTP_stdlib_RVL.c"),
            Object(NonMatching, "RVL_SDK/nhttp/NHTTP_thread.c"),
            Object(NonMatching, "RVL_SDK/nhttp/d_nhttp_private.c"),
            Object(NonMatching, "RVL_SDK/nhttp/d_nhttp.c"),
            Object(NonMatching, "RVL_SDK/nhttp/d_nhttp_common.c"),
            Object(Matching, "RVL_SDK/ssl/ssl_api.c"),
            Object(Matching, "RVL_SDK/ssl/ssl_mutex.c"),
            Object(Matching, "RVL_SDK/si/SIBios.c"),
            Object(Matching, "RVL_SDK/si/SISamplingRate.c"),
            Object(Matching, "RVL_SDK/sp/sp.c"),
            Object(Matching, "RVL_SDK/bte/hcicmds.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/bte_hcisu.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/rfc_port_if.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/bte_init.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/gki_buffer.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/gki_time.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/gki_ppc.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/hcisu_h2.c", cflags=cflags_rvl_bte),
            Object(NonMatching, "RVL_SDK/bte/uusb_ppc.c", cflags=cflags_rvl_bte, mw_version="GC/3.0a5.2"),
            Object(Matching, "RVL_SDK/bte/bta_dm_cfg.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/bta_hh_cfg.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/bta_sys_cfg.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/bte_logmsg.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/bte_main.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/btu_task1.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/bd.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/bta_sys_conn.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/bta_sys_main.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/bta_dm_act.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/bta_dm_api.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/bta_dm_main.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/bta_dm_pm.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/bta_hh_act.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/bta_hh_api.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/bta_hh_main.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/bta_hh_utils.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/btm_acl.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/btm_dev.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/btm_devctl.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/btm_discovery.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/btm_inq.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/btm_main.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/btm_pm.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/btm_sco.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/btm_sec.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/btu_hcif.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/btu_init.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/wbt_ext.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/gap_api.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/gap_conn.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/gap_utils.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/hidd_api.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/hidd_conn.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/hidd_mgmt.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/hidd_pm.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/hidh_api.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/hidh_conn.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/l2c_api.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/l2c_csm.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/l2c_link.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/l2c_main.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/l2c_utils.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/port_api.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/port_rfc.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/port_utils.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/rfc_l2cap_if.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/rfc_mx_fsm.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/rfc_port_fsm.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/rfc_ts_frames.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/rfc_utils.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/sdp_api.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/sdp_db.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/sdp_discovery.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/sdp_main.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/sdp_server.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/sdp_utils.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/ptim.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/bte/utl.c", cflags=cflags_rvl_bte),
            Object(Matching, "RVL_SDK/wud/debug_msg.c", cflags=cflags_rvl_wud),
            Object(Matching, "RVL_SDK/wud/WUD.c", cflags=cflags_rvl_wud, mw_version="GC/3.0a5.2"),
            Object(Matching, "RVL_SDK/wud/WUDHidHost.c", cflags=cflags_rvl_wud),
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
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_arcResourceAccessor.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_common.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_window.cpp", cflags=cflags_rvl_hbm, mw_version="GC/3.0a5.2"),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/math/math_triangular.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/ut/ut_TextWriterBase.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_TagProcessorBase.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_ResFont.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_ResFontBase.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_Util.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundHandle.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_MmlSeqTrackAllocator.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundArchiveLoader.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_WsdTrack.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_WaveSound.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_BasicSound.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_BankFile.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_Bank.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_WaveFile.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_StrmSound.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_TaskManager.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_TaskThread.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundHeap.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_MmlSeqTrack.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/snd/snd_ExternalSoundPlayer.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_NandSoundArchive.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_RemoteSpeaker.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_RemoteSpeakerManager.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_FrameHeap.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_layout.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_material.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundPlayer.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundStartable.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_MmlParser.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SeqFile.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/snd/snd_SeqPlayer.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_WsdFile.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/snd/snd_WsdPlayer.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_textBox.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundArchiveFile.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/snd/snd_WavePlayer.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_MemorySoundArchive.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_DisposeCallbackManager.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_DvdSoundArchive.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundArchivePlayer.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/snd/snd_AxVoice.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_pane.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/snd/snd_AxManager.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_EnvGenerator.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_InstancePool.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/snd/snd_SeqSound.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SeqSoundHandle.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/snd/snd_SeqTrack.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundArchive.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundSystem.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_SoundThread.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_StrmChannel.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_StrmFile.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/HBMBase.cpp", cflags=[*cflags_rvl_hbm, "-DHBM_REVISION=2"]),
            Object(Matching, "RVL_SDK/hbm/HBMAnmController.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/HBMFrameController.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/HBMGUIManager.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/HBMController.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/HBMRemoteSpk.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/db/db_assert.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/db/db_console.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/db/db_directPrint.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/db/db_mapFile.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_Channel.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_StrmPlayer.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_StrmSoundHandle.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/snd/snd_WaveSoundHandle.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_CharWriter.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/lyt/lyt_group.cpp", cflags=cflags_rvl_hbm),
            Object(NonMatching, "RVL_SDK/hbm/nw4hbm/ut/ut_binaryFileFormat.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_CharStrmReader.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_DvdFileStream.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_FileStream.cpp", cflags=cflags_rvl_hbm),
            Object(Matching, "RVL_SDK/hbm/nw4hbm/ut/ut_Font.cpp", cflags=cflags_rvl_hbm),
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
