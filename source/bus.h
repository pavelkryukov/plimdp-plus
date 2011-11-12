/*
 * bus.h
 *
 * PlimDP+ core-memory bus
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#ifndef BUS_H
#define BUS_H

#include "./types.h"
#include "./io.h"
#include "./memory.h"

namespace PlimDP {
class Bus {
    static const DWORD ISTAT = 0177560;
    static const DWORD IDATA = 0177562;
    static const DWORD OSTAT = 0177564;
    static const DWORD ODATA = 0177566;
    IO io;
    Memory* mem;
  public:    
    inline void setMemory(Memory* memory) {
        mem = memory;
    }

    void writeword(DWORD index, WORD x);
    WORD readword(DWORD index) const;
};
}

#endif