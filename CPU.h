#ifndef CPU_H
#define CPU_H
class Display;
class MMU;
class DMA;
class Controller;

class BTInterrupt;


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

    Controller* controller;

    bool interrupted;

    MMU* mmu;
    DMA* dma;

    BTInterrupt* btInterrupt;

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

    void ASL_ZP(); //06
    void RMB0_ZP(); //07
    void PHP(); //08
    void ORA_I(); //09
    void ASL_ACC(); //0A
    void BPL(); //10
    void CLC(); //18
    void ORA_A(); //0D
    void JSR(); //20
    void BIT_ZP(); //24
    void ROL_ZP(); //26
    void PLP(); //28
    void AND_I(); //29
    void ROL_ACC(); //2A
    void AND_A(); //2D
    void RMB3_ZP(); //37
    void SEC(); //38
    void BBR3(); //3F
    void RTI(); //40
    void LSR_ZP(); //46
    void PHA(); //48
    void EOR_I(); //49
    void LSR_ACC(); //4A
    void JMP_A(); //4C
    void EOR_A(); //4D
    void BBR4(); //4F
    void CLI(); //58
    void PHY(); //5A
    void RTS(); //60
    void STZ_ZP(); //64
    void ADC_ZP(); //65
    void RMB6_ZP(); //67
    void PLA(); //68
    void ADC_I(); //69
    void ROR_ACC(); //6A
    void JMP_I(); //6C
    void ADC_A(); //6D
    void SEI(); //78
    void PLY(); //7A
    void BBR7(); //7F
    void BRA(); //80
    void STA_ZP(); //85
    void SMB0_ZP(); //87
    void DEY(); //88
    void STA_A(); //8D
    void STX_A(); //8E
    void BBS0(); //8F
    void BCC(); //90
    void STA_IZP(); //92
    void STA_ZPX(); //95
    void TYA(); //98
    void STA_AY(); //99
    void TXS(); //9A
    void STZ_A(); //9C
    void STA_AX(); //9D
    void LDY_I(); //A0
    void LDA_INDEXED_INDIRECT(); //A1
    void LDX_I(); //A2
    void LDA_ZP(); //A5
    void LDX_ZP(); //A6
    void SMB2_ZP(); //A7
    void TAY(); //A8
    void LDA_I(); //A9
    void TAX(); //AA
    void LDA_A(); //AD
    void BCS(); //B0
    void LDA_INDIRECT_INDEXED(); //B1
    void LDA_IZP(); //B2
    void LDA_ZPX(); //B5
    void LDA_AY(); //B9
    void LDY_AX(); //BC
    void LDA_AX(); //BD
    void CPY_I(); //C0
    void CPY_ZP(); //C4
    void DEC_ZP(); //C6
    void INY(); //C8
    void CMP_I(); //C9
    void DEX(); //CA
    void WAI(); //CB
    void CPY_A(); //CC
    void CMP_A(); //CD
    void DEC_A(); //CE
    void BNE(); //D0
    void DEC_ZPX(); //D6
    void PHX(); //DA
    void CPX_I(); //E0
    void CPX_ZP(); //E4
    void INC_ZP(); //E6
    void SMB6_ZP(); //E7
    void INX(); //E8
    void SBC_I(); //E9
    void NOP(); //EA
    void CPX_A(); //0xEC
    void SBC_A(); //ED
    void INC_A(); //EE
    void BEQ(); //F0
    void INC_ZPX(); //F6
    void SMB7_ZP(); //F7
    void PLX(); //FA

};
#endif // CPU_H
