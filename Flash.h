#ifndef FLASH_H
#define FLASH_H
#define FLASH_SIZE 0x200000
#define BYTE unsigned char

#define SECTOR_SIZE 0x1000
#define BLOCK_SIZE 0x10000

#define READ_MODE 0
#define COMMAND_MODE 1
#define SECTOR_ERASE_MODE 2
#define BLOCK_ERASE_MODE 3
#define BYTE_PROGRAM_MODE 4
class Flash {
public:
    BYTE memory[FLASH_SIZE];
    BYTE mode;
    unsigned long long int current_command;
    BYTE write_cycle;
    unsigned short last_operation_address;

    Flash();
    unsigned short GetExpectedWriteAddress();
    void Write(unsigned int address, BYTE by);
    BYTE Read(unsigned int address);
    void ResetWriteCycle();
    BYTE GetStatusRegister();
    void SectorErase(unsigned short address);
    void BlockErase(unsigned short address);
    void ChipErase();
    void ByteProgram(unsigned short address, BYTE by);
};

#endif // FLASH_H
