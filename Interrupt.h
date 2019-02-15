#ifndef INTERRUPT_H
#define INTERRUPT_H

#define BTEN 0x2A
#define BTREQ 0x2B
#define BTC 0x2C
#define IREQL 0x3C
#define IENAL 0x3E

class Timer;
class CPU;

class BTInterrupt {
public:
    CPU* cpu;
    unsigned long long int clock;
    Timer* timer;

    BTInterrupt(CPU* cpu);
    bool Update();

};

class PTInterrupt {
public:
    CPU* cpu;
    bool triggered;

    PTInterrupt(CPU* cpu);
    void Trigger();
    bool Update();
};
#endif // INTERRUPT_H
