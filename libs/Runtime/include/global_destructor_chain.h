#ifndef RUNTIME_GLOBAL_DESTRUCTOR_CHAIN_H
#define RUNTIME_GLOBAL_DESTRUCTOR_CHAIN_H

#ifdef __cplusplus
extern "C" {
#endif

void* __register_global_object(void* object, void* destructor, void* registration);
void __destroy_global_chain(void);

#ifdef __cplusplus
}
#endif

#endif
