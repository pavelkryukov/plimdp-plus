/*
 * disassembler.cpp
 *
 * PlimDP+ core disassembly print
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#include <cstdio>

#include "./core.h"

namespace PlimDP {
Disassembler::Disassembler(const Core* core) : parent(core) {
}

void Disassembler::mn(const Instr & instr) {
    std::printf("\n%06o: ", parent->reg.readPC() - 2);
    std::printf("%s\t", instr.name.c_str());
    countfrsp = 24;
}
void Disassembler::reg(WORD opcode, WORD oldPC) {
    WORD temp1, temp2;
    char frsp = ' ';
    std::printf("%*c", countfrsp, frsp);
    if (pcsmflag == 0) {
        std::printf("[%06o]", opcode);
    } else if (pcsmflag == 1) {
        temp1 = parent->mem.readword(oldPC - 2);
        std::printf("[%06o %06o]", opcode, temp1);
    } else if (pcsmflag == 2) {
        temp2 = parent->mem.readword(oldPC - 2);
        std::printf("[%06o %06o]", opcode, temp2);
    } else if (pcsmflag == 3) {
        temp1 = parent->mem.readword(oldPC - 4);
        temp2 = parent->mem.readword(oldPC - 2);
        std::printf("[%06o %06o %06o]", opcode, temp1, temp2);
    }
    pcsmflag = pcsmcnt = 0;
}

void Disassembler::op() {
    WORD nt_cm;
    nt_cm = parent->mem.readword(parent->reg.readPC());
    pcsmcnt++;
    switch (parent->mo) {
        case 0:
            std::printf("r%ho", parent->re);
            countfrsp -= 2;
            break;
        case 1:
            std::printf("(r%ho)", parent->re);
            countfrsp -= 4;
            break;
        case 2:
            if (parent->re == 7) {
                std::printf("#%06ho", nt_cm);
                countfrsp -= 7;
                if (pcsmcnt == 1)
                    pcsmflag++;
                else
                    pcsmflag += 2;
            } else {
                std::printf("(r%ho)+", parent->re);
                countfrsp -= 5;
            }
            break;
        case 3:
            if (parent->re == 7) {
                std::printf("@#%06ho", nt_cm);
                countfrsp -= 8;
                if (pcsmcnt == 1)
                    pcsmflag++;
                else
                    pcsmflag += 2;
            } else {
                std::printf("@(r%ho)+", parent->re);
                countfrsp -= 6;
            }
            break;
        case 4:
            std::printf("-(r%ho)", parent->re);
            countfrsp -= 5;
            break;
        case 5:
            std::printf("@-(r%ho)", parent->re);
            countfrsp -= 6;
            break;
        case 6:
            if (parent->re == 7) {
                std::printf("%06ho", nt_cm + parent->reg.readPC() + 2);
                countfrsp -= 6;
                if (pcsmcnt == 1)
                    pcsmflag++;
                else
                    pcsmflag += 2;
            } else {
                std::printf("%06ho(r%ho)", nt_cm, parent->re);
                countfrsp -= 10;
                if (pcsmcnt == 1)
                    pcsmflag++;
                else
                    pcsmflag += 2;
            }
            break;
        case 7:
            if (parent->re == 7) {
                std::printf("@%06ho", nt_cm + parent->reg.readPC() + 2);
                countfrsp -= 7;
                if (pcsmcnt == 1)
                    pcsmflag++;
                else
                    pcsmflag += 2;
            } else {
                std::printf("@%06ho(r%ho)", nt_cm, parent->re);
                countfrsp -= 11;
                if (pcsmcnt == 1)
                    pcsmflag++;
                else
                    pcsmflag += 2;
            }
            break;
    }
}

void Disassembler::comma() {
    std::printf(",");
    countfrsp -= 1;
}

void Disassembler::aim() {
    std::printf("%06o", parent->reg.readPC() + 2*parent->xx);
    countfrsp -= 6;
}
}
