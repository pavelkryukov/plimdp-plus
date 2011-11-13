/*
 * core.cpp
 *
 * PlimDP+ core
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#include "./core.h"
#include "./coredump.h"
#include "./isa.h"
#include "./macro.h"

#if ENABLE_TRACE
    #define TRACE(OPERATIONS) OPERATIONS
#else
    #define TRACE(OPERATIONS)
#endif

#if ENABLE_DISASM
    #define DISASM(OPERATIONS) OPERATIONS
#else
    #define DISASM(OPERATIONS)
#endif

namespace PlimDP {

/*SPECIAL FUNCTIONS*****************************************************/
void Core::f_adcb() {
    BYTE result;
    BYTE temp;
    temp = (BYTE) (this->readword(pD) & 0xff);
    result = temp + C;

    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SBYTE) temp > 0) && ((SBYTE) result < 0);
    C = (((WORD) temp + C) >> 8) & 1;

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Core::f_adc() {
    DWORD temp;
    WORD result;
    temp = (DWORD) this->readword(pD) + C;
    result = (WORD) temp;

    N = (temp >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SWORD) this->readword(pD) > 0) && ((SWORD) result < 0);
    C = (temp >> 16) & 1;

    this->writeword(pD, (WORD) temp);
}
void Core::f_add() {
    DWORD temp;
    WORD result;
    temp = (DWORD) this->readword(pD) + (DWORD) this->readword(pS);
    result = (WORD) temp;

    N = (temp >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SWORD) this->readword(pS) < 0) == ((SWORD) this->readword(pD) < 0) &&
        ((SWORD) this->readword(pS) < 0) != ((SWORD) result < 0);
    C = (temp >> 16) & 1;

    this->writeword(pD, (WORD) temp);
}
void Core::f_ash() {
    WORD temp;
    temp = (SWORD) this->readword(pS) > 0
            ? this->readword(pD) << this->readword(pS)
            : (SWORD) this->readword(pD) >> (-(SWORD) this->readword(pS));

    N = (temp >> 15) & 1;
    Z = temp == 0 ? 1 : 0;
    V = ((SWORD) this->readword(pD) < 0) != ((SWORD) temp < 0);
    C = (SWORD) this->readword(pS) > 0
        ? (this->readword(pD) >> (16 - this->readword(pS))) & 1
        : (SWORD) this->readword(pS) < 0
            ? (this->readword(pD) >> ((-(SWORD)this->readword(pS)) - 1)) & 1
            : 0;

    this->writeword(pD, temp);
}
void Core::f_ashc() {
    DWORD temp;
    DWORD result;
    WORD temp1;
    if (re % 2) {
        temp1 = (SWORD) this->readword(pS) > 0
            ? this->readword(pD) << this->readword(pS)
            : (SWORD) this->readword(pD) >> (-(SWORD) this->readword(pS));

        N = (temp1 >> 15) & 1;
        Z = temp1 == 0 ? 1 : 0;
        V = ((SWORD) this->readword(pD) < 0) != ((SWORD) temp1 < 0);
        C = (SWORD) this->readword(pS) > 0
            ? (this->readword(pD) >> (16 - this->readword(pS))) & 1
            : (SWORD) this->readword(pS) < 0
                ? (this->readword(pD) >> ((-(SWORD)this->readword(pS)) - 1)) & 1
                : 0;

        this->writeword(pD, temp1);
    } else {
        temp = ((DWORD)this->readword(pD) << 16) | ((DWORD) reg.readreg(re + 1));
        result = (SWORD) this->readword(pS) > 0
                    ? temp << this->readword(pS)
                    : (SDWORD) temp >> (-(SWORD) this->readword(pS));

        N = (SDWORD) result < 0;
        Z = result == 0 ? 1 : 0;
        V = ((SDWORD) temp < 0) != ((SDWORD) result < 0);
        C = (SWORD) this->readword(pS) > 0
                ? (temp >> (32 - this->readword(pS))) & 1
                : (SWORD) this->readword(pS) < 0
                    ? (temp >> ((-(SWORD)this->readword(pS)) - 1)) & 1
                    : 0;

        reg.writereg(re + 1, (WORD) result);
        this->writeword(pD, (DWORD) result >> 16);
    }
}
void Core::f_aslb() {
    BYTE temp;
    BYTE result;
    temp = this->readword(pD);
    result = temp << 1;

    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SBYTE) temp < 0) != ((SBYTE) result < 0);
    C = (temp >> 7) & 1;

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Core::f_asl() {
    WORD temp;
    WORD result;
    temp = this->readword(pD);
    result = this->readword(pD) << 1;

    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SWORD) temp < 0) != ((SWORD) result < 0);
    C = (temp >> 15) & 1;

    this->writeword(pD, this->readword(pD) << 1);
}
void Core::f_asrb() {
    BYTE temp;
    BYTE result;
    temp = this->readword(pD);
    result = (SBYTE) temp >> 1;

    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = (((SBYTE) temp < 0) && !(temp & 0x1)) ||
        (((SBYTE) temp > 0) && (temp & 0x1));  // ???
    C = temp & 0x1;

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Core::f_asr() {
    WORD temp;
    WORD result;
    temp = this->readword(pD);
    result = (SWORD) this->readword(pD) >> 1;

    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = (((SWORD) temp < 0) && !(temp & 0x1)) ||
        (((SWORD) temp > 0) && (temp & 0x1));  // ???
    C = temp & 0x1;

    this->writeword(pD, result);
}
void Core::f_bcc() {
    if (C == 0)
        reg.incPC(2 * xx);
}
void Core::f_bcs() {
    if (C == 1)
        reg.incPC(2 * xx);
}
void Core::f_beq() {
    if (Z == 1)
        reg.incPC(2 * xx);
}
void Core::f_bge() {
    if ((N ^ V) == 0)
        reg.incPC(2 * xx);
}
void Core::f_bgt() {
    if ((Z | (N ^ V)) == 0)
        reg.incPC(2 * xx);
}
void Core::f_bicb() {
    BYTE result;
    result = this->readword(pD) & ~this->readword(pS);

    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = 0;

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Core::f_bic() {
    WORD result;
    result = this->readword(pD) & ~this->readword(pS);

    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = 0;

    this->writeword(pD, this->readword(pD) & ~this->readword(pS));
}
void Core::f_bisb() {
    BYTE result;
    result = this->readword(pD) | this->readword(pS);

    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = 0;

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Core::f_bis() {
    WORD result;
    result = this->readword(pD) | this->readword(pS);

    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = 0;

    this->writeword(pD, this->readword(pD) | this->readword(pS));
}
void Core::f_bitb() {
    BYTE result;
    result = this->readword(pD) & this->readword(pS);

    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = 0;
}
void Core::f_bit() {
    WORD result;
    result = this->readword(pD) & this->readword(pS);

    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = 0;
}
void Core::f_bhi() {
    if ((C | Z) == 0)
        reg.incPC(2 * xx);
}
void Core::f_ble() {
    if ((Z | (N ^ V)) == 1)
        reg.incPC(2 * xx);
}
void Core::f_blt() {
    if ((N ^ V) == 1)
        reg.incPC(2 * xx);
}
void Core::f_blos() {
    if ((C | Z) == 1)
        reg.incPC(2 * xx);
}
void Core::f_bmi() {
    if (N == 1)
        reg.incPC(2 * xx);
}
void Core::f_bne() {
    if (Z == 0)
        reg.incPC(2 * xx);
}
void Core::f_bpl() {
    if (N == 0)
        reg.incPC(2 * xx);
}
void Core::f_bpt() {
    DIE("f_bpt");
}
void Core::f_br() {
    reg.incPC(2 * xx);
}
void Core::f_bvc() {
    if (V == 0)
        reg.incPC(2 * xx);
}
void Core::f_bvs() {
    if (V == 1)
        reg.incPC(2 * xx);
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

    this->writeword(pD, this->readword(pD) & 0xff00);
}
void Core::f_clr() {
    N = 0;
    Z = 1;
    V = 0;
    C = 0;

    this->writeword(pD, 0);
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
    temp1 = (BYTE) this->readword(pS);
    temp2 = (BYTE) this->readword(pD);
    result = temp1 - temp2;

    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SBYTE) temp1 < 0) != ((SBYTE) temp2 < 0) &&
        ((SBYTE) temp1 < 0) != ((SBYTE) result < 0);
    C = temp1 < temp2;
}
void Core::f_cmp() {
    WORD result;
    result = this->readword(pS) - this->readword(pD);

    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SWORD) this->readword(pS) < 0) != ((SWORD) this->readword(pD) < 0) &&
        ((SWORD) this->readword(pS) < 0) != ((SWORD) result < 0);
    C = this->readword(pS) < this->readword(pD);
}
void Core::f_comb() {
    BYTE result;
    result = ~this->readword(pD);

    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = 0;
    C = 1;

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Core::f_com() {
    WORD result;
    result = ~this->readword(pD);

    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = 0;
    C = 1;

    this->writeword(pD, ~this->readword(pD));
}
void Core::f_decb() {
    BYTE result;
    BYTE temp;
    temp = (BYTE) (this->readword(pD) & 0xff);
    result = temp - 1;

    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SBYTE) temp < 0) && ((SBYTE) temp < 0) != ((SBYTE) result < 0);

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Core::f_dec() {
    WORD result;
    result = this->readword(pD) - 1;

    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SWORD) this->readword(pD) < 0) &&
        ((SWORD) this->readword(pD) < 0) != ((SWORD) result < 0);

    this->writeword(pD, this->readword(pD) - 1);
}
void Core::f_div() {
    BYTE Ztf = 0;
    BYTE altfl = 0;
    SDWORD temp;
    SDWORD result;
    WORD ostatok;
    WORD itog;
    if (re % 2) {
        temp = ((DWORD) reg.readreg(re - 1) << 16) | ((DWORD) this->readword(pD));
        altfl = 1;
    } else {
        temp = ((DWORD) this->readword(pD) << 16) | ((DWORD) reg.readreg(re + 1));
    }
    if (this->readword(pS) != 0) {
        result = temp / (SWORD) this->readword(pS);
        ostatok = temp % (SWORD) this->readword(pS);

        if (altfl)
            V = 0;
        else
            V = (DWORD)result >= 32768 ? 1 : 0;
        C = 0;

        if (result < 0 && !altfl) {
            result = -result;
            ostatok = -ostatok;
        }
        if ((result >> 16) == 0) {
            if (result >> 15) {
                itog = 0;
                Ztf = 1;
            } else {
                itog = result;
            }
        } else if (altfl) {
            itog = result;
        } else {
            itog = result >> 16;
        }

        N = (itog >> 15) & 1;
        Z = itog == 0 && !Ztf ? 1 : 0;

        if (re % 2) {
            reg.writereg(re - 1, itog);
            this->writeword(pD, ostatok);
        } else {
            this->writeword(pD, itog);
            reg.writereg(re + 1, ostatok);
        }
    } else {
        N = 0;
        Z = V = C = 1;
    }
}
void Core::f_emt() {
    DIE("f_emt");
}
void Core::f_fadd() {
    DIE("f_add");
}
void Core::f_fdiv() {
    DIE("f_fdiv");
}
void Core::f_fmul() {
    DIE("f_fmul");
}
void Core::f_fsub() {
    DIE("f_fsub");
}
void Core::f_halt() {
}
void Core::f_incb() {
    BYTE result;
    BYTE temp;
    temp = (BYTE) (this->readword(pD) & 0xff);
    result = temp + 1;

    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SBYTE) temp > 0) && ((SBYTE) temp < 0) != ((SBYTE) result < 0);

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Core::f_inc() {
    WORD result;
    result = this->readword(pD) + 1;

    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SWORD) this->readword(pD) > 0) &&
        ((SWORD) this->readword(pD) < 0) != ((SWORD) result < 0);

    this->writeword(pD, this->readword(pD) + 1);
}
void Core::f_iot() {
    DIE("f_iot");
}
void Core::f_jmp() {
    reg.writePC(pD.index);
}
void Core::f_jsr() {
    reg.writeSP(reg.readSP() - 2);
    mem.writeword(reg.readSP(), this->readword(pS));
    this->writeword(pS, reg.readPC());
    reg.writePC(pD.index);
}
void Core::f_mark() {
    DIE("f_mark");
}
void Core::f_mfpd() {
    DIE("f_mfpd");
}
void Core::f_mfpi() {
    DIE("f_mfpi");
}
void Core::f_mfps() {
    DIE("f_mfps");
}
void Core::f_movb() {
    BYTE temp;
    temp = (BYTE) (this->readword(pS) & 0xff);

    N = (temp >> 7) & 1;
    Z = temp == 0 ? 1 : 0;
    V = 0;

    if (pD.type == Pointer::MEMORY)
        this->writeword(pD, (this->readword(pD) & 0xff00) | temp);
    else
        this->writeword(pD, (SBYTE) temp);
}
void Core::f_mov() {
    N = (this->readword(pS) >> 15) & 1;
    Z = this->readword(pS) == 0 ? 1 : 0;
    V = 0;

    this->writeword(pD, this->readword(pS));
}
void Core::f_mtpd() {
    DIE("f_mtpd");
}
void Core::f_mtpi() {
    DIE("f_mtpi");
}
void Core::f_mtps() {
    DIE("f_mtps");
}
void Core::f_mul() {
    SDWORD result;
    result = (SWORD) this->readword(pD) * (SWORD) this->readword(pS);

    N = result < 0;
    Z = result == 0 ? 1 : 0;
    V = 0;
    C = (result >> 16) != 0;

    if (re % 2) {
        this->writeword(pD, (WORD) result);
    } else {
        this->writeword(pD, (WORD)(result >> 16));
        reg.writereg(re + 1, (WORD) result);
    }
}
void Core::f_negb() {
    BYTE result;
    result = ~this->readword(pD) + 1;

    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = result == 0200 ? 1 : 0;
    C = result == 0 ? 0 : 1;

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Core::f_neg() {
    WORD result;
    result = ~this->readword(pD) + 1;

    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = result == 0100000 ? 1 : 0;
    C = result == 0 ? 0 : 1;

    this->writeword(pD, ~this->readword(pD) + 1);
}
void Core::f_nop() {
    DIE("f_nop");
}
void Core::f_reset() {
    DIE("f_reset");
}
void Core::f_rolb() {
    BYTE temp;
    BYTE result;
    temp = (this->readword(pD) & 0200) >> 7;
    result = (this->readword(pD) << 1) | C;

    C = temp;
    temp = this->readword(pD);
    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SBYTE) temp < 0) != ((SBYTE) result < 0);

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Core::f_rol() {
    BYTE temp;
    WORD result;
    temp = (this->readword(pD) & 0100000) >> 15;
    result = (this->readword(pD) << 1) | C;

    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SWORD) this->readword(pD) < 0) != ((SWORD) result < 0);
    C = temp;

    this->writeword(pD, result);
}
void Core::f_rorb() {
    BYTE temp;
    BYTE result;
    temp = this->readword(pD) & 1;
    result = ((BYTE) this->readword(pD) >> 1) | (C << 7);

    C = temp;
    temp = this->readword(pD);
    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SBYTE) this->readword(pD) < 0) != ((SBYTE) result < 0);

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Core::f_ror() {
    WORD temp;
    WORD result;
    temp = this->readword(pD) & 1;
    result = (this->readword(pD) >> 1) | ((WORD) C << 15);

    N = (result >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SWORD) this->readword(pD) < 0) != ((SWORD) result < 0);
    C = temp;

    this->writeword(pD, result);
}
void Core::f_rti() {
    DIE("f_rti");
}
void Core::f_rts() {
    reg.writePC(this->readword(pD));
    this->writeword(pD, mem.readword(reg.readSP()));
    reg.writeSP(reg.readSP() + 2);
}
void Core::f_rtt() {
    DIE("f_rtt");
}
void Core::f_sbcb() {
    BYTE result;
    BYTE temp;
    temp = (BYTE) (this->readword(pD) & 0xff);
    result = temp - C;

    N = (result >> 7) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SBYTE) temp < 0) && ((SBYTE) result > 0);
    C = temp == 0 && C == 1;

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Core::f_sbc() {
    DWORD temp;
    WORD result;
    temp = (DWORD) this->readword(pD) - C;
    result = (WORD) temp;

    N = (temp >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SWORD) this->readword(pD) < 0) && ((SWORD) result > 0);
    C = this->readword(pD) == 0 && C == 1;

    this->writeword(pD, (WORD) temp);
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
    DIE("f_sob");
}
void Core::f_spl() {
    DIE("f_spl");
}
void Core::f_sub() {
    DWORD temp;
    WORD result;
    temp = (DWORD) this->readword(pD) - (DWORD) this->readword(pS);
    result = (WORD) temp;

    N = (temp >> 15) & 1;
    Z = result == 0 ? 1 : 0;
    V = ((SWORD) this->readword(pS) < 0) != ((SWORD) this->readword(pD) < 0) &&
        ((SWORD) this->readword(pD) < 0) != ((SWORD) result < 0);
    C = this->readword(pD) < this->readword(pS);

    this->writeword(pD, (WORD) temp);
}
void Core::f_swab() {
    BYTE temp;
    temp = this->readword(pD);

    N = (temp >> 7) & 1;
    Z = temp == 0 ? 1 : 0;
    V = C = 0;

    this->writeword(pD, (this->readword(pD) >> 8) | ((WORD)temp << 8));
}
void Core::f_sxt() {
    DIE("f_sxt");
}
void Core::f_trap() {
    DIE("f_trap");
}
void Core::f_tstb() {
    BYTE temp;
    temp = (BYTE) this->readword(pD);

    N = (temp >> 7) & 1;
    Z = temp == 0 ? 1 : 0;
    V = 0;
    C = 0;
}
void Core::f_tst() {
    N = (this->readword(pD) >> 15) & 1;
    Z = this->readword(pD) == 0 ? 1 : 0;
    V = 0;
    C = 0;
}
void Core::f_wait() {
    DIE("f_wait");
}
void Core::f_xor() {
    DIE("f_xor");
}

/*DECODER***************************************************************/
Core::Pointer Core::select_operand(const Instr & instr, BYTE mo) {
    Core::Pointer pointer;
    WORD offset;
    switch (mo) {
        case 0:
            pointer.index = re;
            pointer.type = Pointer::REGISTER;
            break;
        case 1:
            pointer.index = reg.readreg(re);
            pointer.type = Pointer::MEMORY;
            break;
        case 2:
            pointer.index = reg.readreg(re);
            pointer.type = Pointer::MEMORY;
            if (instr.size == 1 && re < 6)
                reg.increg(re, 1);
            else if (instr.size == 2 || re >= 6)
                reg.increg(re, 2);
            break;
        case 3:
            pointer.index = mem.readword(reg.readreg(re));
            pointer.type = Pointer::MEMORY;
            reg.increg(re, 2);
            break;
        case 4:
            if (instr.size == 1 && re < 6)
                reg.increg(re, -1);
            else if (instr.size == 2 || re >= 6)
                reg.increg(re, -2);
            pointer.index = reg.readreg(re);
            pointer.type = Pointer::MEMORY;
            break;
        case 5:
            reg.increg(re, -2);
            pointer.index = mem.readword(reg.readreg(re));
            pointer.type = Pointer::MEMORY;
            break;
        case 6:
            offset = mem.readword(reg.readPC());
            reg.incPC(2);
            pointer.index = (WORD)(reg.readreg(re) + offset);
            pointer.type = Pointer::MEMORY;
            break;
        case 7:
            offset = mem.readword(reg.readPC());
            reg.incPC(2);
            pointer.index = mem.readword((WORD)(reg.readreg(re) + offset));
            pointer.type = Pointer::MEMORY;
            break;
    }
    return pointer;
}
BYTE Core::decode_m(BYTE a) {
    return (a & 070) >> 3;
}
BYTE Core::decode_r(BYTE a) {
    return a & 07;
}
void Core::decode(WORD opcode, const Instr & instr) {
    BYTE dd;
    BYTE ss;
    BYTE mo;
    switch (instr.type) {
        case Instr::T_SSDD:
            ss = (opcode & 0007700) >> 6;
            mo = decode_m(ss);
            re = decode_r(ss);
            DISASM( disasm->op(mo); )
            DISASM( disasm->comma(); )
            pS = select_operand(instr, mo);

            dd = opcode & 0000077;
            mo = decode_m(dd);
            re = decode_r(dd);
            DISASM( disasm->op(mo); )
            pD = select_operand(instr, mo);
            break;
        case Instr::T_DD:
            dd = opcode & 0000077;
            mo = decode_m(dd);
            re = decode_r(dd);
            DISASM( disasm->op(mo); )
            pD = select_operand(instr, mo);
            break;
        case Instr::T_XX:
            xx = opcode & 0xFF;
            DISASM( disasm->aim(); )
            break;
        case Instr::T_RSS:
            ss = opcode & 0000077;
            mo = decode_m(ss);
            re = decode_r(ss);
            DISASM( disasm->op(mo); )
            DISASM( disasm->comma(); )            
            pS = select_operand(instr, mo);
            
            mo = 0;
            re = (opcode & 0000700) >> 6;
            DISASM( disasm->op(mo); )
            pD = select_operand(instr, mo);
            break;
        case Instr::T_RDD:
            mo = 0;
            re = (opcode & 0000700) >> 6;
            DISASM( disasm->op(mo); )
            DISASM( disasm->comma(); )
            pS = select_operand(instr, mo);

            dd = opcode & 0000077;
            mo = decode_m(dd);
            re = decode_r(dd);
            DISASM( disasm->op(mo); )
            pD = select_operand(instr, mo);
            break;
        case Instr::T_R:
            mo = 0;
            re = opcode & 0000007;
            DISASM( disasm->op(mo) );
            pD = select_operand(instr, mo);
            break;
        case Instr::T_NONE:
            break;
        default:
            break;
    }
}
/* MAIN_FUNCTIONS*******************************************************/
Core::Core() : re(0),
               N(0), Z(0), V(0), C(0),
               xx(0) {
    dump = new CoreDump(this);
    DISASM( disasm = new Disassembler(this); )
}

Core::~Core() {
    delete dump;
    DISASM( delete disasm; )
}

void Core::start() {
    dump->running();
    WORD opcode;
    do {
        // Fetch
        opcode = mem.readword(reg.readPC());        //
        reg.incPC(2);            //

        // Decode
        const Instr instr = ISA::find_instrs(opcode);        //

        DISASM( disasm->mn(instr); )

        decode(opcode, instr);

        DISASM( WORD oldPC = reg.readPC(); )

        // Execute
        (this->*(instr.exec))();            //

        DISASM( disasm->reg(opcode, oldPC); )
        TRACE( dump->core();      )
    }  while (opcode);
    dump->end();
}
}  // namespace PlimDP
