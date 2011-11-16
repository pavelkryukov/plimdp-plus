/*
 * loader.h
 * 
 * PlimDP+ file loader
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#ifndef LOADER_H
#define LOADER_H

#include <string>

#include <plimdp/generic/types.h>
#include <plimdp/devices/memory.h>

namespace PlimDP {
namespace Devices {
class Loader {
    WORD dat[K];
    MemoryA* mem;
  public:
    void load(const std::string & file);
    inline void setMemory(PlimDP::Devices::MemoryA* memory) {
        mem = memory;
    }
};
}
}

#endif
