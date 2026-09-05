#ifndef UNCLASSIFIED_TU_8036D6F8_H
#define UNCLASSIFIED_TU_8036D6F8_H

class MemoryAllocator;
class nlMatrix4;

void fn_8036D6F8(MemoryAllocator* allocator);
MemoryAllocator* fn_8036D71C();

// Matrix upload helpers retained in the following automatic range; their
// parameter types are not established, so they keep C linkage.
extern "C" void fn_8036D774(const nlMatrix4* matrix);
extern "C" void fn_8036D7EC(
    const void* matrices, unsigned long count, const nlMatrix4* matrix, int);

#endif // UNCLASSIFIED_TU_8036D6F8_H
