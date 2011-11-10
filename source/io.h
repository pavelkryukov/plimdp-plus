/*
 * io.h
 * 
 * PlimDP++ input-output subsystem
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#ifndef IO_H
#define IO_H

#include <cstdio>

#include "./types.h"

namespace PlimDP {
class IO {
  public:
    inline BYTE input() const {
        return getchar();
    }
    inline void output(BYTE x) const {
        std::printf("%c", x);
    }
};
}

#endif
