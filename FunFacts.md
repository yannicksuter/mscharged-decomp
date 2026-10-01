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

## Ball bindings and debug data

R4QE01 `Game/Ball.cpp` links from source with `-ipa file` and `-sym on` and
reproduces the original DOL byte-for-byte. A `TweakBinding<float>` reconstruction
preserves the 16-byte binding layout and virtual slots while emitting its
virtual methods after the static initializer, binding destructor, and array
destructor. Ordinary inline float-binding methods are emitted before the static
initializer with file IPA. The stripped retail image does not establish the
original template's name; `TweakFloatBinding` remains a typedef for callers.

The float pointer constructor is specialized in `TweakValue.inl`, after the
integer binding destructor. This preserves their code group order in
`GameTweaks.cpp` while allowing the constructor to inline in other TUs. An
explicit format array also keeps `FormatValue` in the virtual method group;
its string literal would create an earlier code group with `-sym on`.

The inline ball debug-field registration emits its field-name strings in reverse
registration order, after the float-binding vtable. Its retail names identify
the timers at offsets `0x0C`, `0x14`, `0x2C`, and `0x34` as the shot, lightning,
charge-loss, and riot timers respectively.

A function can show 100% code matching when relocation differences are ignored
and still load the wrong constant or tuning variable. Ball's shot duration,
charge-loss rate, failed landing prediction, and shot resistance exposed this.
The final source link and DOL checksum verify both instructions and relocations.
The object report's data coverage stays low with split source sections and
duplicate weak objects, although the linked DOL matches byte for byte.
