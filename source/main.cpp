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
    if (ac == 1) {
        std::printf("error\n");
        return 1;
    }
    PlimDP::Core pdp11;
    if (ac > 2 && !std::strcmp(av[2], "-t")) {
        pdp11.setDump();
    }
    pdp11.load(std::string(av[1]));
    pdp11.start();
    return 0;
}
