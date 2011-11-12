/*
 * macro.h
 * 
 * PlimDP+ macro definitions
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#ifndef MACRO_H
#define MACRO_H

#include <cassert>
#include <cstdio>

#define ASSERTX(C) assert(C)
#define DIE(M) std::fprintf(stderr, "ASSERT: %s\n", (M)), assert(0)
#define ASSERT(C, M) if (!(C)) DIE(M)

#endif
