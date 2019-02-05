#ifndef CPU_H
#define CPU_H
class Display;
class MMU;
class DMA;

#define STACK_START 0x100
#define IRR 0x30
#define PRR 0x32
#define DRR 0x34
#define BRR 0x36
#define BYTE unsigned char

#define OTP_SIZE 0x4000
#define FLASH_SIZE 0x200000

class CPU {
public:
    Display* display;

    //Flags
    bool c; //Carry
    bool z; //Zero
    bool i; //Interrupt Disable
    bool d; //Decimal Mode
    bool b; //Break
    bool v; //Overflow
    bool n; //Negative

    //Registers
    BYTE A; //Accumulator
    BYTE X; //X
    BYTE Y; //Y
    unsigned short S; //Stack
    unsigned short PC; //Program Counter

    //Memory
    BYTE memRegisters[0x80];
    BYTE RAM[0x10000];
    BYTE videoRegisters[0x2];
    BYTE OTP[OTP_SIZE];
    BYTE Flash[FLASH_SIZE];

    bool interrupted;

    MMU* mmu;

    DMA* dma;

    unsigned short pre_interrupt_PRR;

    CPU(char* OTPFile, char* flashFile);
    void PrintState();
    unsigned short GetIRR();
    unsigned short GetPRR();
    unsigned short GetDRR();
    unsigned short GetBRR();
    void BeginInterrupt();
    void EndInterrupt();
    unsigned short GetResetVector();
    void Reset();
    unsigned short GetBTVector();
    unsigned short GetPTVector();
    void WriteVideoRegister(unsigned short address, BYTE by);

    BYTE ImmediateVal();

    BYTE ZeroPagePtr();
    BYTE ZeroPageVal();

    BYTE ZeroPageXPtr();
    BYTE ZeroPageXVal();

    BYTE ZeroPageYPtr();
    BYTE ZeroPageYVal();

    unsigned short RelativePtr();
    BYTE RelativeVal();

    unsigned short AbsolutePtr();
    BYTE AbsoluteVal();

    unsigned short AbsoluteXPtr();
    BYTE AbsoluteXVal();

    unsigned short AbsoluteYPtr();
    BYTE AbsoluteYVal();

    unsigned short IndirectPtr();
    unsigned short IndirectVal();

    unsigned short IndirectZeroPagePtr();
    BYTE IndirectZeroPageVal();

    unsigned short IndexedIndirectPtr();
    BYTE IndexedIndirectVal();

    unsigned short IndirectIndexedPtr();
    BYTE IndirectIndexedVal();

    unsigned short AbsoluteIndexIndirectPtr();
    unsigned short AbsoluteIndexIndirectVal();

    void Push(BYTE by);
    void PushShort(unsigned short sh);

    BYTE Pop();
    unsigned short PopShort();

    BYTE ExportFlags();
    void ImportFlags(BYTE flags);

    bool Step();

    void PHP(); //08
    void ORA_I(); //09
    void ORA_A(); //0D
    void JSR(); //20
    void PLP(); //28
    void AND_I(); //29
    void PHA(); //48
    void JMP_A(); //4C
    void CLI(); //58
    void RTS(); //60
    void STZ_ZP(); //64
    void RMB6_ZP(); //67
    void PLA(); //68
    void SEI(); //78
    void BRA(); //80
    void STA_ZP(); //85
    void SMB0_ZP(); //87
    void DEY(); //88
    void STA_A(); //8D
    void STX_A(); //8E
    void BCC(); //90
    void STA_IZP(); //92
    void STA_ZPX(); //95
    void TYA(); //98
    void TXS(); //9A
    void STZ_A(); //9C
    void STA_AX(); //9D
    void LDY_I(); //A0
    void LDX_I(); //A2
    void LDA_ZP(); //A5
    void SMB2_ZP(); //A7
    void LDA_I(); //A9
    void LDA_A(); //AD
    void LDA_AX(); //BD
    void DEC_ZP(); //C6
    void CMP_I(); //C9
    void DEX(); //CA
    void WAI(); //CB
    void BNE(); //D0
    void CPX_I(); //E0
    void INC_ZP(); //E6
    void SMB6_ZP(); //E7
    void INX(); //E8
    void NOP(); //EA
    void BEQ(); //F0
    void SMB7_ZP(); //F7

};
#endif // CPU_H
