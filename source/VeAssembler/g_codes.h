#include <stdint.h>

#ifndef _AS_CODES_H
#define _AS_CODES_H

#define F_REX_OP 0x48

#define I_MOV_I32_OP 0xC7
#define I_ADD_I32_OP 0x81
#define I_SUB_I32_OP 0x81
#define I_IMUL_I32_OP 0x69

#define I_MOV_R64_OP 0x89
#define I_ADD_R64_OP 0x01
#define I_SUB_R64_OP 0x81
#define I_IMUL_R64_OP 0x69

#define R_RAX_OP 0x00
#define R_RCX_OP 0x01
#define R_RDX_OP 0x02
#define R_RBX_OP 0x03
#define R_RSP_OP 0x04
#define R_RBP_OP 0x05
#define R_RSI_OP 0x06
#define R_RDI_OP 0x07


inline uint8_t _c_mrmb(int mod, int reg, int rm) { // Calc ModR/M byte
    if (mod < 0 || mod > 3 || reg < 0 || reg > 7 || rm < 0 || rm > 7) {
        return 0xFF;
    }
    return (mod << 6) | (reg << 3) | rm;
}

#endif