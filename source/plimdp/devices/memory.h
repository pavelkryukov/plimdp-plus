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

#include <plimdp/generic/types.h>

namespace PlimDP {
namespace Devices {

#define K 1024
#define MEMSIZE (64*K)

class Memory {
    BYTE memory[MEMSIZE];

    inline bool checkmem(DWORD index) const {
        return (index < MEMSIZE);
    }
  public:    
    void writebyte(DWORD index, BYTE x);
    BYTE readbyte(DWORD index) const;
    
    void writeword(DWORD index, WORD x);
    WORD readword(DWORD index) const;
    
    void dump();
};
}
}

#endif
