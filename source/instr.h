/*
 * instr.h
 * 
 * PlimDP+ instruction class
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#ifndef INSTR_H
#define INSTR_H

#include <string>

#include "./types.h"

namespace PlimDP {
struct Instr {
    std::string name;
    WORD code;
    WORD mask;
    enum {
        T_DD,
        T_SSDD,
        T_RSS,
        T_XX,
        T_NONE,
        T_TT,
        T_R,
        T_RDD,
        T_NN,
        T_SS,
        T_RNN,
        T_N,
    } type;
    void (Core::*exec)();
    BYTE size;
};
}

#endif