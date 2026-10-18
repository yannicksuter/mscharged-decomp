#ifndef REVOLUTION_NAND_FWD_H
#define REVOLUTION_NAND_FWD_H

#define NAND_MAX_PATH 64

#ifdef __cplusplus
extern "C" {
#endif

long NANDGetHomeDir(char* out);
long NANDMove(const char* from, const char* to);

#ifdef __cplusplus
}
#endif

#endif // REVOLUTION_NAND_FWD_H
