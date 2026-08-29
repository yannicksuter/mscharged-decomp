#ifndef REVOLUTION_OS_LINK_H
#define REVOLUTION_OS_LINK_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef u32 OSModuleID;
typedef struct OSModuleLink OSModuleLink;
typedef struct OSModuleInfo OSModuleInfo;
typedef struct OSSectionInfo OSSectionInfo;

struct OSModuleLink {
    OSModuleInfo* next;
    OSModuleInfo* prev;
};

struct OSModuleInfo {
    OSModuleID id;
    OSModuleLink link;
    u32 numSections;
    u32 sectionInfoOffset;
    u32 nameOffset;
    u32 nameSize;
    u32 version;
};

struct OSSectionInfo {
    u32 offset;
    u32 size;
};

#define OSGetSectionInfo(module) ((OSSectionInfo*)(((OSModuleInfo*)(module))->sectionInfoOffset))

void __OSModuleInit(void);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_OS_LINK_H
