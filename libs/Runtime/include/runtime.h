#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define SAVE_FPR(reg) _savefpr_ ## reg
#define RESTORE_FPR(reg) _restfpr_ ## reg
#define SAVE_FPR2(reg) _savef ## reg
#define RESTORE_FPR2(reg) _restf ## reg
#define SAVE_GPR(reg) _savegpr_ ## reg
#define SAVE32_GPR(reg) _save32gpr_ ## reg
#define RESTORE_GPR(reg) _restgpr_ ## reg
#define RESTORE32_GPR(reg) _rest32gpr_ ## reg

#define DECLARE_REGISTER_HELPERS(reg) \
    void SAVE_FPR(reg)(void); \
    void RESTORE_FPR(reg)(void); \
    void SAVE_FPR2(reg)(void); \
    void RESTORE_FPR2(reg)(void); \
    void SAVE_GPR(reg)(void); \
    void SAVE32_GPR(reg)(void); \
    void RESTORE_GPR(reg)(void); \
    void RESTORE32_GPR(reg)(void)

DECLARE_REGISTER_HELPERS(14);
DECLARE_REGISTER_HELPERS(15);
DECLARE_REGISTER_HELPERS(16);
DECLARE_REGISTER_HELPERS(17);
DECLARE_REGISTER_HELPERS(18);
DECLARE_REGISTER_HELPERS(19);
DECLARE_REGISTER_HELPERS(20);
DECLARE_REGISTER_HELPERS(21);
DECLARE_REGISTER_HELPERS(22);
DECLARE_REGISTER_HELPERS(23);
DECLARE_REGISTER_HELPERS(24);
DECLARE_REGISTER_HELPERS(25);
DECLARE_REGISTER_HELPERS(26);
DECLARE_REGISTER_HELPERS(27);
DECLARE_REGISTER_HELPERS(28);
DECLARE_REGISTER_HELPERS(29);
DECLARE_REGISTER_HELPERS(30);
DECLARE_REGISTER_HELPERS(31);

#undef DECLARE_REGISTER_HELPERS

unsigned long __cvt_fp2unsigned(double);
asm void __div2u(void);
asm void __div2i(void);
asm void __mod2u(void);
asm void __mod2i(void);
asm void __shl2i(void);
asm void __cvt_ull_dbl(void);
asm void __cvt_ull_flt(void);
asm void __cvt_dbl_ull(void);

#ifdef __cplusplus
}
#endif
