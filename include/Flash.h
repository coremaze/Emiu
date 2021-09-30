#ifndef FLASH_H
#define FLASH_H
#include "types.h"

#define FLASH_SIZE 0x200000

#define SECTOR_SIZE 0x1000
#define BLOCK_SIZE 0x10000

#define READ_MODE 0
#define COMMAND_MODE 1
#define SECTOR_ERASE_MODE 2
#define BLOCK_ERASE_MODE 3
#define BYTE_PROGRAM_MODE 4

class Flash {
public:
    u8 memory[FLASH_SIZE];
    u8 mode;
    unsigned long long int current_command;
    u8 write_cycle;
    unsigned int last_operation_address;

    Flash();
    unsigned int GetExpectedWriteAddress();
    void Write(unsigned int address, u8 by);
    u8 Read(unsigned int address);
    void ResetWriteCycle();
    u8 GetStatusRegister();
    void SectorErase(unsigned int address);
    void BlockErase(unsigned int address);
    void ChipErase();
    void ByteProgram(unsigned int address, u8 by);
    void Save(const char* fileName);
};

#endif // FLASH_H
