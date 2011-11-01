/*
 * core.cpp
 * 
 * PlimDP+ core
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#include <cstdio>
#include <cstring>

#include "./core.h"

namespace PlimDP {

#define PC (reg[7])
#define SP (reg[6])

#define ISTAT 0177560
#define IDATA 0177562
#define OSTAT 0177564
#define ODATA 0177566

Core::Instruction Core::instrs[] = {
    {"adcb", 0105500, 0177700, T_DD,   &Core::f_adcb, 1},
    {"adc",  0005500, 0177700, T_DD,   &Core::f_adc,  2},
    {"add",  0060000, 0170000, T_SSDD, &Core::f_add,  2},
    {"ash",  0072000, 0177000, T_RSS,  &Core::f_ash,  2},
    {"ashc", 0073000, 0177000, T_RSS,  &Core::f_ashc, 2},
    {"aslb", 0106300, 0177700, T_DD,   &Core::f_aslb, 1},
    {"asl",  0006300, 0177700, T_DD,   &Core::f_asl,  2},
    {"asrb", 0106200, 0177700, T_DD,   &Core::f_asrb, 1},
    {"asr",  0006200, 0177700, T_DD,   &Core::f_asr,  2},
    {"bcc",  0x8600,  0xff00,  T_XX,   &Core::f_bcc,  2},
    {"bcs",  0x8700,  0xff00,  T_XX,   &Core::f_bcs,  2},
    {"beq",  0x0300,  0xff00,  T_XX,   &Core::f_beq,  2},
    {"bge",  0x0400,  0xff00,  T_XX,   &Core::f_bge,  2},
    {"bgt",  0x0600,  0xff00,  T_XX,   &Core::f_bgt,  2},
    {"bicb", 0140000, 0170000, T_SSDD, &Core::f_bicb, 1},
    {"bic",  0040000, 0170000, T_SSDD, &Core::f_bic,  2},
    {"bisb", 0150000, 0170000, T_SSDD, &Core::f_bisb, 1},
    {"bis",  0050000, 0170000, T_SSDD, &Core::f_bis,  2},
    {"bitb", 0130000, 0170000, T_SSDD, &Core::f_bitb, 1},
    {"bit",  0030000, 0170000, T_SSDD, &Core::f_bit,  2},
    {"bhi",  0x8200,  0xff00,  T_XX,   &Core::f_bhi,  2},
    {"ble",  0x0700,  0xff00,  T_XX,   &Core::f_ble,  2},
    {"blt",  0x0500,  0xff00,  T_XX,   &Core::f_blt,  2},
    {"blos", 0x8300,  0xff00,  T_XX,   &Core::f_blos, 2},
    {"bmi",  0x8100,  0xff00,  T_XX,   &Core::f_bmi,  2},
    {"bne",  0x0200,  0xff00,  T_XX,   &Core::f_bne,  2},
    {"bpl",  0x8000,  0xff00,  T_XX,   &Core::f_bpl,  2},
    {"bpt",  0000003, 0177777, T_NONE, &Core::f_bpt,  2},
    {"br",   0x0100,  0xff00,  T_XX,   &Core::f_br,   2},
    {"bvc",  0x8400,  0xff00,  T_XX,   &Core::f_bvc,  2},
    {"bvs",  0x8500,  0xff00,  T_XX,   &Core::f_bvs,  2},
    {"ccc",  0000257, 0177777, T_NONE, &Core::f_ccc,  2},
    {"clc",  0000241, 0177777, T_NONE, &Core::f_clc,  2},
    {"cln",  0000250, 0177777, T_NONE, &Core::f_cln,  2},
    {"clrb", 0105000, 0177700, T_DD,   &Core::f_clrb, 1},
    {"clr",  0005000, 0177700, T_DD,   &Core::f_clr,  2},
    {"clv",  0000242, 0177777, T_NONE, &Core::f_clv,  2},
    {"clz",  0000244, 0177777, T_NONE, &Core::f_clz,  2},
    {"cmpb", 0120000, 0170000, T_SSDD, &Core::f_cmpb, 1},
    {"cmp",  0020000, 0170000, T_SSDD, &Core::f_cmp,  2},
    {"comb", 0105100, 0177700, T_DD,   &Core::f_comb, 1},
    {"com",  0005100, 0177700, T_DD,   &Core::f_com,  2},
    {"decb", 0105300, 0177700, T_DD,   &Core::f_decb, 1},
    {"dec",  0005300, 0177700, T_DD,   &Core::f_dec,  2},
    {"div",  0071000, 0177000, T_RSS,  &Core::f_div,  2},
    {"emt",  0104000, 0177700, T_TT,   &Core::f_emt,  2},
    {"fadd", 0075000, 0177770, T_R,    &Core::f_fadd, 2},
    {"fdiv", 0075030, 0177770, T_R,    &Core::f_fdiv, 2},
    {"fmul", 0075020, 0177770, T_R,    &Core::f_fmul, 2},
    {"fsub", 0075010, 0177770, T_R,    &Core::f_fsub, 2},
    {"halt", 0000000, 0177777, T_NONE, &Core::f_halt, 2},
    {"incb", 0105200, 0177700, T_DD,   &Core::f_incb, 1},
    {"inc",  0005200, 0177700, T_DD,   &Core::f_inc,  2},
    {"iot",  0000004, 0177777, T_NONE, &Core::f_iot,  2},
    {"jmp",  0000100, 0177700, T_DD,   &Core::f_jmp,  2},
    {"jsr",  0004000, 0177000, T_RDD,  &Core::f_jsr,  2},
    {"mark", 0006400, 0177700, T_NN,   &Core::f_mark, 2},
    {"mfpd", 0106500, 0177700, T_SS,   &Core::f_mfpd, 2},
    {"mfpi", 0006500, 0177700, T_SS,   &Core::f_mfpi, 2},
    {"mfps", 0106700, 0177700, T_DD,   &Core::f_mfps, 2},
    {"movb", 0110000, 0170000, T_SSDD, &Core::f_movb, 1},
    {"mov",  0010000, 0170000, T_SSDD, &Core::f_mov,  2},
    {"mtpd", 0106600, 0177700, T_DD,   &Core::f_mtpd, 2},
    {"mtpi", 0006600, 0177700, T_DD,   &Core::f_mtpi, 2},
    {"mtps", 0106400, 0177700, T_SS,   &Core::f_mtps, 2},
    {"mul",  0070000, 0177000, T_RSS,  &Core::f_mul,  2},
    {"negb", 0105400, 0177700, T_DD,   &Core::f_negb, 1},
    {"neg",  0005400, 0177700, T_DD,   &Core::f_neg,  2},
    {"nop",  0000240, 0177777, T_NONE, &Core::f_nop,  2},
    {"reset",  0000005, 0177777, T_NONE, &Core::f_reset, 2},
    {"rolb", 0106100, 0177700, T_DD,   &Core::f_rolb, 1},
    {"rol",  0006100, 0177700, T_DD,   &Core::f_rol,  2},
    {"rorb", 0106000, 0177700, T_DD,   &Core::f_rorb, 1},
    {"ror",  0006000, 0177700, T_DD,   &Core::f_ror,  2},
    {"rti",  0000002, 0177777, T_NONE, &Core::f_rti,  2},
    {"rts",  0000200, 0177770, T_R,    &Core::f_rts,  2},
    {"rtt",  0000006, 0177777, T_NONE, &Core::f_rtt,  2},
    {"sbcb", 0105600, 0177700, T_DD,   &Core::f_sbcb, 1},
    {"sbc",  0005600, 0177700, T_DD,   &Core::f_sbc,  2},
    {"scc",  0000277, 0177777, T_NONE, &Core::f_scc,  2},
    {"sec",  0000261, 0177777, T_NONE, &Core::f_sec,  2},
    {"sen",  0000270, 0177777, T_NONE, &Core::f_sen,  2},
    {"sev",  0000262, 0177777, T_NONE, &Core::f_sev,  2},
    {"sez",  0000264, 0177777, T_NONE, &Core::f_sez,  2},
    {"sob",  0077000, 0177000, T_RNN,  &Core::f_sob,  2},
    {"spl",  0000230, 0177770, T_N,    &Core::f_spl,  2},
    {"sub",  0160000, 0170000, T_SSDD, &Core::f_sub,  2},
    {"swab", 0000300, 0177700, T_DD,   &Core::f_swab, 2},
    {"sxt",  0006700, 0177700, T_DD,   &Core::f_sxt,  2},
    {"trap", 0104400, 0177700, T_TT,   &Core::f_trap, 2},
    {"tstb", 0105700, 0177700, T_DD,   &Core::f_tstb, 1},
    {"tst",  0005700, 0177700, T_DD,   &Core::f_tst,  2},
    {"wait", 0000001, 0177777, T_NONE, &Core::f_wait, 2},
    {"xor",  0074000, 0177000, T_RDD,  &Core::f_xor,  2},
};

size_t Core::instrs_s = sizeof(instrs) / sizeof(instrs[0]);

//SPECIAL FUNCTIONS*****************************************************
void Core::f_adcb() {
    BYTE result;
    BYTE temp;
    temp = (BYTE) (*ptrD & 0xff);
    result = temp + C;
    
    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SBYTE) temp > 0) && ((SBYTE) result < 0);
    C = (((WORD) temp + C) >> 8) & 1;
    
    *ptrD = (*ptrD & 0xff00) | (WORD) result;
}
void Core::f_adc() {
    DWORD temp;
    WORD result;
    temp = (DWORD) *ptrD + C;
    result = (WORD) temp;
    
    N = (temp >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SWORD) *ptrD > 0) && ((SWORD) result < 0);
    C = (temp >> 16) & 1;
    
    *ptrD = (WORD) temp;
}
void Core::f_add() {
    DWORD temp;
    WORD result;
    temp = (DWORD) *ptrD + (DWORD) *ptrS;
    result = (WORD) temp;
    
    N = (temp >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SWORD) *ptrS < 0) == ((SWORD) *ptrD < 0) && ((SWORD) *ptrS < 0) != ((SWORD) result < 0);
    C = (temp >> 16) & 1;
    
    *ptrD = (WORD) temp;
}
void Core::f_ash() {
    WORD temp;
    temp = (SWORD) *ptrS > 0 ? *ptrD << *ptrS : (SWORD) *ptrD >> (-(SWORD) *ptrS);
    
    N = (temp >> 15) & 1;
    Z = temp == 0 ? 1 : 0;
    V = ((SWORD) *ptrD < 0) != ((SWORD) temp < 0);
    C = (SWORD) *ptrS > 0 ? (*ptrD >> (16 - *ptrS)) & 1 : (SWORD) *ptrS < 0 ? (*ptrD >> ((-(SWORD)*ptrS) - 1)) & 1 : 0;

    *ptrD = temp;
}
void Core::f_ashc() {
    DWORD temp;
    DWORD result;
    WORD temp1;
    if(re % 2) {
        temp1 = (SWORD) *ptrS > 0 ? *ptrD << *ptrS : (SWORD) *ptrD >> (-(SWORD) *ptrS);
    
        N = (temp1 >> 15) & 1;
        Z = temp1 == 0 ? 1 : 0;
        V = ((SWORD) *ptrD < 0) != ((SWORD) temp1 < 0);
        C = (SWORD) *ptrS > 0 ? (*ptrD >> (16 - *ptrS)) & 1 : (SWORD) *ptrS < 0 ? (*ptrD >> ((-(SWORD)*ptrS) - 1)) & 1 : 0;

        *ptrD = temp1;
    }
    else {
        temp = ((DWORD)*ptrD << 16) | ((DWORD) reg[re + 1]);
        result = (SWORD) *ptrS > 0 ? temp << *ptrS : (SDWORD) temp >> (-(SWORD) *ptrS);
        
        N = (SDWORD) result < 0;
        Z = result == 0 ? 1 : 0;
        V = ((SDWORD) temp < 0) != ((SDWORD) result < 0);
        C = (SWORD) *ptrS > 0 ? (temp >> (32 - *ptrS)) & 1 : (SWORD) *ptrS < 0 ? (temp >> ((-(SWORD)*ptrS) - 1)) & 1 : 0;
        
        reg[re + 1] = (WORD) result;
        *ptrD = (DWORD) result >> 16;
    }
}
void Core::f_aslb() {
    BYTE temp;
    BYTE result;
    temp = *ptrD;
    result = temp << 1;
    
    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SBYTE) temp < 0) != ((SBYTE) result < 0);
    C = (temp >> 7) & 1;
    
    *ptrD = (*ptrD & 0xff00) | (WORD) result;
}
void Core::f_asl() {
    WORD temp;
    WORD result;
    temp = *ptrD;
    result = *ptrD << 1;
    
    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SWORD) temp < 0) != ((SWORD) result < 0);
    C = (temp >> 15) & 1;
    
    *ptrD <<= 1;
}
void Core::f_asrb() {
    BYTE temp;
    BYTE result;
    temp = *ptrD;
    result = (SBYTE) temp >> 1;
    
    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = (((SBYTE) temp < 0) && !(temp & 0x1)) || (((SBYTE) temp > 0) && (temp & 0x1));        //???
    C = temp & 0x1;
    
    *ptrD = (*ptrD & 0xff00) | (WORD) result;
}
void Core::f_asr() {
    WORD temp;
    WORD result;
    temp = *ptrD;
    result = (SWORD) *ptrD >> 1;
    
    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = (((SWORD) temp < 0) && !(temp & 0x1)) || (((SWORD) temp > 0) && (temp & 0x1));        //???
    C = temp & 0x1;
    
    *ptrD = result;
}
void Core::f_bcc() {
    if(C == 0)
        PC += 2*xx;
}
void Core::f_bcs() {
    if(C == 1)
        PC += 2*xx;
}
void Core::f_beq() {
    if(Z == 1)
        PC += 2*xx;
}
void Core::f_bge() {
    if((N ^ V) == 0)
        PC += 2*xx;
}
void Core::f_bgt() {
    if((Z | (N ^ V)) == 0)
        PC += 2*xx;
}
void Core::f_bicb() {
    BYTE result;
    result = *ptrD & ~*ptrS;
    
    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = 0;
    
    *ptrD = (*ptrD & 0xff00) | (WORD) result;
}
void Core::f_bic() {
    WORD result;
    result = *ptrD & ~*ptrS;
    
    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = 0;
    
    *ptrD = *ptrD & ~*ptrS;
}
void Core::f_bisb() {
    BYTE result;
    result = *ptrD | *ptrS;
    
    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = 0;
    
    *ptrD = (*ptrD & 0xff00) | (WORD) result;
}
void Core::f_bis() {
    WORD result;
    result = *ptrD | *ptrS;
    
    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = 0;
    
    *ptrD = *ptrD | *ptrS;
}
void Core::f_bitb() {
    BYTE result;
    result = *ptrD & *ptrS;
    
    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = 0;
}
void Core::f_bit() {
    WORD result;
    result = *ptrD & *ptrS;
    
    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = 0;
}
void Core::f_bhi() {
    if((C | Z) == 0)
        PC += 2*xx;
}
void Core::f_ble() {
    if((Z | (N ^ V)) == 1)
        PC += 2*xx;
}
void Core::f_blt() {
    if((N ^ V) == 1)
        PC += 2*xx;
}
void Core::f_blos() {
    if((C | Z) == 1)
        PC += 2*xx;
}
void Core::f_bmi() {
    if(N == 1)
        PC += 2*xx;
}
void Core::f_bne() {
    if(Z == 0)
        PC += 2*xx;
}
void Core::f_bpl() {
    if(N == 0)
        PC += 2*xx;
}
void Core::f_bpt() {
    std::printf("f_bpt");
}
void Core::f_br() {
    PC += 2*xx;
}
void Core::f_bvc() {
    if(V == 0)
        PC += 2*xx;
}
void Core::f_bvs() {
    if(V == 1)
        PC += 2*xx;
}
void Core::f_ccc() {
    N = Z = V = C = 0;
}
void Core::f_clc() {
    C = 0;
}
void Core::f_cln() {
    N = 0;
}
void Core::f_clrb() {
    N = 0;
    Z = 1;
    V = 0;
    C = 0;
    
    *ptrD = *ptrD & 0xff00;
}
void Core::f_clr() {
    N = 0;
    Z = 1;
    V = 0;
    C = 0;
    
    *ptrD = 0;
}
void Core::f_clv() {
    V = 0;
}
void Core::f_clz() {
    Z = 0;
}
void Core::f_cmpb() {
    BYTE result;
    BYTE temp1, temp2;
    temp1 = (BYTE) *ptrS;
    temp2 = (BYTE) *ptrD;
    result = temp1 - temp2;
    
    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SBYTE) temp1 < 0) != ((SBYTE) temp2 < 0) && ((SBYTE) temp1 < 0) != ((SBYTE) result < 0);
    C = temp1 < temp2;    
}
void Core::f_cmp() {
    WORD result;
    result = *ptrS - *ptrD;
    
    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SWORD) *ptrS < 0) != ((SWORD) *ptrD < 0) && ((SWORD) *ptrS < 0) != ((SWORD) result < 0);
    C = *ptrS < *ptrD;
}
void Core::f_comb() {
    BYTE result;
    result = ~*ptrD;
    
    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = 0;
    C = 1;
    
    *ptrD = (*ptrD & 0xff00) | (WORD) result;
}
void Core::f_com() {
    WORD result;
    result = ~*ptrD;
    
    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = 0;
    C = 1;
    
    *ptrD = ~*ptrD;
}
void Core::f_decb() {
    BYTE result;
    BYTE temp;
    temp = (BYTE) (*ptrD & 0xff);
    result = temp - 1;
    
    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SBYTE) temp < 0) && ((SBYTE) temp < 0) != ((SBYTE) result < 0);
    
    *ptrD = (*ptrD & 0xff00) | (WORD) result;
}
void Core::f_dec() {
    WORD result;
    result = *ptrD - 1;
    
    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SWORD) *ptrD < 0) && ((SWORD) *ptrD < 0) != ((SWORD) result < 0);
    
    *ptrD -= 1;
}
void Core::f_div() {
    BYTE Ztf = 0;
    BYTE altfl = 0;
    SDWORD temp;
    SDWORD result;
    WORD ostatok;
    WORD itog;
    if(re % 2) {
        temp = ((DWORD) reg[re - 1] << 16) | ((DWORD) *ptrD);
        altfl = 1;
    }
    else
        temp = ((DWORD) *ptrD << 16) | ((DWORD) reg[re + 1]);
        
    if(*ptrS != 0) {
        result = temp / (SWORD) *ptrS;
        ostatok = temp % (SWORD) *ptrS;

        if(altfl)
            V = 0;
        else
            V = (DWORD)result >= 32768 ? 1 : 0;
        C = 0;
        
        if(result < 0 && !altfl) {
            result = -result;
            ostatok = -ostatok;
        }
        if((result >> 16) == 0) {
            if(result >> 15) {
                itog = 0;
                Ztf = 1;
            }
            else
                itog = result;
        }
        else if(altfl)
            itog = result;
        else
            itog = result >> 16;
            
        N = (itog >> 15) & 1;
        Z = itog == 0 && !Ztf ? 1 : 0;
        
        if(re % 2) {
            reg[re - 1] = itog;
            *ptrD = ostatok;
        }
        else {
            *ptrD = itog;
            reg[re + 1] = ostatok;
        }
    }
    else {
        N = 0;
        Z = V = C = 1;
    }
}
void Core::f_emt() {
    std::printf("f_emt");
}
void Core::f_fadd() {
    std::printf("f_add");
}
void Core::f_fdiv() {
    std::printf("f_fdiv");
}
void Core::f_fmul() {
    std::printf("f_fmul");
}
void Core::f_fsub() {
    std::printf("f_fsub");
}
void Core::f_halt() {
}
void Core::f_incb() {
    BYTE result;
    BYTE temp;
    temp = (BYTE) (*ptrD & 0xff);
    result = temp + 1;
    
    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SBYTE) temp > 0) && ((SBYTE) temp < 0) != ((SBYTE) result < 0);
    
    *ptrD = (*ptrD & 0xff00) | (WORD) result;
}
void Core::f_inc() {
    WORD result;
    result = *ptrD + 1;
    
    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SWORD) *ptrD > 0) && ((SWORD) *ptrD < 0) != ((SWORD) result < 0);
    
    *ptrD += 1;
}
void Core::f_iot() {
    std::printf("f_iot");
}
void Core::f_jmp() {
    PC = 2 * (ptrD - (WORD*)mem->memory);
}
void Core::f_jsr() {
    SP -= 2;
    mem->writeword(SP, *ptrS);
    *ptrS = PC;
    PC = 2 * (ptrD - (WORD*)mem->memory);
}
void Core::f_mark() {
    std::printf("f_mark");
}
void Core::f_mfpd() {
    std::printf("f_mfpd");
}
void Core::f_mfpi() {
    std::printf("f_mfpi");
}
void Core::f_mfps() {
    std::printf("f_mfps");
}
void Core::f_movb() {
    BYTE temp;
    if(last_mo) {
        if(2 * (ptrS - (WORD*)mem->memory) == IDATA)
            *ptrS = getchar();
    }
    temp = (BYTE) (*ptrS & 0xff);
    
    N = (temp >> 7) & 1;
    Z = temp == 0 ? 1 : 0;
    V = 0;
    
    if(mo)
        *ptrD = (*ptrD & 0xff00) | (*ptrS & 0xff);
    else
        *ptrD = (SBYTE) temp;
}
void Core::f_mov() {
    if(last_mo) {
        if(2 * (ptrS - (WORD*)mem->memory) == IDATA)
            *ptrS = getchar();
    }
    
    N = (*ptrS >> 15) & 1;
    Z = *ptrS == 0 ? 1 : 0;
    V = 0;

    *ptrD = *ptrS;
}
void Core::f_mtpd() {
    std::printf("f_mtpd");
}
void Core::f_mtpi() {
    std::printf("f_mtpi");
}
void Core::f_mtps() {
    std::printf("f_mtps");
}
void Core::f_mul() {
    SDWORD result;
    result = (SWORD) *ptrD * (SWORD) *ptrS;
    
    N = result < 0;
    Z = result == 0 ? 1 : 0;
    V = 0;
    C = (result >> 16) != 0;
    
    if(re % 2)
        *ptrD = (WORD) result;
    else {
        *ptrD = (WORD)(result >> 16);
        reg[re + 1] = (WORD) result;
    }
}
void Core::f_negb() {
    BYTE result;
    result = ~*ptrD + 1;
    
    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = result == 0200 ? 1 : 0;
    C = result == 0 ? 0 : 1;
    
    *ptrD = (*ptrD & 0xff00) | (WORD) result;
}
void Core::f_neg() {
    WORD result;
    result = ~*ptrD + 1;
    
    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = result == 0100000 ? 1 : 0;
    C = result == 0 ? 0 : 1;
    
    *ptrD = ~*ptrD + 1;
}
void Core::f_nop() {
    std::printf("f_nop");
}
void Core::f_reset() {
    std::printf("f_reset");
}
void Core::f_rolb() {
    BYTE temp;
    BYTE result;
    temp = (*ptrD & 0200) >> 7;
    result = (*ptrD << 1) | C;
    
    C = temp;
    temp = *ptrD;
    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SBYTE) temp < 0) != ((SBYTE) result < 0);
    
    *ptrD = (*ptrD & 0xff00) | (WORD) result;
}
void Core::f_rol() {
    BYTE temp;
    WORD result;
    temp = (*ptrD & 0100000) >> 15;
    result = (*ptrD << 1) | C;
    
    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SWORD) *ptrD < 0) != ((SWORD) result < 0);
    C = temp;
    
    *ptrD = result;
}
void Core::f_rorb() {
    BYTE temp;
    BYTE result;
    temp = *ptrD & 1;
    result = ((BYTE) *ptrD >> 1) | (C << 7);
    
    C = temp;
    temp = *ptrD;
    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SBYTE) *ptrD < 0) != ((SBYTE) result < 0);
    
    *ptrD = (*ptrD & 0xff00) | (WORD) result;
}
void Core::f_ror() {
    WORD temp;
    WORD result;
    temp = *ptrD & 1;
    result = (*ptrD >> 1) | ((WORD) C << 15);
    
    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SWORD) *ptrD < 0) != ((SWORD) result < 0);
    C = temp;
    
    *ptrD = result;
}
void Core::f_rti() {
    std::printf("f_rti");
}
void Core::f_rts() {
    PC = *ptrD;
    *ptrD = mem->readword(SP);
    SP += 2;
}
void Core::f_rtt() {
    std::printf("f_rtt");
}
void Core::f_sbcb() {
    BYTE result;
    BYTE temp;
    temp = (BYTE) (*ptrD & 0xff);
    result = temp - C;
    
    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SBYTE) temp < 0) && ((SBYTE) result > 0);
    C = temp == 0 && C == 1;
    
    *ptrD = (*ptrD & 0xff00) | (WORD) result;
}
void Core::f_sbc() {
    DWORD temp;
    WORD result;
    temp = (DWORD) *ptrD - C;
    result = (WORD) temp;
    
    N = (temp >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SWORD) *ptrD < 0) && ((SWORD) result > 0);
    C = *ptrD == 0 && C == 1;
    
    *ptrD = (WORD) temp;
}
void Core::f_scc() {
    N = Z = V = C = 1;
}
void Core::f_sec() {
    C = 1;
}
void Core::f_sen() {
    N = 1;
}
void Core::f_sev() {
    V = 1;
}
void Core::f_sez() {
    Z = 1;
}
void Core::f_sob() {
    std::printf("f_sob");
}
void Core::f_spl() {
    std::printf("f_spl");
}
void Core::f_sub() {
    DWORD temp;
    WORD result;
    temp = (DWORD) *ptrD - (DWORD) *ptrS;
    result = (WORD) temp;
    
    N = (temp >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SWORD) *ptrS < 0) != ((SWORD) *ptrD < 0) && ((SWORD) *ptrD < 0) != ((SWORD) result < 0);
    C = *ptrD < *ptrS;
    
    *ptrD = (WORD) temp;
}
void Core::f_swab() {
    BYTE temp;
    temp = *ptrD;
    
    N = (temp >> 7) & 1;
    Z = temp == 0 ? 1 : 0;
    V = C = 0;
    
    *ptrD = (*ptrD >> 8) | ((WORD)temp << 8);
}
void Core::f_sxt() {
    std::printf("f_sxt");
}
void Core::f_trap() {
    std::printf("f_trap");
}
void Core::f_tstb() {
    BYTE temp;
    temp = (BYTE) *ptrD;
    
    N = (temp >> 7) & 1;
    Z = temp == 0 ? 1 : 0;
    V = 0;
    C = 0;
}
void Core::f_tst() {
    N = (*ptrD >> 15) & 1;
    Z = *ptrD == 0 ? 1 : 0;
    V = 0;
    C = 0;
}
void Core::f_wait() {
    std::printf("f_wait");
}
void Core::f_xor() {
    std::printf("f_xor");
}

//PRINTER***************************************************************
void Core::print_mn() {
    std::printf("\n%06o: ", PC - 2);
    std::printf("%s\n", instrs[idx].name.c_str());
    countfrsp = 24;
}
void Core::print_reg() {
    BYTE i;
    WORD temp1, temp2;
    char frsp = ' ';
    std::printf("%*c", countfrsp, frsp);
    if(pcsmflag == 0)
        std::printf("[%06o]\n", opcode);
    else if(pcsmflag == 1) {
        temp1 = mem->readword(oldPC - 2);
        std::printf("[%06o %06o]\n", opcode, temp1);
    }
    else if(pcsmflag == 2) {
        temp2 = mem->readword(oldPC - 2);
        std::printf("[%06o %06o]\n", opcode, temp2);
    }
    else if(pcsmflag == 3) {
        temp1 = mem->readword(oldPC - 4);
        temp2 = mem->readword(oldPC - 2);
        std::printf("[%06o %06o %06o]\n", opcode, temp1, temp2);
    }
    pcsmflag = pcsmcnt = 0;
    if(N == 1)
        std::printf("N");
    else
        std::printf("-");
    if(Z == 1)
        std::printf("Z");
    else
        std::printf("-");
    if(V == 1)
        std::printf("V");
    else
        std::printf("-");
    if(C == 1)
        std::printf("C");
    else
        std::printf("-");
    std::printf(" ");
    for(i = 0; i < 8; i++)
        std::printf("%hd:%06o ", i, reg[i]);
}
void Core::print_op() {
    WORD nt_cm;
    nt_cm = mem->readword(PC);
    pcsmcnt++;
    switch(mo) {
        case 0:
            std::printf("r%ho", re);
            countfrsp -= 2;
            break;
        case 1:
            std::printf("(r%ho)", re);
            countfrsp -= 4;
            break;
        case 2:
            if(re == 7) {
                std::printf("#%06ho", nt_cm);
                countfrsp -= 7;
                if(pcsmcnt == 1)
                    pcsmflag++;
                else
                    pcsmflag += 2;
            }
            else {
                std::printf("(r%ho)+", re);
                countfrsp -= 5;
            }
            break;
        case 3:
            if(re == 7) {
                std::printf("@#%06ho", nt_cm);
                countfrsp -= 8;
                if(pcsmcnt == 1)
                    pcsmflag++;
                else
                    pcsmflag += 2;
            }
            else {
                std::printf("@(r%ho)+", re);
                countfrsp -= 6;
            }
            break;
        case 4:
            std::printf("-(r%ho)", re);
            countfrsp -= 5;
            break;
        case 5:
            std::printf("@-(r%ho)", re);
            countfrsp -= 6;
            break;
        case 6:
            if(re == 7) {
                std::printf("%06ho", nt_cm + PC + 2);
                countfrsp -= 6;
                if(pcsmcnt == 1)
                    pcsmflag++;
                else
                    pcsmflag += 2;
            }
            else {
                std::printf("%06ho(r%ho)", nt_cm, re);
                countfrsp -= 10;
                if(pcsmcnt == 1)
                    pcsmflag++;
                else
                    pcsmflag += 2;
            }
            break;
        case 7:
            if(re == 7) {
                std::printf("@%06ho", nt_cm + PC + 2);
                countfrsp -= 7;
                if(pcsmcnt == 1)
                    pcsmflag++;
                else
                    pcsmflag += 2;
            }
            else {
                std::printf("@%06ho(r%ho)", nt_cm, re);
                countfrsp -= 11;
                if(pcsmcnt == 1)
                    pcsmflag++;
                else
                    pcsmflag += 2;
            }
            break;
    }
}
void Core::print_aim() {
    std::printf("%06o", PC + 2*xx);
    countfrsp -= 6;
}
void Core::print_end() {
    std::printf("r0=%06o r2=%06o r4=%06o sp=%06o\n", reg[0], reg[2], reg[4], SP);
    std::printf("r1=%06o r3=%06o r4=%06o pc=%06o\n", reg[1], reg[3], reg[5], PC);
    std::printf("n=%ho z=%ho v=%ho c=%ho\n", N, Z, V, C);
}
//DECODER***************************************************************
void Core::select_operand(Ident ident) {
    WORD offset;
    switch(mo) {
        case 0:
            if(ident == SRC)
                ptrS = &reg[re];
            else if(ident == DTN)
                ptrD = &reg[re];
            break;
        case 1:
            if(ident == SRC) {
                if(mem->checkmem(reg[re], 1))
                    ptrS = (WORD*)&mem->memory[reg[re]];
            }
            else if(ident == DTN) {
                if(mem->checkmem(reg[re], 1))
                    ptrD = (WORD*)&mem->memory[reg[re]];
            }
            break;
        case 2:
            if(ident == SRC) {
                if(mem->checkmem(reg[re], 1))
                    ptrS = (WORD*)&mem->memory[reg[re]];
            }
            else if(ident == DTN) {
                if(mem->checkmem(reg[re], 1))
                    ptrD = (WORD*)&mem->memory[reg[re]];
            }
            if(instrs[idx].size == 1 && re < 6)
                reg[re]++;
            else if(instrs[idx].size == 2 || re >= 6)
                reg[re] += 2;
            break;
        case 3:
            if(ident == SRC) {
                if(mem->checkmem(reg[re], 2) && mem->checkmem(mem->readword(reg[re]), 1))
                    ptrS = (WORD*)&mem->memory[mem->readword(reg[re])];
            }
            else if(ident == DTN) {
                if(mem->checkmem(reg[re], 2) && mem->checkmem(mem->readword(reg[re]), 1))
                    ptrD = (WORD*)&mem->memory[mem->readword(reg[re])];
            }
            reg[re] += 2;
            break;
        case 4:
            if(instrs[idx].size == 1 && re < 6)
                reg[re]--;
            else if(instrs[idx].size == 2 || re >= 6)
                reg[re] -= 2;
            if(ident == SRC) {
                if(mem->checkmem(reg[re], 1))
                    ptrS = (WORD*)&mem->memory[reg[re]];
            }
            else if(ident == DTN) {
                if(mem->checkmem(reg[re], 1))
                    ptrD = (WORD*)&mem->memory[reg[re]];
            }
            break;
        case 5:
            reg[re] -= 2;
            if(ident == SRC) {
                if(mem->checkmem(reg[re], 2) && mem->checkmem(mem->readword(reg[re]), 1))
                    ptrS = (WORD*)&mem->memory[mem->readword(reg[re])];
            }
            else if(ident == DTN) {
                if(mem->checkmem(reg[re], 2) && mem->checkmem(mem->readword(reg[re]), 1))
                    ptrD = (WORD*)&mem->memory[mem->readword(reg[re])];
            }
            break;
        case 6:
            offset = mem->readword(PC);
            PC += 2;
            if(ident == SRC) {
                if(mem->checkmem((WORD)(reg[re] + offset), 1))
                    ptrS = (WORD*)&mem->memory[(WORD)(reg[re] + offset)];
            }
            else if(ident == DTN) {
                if(mem->checkmem((WORD)(reg[re] + offset), 1))
                    ptrD = (WORD*)&mem->memory[(WORD)(reg[re] + offset)];
            }
            break;
        case 7:
            offset = mem->readword(PC);
            PC += 2;
            if(ident == SRC) {
                if(mem->checkmem((WORD)(reg[re] + offset), 2) && mem->checkmem(mem->readword((WORD)(reg[re] + offset)), 1))
                    ptrS = (WORD*)&mem->memory[mem->readword((WORD)(reg[re] + offset))];
            }
            else if(ident == DTN) {
                if(mem->checkmem((WORD)(reg[re] + offset), 2) && mem->checkmem(mem->readword((WORD)(reg[re] + offset)), 1))
                    ptrD = (WORD*)&mem->memory[mem->readword((WORD)(reg[re] + offset))];
            }
            break;
    }
}
BYTE Core::decode_m(BYTE a) {
    return (a & 070) >> 3;
}
BYTE Core::decode_r(BYTE a) {
    return a & 07;
}
void Core::decode(KeyRW mode) {
    last_mo = 0;
    switch(instrs[idx].type) {
        case T_SSDD:
                ss = (opcode & 0007700) >> 6;
            mo = decode_m(ss);
            re = decode_r(ss);
            if(mode == WRITE) {
                print_op();
                std::printf(",");
                countfrsp -= 1;
            }
            select_operand(SRC);
            last_mo = mo;
                dd = opcode & 0000077;
            mo = decode_m(dd);
            re = decode_r(dd);
            if(mode == WRITE)
                print_op();
            select_operand(DTN);
            break;
        case T_DD:
                dd = opcode & 0000077;
            mo = decode_m(dd);
            re = decode_r(dd);
            if(mode == WRITE)
                print_op();
            select_operand(DTN);
            break;
        case T_XX:
                xx = opcode & 0xFF;
            if(mode == WRITE)
                print_aim();
            break;
        case T_RSS:
                ss = opcode & 0000077;
            mo = decode_m(ss);
            re = decode_r(ss);
            if(mode == WRITE) {
                print_op();
                std::printf(",");
                countfrsp -= 1;
            }
            select_operand(SRC);
            last_mo = mo;
            mo = 0;
            re = (opcode & 0000700) >> 6;
            if(mode == WRITE)
                print_op();
            select_operand(DTN);
            break;
        case T_RDD:
            mo = 0;
            re = (opcode & 0000700) >> 6;
            if(mode == WRITE) {
                print_op();
                std::printf(",");
                countfrsp -= 1;
            }
            select_operand(SRC);
            last_mo = mo;
                dd = opcode & 0000077;
            mo = decode_m(dd);
            re = decode_r(dd);
            if(mode == WRITE)
                print_op();
            select_operand(DTN);
            break;
        case T_R:
            mo = 0;
            re = opcode & 0000007;
            if(mode == WRITE)
                print_op();
            select_operand(DTN);
            break;
        case T_NONE:
            break;
        default:
            break;
    }
}
//MAIN_FUNCTIONS********************************************************
BYTE Core::find_instrs() {
    BYTE i;
    for(i = 0; i < instrs_s; i++)
        if((opcode & instrs[i].mask) == instrs[i].code)
            return i;
    std::printf("Such function doesn't exist");
    return 0;
}
void Core::prep_devices() {
    mem->writeword(ISTAT, 0200);
    mem->writeword(OSTAT, 0200);
    mem->writeword(IDATA, 0);
    mem->writeword(ODATA, 0);
}
void Core::output() {
    if(mem->readword(ODATA))
        std::printf("%c", mem->readword(ODATA));
}

Core::Core() : mem(new Memory()),
                       opcode(0),
                       oldPC(0),
                       dd(0),
                       ss(0),
                       mo(0),
                       re(0),
                       last_mo(0),
                       xx(0),
                       N(0), Z(0), V(0), C(0),
                       idx(0),
                       countfrsp(0),
                       ptrD(0),
                       ptrS(0),
                       pcsmflag(0),
                       pcsmcnt(0) {
    for (unsigned i = 0; i < sizeof(reg) / sizeof(reg[0]); ++i) {
        reg[i] = 0;
    }
}

Core::~Core() {
    delete mem;
}

void Core::start(KeyRW keyRW) {
    std::printf("\n---------------- running --------------\n");
    PC = 0x200;
    do {
        output();
        prep_devices();
        opcode = mem->readword(PC);        //
        PC += 2;                    //
        idx = find_instrs();        //
          if(keyRW == WRITE) {
            print_mn();
            decode(WRITE);
        }
        else
            decode(READ);
        oldPC = PC;
        (this->*(instrs[idx].exec))();            //
        if(keyRW == WRITE)
            print_reg();
    }  while(opcode);
    std::printf("\n---------------- halted ---------------\n");
    print_end();
}

} // namespace PlimDP