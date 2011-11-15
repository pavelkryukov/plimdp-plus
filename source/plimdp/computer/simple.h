/*
 * simple.h
 * 
 * PlimDP+ simple computer
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#ifndef SIMPLE_H
#define SIMPLE_H

#include <plimdp/cpu/core.h>
#include <plimdp/devices/memory.h>
#include <plimdp/devices/loader.h>

namespace PlimDP {
namespace Computer {
class Simple {
  private:
    PlimDP::CPU::Core core;
    PlimDP::Devices::Memory mem;
    PlimDP::Devices::Loader loader;
  public:
    inline Simple() {
        core.setMemory(&mem);
        loader.setMemory(&mem);
    }
  
    inline void load(const std::string & file) {
        loader.load(file);
    }
    inline void start() {
        core.start();
    }
};
}
}

#endif
