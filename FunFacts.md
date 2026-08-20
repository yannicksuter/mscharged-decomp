# Fun facts

## CodeWarrior runtime

The `Runtime.PPCEABI.H` implementations of `global_destructor_chain.c` and
`__init_cpp_exceptions.cpp` reproduce all four target functions and the final
R4QE01 DOL byte-for-byte.

The isolated functions do not uniquely identify the exact GameCube
CodeWarrior revision: `GC/3.0a3`, `GC/3.0a5`, and `GC/3.0a5.2` generate matching
code and layout. The project therefore retains `GC/3.0a5` as a compatible
bootstrap default while compiler versions are determined per library and
translation unit.
