#ifndef CPU_H
#define CPU_H
class Display;
class MMU;
class DMA;
class Controller;

class BTInterrupt;
class PTInterrupt;

class Flash;

class Timer;



#define STACK_START 0x100
#define IRR 0x30
#define PRR 0x32
#define DRR 0x34
#define BRR 0x36
#define BYTE unsigned char

#define OTP_SIZE 0x4000
#define FLASH_SIZE 0x200000

#define CPU_NO_ERROR 0
#define CPU_OTP_NOT_LOADED 1
#define CPU_FLASH_NOT_LOADED 2


class CPU {
public:
    Display* display;
    BYTE error;

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
    BYTE S; //Stack
    unsigned short PC; //Program Counter

    //Memory
    BYTE memRegisters[0x80];
    BYTE RAM[0x1000000];
    BYTE videoRegisters[0x2];
    BYTE OTP[OTP_SIZE];

    Flash* flash;

    Controller* controller;

    bool interrupted;

    MMU* mmu;
    DMA* dma;

    BTInterrupt* btInterrupt;
    PTInterrupt* ptInterrupt;

    Timer* wait_timer;

    CPU(char* OTPFile, char* flashFile);
    void PrintState();
    unsigned short GetIRR();
    unsigned short GetPRR();
    unsigned short GetEffectivePRR();
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


    void StartWaitTimer();
    void Wait(unsigned int loop_size);
    bool Step();

    void ORA_ZP(); //05
    void ASL_ZP(); //06
    void RMB0_ZP(); //07
    void PHP(); //08
    void ORA_I(); //09
    void ASL_ACC(); //0A
    void TSB_A(); //0C
    void ORA_A(); //0D
    void ASL_A(); //0E
    void BBR0(); //0F
    void BPL(); //10
    void RMB1_ZP(); //17
    void CLC(); //18
    void ORA_AY(); //19
    void INC_ACC(); //1A
    void ORA_AX(); //1D
    void BBR1(); //1F
    void JSR(); //20
    void BIT_ZP(); //24
    void AND_ZP(); //25
    void ROL_ZP(); //26
    void RMB2_ZP(); //27
    void PLP(); //28
    void AND_I(); //29
    void ROL_ACC(); //2A
    void AND_A(); //2D
    void ROL_A(); //2E
    void BBR2(); //2F
    void BMI(); //30
    void RMB3_ZP(); //37
    void SEC(); //38
    void AND_AY(); //39
    void DEC_ACC(); //3A
    void AND_AX(); //3D
    void ROL_AX(); //3E
    void BBR3(); //3F
    void RTI(); //40
    void EOR_ZP(); //45
    void LSR_ZP(); //46
    void RMB4_ZP(); //47
    void PHA(); //48
    void EOR_I(); //49
    void LSR_ACC(); //4A
    void JMP_A(); //4C
    void EOR_A(); //4D
    void LSR_A(); //4E  //Used for 1.09.03
    void BBR4(); //4F
    void RMB5_ZP(); //57
    void CLI(); //58
    void PHY(); //5A
    void BBR5(); //5F
    void RTS(); //60
    void STZ_ZP(); //64
    void ADC_ZP(); //65
    void ROR_ZP(); //66
    void RMB6_ZP(); //67
    void PLA(); //68
    void ADC_I(); //69
    void ROR_ACC(); //6A
    void JMP_I(); //6C
    void ADC_A(); //6D
    void ROR_A(); //6E
    void BBR6(); //6F
    void ADC_INDIRECT_INDEXED(); //71
    void ADC_IZP(); //72
    void RMB7_ZP(); //77
    void SEI(); //78
    void PLY(); //7A
    void JMP_ABSOLUTE_INDEXED_INDIRECT(); //7C
    void ADC_AX(); //7D
    void BBR7(); //7F
    void BRA(); //80
    void STY_ZP(); //84
    void STA_ZP(); //85
    void STX_ZP(); //86
    void SMB0_ZP(); //87
    void DEY(); //88
    void TXA(); //8A
    void STY_A(); //8C
    void STA_A(); //8D
    void STX_A(); //8E
    void BBS0(); //8F
    void BCC(); //90
    void STA_INDIRECT_INDEXED(); //91
    void STA_IZP(); //92
    void STA_ZPX(); //95
    void SMB1_ZP(); //97
    void TYA(); //98
    void STA_AY(); //99
    void TXS(); //9A
    void STZ_A(); //9C
    void STA_AX(); //9D
    void STZ_AX(); //9E
    void BBS1(); //9F
    void LDY_I(); //A0
    void LDA_INDEXED_INDIRECT(); //A1
    void LDX_I(); //A2
    void LDY_ZP(); //A4
    void LDA_ZP(); //A5
    void LDX_ZP(); //A6
    void SMB2_ZP(); //A7
    void TAY(); //A8
    void LDA_I(); //A9
    void TAX(); //AA
    void LDY_A(); //AC
    void LDA_A(); //AD
    void LDX_A(); //AE
    void BBS2(); //AF
    void BCS(); //B0
    void LDA_INDIRECT_INDEXED(); //B1
    void LDA_IZP(); //B2
    void LDY_ZPX(); //B4
    void LDA_ZPX(); //B5
    void SMB3_ZP(); //B7
    void LDA_AY(); //B9
    void TSX(); //BA
    void LDY_AX(); //BC
    void LDA_AX(); //BD
    void LDX_AY(); //BE  used for 1.09.03
    void BBS3(); //BF
    void CPY_I(); //C0
    void CPY_ZP(); //C4
    void CMP_ZP(); //C5
    void DEC_ZP(); //C6
    void SMB4_ZP(); //C7
    void INY(); //C8
    void CMP_I(); //C9
    void DEX(); //CA
    void WAI(); //CB
    void CPY_A(); //CC
    void CMP_A(); //CD
    void DEC_A(); //CE
    void BBS4(); //CF
    void BNE(); //D0
    void DEC_ZPX(); //D6
    void SMB5_ZP(); //D7
    void CLD(); //D8
    void CMP_AY(); //D9
    void PHX(); //DA
    void CMP_AX(); //DD
    void DEC_AX(); //DE
    void BBS5(); //DF
    void CPX_I(); //E0
    void CPX_ZP(); //E4
    void SBC_ZP(); //E5
    void INC_ZP(); //E6
    void SMB6_ZP(); //E7
    void INX(); //E8
    void SBC_I(); //E9
    void NOP(); //EA
    void CPX_A(); //0xEC
    void SBC_A(); //ED
    void INC_A(); //EE
    void BBS6(); //EF
    void BEQ(); //F0
    void SBC_IZP(); //F2
    void INC_ZPX(); //F6
    void SMB7_ZP(); //F7
    void SED(); //F8
    void PLX(); //FA
    void SBC_AX(); //FD
    void INC_AX(); //FE
    void BBS7(); //FF

};
#endif // CPU_H
