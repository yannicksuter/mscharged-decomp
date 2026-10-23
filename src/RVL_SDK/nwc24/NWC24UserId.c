#include <revolution/nwc24/internal/NWC24iUserId.h>

#define USER_ID_MASK 0x1FFFFFFFFFFFFFULL

static const u8 TtableInv[16] = {13, 5, 9, 7, 0, 15, 10, 2, 12, 3, 14, 1, 8, 6, 11, 4};
static const u8 ExcTable[6] = {1, 5, 0, 4, 2, 3};

static u64 getUnScrambleId(u64 value);

static int checkCRC(u64 value) {
    int i;

    for (i = 0; i < 43; i++) {
        if (((value >> (53 - (i + 1))) & 1) != 0) {
            value ^= 1589ULL << (42 - i);
        }
    }

    return value != 0;
}

NWC24Err NWC24iCheckUserIdCRC(u64 userId) {
    u64 decoded = getUnScrambleId(userId);
    if (!checkCRC(decoded)) {
        return NWC24_OK;
    }

    return NWC24_ERR_ID_CRC;
}

static u8 getbyte(u64 value, u8 index) {
    return (value >> (index * 8)) & 0xFF;
}

static u64 setbyte(u64 value, u8 index, u8 byte) {
    u64 mask = 0xFFULL << (8 * index);
    return (value & ~mask) | ((u64)byte << (8 * index));
}

static u64 getUnScrambleId(u64 value) {
    u64 temp;
    u8 i;

    value &= USER_ID_MASK;
    value ^= 0x5E5E5E5E5E5EULL;
    value &= USER_ID_MASK;
    value |= (value & 1) << 53;
    value >>= 1;

    temp = value;
    for (i = 0; i < 6; ++i) {
        value = setbyte(value, i, getbyte(temp, ExcTable[i]));
    }

    for (i = 0; i < 6; ++i) {
        value = setbyte(value, i, (TtableInv[(getbyte(value, i) >> 4) & 0xF] << 4) | TtableInv[getbyte(value, i) & 0xF]);
    }

    value = ((value & 0x7FFFFFFFFFFULL) << 10) | ((value >> 43) & 0x3FF);
    value ^= 0xB3B3B3B3B3B3ULL;
    return value;
}
