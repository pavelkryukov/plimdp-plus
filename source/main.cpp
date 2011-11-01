/*
 * main.cpp
 * 
 * PlimDP+ launcher
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#include <cstring>

#include <iostream>

#include "./core.h"

int main(int ac, char *av[]) {
    PlimDP::Core pdp11;
    if(ac == 1)
        std::cout << "error" << std::endl;
    else {
        pdp11.load(std::string(av[1]));
        pdp11.start((ac > 2 && !std::strcmp(av[2], "-t")) ? 
                PlimDP::WRITE : 
                PlimDP::READ);
    }
    return 0;
}
