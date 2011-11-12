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
#include "./io.h"

namespace PlimDP {

#define K 1024
#define MEMSIZE (64*K)

class Memory {
    WORD dat[K];
    BYTE memory[MEMSIZE];

    bool checkmem(DWORD index, BYTE kol) const;
    BYTE readbyte(DWORD index) const;
    void writebyte(DWORD index, BYTE x);
  public:

    Memory();
    ~Memory();

    void load(const std::string & file);
    void writeword(DWORD index, WORD x);
    WORD readword(DWORD index) const;
    void dump();
};
}

#endif
