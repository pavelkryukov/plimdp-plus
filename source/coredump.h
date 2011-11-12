/*
 * coredump.h
 *
 * PlimDP+ core dump
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#ifndef COREDUMP_H
#define COREDUMP_H

#include "./types.h"
#include "./instr.h"

namespace PlimDP {
class Core;

class CoreDump {
  private:
    const Core* const parent;
  public:
    explicit CoreDump(const Core* core);

    void running();
    void core();
    void end();
};
}

#endif