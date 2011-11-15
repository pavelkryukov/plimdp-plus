/*
 * tracer.cpp
 *
 * PlimDP+ core trace
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#include <cstdio>

#include <plimdp/cpu/executor.h>

#include "./tracer.h"

namespace PlimDP {
namespace CPU {
Tracer::Tracer(const Executor* core) : parent(core) {
}

void Tracer::running() {
    std::printf("\n---------------- running --------------\n");
}

void Tracer::core() {
    std::printf("\n");

    std::printf(parent->flags.N ? "N" : "-");
    std::printf(parent->flags.Z ? "Z" : "-");
    std::printf(parent->flags.V ? "V" : "-");
    std::printf(parent->flags.C ? "C" : "-");

    std::printf(" ");
    for (unsigned i = 0; i < 8; i++)
        std::printf("%hd:%06o ", i, parent->reg.readreg(i));
}

void Tracer::end() {
    std::printf("\n---------------- halted ---------------\n");
    std::printf("r0=%06o r2=%06o r4=%06o SP=%06o\n",
                    parent->reg.readreg(0), parent->reg.readreg(2),
                    parent->reg.readreg(4), parent->reg.readSP());
    std::printf("r1=%06o r3=%06o r5=%06o PC=%06o\n",
                    parent->reg.readreg(1), parent->reg.readreg(3),
                    parent->reg.readreg(5), parent->reg.readPC());
    std::printf("n=%ho z=%ho v=%ho c=%ho\n",
                    parent->flags.N, parent->flags.Z,
                    parent->flags.V, parent->flags.C);
}
}
}