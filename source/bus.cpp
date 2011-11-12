/*
 * bus.cpp
 *
 * PlimDP+ core-memory bus
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#include <cstdio>

#include <string>

#include "./bus.h"
#include "./macro.h"

namespace PlimDP {
void Bus::writeword(DWORD index, WORD x) {
    if (index == ODATA) {
        io.output(x);
    }
    if (index == IDATA || index == OSTAT || index == ISTAT) {
        DIE("Incorrect work with I/O system");
    }
    mem->writeword(index, x);
}

WORD Bus::readword(DWORD index) const {
    if (index == IDATA) {
        return io.input();
    }
    if (index == ISTAT || index == OSTAT) {
        return 0200;
    }
    if (index == ODATA) {
        return 0;
    }
    return mem->readword(index);
}
}
