#ifndef MSL_INTERNAL_LOCALE_H
#define MSL_INTERNAL_LOCALE_H

#ifdef __cplusplus
extern "C" {
#endif

struct _loc_ctype_cmpt
{
    char name[8];
    const unsigned short* ctype_map_ptr;
    const unsigned char* upper_map_ptr;
    const unsigned char* lower_map_ptr;
    const unsigned short* wctype_map_ptr;
    const void* wupper_map_ptr;
    const void* wlower_map_ptr;
    void* decode_mb;
    void* encode_wc;
};

struct __locale
{
    struct __locale* next_locale;
    char name[0x30];
    void* coll_cmpt_ptr;
    struct _loc_ctype_cmpt* ctype_cmpt_ptr;
    void* mon_cmpt_ptr;
    void* num_cmpt_ptr;
    void* time_cmpt_ptr;
};

extern struct __locale _current_locale;

#ifdef __cplusplus
}
#endif

#endif // MSL_INTERNAL_LOCALE_H
