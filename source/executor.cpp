/*
 * executor.cpp
 *
 * PlimDP+ executor
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#include "./macro.h"

#include "./executor.h"

namespace PlimDP {
namespace CPU {
void Executor::f_adcb() {
    BYTE result;
    BYTE temp;
    temp = (BYTE) (this->readword(pD) & 0xff);
    result = temp + flags.C;

    flags.N = (result >> 7) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = ((SBYTE) temp > 0) && ((SBYTE) result < 0);
    flags.C = (((WORD) temp + flags.C) >> 8) & 1;

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Executor::f_adc() {
    DWORD temp;
    WORD result;
    temp = (DWORD) this->readword(pD) + flags.C;
    result = (WORD) temp;

    flags.N = (temp >> 15) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = ((SWORD) this->readword(pD) > 0) && ((SWORD) result < 0);
    flags.C = (temp >> 16) & 1;

    this->writeword(pD, (WORD) temp);
}
void Executor::f_add() {
    DWORD temp;
    WORD result;
    temp = (DWORD) this->readword(pD) + (DWORD) this->readword(pS);
    result = (WORD) temp;

    flags.N = (temp >> 15) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = ((SWORD) this->readword(pS) < 0) == ((SWORD) this->readword(pD) < 0) &&
        ((SWORD) this->readword(pS) < 0) != ((SWORD) result < 0);
    flags.C = (temp >> 16) & 1;

    this->writeword(pD, (WORD) temp);
}
void Executor::f_ash() {
    WORD temp;
    temp = (SWORD) this->readword(pS) > 0
            ? this->readword(pD) << this->readword(pS)
            : (SWORD) this->readword(pD) >> (-(SWORD) this->readword(pS));

    flags.N = (temp >> 15) & 1;
    flags.Z = temp == 0 ? 1 : 0;
    flags.V = ((SWORD) this->readword(pD) < 0) != ((SWORD) temp < 0);
    flags.C = (SWORD) this->readword(pS) > 0
        ? (this->readword(pD) >> (16 - this->readword(pS))) & 1
        : (SWORD) this->readword(pS) < 0
            ? (this->readword(pD) >> ((-(SWORD)this->readword(pS)) - 1)) & 1
            : 0;

    this->writeword(pD, temp);
}
void Executor::f_ashc() {
    DWORD temp;
    DWORD result;
    WORD temp1;
    ASSERTX(pD.type == Operand::REGISTER);
    if (pD.index % 2) {
        temp1 = (SWORD) this->readword(pS) > 0
            ? this->readword(pD) << this->readword(pS)
            : (SWORD) this->readword(pD) >> (-(SWORD) this->readword(pS));

        flags.N = (temp1 >> 15) & 1;
        flags.Z = temp1 == 0 ? 1 : 0;
        flags.V = ((SWORD) this->readword(pD) < 0) != ((SWORD) temp1 < 0);
        flags.C = (SWORD) this->readword(pS) > 0
            ? (this->readword(pD) >> (16 - this->readword(pS))) & 1
            : (SWORD) this->readword(pS) < 0
                ? (this->readword(pD) >> ((-(SWORD)this->readword(pS)) - 1)) & 1
                : 0;

        this->writeword(pD, temp1);
    } else {
        temp = ((DWORD)this->readword(pD) << 16) | ((DWORD) reg.readreg(pD.index + 1));
        result = (SWORD) this->readword(pS) > 0
                    ? temp << this->readword(pS)
                    : (SDWORD) temp >> (-(SWORD) this->readword(pS));

        flags.N = (SDWORD) result < 0;
        flags.Z = result == 0 ? 1 : 0;
        flags.V = ((SDWORD) temp < 0) != ((SDWORD) result < 0);
        flags.C = (SWORD) this->readword(pS) > 0
                ? (temp >> (32 - this->readword(pS))) & 1
                : (SWORD) this->readword(pS) < 0
                    ? (temp >> ((-(SWORD)this->readword(pS)) - 1)) & 1
                    : 0;

        reg.writereg(pD.index + 1, (WORD) result);
        this->writeword(pD, (DWORD) result >> 16);
    }
}
void Executor::f_aslb() {
    BYTE temp;
    BYTE result;
    temp = this->readword(pD);
    result = temp << 1;

    flags.N = (result >> 7) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = ((SBYTE) temp < 0) != ((SBYTE) result < 0);
    flags.C = (temp >> 7) & 1;

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Executor::f_asl() {
    WORD temp;
    WORD result;
    temp = this->readword(pD);
    result = this->readword(pD) << 1;

    flags.N = (result >> 15) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = ((SWORD) temp < 0) != ((SWORD) result < 0);
    flags.C = (temp >> 15) & 1;

    this->writeword(pD, this->readword(pD) << 1);
}
void Executor::f_asrb() {
    BYTE temp;
    BYTE result;
    temp = this->readword(pD);
    result = (SBYTE) temp >> 1;

    flags.N = (result >> 7) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = (((SBYTE) temp < 0) && !(temp & 0x1)) ||
        (((SBYTE) temp > 0) && (temp & 0x1));  // ???
    flags.C = temp & 0x1;

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Executor::f_asr() {
    WORD temp;
    WORD result;
    temp = this->readword(pD);
    result = (SWORD) this->readword(pD) >> 1;

    flags.N = (result >> 15) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = (((SWORD) temp < 0) && !(temp & 0x1)) ||
        (((SWORD) temp > 0) && (temp & 0x1));  // ???
    flags.C = temp & 0x1;

    this->writeword(pD, result);
}
void Executor::f_bcc() {
    if (flags.C == 0)
        this->branch(xx);
}
void Executor::f_bcs() {
    if (flags.C == 1)
        this->branch(xx);
}
void Executor::f_beq() {
    if (flags.Z == 1)
        this->branch(xx);
}
void Executor::f_bge() {
    if ((flags.N ^ flags.V) == 0)
        this->branch(xx);
}
void Executor::f_bgt() {
    if ((flags.Z | (flags.N ^ flags.V)) == 0)
        this->branch(xx);
}
void Executor::f_bicb() {
    BYTE result;
    result = this->readword(pD) & ~this->readword(pS);

    flags.N = (result >> 7) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = 0;

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Executor::f_bic() {
    WORD result;
    result = this->readword(pD) & ~this->readword(pS);

    flags.N = (result >> 15) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = 0;

    this->writeword(pD, this->readword(pD) & ~this->readword(pS));
}
void Executor::f_bisb() {
    BYTE result;
    result = this->readword(pD) | this->readword(pS);

    flags.N = (result >> 7) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = 0;

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Executor::f_bis() {
    WORD result;
    result = this->readword(pD) | this->readword(pS);

    flags.N = (result >> 15) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = 0;

    this->writeword(pD, this->readword(pD) | this->readword(pS));
}
void Executor::f_bitb() {
    BYTE result;
    result = this->readword(pD) & this->readword(pS);

    flags.N = (result >> 7) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = 0;
}
void Executor::f_bit() {
    WORD result;
    result = this->readword(pD) & this->readword(pS);

    flags.N = (result >> 15) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = 0;
}
void Executor::f_bhi() {
    if ((flags.C | flags.Z) == 0)
        this->branch(xx);
}
void Executor::f_ble() {
    if ((flags.Z | (flags.N ^ flags.V)) == 1)
        this->branch(xx);
}
void Executor::f_blt() {
    if ((flags.N ^ flags.V) == 1)
        this->branch(xx);
}
void Executor::f_blos() {
    if ((flags.C | flags.Z) == 1)
        this->branch(xx);
}
void Executor::f_bmi() {
    if (flags.N == 1)
        this->branch(xx);
}
void Executor::f_bne() {
    if (flags.Z == 0)
        this->branch(xx);
}
void Executor::f_bpl() {
    if (flags.N == 0)
        this->branch(xx);
}
void Executor::f_bpt() {
    DIE("f_bpt");
}
void Executor::f_br() {
    this->branch(xx);
}
void Executor::f_bvc() {
    if (flags.V == 0)
        this->branch(xx);
}
void Executor::f_bvs() {
    if (flags.V == 1)
        this->branch(xx);
}
void Executor::f_ccc() {
    flags.N = flags.Z = flags.V = flags.C = 0;
}
void Executor::f_clc() {
    flags.C = 0;
}
void Executor::f_cln() {
    flags.N = 0;
}
void Executor::f_clrb() {
    flags.N = 0;
    flags.Z = 1;
    flags.V = 0;
    flags.C = 0;

    this->writeword(pD, this->readword(pD) & 0xff00);
}
void Executor::f_clr() {
    flags.N = 0;
    flags.Z = 1;
    flags.V = 0;
    flags.C = 0;

    this->writeword(pD, 0);
}
void Executor::f_clv() {
    flags.V = 0;
}
void Executor::f_clz() {
    flags.Z = 0;
}
void Executor::f_cmpb() {
    BYTE result;
    BYTE temp1, temp2;
    temp1 = (BYTE) this->readword(pS);
    temp2 = (BYTE) this->readword(pD);
    result = temp1 - temp2;

    flags.N = (result >> 7) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = ((SBYTE) temp1 < 0) != ((SBYTE) temp2 < 0) &&
        ((SBYTE) temp1 < 0) != ((SBYTE) result < 0);
    flags.C = temp1 < temp2;
}
void Executor::f_cmp() {
    WORD result;
    result = this->readword(pS) - this->readword(pD);

    flags.N = (result >> 15) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = ((SWORD) this->readword(pS) < 0) != ((SWORD) this->readword(pD) < 0) &&
        ((SWORD) this->readword(pS) < 0) != ((SWORD) result < 0);
    flags.C = this->readword(pS) < this->readword(pD);
}
void Executor::f_comb() {
    BYTE result;
    result = ~this->readword(pD);

    flags.N = (result >> 7) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = 0;
    flags.C = 1;

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Executor::f_com() {
    WORD result;
    result = ~this->readword(pD);

    flags.N = (result >> 15) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = 0;
    flags.C = 1;

    this->writeword(pD, ~this->readword(pD));
}
void Executor::f_decb() {
    BYTE result;
    BYTE temp;
    temp = (BYTE) (this->readword(pD) & 0xff);
    result = temp - 1;

    flags.N = (result >> 7) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = ((SBYTE) temp < 0) && ((SBYTE) temp < 0) != ((SBYTE) result < 0);

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Executor::f_dec() {
    WORD result;
    result = this->readword(pD) - 1;

    flags.N = (result >> 15) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = ((SWORD) this->readword(pD) < 0) &&
        ((SWORD) this->readword(pD) < 0) != ((SWORD) result < 0);

    this->writeword(pD, this->readword(pD) - 1);
}
void Executor::f_div() {
    BYTE Ztf = 0;
    BYTE altfl = 0;
    SDWORD temp;
    SDWORD result;
    WORD ostatok;
    WORD itog;
    ASSERTX(pD.type == Operand::REGISTER);
    if (pD.index % 2) {
        temp = ((DWORD) reg.readreg(pD.index - 1) << 16) | ((DWORD) this->readword(pD));
        altfl = 1;
    } else {
        temp = ((DWORD) this->readword(pD) << 16) | ((DWORD) reg.readreg(pD.index + 1));
    }
    if (this->readword(pS) != 0) {
        result = temp / (SWORD) this->readword(pS);
        ostatok = temp % (SWORD) this->readword(pS);

        if (altfl)
            flags.V = 0;
        else
            flags.V = (DWORD)result >= 32768 ? 1 : 0;
        flags.C = 0;

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

        flags.N = (itog >> 15) & 1;
        flags.Z = itog == 0 && !Ztf ? 1 : 0;

        if (pD.index % 2) {
            reg.writereg(pD.index - 1, itog);
            this->writeword(pD, ostatok);
        } else {
            this->writeword(pD, itog);
            reg.writereg(pD.index + 1, ostatok);
        }
    } else {
        flags.N = 0;
        flags.Z = flags.V = flags.C = 1;
    }
}
void Executor::f_emt() {
    DIE("f_emt");
}
void Executor::f_fadd() {
    DIE("f_add");
}
void Executor::f_fdiv() {
    DIE("f_fdiv");
}
void Executor::f_fmul() {
    DIE("f_fmul");
}
void Executor::f_fsub() {
    DIE("f_fsub");
}
void Executor::f_halt() {
}
void Executor::f_incb() {
    BYTE result;
    BYTE temp;
    temp = (BYTE) (this->readword(pD) & 0xff);
    result = temp + 1;

    flags.N = (result >> 7) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = ((SBYTE) temp > 0) && ((SBYTE) temp < 0) != ((SBYTE) result < 0);

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Executor::f_inc() {
    WORD result;
    result = this->readword(pD) + 1;

    flags.N = (result >> 15) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = ((SWORD) this->readword(pD) > 0) &&
        ((SWORD) this->readword(pD) < 0) != ((SWORD) result < 0);

    this->writeword(pD, this->readword(pD) + 1);
}
void Executor::f_iot() {
    DIE("f_iot");
}
void Executor::f_jmp() {
    reg.writePC(pD.index);
}
void Executor::f_jsr() {
    reg.writeSP(reg.readSP() - 2);
    mem.writeword(reg.readSP(), this->readword(pS));
    this->writeword(pS, reg.readPC());
    reg.writePC(pD.index);
}
void Executor::f_mark() {
    DIE("f_mark");
}
void Executor::f_mfpd() {
    DIE("f_mfpd");
}
void Executor::f_mfpi() {
    DIE("f_mfpi");
}
void Executor::f_mfps() {
    DIE("f_mfps");
}
void Executor::f_movb() {
    BYTE temp;
    temp = (BYTE) (this->readword(pS) & 0xff);

    flags.N = (temp >> 7) & 1;
    flags.Z = temp == 0 ? 1 : 0;
    flags.V = 0;

    if (pD.type == Operand::MEMORY)
        this->writeword(pD, (this->readword(pD) & 0xff00) | temp);
    else
        this->writeword(pD, (SBYTE) temp);
}
void Executor::f_mov() {
    flags.N = (this->readword(pS) >> 15) & 1;
    flags.Z = this->readword(pS) == 0 ? 1 : 0;
    flags.V = 0;

    this->writeword(pD, this->readword(pS));
}
void Executor::f_mtpd() {
    DIE("f_mtpd");
}
void Executor::f_mtpi() {
    DIE("f_mtpi");
}
void Executor::f_mtps() {
    DIE("f_mtps");
}
void Executor::f_mul() {
    SDWORD result;
    result = (SWORD) this->readword(pD) * (SWORD) this->readword(pS);

    flags.N = result < 0;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = 0;
    flags.C = (result >> 16) != 0;

    ASSERTX(pD.type == Operand::REGISTER);
    if (pD.index % 2) {
        this->writeword(pD, (WORD) result);
    } else {
        this->writeword(pD, (WORD)(result >> 16));
        reg.writereg(pD.index + 1, (WORD) result);
    }
}
void Executor::f_negb() {
    BYTE result;
    result = ~this->readword(pD) + 1;

    flags.N = (result >> 7) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = result == 0200 ? 1 : 0;
    flags.C = result == 0 ? 0 : 1;

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Executor::f_neg() {
    WORD result;
    result = ~this->readword(pD) + 1;

    flags.N = (result >> 15) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = result == 0100000 ? 1 : 0;
    flags.C = result == 0 ? 0 : 1;

    this->writeword(pD, ~this->readword(pD) + 1);
}
void Executor::f_nop() {
    DIE("f_nop");
}
void Executor::f_reset() {
    DIE("f_reset");
}
void Executor::f_rolb() {
    BYTE temp;
    BYTE result;
    temp = (this->readword(pD) & 0200) >> 7;
    result = (this->readword(pD) << 1) | flags.C;

    flags.C = temp;
    temp = this->readword(pD);
    flags.N = (result >> 7) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = ((SBYTE) temp < 0) != ((SBYTE) result < 0);

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Executor::f_rol() {
    BYTE temp;
    WORD result;
    temp = (this->readword(pD) & 0100000) >> 15;
    result = (this->readword(pD) << 1) | flags.C;

    flags.N = (result >> 15) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = ((SWORD) this->readword(pD) < 0) != ((SWORD) result < 0);
    flags.C = temp;

    this->writeword(pD, result);
}
void Executor::f_rorb() {
    BYTE temp;
    BYTE result;
    temp = this->readword(pD) & 1;
    result = ((BYTE) this->readword(pD) >> 1) | (flags.C << 7);

    flags.C = temp;
    temp = this->readword(pD);
    flags.N = (result >> 7) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = ((SBYTE) this->readword(pD) < 0) != ((SBYTE) result < 0);

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Executor::f_ror() {
    WORD temp;
    WORD result;
    temp = this->readword(pD) & 1;
    result = (this->readword(pD) >> 1) | ((WORD) flags.C << 15);

    flags.N = (result >> 15) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = ((SWORD) this->readword(pD) < 0) != ((SWORD) result < 0);
    flags.C = temp;

    this->writeword(pD, result);
}
void Executor::f_rti() {
    DIE("f_rti");
}
void Executor::f_rts() {
    reg.writePC(this->readword(pD));
    this->writeword(pD, mem.readword(reg.readSP()));
    reg.writeSP(reg.readSP() + 2);
}
void Executor::f_rtt() {
    DIE("f_rtt");
}
void Executor::f_sbcb() {
    BYTE result;
    BYTE temp;
    temp = (BYTE) (this->readword(pD) & 0xff);
    result = temp - flags.C;

    flags.N = (result >> 7) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = ((SBYTE) temp < 0) && ((SBYTE) result > 0);
    flags.C = temp == 0 && flags.C == 1;

    this->writeword(pD, (this->readword(pD) & 0xff00) | (WORD) result);
}
void Executor::f_sbc() {
    DWORD temp;
    WORD result;
    temp = (DWORD) this->readword(pD) - flags.C;
    result = (WORD) temp;

    flags.N = (temp >> 15) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = ((SWORD) this->readword(pD) < 0) && ((SWORD) result > 0);
    flags.C = this->readword(pD) == 0 && flags.C == 1;

    this->writeword(pD, (WORD) temp);
}
void Executor::f_scc() {
    flags.N = flags.Z = flags.V = flags.C = 1;
}
void Executor::f_sec() {
    flags.C = 1;
}
void Executor::f_sen() {
    flags.N = 1;
}
void Executor::f_sev() {
    flags.V = 1;
}
void Executor::f_sez() {
    flags.Z = 1;
}
void Executor::f_sob() {
    DIE("f_sob");
}
void Executor::f_spl() {
    DIE("f_spl");
}
void Executor::f_sub() {
    DWORD temp;
    WORD result;
    temp = (DWORD) this->readword(pD) - (DWORD) this->readword(pS);
    result = (WORD) temp;

    flags.N = (temp >> 15) & 1;
    flags.Z = result == 0 ? 1 : 0;
    flags.V = ((SWORD) this->readword(pS) < 0) != ((SWORD) this->readword(pD) < 0) &&
        ((SWORD) this->readword(pD) < 0) != ((SWORD) result < 0);
    flags.C = this->readword(pD) < this->readword(pS);

    this->writeword(pD, (WORD) temp);
}
void Executor::f_swab() {
    BYTE temp;
    temp = this->readword(pD);

    flags.N = (temp >> 7) & 1;
    flags.Z = temp == 0 ? 1 : 0;
    flags.V = flags.C = 0;

    this->writeword(pD, (this->readword(pD) >> 8) | ((WORD)temp << 8));
}
void Executor::f_sxt() {
    DIE("f_sxt");
}
void Executor::f_trap() {
    DIE("f_trap");
}
void Executor::f_tstb() {
    BYTE temp;
    temp = (BYTE) this->readword(pD);

    flags.N = (temp >> 7) & 1;
    flags.Z = temp == 0 ? 1 : 0;
    flags.V = 0;
    flags.C = 0;
}
void Executor::f_tst() {
    flags.N = (this->readword(pD) >> 15) & 1;
    flags.Z = this->readword(pD) == 0 ? 1 : 0;
    flags.V = 0;
    flags.C = 0;
}
void Executor::f_wait() {
    DIE("f_wait");
}
void Executor::f_xor() {
    DIE("f_xor");
}

Executor::Executor() {
#if ENABLE_TRACE  
    tracer = new Tracer(this);
#endif
}

Executor::~Executor() {
#if ENABLE_TRACE  
    delete tracer;
#endif
}
}
}  // namespace PlimDP
