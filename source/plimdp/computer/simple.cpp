/*
 * simple.cpp
 * 
 * PlimDP+ simple computer
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#include "./simple.h"

#include <plimdp/cpu/core.h>
#include <plimdp/devices/memory.h>
#include <plimdp/devices/loader.h>

namespace PlimDP {
namespace Computer {

Simple::Simple() {
    core = new PlimDP::CPU::Core();
    mem = new PlimDP::Devices::Memory<64 * K>();
    loader = new PlimDP::Devices::Loader();
    io = new PlimDP::Devices::IO();
    
    core->setMemory(mem);
    core->setIO(io);
    loader->setMemory(mem);
}

Simple::~Simple() {
    delete core;
    delete mem;
    delete loader;
    delete io;
}
  
void Simple::load(const std::string & file) {
    loader->load(file);
}

void Simple::start() {
    core->start();
}
}
}