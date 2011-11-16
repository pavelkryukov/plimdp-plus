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

#include <plimdp/generic/types.h>
#include <plimdp/devices/io.h>
#include <plimdp/devices/memory.h>

namespace PlimDP {
namespace CPU {
class Bus {
    static const DWORD ISTAT = 0177560;
    static const DWORD IDATA = 0177562;
    static const DWORD OSTAT = 0177564;
    static const DWORD ODATA = 0177566;
    PlimDP::Devices::IO* io;
    PlimDP::Devices::MemoryA* mem;
  public:
    inline Bus() {
        io = NULL;
        mem = NULL;
    }

    inline void setMemory(PlimDP::Devices::MemoryA* memory) {
        mem = memory;
    }

    inline void setIO(PlimDP::Devices::IO* inputoutput) {
        io = inputoutput;
    }

    void writeword(DWORD index, WORD x);
    WORD readword(DWORD index) const;
};
}
}

#endif