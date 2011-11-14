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

namespace PlimDP {
class Executor;
class CoreDump {
  private:
    const Executor* const parent;
  public:
    explicit CoreDump(const Executor* core);

    void running();
    void core();
    void end();
};
}

#endif