#include <stdio.h>
#include <string.h>

#define K 1024
#define SRC 0
#define DTN 1
#define R 2
#define W 3
#define PC (reg[7])
#define SP (reg[6])
#define MEMSIZE (64*K)
#define ISTAT 0177560
#define IDATA 0177562
#define OSTAT 0177564
#define ODATA 0177566

#define T_DD 1
#define T_SSDD 2
#define T_RSS 3
#define T_XX 4
#define T_NONE 5
#define T_TT 6
#define T_R 7
#define T_RDD 8
#define T_NN 9
#define T_SS 10
#define T_RNN 11
#define T_N 12

typedef unsigned char BYTE;
typedef signed char SBYTE;
typedef unsigned short WORD;
typedef signed short SWORD;
typedef unsigned DWORD;
typedef signed SDWORD;

//GLOBAL****************************************************************
BYTE memory[MEMSIZE];
WORD dat[K];
WORD opcode, reg[8], oldPC;
BYTE dd, ss, mo, re, last_mo;
SBYTE xx;
BYTE N, Z, V, C;
BYTE idx, keyRW, countfrsp;
WORD *ptr, *ptrD, *ptrS;
BYTE pcsmflag, pcsmcnt;
//R/W_FUNCTIONS*********************************************************
BYTE checkmem(DWORD index, BYTE kol) {
	if(index < MEMSIZE) {
		if(kol == 1)
			return 1;
		if(kol == 2 && index % 2 == 0)
			return 1;
	}
	printf("Memory check failed");
	return 0;
}
void writebyte(DWORD index, BYTE x) {
	if(checkmem(index, 1))
		memory[index] = x;
	else
		printf("Error: writing byte outside the memory");
}
void writeword(DWORD index, WORD x) {
	if(checkmem(index, 2)) {
		ptr = &memory[index];
		*ptr = x;
	}
	else
		printf("Error: writing word outside the memory or bad address");
}
BYTE readbyte(DWORD index) {
	if(checkmem(index, 1))
		return memory[index];
	else {
		printf("Error: reading byte outside the memory");
		return 0;
	}
}
WORD readword(DWORD index) {
	if(checkmem(index, 2)) {
		ptr = &memory[index];
		return *ptr;
	}
	else {
		printf("Error: reading word outside the memory or bad address");
		return 0;
	}
}
//LOAD_FUNCTIONS********************************************************
void load(char* file) {
	FILE *fd;
	WORD b;
	WORD a, c;
	WORD i, k = 0;
	fd = fopen(file, "r");
	while(fscanf(fd, "%hx %hx", &a, &c) == 2) {
		dat[k++] = a;
		dat[k++] = c;
		for(i = 0; i < c; i++)
			if(fscanf(fd, "%hx", &b) == 1)
				writebyte(a+i, b);
	}
	fclose(fd);
}
//SPECIAL FUNCTIONS*****************************************************
void f_adcb(void) {
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
void f_adc(void) {
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
void f_add(void) {
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
void f_ash(void) {
	WORD temp;
	temp = (SWORD) *ptrS > 0 ? *ptrD << *ptrS : (SWORD) *ptrD >> (-(SWORD) *ptrS);
	
	N = (temp >> 15) & 1;
	Z = temp == 0 ? 1 : 0;
	V = ((SWORD) *ptrD < 0) != ((SWORD) temp < 0);
	C = (SWORD) *ptrS > 0 ? (*ptrD >> (16 - *ptrS)) & 1 : (SWORD) *ptrS < 0 ? (*ptrD >> ((-(SWORD)*ptrS) - 1)) & 1 : 0;

	*ptrD = temp;
}
void f_ashc(void) {
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
void f_aslb(void) {
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
void f_asl(void) {
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
void f_asrb(void) {
	BYTE temp;
	BYTE result;
	temp = *ptrD;
	result = (SBYTE) temp >> 1;
	
	N = (result >> 7) & 1;
	Z = result == 0 ? 1 : 0;
	V = (((SBYTE) temp < 0) && !(temp & 0x1)) || (((SBYTE) temp > 0) && (temp & 0x1));		//???
	C = temp & 0x1;
	
	*ptrD = (*ptrD & 0xff00) | (WORD) result;
}
void f_asr(void) {
	WORD temp;
	WORD result;
	temp = *ptrD;
	result = (SWORD) *ptrD >> 1;
	
	N = (result >> 15) & 1;
	Z = result == 0 ? 1 : 0;
	V = (((SWORD) temp < 0) && !(temp & 0x1)) || (((SWORD) temp > 0) && (temp & 0x1));		//???
	C = temp & 0x1;
	
	*ptrD = result;
}
void f_bcc(void) {
	if(C == 0)
		PC += 2*xx;
}
void f_bcs(void) {
	if(C == 1)
		PC += 2*xx;
}
void f_beq(void) {
	if(Z == 1)
		PC += 2*xx;
}
void f_bge(void) {
	if((N ^ V) == 0)
		PC += 2*xx;
}
void f_bgt(void) {
	if((Z | (N ^ V)) == 0)
		PC += 2*xx;
}
void f_bicb(void) {
	BYTE result;
	result = *ptrD & ~*ptrS;
	
	N = (result >> 7) & 1;
	Z = result == 0 ? 1 : 0;
	V = 0;
	
	*ptrD = (*ptrD & 0xff00) | (WORD) result;
}
void f_bic(void) {
	WORD result;
	result = *ptrD & ~*ptrS;
	
	N = (result >> 15) & 1;
	Z = result == 0 ? 1 : 0;
	V = 0;
	
	*ptrD = *ptrD & ~*ptrS;
}
void f_bisb(void) {
	BYTE result;
	result = *ptrD | *ptrS;
	
	N = (result >> 7) & 1;
	Z = result == 0 ? 1 : 0;
	V = 0;
	
	*ptrD = (*ptrD & 0xff00) | (WORD) result;
}
void f_bis(void) {
	WORD result;
	result = *ptrD | *ptrS;
	
	N = (result >> 15) & 1;
	Z = result == 0 ? 1 : 0;
	V = 0;
	
	*ptrD = *ptrD | *ptrS;
}
void f_bitb(void) {
	BYTE result;
	result = *ptrD & *ptrS;
	
	N = (result >> 7) & 1;
	Z = result == 0 ? 1 : 0;
	V = 0;
}
void f_bit(void) {
	WORD result;
	result = *ptrD & *ptrS;
	
	N = (result >> 15) & 1;
	Z = result == 0 ? 1 : 0;
	V = 0;
}
void f_bhi(void) {
	if((C | Z) == 0)
		PC += 2*xx;
}
void f_ble(void) {
	if((Z | (N ^ V)) == 1)
		PC += 2*xx;
}
void f_blt(void) {
	if((N ^ V) == 1)
		PC += 2*xx;
}
void f_blos(void) {
	if((C | Z) == 1)
		PC += 2*xx;
}
void f_bmi(void) {
	if(N == 1)
		PC += 2*xx;
}
void f_bne(void) {
	if(Z == 0)
		PC += 2*xx;
}
void f_bpl(void) {
	if(N == 0)
		PC += 2*xx;
}
void f_bpt(void) {
	printf("f_bpt");
}
void f_br(void) {
	PC += 2*xx;
}
void f_bvc(void) {
	if(V == 0)
		PC += 2*xx;
}
void f_bvs(void) {
	if(V == 1)
		PC += 2*xx;
}
void f_ccc(void) {
	N = Z = V = C = 0;
}
void f_clc(void) {
	C = 0;
}
void f_cln(void) {
	N = 0;
}
void f_clrb(void) {
	N = 0;
	Z = 1;
	V = 0;
	C = 0;
	
	*ptrD = *ptrD & 0xff00;
}
void f_clr(void) {
	N = 0;
	Z = 1;
	V = 0;
	C = 0;
	
	*ptrD = 0;
}
void f_clv(void) {
	V = 0;
}
void f_clz(void) {
	Z = 0;
}
void f_cmpb(void) {
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
void f_cmp(void) {
	WORD result;
	result = *ptrS - *ptrD;
	
	N = (result >> 15) & 1;
	Z = result == 0 ? 1 : 0;
	V = ((SWORD) *ptrS < 0) != ((SWORD) *ptrD < 0) && ((SWORD) *ptrS < 0) != ((SWORD) result < 0);
	C = *ptrS < *ptrD;
}
void f_comb(void) {
	BYTE result;
	result = ~*ptrD;
	
	N = (result >> 7) & 1;
	Z = result == 0 ? 1 : 0;
	V = 0;
	C = 1;
	
	*ptrD = (*ptrD & 0xff00) | (WORD) result;
}
void f_com(void) {
	WORD result;
	result = ~*ptrD;
	
	N = (result >> 15) & 1;
	Z = result == 0 ? 1 : 0;
	V = 0;
	C = 1;
	
	*ptrD = ~*ptrD;
}
void f_decb(void) {
	BYTE result;
	BYTE temp;
	temp = (BYTE) (*ptrD & 0xff);
	result = temp - 1;
	
	N = (result >> 7) & 1;
	Z = result == 0 ? 1 : 0;
	V = ((SBYTE) temp < 0) && ((SBYTE) temp < 0) != ((SBYTE) result < 0);
	
	*ptrD = (*ptrD & 0xff00) | (WORD) result;
}
void f_dec(void) {
	WORD result;
	result = *ptrD - 1;
	
	N = (result >> 15) & 1;
	Z = result == 0 ? 1 : 0;
	V = ((SWORD) *ptrD < 0) && ((SWORD) *ptrD < 0) != ((SWORD) result < 0);
	
	*ptrD -= 1;
}
void f_div(void) {
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
void f_emt(void) {
	printf("f_emt");
}
void f_fadd(void) {
	printf("f_add");
}
void f_fdiv(void) {
	printf("f_fdiv");
}
void f_fmul(void) {
	printf("f_fmul");
}
void f_fsub(void) {
	printf("f_fsub");
}
void f_halt(void) {
}
void f_incb(void) {
	BYTE result;
	BYTE temp;
	temp = (BYTE) (*ptrD & 0xff);
	result = temp + 1;
	
	N = (result >> 7) & 1;
	Z = result == 0 ? 1 : 0;
	V = ((SBYTE) temp > 0) && ((SBYTE) temp < 0) != ((SBYTE) result < 0);
	
	*ptrD = (*ptrD & 0xff00) | (WORD) result;
}
void f_inc(void) {
	WORD result;
	result = *ptrD + 1;
	
	N = (result >> 15) & 1;
	Z = result == 0 ? 1 : 0;
	V = ((SWORD) *ptrD > 0) && ((SWORD) *ptrD < 0) != ((SWORD) result < 0);
	
	*ptrD += 1;
}
void f_iot(void) {
	printf("f_iot");
}
void f_jmp(void) {
	ptr = memory;
	PC = 2 * (ptrD - ptr);
}
void f_jsr(void) {
	SP -= 2;
	writeword(SP, *ptrS);
	*ptrS = PC;
	ptr = memory;
	PC = 2 * (ptrD - ptr);
}
void f_mark(void) {
	printf("f_mark");
}
void f_mfpd(void) {
	printf("f_mfpd");
}
void f_mfpi(void) {
	printf("f_mfpi");
}
void f_mfps(void) {
	printf("f_mfps");
}
void f_movb(void) {
	BYTE temp;
	if(last_mo) {
		ptr = memory;
		if(2 * (ptrS - ptr) == IDATA)
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
void f_mov(void) {
	if(last_mo) {
		ptr = memory;
		if(2 * (ptrS - ptr) == IDATA)
			*ptrS = getchar();
	}
	
	N = (*ptrS >> 15) & 1;
	Z = *ptrS == 0 ? 1 : 0;
	V = 0;

	*ptrD = *ptrS;
}
void f_mtpd(void) {
	printf("f_mtpd");
}
void f_mtpi(void) {
	printf("f_mtpi");
}
void f_mtps(void) {
	printf("f_mtps");
}
void f_mul(void) {
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
void f_negb(void) {
	BYTE result;
	result = ~*ptrD + 1;
	
	N = (result >> 7) & 1;
	Z = result == 0 ? 1 : 0;
	V = result == 0200 ? 1 : 0;
	C = result == 0 ? 0 : 1;
	
	*ptrD = (*ptrD & 0xff00) | (WORD) result;
}
void f_neg(void) {
	WORD result;
	result = ~*ptrD + 1;
	
	N = (result >> 15) & 1;
	Z = result == 0 ? 1 : 0;
	V = result == 0100000 ? 1 : 0;
	C = result == 0 ? 0 : 1;
	
	*ptrD = ~*ptrD + 1;
}
void f_nop(void) {
	printf("f_nop");
}
void f_reset(void) {
	printf("f_reset");
}
void f_rolb(void) {
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
void f_rol(void) {
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
void f_rorb(void) {
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
void f_ror(void) {
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
void f_rti(void) {
	printf("f_rti");
}
void f_rts(void) {
	PC = *ptrD;
	*ptrD = readword(SP);
	SP += 2;
}
void f_rtt(void) {
	printf("f_rtt");
}
void f_sbcb(void) {
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
void f_sbc(void) {
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
void f_scc(void) {
	N = Z = V = C = 1;
}
void f_sec(void) {
	C = 1;
}
void f_sen(void) {
	N = 1;
}
void f_sev(void) {
	V = 1;
}
void f_sez(void) {
	Z = 1;
}
void f_sob(void) {
	printf("f_sob");
}
void f_spl(void) {
	printf("f_spl");
}
void f_sub(void) {
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
void f_swab(void) {
	BYTE temp;
	temp = *ptrD;
	
	N = (temp >> 7) & 1;
	Z = temp == 0 ? 1 : 0;
	V = C = 0;
	
	*ptrD = (*ptrD >> 8) | ((WORD)temp << 8);
}
void f_sxt(void) {
	printf("f_sxt");
}
void f_trap(void) {
	printf("f_trap");
}
void f_tstb(void) {
	BYTE temp;
	temp = (BYTE) *ptrD;
	
	N = (temp >> 7) & 1;
	Z = temp == 0 ? 1 : 0;
	V = 0;
	C = 0;
}
void f_tst(void) {
	N = (*ptrD >> 15) & 1;
	Z = *ptrD == 0 ? 1 : 0;
	V = 0;
	C = 0;
}
void f_wait(void) {
	printf("f_wait");
}
void f_xor(void) {
	printf("f_xor");
}
//STRUCTURE*************************************************************
struct {
		char *name;
        WORD code;
        WORD mask;
        BYTE type;
        void (*exec)(void);
        BYTE size;
} instrs[] = {
	{"adcb", 0105500, 0177700, T_DD,   f_adcb, 1},
	{"adc",  0005500, 0177700, T_DD,   f_adc,  2},
	{"add",  0060000, 0170000, T_SSDD, f_add,  2},
	{"ash",  0072000, 0177000, T_RSS,  f_ash,  2},
	{"ashc", 0073000, 0177000, T_RSS,  f_ashc, 2},
	{"aslb", 0106300, 0177700, T_DD,   f_aslb, 1},
	{"asl",  0006300, 0177700, T_DD,   f_asl,  2},
	{"asrb", 0106200, 0177700, T_DD,   f_asrb, 1},
	{"asr",  0006200, 0177700, T_DD,   f_asr,  2},
	{"bcc",  0x8600,  0xff00,  T_XX,   f_bcc,  2},
	{"bcs",  0x8700,  0xff00,  T_XX,   f_bcs,  2},
	{"beq",  0x0300,  0xff00,  T_XX,   f_beq,  2},
	{"bge",  0x0400,  0xff00,  T_XX,   f_bge,  2},
	{"bgt",  0x0600,  0xff00,  T_XX,   f_bgt,  2},
	{"bicb", 0140000, 0170000, T_SSDD, f_bicb, 1},
	{"bic",  0040000, 0170000, T_SSDD, f_bic,  2},
	{"bisb", 0150000, 0170000, T_SSDD, f_bisb, 1},
	{"bis",  0050000, 0170000, T_SSDD, f_bis,  2},
	{"bitb", 0130000, 0170000, T_SSDD, f_bitb, 1},
	{"bit",  0030000, 0170000, T_SSDD, f_bit,  2},
	{"bhi",  0x8200,  0xff00,  T_XX,   f_bhi,  2},
	{"ble",  0x0700,  0xff00,  T_XX,   f_ble,  2},
	{"blt",  0x0500,  0xff00,  T_XX,   f_blt,  2},
	{"blos", 0x8300,  0xff00,  T_XX,   f_blos, 2},
	{"bmi",  0x8100,  0xff00,  T_XX,   f_bmi,  2},
	{"bne",  0x0200,  0xff00,  T_XX,   f_bne,  2},
	{"bpl",  0x8000,  0xff00,  T_XX,   f_bpl,  2},
	{"bpt",  0000003, 0177777, T_NONE, f_bpt,  2},
	{"br",   0x0100,  0xff00,  T_XX,   f_br,   2},
	{"bvc",  0x8400,  0xff00,  T_XX,   f_bvc,  2},
	{"bvs",  0x8500,  0xff00,  T_XX,   f_bvs,  2},
	{"ccc",  0000257, 0177777, T_NONE, f_ccc,  2},
	{"clc",  0000241, 0177777, T_NONE, f_clc,  2},
	{"cln",  0000250, 0177777, T_NONE, f_cln,  2},
	{"clrb", 0105000, 0177700, T_DD,   f_clrb, 1},
	{"clr",  0005000, 0177700, T_DD,   f_clr,  2},
	{"clv",  0000242, 0177777, T_NONE, f_clv,  2},
	{"clz",  0000244, 0177777, T_NONE, f_clz,  2},
	{"cmpb", 0120000, 0170000, T_SSDD, f_cmpb, 1},
	{"cmp",  0020000, 0170000, T_SSDD, f_cmp,  2},
	{"comb", 0105100, 0177700, T_DD,   f_comb, 1},
	{"com",  0005100, 0177700, T_DD,   f_com,  2},
	{"decb", 0105300, 0177700, T_DD,   f_decb, 1},
	{"dec",  0005300, 0177700, T_DD,   f_dec,  2},
	{"div",  0071000, 0177000, T_RSS,  f_div,  2},
	{"emt",  0104000, 0177700, T_TT,   f_emt,  2},
	{"fadd", 0075000, 0177770, T_R,    f_fadd, 2},
	{"fdiv", 0075030, 0177770, T_R,    f_fdiv, 2},
	{"fmul", 0075020, 0177770, T_R,    f_fmul, 2},
	{"fsub", 0075010, 0177770, T_R,    f_fsub, 2},
	{"halt", 0000000, 0177777, T_NONE, f_halt, 2},
	{"incb", 0105200, 0177700, T_DD,   f_incb, 1},
	{"inc",  0005200, 0177700, T_DD,   f_inc,  2},
	{"iot",  0000004, 0177777, T_NONE, f_iot,  2},
	{"jmp",  0000100, 0177700, T_DD,   f_jmp,  2},
	{"jsr",  0004000, 0177000, T_RDD,  f_jsr,  2},
	{"mark", 0006400, 0177700, T_NN,   f_mark, 2},
	{"mfpd", 0106500, 0177700, T_SS,   f_mfpd, 2},
	{"mfpi", 0006500, 0177700, T_SS,   f_mfpi, 2},
	{"mfps", 0106700, 0177700, T_DD,   f_mfps, 2},
	{"movb", 0110000, 0170000, T_SSDD, f_movb, 1},
	{"mov",  0010000, 0170000, T_SSDD, f_mov,  2},
	{"mtpd", 0106600, 0177700, T_DD,   f_mtpd, 2},
	{"mtpi", 0006600, 0177700, T_DD,   f_mtpi, 2},
	{"mtps", 0106400, 0177700, T_SS,   f_mtps, 2},
	{"mul",  0070000, 0177000, T_RSS,  f_mul,  2},
	{"negb", 0105400, 0177700, T_DD,   f_negb, 1},
	{"neg",  0005400, 0177700, T_DD,   f_neg,  2},
	{"nop",  0000240, 0177777, T_NONE, f_nop,  2},
	{"reset",  0000005, 0177777, T_NONE, f_reset, 2},
	{"rolb", 0106100, 0177700, T_DD,   f_rolb, 1},
	{"rol",  0006100, 0177700, T_DD,   f_rol,  2},
	{"rorb", 0106000, 0177700, T_DD,   f_rorb, 1},
	{"ror",  0006000, 0177700, T_DD,   f_ror,  2},
	{"rti",  0000002, 0177777, T_NONE, f_rti,  2},
	{"rts",  0000200, 0177770, T_R,    f_rts,  2},
	{"rtt",  0000006, 0177777, T_NONE, f_rtt,  2},
	{"sbcb", 0105600, 0177700, T_DD,   f_sbcb, 1},
	{"sbc",  0005600, 0177700, T_DD,   f_sbc,  2},
	{"scc",  0000277, 0177777, T_NONE, f_scc,  2},
	{"sec",  0000261, 0177777, T_NONE, f_sec,  2},
	{"sen",  0000270, 0177777, T_NONE, f_sen,  2},
	{"sev",  0000262, 0177777, T_NONE, f_sev,  2},
	{"sez",  0000264, 0177777, T_NONE, f_sez,  2},
	{"sob",  0077000, 0177000, T_RNN,  f_sob,  2},
	{"spl",  0000230, 0177770, T_N,    f_spl,  2},
	{"sub",  0160000, 0170000, T_SSDD, f_sub,  2},
	{"swab", 0000300, 0177700, T_DD,   f_swab, 2},
	{"sxt",  0006700, 0177700, T_DD,   f_sxt,  2},
	{"trap", 0104400, 0177700, T_TT,   f_trap, 2},
	{"tstb", 0105700, 0177700, T_DD,   f_tstb, 1},
	{"tst",  0005700, 0177700, T_DD,   f_tst,  2},
	{"wait", 0000001, 0177777, T_NONE, f_wait, 2},
	{"xor",  0074000, 0177000, T_RDD,  f_xor,  2},
};
//PRINTER***************************************************************
void print_mn(void) {
	printf("\n%06o: ", PC - 2);
	printf("%s\t", instrs[idx].name);
	countfrsp = 24;
}
void print_reg(void) {
	BYTE i;
	WORD temp1, temp2;
	char frsp = ' ';
	printf("%*c", countfrsp, frsp);
	if(pcsmflag == 0)
		printf("[%06o]\n", opcode);
	else if(pcsmflag == 1) {
		temp1 = readword(oldPC - 2);
		printf("[%06o %06o]\n", opcode, temp1);
	}
	else if(pcsmflag == 2) {
		temp2 = readword(oldPC - 2);
		printf("[%06o %06o]\n", opcode, temp2);
	}
	else if(pcsmflag == 3) {
		temp1 = readword(oldPC - 4);
		temp2 = readword(oldPC - 2);
		printf("[%06o %06o %06o]\n", opcode, temp1, temp2);
	}
	pcsmflag = pcsmcnt = 0;
	if(N == 1)
		printf("N");
	else
		printf("-");
	if(Z == 1)
		printf("Z");
	else
		printf("-");
	if(V == 1)
		printf("V");
	else
		printf("-");
	if(C == 1)
		printf("C");
	else
		printf("-");
	printf(" ");
	for(i = 0; i < 8; i++)
		printf("%hd:%06o ", i, reg[i]);
}
void print_op(void) {
	WORD nt_cm;
	nt_cm = readword(PC);
	pcsmcnt++;
	switch(mo) {
		case 0:
			printf("r%ho", re);
			countfrsp -= 2;
			break;
		case 1:
			printf("(r%ho)", re);
			countfrsp -= 4;
			break;
		case 2:
			if(re == 7) {
				printf("#%06ho", nt_cm);
				countfrsp -= 7;
				if(pcsmcnt == 1)
					pcsmflag++;
				else
					pcsmflag += 2;
			}
			else {
				printf("(r%ho)+", re);
				countfrsp -= 5;
			}
			break;
		case 3:
			if(re == 7) {
				printf("@#%06ho", nt_cm);
				countfrsp -= 8;
				if(pcsmcnt == 1)
					pcsmflag++;
				else
					pcsmflag += 2;
			}
			else {
				printf("@(r%ho)+", re);
				countfrsp -= 6;
			}
			break;
		case 4:
			printf("-(r%ho)", re);
			countfrsp -= 5;
			break;
		case 5:
			printf("@-(r%ho)", re);
			countfrsp -= 6;
			break;
		case 6:
			if(re == 7) {
				printf("%06ho", nt_cm + PC + 2);
				countfrsp -= 6;
				if(pcsmcnt == 1)
					pcsmflag++;
				else
					pcsmflag += 2;
			}
			else {
				printf("%06ho(r%ho)", nt_cm, re);
				countfrsp -= 10;
				if(pcsmcnt == 1)
					pcsmflag++;
				else
					pcsmflag += 2;
			}
			break;
		case 7:
			if(re == 7) {
				printf("@%06ho", nt_cm + PC + 2);
				countfrsp -= 7;
				if(pcsmcnt == 1)
					pcsmflag++;
				else
					pcsmflag += 2;
			}
			else {
				printf("@%06ho(r%ho)", nt_cm, re);
				countfrsp -= 11;
				if(pcsmcnt == 1)
					pcsmflag++;
				else
					pcsmflag += 2;
			}
			break;
	}
}
void print_aim(void) {
	printf("%06o", PC + 2*xx);
	countfrsp -= 6;
}
void print_end(void) {
	printf("r0=%06o r2=%06o r4=%06o sp=%06o\n", reg[0], reg[2], reg[4], SP);
	printf("r1=%06o r3=%06o r4=%06o pc=%06o\n", reg[1], reg[3], reg[5], PC);
	printf("n=%ho z=%ho v=%ho c=%ho\n", N, Z, V, C);
}
//DECODER***************************************************************
void select_operand(BYTE ident) {
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
				if(checkmem(reg[re], 1))
					ptrS = &memory[reg[re]];
			}
			else if(ident == DTN) {
				if(checkmem(reg[re], 1))
					ptrD = &memory[reg[re]];
			}
			break;
		case 2:
			if(ident == SRC) {
				if(checkmem(reg[re], 1))
					ptrS = &memory[reg[re]];
			}
			else if(ident == DTN) {
				if(checkmem(reg[re], 1))
					ptrD = &memory[reg[re]];
			}
			if(instrs[idx].size == 1 && re < 6)
				reg[re]++;
			else if(instrs[idx].size == 2 || re >= 6)
				reg[re] += 2;
			break;
		case 3:
			if(ident == SRC) {
				if(checkmem(reg[re], 2) && checkmem(readword(reg[re]), 1))
					ptrS = &memory[readword(reg[re])];
			}
			else if(ident == DTN) {
				if(checkmem(reg[re], 2) && checkmem(readword(reg[re]), 1))
					ptrD = &memory[readword(reg[re])];
			}
			reg[re] += 2;
			break;
		case 4:
			if(instrs[idx].size == 1 && re < 6)
				reg[re]--;
			else if(instrs[idx].size == 2 || re >= 6)
				reg[re] -= 2;
			if(ident == SRC) {
				if(checkmem(reg[re], 1))
					ptrS = &memory[reg[re]];
			}
			else if(ident == DTN) {
				if(checkmem(reg[re], 1))
					ptrD = &memory[reg[re]];
			}
			break;
		case 5:
			reg[re] -= 2;
			if(ident == SRC) {
				if(checkmem(reg[re], 2) && checkmem(readword(reg[re]), 1))
					ptrS = &memory[readword(reg[re])];
			}
			else if(ident == DTN) {
				if(checkmem(reg[re], 2) && checkmem(readword(reg[re]), 1))
					ptrD = &memory[readword(reg[re])];
			}
			break;
		case 6:
			offset = readword(PC);
			PC += 2;
			if(ident == SRC) {
				if(checkmem((WORD)(reg[re] + offset), 1))
					ptrS = &memory[(WORD)(reg[re] + offset)];
			}
			else if(ident == DTN) {
				if(checkmem((WORD)(reg[re] + offset), 1))
					ptrD = &memory[(WORD)(reg[re] + offset)];
			}
			break;
		case 7:
			offset = readword(PC);
			PC += 2;
			if(ident == SRC) {
				if(checkmem((WORD)(reg[re] + offset), 2) && checkmem(readword((WORD)(reg[re] + offset)), 1))
					ptrS = &memory[readword((WORD)(reg[re] + offset))];
			}
			else if(ident == DTN) {
				if(checkmem((WORD)(reg[re] + offset), 2) && checkmem(readword((WORD)(reg[re] + offset)), 1))
					ptrD = &memory[readword((WORD)(reg[re] + offset))];
			}
			break;
	}
}
BYTE decode_m(BYTE a) {
	return (a & 070) >> 3;
}
BYTE decode_r(BYTE a) {
	return a & 07;
}
void decode(BYTE mode) {
	last_mo = 0;
	switch(instrs[idx].type) {
		case T_SSDD:
				ss = (opcode & 0007700) >> 6;
			mo = decode_m(ss);
			re = decode_r(ss);
			if(mode == W) {
				print_op();
				printf(",");
				countfrsp -= 1;
			}
			select_operand(SRC);
			last_mo = mo;
				dd = opcode & 0000077;
			mo = decode_m(dd);
			re = decode_r(dd);
			if(mode == W)
				print_op();
			select_operand(DTN);
			break;
		case T_DD:
				dd = opcode & 0000077;
			mo = decode_m(dd);
			re = decode_r(dd);
			if(mode == W)
				print_op();
			select_operand(DTN);
			break;
		case T_XX:
				xx = opcode & 0xFF;
			if(mode == W)
				print_aim();
			break;
		case T_RSS:
				ss = opcode & 0000077;
			mo = decode_m(ss);
			re = decode_r(ss);
			if(mode == W) {
				print_op();
				printf(",");
				countfrsp -= 1;
			}
			select_operand(SRC);
			last_mo = mo;
			mo = 0;
			re = (opcode & 0000700) >> 6;
			if(mode == W)
				print_op();
			select_operand(DTN);
			break;
		case T_RDD:
			mo = 0;
			re = (opcode & 0000700) >> 6;
			if(mode == W) {
				print_op();
				printf(",");
				countfrsp -= 1;
			}
			select_operand(SRC);
			last_mo = mo;
				dd = opcode & 0000077;
			mo = decode_m(dd);
			re = decode_r(dd);
			if(mode == W)
				print_op();
			select_operand(DTN);
			break;
		case T_R:
			mo = 0;
			re = opcode & 0000007;
			if(mode == W)
				print_op();
			select_operand(DTN);
			break;
		case T_NONE:
			break;
	}
}
//MAIN_FUNCTIONS********************************************************
BYTE find_instrs(void) {
	BYTE i;
	for(i = 0; i < sizeof(instrs)/sizeof(instrs[0]); i++)
		if((opcode & instrs[i].mask) == instrs[i].code)
			return i;
	printf("Such function doesn't exist");
	return 0;
}
void prep_devices(void) {
	writeword(ISTAT, 0200);
	writeword(OSTAT, 0200);
	writeword(IDATA, 0);
	writeword(ODATA, 0);
}
void output(void) {
	if(readword(ODATA))
		printf("%c", readword(ODATA));
}
void pdp11(void) {
	printf("\n---------------- running --------------\n");
	PC = 0x200;
	do {
		output();
		prep_devices();
		opcode = readword(PC);		//
		PC += 2;					//
		idx = find_instrs();		//
  		if(keyRW == W) {
			print_mn();
			decode(W);
		}
		else
			decode(R);
		oldPC = PC;
		instrs[idx].exec();			//
		if(keyRW == W)
			print_reg();
	}  while(opcode);
	printf("\n---------------- halted ---------------\n");
	print_end();
}
//**********************************************************************
int main(int ac, char *av[]) {
	if(ac == 1)
		printf("error");
	else {
		load(av[1]);
		if(ac > 2 && !strcmp(av[2], "-t"))
			keyRW = W;
		else
			keyRW = R;
		pdp11();
	}
	return 0;
}
