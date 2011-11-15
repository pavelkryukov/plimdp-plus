/*
 * register.cpp
 * 
 * PlimDP+ register file
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#include <plimdp/generic/macro.h>
#include "./register.h"

namespace PlimDP {
namespace CPU {
RegisterFile::RegisterFile() {
    for (unsigned i = 0; i < sizeof(reg) / sizeof(reg[0]); ++i) {
        reg[i] = 0;
    }
    reg[7] = 0x200;
}

void RegisterFile::increg(DWORD index, SWORD x) {
    reg[index] += x;
}

void RegisterFile::incPC(SWORD x) {
    reg[7] += x;
}

void RegisterFile::incSP(SWORD x) {
    reg[6] += x;
}

void RegisterFile::writereg(DWORD index, WORD x) {
    reg[index] = x;
}

WORD RegisterFile::readreg(DWORD index) const {
    return reg[index];
}

void RegisterFile::writePC(WORD x) {
    reg[7] = x;
}

WORD RegisterFile::readPC() const {
    return reg[7];
}

void RegisterFile::writeSP(WORD x) {
    reg[6] = x;
}

WORD RegisterFile::readSP() const {
    return reg[6];
}
}
}