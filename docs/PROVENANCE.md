# Source provenance

This project reconstructs source code from the behavior and machine code in a
legally obtained copy of *Mario Strikers Charged*. It does not distribute game
assets, Nintendo SDK binaries, extracted assembly, or original proprietary
source code.

Some standard runtime and Wii SDK implementations are adapted from existing
decompilation projects. Those projects publish their reconstruction work under
CC0 1.0. Third-party library reconstructions retain their upstream terms.
Although CC0 does not require attribution, the exact reference snapshots are
recorded here so that the origin and later local changes remain auditable.

| Local area | Reference snapshot | Upstream license | Use in this project |
| --- | --- | --- | --- |
| `src/Runtime/` and supporting runtime headers, except `runtime.c` | [SMGCommunity/Petari at `e789e52`](https://github.com/SMGCommunity/Petari/tree/e789e52e7774cf52cdf093002fc32035b6eab65d) | CC0 1.0 | Adapted CodeWarrior runtime reconstructions; retained only where the R4QE01 object output matches. |
| `src/Runtime/runtime.c` | [ThePlayerRolo/LegoCloneWarsWii at `b0ed795`](https://github.com/ThePlayerRolo/LegoCloneWarsWii/tree/b0ed795a586d1d921c536c991bf250bec39ae811) | CC0 1.0 | Adapted runtime reconstruction, including the compiler-generated inline assembly forms found in Wii and GameCube titles. |
| `src/RVL_SDK/` and `libs/RVL_SDK/include/` | [koopthekoopa/wii-ipl at `0b0cedd`](https://github.com/koopthekoopa/wii-ipl/tree/0b0cedd44d83fa187eb3f52748c3eecf0840865b) | CC0 1.0 | Adapted or reduced Wii SDK reconstructions. Matched units based primarily on this snapshot are `mem_allocator.c`, `mem_list.c`, `OSAlloc.c`, `OSArena.c`, `OSIpc.c`, and the three linked routines in `OSMessage.c`. |
| `src/RVL_SDK/os/OSLink.c`, `OSMutex.c`, and `OSReboot.c` | [SMGCommunity/Petari at `e789e52`](https://github.com/SMGCommunity/Petari/tree/e789e52e7774cf52cdf093002fc32035b6eab65d) | CC0 1.0 | Adapted from the older Wii SDK reconstruction variant, retaining only routines present in R4QE01. Supporting declarations are reduced to the currently reconstructed surface. |
| `src/RVL_SDK/os/OSRtc.c` | [SMGCommunity/Petari at `e789e52`](https://github.com/SMGCommunity/Petari/tree/e789e52e7774cf52cdf093002fc32035b6eab65d) | CC0 1.0 | Adapted the older SRAM/RTC implementation and reduced supporting EXI/OS declarations. All nine retained C functions and the complete SRAM control block match R4QE01 exactly. |
| `src/RVL_SDK/os/OSStateTM.c` | [zeldaret/oot-vc at `9c18537`](https://github.com/zeldaret/oot-vc/tree/9c1853710bcf6b598b0710022fdb87b9a8e99e2c) and [SMGCommunity/Petari at `e789e52`](https://github.com/SMGCommunity/Petari/tree/e789e52e7774cf52cdf093002fc32035b6eab65d) | CC0 1.0 | References for the R4QE01 STM variant; neither source is used unchanged. R4QE01 combines oot-vc's event-buffer and callback layout with Petari's reset-button polling entry point. |
| `src/RVL_SDK/mem/mem_heapCommon.c` and `mem_expHeap.c` | [SMGCommunity/Petari at `e789e52`](https://github.com/SMGCommunity/Petari/tree/e789e52e7774cf52cdf093002fc32035b6eab65d) and [zeldaret/tp at `ecafc81`](https://github.com/zeldaret/tp/tree/ecafc81d53cadf3c29ac01b03170cbe9bc37c686) | CC0 1.0 | Adapted MEM reconstructions. The target comparison identifies R4QE01's pre-August-2010 allocation-margin and locking behavior; every retained function and global matches exactly. |
| `src/RVL_SDK/mem/mem_frameHeap.c` and `mem_unitHeap.c` | [koopthekoopa/wii-ipl at `0b0cedd`](https://github.com/koopthekoopa/wii-ipl/tree/0b0cedd44d83fa187eb3f52748c3eecf0840865b) and [zeldaret/oot-vc at `9c18537`](https://github.com/zeldaret/oot-vc/tree/9c1853710bcf6b598b0710022fdb87b9a8e99e2c) | CC0 1.0 | Adapted the retained frame- and unit-heap routines. Unlinked routines are omitted; all included functions match R4QE01 exactly. |
| `src/ode/` except `src/ode/ext/` and `src/ode/NLGAdditions.cpp`, `include/ode/`, and the ODE API | Open Dynamics Engine 0.5 (`ode-040529`) and [yannicksuter/smstrikers-decomp at `8bd87299`](https://github.com/yannicksuter/smstrikers-decomp/tree/8bd872998439d7204061624980cc268528874ea2) | [ODE BSD-style license](../LICENSES/ODE-BSD.txt) | Pristine ODE 0.5 is the source baseline. The predecessor reconstruction is used to identify the library, retained algorithms, and likely local adaptations; it is not treated as target evidence. R4QE01's object boundaries, exported surface, compiler behavior, and data layout are validated independently. The currently source-linked units are `collision_kernel.cpp`, `collision_space.cpp`, `collision_transform.cpp`, `error.cpp`, `joint.cpp`, `matrix.cpp`, `memory.cpp`, `mass.cpp`, `obstack.cpp`, `odemath.cpp`, `ode.cpp`, and `rotation.cpp`; each reproduces all of its retained R4QE01 code and data and passes the full-DOL hash check. |
| `src/ode/NLGAdditions.cpp` and `include/ode/NLGAdditions.h` | [yannicksuter/smstrikers-decomp at `8bd87299`](https://github.com/yannicksuter/smstrikers-decomp/tree/8bd872998439d7204061624980cc268528874ea2) and Open Dynamics Engine 0.5 (`ode-040529`) | CC0 1.0 and [ODE BSD-style license](../LICENSES/ODE-BSD.txt) | The predecessor identifies the proprietary Next Level Games extension and provides the reconstruction starting point. The AABB helper is the pristine ODE 0.5 implementation. Charged adds output-buffer geometry accessors and a variable three- or four-row character joint. Every retained function, the character-joint vtable, and all constants reproduce R4QE01 exactly. |
| `src/ode/ext/dFinitePlane.cpp` and `include/ode/ext/` | [yannicksuter/smstrikers-decomp at `8bd87299`](https://github.com/yannicksuter/smstrikers-decomp/tree/8bd872998439d7204061624980cc268528874ea2) | CC0 1.0 | The predecessor identifies this Next Level Games extension and provides the reconstruction starting point. R4QE01's complete translation unit was validated independently and required Charged-specific expression and declaration ordering. All four retained functions and both data sections reproduce the retail object. |

External source is not considered correct merely because another project uses
it. Each translation unit is compiled with this project's flags, compared
against the R4QE01 target object, and marked `Matching` only after code and data
agree. A full build must also reproduce the original `main.dol` hash.

The repository's [CC0 license](../LICENSE) applies only to contributions whose
authors have the right to make that dedication. Nintendo, Next Level Games,
and third-party names, trademarks, game content, and other proprietary
material are not granted or licensed by this repository.
