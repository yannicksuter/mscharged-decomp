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
    "R4QP01",  # 1: Europe, Rev 2
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
config.linker_alignment_splits = config.config_path.parent / "splits.txt"
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

# NHTTP_thread retains every intra-unit helper call (CheckHeaderEnd stays a
# real call); auto-inlining collapses its single-caller helpers, unlike the
# sibling NHTTP units whose retained bytes require auto-inlining.
cflags_game_inline2 = [
    *cflags_game_common,
    "-inline level=2",
]

cflags_rvl_nhttp = [
    *(flag for flag in cflags_rvl_sdk if flag != "-inline auto"),
    "-inline on",
]

# DWC consumes the vendored GameSpy interfaces below the RVL SDK source root.
cflags_rvl_dwc = [
    *cflags_rvl_sdk,
    "-DSDK_FINALROM=1",
    "-i src/RVL_SDK",
    "-i src/RVL_SDK/gamespy/GP",
    "-i src/RVL_SDK/gamespy/common",
    "-i src/RVL_SDK/gamespy/gstats",
    "-i src/RVL_SDK/gamespy/natneg",
    "-i src/RVL_SDK/gamespy/qr2",
    "-i src/RVL_SDK/gamespy/serverbrowsing",
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
            # Game
            Object(MatchingFor("R4QE01"), "Game/AIPad.cpp"),
            Object(MatchingFor("R4QE01"), "Game/AnimInventory.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/AsyncLoading.cpp", extra_cflags=["-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Ball.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/BasicStadium.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Blinker.cpp", cflags=cflags_game_deferred),
            Object(MatchingFor("R4QE01"), "Game/Character.cpp", extra_cflags=["-inline deferred", "-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/CharacterEffects.cpp"),
            Object(MatchingFor("R4QE01"), "Game/CharacterTemplate.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/CharacterTriggers.cpp", cflags=cflags_game + ["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/CharacterTweaks.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/CrowdRiot.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/DebugWriteCache.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/DetermDataEvent.cpp"),
            Object(MatchingFor("R4QE01"), "Game/DetInput.cpp"),
            Object(MatchingFor("R4QE01"), "Game/EventDataTypes.cpp", cflags=cflags_game, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/ExcitementSystem.cpp", cflags=cflags_game, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Field.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Formation.cpp", cflags=cflags_game, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/FormationDefines.cpp", cflags=cflags_game, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FriendManager.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Game.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/GameInfo.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/GameObjectLighting.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/GameTweaks.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/GameTweaksManager.cpp", extra_cflags=["-use_lmw_stmw off", "-sym on", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Goalie.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/GoalieFatigue.cpp"),
            Object(MatchingFor("R4QE01"), "Game/GoalieTweaks.cpp"),
            Object(MatchingFor("R4QE01"), "Game/HBMManager.cpp", extra_cflags=["-sym on", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/InputManager.cpp"),
            Object(MatchingFor("R4QE01"), "Game/InputRouter.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/InterpreterOperations.cpp", cflags=cflags_game),
            Object(MatchingFor("R4QE01"), "Game/InterpreterCore.cpp", cflags=cflags_game + ["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/LANConnectionMessages.cpp"),
            Object(MatchingFor("R4QE01"), "Game/LANDiscoveryMessages.cpp"),
            Object(MatchingFor("R4QE01"), "Game/LANLobby.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/LANMessageRegistry.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/main.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/MiiManager.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Net.cpp"),
            Object(MatchingFor("R4QE01"), "Game/NetMeshEdge.cpp"),
            Object(MatchingFor("R4QE01"), "Game/NetMeshModelLoader.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/NetworkTournamentMessages.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/NetTournManager.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/NetworkDebug.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/NetworkDiagnostics.cpp"),
            Object(MatchingFor("R4QE01"), "Game/NetworkDraftMessages.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/NetworkDraft.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/NetworkInput.cpp"),
            Object(MatchingFor("R4QE01"), "Game/NetMessageAllInputs.cpp"),
            Object(MatchingFor("R4QE01"), "Game/NetworkInputRecording.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/NetworkLobby.cpp", extra_cflags=[flag for flag in cflags_rvl_dwc if flag.startswith("-i ")] + ["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/NetworkMessageRegistry.cpp"),
            Object(MatchingFor("R4QE01"), "Game/NetworkGameStartMessage.cpp"),
            Object(MatchingFor("R4QE01"), "Game/NetworkMessages.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/NetworkPauseMessages.cpp"),
            Object(MatchingFor("R4QE01"), "Game/NetworkSkipNisMessages.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/NetworkMegaStrikeMessages.cpp"),
            Object(MatchingFor("R4QE01"), "Game/NetworkMessageSerializer.cpp"),
            Object(MatchingFor("R4QE01"), "Game/NetworkRandom.cpp"),
            Object(MatchingFor("R4QE01"), "Game/NetworkRandomSeed.cpp"),
            Object(MatchingFor("R4QE01"), "Game/NetworkSession.cpp", extra_cflags=[flag for flag in cflags_rvl_dwc if flag.startswith("-i ")] + ["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/NetworkSessionData.cpp"),
            Object(MatchingFor("R4QE01"), "Game/NetworkSocket.cpp"),
            Object(MatchingFor("R4QE01"), "Game/NetworkStats.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/NetworkStatsManager.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/NetworkSeasonCalendar.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/NetworkSync.cpp"),
            Object(MatchingFor("R4QE01"), "Game/NisPlayer.cpp", extra_cflags=["-inline deferred", "-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/objectblur.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/PackedDetInput.cpp"),
            Object(MatchingFor("R4QE01"), "Game/PadActions.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/PadMonkey.cpp", extra_cflags=["-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Physics.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/PhysicsAIBall.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Player.cpp", extra_cflags=["-inline deferred", "-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/PoseAccumulator.cpp", extra_cflags=["-inline auto", "-inline deferred", "-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/PoseNode.cpp"),
            Object(MatchingFor("R4QE01"), "Game/RenderSnapshot.cpp", cflags=cflags_game),
            Object(MatchingFor("R4QE01"), "Game/Replay.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/ReplayChoreo.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/ReplayManager.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/RumbleActions.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SAnim.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SAnimDecode.cpp"),
            Object(MatchingFor("R4QE01"), "Game/ScriptTuning.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SHierarchy.cpp", extra_cflags=["-inline deferred"]),
            Object(MatchingFor("R4QE01"), "Game/Team.cpp", cflags=cflags_game_deferred),
            Object(MatchingFor("R4QE01"), "Game/Terrain.cpp", extra_cflags=["-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/TerrainTweaks.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/TrophyInfo.cpp"),
            Object(MatchingFor("R4QE01"), "Game/CharacterLoader.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/tu_8013E2EC.cpp"),
            Object(MatchingFor("R4QE01"), "Game/TweakCallback.cpp", extra_cflags=["-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/TweakConfig.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/TweakEntry.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/TweakFileLoader.cpp"),
            Object(MatchingFor("R4QE01"), "Game/TweakNameRecycler.cpp"),
            Object(MatchingFor("R4QE01"), "Game/TweakNode.cpp", cflags=cflags_game, extra_cflags=["-inline nobottomup", "-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/TweakRegistry.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/TweaksBase.cpp"),
            Object(MatchingFor("R4QE01"), "Game/TweakValue.cpp"),
            Object(MatchingFor("R4QE01"), "Game/TweakValueBase.cpp"),
            Object(MatchingFor("R4QE01"), "Game/TweakAction.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Weather.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/WeatherData.cpp"),
            Object(MatchingFor("R4QE01"), "Game/world.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/WorldTriggers.cpp"),

            # Game/AI
            Object(MatchingFor("R4QE01"), "Game/AI/AISandbox.cpp"),
            Object(MatchingFor("R4QE01"), "Game/AI/AiUtil.cpp"),
            Object(MatchingFor("R4QE01"), "Game/AI/AvoidableObject.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/AI/AvoidController.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/AI/Desire.cpp", extra_cflags=["-inline noauto", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/DesireCutAndBreak.cpp", extra_cflags=["-inline noauto", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/DesireDeke.cpp", extra_cflags=["-inline noauto", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/DesireGetInPosition.cpp", extra_cflags=["-inline noauto", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/DesireGetOpen.cpp", extra_cflags=["-inline noauto", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/DesireHit.cpp", extra_cflags=["-inline noauto", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/DesireInterceptBall.cpp", extra_cflags=["-inline noauto", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/DesireMark.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/DesireMegaStrike.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/DesirePass.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/DesireReceivePass.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/AI/DesireRunToNet.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/DesireShoot.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/DesireSlideAttack.cpp"),
            Object(MatchingFor("R4QE01"), "Game/AI/DesireSteering.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/AI/DesireSuperPower.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/AI/DesireUsePowerup.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/DesireUserControlled.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/Fielder.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/AI/FielderAbility.cpp"),
            Object(MatchingFor("R4QE01"), "Game/AI/FielderActions.cpp", extra_cflags=["-inline deferred", "-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/AI/FilteredRandom.cpp", cflags=cflags_game_deferred),
            Object(MatchingFor("R4QE01"), "Game/AI/Fuzzy.cpp"),
            Object(MatchingFor("R4QE01"), "Game/AI/FuzzyVariant.cpp", cflags=[*cflags_game_deferred, "-char signed"], extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/AI/GoalieActions.cpp", cflags=cflags_game, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/AI/GoalieLooseBall.cpp"),
            Object(MatchingFor("R4QE01"), "Game/AI/GoalieSave.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/AI/HeadTrack.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/Powerups.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/ShotMeter.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/SkillTweaks.cpp", cflags=cflags_game, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/AI/SpaceSearch.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/StatsGatherer.cpp", cflags=cflags_game_deferred),
            Object(MatchingFor("R4QE01"), "Game/AI/TeamDesire.cpp"),
            Object(MatchingFor("R4QE01"), "Game/AI/TeamPlayMachine.cpp"),
            Object(MatchingFor("R4QE01"), "Game/AI/DesireStatusEffects.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/FielderDesireMachine.cpp"),
            Object(MatchingFor("R4QE01"), "Game/AI/FielderDesireTransitions.cpp"),
            Object(MatchingFor("R4QE01"), "Game/AI/tu_8030EDB0.cpp", cflags=[*cflags_game, "-char signed"], extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/AIContext.cpp", cflags=[*cflags_game, "-char signed"], extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/AI/ScriptActionQueue.cpp", cflags=[*cflags_game, "-char signed"], extra_cflags=["-ipa file", "-inline noauto", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/AI/tu_803115F4.cpp", cflags=[*cflags_game, "-char signed"]),
            Object(MatchingFor("R4QE01"), "Game/AI/Scripts/ScriptCaching.cpp", cflags=[*cflags_game, "-char signed"], extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/AI/FuzzyRuntimeBase.cpp", cflags=[*cflags_game, "-char signed"], extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/AI/TransitionFunc.cpp", cflags=[*cflags_game, "-char signed"], extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/AI/shdStateMachine.cpp", cflags=[*cflags_game, "-char signed"], extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/ScriptMachine.cpp", cflags=[*cflags_game_deferred, "-char signed"], extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/AI/TutorialMegastrikeDesire.cpp"),
            Object(MatchingFor("R4QE01"), "Game/AI/Variant.cpp", cflags=[*cflags_game_deferred, "-char signed"], extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/AI/Scripts/FuzzyAIRuntime.cpp", cflags=[*cflags_game_deferred, "-char signed"], extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/AI/Scripts/ScriptDefines.cpp", cflags=cflags_game_deferred),
            Object(MatchingFor("R4QE01"), "Game/AI/Scripts/ScriptQuestions.cpp", extra_cflags=["-ipa file"]),

            # Game/AnimProps
            Object(MatchingFor("R4QE01"), "Game/AnimProps/globalanimproperties.cpp"),
            Object(MatchingFor("R4QE01"), "Game/AnimProps/goalieanimproperties.cpp"),

            # Game/Audio
            Object(MatchingFor("R4QE01"), "Game/Audio/audio.cpp", extra_cflags=["-inline auto,nobottomup,depth=5", "-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/AudioBackend.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/AudioBankLoader.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/AudioBankTable.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Audio/AudioBundleManager.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/AudioBundleManagerPlatform.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Audio/AudioCalculation.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/AudioEffectBinding.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/AudioEffects.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/AudioResourceBundle.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Audio/AudioResourceLoader.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Audio/AudioResourceLoadOwner.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Audio/AudioResourcePlatform.cpp", extra_cflags=["-ipa file", "-inline noauto", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/AudioResourceRuntime.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/AudioRpc.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/AudioRuntimeGroup.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/AudioSequenceEvent.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/AudioSequenceInstance.cpp", extra_cflags=["-ipa file", "-inline auto,depth=3"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/AudioSlider.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/AudioSource.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/AudioSystem.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/AuxEffectMap.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/CategoryVolume.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/Delay.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/GameStreams.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Audio/LowPassFilter.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/Pitch.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/Plat3dSoundSrc.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/Reverb.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/SoundInstance.cpp", extra_cflags=["-ipa file", "-inline auto,depth=3", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/SoundMap.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/Transition.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Audio/AudioScriptRuntime.cpp", extra_cflags=["-inline auto,depth=3", "-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/XSoundCueHandle.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Audio/XSoundHandle.cpp"),

            # Game/Camera
            Object(MatchingFor("R4QE01"), "Game/Camera/animcam.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Camera/AnimViewerCam.cpp", cflags=cflags_game_deferred),
            Object(MatchingFor("R4QE01"), "Game/Camera/BaseCam.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Camera/CameraMan.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Camera/DebugCam.cpp", cflags=cflags_game, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Camera/FaceCam.cpp", cflags=cflags_game_deferred),
            Object(MatchingFor("R4QE01"), "Game/Camera/FollowCam.cpp", cflags=cflags_game, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Camera/GameplayCam.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Camera/GoalCam.cpp", cflags=cflags_game_deferred),
            Object(MatchingFor("R4QE01"), "Game/Camera/kickoffcam.cpp", cflags=cflags_game_deferred),
            Object(MatchingFor("R4QE01"), "Game/Camera/MatrixEffectCam.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Camera/noisefilter.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Camera/ReplayCamera.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Camera/rumblefilter.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Camera/ShootToScoreCam.cpp", cflags=cflags_game_deferred),
            Object(MatchingFor("R4QE01"), "Game/Camera/TopDownCamera.cpp", cflags=cflags_game_deferred),
            Object(MatchingFor("R4QE01"), "Game/Camera/GameplayCameraEffects.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file"]),

            # Game/Core
            Object(MatchingFor("R4QE01"), "Game/Core/mtRandom.cpp"),

            # Game/DB
            Object(MatchingFor("R4QE01"), "Game/DB/BasicGameInfo.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/DB/CupManager.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/DB/GameProgress.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/DB/StrikerChallenge.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/DB/SaveLoad.cpp", extra_cflags=["-ipa file", "-inline noauto", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/DB/Simmer.cpp"),
            Object(MatchingFor("R4QE01"), "Game/DB/StadiumInfo.cpp"),
            Object(MatchingFor("R4QE01"), "Game/DB/StatsTracker.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/DB/UserOptions.cpp"),

            # Game/Debug
            Object(MatchingFor("R4QE01"), "Game/Debug/FrameCounter.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Debug/Histogram.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Debug/ShapeRender.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Debug/TimeRegions.cpp", cflags=cflags_game),

            # Game/Drawable
            Object(MatchingFor("R4QE01"), "Game/Drawable/DrawableBall.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Drawable/DrawableBirdoEgg.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Drawable/DrawableBulletBill.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Drawable/DrawableCharacter.cpp", extra_cflags=["-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Drawable/DrawableDaisyFist.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Drawable/DrawableFlyingCamera.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Drawable/DrawableHammer.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Drawable/DrawableKoopaShell.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Drawable/DrawableModel.cpp", cflags=cflags_game, extra_cflags=["-sym on", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Drawable/DrawableNetMesh.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Drawable/DrawablePowerup.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Drawable/DrawableThwomp.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Drawable/DrawableYoshiEgg.cpp", extra_cflags=["-ipa file"]),

            # Game/Effects
            Object(MatchingFor("R4QE01"), "Game/Effects/EffectsGroup.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Effects/EffectsTemplate.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Effects/EmissionController.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Effects/EffectsBundleData.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Effects/EmissionManager.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Effects/EmitterCallbacks.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Effects/ParticleSystem.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Effects/PhotoFlashEffect.cpp"),

            # Game/FE
            Object(MatchingFor("R4QE01"), "Game/FE/BaseGameSceneManager.cpp", cflags=cflags_game, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/FE/BaseSceneHandler.cpp"),
            Object(MatchingFor("R4QE01"), "Game/FE/feAnimation.cpp"),
            Object(MatchingFor("R4QE01"), "Game/FE/feAsyncImage.cpp", cflags=cflags_game_deferred),
            Object(MatchingFor("R4QE01"), "Game/FE/FEAudio.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/feBackButton.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/FE/feButtonComponent.cpp"),
            Object(MatchingFor("R4QE01"), "Game/FE/feCamera.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/feCaptainComponent.cpp", extra_cflags=["-inline auto", "-inline deferred", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/feCharacterPDAComponent.cpp", extra_cflags=["-inline auto", "-inline deferred", "-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/FE/CaptainSelectionOrder.cpp"),
            Object(MatchingFor("R4QE01"), "Game/OverlayManager.cpp", extra_cflags=["-inline auto", "-inline deferred", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/feCupFlow.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/feDPD.cpp"),
            Object(MatchingFor("R4QE01"), "Game/FE/feFinder.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/feFontResource.cpp"),
            Object(MatchingFor("R4QE01"), "Game/FE/feGroup.cpp"),
            Object(MatchingFor("R4QE01"), "Game/FE/feHelpFuncs.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/FE/feImage.cpp"),
            Object(MatchingFor("R4QE01"), "Game/FE/feInput.cpp", extra_cflags=["-inline depth=3", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/feLayer.cpp"),
            Object(MatchingFor("R4QE01"), "Game/FE/feLibObject.cpp"),
            Object(MatchingFor("R4QE01"), "Game/FE/feManager.cpp", extra_cflags=["-ipa file"]),
            # Retained menu template copies; the emitting source unit is unidentified.
            Object(MatchingFor("R4QE01"), "Game/FE/MenuListComponentInstantiations.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/FE/feModelManager.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/FE/feMusic.cpp", cflags=cflags_game_deferred),
            Object(MatchingFor("R4QE01"), "Game/FE/feOptionsSubMenus.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/SHCrossFader.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/FE/feOnlinePlayerRow.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/FE/fePackage.cpp"),
            Object(MatchingFor("R4QE01"), "Game/FE/fePageControls.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/fePointer.cpp"),
            Object(MatchingFor("R4QE01"), "Game/FE/fePointerButton.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/fePointerManager.cpp", extra_cflags=["-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/FE/fePopupMenu.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/FE/fePresentation.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/feRender.cpp", cflags=cflags_game),
            Object(MatchingFor("R4QE01"), "Game/FE/feResourceManager.cpp", cflags=cflags_game, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/FE/feScene.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/feSceneManager.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/feSceneResource.cpp"),
            Object(MatchingFor("R4QE01"), "Game/FE/feScrollBar.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/FE/feScrollText.cpp", extra_cflags=["-inline auto", "-inline deferred", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/feSlideMenu.cpp"),
            Object(MatchingFor("R4QE01"), "Game/FE/feText.cpp"),
            Object(MatchingFor("R4QE01"), "Game/FE/feTextureResource.cpp"),
            Object(MatchingFor("R4QE01"), "Game/FE/feTimer.cpp", extra_cflags=["-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/FE/GameSceneManager.cpp"),
            Object(MatchingFor("R4QE01"), "Game/FE/LidOpenMessage.cpp", cflags=cflags_game),
            Object(MatchingFor("R4QE01"), "Game/FE/MatchSummary.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/OnlineRanking.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/FE/tlComponent.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/tlComponentInstance.cpp"),
            Object(MatchingFor("R4QE01"), "Game/FE/tlInstance.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/tlSlide.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/tlTextInstance.cpp"),
            Object(MatchingFor("R4QE01"), "Game/FE/tlTextInstance_runtime.cpp"),
            Object(MatchingFor("R4QE01"), "Game/FE/tlDefault.cpp"),
            Object(MatchingFor("R4QE01"), "Game/FE/Overlay/OverlayHandlerChallengePreview.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/FE/Overlay/OverlayHandlerControllerMap.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/Overlay/OverlayHandlerDefensivePlay.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/Overlay/OverlayHandlerDemo.cpp"),
            Object(MatchingFor("R4QE01"), "Game/FE/Overlay/OverlayHandlerGoal.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/FE/Overlay/OverlayHandlerHUD.cpp", extra_cflags=["-inline noauto", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/Overlay/OverlayHandlerInGameText.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/FE/Overlay/OverlayHandlerMegaStrikeMeter.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/Overlay/OverlayHandlerPIP.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/FE/Overlay/OverlayHandlerStrikerTimes.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/FE/Overlay/OverlayHandlerSuperAbility.cpp", extra_cflags=["-ipa file"]),

            # Game/Font
            Object(MatchingFor("R4QE01"), "Game/Font/FontLoading.cpp", cflags=cflags_game_deferred),
            Object(MatchingFor("R4QE01"), "Game/Font/fontmanager.cpp", extra_cflags=["-ipa file", "-sym on"]),

            # Game/GL
            Object(MatchingFor("R4QE01"), "Game/GL/GLColourMeshWriter.cpp"),
            Object(MatchingFor("R4QE01"), "Game/GL/GLCompactColourMeshWriter.cpp"),
            Object(MatchingFor("R4QE01"), "Game/GL/GLFloatTexturedColourMeshWriter.cpp"),
            Object(MatchingFor("R4QE01"), "Game/GL/GLFourTextureAddMeshWriter.cpp"),
            Object(MatchingFor("R4QE01"), "Game/GL/GLInventory.cpp", extra_cflags=["-ipa file", "-inline depth=3", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/GL/glModelBuilder.cpp"),
            Object(MatchingFor("R4QE01"), "Game/GL/GLShadowBlendMeshWriter.cpp"),
            Object(MatchingFor("R4QE01"), "Game/GL/GLSkinMesh.cpp"),
            Object(MatchingFor("R4QE01"), "Game/GL/GLTextureAnim.cpp"),
            Object(MatchingFor("R4QE01"), "Game/GL/GLTexturedColourMeshWriter.cpp"),
            Object(MatchingFor("R4QE01"), "Game/GL/GLVertexAnim.cpp"),
            Object(MatchingFor("R4QE01"), "Game/GL/GLWarbleMeshWriter.cpp"),
            Object(MatchingFor("R4QE01"), "Game/GL/MeshWriter.cpp"),
            Object(MatchingFor("R4QE01"), "Game/GL/ShaderSkinMesh.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/GL/GLMovieMeshWriter.cpp"),

            # Game/Pad
            Object(MatchingFor("R4QE01"), "Game/Pad/FlickDetection.cpp", cflags=cflags_game_deferred),

            # Game/Physics
            Object(MatchingFor("R4QE01"), "Game/Physics/CharacterPhysicsElement.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/CollisionSpace.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/LoadablePhysicsMesh.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsBall.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsBanana.cpp", extra_cflags=["-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsBirdoEgg.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsBox.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsBulletBill.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsCapsule.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsCharacter.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsCharacterBase.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsCharacterBaseData.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsColumn.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsCompositeObject.cpp", mw_version="GC/3.0a3", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsCylinder.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsEventQueue.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsFakeBall.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsFinitePlane.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsGoalie.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsGroundPlane.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsHammer.cpp", extra_cflags=["-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsKoopaShell.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsNet.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsNPC.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsObject.cpp", extra_cflags=["-opt nolifetimes"]),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsPatch.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsPlane.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsRoundedCorner.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsShell.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsShockwave.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsSphere.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsThwomp.cpp", extra_cflags=["-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsTransform.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsTriggerVolume.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsWall.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsWaluigiWall.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsWorld.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Physics/PhysicsYoshiEgg.cpp"),

            # Game/Render
            Object(MatchingFor("R4QE01"), "Game/Render/AttackSideIndicators.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/BirdoEgg.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Render/BulletBill.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/ChainChomp.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Render/CrowdImpostorManager.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Render/CrowdImpostors.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Render/CrowdLayoutObject.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Render/CrowdModelCollection.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/World/WorldVisibility.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Render/DaisyFist.cpp", cflags=cflags_game),
            Object(MatchingFor("R4QE01"), "Game/Render/depthoffield.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/DiddyBanana.cpp", cflags=cflags_game, extra_cflags=["-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Render/ElectricFence.cpp", cflags=cflags_game, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Render/FlareHandler.cpp", cflags=cflags_game),
            Object(MatchingFor("R4QE01"), "Game/Render/FlyingCamera.cpp", cflags=cflags_game, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Render/FrontEndPresentation.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Render/HammerObject.cpp", cflags=cflags_game),
            Object(MatchingFor("R4QE01"), "Game/Render/HighRange.cpp", cflags=cflags_game),
            Object(MatchingFor("R4QE01"), "Game/Render/HomeButtonFade.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/Impostor.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/ImpostorCharacter.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Render/ImpostorCluster.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/ImpostorLightingColour.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/ImpostorLighting.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Render/ImpostorManager.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Render/ImpostorModel.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/ImpostorSprite.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Render/Indicators.cpp", cflags=cflags_game),
            Object(MatchingFor("R4QE01"), "Game/Render/Jumbotron.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/KoopaShellObject.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/MegaBallIndicators.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/MegastrikeBackgroundOverlay.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/NetMesh.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Render/Nis.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Render/NPCManager.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Render/NumberDisplay.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Render/PeachPhoto.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Render/Presentation.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Render/RenderShadow.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Render/RLView.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/RLViewLayers.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Render/ShadowVolume.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/ShootToScoreArrow.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/ShootToScoreMeter.cpp", cflags=cflags_game_deferred),
            Object(MatchingFor("R4QE01"), "Game/Render/SkinAnimatedMovableNPC.cpp", cflags=cflags_game_deferred),
            Object(MatchingFor("R4QE01"), "Game/Render/SkinAnimatedNPC.cpp", cflags=cflags_game),
            Object(MatchingFor("R4QE01"), "Game/Render/StadiumLoading.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Render/StadiumTweaks.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/ThwompObject.cpp", cflags=cflags_game),
            Object(MatchingFor("R4QE01"), "Game/Render/TimedObject.cpp", cflags=cflags_game, extra_cflags=["-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Render/WarbleOwner.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/WindDebris.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Render/WindDebrisConfig.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/StadiumPhysicsObject.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Render/StadiumWorldObjects.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/PlanarShadowDrawable.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/ChargeShadowDrawable.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/SolarFlareEffect.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Render/NisPlayerOverlay.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Render/Warble.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/Wiper.cpp", cflags=cflags_game, extra_cflags=["-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Render/Frustum.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/WorldNPC.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Render/YoshiEggObject.cpp", cflags=cflags_game, extra_cflags=["-ipa file"]),

            # Game/SAnim
            Object(MatchingFor("R4QE01"), "Game/SAnim/AnimRetargeter.cpp", extra_cflags=["-ipa file", "-inline noauto"]),
            Object(MatchingFor("R4QE01"), "Game/SAnim/pnBlender.cpp", extra_cflags=["-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SAnim/pnFeather.cpp", extra_cflags=["-inline auto,depth=3", "-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SAnim/pnSAnimController.cpp", extra_cflags=["-inline deferred", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SAnim/pnSingleAxisBlender.cpp", extra_cflags=["-sym on", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SAnim/pnScaleBlender.cpp", extra_cflags=["-sym on"]),

            # Game/SH
            Object(MatchingFor("R4QE01"), "Game/SH/OnlineGameInfo.cpp"),
            Object(MatchingFor("R4QE01"), "Game/SH/SHBootLoading.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHChallengeSelect.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHChooseCaptains.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHChooseSidekicks.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHChooseSides.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHCredits.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHCupCheater.cpp", extra_cflags=["-inline auto", "-inline deferred", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHCupFinalRounds.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHCupHub.cpp", extra_cflags=["-inline noauto", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHCupKnockout.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHCupNews.cpp", cflags=cflags_game_deferred),
            Object(MatchingFor("R4QE01"), "Game/SH/SHGameplayOptions.cpp", extra_cflags=["-inline noauto", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHGameResults.cpp"),
            Object(MatchingFor("R4QE01"), "Game/SH/SHHallOfFame.cpp", extra_cflags=["-inline noauto", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHHallOfFameHistory.cpp", extra_cflags=["-inline noauto", "-sym on", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHHallOfFameRoom.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHHallOfFameSummary.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHLoading.cpp", extra_cflags=["-inline auto", "-sym on", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHMainMenu.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHMoviePlayer.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHNavigation.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHNetworkStart.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHOnlineConnectionQuality.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHOnlineFriendCodeEntry.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHOnlineFriends.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHOnlineFriendsChooseSides.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHOnlineFriendsDraft.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHOnlineGuestControllerSelect.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHOnlineHub.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHOnlineInvitePlayers.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHOnlineInvitePreview.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHOnlineInviteResponse.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHOnlineInviteStatus.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHOnlineLogin.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHOnlineMatchmakingDraft.cpp", extra_cflags=["-ipa file", "-sym on"] + [flag for flag in cflags_rvl_dwc if flag.startswith("-i ")]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHOnlineMiiSelect.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHOnlineMiiSelectOverlay.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHOnlinePlayerCount.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHOnlineRanking.cpp", cflags=cflags_game, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHOptions.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHOptionsCheatsList.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHPause.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHPausePostGame.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHRoadToStrikersCupHub.cpp", cflags=cflags_game, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHStadiumSelect.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHStrikerCupAwards.cpp", cflags=cflags_game, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHStrikerCupStandings.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHStrikerTimesBase.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHStrikerTimesChallenge.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/SH/SHTitleScreen.cpp", extra_cflags=["-ipa file"]),

            # Game/Sys
            Object(MatchingFor("R4QE01"), "Game/Sys/clock.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Sys/movie.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Sys/simpleparser.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Sys/tweak.cpp", extra_cflags=["-ipa file", "-sym on"]),

            # Game/Task
            Object(MatchingFor("R4QE01"), "Game/Task/BeginFrameTask.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Task/ComUpdateTask.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Task/DispatchEventsTask.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Task/EndFrameTask.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Task/FixedUpdateTask.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Task/FrontEndTask.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Task/GameRenderTask.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Task/LoadingTask.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Task/MovieRenderTask.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Task/NetworkUpdateTask.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Task/ParticleUpdateCallbacks.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Task/ParticleUpdateTask.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Task/PlatPadUpdateTask.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Task/ProfilerTask.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Task/ResetTask.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Task/SmokeTestUpdateTask.cpp", extra_cflags=["-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Task/TextWindowTask.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Task/TransitionTask.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Task/TweakerTask.cpp"),
            Object(MatchingFor("R4QE01"), "Game/Task/WorldUpdateTask.cpp"),

            # Game/Transitions
            Object(MatchingFor("R4QE01"), "Game/Transitions/ColourBlendScreenTransition.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Transitions/ModelTransition.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Transitions/ScreenTransitionManager.cpp", extra_cflags=["-inline auto", "-inline deferred", "-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Transitions/ScriptedTransition.cpp", extra_cflags=["-inline auto", "-inline deferred", "-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/Transitions/TransitionSequence.cpp"),

            # Game/Triggers
            Object(MatchingFor("R4QE01"), "Game/Triggers/AnimTagScript.cpp", cflags=cflags_game, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/Triggers/AnimTrigger.cpp", cflags=cflags_game_deferred),
            Object(MatchingFor("R4QE01"), "Game/Triggers/BinaryTriggerFile.cpp", cflags=cflags_game_deferred),
            Object(MatchingFor("R4QE01"), "Game/Triggers/SebringAnimScript.cpp", cflags=cflags_game_deferred),

            # Game/World
            Object(MatchingFor("R4QE01"), "Game/World/WorldPhysics.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/World/worldanim.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "Game/World/worldanimobjects.cpp", extra_cflags=["-inline nobottomup", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "Game/World/WorldEffect.cpp", extra_cflags=["-inline nobottomup", "-ipa file"]),

            # NL
            Object(MatchingFor("R4QE01"), "NL/blowfish.cpp"),
            Object(MatchingFor("R4QE01"), "NL/InflateStream.cpp", extra_cflags=["-i src/zlib"]),
            Object(MatchingFor("R4QE01"), "NL/math.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/plane.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/polar.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/MemAlloc.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/nlAllocatorStack.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlAsyncFileBuffer.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlAVLTree.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/nlBind.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlBufferedWriter.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlBundleFile.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlConfig.cpp", extra_cflags=["-ipa file", "-iso_templates on", "-sym on"]),
            Object(MatchingFor("R4QE01"), "NL/nlDebug.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlDebugFile.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlDebugString.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlDebugViews.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlEndian.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlEvent.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "NL/nlFile.cpp", extra_cflags=["-i src/zlib"]),
            Object(MatchingFor("R4QE01"), "NL/nlFileGC.cpp", extra_cflags=["-inline nobottomup", "-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "NL/nlFont.cpp", cflags=cflags_game_deferred, extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "NL/nlFunctionMemory.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/nlInit.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlIntersection.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlLocalization.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlMain.cpp", cflags=cflags_game_deferred),
            Object(MatchingFor("R4QE01"), "NL/nlMath.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlMemory.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlPolygonRegion.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/nlPrint.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlRegistry.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "NL/nlRegistryLookup.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlRegistryOwner.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "NL/nlSlotPool.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlString.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlStringSupport.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/nlTask.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlTextBox.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlTextEscape.cpp", cflags=[*cflags_game_deferred, "-sym on"]),
            Object(MatchingFor("R4QE01"), "NL/nlTicker.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlTime.cpp"),
            Object(MatchingFor("R4QE01"), "NL/nlTimer.cpp"),
            Object(MatchingFor("R4QE01"), "NL/PointerEntryTable.cpp", extra_cflags=["-sym on"]),
            Object(MatchingFor("R4QE01"), "NL/nlRandom.cpp", cflags=cflags_game_deferred),
            Object(MatchingFor("R4QE01"), "NL/utility.cpp", cflags=cflags_game_deferred),

            # NL/gc
            Object(MatchingFor("R4QE01"), "NL/gc/gcSwizzler.cpp"),

            # NL/gl
            Object(MatchingFor("R4QE01"), "NL/gl/gl.cpp"),
            Object(MatchingFor("R4QE01"), "NL/gl/glDraw2.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/gl/glDraw3.cpp"),
            Object(MatchingFor("R4QE01"), "NL/gl/glDrawSyncLog.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/gl/glFont.cpp"),
            Object(MatchingFor("R4QE01"), "NL/gl/glLoadModel.cpp", extra_cflags=["-inline nobottomup", "-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "NL/gl/glMaterialProgram.cpp", extra_cflags=["-inline nobottomup", "-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "NL/gl/glMaterialParameters.cpp"),
            Object(MatchingFor("R4QE01"), "NL/gl/glMatrix.cpp"),
            Object(MatchingFor("R4QE01"), "NL/gl/glResourcePool.cpp"),
            Object(MatchingFor("R4QE01"), "NL/gl/glMemory.cpp"),
            Object(MatchingFor("R4QE01"), "NL/gl/glMemoryInit.cpp"),
            Object(MatchingFor("R4QE01"), "NL/gl/glModel.cpp"),
            Object(MatchingFor("R4QE01"), "NL/gl/glMultiTextureModelWriter.cpp"),
            Object(MatchingFor("R4QE01"), "NL/gl/glPlat.cpp"),
            Object(MatchingFor("R4QE01"), "NL/gl/glRenderList.cpp"),
            Object(MatchingFor("R4QE01"), "NL/gl/glStat.cpp"),
            Object(MatchingFor("R4QE01"), "NL/gl/glState.cpp"),
            Object(MatchingFor("R4QE01"), "NL/gl/glStruct.cpp"),
            Object(MatchingFor("R4QE01"), "NL/gl/glTarget.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/gl/glTexture.cpp"),
            Object(MatchingFor("R4QE01"), "NL/gl/glTextureManager.cpp"),
            Object(MatchingFor("R4QE01"), "NL/gl/glView.cpp", extra_cflags=["-ipa file", "-sym on"]),
            Object(MatchingFor("R4QE01"), "NL/gl/glShadowedTexturedColourModelWriter.cpp"),

            # NL/glx
            Object(MatchingFor("R4QE01"), "NL/glx/glxCharacterDamage.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/glxDisplayList.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/glxFog.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/glxFont.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/glxGX.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/glxLight.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/glxLoadModel.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/glxMatrix.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/glx/glxModel.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/glxSend.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/glxSkinMatrix.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/glxSwap.cpp", extra_cflags=["-inline noauto"]),
            Object(MatchingFor("R4QE01"), "NL/glx/glxTarget.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/glxTexture.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/glx/GXBlackTextureAlphaMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXBlackTextureAlphaMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXCameraScrolledOverlayMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXCameraScrolledOverlayMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXCharacterDamageMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXCharacterDamageMaterialProgramRender.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/glx/GXCharacterSkinCustomMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXCharacterSkinCustomMaterialProgramRender.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/glx/GXColourFresnelMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXColourFresnelMaterialProgramRender.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/glx/GXCompactColourMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXCompactColourMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXConstantColourMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXConstantColourMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXCrystalMaterialProgram.cpp", extra_cflags=["-sym on"]),
            Object(MatchingFor("R4QE01"), "NL/glx/GXCrystalMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXDetailModulateMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXDetailModulateMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXFixedLightMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXFixedLightMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXFloatTexturedColourMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXFloatTexturedColourMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXFourTextureAddMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXFourTextureAddMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXMaskedDetailBlendMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXMaskedDetailBlendMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXMaskedDiffuseBlendMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXMaskedDiffuseBlendMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXMaskedSpecularFresnelMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXMaskedSpecularFresnelMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXMaterialProgramRegistry.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXMegaDiffuseMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXMegaDiffuseMaterialProgramRender.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/glx/GXMegaSpecularFresnelMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXMegaSpecularFresnelMaterialProgramRender.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/glx/GXMegaSpecularMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXMegaSpecularMaterialProgramRender.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/glx/GXMovieMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXMovieMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXRedColourMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXRedColourMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXScissoredVertexColourTextureMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXScissoredVertexColourTextureMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXScrollingCameraOverlayMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXScrollingCameraOverlayMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXScrollingDiffuseMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXScrollingDiffuseMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXScrollingMaskedDetailBlendMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXScrollingMaskedDetailBlendMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXScrollingShadowedDetailBlendMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXScrollingShadowedDetailBlendMaterialProgramRender.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/glx/GXScrollingSpecularMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXScrollingSpecularMaterialProgramRender.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/glx/GXShadowedDetailBlendMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXShadowedDetailBlendMaterialProgramRender.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/glx/GXShadowedDiffuseMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXShadowedDiffuseMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXShadowVolumeMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXShadowVolumeMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXSkinnedMultiLightMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXSkinnedMultiLightMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXSkinnedUnlitTextureMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXSkinnedUnlitTextureMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXSpecularDetailBlendMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXSpecularDetailBlendMaterialProgramRender.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/glx/GXSpecularFresnelMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXSpecularFresnelMaterialProgramRender.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/glx/GXSpecularLookupMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXSpecularLookupMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXSpecularMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXSpecularMaterialProgramRender.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/glx/GXTextureBlendMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXTextureBlendMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXTextureColourAddMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXTextureColourAddMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXThreeLightDiffuseMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXThreeLightDiffuseMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXUnlitTextureMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXUnlitTextureMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXVertexColourDetailBlendMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXVertexColourDetailBlendMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXVertexColourMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXVertexColourMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXVertexColourTextureMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXVertexColourTextureMaterialProgramRender.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXWarbleMaterialProgram.cpp"),
            Object(MatchingFor("R4QE01"), "NL/glx/GXWarbleMaterialProgramRender.cpp"),

            # NL/plat
            Object(MatchingFor("R4QE01"), "NL/plat/cGlobalPad.cpp", extra_cflags=["-sym on"]),
            Object(MatchingFor("R4QE01"), "NL/plat/cPlatPad.cpp", extra_cflags=["-sym on", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/plat/DPDData.cpp"),
            Object(MatchingFor("R4QE01"), "NL/plat/GameCubePad.cpp"),
            Object(MatchingFor("R4QE01"), "NL/plat/globalpad.cpp"),
            Object(MatchingFor("R4QE01"), "NL/plat/PadBackend.cpp"),
            Object(MatchingFor("R4QE01"), "NL/plat/platqmath.cpp"),
            Object(MatchingFor("R4QE01"), "NL/plat/platvmath.cpp"),
            Object(MatchingFor("R4QE01"), "NL/plat/ReliableSocket.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/plat/SwappablePad.cpp", extra_cflags=["-sym on", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/plat/TransportConnection.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/plat/TransportMessage.cpp"),
            Object(MatchingFor("R4QE01"), "NL/plat/TransportPacket.cpp"),
            Object(MatchingFor("R4QE01"), "NL/plat/WiiClassicPad.cpp", extra_cflags=["-sym on", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/plat/WiiFreestylePad.cpp", extra_cflags=["-sym on", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/plat/WiiPad.cpp"),
            Object(MatchingFor("R4QE01"), "NL/plat/WiiRemotePad.cpp", extra_cflags=["-sym on", "-ipa file"]),

            # zlib
            Object(MatchingFor("R4QE01"), "zlib/adler32.c", cflags=cflags_zlib, progress_category="sdk"),
            Object(MatchingFor("R4QE01"), "zlib/crc32.c", cflags=cflags_zlib, progress_category="sdk"),
            Object(MatchingFor("R4QE01"), "zlib/inffast.c", cflags=cflags_zlib, progress_category="sdk"),
            Object(MatchingFor("R4QE01"), "zlib/inflate.c", cflags=cflags_zlib, progress_category="sdk"),
            Object(MatchingFor("R4QE01"), "zlib/inftrees.c", cflags=cflags_zlib, progress_category="sdk"),
            Object(MatchingFor("R4QE01"), "zlib/zutil.c", cflags=cflags_zlib, progress_category="sdk"),
        ],
    },
    {
        "lib": "Open Dynamics Engine (ODE)",
        "mw_version": config.linker_version,
        "cflags": cflags_ode,
        "progress_category": "game",
        "objects": [
            # ode
            Object(MatchingFor("R4QE01"), "ode/body_debug.cpp", extra_cflags=["-pool off"]),
            Object(MatchingFor("R4QE01"), "ode/collision_kernel.cpp"),
            Object(MatchingFor("R4QE01"), "ode/collision_space.cpp"),
            Object(MatchingFor("R4QE01"), "ode/collision_std.cpp"),
            Object(MatchingFor("R4QE01"), "ode/collision_transform.cpp"),
            Object(MatchingFor("R4QE01"), "ode/collision_util.cpp"),
            Object(MatchingFor("R4QE01"), "ode/dCylinder.cpp", extra_cflags=["-ipa file"]),
            Object(MatchingFor("R4QE01"), "ode/error.cpp"),
            Object(MatchingFor("R4QE01"), "ode/joint.cpp"),
            Object(MatchingFor("R4QE01"), "ode/mass.cpp", extra_cflags=["-inline deferred"]),
            Object(MatchingFor("R4QE01"), "ode/matrix.cpp"),
            Object(MatchingFor("R4QE01"), "ode/memory.cpp", extra_cflags=["-inline deferred"]),
            Object(MatchingFor("R4QE01"), "ode/NLGAdditions.cpp"),
            Object(MatchingFor("R4QE01"), "ode/obstack.cpp", extra_cflags=["-inline deferred"]),
            Object(MatchingFor("R4QE01"), "ode/ode.cpp"),
            Object(MatchingFor("R4QE01"), "ode/odemath.cpp"),
            Object(MatchingFor("R4QE01"), "ode/quickstep.cpp"),
            Object(MatchingFor("R4QE01"), "ode/rotation.cpp", extra_cflags=["-inline deferred"]),
            Object(MatchingFor("R4QE01"), "ode/util.cpp"),

            # ode/ext
            Object(MatchingFor("R4QE01"), "ode/ext/dColumn.cpp"),
            Object(MatchingFor("R4QE01"), "ode/ext/dFinitePlane.cpp"),
            Object(MatchingFor("R4QE01"), "ode/ext/dRoundedCorner.cpp"),
        ],
    },
    {
        "lib": "MetroTRK",
        "mw_version": "GC/2.7",
        "cflags": cflags_metrotrk,
        "progress_category": "sdk",
        "objects": [
            Object(MatchingFor("R4QE01"), "MetroTRK/__exception.s"),
            Object(MatchingFor("R4QE01"), "MetroTRK/cc_gdev.c", extra_cflags=["-sdata 8"]),
            Object(MatchingFor("R4QE01"), "MetroTRK/CircleBuffer.c"),
            Object(MatchingFor("R4QE01"), "MetroTRK/dispatch.c"),
            Object(MatchingFor("R4QE01"), "MetroTRK/dolphin_trk.c", cflags=cflags_metrotrk_deferred_auto),
            Object(MatchingFor("R4QE01"), "MetroTRK/dolphin_trk_glue.c"),
            Object(MatchingFor("R4QE01"), "MetroTRK/flush_cache.c"),
            Object(MatchingFor("R4QE01"), "MetroTRK/main_TRK.c"),
            Object(MatchingFor("R4QE01"), "MetroTRK/mainloop.c", cflags=cflags_metrotrk_deferred_auto),
            Object(MatchingFor("R4QE01"), "MetroTRK/mem_TRK.c"),
            Object(MatchingFor("R4QE01"), "MetroTRK/mpc_7xx_603e.c"),
            Object(MatchingFor("R4QE01"), "MetroTRK/msg.c"),
            Object(MatchingFor("R4QE01"), "MetroTRK/msgbuf.c", cflags=cflags_metrotrk_deferred_auto),
            Object(MatchingFor("R4QE01"), "MetroTRK/msghndlr.c", cflags=cflags_metrotrk_deferred_auto),
            Object(MatchingFor("R4QE01"), "MetroTRK/mslsupp.c"),
            Object(MatchingFor("R4QE01"), "MetroTRK/mutex_TRK.c"),
            Object(MatchingFor("R4QE01"), "MetroTRK/MWCriticalSection_gc.c"),
            Object(MatchingFor("R4QE01"), "MetroTRK/notify.c", cflags=cflags_metrotrk_deferred_auto),
            Object(MatchingFor("R4QE01"), "MetroTRK/nubevent.c"),
            Object(MatchingFor("R4QE01"), "MetroTRK/nubinit.c", cflags=cflags_metrotrk_deferred_auto),
            Object(MatchingFor("R4QE01"), "MetroTRK/serpoll.c", cflags=cflags_metrotrk_deferred_auto, extra_cflags=["-sdata 8"]),
            Object(MatchingFor("R4QE01"), "MetroTRK/string_TRK.c"),
            Object(MatchingFor("R4QE01"), "MetroTRK/support.c", cflags=cflags_metrotrk_deferred_auto),
            Object(MatchingFor("R4QE01"), "MetroTRK/targcont.c"),
            Object(MatchingFor("R4QE01"), "MetroTRK/target_options.c"),
            Object(MatchingFor("R4QE01"), "MetroTRK/targimpl.c", cflags=cflags_metrotrk_deferred_auto),
            Object(MatchingFor("R4QE01"), "MetroTRK/targsupp.c"),
            Object(MatchingFor("R4QE01"), "MetroTRK/UDP_Stubs.c"),
            Object(MatchingFor("R4QE01"), "MetroTRK/usr_put.c"),
        ],
    },
    {
        "lib": "MSL_C",
        "mw_version": config.linker_version,
        "cflags": cflags_rvl_sdk,
        "progress_category": "sdk",
        "objects": [
            Object(MatchingFor("R4QE01"), "MSL/abort_exit_ppc_eabi.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(MatchingFor("R4QE01"), "MSL/alloc.c", extra_cflags=["-Cpp_exceptions on", "-use_lmw_stmw on"]),
            Object(MatchingFor("R4QE01"), "MSL/ansi_files.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(MatchingFor("R4QE01"), "MSL/ansi_fp.c", extra_cflags=["-Cpp_exceptions on", "-rostr", "-use_lmw_stmw on"]),
            Object(MatchingFor("R4QE01"), "MSL/arith.c"),
            Object(MatchingFor("R4QE01"), "MSL/buffer_io.c", extra_cflags=["-Cpp_exceptions on", "-use_lmw_stmw on"]),
            Object(MatchingFor("R4QE01"), "MSL/char_io.c", extra_cflags=["-Cpp_exceptions on", "-use_lmw_stmw on"]),
            Object(MatchingFor("R4QE01"), "MSL/ctype.c"),
            Object(MatchingFor("R4QE01"), "MSL/direct_io.c", extra_cflags=["-Cpp_exceptions on", "-use_lmw_stmw on"]),
            Object(MatchingFor("R4QE01"), "MSL/e_acos.c"),
            Object(MatchingFor("R4QE01"), "MSL/e_atan2.c"),
            Object(MatchingFor("R4QE01"), "MSL/e_pow.c"),
            Object(MatchingFor("R4QE01"), "MSL/e_rem_pio2.c"),
            Object(MatchingFor("R4QE01"), "MSL/e_sqrt.c"),
            Object(MatchingFor("R4QE01"), "MSL/errno.c"),
            Object(MatchingFor("R4QE01"), "MSL/extras.c"),
            Object(MatchingFor("R4QE01"), "MSL/file_io.c", extra_cflags=["-Cpp_exceptions on", "-use_lmw_stmw on"]),
            Object(MatchingFor("R4QE01"), "MSL/FILE_POS.c", extra_cflags=["-Cpp_exceptions on", "-use_lmw_stmw on"]),
            Object(MatchingFor("R4QE01"), "MSL/float.c"),
            Object(MatchingFor("R4QE01"), "MSL/k_cos.c"),
            Object(MatchingFor("R4QE01"), "MSL/k_rem_pio2.c", extra_cflags=["-use_lmw_stmw on"]),
            Object(MatchingFor("R4QE01"), "MSL/k_sin.c"),
            Object(MatchingFor("R4QE01"), "MSL/k_tan.c"),
            Object(MatchingFor("R4QE01"), "MSL/locale.c", extra_cflags=["-rostr"]),
            Object(MatchingFor("R4QE01"), "MSL/math_api.c"),
            Object(MatchingFor("R4QE01"), "MSL/math_ppc.c"),
            Object(MatchingFor("R4QE01"), "MSL/math_sun.c", cflags=cflags_runtime, extra_cflags=["-Cpp_exceptions on"]),
            Object(MatchingFor("R4QE01"), "MSL/mbstring.c", extra_cflags=["-use_lmw_stmw on"]),
            Object(MatchingFor("R4QE01"), "MSL/mem.c"),
            Object(MatchingFor("R4QE01"), "MSL/mem_funcs.c"),
            Object(MatchingFor("R4QE01"), "MSL/misc_io.c"),
            Object(MatchingFor("R4QE01"), "MSL/printf.c", extra_cflags=["-Cpp_exceptions on", "-str reuse,pool,readonly", "-use_lmw_stmw on"], mw_version="GC/3.0a5.2"),
            Object(MatchingFor("R4QE01"), "MSL/qsort.c", extra_cflags=["-Cpp_exceptions on", "-use_lmw_stmw on"]),
            Object(MatchingFor("R4QE01"), "MSL/rand.c"),
            Object(MatchingFor("R4QE01"), "MSL/s_atan.c"),
            Object(MatchingFor("R4QE01"), "MSL/s_ceil.c"),
            Object(MatchingFor("R4QE01"), "MSL/s_copysign.c"),
            Object(MatchingFor("R4QE01"), "MSL/s_cos.c"),
            Object(MatchingFor("R4QE01"), "MSL/s_floor.c"),
            Object(MatchingFor("R4QE01"), "MSL/s_frexp.c"),
            Object(MatchingFor("R4QE01"), "MSL/s_ldexp.c"),
            Object(MatchingFor("R4QE01"), "MSL/s_sin.c"),
            Object(MatchingFor("R4QE01"), "MSL/s_tan.c"),
            Object(MatchingFor("R4QE01"), "MSL/scanf.c", extra_cflags=["-Cpp_exceptions on", "-str reuse,pool,readonly", "-use_lmw_stmw on"]),
            Object(MatchingFor("R4QE01"), "MSL/string.c", extra_cflags=["-Cpp_exceptions on", "-str reuse,pool,readonly"]),
            Object(MatchingFor("R4QE01"), "MSL/strtold.c", extra_cflags=["-Cpp_exceptions on", "-str reuse,pool,readonly", "-use_lmw_stmw on"]),
            Object(MatchingFor("R4QE01"), "MSL/strtoul.c", extra_cflags=["-Cpp_exceptions on", "-use_lmw_stmw on"]),
            Object(MatchingFor("R4QE01"), "MSL/uart_console_io_gcn.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(MatchingFor("R4QE01"), "MSL/w_acos.c", extra_cflags=["-D_IEEE_LIBM"]),
            Object(MatchingFor("R4QE01"), "MSL/w_atan2.c", extra_cflags=["-D_IEEE_LIBM"]),
            Object(MatchingFor("R4QE01"), "MSL/w_pow.c", extra_cflags=["-D_IEEE_LIBM"]),
            Object(MatchingFor("R4QE01"), "MSL/w_sqrt.c", extra_cflags=["-D_IEEE_LIBM"]),
            Object(MatchingFor("R4QE01"), "MSL/wchar_io.c"),
            Object(MatchingFor("R4QE01"), "MSL/wctype.c"),
            Object(MatchingFor("R4QE01"), "MSL/wmem.c"),
            Object(MatchingFor("R4QE01"), "MSL/wprintf.c", extra_cflags=["-Cpp_exceptions on", "-str reuse,pool,readonly", "-use_lmw_stmw on"], mw_version="GC/3.0a5.2"),
            Object(MatchingFor("R4QE01"), "MSL/wstring.c"),
        ],
    },
    {
        "lib": "Runtime.PPCEABI.H",
        "mw_version": config.linker_version,
        "cflags": cflags_runtime,
        "progress_category": "sdk",  # str | List[str]
        "objects": [
            # GC/3.0a3, GC/3.0a5, and GC/3.0a5.2 all reproduce these units.
            Object(MatchingFor("R4QE01"), "Runtime/__init_cpp_exceptions.cpp"),
            Object(MatchingFor("R4QE01"), "Runtime/__mem.c"),
            Object(MatchingFor("R4QE01"), "Runtime/__va_arg.c"),
            Object(MatchingFor("R4QE01"), "Runtime/GCN_mem_alloc.c"),
            Object(MatchingFor("R4QE01"), "Runtime/Gecko_ExceptionPPC.cpp"),
            Object(MatchingFor("R4QE01"), "Runtime/global_destructor_chain.c"),
            Object(MatchingFor("R4QE01"), "Runtime/NMWException.cpp", extra_cflags=["-Cpp_exceptions on"]),
            Object(MatchingFor("R4QE01"), "Runtime/ptmf.c"),
            Object(MatchingFor("R4QE01"), "Runtime/runtime.c"),
        ],
    },
    {
        "lib": "RVL_SDK",
        # Every RVL_SDK and DWC banner in the retail DOL carries the compiler stamp
        # 0x4199_60831, which is the GC/3.0a5.2 package (mwcceppc -version: 4.1 build
        # 60831); the game itself needs GC/3.0a5 (4.2 build 60422). Nintendo built the
        # SDK libraries with 3.0a5.2, so that is the default here and NLG code inside
        # this block pins the game compiler explicitly.
        # Libraries R4QE01 links from other suppliers or packages (the Broadcom
        # Bluetooth stack, NHTTP and RevoEX) keep their own blocks below.
        "mw_version": "GC/3.0a5.2",
        "cflags": cflags_rvl_sdk,
        "progress_category": "sdk",
        "objects": [
            # NL/glx
            Object(MatchingFor("R4QE01"), "NL/glx/glxMemory.cpp", cflags=cflags_game, mw_version="GC/3.0a5"),

            # NL/plat
            Object(MatchingFor("R4QE01"), "NL/plat/nlFileCache.cpp", cflags=cflags_game, extra_cflags=["-ipa file", "-sym on"], mw_version="GC/3.0a5"),
            Object(MatchingFor("R4QE01"), "NL/plat/nlFlash.cpp", cflags=cflags_game, mw_version="GC/3.0a5"),
            Object(MatchingFor("R4QE01"), "NL/plat/nlMemory.cpp", cflags=cflags_game, mw_version="GC/3.0a5"),
            Object(MatchingFor("R4QE01"), "NL/plat/PlatPadManager.cpp", cflags=cflags_game, mw_version="GC/3.0a5", extra_cflags=["-inline noauto", "-ipa file"]),
            Object(MatchingFor("R4QE01"), "NL/plat/SocketNetwork.cpp", cflags=cflags_game, mw_version="GC/3.0a5"),
            Object(MatchingFor("R4QE01"), "NL/plat/TransportSocket.cpp", cflags=cflags_game, mw_version="GC/3.0a5"),
            Object(MatchingFor("R4QE01"), "NL/plat/WiiPadMonkey.cpp", cflags=cflags_game, mw_version="GC/3.0a5"),

            # RVL_SDK/ai
            Object(MatchingFor("R4QE01"), "RVL_SDK/ai/ai.c"),

            # RVL_SDK/arc
            Object(MatchingFor("R4QE01"), "RVL_SDK/arc/arc.c"),

            # RVL_SDK/ax
            Object(MatchingFor("R4QE01"), "RVL_SDK/ax/AX.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/ax/AXAlloc.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/ax/AXAux.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/ax/AXCL.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/ax/AXComp.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/ax/AXOut.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/ax/AXProf.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/ax/AXSPB.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/ax/AXVPB.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/ax/DSPCode.c"),

            # RVL_SDK/axfx
            Object(MatchingFor("R4QE01"), "RVL_SDK/axfx/AXFXDelay.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/axfx/AXFXDelayExpDpl2.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/axfx/AXFXHooks.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/axfx/AXFXReverbHi.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/axfx/AXFXReverbHiDpl2.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/axfx/AXFXReverbHiExp.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/axfx/AXFXReverbHiExpDpl2.c"),

            # RVL_SDK/base
            Object(MatchingFor("R4QE01"), "RVL_SDK/base/PPCArch.c"),

            # RVL_SDK/db
            Object(MatchingFor("R4QE01"), "RVL_SDK/db/db.c"),

            # RVL_SDK/dsp
            Object(MatchingFor("R4QE01"), "RVL_SDK/dsp/dsp.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dsp/dsp_debug.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dsp/dsp_task.c"),

            # RVL_SDK/dvd
            Object(MatchingFor("R4QE01"), "RVL_SDK/dvd/dvd.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dvd/dvd_broadway.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dvd/dvderror.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dvd/dvdFatal.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dvd/dvdfs.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dvd/dvdidutils.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dvd/dvdqueue.c"),

            # RVL_SDK/dwc
            Object(MatchingFor("R4QE01"), "RVL_SDK/dwc/dwc_account.c", cflags=cflags_rvl_dwc),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dwc/dwc_auth_interface.c", cflags=cflags_rvl_dwc),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dwc/dwc_base64.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dwc/dwc_common.c", cflags=cflags_rvl_dwc),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dwc/dwc_encsession.c", cflags=cflags_rvl_dwc),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dwc/dwc_error.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dwc/dwc_friend.c", cflags=cflags_rvl_dwc),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dwc/dwc_ghttp.c", cflags=cflags_rvl_dwc),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dwc/dwc_init.c", cflags=cflags_rvl_dwc),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dwc/dwc_login.c", cflags=cflags_rvl_dwc),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dwc/dwc_main.c", cflags=cflags_rvl_dwc),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dwc/dwc_match.c", cflags=cflags_rvl_dwc),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dwc/dwc_memfunc.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dwc/dwc_nastime.c", cflags=cflags_rvl_dwc),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dwc/dwc_nonport.c", cflags=cflags_rvl_dwc),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dwc/dwc_ranking.c", cflags=cflags_rvl_dwc),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dwc/dwc_ranksession.c", cflags=cflags_rvl_dwc),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dwc/dwc_report.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dwc/dwc_transport.c", cflags=cflags_rvl_dwc),
            Object(MatchingFor("R4QE01"), "RVL_SDK/dwc/dwci_np_math.c", cflags=cflags_rvl_dwc),

            # RVL_SDK/euart
            Object(MatchingFor("R4QE01"), "RVL_SDK/euart/euart.c"),

            # RVL_SDK/exi
            Object(MatchingFor("R4QE01"), "RVL_SDK/exi/EXIBios.c", extra_cflags=["-schedule off"]),
            Object(MatchingFor("R4QE01"), "RVL_SDK/exi/EXICommon.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/exi/EXIUart.c"),

            # RVL_SDK/fs
            Object(MatchingFor("R4QE01"), "RVL_SDK/fs/fs.c"),

            # RVL_SDK/gamespy
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/darray.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/hashtable.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/md5c.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/nonport.c", cflags=cflags_rvl_spy, extra_cflags=["-D_REVOLUTION"]),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/common/gsAvailable.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/common/gsCrypt.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/common/gsLargeInt.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/common/gsRC4.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/common/gsSHA1.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/common/gsSSL.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/common/gsXML.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/ghttp/ghttpBuffer.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/ghttp/ghttpCallbacks.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/ghttp/ghttpCommon.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/ghttp/ghttpConnection.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/ghttp/ghttpEncryption.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/ghttp/ghttpMain.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/ghttp/ghttpPost.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/ghttp/ghttpProcess.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/GP/gp.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/GP/gpi.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/GP/gpiBuddy.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/GP/gpiBuffer.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/GP/gpiCallback.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/GP/gpiConnect.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/GP/gpiInfo.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/GP/gpiOperation.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/GP/gpiPeer.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/GP/gpiProfile.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/GP/gpiSearch.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/GP/gpiTransfer.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/GP/gpiUnique.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/GP/gpiUtility.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/gstats/gbucket.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/gstats/gstats.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/gt2/gt2Auth.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/gt2/gt2Buffer.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/gt2/gt2Callback.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/gt2/gt2Connection.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/gt2/gt2Main.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/gt2/gt2Message.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/gt2/gt2Socket.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/gt2/gt2Utility.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/natneg/NATify.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/natneg/natneg.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/qr2/qr2.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/qr2/qr2regkeys.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/serverbrowsing/sb_crypt.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/serverbrowsing/sb_queryengine.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/serverbrowsing/sb_server.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/serverbrowsing/sb_serverbrowsing.c", cflags=cflags_rvl_spy),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gamespy/serverbrowsing/sb_serverlist.c", cflags=cflags_rvl_spy),

            # RVL_SDK/gx
            Object(MatchingFor("R4QE01"), "RVL_SDK/gx/GXAttr.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gx/GXBump.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gx/GXDisplayList.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gx/GXFifo.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gx/GXFrameBuf.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gx/GXGeometry.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gx/GXInit.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gx/GXLight.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gx/GXMisc.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gx/GXPerf.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gx/GXPixel.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gx/GXTev.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gx/GXTexture.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/gx/GXTransform.c"),

            # RVL_SDK/hbm
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/HBMAnmController.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/HBMBase.cpp", cflags=[*cflags_rvl_hbm, "-DHBM_REVISION=2"]),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/HBMController.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/HBMFrameController.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/HBMGUIManager.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/HBMRemoteSpk.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/db/db_assert.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/db/db_console.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/db/db_DbgPrintBase.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/db/db_directPrint.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/db/db_mapFile.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/lyt/lyt_animation.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/lyt/lyt_arcResourceAccessor.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/lyt/lyt_bounding.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/lyt/lyt_common.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/lyt/lyt_drawInfo.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/lyt/lyt_group.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/lyt/lyt_layout.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/lyt/lyt_material.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/lyt/lyt_pane.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/lyt/lyt_picture.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/lyt/lyt_resourceAccessor.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/lyt/lyt_textBox.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/lyt/lyt_window.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/math/math_triangular.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_AnimSound.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_AxManager.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_AxVoice.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_Bank.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_BankFile.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_BasicSound.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_Channel.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_DisposeCallbackManager.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_DvdSoundArchive.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_EnvGenerator.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_ExternalSoundPlayer.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_FrameHeap.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_InstancePool.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_Lfo.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_MemorySoundArchive.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_MidiSeqPlayer.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_MidiSeqTrack.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_MmlParser.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_MmlSeqTrack.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_MmlSeqTrackAllocator.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_NandSoundArchive.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_PlayerHeap.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_RemoteSpeaker.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_RemoteSpeakerManager.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_SeqFile.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_SeqPlayer.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_SeqSound.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_SeqSoundHandle.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_SeqTrack.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_SoundArchive.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_SoundArchiveFile.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_SoundArchiveLoader.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_SoundArchivePlayer.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_SoundHandle.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_SoundHeap.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_SoundPlayer.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_SoundStartable.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_SoundSystem.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_SoundThread.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_StrmChannel.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_StrmFile.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_StrmPlayer.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_StrmSound.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_StrmSoundHandle.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_TaskManager.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_TaskThread.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_Util.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_WaveFile.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_WavePlayer.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_WaveSound.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_WaveSoundHandle.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_WsdFile.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_WsdPlayer.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/snd/snd_WsdTrack.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/ut/ut_binaryFileFormat.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/ut/ut_CharStrmReader.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/ut/ut_CharWriter.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/ut/ut_DvdFileStream.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/ut/ut_FileStream.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/ut/ut_Font.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/ut/ut_IOStream.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/ut/ut_LinkList.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/ut/ut_list.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/ut/ut_ResFont.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/ut/ut_ResFontBase.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/ut/ut_TagProcessorBase.cpp", cflags=cflags_rvl_hbm),
            Object(MatchingFor("R4QE01"), "RVL_SDK/hbm/nw4hbm/ut/ut_TextWriterBase.cpp", cflags=cflags_rvl_hbm),

            # RVL_SDK/ipc
            Object(MatchingFor("R4QE01"), "RVL_SDK/ipc/ipcclt.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/ipc/ipcMain.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/ipc/ipcProfile.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/ipc/memory.c"),

            # RVL_SDK/kpad
            Object(MatchingFor("R4QE01"), "RVL_SDK/kpad/KPAD.c"),

            # RVL_SDK/mem
            Object(MatchingFor("R4QE01"), "RVL_SDK/mem/mem_allocator.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/mem/mem_expHeap.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/mem/mem_frameHeap.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/mem/mem_heapCommon.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/mem/mem_list.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/mem/mem_unitHeap.c"),

            # RVL_SDK/mix
            Object(MatchingFor("R4QE01"), "RVL_SDK/mix/mix.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/mix/remote.c"),

            # RVL_SDK/mtx
            Object(MatchingFor("R4QE01"), "RVL_SDK/mtx/mtx.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/mtx/mtx44.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/mtx/mtxvec.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/mtx/quat.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/mtx/vec.c"),

            # RVL_SDK/nand
            Object(MatchingFor("R4QE01"), "RVL_SDK/nand/nand.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nand/NANDCheck.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nand/NANDCore.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nand/NANDOpenClose.c"),

            # RVL_SDK/ncd
            Object(MatchingFor("R4QE01"), "RVL_SDK/ncd/ncdsystem.c"),

            # RVL_SDK/ndev
            Object(MatchingFor("R4QE01"), "RVL_SDK/ndev/DebuggerDriver.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/ndev/exi2.c"),

            # RVL_SDK/nwc24
            Object(MatchingFor("R4QE01"), "RVL_SDK/nwc24/NWC24Config.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nwc24/NWC24Download.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nwc24/NWC24FileApi.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nwc24/NWC24FriendList.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nwc24/NWC24Ipc.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nwc24/NWC24Manage.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nwc24/NWC24MBoxCtrl.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nwc24/NWC24Mime.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nwc24/NWC24Schedule.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nwc24/NWC24SecretFList.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nwc24/NWC24StdApi.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nwc24/NWC24System.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nwc24/NWC24Time.c"),

            # RVL_SDK/os
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/__ppc_eabi_init.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/__start.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OS.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSAlarm.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSAlloc.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSArena.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSAudioSystem.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSCache.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSContext.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSError.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSExec.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSFatal.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSFont.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSInterrupt.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSIpc.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSLink.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSMemory.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSMessage.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSMutex.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSNandbootInfo.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSNet.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSPlayRecord.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSReboot.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSReset.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSRtc.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSStateFlags.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSStateTM.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSSync.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSThread.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSTime.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/os/OSUtf.c"),

            # RVL_SDK/pad
            Object(MatchingFor("R4QE01"), "RVL_SDK/pad/Pad.c", extra_cflags=["-inline noauto"]),

            # RVL_SDK/sc
            Object(MatchingFor("R4QE01"), "RVL_SDK/sc/scapi.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/sc/scapi_prdinfo.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/sc/scsystem.c"),

            # RVL_SDK/si
            Object(MatchingFor("R4QE01"), "RVL_SDK/si/SIBios.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/si/SISamplingRate.c"),

            # RVL_SDK/so
            Object(MatchingFor("R4QE01"), "RVL_SDK/so/soBasic.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/so/soCommon.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/so/SOInformation.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/so/SOOption.c"),

            # RVL_SDK/sp
            Object(MatchingFor("R4QE01"), "RVL_SDK/sp/sp.c"),

            # RVL_SDK/ssl
            Object(MatchingFor("R4QE01"), "RVL_SDK/ssl/ssl_api.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/ssl/ssl_mutex.c"),

            # RVL_SDK/thp
            Object(MatchingFor("R4QE01"), "RVL_SDK/thp/THPAudio.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/thp/THPDec.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/thp/THPSimple.cpp", cflags=cflags_game),

            # RVL_SDK/tpl
            Object(MatchingFor("R4QE01"), "RVL_SDK/tpl/TPL.c"),

            # RVL_SDK/usb
            Object(MatchingFor("R4QE01"), "RVL_SDK/usb/usb.c"),

            # RVL_SDK/vi
            Object(MatchingFor("R4QE01"), "RVL_SDK/vi/i2c.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vi/vi.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vi/vi3in1.c"),

            # RVL_SDK/wenc
            Object(MatchingFor("R4QE01"), "RVL_SDK/wenc/wenc.c"),

            # RVL_SDK/wpad
            Object(MatchingFor("R4QE01"), "RVL_SDK/wpad/debug_msg.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/wpad/WPAD.c", cflags=cflags_rvl_wpad),
            Object(MatchingFor("R4QE01"), "RVL_SDK/wpad/WPADEncrypt.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/wpad/WPADHIDParser.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/wpad/WPADMem.c"),

            # RVL_SDK/wud
            Object(MatchingFor("R4QE01"), "RVL_SDK/wud/debug_msg.c", cflags=cflags_rvl_wud),
            Object(MatchingFor("R4QE01"), "RVL_SDK/wud/WUD.c", cflags=cflags_rvl_wud),
            Object(MatchingFor("R4QE01"), "RVL_SDK/wud/WUDHidHost.c", cflags=cflags_rvl_wud),
        ],
    },
    {
        # RVLFaceLib ships as its own package and registers no banner. Its one
        # compiler signature is data: RFLiInitShapeRes keeps a static const
        # header table that only debug assertions read, GC/3.0a5 drops the
        # unreferenced table as retail does, and GC/3.0a5.2 emits it into
        # .rodata, moving every later section of the DOL by 0x20.
        "lib": "RVLFaceLib",
        "mw_version": "GC/3.0a5",
        "cflags": cflags_rvl_sdk,
        "progress_category": "sdk",
        "objects": [
            # RVL_SDK/rfl
            Object(MatchingFor("R4QE01"), "RVL_SDK/rfl/RFL_Controller.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(MatchingFor("R4QE01"), "RVL_SDK/rfl/RFL_Database.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(MatchingFor("R4QE01"), "RVL_SDK/rfl/RFL_DataUtility.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(MatchingFor("R4QE01"), "RVL_SDK/rfl/RFL_DefaultDatabase.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(MatchingFor("R4QE01"), "RVL_SDK/rfl/RFL_Format.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(MatchingFor("R4QE01"), "RVL_SDK/rfl/RFL_HiddenDatabase.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(MatchingFor("R4QE01"), "RVL_SDK/rfl/RFL_Icon.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(MatchingFor("R4QE01"), "RVL_SDK/rfl/RFL_MakeRandomFace.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(MatchingFor("R4QE01"), "RVL_SDK/rfl/RFL_MakeTex.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(MatchingFor("R4QE01"), "RVL_SDK/rfl/RFL_MiddleDatabase.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(MatchingFor("R4QE01"), "RVL_SDK/rfl/RFL_Model.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(MatchingFor("R4QE01"), "RVL_SDK/rfl/RFL_NANDAccess.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(MatchingFor("R4QE01"), "RVL_SDK/rfl/RFL_NANDLoader.c", extra_cflags=["-Cpp_exceptions on"]),
            Object(MatchingFor("R4QE01"), "RVL_SDK/rfl/RFL_System.c", extra_cflags=["-Cpp_exceptions on"]),
        ],
    },
    {
        # Broadcom Bluetooth stack (the bluedroid-derived files). It carries no
        # banner, and its retail code keeps the older compiler's constant
        # placement: the pristine Broadcom spelling of rfc_send_test, sdp_init or
        # WBT_ExtCreateRecord is exact under GC/3.0a5 and not under GC/3.0a5.2.
        # Nintendo's USB transport glue for it, uusb_ppc.c, is the reverse and
        # belongs to the 3.0a5.2 SDK build.
        "lib": "BTE",
        "mw_version": "GC/3.0a5",
        "cflags": cflags_rvl_bte,
        "progress_category": "sdk",
        "objects": [
            # RVL_SDK/bte
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/bd.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/bta_dm_act.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/bta_dm_api.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/bta_dm_cfg.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/bta_dm_main.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/bta_dm_pm.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/bta_hh_act.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/bta_hh_api.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/bta_hh_cfg.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/bta_hh_main.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/bta_hh_utils.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/bta_sys_cfg.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/bta_sys_conn.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/bta_sys_main.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/bte_hcisu.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/bte_init.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/bte_logmsg.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/bte_main.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/btm_acl.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/btm_dev.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/btm_devctl.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/btm_discovery.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/btm_inq.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/btm_main.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/btm_pm.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/btm_sco.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/btm_sec.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/btu_hcif.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/btu_init.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/btu_task1.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/gap_api.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/gap_conn.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/gap_utils.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/gki_buffer.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/gki_ppc.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/gki_time.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/hcicmds.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/hcisu_h2.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/hidd_api.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/hidd_conn.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/hidd_mgmt.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/hidd_pm.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/hidh_api.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/hidh_conn.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/l2c_api.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/l2c_csm.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/l2c_link.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/l2c_main.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/l2c_utils.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/port_api.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/port_rfc.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/port_utils.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/ptim.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/rfc_l2cap_if.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/rfc_mx_fsm.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/rfc_port_fsm.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/rfc_port_if.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/rfc_ts_frames.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/rfc_utils.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/sdp_api.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/sdp_db.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/sdp_discovery.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/sdp_main.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/sdp_server.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/sdp_utils.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/utl.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/uusb_ppc.c", mw_version="GC/3.0a5.2"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/bte/wbt_ext.c"),
        ],
    },
    {
        # NHTTP ships as its own library package without a banner. Its retail
        # code keeps the older compiler's constant order and saved-register
        # zeros (NHTTPi_strnicmp, NHTTPi_compareToken, NHTTPi_ThreadParseHeaderProc),
        # exact under GC/3.0a5 only.
        "lib": "NHTTP",
        "mw_version": "GC/3.0a5",
        "cflags": cflags_rvl_sdk,
        "progress_category": "sdk",
        "objects": [
            # RVL_SDK/nhttp
            Object(MatchingFor("R4QE01"), "RVL_SDK/nhttp/d_nhttp.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nhttp/d_nhttp_common.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nhttp/d_nhttp_private.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nhttp/NHTTP_bgnend.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nhttp/NHTTP_control.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nhttp/NHTTP_list.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nhttp/NHTTP_os_RVL.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nhttp/NHTTP_recvbuf.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nhttp/NHTTP_request.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nhttp/NHTTP_response.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nhttp/NHTTP_socket_RVL.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nhttp/NHTTP_stdlib_RVL.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/nhttp/NHTTP_thread.c", cflags=cflags_rvl_nhttp),
        ],
    },
    {
        # RevoEX 1.0: NETVersion.c registers the "<< REX-PPC 1.0.0.0 (RevoEX-1.0)
        # REL 070309140556 >>" banner, which carries no compiler stamp. The VF
        # file system (RevoEX in the wii-ipl tree) shows both compilers: the
        # constant placement of VFiPFENT_ITER_DoGetEntry and VFiPFCODE_CP932_* is
        # exact under GC/3.0a5 only and inert to every donor spelling, while
        # VFiPFPATH_DoSplitPath copies its 16-byte token through GPR words, which
        # only GC/3.0a5.2 emits (3.0a5 uses FPR pairs). pf_path.c is pinned; the
        # objects were evidently not all produced by one compiler build.
        "lib": "RevoEX",
        "mw_version": "GC/3.0a5",
        "cflags": cflags_rvl_sdk,
        "progress_category": "sdk",
        "objects": [
            # RVL_SDK/net
            Object(MatchingFor("R4QE01"), "RVL_SDK/net/hmac.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/net/md5.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/net/neterrorcode.c", cflags=[*(flag for flag in cflags_rvl_sdk if flag != "-inline auto"), "-inline on"]),
            Object(MatchingFor("R4QE01"), "RVL_SDK/net/NETVersion.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/net/wireless_macaddr.c"),

            # RVL_SDK/vf
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/d_common.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/d_hash.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/d_time.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/d_vf.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/d_vf_sys.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/nand_drv.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pdm_bpb.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pdm_disk.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pdm_dskmng.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pdm_mbr.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pdm_partition.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_api_util.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_cache.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_clib.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_cluster.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_code.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_cp932.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_dir.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_driver.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_entry.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_entry_iterator.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_errnum.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_fat.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_fat12.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_fat16.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_fat32.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_fatfs.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_fclose.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_file.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_filelock.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_finfo.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_fopen.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_fread.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_fseek.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_fwrite.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_init_prfile2.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_path.c", mw_version="GC/3.0a5.2"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_sector.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_service.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_str.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_system.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_volume.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/pf_w_clib.c"),
            Object(MatchingFor("R4QE01"), "RVL_SDK/vf/sd_drv.c"),
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
