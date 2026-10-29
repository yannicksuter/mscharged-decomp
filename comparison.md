# Mario Strikers Charged: regional version comparison

This report compares the five retail builds that this project reproduces:
four regions, two of them European disc revisions. It uses the disc images
in `orig/`, the per-version `config/` symbol and split data, and the
`VERSION_*` conditionals in `src/` and `include/`. Columns follow build
order.

| Short | Game ID | Release |
| --- | --- | --- |
| EU1 | `R4QP01_rev1` | Europe, *Mario Strikers Charged Football*, disc revision 1 |
| EU2 | `R4QP01_rev2` | Europe, *Mario Strikers Charged Football*, disc revision 2 |
| US | `R4QE01` | USA, *Mario Strikers Charged*, disc revision 1 |
| JP | `R4QJ01` | Japan, *Mario Strikers Charged*, disc revision 0 |
| KR | `R4QK01` | Korea, *Mario Power Soccer*, disc revision 0 |

Legend: ✓ = present, — = absent. A value is stated where builds differ in
degree. *Interpretation* marks conclusions the data does not state directly.

## 1. Summary

| | EU1 | EU2 | US | JP | KR |
| --- | --- | --- | --- | --- | --- |
| Build stamp (Perforce change) | 41012, 2007-04-13 | 41012 (same stamp as EU1) | 48053, 2007-05-30 | 47759, 2007-05-28 | 151174, 2010-01-26 |
| Online version word | `0x1BE3A034` | `0x1BE3A034` | `0x1BA3BBB5` | `0x1BF3BA8F` | `0x1BD34E86` |
| SDK libraries (newest date) | Apr 2007 | Apr 2007 | May 2007 | May 2007 | 2009 (SDK 4.x) |
| IOS / System Menu on disc | IOS21 / 2.1E | IOS21 / 2.1E | IOS21 / 2.2U | IOS21 / 2.0J | IOS55 / 4.2K |
| Encryption common key | retail | retail | retail | retail | Korean |
| Age rating | PEGI 7, USK 6 | same | ESRB age 10 (E10+) | CERO A | GRB All |
| `main.dol` bytes / functions | 5,691,072 / 17,643 | 5,692,416 / 17,643 | 5,695,424 / 17,661 | 5,696,544 / 17,668 | 5,453,344 / 16,891 |
| Files on disc | 2,399 (46 unused leftovers) | 2,353 | 2,354 | 2,354 | 2,334 (+ developer files) |
| Languages | 9 + 2 test sets | same | same | same | English, Japanese, Korean |
| Title-screen logo | "Charged" letters + lightning | same | same | same | fire-trail and flame animation |
| ~20 gameplay bug fixes | — | partial | ✓ | ✓ | ✓ |
| Goalie AI retune | — | — | — | — | ✓ |
| Tutorial 3 / 4 | Mega Saves, then Mega Strike | same | same | Mega Strike first | Mega Strike first |
| Online login timeout | 90 s | 90 s | 180 s | 180 s | 180 s |

- **EU1** is the oldest build. It lacks the gameplay fixes, uses the in-game
  music bank in menus, and ships 46 unused files.
- **EU2** has EU1's SDK and stamp but takes about 25 fixes in their US form.
  Keeping stamp 41012 keeps EU1 and EU2 in the same online pool.
- **US** adds the gameplay fixes, a newer SDK and a reworked online login.
- **JP** is US code plus Japanese UI fixes; only 29 functions differ from US.
- **KR** is a 2010 rebuild on SDK 4.x, with Korean grammar, compliance
  changes, its own title-screen logo animation and a retuned goalie.

## 2. Notable differences by build

This section lists the changes a player or researcher would notice. Each
item says whether it comes from code or from disc data. Section 7 lists the
complete code differences.

### Korea (KR)

- **Title-screen logo (data).** The 2007 builds build the logo from
  separate "CHARGED" letters (`SMC_c`, `SMC_h`, `SMC_a`, `SMC_r`, `SMC_g`,
  `SMC_e`, `SMC_d`) with a lightning animation (`SMC_lightning`, `charged`,
  `lightning`). KR's `sms2_start.fen` replaces these with 19
  `logo_firetrails` components and `flame1`–`2`, `smallflame1`–`3`, `flare4`
  and `firetrails`. The front-end scene system has no particle or emitter
  component, so the fire is keyframed sprite animation in the scene file.
  `SHTitleScreen.cpp` has no KR change except the cheat sound below.
  *Interpretation*: the letter animation spells "Charged", which is not
  part of the Korean title *Mario Power Soccer*.
- **Static menu logo (data).** The main menu and captain select drop the
  letter-and-lightning animation and show a single `SMC_mainlogo_UK`. The KR
  main menu has no `copyright` component.
- **Korean grammar (code).** `GetKoreanParticle` (`NL/nlLocalization.cpp`)
  chooses the right grammatical particle (조사) from a word's final
  consonant, or from the last digit of a number. `FormatKorean` inserts
  these particles into goal titles, Striker Times stories, post-game text
  and Cup news. This "particle" code is about grammar, not graphics.
- **Korean text layout (code).** Fonts get separate in-game Korean fonts.
  The line height is read raw and added between rows (`nlFont.cpp`,
  `nlTextBox.cpp`), and popups size to the text height.
- **Boot sequence (code and data).**
  - Two Korean safety straps (`strapA_Korean`, `strapB_Korean`) alternate
    every 5 s and auto-dismiss at 20.5 s.
  - The per-language and 16:9 straps are gone.
  - The Next Level Games logo phase is skipped; `boot_loading.fen` has no
    `nlgameslogo`.
- **Goalie and ball (code).**
  - Goalie: loose-ball net range 13 → 15 and lob-save descent angle
    40° → 60° (`Goalie.cpp:88`, `:125`). Pickup distance 1.2 → 1.5, a new
    pursuit of deking or running shooters (range 8), and loose-ball
    intercept prediction (`GoalieActions.cpp`).
  - Ball: re-enables player and goalie collision when released or shot
    (`Ball.cpp`).
  - AI: `IsPointInCone` gains an edge-plane test (`AiUtil.cpp`).
  - No other build has these changes.
- **Compliance and system (code).**
  - The Online menu checks parental controls
    (`SCCheckPCMessageRestriction`) and shows an "online restricted" popup.
  - The save banner is written to `/tmp` and moved into place with
    `NANDMove`.
  - A save error resets to the Data Management screen.
  - Reset uses `OSReturnToMenu` and waits for the online login thread.
  - The newer SDK adds an unauthorized-device check at boot
    (`dvdDeviceError.c`, "Error #001") and play-time-limit support
    (`OSPlayTime.c`).
- **Video (code).** KR forces NTSC 480i and skips PAL and EURGB60
  detection (`glPlat.cpp`).
- **Online (code).**
  - DWC game code `R4QK`, network channel 13, Korean ranking region and a
    UTC+9 season clock.
  - The newer DWC matchmaking uses reservation callbacks.
  - Friend count includes every profile friend.
- **Smaller front-end changes (code and data).**
  - Disc-error screens are Korean only, and the "no disc" message is gone.
  - The credits copyright phase is removed.
  - Player 1's colour in online choose-sides changes from red to yellow.
  - The unlock-all cheat plays a different sound (`SHTitleScreen.cpp:306`).
  - `online_invite_players.fen` holds the invite layout twice (every
    component count doubles), and the code looks rows up under `group`.
  - The HOME Menu adds Korean and Chinese archives.
- **Developer leftovers (data).** The disc has six 640×448 QA screenshots
  (`shot000`–`005.tga`), `recentTweaks.txt`, `runUniqueID.txt` and a debug
  `ini/user.ini`.
  - The screenshots show a Korean NAND-error dialog and the Mii Select
    screen with Miis named in symbol characters.
  - `user.ini` holds commented-out debug options such as `skipfe`,
    `unlock_all` and `dosoak`.
  - No build reads any of these files.

### Japan (JP)

- **Tutorial order (code).** Tutorials 3 and 4 swap, so Mega Strike comes
  before Mega Saves (`StrikerChallenge.cpp:23`). KR inherits this.
- **Online scores (code).** Scores are capped at 99,999 instead of 999,999
  (`NetworkStatsManager.cpp:882`, `SHOnlineRanking.cpp`).
- **Mii names (code).** Mii-name filtering keeps Greek-range characters
  when the language is Japanese.
- **Striker Times (code and data).** All Striker Times scenes use the
  Japanese layout `striker_times_jp.fen` with its `st_logo_jp` masthead.
- **Input fixes (code).**
  - The options screen disables Done and Back during page flips.
  - The five Striker Cup screens give Back priority and clear held +/−.
  - Online choose-sides gives each console a fixed host and guest label.
- **Disc oddity (data).** The data partition's internal title still reads
  "Mario Strikers Charged Football", the EU string.

### USA (US), compared with both EU revisions

- **Gameplay fixes (code).**
  - The goalie cannot cross midfield.
  - The Mega Strike meter freezes the ball and both goalies.
  - Captain super abilities are blocked during the hit state.
  - An electrocuted player cannot also catch fire.
  - Birdo stops at Waluigi walls.
  - The ball bounces only when falling and resets its air resistance.
  - The Yoshi egg is pushed out of a Thwomp.
  - A zero-vector (NaN) launch on Thunder Island is guarded.
  - An uninitialized deke launch scale is fixed.
- **Gameplay changes (code).**
  - Waluigi walls now survive a Mega Strike.
  - The electrocution-scream throttle is removed.
  - The "Shells" cheat gives 66% red shells, and "Peelin' Out" gives 50%
    mushrooms.
  - The landed-hammer angle changes.
- **Online (code).**
  - A multi-stage login with a 180 s timeout.
  - A network heap of 710,000 or 510,000 bytes.
  - Regional clocks for day and season boundaries.
  - Friend-code error codes.
  - Cleanup of slots whose Mii was deleted.
- **Front end (code).**
  - The health-and-safety strap is shown before the ESRB screen.
  - Text buffers grow from 32 to 128 characters, and a text-wrap width bug
    is fixed.
  - Disc messages use French or Spanish images on US consoles.
- **Main logo (data).** US and JP use `SMC_mainlogo`; both EU revisions
  (and KR) use the wider `SMC_mainlogo_UK`.

### EU rev2 (EU2), compared with EU rev1

- **Text (data).** EU1's French HOME Menu text says "Wiimote", which
  Nintendo does not allow; EU2 says "télécommande Wii".
- **Credits (data).** EU2 fixes "The Producers", "KOUJI KONDO" and the
  missing Tony Iammarino.
- **Save data (code).** The Cup-history date base year moves from 2006 to
  2000. *Interpretation*: an EU1 save read by EU2 would show years six too
  early.
- **Audio (code).**
  - The audio-effect "owner" flag moves out of a float's low bit.
  - Movies play mono on Mono consoles.
  - The Wii Remote speaker is muted when idle.
  - A dedicated front-end music bank is added, with the 39 MB
    `FE_GEN_Music` files back on the disc.
- **Rendering (code).**
  - The pixel centre goes from 0.5 to 0.0.
  - The Peach photo becomes a single alpha-tested quad.
  - Reduced stadium updates are on, for Vice only.
  - A cutscene stadium-effects opcode is added.
  - The in-game texture pool is 0x900000, a value no other build uses.
- **Dead code and leftovers removed.** The unreachable online-tournament
  exit is gone, along with EU1's 46 unused files.
- **Not back-ported from US.** The goalie midfield clamp, the Mega Strike
  freeze, the online login rework and the ~20 other US fixes stay EU1-style.

## 3. Lineage

```
Change 41012 (2007-04-13) ── EU1 ──► EU2 (back-ports ~25 US fixes, same SDK and stamp)
                              │
                              └────► US  (Change 48053, 2007-05-30, newer SDK)
                                      └─► JP (US code + JP UI fixes; asset stamp Change 47759)
                                            └─► KR (2010 rebuild, SDK 4.x, Change 151174)
```

Pairwise similarity. Each cell is *identical files on disc* / *functions
added, removed or resized*.

|  | EU1 | EU2 | US | JP | KR |
| --- | --- | --- | --- | --- | --- |
| **EU1** | 2399 / 0 | 2038 / 87 | 1630 / 243 | 1616 / 260 | 1233 / 2961 |
| **EU2** | 2038 / 87 | 2353 / 0 | 1737 / 160 | 1723 / 180 | 1306 / 2882 |
| **US** | 1630 / 243 | 1737 / 160 | 2354 / 0 | 2318 / 29 | 1527 / 2807 |
| **JP** | 1616 / 260 | 1723 / 180 | 2318 / 29 | 2354 / 0 | 1540 / 2781 |
| **KR** | 1233 / 2961 | 1306 / 2882 | 1527 / 2807 | 1540 / 2781 | 2334 / 0 |

US and JP are almost the same build. EU2 sits between EU1 and US. KR is a
separate rebuild on a later SDK.

## 4. Disc and system data

| Item | EU1 | EU2 | US | JP | KR |
| --- | --- | --- | --- | --- | --- |
| Disc header revision | 1 | 2 | 1 | 0 | 0 |
| Disc header title | Mario Strikers Charged Football | Mario Strikers Charged Football | Mario Strikers Charged | Mario Strikers Charged | Mario Strikers Charged |
| Data-partition `boot.bin` title | …Charged Football | …Charged Football | Mario Strikers Charged | **Mario Strikers Charged Football** (EU string left in) | Mario Strikers Charged |
| Banner title (`opening.bnr`) | "Mario Strikers Charged Football", all languages | same as EU1 | JP slot マリオストライカーズチャージド, others "Mario Strikers Charged" | same text as US, different file | JP slot Japanese; every other slot, KO included, 마리오 파워 사커 |
| Region code (`region.bin`) | 2 (PAL) | 2 | 1 (NTSC-U) | 0 (NTSC-J) | 4 (Korea) |
| Age ratings | USK 6, PEGI 7, PEGI-FI 7, PEGI-PT 6, BBFC 7, AU 7 | same | ESRB byte 0x2A (age 10, E10+) | CERO 0 (A) | GRB 0 (All) |
| Ticket common key | 0 (retail) | 0 | 0 | 0 | 1 (Korean key) |
| Encrypted title key | `1536da8e…` | `1536da8e…` (same key) | `43a3633b…` | `e4e936a0…` | `82944232…` |
| IOS in TMD | IOS21 | IOS21 | IOS21 | IOS21 | IOS55 |
| Apploader | 2007/03/01 | 2007/03/01 | 2007/03/01 | 2007/03/01 | 2009/06/22 (244,892 B) |
| Update partition | 72.0 MiB, System Menu v162 (2.1E) | identical to EU1 except the header revision byte | 145 MiB, v193 (2.2U), adds IOS20 and the Forecast, News and Region Select channels | 73.2 MiB, v128 (2.0J) | 99.5 MiB, v486 (4.2K), boot2 v4, 24 IOS titles up to IOS70 and IOS254 |
| Files on disc (`/files`) | 2,399 | 2,353 | 2,354 | 2,354 | 2,334 |
| Bytes in `/files` | 1,861,323,896 | 1,897,057,486 | 1,895,722,439 | 1,895,344,344 | 1,898,251,107 |

## 5. Build provenance

`ini/datetime.ini` holds the Perforce stamp of the asset build. The game
parses its changelist number into `g_BuildNumber`
(`src/Game/main.cpp:386`). `GetNetworkVersionWord`
(`src/Game/NetworkSession.cpp:2629`) combines it with a regional channel. The
stamp therefore decides which builds can match online.

| Item | EU1 | EU2 | US | JP | KR |
| --- | --- | --- | --- | --- | --- |
| `datetime.ini` date | Apr 13 2007 18:18:30 | same as EU1 | May 30 2007 11:14:36 | May 28 2007 16:05:26 | Jan 26 2010 12:01:19 |
| Perforce change | 41012, HWOOLLEY: "Fix for corrupt save banner pro…" | 41012 | 48053, ALEONATI: "fixed shadow volume" | 47759, NSINGH: "credits files for NA and EU" | 151174, JMYKLEBUST.S2K: "Strikers2Korea - removed DWC_Is…" |
| Build user | WIIBURNER | WIIBURNER | WIIBURNER | WIIBURNER | SMAH2 |
| Extra stamp | `datetime.ini.real`: change 41751, Apr 19 2007, "Clean audio build for bug # 449" | — | — | — | — |
| Network channel | 14 | 14 | 10 | 15 | 13 |
| Network version word | `0x1BE3A034` | `0x1BE3A034` | `0x1BA3BBB5` | `0x1BF3BA8F` | `0x1BD34E86` |

EU2 kept EU1's `datetime.ini`, so both EU revisions share one online pool.
EU1's `.real` file suggests that the shipped stamp was pinned on purpose
(*interpretation*). JP's stamp predates US, yet JP's code is a superset of US.
The JP disc therefore used an older asset cut with a newer executable
(*interpretation*).

## 6. Executable (`main.dol`) and SDK

| Item | EU1 | EU2 | US | JP | KR |
| --- | --- | --- | --- | --- | --- |
| SHA-1 | `0a066b75…` | `c39e1eda…` | `e96d2298…` | `74be7234…` | `c2f893aa…` |
| Size (bytes) | 5,691,072 | 5,692,416 | 5,695,424 | 5,696,544 | 5,453,344 |
| Entry point | 0x80006124 | 0x80006124 | 0x80006124 | 0x80006124 | 0x80006140 |
| `.text` | 0x4D32C8 | 0x4D3858 | 0x4D40CC | 0x4D452C | 0x4A952C |
| `.rodata` | 0x1868C | 0x1868C | 0x18504 | 0x18504 | 0xDCA8 |
| `.data` | 0x73E94 | 0x73E54 | 0x742A4 | 0x742A4 | 0x6EA28 |
| `.bss` | 0x172C70 | 0x172CB0 | 0x172FB0 | 0x172FB0 | 0xD1560 (−647 KiB; the 416 KiB `vf` drive work area is gone) |
| Functions | 17,643 | 17,643 | 17,661 | 17,668 | 16,891 |
| Translation units | 1,314 | 1,314 | 1,314 | 1,314 | 1,235 (26 new, 105 removed) |
| SDK compiler stamp | 0x4199_60831 (GC/3.0a5.2) | same | same | same | 0x4302_145 (Wii/1.0); network libraries keep 0x4199_60831 |
| Game-code compiler | GC/3.0a5 | GC/3.0a5 | GC/3.0a5 | GC/3.0a5 | GC/3.0a5 |
| OS / DVD | Jan 9 2007 | Jan 9 2007 | Apr 24 2007 | Apr 24 2007 | Feb 27 2009 |
| WPAD / KPAD | Jan 9 2007 / Nov 30 2006 | same | May 17 2007 | May 17 2007 | Jun 22 2009 |
| DWC | Apr 13 2007 | Apr 13 2007 | May 11 2007 | May 11 2007 | May 14 2009 |
| HOME Menu (HBM) | Dec 7 2006 | same | same | same | Mar 3 2009 (0x4201_142), AX-synth sound |
| RevoEX (network) | 1.0, REL 070309140556 | same | same | same | 2.4, REL 090609111526 |
| Libraries only in this build | — | — | — | — | NHTTP, SSL, SOCKET, `OSPlayTime`/ES, `dvdDeviceError` (unauthorized-device check), `NANDLogging`, `SCCheckPCMessageRestriction`, RFL version stamp |
| Libraries dropped | — | — | — | — | `vf` (28 units), NWC24 mail units, `nw4hbm/snd`, old GameSpy crypto |

## 7. Code differences

### Gameplay

| Change | EU1 | EU2 | US | JP | KR |
| --- | --- | --- | --- | --- | --- |
| Goalie forces the carrier to drop the ball on catch or pickup (`StealBall`) | — | ✓ (some call sites placed differently from US) | ✓ | ✓ | ✓ |
| Goalie cannot cross midfield (`Goalie.cpp:329`) | — | — | ✓ | ✓ | ✓ |
| Mega Strike meter freezes the ball and both goalies | — | — | ✓ | ✓ | ✓ |
| Captain super ability blocked while in hit state (Bowser, Petey, Waluigi) | — | — | ✓ | ✓ | ✓ |
| Electrocuted player cannot also be set on fire | — | — | ✓ | ✓ | ✓ |
| Birdo in hit state stops at a Waluigi wall | — | — | ✓ | ✓ | ✓ |
| Waluigi walls end when a Mega Strike starts | ✓ | ✓ | — | — | — |
| Ball bounces only when falling and resets air resistance | — | — | ✓ | ✓ | ✓ |
| Yoshi egg pushed out of a Thwomp | — | — | ✓ | ✓ | ✓ |
| Thunder Island launch guards a zero vector (NaN) | — | — | ✓ | ✓ | ✓ |
| Uninitialized deke launch scale fixed | — | — | ✓ | ✓ | ✓ |
| Power-up award timer pauses during a Mega Strike | — | — | ✓ | ✓ | ✓ |
| Electrocution scream throttled to once per 5 frames | ✓ | ✓ | — | — | — |
| Shrink sound only during play | — | ✓ | ✓ | ✓ | ✓ |
| "Shells" cheat | green only | green only | 66% red / 34% green | same as US | same as US |
| "Peelin' Out" cheat | banana only | banana only | 50% banana / 50% mushroom | same as US | same as US |
| Landed hammer angle | 125° (100–150°) | same | 135° (105–165°) | same as US | same as US |
| Goalie loose-ball net range | 13 | 13 | 13 | 13 | **15** |
| Goalie lob-save descent angle | 40° | 40° | 40° | 40° | **60°** |
| Goalie pickup distance | 1.2 | 1.2 | 1.2 | 1.2 | **1.5** |
| Goalie chases deking or running shooters (range 8) | — | — | — | — | ✓ |
| Ball re-enables player and goalie collision on release | — | — | — | — | ✓ |
| Tutorials 3 and 4 | Mega Saves, then Mega Strike | same | same | **Mega Strike first** | **Mega Strike first** |

### Presentation, audio and memory

| Change | EU1 | EU2 | US | JP | KR |
| --- | --- | --- | --- | --- | --- |
| Pixel centre | 0.5 | 0.0 | 0.0 | 0.0 | 0.0 |
| Peach photo | 3×3 grid, no alpha test | single quad with alpha test | same as EU2 | same as EU2 | same as EU2 |
| Mono downmix for movies on Mono consoles | — | ✓ | ✓ | ✓ | ✓ |
| Wii Remote speaker muted when idle | — | ✓ | ✓ | ✓ | ✓ |
| Dedicated front-end music bank | — (reuses the in-game stream bank; no `FE_GEN_Music` on disc) | ✓ | ✓ | ✓ | ✓ |
| Audio-effect "owner" flag | packed into the low bit of a float | separate field | separate field | separate field | separate field |
| Presentation script opcode 63 (stadium effects) | — | ✓ | ✓ | ✓ | ✓ |
| Reduced stadium updates | off (a dead list of 6 stadiums plus Wastelands NPCs) | Vice only | Vice only | Vice only | Vice only |
| Cutscene shadow level reset | — | ✓ | ✓ | ✓ | ✓ |
| In-game texture pool | 0x966666 | 0x900000 | 0x8B3333 | 0x8B3333 | 0x8B3333 |
| Large FE texture pool | 0x900000 | 0x900000 | 0xA00000 | 0xA00000 | 0xB00000 |
| Video mode | PAL/EURGB60 detection | same | same | same | forced NTSC 480i |

### Front end

| Change | EU1 | EU2 | US | JP | KR |
| --- | --- | --- | --- | --- | --- |
| Boot order | ESRB*, strap, Nunchuk, NLG logo | same | strap, Nunchuk, ESRB*, NLG logo | same as US | two straps alternate (20.5 s), Nunchuk, no logo phase |
| Save icons and banner preloaded at the title screen | — (loaded during the NAND write) | ✓ | ✓ | ✓ | ✓ (and written to `/tmp`, then `NANDMove`) |
| Cup-history date base year | 2006 | 2000 | 2000 | 2000 | 2000 |
| Input guards on options, Cup and Hall of Fame screens | — | ✓ | ✓ | ✓ (and extra Back/page and held +/- fixes) | ✓ (same as JP) |
| Online tournament Back exit (dead code) | present | removed | removed | removed | removed |
| Text buffers 32 → 128 characters; text-wrap width fix | — | — | ✓ | ✓ | ✓ |
| Striker Times layout | `striker_times_2` | `striker_times_2` | `_2` (JP layout only for Japanese) | `striker_times_jp` always | `striker_times_jp` always |
| Online score display cap | 999,999 | 999,999 | 999,999 | 99,999 | 99,999 |
| Korean particle grammar, Korean fonts and line metrics | — | — | — | — | ✓ |
| Parental-control online restriction popup | — | — | — | — | ✓ |
| Save-error reset target | Wii Menu | Wii Menu | Wii Menu | Wii Menu | Data Management |
| Credits copyright phase | ✓ | ✓ | ✓ | ✓ | removed |

\* The ESRB screen is shown only when the console region is US.

### Online (Nintendo WFC)

| Change | EU1 | EU2 | US | JP | KR |
| --- | --- | --- | --- | --- | --- |
| Login timeout | 90 s | 90 s | 180 s | 180 s | 180 s |
| Network heap | 310,000 B | 310,000 B | 710,000 B (US region) / 510,000 B (others) | same as US | same as US |
| Login waits for all leaderboard stages | — | — | ✓ | ✓ | ✓ |
| Leaderboard refresh interval | 70 s | 70 s | 60 s | 60 s | 60 s |
| Regional clock for day and season boundaries | UTC | UTC | US −8 / JP +9 / KR +9 | same as US | same as US |
| Friend-code error codes (invalid, own, duplicate, full) | popup inside `AddFriendKey` | same | error codes, single popup | same as US | same as US |
| Deleted-Mii slot cleanup | — | — | ✓ | ✓ | ✓ |
| Matchmaking-thread guard and status during dialogs | — | ✓ | ✓ | ✓ | ✓ |
| Unranked results written only when reported | — | ✓ | ✓ | ✓ | ✓ |
| Matchmaking API | DWC 2007 | DWC 2007 | DWC 2007 | DWC 2007 | DWC 2009 (reservation callbacks) |

## 8. Front-end scene differences

Front-end screens are `.fen` scene files under `Art/fe`. The table compares
their component names. Hashed texture bundles (`mainui.dmn`,
`ingameui.dmn`) also differ between builds but carry no readable names.

| Scene | EU1 | EU2 | US | JP | KR |
| --- | --- | --- | --- | --- | --- |
| `sms2_start.fen` (title) | letters `SMC_a`…`SMC_r` + `SMC_lightning`, `charged`, `lightning` | same | same | same | `logo_firetrails`, `logo_firetrails2`–`19`, `flame1`–`2`, `smallflame1`–`3`, `flare4`, `firetrails` |
| `main_menu_v3.fen` | letters + lightning, `SMC_mainlogo_UK` | same | letters + lightning, `SMC_mainlogo`, `btn_main_101_jp` | same as US | static `SMC_mainlogo_UK` only, no `copyright` (105,536 B vs 189,588 B) |
| `choose_captains_domination.fen` | letters + lightning, `SMC_mainlogo_UK` | same | letters + lightning, `SMC_mainlogo` | same as US | static `SMC_mainlogo_UK` only |
| `boot_loading.fen` | per-language straps (US, French, German, Italian, Spanish, Dutch; 4:3 and 16:9), `nlgameslogo` | same | same | same | `strapA_Korean`, `strapB_Korean`, `nunchukArt`; no `nlgameslogo` |
| `striker_times_jp.fen` | `st_logo_jp` | — | `st_logo_jp` | `st_logo_jp` | `st_logo_korean` |
| `controller_config.fen` | — | — | — | — | adds `CONTROLLER_CONFIG_Korean` |
| `popup_menu.fen` | — | — | adds `button_OK` | same as US | same as US |
| `online_invite_players.fen` | 164,744 B | same | same | same | 323,908 B; every component appears twice |

Other scenes, such as `goal_overlay.fen`, `options.fen` and
`leaderboard_rankings.fen`, keep the same component names but differ in
layout data between builds.

## 9. Disc assets

| Item | EU1 | EU2 | US | JP | KR |
| --- | --- | --- | --- | --- | --- |
| Localisation files (`.loc`) | English, UK English, French, NA French, German, Italian, Spanish, NA Spanish, Japanese, plus `bob` and `longest` test sets | same | same | same | English, Japanese and Korean only |
| `japanese.loc` size | 102,020 | 102,020 | 117,994 | 119,610 | 123,084 |
| HOME Menu archives | ENG, FRA, GER, ITA, NED, SPA + `.brsar` sound | same | same | same | adds KOR and CHN; sound as `.arc` |
| "Wiimote" in the French HOME Menu text | ✓ (`home.csv`, `home_nosave.csv`) | — ("télécommande Wii") | — | — | — |
| `FE_GEN_Music.nlxwb` / `.resbun` (39 MB) | — | ✓ | ✓ | ✓ | ✓ |
| ODE BSD licence (`license-bsd.txt`) | ✓ | ✓ | — | — | ✓ |
| Unused leftovers | 46 files: NPCs `diddybanana`, `diddyzenleft`/`right`, `dkmask1`–`3`, `luigiskeleton`; `in_spiral_ball` transition; `*heading24` fonts; `mc_icon.tpl`; `riotguard_home_0.nis`; `datetime.ini.real` | — | — | — | Developer files: `shot000`–`005.tga`, `recentTweaks.txt`, `runUniqueID.txt`, `ini/user.ini` |
| `striker_times_jp.fen` / `controller_config_jp.fen` | ✓ / ✓ | — / — | ✓ / ✓ | ✓ / ✓ | ✓ / — |
| `stad_thunderisland.thp` | EU version | EU version | shared | shared | shared |
| Movies, including the `_pal` variants | same bytes on every disc except `stad_thunderisland.thp` | same | same | same | same |

KR's `shot*.tga` are 640×448 QA captures: a Korean NAND-error dialog and the
Mii Select screen with Miis named in symbol characters. The capture code
(`glxSwap.cpp`) exists in every build but is never enabled. No build reads
`recentTweaks.txt`, `runUniqueID.txt` or `user.ini`. The last file holds
commented-out debug options such as `skipfe`, `unlock_all` and `dosoak`.

### Credits (`credits.txt`)

| Build | Differences |
| --- | --- |
| EU1 | "The Producers"; no Tony Iammarino; "KOUJI KONDO"; adds Nintendo of Europe localisation and testing staff |
| EU2 | Corrects all three to the US wording; keeps the Nintendo of Europe staff |
| US | Baseline: NOA Product Testing and a "Text Editing" section |
| JP | US without the "Text Editing" section |
| KR | Adds NLG staff Scott Hansen, Paul Gill, Jahn Myklebust and Sarah Mah, Korean localisation support, and Nintendo of Korea staff; drops NOA testing and Debug Support; "SUPER MARIO CLUB" becomes "MARIO CLUB CO., LTD." |

## 10. What does not differ

- **Tuning files.** Every gameplay tuning file is byte-identical on all five
  discs: `GameTweaks.ini`, `CharTweaks.ini`, `FuzzyTweaks.ini`, `goalie.ini`,
  `FormationSets.ini` and the `Characters`, `Stadiums`, `StrikerChallenges`,
  `DifficultyLevels` and `Terrain` folders. All balance differences
  therefore come from code.
- **Movies.** All movies are byte-identical on every disc, including the
  `_pal` variants on the NTSC discs, except `stad_thunderisland.thp` on the
  EU discs.
- **Shared language data.** US and JP carry the full European language set,
  and every build carries `japanese.loc`. Code decides which languages can
  be selected.
- **Title cheat input.** The title-screen unlock input sequence
  (`gTitleUnlockInputSequence`) has no version conditional.
- **Compiler.** The game code uses the same compiler, GC/3.0a5, in every
  build. Only KR's SDK libraries moved to a newer compiler.

## 11. Method and limits

- Disc data came from `dtk disc info`/`extract` and SHA-1 hashes of every
  file. Header fields were parsed from `boot.bin`, `bi2.bin`, `apploader.img`,
  `region.bin`, `tmd.bin`, `ticket.bin` and `opening.bnr`.
- Executable data came from `dtk dol info`, the `config/*/symbols.txt` and
  `splits.txt` files, and SDK banner strings.
- Code differences came from the `VERSION_*` conditionals in `src/Game`,
  `src/NL` and their headers. These conditionals reproduce all five
  executables byte for byte, but the gameplay effects above are readings of
  that code, not tested behaviour.
- System Menu names (2.0J, 2.1E, 2.2U, 4.2K) follow the standard
  version-number scheme and its matching IOS set (IOS20 for 2.2, IOS70 for
  4.2).
- The rating names assume the standard `region.bin` slot order.
- Front-end scene differences compare component names found in the `.fen`
  files; positions and keyframes were not decoded.
