/*
 * isa.cpp
 *
 * PlimDP+ pdp11 instruction set
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#include "./core.h"
#include "./isa.h"
#include "./macro.h"

namespace PlimDP {
const Instr ISA::instrs[] = {
    {"adcb", 0105500, 0177700, Instr::T_DD,   &Core::f_adcb, 1},
    {"adc",  0005500, 0177700, Instr::T_DD,   &Core::f_adc,  2},
    {"add",  0060000, 0170000, Instr::T_SSDD, &Core::f_add,  2},
    {"ash",  0072000, 0177000, Instr::T_RSS,  &Core::f_ash,  2},
    {"ashc", 0073000, 0177000, Instr::T_RSS,  &Core::f_ashc, 2},
    {"aslb", 0106300, 0177700, Instr::T_DD,   &Core::f_aslb, 1},
    {"asl",  0006300, 0177700, Instr::T_DD,   &Core::f_asl,  2},
    {"asrb", 0106200, 0177700, Instr::T_DD,   &Core::f_asrb, 1},
    {"asr",  0006200, 0177700, Instr::T_DD,   &Core::f_asr,  2},
    {"bcc",  0x8600,  0xff00,  Instr::T_XX,   &Core::f_bcc,  2},
    {"bcs",  0x8700,  0xff00,  Instr::T_XX,   &Core::f_bcs,  2},
    {"beq",  0x0300,  0xff00,  Instr::T_XX,   &Core::f_beq,  2},
    {"bge",  0x0400,  0xff00,  Instr::T_XX,   &Core::f_bge,  2},
    {"bgt",  0x0600,  0xff00,  Instr::T_XX,   &Core::f_bgt,  2},
    {"bicb", 0140000, 0170000, Instr::T_SSDD, &Core::f_bicb, 1},
    {"bic",  0040000, 0170000, Instr::T_SSDD, &Core::f_bic,  2},
    {"bisb", 0150000, 0170000, Instr::T_SSDD, &Core::f_bisb, 1},
    {"bis",  0050000, 0170000, Instr::T_SSDD, &Core::f_bis,  2},
    {"bitb", 0130000, 0170000, Instr::T_SSDD, &Core::f_bitb, 1},
    {"bit",  0030000, 0170000, Instr::T_SSDD, &Core::f_bit,  2},
    {"bhi",  0x8200,  0xff00,  Instr::T_XX,   &Core::f_bhi,  2},
    {"ble",  0x0700,  0xff00,  Instr::T_XX,   &Core::f_ble,  2},
    {"blt",  0x0500,  0xff00,  Instr::T_XX,   &Core::f_blt,  2},
    {"blos", 0x8300,  0xff00,  Instr::T_XX,   &Core::f_blos, 2},
    {"bmi",  0x8100,  0xff00,  Instr::T_XX,   &Core::f_bmi,  2},
    {"bne",  0x0200,  0xff00,  Instr::T_XX,   &Core::f_bne,  2},
    {"bpl",  0x8000,  0xff00,  Instr::T_XX,   &Core::f_bpl,  2},
    {"bpt",  0000003, 0177777, Instr::T_NONE, &Core::f_bpt,  2},
    {"br",   0x0100,  0xff00,  Instr::T_XX,   &Core::f_br,   2},
    {"bvc",  0x8400,  0xff00,  Instr::T_XX,   &Core::f_bvc,  2},
    {"bvs",  0x8500,  0xff00,  Instr::T_XX,   &Core::f_bvs,  2},
    {"ccc",  0000257, 0177777, Instr::T_NONE, &Core::f_ccc,  2},
    {"clc",  0000241, 0177777, Instr::T_NONE, &Core::f_clc,  2},
    {"cln",  0000250, 0177777, Instr::T_NONE, &Core::f_cln,  2},
    {"clrb", 0105000, 0177700, Instr::T_DD,   &Core::f_clrb, 1},
    {"clr",  0005000, 0177700, Instr::T_DD,   &Core::f_clr,  2},
    {"clv",  0000242, 0177777, Instr::T_NONE, &Core::f_clv,  2},
    {"clz",  0000244, 0177777, Instr::T_NONE, &Core::f_clz,  2},
    {"cmpb", 0120000, 0170000, Instr::T_SSDD, &Core::f_cmpb, 1},
    {"cmp",  0020000, 0170000, Instr::T_SSDD, &Core::f_cmp,  2},
    {"comb", 0105100, 0177700, Instr::T_DD,   &Core::f_comb, 1},
    {"com",  0005100, 0177700, Instr::T_DD,   &Core::f_com,  2},
    {"decb", 0105300, 0177700, Instr::T_DD,   &Core::f_decb, 1},
    {"dec",  0005300, 0177700, Instr::T_DD,   &Core::f_dec,  2},
    {"div",  0071000, 0177000, Instr::T_RSS,  &Core::f_div,  2},
    {"emt",  0104000, 0177700, Instr::T_TT,   &Core::f_emt,  2},
    {"fadd", 0075000, 0177770, Instr::T_R,    &Core::f_fadd, 2},
    {"fdiv", 0075030, 0177770, Instr::T_R,    &Core::f_fdiv, 2},
    {"fmul", 0075020, 0177770, Instr::T_R,    &Core::f_fmul, 2},
    {"fsub", 0075010, 0177770, Instr::T_R,    &Core::f_fsub, 2},
    {"halt", 0000000, 0177777, Instr::T_NONE, &Core::f_halt, 2},
    {"incb", 0105200, 0177700, Instr::T_DD,   &Core::f_incb, 1},
    {"inc",  0005200, 0177700, Instr::T_DD,   &Core::f_inc,  2},
    {"iot",  0000004, 0177777, Instr::T_NONE, &Core::f_iot,  2},
    {"jmp",  0000100, 0177700, Instr::T_DD,   &Core::f_jmp,  2},
    {"jsr",  0004000, 0177000, Instr::T_RDD,  &Core::f_jsr,  2},
    {"mark", 0006400, 0177700, Instr::T_NN,   &Core::f_mark, 2},
    {"mfpd", 0106500, 0177700, Instr::T_SS,   &Core::f_mfpd, 2},
    {"mfpi", 0006500, 0177700, Instr::T_SS,   &Core::f_mfpi, 2},
    {"mfps", 0106700, 0177700, Instr::T_DD,   &Core::f_mfps, 2},
    {"movb", 0110000, 0170000, Instr::T_SSDD, &Core::f_movb, 1},
    {"mov",  0010000, 0170000, Instr::T_SSDD, &Core::f_mov,  2},
    {"mtpd", 0106600, 0177700, Instr::T_DD,   &Core::f_mtpd, 2},
    {"mtpi", 0006600, 0177700, Instr::T_DD,   &Core::f_mtpi, 2},
    {"mtps", 0106400, 0177700, Instr::T_SS,   &Core::f_mtps, 2},
    {"mul",  0070000, 0177000, Instr::T_RSS,  &Core::f_mul,  2},
    {"negb", 0105400, 0177700, Instr::T_DD,   &Core::f_negb, 1},
    {"neg",  0005400, 0177700, Instr::T_DD,   &Core::f_neg,  2},
    {"nop",  0000240, 0177777, Instr::T_NONE, &Core::f_nop,  2},
    {"reset",  0000005, 0177777, Instr::T_NONE, &Core::f_reset, 2},
    {"rolb", 0106100, 0177700, Instr::T_DD,   &Core::f_rolb, 1},
    {"rol",  0006100, 0177700, Instr::T_DD,   &Core::f_rol,  2},
    {"rorb", 0106000, 0177700, Instr::T_DD,   &Core::f_rorb, 1},
    {"ror",  0006000, 0177700, Instr::T_DD,   &Core::f_ror,  2},
    {"rti",  0000002, 0177777, Instr::T_NONE, &Core::f_rti,  2},
    {"rts",  0000200, 0177770, Instr::T_R,    &Core::f_rts,  2},
    {"rtt",  0000006, 0177777, Instr::T_NONE, &Core::f_rtt,  2},
    {"sbcb", 0105600, 0177700, Instr::T_DD,   &Core::f_sbcb, 1},
    {"sbc",  0005600, 0177700, Instr::T_DD,   &Core::f_sbc,  2},
    {"scc",  0000277, 0177777, Instr::T_NONE, &Core::f_scc,  2},
    {"sec",  0000261, 0177777, Instr::T_NONE, &Core::f_sec,  2},
    {"sen",  0000270, 0177777, Instr::T_NONE, &Core::f_sen,  2},
    {"sev",  0000262, 0177777, Instr::T_NONE, &Core::f_sev,  2},
    {"sez",  0000264, 0177777, Instr::T_NONE, &Core::f_sez,  2},
    {"sob",  0077000, 0177000, Instr::T_RNN,  &Core::f_sob,  2},
    {"spl",  0000230, 0177770, Instr::T_N,    &Core::f_spl,  2},
    {"sub",  0160000, 0170000, Instr::T_SSDD, &Core::f_sub,  2},
    {"swab", 0000300, 0177700, Instr::T_DD,   &Core::f_swab, 2},
    {"sxt",  0006700, 0177700, Instr::T_DD,   &Core::f_sxt,  2},
    {"trap", 0104400, 0177700, Instr::T_TT,   &Core::f_trap, 2},
    {"tstb", 0105700, 0177700, Instr::T_DD,   &Core::f_tstb, 1},
    {"tst",  0005700, 0177700, Instr::T_DD,   &Core::f_tst,  2},
    {"wait", 0000001, 0177777, Instr::T_NONE, &Core::f_wait, 2},
    {"xor",  0074000, 0177000, Instr::T_RDD,  &Core::f_xor,  2},
};

const size_t ISA::instrs_s = sizeof(instrs) / sizeof(instrs[0]);

Instr ISA::find_instrs(WORD opcode) {
    for (unsigned i = 0; i < ISA::instrs_s; i++)
        if ((opcode & ISA::instrs[i].mask) == ISA::instrs[i].code)
            return ISA::instrs[i];
    DIE("Such function doesn't exist");
    return instrs[0];
}
}