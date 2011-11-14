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

#include "./decoder.h"

namespace PlimDP {
class Core : public Decoder {
  public:
    void start();
};
}

#endif
