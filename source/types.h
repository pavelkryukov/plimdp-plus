/*
 * types.h
 * 
 * PlimDP++ types file
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#ifndef TYPES_H
#define TYPES_H

namespace PlimDP {

typedef unsigned char BYTE;
typedef signed char SBYTE;
typedef unsigned short WORD;
typedef signed short SWORD;
typedef unsigned DWORD;
typedef signed SDWORD;

enum KeyRW {
    READ,
    WRITE,
};

enum InstrType {
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
};
}

#endif
