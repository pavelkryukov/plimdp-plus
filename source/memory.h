/*
 * memory.h
 * 
 * PlimDP+ memory subsystem
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#ifndef MEMORY_H
#define MEMORY_H

#include <string>

#include "./types.h"

namespace PlimDP {

#define K 1024
#define MEMSIZE (64*K)

class Memory {
    WORD dat[K];
  public:
    BYTE memory[MEMSIZE];

    Memory();

    void load(const std::string & file);
    BYTE checkmem(DWORD index, BYTE kol);
    void writebyte(DWORD index, BYTE x);
    void writeword(DWORD index, WORD x);
    BYTE readbyte(DWORD index);
    WORD readword(DWORD index);
};
}

#endif
