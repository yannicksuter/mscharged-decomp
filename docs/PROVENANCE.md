# Source provenance

This project reconstructs source code from the behavior and machine code in a
legally obtained copy of *Mario Strikers Charged*. It does not distribute game
assets, Nintendo SDK binaries, extracted assembly, or original proprietary
source code.

The project was built with or adapted source from the following projects:

| Project | Use in this project |
| --- | --- |
| [encounter/dtk-template](https://github.com/encounter/dtk-template) | Project and build-system foundation. |
| [SMGCommunity/Petari at `e789e52`](https://github.com/SMGCommunity/Petari/tree/e789e52e7774cf52cdf093002fc32035b6eab65d) | CodeWarrior runtime and RVL SDK reconstructions. |
| [ThePlayerRolo/LegoCloneWarsWii at `b0ed795`](https://github.com/ThePlayerRolo/LegoCloneWarsWii/tree/b0ed795a586d1d921c536c991bf250bec39ae811) | CodeWarrior runtime reconstruction. |
| [koopthekoopa/wii-ipl at `0b0cedd`](https://github.com/koopthekoopa/wii-ipl/tree/0b0cedd44d83fa187eb3f52748c3eecf0840865b) | RVL SDK reconstructions. |
| [zeldaret/oot-vc at `9c18537`](https://github.com/zeldaret/oot-vc/tree/9c1853710bcf6b598b0710022fdb87b9a8e99e2c) | RVL SDK reconstructions. |
| [zeldaret/tp at `ecafc81`](https://github.com/zeldaret/tp/tree/ecafc81d53cadf3c29ac01b03170cbe9bc37c686) | RVL SDK reconstructions. |
| [yannicksuter/smstrikers-decomp at `8bd87299`](https://github.com/yannicksuter/smstrikers-decomp/tree/8bd872998439d7204061624980cc268528874ea2) | Predecessor game code and Next Level Games' ODE extensions. |
| Open Dynamics Engine 0.5 (`ode-040529`) | Baseline for ODE-derived source. |

The referenced decompilation projects publish their reconstruction work under
CC0 1.0. ODE-derived source retains the
[ODE BSD-style license](../LICENSES/ODE-BSD.txt). Other third-party material
retains its respective upstream terms.

External source is accepted only after comparison against R4QE01. A unit is
marked `Matching` only when its code and owned data agree and the complete
build reproduces the original `main.dol` hash.

The repository's [CC0 license](../LICENSE) applies only to contributions whose
authors have the right to make that dedication. Nintendo, Next Level Games,
and third-party names, trademarks, game content, and other proprietary
material are not granted or licensed by this repository.
