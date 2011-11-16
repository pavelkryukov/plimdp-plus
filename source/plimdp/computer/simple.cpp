/*
 * simple.cpp
 * 
 * PlimDP+ simple computer
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#include "./simple.h"

namespace PlimDP {
namespace Computer {

Simple::Simple() {
    core.setMemory(&mem);
    core.setIO(&io);
    loader.setMemory(&mem);
}
  
void Simple::load(const std::string & file) {
    loader.load(file);
}

void Simple::start() {
    core.start();
}
}
}