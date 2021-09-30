#ifndef DMA_H
#define DMA_H

#include "types.h"

#define DPTR 0x58
#define DBKR 0x5A
#define DCNT 0x5C
#define DSEL 0x5E
#define DMOD 0x5F

class CPU;

class DMA {
public:
    CPU* cpu;
    unsigned short sourcePtr;
    unsigned short destPtr;
    unsigned short sourceBank;
    unsigned short destBank;
    unsigned short number;
    DMA(CPU* cpu);
    void SetDPTRL(u8 by);
    u8 GetDPTRL();
    void SetDPTRH(u8 by);
    u8 GetDPTRH();

    void SetDBKRL(u8 by);
    u8 GetDBKRL();
    void SetDBKRH(u8 by);
    u8 GetDBKRH();

    void SetDCNTL(u8 by);
    u8 GetDCNTL();
    void SetDCNTH(u8 by);
    u8 GetDCNTH();
    void Execute();

};
#endif // DMA_H
