/*
 * disassembler.h
 *
 * PlimDP+ core disassembly print
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include "./types.h"
#include "./instr.h"

namespace PlimDP {
namespace CPU {
class Decoder;
class Disassembler {
  private:
    char pcsmflag;
    char pcsmcnt;
    char countfrsp;
    const Decoder* const parent;
  public:
    explicit Disassembler(const Decoder* core);

    void mn(const Instr & instr);
    void reg(WORD opcode);
    void op(BYTE re, BYTE mo);
    void aim();
    void comma();
};
}
}

#endif