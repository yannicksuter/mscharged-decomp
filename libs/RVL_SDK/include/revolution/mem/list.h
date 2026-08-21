#ifndef REVOLUTION_MEM_LIST_H
#define REVOLUTION_MEM_LIST_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MEMList {
    void* head;
    void* tail;
    u16 length;
    u16 offset;
} MEMList;

typedef struct MEMLink {
    void* prev;
    void* next;
} MEMLink;

void MEMInitList(MEMList* list, u16 offset);
void MEMAppendListObject(MEMList* list, void* object);
void MEMRemoveListObject(MEMList* list, void* object);
void* MEMGetNextListObject(MEMList* list, void* object);

#define MEM_INIT_LIST(list, structName, linkName) \
    MEMInitList((list), (u16)(u32) & (((structName*)0)->linkName))

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_MEM_LIST_H
