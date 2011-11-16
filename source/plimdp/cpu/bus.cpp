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

#include <plimdp/generic/macro.h>

#include "./bus.h"

namespace PlimDP {
namespace CPU {
void Bus::writeword(DWORD index, WORD x) {
    if (io != NULL)  {
        if (index == ODATA) {
            io->output(x);
            return;
        }
        if (index == IDATA || index == OSTAT || index == ISTAT) {
            DIE("Incorrect work with I/O system");
            return;
        }
    }
    if (mem != NULL) {
        mem->writeword(index, x);
        return;
    }
    DIE("Bus is not connected to devices");
}

WORD Bus::readword(DWORD index) const {
    if (io != NULL)  {
        if (index == IDATA) {
            return io->input();
        }
        if (index == ISTAT || index == OSTAT) {
            return 0200;
        }
        if (index == ODATA) {
            return 0;
        }
    }
    if (mem != NULL) {
        return mem->readword(index);
    }
    DIE("Bus is not connected to devices");
    return 0;
}
}
}