/*
 * coredump.cpp
 * 
 * PlimDP+ core dump
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#include <cstdio>

#include "./core.h"

#define PC (parent->reg[7])
#define SP (parent->reg[6])

namespace PlimDP {
CoreDump::CoreDump(const Core* core) : parent(core) {
}

void CoreDump::mn() {
    std::printf("\n%06o: ", PC - 2);
    std::printf("%s\t", parent->instrs[parent->idx].name.c_str());
    countfrsp = 24;
}
void CoreDump::reg() {
    BYTE i;
    WORD temp1, temp2;
    char frsp = ' ';
    std::printf("%*c", countfrsp, frsp);
    if (pcsmflag == 0) {
        std::printf("[%06o]\n", parent->opcode);
    } else if (pcsmflag == 1) {
        temp1 = parent->mem->readword(parent->oldPC - 2);
        std::printf("[%06o %06o]\n", parent->opcode, temp1);
    } else if (pcsmflag == 2) {
        temp2 = parent->mem->readword(parent->oldPC - 2);
        std::printf("[%06o %06o]\n", parent->opcode, temp2);
    } else if (pcsmflag == 3) {
        temp1 = parent->mem->readword(parent->oldPC - 4);
        temp2 = parent->mem->readword(parent->oldPC - 2);
        std::printf("[%06o %06o %06o]\n", parent->opcode, temp1, temp2);
    }
    pcsmflag = pcsmcnt = 0;
    
    std::printf(parent->N == 1 ? "N" : "-");
    std::printf(parent->Z == 1 ? "Z" : "-");
    std::printf(parent->V == 1 ? "V" : "-");
    std::printf(parent->C == 1 ? "C" : "-");
                
    std::printf(" ");
    for (i = 0; i < 8; i++)
        std::printf("%hd:%06o ", i, parent->reg[i]);
}
void CoreDump::op() {
    WORD nt_cm;
    nt_cm = parent->mem->readword(PC);
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
                std::printf("%06ho", nt_cm + PC + 2);
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
                std::printf("@%06ho", nt_cm + PC + 2);
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

void CoreDump::comma() {
    std::printf(",");
    countfrsp -= 1;
}

void CoreDump::aim() {
    std::printf("%06o", PC + 2*parent->xx);
    countfrsp -= 6;
}
void CoreDump::end() {
    std::printf("r0=%06o r2=%06o r4=%06o sp=%06o\n",
                    parent->reg[0], parent->reg[2], parent->reg[4], SP);
    std::printf("r1=%06o r3=%06o r4=%06o pc=%06o\n",
                    parent->reg[1], parent->reg[3], parent->reg[5], PC);
    std::printf("n=%ho z=%ho v=%ho c=%ho\n",
                    parent->N, parent->Z, parent->V, parent->C);
}
}