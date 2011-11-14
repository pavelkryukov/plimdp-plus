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

#include "./executor.h"
#if ENABLE_DISASM
    #include "./disassembler.h"
#endif
#include "./instr.h"

namespace PlimDP {
class Decoder : public Executor {
  protected:
    // Decoder
    Operand select_operand(const Instr & instr, BYTE re, BYTE mo);
    BYTE decode_m(BYTE a);
    BYTE decode_r(BYTE a);
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

#endif