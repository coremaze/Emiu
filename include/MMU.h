#ifndef MMU_H
#define MMU_H
class CPU;
class DMA;
class Flash;

#include "types.h"

#define BRR_START 0x2000
#define BRR_LENGTH 0x2000
#define PRR_START 0x4000
#define PRR_LENGTH 0x4000
#define DRR_START 0x8000
#define DRR_LENGTH 0x8000
#define REGISTERS_START 0x0
#define REGISTERS_LENGTH 0x80

#define BTREQ 0x2B

class MMU {
public:
    CPU* cpu;
    MMU(CPU* cpu);
    void StoreByte(unsigned short address, u8 by);
    u8 ReadByte(unsigned short address);
    void StoreShort(unsigned short address, unsigned short sh);
    unsigned short ReadShort(unsigned short address);
};
#endif // MMU_H
