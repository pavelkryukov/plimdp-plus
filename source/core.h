/*
 * core.h
 *
 * PlimDP+ core header
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#ifndef CORE_H
#define CORE_H

#include "./types.h"
#include "./bus.h"
#include "./coredump.h"
#include "./instr.h"
#include "./register.h"
#include "./disassembler.h"

namespace PlimDP {
class Core {
  private:
    friend class ISA;

    // Memory
    Bus mem;

    // Registers
    RegisterFile reg;
    BYTE re;

    // Flags
    BYTE N, Z, V, C;

    BYTE mo;
    SBYTE xx;

    // Pointers
    struct Pointer {
        DWORD index;
        enum {
            MEMORY,
            REGISTER,
        } type;
    };

    inline WORD readword(Pointer p) {
        switch (p.type) {
            case Pointer::MEMORY:
                return mem.readword(p.index);
            case Pointer::REGISTER:
                return reg.readreg(p.index);
            default:
                return 0;
        }
    }

    inline void writeword(Pointer p, WORD data) {
        switch (p.type) {
            case Pointer::MEMORY:
                mem.writeword(p.index, data);
                return;
            case Pointer::REGISTER:
                reg.writereg(p.index, data);
                return;
        }
    }

    Pointer pS, pD;

    void f_adcb();  void f_adc();  void f_add();
    void f_ash();   void f_ashc(); void f_aslb();
    void f_asl();   void f_asrb(); void f_asr();
    void f_bcc();   void f_bcs();  void f_beq();
    void f_bge();   void f_bgt();  void f_bicb();
    void f_bic();   void f_bisb(); void f_bis();
    void f_bitb();  void f_bit();  void f_bhi();
    void f_ble();   void f_blt();  void f_blos();
    void f_bmi();   void f_bne();  void f_bpl();
    void f_bpt();   void f_br();   void f_bvc();
    void f_bvs();   void f_ccc();  void f_clc();
    void f_cln();   void f_clrb(); void f_clr();
    void f_clv();   void f_clz();  void f_cmpb();
    void f_cmp();   void f_comb(); void f_com();
    void f_decb();  void f_dec();  void f_div();
    void f_emt();   void f_fadd(); void f_fdiv();
    void f_fmul();  void f_fsub(); void f_halt();
    void f_incb();  void f_inc();  void f_iot();
    void f_jmp();   void f_jsr();  void f_mark();
    void f_mfpd();  void f_mfpi(); void f_mfps();
    void f_movb();  void f_mov();  void f_mtpd();
    void f_mtpi();  void f_mtps(); void f_mul();
    void f_negb();  void f_neg();  void f_nop();
    void f_reset(); void f_rolb(); void f_rol();
    void f_rorb();  void f_ror();  void f_rti();
    void f_rts();   void f_rtt();  void f_sbcb();
    void f_sbc();   void f_scc();  void f_sec();
    void f_sen();   void f_sev();  void f_sez();
    void f_sob();   void f_spl();  void f_sub();
    void f_swab();  void f_sxt();  void f_trap();
    void f_tstb();  void f_tst();  void f_wait();
    void f_xor();

    // Decoder
    Pointer select_operand(const Instr & instr);
    BYTE decode_m(BYTE a);
    BYTE decode_r(BYTE a);
    void decode(WORD opcode, const Instr & instr);

#if ENABLE_DISASM
    // Disassembler
    friend class Disassembler;
    Disassembler* disasm;
#endif
    
    // Dump
    friend class CoreDump;
    CoreDump* dump;
  public:
    Core();
    ~Core();

    inline void setMemory(Memory* memory) {
        mem.setMemory(memory);
    }

    void start();
};
}

#endif
