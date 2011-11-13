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
class Core;

class Disassembler {
  private:
    char pcsmflag;
    char pcsmcnt;
    char countfrsp;
    const Core* const parent;
  public:
    explicit Disassembler(const Core* core);

    void mn(const Instr & instr);
    void reg(WORD opcode);
    void op(BYTE re, BYTE mo);
    void aim();
    void comma();
};
}

#endif