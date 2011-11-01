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

#include <string>

#include "./types.h"
#include "./memory.h"

namespace PlimDP {

class Core {
  private:
    Memory* const mem;

//GLOBAL****************************************************************
    WORD opcode, reg[8], oldPC;
    BYTE dd, ss, mo, re, last_mo;
    SBYTE xx;
    BYTE N, Z, V, C;
    BYTE idx, countfrsp;
    WORD *ptrD, *ptrS;
    BYTE pcsmflag, pcsmcnt;
    

//STRUCTURE*************************************************************
    static struct Instruction {
		std::string name;
        WORD code;
        WORD mask;
        InstrType type;
        void (Core::*exec)();
        BYTE size;
    } instrs[];
    static size_t instrs_s;

//SPECIAL FUNCTIONS*****************************************************
    void f_adcb();
    void f_adc();
    void f_add();
    void f_ash();
    void f_ashc();
    void f_aslb();
    void f_asl();
    void f_asrb();
    void f_asr();
    void f_bcc();
    void f_bcs();
    void f_beq();
    void f_bge();
    void f_bgt();
    void f_bicb();
    void f_bic();
    void f_bisb();
    void f_bis();
    void f_bitb();
    void f_bit();
    void f_bhi();
    void f_ble();
    void f_blt();
    void f_blos();
    void f_bmi();
    void f_bne();
    void f_bpl();
    void f_bpt();
    void f_br();
    void f_bvc();
    void f_bvs();
    void f_ccc();
    void f_clc();
    void f_cln();
    void f_clrb();
    void f_clr();
    void f_clv();
    void f_clz();
    void f_cmpb();
    void f_cmp();
    void f_comb();
    void f_com();
    void f_decb();
    void f_dec();
    void f_div();
    void f_emt();
    void f_fadd();
    void f_fdiv();
    void f_fmul();
    void f_fsub();
    void f_halt();
    void f_incb();
    void f_inc();
    void f_iot();
    void f_jmp();
    void f_jsr();
    void f_mark();
    void f_mfpd();
    void f_mfpi();
    void f_mfps();
    void f_movb();
    void f_mov();
    void f_mtpd();
    void f_mtpi();
    void f_mtps();
    void f_mul();
    void f_negb();
    void f_neg();
    void f_nop();
    void f_reset();
    void f_rolb();
    void f_rol();
    void f_rorb();
    void f_ror();
    void f_rti();
    void f_rts();
    void f_rtt();
    void f_sbcb();
    void f_sbc();
    void f_scc();
    void f_sec();
    void f_sen();
    void f_sev();
    void f_sez();
    void f_sob();
    void f_spl();
    void f_sub();
    void f_swab();
    void f_sxt();
    void f_trap();
    void f_tstb();
    void f_tst();
    void f_wait();
    void f_xor();

//PRINTER***************************************************************
    void print_mn();
    void print_reg();
    void print_op();
    void print_aim();
    void print_end();
//DECODER***************************************************************
    void select_operand(Ident ident);
    BYTE decode_m(BYTE a);
    BYTE decode_r(BYTE a);
    void decode(KeyRW mode);
//MAIN_FUNCTIONS********************************************************
    BYTE find_instrs();
    void prep_devices();
    void output();
  public:
    Core();
    ~Core();
//STARTER***************************************************************
    inline void load(const std::string & file) {
        mem->load(file);
    }
    void start(KeyRW keyRW);
};

}

#endif
