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
#include <plimdp/generic/macro.h>

namespace PlimDP {
namespace Devices {

#define K 1024

class MemoryA {
  public:    
    virtual void writebyte(DWORD index, BYTE x) = 0;
    virtual BYTE readbyte(DWORD index) const = 0;
    
    virtual void writeword(DWORD index, WORD x) = 0;
    virtual WORD readword(DWORD index) const = 0;
    
    virtual void dump() = 0;
};
    
template<unsigned MEMSIZE>
class Memory : public MemoryA {
  private:
    BYTE memory[MEMSIZE];

    inline bool checkmem(DWORD index) const {
        return (index < MEMSIZE);
    }
  public:    
    virtual void writebyte(DWORD index, BYTE x);
    virtual BYTE readbyte(DWORD index) const;
    
    virtual void writeword(DWORD index, WORD x);
    virtual WORD readword(DWORD index) const;
    
    virtual void dump();
};

template<unsigned MEMSIZE>
void Memory<MEMSIZE>::writebyte(DWORD index, BYTE x) {
    if (checkmem(index))
        memory[index] = x;
    else
        DIE("Error: writing byte outside the memory");
}

template<unsigned MEMSIZE>
void Memory<MEMSIZE>::writeword(DWORD index, WORD x) {
    if (checkmem(index)) {
        WORD* ptr = reinterpret_cast<WORD*>(&memory[index]);
        *ptr = x;
    } else {
        DIE("Error: writing word outside the memory or bad address");
    }
}

template<unsigned MEMSIZE>
BYTE Memory<MEMSIZE>::readbyte(DWORD index) const {
    if (checkmem(index)) {
        return memory[index];
    } else {
        DIE("Error: reading byte outside the memory");
        return 0;
    }
}

template<unsigned MEMSIZE>
WORD Memory<MEMSIZE>::readword(DWORD index) const {
    if (checkmem(index)) {
        const WORD* ptr = reinterpret_cast<const WORD*>(&memory[index]);
        return *ptr;
    } else {
        DIE("Error: reading word outside the memory or bad address");
        return 0;
    }
}

template<unsigned MEMSIZE>
void Memory<MEMSIZE>::dump() {
    for (unsigned i = 0; i < MEMSIZE; ++i) {
        std::printf("%ho\n ", memory[i]);
    }
}
}
}

#endif
