/*
 * coredump.h
 * 
 * PlimDP+ core dump
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#ifndef COREDUMP_H
#define COREDUMP_H

#include "./types.h"

namespace PlimDP {
class Core;

class CoreDump {
  private:
    char pcsmflag;
    char pcsmcnt;
    char countfrsp;
    const Core* const parent;
  public:
    explicit CoreDump(const Core* core);
  
    WORD oldPC;
  
    void mn(BYTE idx);
    void reg(WORD opcode);
    void op();
    void aim();
    void comma();
    void end();
};
}

#endif