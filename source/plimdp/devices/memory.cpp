/*
 * memory.cpp
 * 
 * PlimDP+ memory subsystem
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#include <cstdio>

#include <string>

#include <plimdp/generic/macro.h>

#include "./memory.h"

namespace PlimDP {
namespace Devices {
void Memory::writebyte(DWORD index, BYTE x) {
    if (checkmem(index))
        memory[index] = x;
    else
        DIE("Error: writing byte outside the memory");
}

void Memory::writeword(DWORD index, WORD x) {
    if (checkmem(index)) {
        WORD* ptr = reinterpret_cast<WORD*>(&memory[index]);
        *ptr = x;
    } else {
        DIE("Error: writing word outside the memory or bad address");
    }
}

BYTE Memory::readbyte(DWORD index) const {
    if (checkmem(index)) {
        return memory[index];
    } else {
        DIE("Error: reading byte outside the memory");
        return 0;
    }
}

WORD Memory::readword(DWORD index) const {
    if (checkmem(index)) {
        const WORD* ptr = reinterpret_cast<const WORD*>(&memory[index]);
        return *ptr;
    } else {
        DIE("Error: reading word outside the memory or bad address");
        return 0;
    }
}

void Memory::dump() {
    for (unsigned i = 0; i < MEMSIZE; ++i) {
        std::printf("%ho\n ", memory[i]);
    }
}
}
}