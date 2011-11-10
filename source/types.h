/*
 * types.h
 * 
 * PlimDP++ types file
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#ifndef TYPES_H
#define TYPES_H

namespace PlimDP {

typedef unsigned char BYTE;
typedef signed char SBYTE;
typedef unsigned short WORD;
typedef signed short SWORD;
typedef unsigned DWORD;
typedef signed SDWORD;

enum KeyRW {
    READ,
    WRITE,
};
}

#endif
