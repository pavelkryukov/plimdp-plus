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

#include <plimdp/generic/types.h>

#include <plimdp/cpu/instr.h>

namespace PlimDP {
namespace CPU {
class ISA {
    private:
        static const Instr instrs[];
        static const size_t instrs_s;
        static signed table[(WORD)~1];
        static const bool table_f;
        static bool init_table();
    public:
        static Instr find_instrs(WORD opcode);
};
}
}

#endif
