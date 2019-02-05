#ifndef DMA_H
#define DMA_H

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
    void SetDPTRL(BYTE by);
    BYTE GetDPTRL();
    void SetDPTRH(BYTE by);
    BYTE GetDPTRH();

    void SetDBKRL(BYTE by);
    BYTE GetDBKRL();
    void SetDBKRH(BYTE by);
    BYTE GetDBKRH();

    void SetDCNTL(BYTE by);
    BYTE GetDCNTL();
    void SetDCNTH(BYTE by);
    BYTE GetDCNTH();
    void Execute();

};
#endif // DMA_H
