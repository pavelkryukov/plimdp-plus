/*
 * coredump.cpp
 *
 * PlimDP+ core dump
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#include <cstdio>

#include "./core.h"

namespace PlimDP {
CoreDump::CoreDump(const Core* core) : parent(core) {
}

void CoreDump::running() {
    std::printf("\n---------------- running --------------\n");
}

void CoreDump::core() {
    std::printf("\n");

    std::printf(parent->N == 1 ? "N" : "-");
    std::printf(parent->Z == 1 ? "Z" : "-");
    std::printf(parent->V == 1 ? "V" : "-");
    std::printf(parent->C == 1 ? "C" : "-");

    std::printf(" ");
    for (unsigned i = 0; i < 8; i++)
        std::printf("%hd:%06o ", i, parent->reg.readreg(i));
}

void CoreDump::end() {
    std::printf("\n---------------- halted ---------------\n");
    std::printf("r0=%06o r2=%06o r4=%06o SP=%06o\n",
                    parent->reg.readreg(0), parent->reg.readreg(2),
                    parent->reg.readreg(4), parent->reg.readSP());
    std::printf("r1=%06o r3=%06o r5=%06o PC=%06o\n",
                    parent->reg.readreg(1), parent->reg.readreg(3),
                    parent->reg.readreg(5), parent->reg.readPC());
    std::printf("n=%ho z=%ho v=%ho c=%ho\n",
                    parent->N, parent->Z, parent->V, parent->C);
}
}
