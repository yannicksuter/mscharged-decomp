#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
  DWC_ALLOCTYPE_AUTH,
  DWC_ALLOCTYPE_AC,
  DWC_ALLOCTYPE_BM,
  DWC_ALLOCTYPE_UTIL,
  DWC_ALLOCTYPE_BASE,
  DWC_ALLOCTYPE_LANMATCH,
  DWC_ALLOCTYPE_GHTTP,
  DWC_ALLOCTYPE_RANKING,
  DWC_ALLOCTYPE_ENC,
  DWC_ALLOCTYPE_GS,
  DWC_ALLOCTYPE_ND,
  DWC_ALLOCTYPE_OPTION_CF,
#if defined(VERSION_R4QK01)
  DWC_ALLOCTYPE_NHTTP = 13,
#else
  DWC_ALLOCTYPE_NHTTP,
#endif
  DWC_ALLOCTYPE_MAIL,
  DWC_ALLOCTYPE_NUM
} DWCAllocType;

typedef void* (*DWCAllocEx)(DWCAllocType name, unsigned long size, int align);
typedef void (*DWCFreeEx)(DWCAllocType name, void* ptr, unsigned long size);
typedef void* (*DWCAllocFunc)(DWCAllocType name, unsigned long size);
typedef void (*DWCFreeFunc)(DWCAllocType name, void* ptr, unsigned long size);

#ifdef __cplusplus
}
#endif
