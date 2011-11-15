/*
 * executor.h
 *
 * PlimDP+ executor
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "./types.h"
#include "./bus.h"
#include "./register.h"
#include "./flags.h"

#if ENABLE_TRACE
#include "./tracer.h"
#endif

namespace PlimDP {
namespace CPU {
class Executor {
   private:
    // Flags
    Flags flags;
   protected:
    // Memory
    Bus mem;

    // Registers
    RegisterFile reg;

    // Operands
    struct Operand {
        DWORD index;
        enum {
            MEMORY,
            REGISTER,
        } type;
    };

    inline WORD readword(Operand p) {
        switch (p.type) {
            case Operand::MEMORY:
                return mem.readword(p.index);
            case Operand::REGISTER:
                return reg.readreg(p.index);
            default:
                return 0;
        }
    }

    inline void writeword(Operand p, WORD data) {
        switch (p.type) {
            case Operand::MEMORY:
                mem.writeword(p.index, data);
                return;
            case Operand::REGISTER:
                reg.writereg(p.index, data);
                return;
        }
    }
    
    struct BranchOperand {
        SBYTE value;
    };
    
    inline void branch(BranchOperand x) {
        reg.incPC(2 * x.value);
    }

    Operand pS, pD;
    BranchOperand xx;

    // ISA
    friend class ISA;
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

    // Trace
#if ENABLE_TRACE
    friend class Tracer;
    Tracer* tracer;
#endif
    
    // Constructors
    Executor();
    ~Executor();
};
}
}

#endif
