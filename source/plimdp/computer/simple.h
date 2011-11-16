/*
 * simple.h
 * 
 * PlimDP+ simple computer
 *
 * Copyright 2009 (C) Boris Belousov
 * Copyright 2011 (C) Pavel Kryukov (remastering)
*/

#ifndef SIMPLE_H
#define SIMPLE_H

#include <string>

#define K 1024

namespace PlimDP {
namespace CPU {
    class Core;
}
namespace Devices {
    template<unsigned int> class Memory;
    class Loader;
    class IO;
}  
    
namespace Computer {
class Simple {
  private:
    PlimDP::CPU::Core* core;
    PlimDP::Devices::Memory<64 * K>* mem;
    PlimDP::Devices::Loader* loader;
    PlimDP::Devices::IO* io;
  public:
    Simple();
    ~Simple();
    void load(const std::string & file);
    void start();
};
}
}

#endif
