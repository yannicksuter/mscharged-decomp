#ifndef NL_FUNCTION_MEMORY_H
#define NL_FUNCTION_MEMORY_H

void InitializeFunctionMemory();
void PushFunctionMemoryState();
void PopFunctionMemoryState();
void FreeFunctionMemoryPools();
void* AllocateFunctionMemory(unsigned long size);
void FreeFunctionMemory(void* entry, unsigned long size);

#endif // NL_FUNCTION_MEMORY_H
