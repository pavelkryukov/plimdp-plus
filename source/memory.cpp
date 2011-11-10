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

#define ISTAT 0177560
#define IDATA 0177562
#define OSTAT 0177564
#define ODATA 0177566

namespace PlimDP {

Memory::Memory() {
    for (unsigned i = 0; i < MEMSIZE; ++i) {
        memory[i] = 0;
    }
}

Memory::~Memory() {
}

void Memory::load(const std::string & file) {
    std::FILE *fd;
    WORD a, b, c;
    WORD i, k = 0;
    fd = std::fopen(file.c_str(), "r");
    while (std::fscanf(fd, "%hx %hx", &a, &c) == 2) {
        dat[k++] = a;
        dat[k++] = c;
        for (i = 0; i < c; i++)
            if (std::fscanf(fd, "%hx", &b) == 1)
                writebyte(a+i, b);
    }
    std::fclose(fd);
}

bool Memory::checkmem(DWORD index, BYTE kol) const {
    if (index < MEMSIZE) {
        if (kol == 1)
            return true;
        if (kol == 2)
            return true;
    }
    std::printf("Memory check failed");
    return false;
}

void Memory::writebyte(DWORD index, BYTE x) {
    if (index == ODATA) {
        io.output(x);
    }
    if (index == IDATA || index == OSTAT || index == ISTAT) {
    }
    if (checkmem(index, 1))
        memory[index] = x;
    else
        std::printf("Error: writing byte outside the memory");
}

void Memory::writeword(DWORD index, WORD x) {
    if (index == ODATA) {
        io.output(x);
    }
    if (index == IDATA || index == OSTAT || index == ISTAT) {
    }
    if (checkmem(index, 2)) {
        WORD* ptr = reinterpret_cast<WORD*>(&memory[index]);
        *ptr = x;
    } else {
        std::printf("Error: writing word outside the memory or bad address");
    }
}
BYTE Memory::readbyte(DWORD index) const {
    if (index == IDATA || index == ISTAT || index == OSTAT || index == ODATA) {
        return 0;
    }
    if (checkmem(index, 1)) {
        return memory[index];
    } else {
        std::printf("Error: reading byte outside the memory");
        return 0;
    }
}

WORD Memory::readword(DWORD index) const {
    if (index == IDATA) {
        return io.input();
    }
    if (index == ISTAT || index == OSTAT) {
        return 0200;
    }
    if (index == ODATA) {
        return 0;
    }
    if (checkmem(index, 2)) {
        const WORD* ptr = reinterpret_cast<const WORD*>(&memory[index]);
        return *ptr;
    } else {
        std::printf("Error: reading word outside the memory or bad address");
        return 0;
    }
}

void Memory::dump() {
    for (unsigned i = 0; i < MEMSIZE; ++i) {
        std::printf("%c\n", memory[i]);
    }
}
}