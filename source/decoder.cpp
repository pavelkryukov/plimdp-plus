/*
 * decoder.cpp
 *
 * PlimDP+ decoder
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#include "./macro.h"

#include "./decoder.h"

#if ENABLE_DISASM
    #define DISASM(OPERATIONS) OPERATIONS
#else
    #define DISASM(OPERATIONS)
#endif

namespace PlimDP {
Executor::Operand Decoder::select_operand(const Instr & instr, BYTE re, BYTE mo) {
    Executor::Operand pointer;
    WORD offset;
    switch (mo) {
        case 0:
            pointer.index = re;
            pointer.type = Operand::REGISTER;
            break;
        case 1:
            pointer.index = reg.readreg(re);
            pointer.type = Operand::MEMORY;
            break;
        case 2:
            pointer.index = reg.readreg(re);
            pointer.type = Operand::MEMORY;
            if (instr.size == 1 && re < 6)
                reg.increg(re, 1);
            else if (instr.size == 2 || re >= 6)
                reg.increg(re, 2);
            break;
        case 3:
            pointer.index = mem.readword(reg.readreg(re));
            pointer.type = Operand::MEMORY;
            reg.increg(re, 2);
            break;
        case 4:
            if (instr.size == 1 && re < 6)
                reg.increg(re, -1);
            else if (instr.size == 2 || re >= 6)
                reg.increg(re, -2);
            pointer.index = reg.readreg(re);
            pointer.type = Operand::MEMORY;
            break;
        case 5:
            reg.increg(re, -2);
            pointer.index = mem.readword(reg.readreg(re));
            pointer.type = Operand::MEMORY;
            break;
        case 6:
            offset = mem.readword(reg.readPC());
            reg.incPC(2);
            pointer.index = (WORD)(reg.readreg(re) + offset);
            pointer.type = Operand::MEMORY;
            break;
        case 7:
            offset = mem.readword(reg.readPC());
            reg.incPC(2);
            pointer.index = mem.readword((WORD)(reg.readreg(re) + offset));
            pointer.type = Operand::MEMORY;
            break;
    }
    return pointer;
}
BYTE Decoder::decode_m(BYTE a) {
    return (a & 070) >> 3;
}
BYTE Decoder::decode_r(BYTE a) {
    return a & 07;
}
void Decoder::decode(WORD opcode, const Instr & instr) {
    DISASM( disasm->mn(instr); )
    BYTE dd, ss, mo, re;
    switch (instr.type) {
        case Instr::T_SSDD:
            ss = (opcode & 0007700) >> 6;
            mo = decode_m(ss);
            re = decode_r(ss);
            DISASM( disasm->op(re, mo); )
            DISASM( disasm->comma(); )
            pS = select_operand(instr, re, mo);

            dd = opcode & 0000077;
            mo = decode_m(dd);
            re = decode_r(dd);
            DISASM( disasm->op(re, mo); )
            pD = select_operand(instr, re, mo);
            break;
        case Instr::T_DD:
            dd = opcode & 0000077;
            mo = decode_m(dd);
            re = decode_r(dd);
            DISASM( disasm->op(re, mo); )
            pD = select_operand(instr, re, mo);
            break;
        case Instr::T_XX:
            xx.value = opcode & 0xFF;
            DISASM( disasm->aim(); )
            break;
        case Instr::T_RSS:
            ss = opcode & 0000077;
            mo = decode_m(ss);
            re = decode_r(ss);
            DISASM( disasm->op(re, mo); )
            DISASM( disasm->comma(); )            
            pS = select_operand(instr, re, mo);
            
            mo = 0;
            re = (opcode & 0000700) >> 6;
            DISASM( disasm->op(re, mo); )
            pD = select_operand(instr, re, mo);
            break;
        case Instr::T_RDD:
            mo = 0;
            re = (opcode & 0000700) >> 6;
            DISASM( disasm->op(re, mo); )
            DISASM( disasm->comma(); )
            pS = select_operand(instr, re, mo);

            dd = opcode & 0000077;
            mo = decode_m(dd);
            re = decode_r(dd);
            DISASM( disasm->op(re, mo); )
            pD = select_operand(instr, re, mo);
            break;
        case Instr::T_R:
            mo = 0;
            re = opcode & 0000007;
            DISASM( disasm->op(re, mo) );
            pD = select_operand(instr, re, mo);
            break;
        case Instr::T_NONE:
            break;
        default:
            break;
    }
    DISASM( disasm->reg(opcode); )
}

Decoder::Decoder() : Executor() {
    DISASM( disasm = new Disassembler(this); )
}

Decoder::~Decoder() {
    DISASM( delete disasm; )
}

}