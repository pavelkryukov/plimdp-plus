/*
 * core.h
 *
 * PlimDP+ core header
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#ifndef CORE_H
#define CORE_H

#include <plimdp/cpu/decoder.h>

namespace PlimDP {
namespace CPU {
class Core : private Decoder {
  public:
    inline void setMemory(PlimDP::Devices::MemoryA* memory) {
        mem.setMemory(memory);
    }
    inline void setIO(PlimDP::Devices::IO* iosys) {
        mem.setIO(iosys);
    }
    void start();
};
}
}

#endif
