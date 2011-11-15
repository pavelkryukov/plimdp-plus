/*
 * loader.cpp
 * 
 * PlimDP+ file loader
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#include <cstdio>

#include <string>

#include "./loader.h"

namespace PlimDP {
namespace Devices {
void Loader::load(const std::string & file) {
    std::FILE *fd;
    WORD a, b, c;
    WORD i, k = 0;
    fd = std::fopen(file.c_str(), "r");
    while (std::fscanf(fd, "%hx %hx", &a, &c) == 2) {
        dat[k++] = a;
        dat[k++] = c;
        for (i = 0; i < c; i++)
            if (std::fscanf(fd, "%hx", &b) == 1)
                mem->writebyte(a+i,b);
    }
    std::fclose(fd);
}
}
}