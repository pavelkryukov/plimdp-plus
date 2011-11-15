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

#include <plimdp/computer/simple.h>

int main(int ac, char *av[]) {
    if (ac != 2) {
        std::printf("Syntax error\n");
        return 1;
    }

    PlimDP::Computer::Simple pdp11;
    
    pdp11.load(std::string(av[1]));
    pdp11.start();
    
    return 0;
}
