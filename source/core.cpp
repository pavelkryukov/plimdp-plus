/*
 * core.cpp
 *
 * PlimDP+ core
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/


#include "./macro.h"

#include "./types.h"
#include "./isa.h"
#include "./instr.h"
#include "./core.h"

#if ENABLE_TRACE
    #define TRACE(OPERATIONS) OPERATIONS
#else
    #define TRACE(OPERATIONS)
#endif

namespace PlimDP {
void Core::start() {
    this->dump->running();
    WORD opcode;
    do {
        // Fetch
        opcode = this->mem.readword(this->reg.readPC());        //
        this->reg.incPC(2);            //

        // Decode
        const Instr instr = ISA::find_instrs(opcode);        //
        this->decode(opcode, instr);     

        // Execute
        (this->*(instr.exec))();            //

        TRACE( this->dump->core(); )
    }  while (opcode);
    this->dump->end();
}
}  // namespace PlimDP
