/*
 * tracer.h
 *
 * PlimDP+ core trace
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#ifndef COREDUMP_H
#define COREDUMP_H

#include "./types.h"

namespace PlimDP {
class Executor;
class Tracer {
  private:
    const Executor* const parent;
  public:
    explicit Tracer(const Executor* core);

    void running();
    void core();
    void end();
};
}

#endif