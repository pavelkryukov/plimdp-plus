/*
 * decoder.h
 *
 * PlimDP+ core decoder subsystem
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#ifndef DECODER_H
#define DECODER_H

#include <plimdp/cpu/executor.h>
#if ENABLE_DISASM
    #include <plimdp/cpu/tracing/disassembler.h>
#endif
#include <plimdp/cpu/instr.h>

namespace PlimDP {
namespace CPU {
class Decoder : protected Executor {
  protected:
    // Decoder
    Operand select_operand(const Instr & instr, BYTE re, BYTE mo);

    inline BYTE decode_m(BYTE a) {
        return (a & 070) >> 3;
    }

    inline BYTE decode_r(BYTE a) {
        return a & 07;
    }

    void decode(WORD opcode, const Instr & instr);

#if ENABLE_DISASM
    // Disassembler
    friend class Disassembler;
    Disassembler* disasm;
#endif
    Decoder();
    ~Decoder();
};
}
}

#endif