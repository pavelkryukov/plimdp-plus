/*
 * isa.h
 *
 * PlimDP+ pdp11 instruction set
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#ifndef ISA_H
#define ISA_H

#include "./types.h"
#include "./instr.h"

namespace PlimDP {
class ISA {
    private:
        static const Instr instrs[];
        static const size_t instrs_s;
    public:
        static Instr find_instrs(WORD opcode);
};
}

#endif
