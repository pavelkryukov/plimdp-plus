/*
 * flags.h
 *
 * PlimDP+ core flags file
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#ifndef FLAGS_H
#define FLAGS_H

#include <plimdp/generic/types.h>

namespace PlimDP {
namespace CPU {
class Flags {
    public:
        bool N, Z, V, C;
        inline Flags() : N(false), Z(false), V(false), C(false) {}
};
}
}

#endif
