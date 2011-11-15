/*
 * register.h
 *
 * PlimDP+ core register file
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#ifndef REGISTER_H
#define REGISTER_H

#include "./types.h"

namespace PlimDP {
namespace CPU {
class RegisterFile {
    WORD reg[8];
  public:
    RegisterFile();
  
    void increg(DWORD index, SWORD x);
    void incPC(SWORD x);
    void incSP(SWORD x);
  
    void writereg(DWORD index, WORD x);
    WORD readreg(DWORD index) const;
    
    void writePC(WORD x);
    WORD readPC() const;
    
    void writeSP(WORD x);
    WORD readSP() const;
};
}
}

#endif
