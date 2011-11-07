/*
 * memory.cpp
 * 
 * PlimDP+ memory subsystem
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#include <cstdio>

#include <string>

#include "./memory.h"

namespace PlimDP {

Memory::Memory() {
}

void Memory::load(const std::string & file) {
    std::FILE *fd;
    BYTE b;
    WORD a, c;
    WORD i, k = 0;
    fd = std::fopen(file.c_str(), "r");
    while (std::fscanf(fd, "%hx %hx", &a, &c) == 2) {
        dat[k++] = a;
        dat[k++] = c;
        for (i = 0; i < c; i++)
            if (std::fscanf(fd, "%hx", reinterpret_cast<WORD*>(&b)) == 1)
                writebyte(a+i, b);
    }
    fclose(fd);
}

BYTE Memory::checkmem(DWORD index, BYTE kol) {
    if (index < MEMSIZE) {
        if (kol == 1)
            return 1;
        if (kol == 2 && index % 2 == 0)
            return 1;
    }
    std::printf("Memory check failed");
    return 0;
}

void Memory::writebyte(DWORD index, BYTE x) {
    if (checkmem(index, 1))
        memory[index] = x;
    else
        std::printf("Error: writing byte outside the memory");
}

void Memory::writeword(DWORD index, WORD x) {
    if (checkmem(index, 2)) {
        WORD* ptr = reinterpret_cast<WORD*>(&memory[index]);
        *ptr = x;
    } else {
        std::printf("Error: writing word outside the memory or bad address");
    }
}
BYTE Memory::readbyte(DWORD index) {
    if (checkmem(index, 1)) {
        return memory[index];
    } else {
        std::printf("Error: reading byte outside the memory");
        return 0;
    }
}

WORD Memory::readword(DWORD index) {
    if (checkmem(index, 2)) {
        WORD* ptr = reinterpret_cast<WORD*>(&memory[index]);
        return *ptr;
    } else {
        std::printf("Error: reading word outside the memory or bad address");
        return 0;
    }
}
}
