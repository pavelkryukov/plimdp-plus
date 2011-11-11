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

#include "./core.h"

int main(int ac, char *av[]) {
    if (ac != 2) {
        std::printf("Syntax error\n");
        return 1;
    }
    
    PlimDP::Memory memory11;
    memory11.load(std::string(av[1]));

    PlimDP::Core pdp11;
    pdp11.setMemory(&memory11);
    pdp11.start();
    
    return 0;
}
