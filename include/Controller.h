#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "SDL2/SDL.h"

class CPU;
class MMU;
class PTInterrupt;

class Controller{
public:
    CPU* cpu;
    unsigned short controls;
    Controller(CPU* cpu);
    void Update();
};
#endif // CONTROLLER_H
