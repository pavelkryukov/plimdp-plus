/*
 * main.cpp
 * 
 * PlimDP+ launcher
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#include <cstdio>
#include <cstring>

#include <string>

#include <plimdp/cpu/core.h>
#include <plimdp/devices/loader.h>

int main(int ac, char *av[]) {
    if (ac != 2) {
        std::printf("Syntax error\n");
        return 1;
    }
   
    PlimDP::Devices::Memory memory11;

    PlimDP::CPU::Core pdp11;
    pdp11.setMemory(&memory11);

    PlimDP::Devices::Loader loader;    
    loader.setMemory(&memory11);

    loader.load(std::string(av[1]));
    pdp11.start();
    
    return 0;
}
