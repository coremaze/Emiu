#include <windows.h>
#include "CPU.h"
#include "Display.h"
#include "MMU.h"
#include "DMA.h"
typedef void(CPU::*opcode_func)();
opcode_func OPCODES[] = {
        (opcode_func)nullptr, //00
        (opcode_func)nullptr, //01
        (opcode_func)nullptr, //02
        (opcode_func)nullptr, //03
        (opcode_func)nullptr, //04
        (opcode_func)nullptr, //05
        (opcode_func)nullptr, //06
        (opcode_func)nullptr, //07
        CPU::PHP, //08
        CPU::ORA_I, //09
        (opcode_func)nullptr, //0A
        (opcode_func)nullptr, //0B
        (opcode_func)nullptr, //0C
        CPU::ORA_A, //0D
        (opcode_func)nullptr, //0E
        (opcode_func)nullptr, //0F
        (opcode_func)nullptr, //10
        (opcode_func)nullptr, //11
        (opcode_func)nullptr, //12
        (opcode_func)nullptr, //13
        (opcode_func)nullptr, //14
        (opcode_func)nullptr, //15
        (opcode_func)nullptr, //16
        (opcode_func)nullptr, //17
        (opcode_func)nullptr, //18
        (opcode_func)nullptr, //19
        (opcode_func)nullptr, //1A
        (opcode_func)nullptr, //1B
        (opcode_func)nullptr, //1C
        (opcode_func)nullptr, //1D
        (opcode_func)nullptr, //1E
        (opcode_func)nullptr, //1F
        CPU::JSR, //20
        (opcode_func)nullptr, //21
        (opcode_func)nullptr, //22
        (opcode_func)nullptr, //23
        (opcode_func)nullptr, //24
        (opcode_func)nullptr, //25
        (opcode_func)nullptr, //26
        (opcode_func)nullptr, //27
        CPU::PLP, //28
        CPU::AND_I, //29
        (opcode_func)nullptr, //2A
        (opcode_func)nullptr, //2B
        (opcode_func)nullptr, //2C
        (opcode_func)nullptr, //2D
        (opcode_func)nullptr, //2E
        (opcode_func)nullptr, //2F
        (opcode_func)nullptr, //30
        (opcode_func)nullptr, //31
        (opcode_func)nullptr, //32
        (opcode_func)nullptr, //33
        (opcode_func)nullptr, //34
        (opcode_func)nullptr, //35
        (opcode_func)nullptr, //36
        (opcode_func)nullptr, //37
        (opcode_func)nullptr, //38
        (opcode_func)nullptr, //39
        (opcode_func)nullptr, //3A
        (opcode_func)nullptr, //3B
        (opcode_func)nullptr, //3C
        (opcode_func)nullptr, //3D
        (opcode_func)nullptr, //3E
        (opcode_func)nullptr, //3F
        (opcode_func)nullptr, //40
        (opcode_func)nullptr, //41
        (opcode_func)nullptr, //42
        (opcode_func)nullptr, //43
        (opcode_func)nullptr, //44
        (opcode_func)nullptr, //45
        (opcode_func)nullptr, //46
        (opcode_func)nullptr, //47
        CPU::PHA, //48
        (opcode_func)nullptr, //49
        (opcode_func)nullptr, //4A
        (opcode_func)nullptr, //4B
        CPU::JMP_A, //4C
        (opcode_func)nullptr, //4D
        (opcode_func)nullptr, //4E
        (opcode_func)nullptr, //4F
        (opcode_func)nullptr, //50
        (opcode_func)nullptr, //51
        (opcode_func)nullptr, //52
        (opcode_func)nullptr, //53
        (opcode_func)nullptr, //54
        (opcode_func)nullptr, //55
        (opcode_func)nullptr, //56
        (opcode_func)nullptr, //57
        CPU::CLI, //58
        (opcode_func)nullptr, //59
        (opcode_func)nullptr, //5A
        (opcode_func)nullptr, //5B
        (opcode_func)nullptr, //5C
        (opcode_func)nullptr, //5D
        (opcode_func)nullptr, //5E
        (opcode_func)nullptr, //5F
        CPU::RTS, //60
        (opcode_func)nullptr, //61
        (opcode_func)nullptr, //62
        (opcode_func)nullptr, //63
        CPU::STZ_ZP, //64
        (opcode_func)nullptr, //65
        (opcode_func)nullptr, //66
        CPU::RMB6_ZP, //67
        CPU::PLA, //68
        (opcode_func)nullptr, //69
        (opcode_func)nullptr, //6A
        (opcode_func)nullptr, //6B
        (opcode_func)nullptr, //6C
        (opcode_func)nullptr, //6D
        (opcode_func)nullptr, //6E
        (opcode_func)nullptr, //6F
        (opcode_func)nullptr, //70
        (opcode_func)nullptr, //71
        (opcode_func)nullptr, //72
        (opcode_func)nullptr, //73
        (opcode_func)nullptr, //74
        (opcode_func)nullptr, //75
        (opcode_func)nullptr, //76
        (opcode_func)nullptr, //77
        CPU::SEI, //78
        (opcode_func)nullptr, //79
        (opcode_func)nullptr, //7A
        (opcode_func)nullptr, //7B
        (opcode_func)nullptr, //7C
        (opcode_func)nullptr, //7D
        (opcode_func)nullptr, //7E
        (opcode_func)nullptr, //7F
        CPU::BRA, //80
        (opcode_func)nullptr, //81
        (opcode_func)nullptr, //82
        (opcode_func)nullptr, //83
        (opcode_func)nullptr, //84
        CPU::STA_ZP, //85
        (opcode_func)nullptr, //86
        CPU::SMB0_ZP, //87
        CPU::DEY, //88
        (opcode_func)nullptr, //89
        (opcode_func)nullptr, //8A
        (opcode_func)nullptr, //8B
        (opcode_func)nullptr, //8C
        CPU::STA_A, //8D
        CPU::STX_A, //8E
        (opcode_func)nullptr, //8F
        CPU::BCC, //90
        (opcode_func)nullptr, //91
        CPU::STA_IZP, //92
        (opcode_func)nullptr, //93
        (opcode_func)nullptr, //94
        CPU::STA_ZPX, //95
        (opcode_func)nullptr, //96
        (opcode_func)nullptr, //97
        CPU::TYA, //98
        (opcode_func)nullptr, //99
        CPU::TXS, //9A
        (opcode_func)nullptr, //9B
        CPU::STZ_A, //9C
        CPU::STA_AX, //9D
        (opcode_func)nullptr, //9E
        (opcode_func)nullptr, //9F
        CPU::LDY_I, //A0
        (opcode_func)nullptr, //A1
        CPU::LDX_I, //A2
        (opcode_func)nullptr, //A3
        (opcode_func)nullptr, //A4
        CPU::LDA_ZP, //A5
        (opcode_func)nullptr, //A6
        CPU::SMB2_ZP, //A7
        (opcode_func)nullptr, //A8
        CPU::LDA_I, //A9
        (opcode_func)nullptr, //AA
        (opcode_func)nullptr, //AB
        (opcode_func)nullptr, //AC
        CPU::LDA_A, //AD
        (opcode_func)nullptr, //AE
        (opcode_func)nullptr, //AF
        (opcode_func)nullptr, //B0
        (opcode_func)nullptr, //B1
        (opcode_func)nullptr, //B2
        (opcode_func)nullptr, //B3
        (opcode_func)nullptr, //B4
        (opcode_func)nullptr, //B5
        (opcode_func)nullptr, //B6
        (opcode_func)nullptr, //B7
        (opcode_func)nullptr, //B8
        (opcode_func)nullptr, //B9
        (opcode_func)nullptr, //BA
        (opcode_func)nullptr, //BB
        (opcode_func)nullptr, //BC
        CPU::LDA_AX, //BD
        (opcode_func)nullptr, //BE
        (opcode_func)nullptr, //BF
        (opcode_func)nullptr, //C0
        (opcode_func)nullptr, //C1
        (opcode_func)nullptr, //C2
        (opcode_func)nullptr, //C3
        (opcode_func)nullptr, //C4
        (opcode_func)nullptr, //C5
        CPU::DEC_ZP, //C6
        (opcode_func)nullptr, //C7
        (opcode_func)nullptr, //C8
        CPU::CMP_I, //C9
        CPU::DEX, //CA
        CPU::WAI, //CB
        (opcode_func)nullptr, //CC
        (opcode_func)nullptr, //CD
        (opcode_func)nullptr, //CE
        (opcode_func)nullptr, //CF
        CPU::BNE, //D0
        (opcode_func)nullptr, //D1
        (opcode_func)nullptr, //D2
        (opcode_func)nullptr, //D3
        (opcode_func)nullptr, //D4
        (opcode_func)nullptr, //D5
        (opcode_func)nullptr, //D6
        (opcode_func)nullptr, //D7
        (opcode_func)nullptr, //D8
        (opcode_func)nullptr, //D9
        (opcode_func)nullptr, //DA
        (opcode_func)nullptr, //DB
        (opcode_func)nullptr, //DC
        (opcode_func)nullptr, //DD
        (opcode_func)nullptr, //DE
        (opcode_func)nullptr, //DF
        CPU::CPX_I, //E0
        (opcode_func)nullptr, //E1
        (opcode_func)nullptr, //E2
        (opcode_func)nullptr, //E3
        (opcode_func)nullptr, //E4
        (opcode_func)nullptr, //E5
        CPU::INC_ZP, //E6
        CPU::SMB6_ZP, //E7
        CPU::INX, //E8
        (opcode_func)nullptr, //E9
        CPU::NOP, //EA
        (opcode_func)nullptr, //EB
        (opcode_func)nullptr, //EC
        (opcode_func)nullptr, //ED
        (opcode_func)nullptr, //EE
        (opcode_func)nullptr, //EF
        CPU::BEQ, //F0
        (opcode_func)nullptr, //F1
        (opcode_func)nullptr, //F2
        (opcode_func)nullptr, //F3
        (opcode_func)nullptr, //F4
        (opcode_func)nullptr, //F5
        (opcode_func)nullptr, //F6
        CPU::SMB7_ZP, //F7
        (opcode_func)nullptr, //F8
        (opcode_func)nullptr, //F9
        (opcode_func)nullptr, //FA
        (opcode_func)nullptr, //FB
        (opcode_func)nullptr, //FC
        (opcode_func)nullptr, //FD
        (opcode_func)nullptr, //FE
        (opcode_func)nullptr, //FF
};


CPU::CPU(char* OTPFile, char* flashFile){
    FILE* file;

    //Load OTP
    file = fopen(OTPFile, "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        long fsize = ftell(file);
        fseek(file, 0, SEEK_SET);
        if (fsize != OTP_SIZE) {
            printf("Error: OTP file is incorrect size.\n");
            return;
        }
        fread(this->OTP, fsize, 1, file);
        fclose(file);
    }
    else {
        printf("Error: OTP file could not be read.\n");
        return;
    }

    //Load Flash
    file = fopen(flashFile, "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        long fsize = ftell(file);
        fseek(file, 0, SEEK_SET);
        if (fsize != FLASH_SIZE) {
            printf("Error: Flash file is incorrect size.\n");
            return;
        }
        fread(this->Flash, fsize, 1, file);
        fclose(file);
    }
    else {
        printf("Error: Flash file could not be read.\n");
        return;
    }

    //Initialize flags
    this->c = false; //Carry
    this->z = false; //Zero
    this->i = false; //Interrupt Disable
    this->d = false; //Decimal Mode
    this->b = false; //Break
    this->v = false; //Overflow
    this->n = false; //Negative

    //Initialize registers
    this->A = 0;
    this->X = 0;
    this->Y = 0;
    this->S = 0;
    this->PC = 0;

    ////Make controller

    this->interrupted = true;

    this->display = new Display();
    this->mmu = new MMU(this);
    this->dma = new DMA(this);

    this->pre_interrupt_PRR = this->GetPRR();

    ////BT interrupt
    ////PT interrupt

    this->Reset();

    this->interrupted = false;


}

void CPU::PrintState(){
    printf("A:%02X X:%02X Y:%02X PC:%04X C:%s Z:%s PRR:%02X\n", (unsigned int)this->A, (unsigned int)this->X, (unsigned int)this->Y, (unsigned int)this->PC, this->c ? "True" : "False",
                                                               this->z ? "True" : "False", (unsigned int)this->GetPRR());
}

unsigned short CPU::GetIRR(){
    unsigned short irr = 0;
    memcpy(&irr, this->memRegisters + IRR, 2);
    return irr;
}

unsigned short CPU::GetPRR(){
    unsigned short prr = 0;
    memcpy(&prr, this->memRegisters + PRR, 2);
    return prr;
}

unsigned short CPU::GetDRR(){
    unsigned short drr = 0;
    memcpy(&drr, this->memRegisters + DRR, 2);
    return drr;
}

unsigned short CPU::GetBRR(){
    unsigned short brr = 0;
    memcpy(&brr, this->memRegisters + BRR, 2);
    return brr;
}

void CPU::BeginInterrupt(){
    this->pre_interrupt_PRR = this->GetPRR();
    unsigned short this_IRR = this->GetIRR();
    this->mmu->StoreShort(PRR, this_IRR);
    this->interrupted = true;
}

void CPU::EndInterrupt(){
    this->mmu->StoreShort(PRR, this->pre_interrupt_PRR);
    this->interrupted = false;
}

unsigned short CPU::GetResetVector(){
    if (!this->interrupted){
        printf("WARNING: Can't get vector unless interrupted.\n");
    }
    return this->mmu->ReadShort(0x7FFC);
}

void CPU::Reset(){
    this->i = true;
    this->d = false;
    this->PC = this->GetResetVector();
    //this->display = Display();
}

unsigned short CPU::GetBTVector(){
    if (!this->interrupted){
        printf("WARNING: Can't get vector unless interrupted.\n");
    }
    return this->mmu->ReadShort(0x7FEC);
}

unsigned short CPU::GetPTVector(){
    if (!this->interrupted){
        printf("WARNING: Can't get vector unless interrupted.\n");
    }
    return this->mmu->ReadShort(0x7FEE);
}

void CPU::WriteVideoRegister(unsigned short address, BYTE by){
    address %= 2;
    this->videoRegisters[address] = by;
    if (address == 0){
        this->display->SendCommand(by);
    }
    else {
        this->display->SendData(by);
    }
}

BYTE CPU::ImmediateVal(){
    BYTE value = this->mmu->ReadByte(this->PC + 1);
    return value;
}

BYTE CPU::ZeroPagePtr(){
    BYTE pointer = this->mmu->ReadByte(this->PC + 1);
    return pointer;
}
BYTE CPU::ZeroPageVal(){
    BYTE pointer = this->ZeroPagePtr();
    BYTE value = this->mmu->ReadByte(pointer);
    return value;
}

BYTE CPU::ZeroPageXPtr(){
    BYTE pointer = this->mmu->ReadByte(this->PC + 1);
    pointer += this->X;
    return pointer;
}
BYTE CPU::ZeroPageXVal(){
    BYTE pointer = this->ZeroPageXPtr();
    BYTE value = this->mmu->ReadByte(pointer);
    return value;
}

BYTE CPU::ZeroPageYPtr(){
    BYTE pointer = this->mmu->ReadByte(this->PC + 1);
    pointer += this->Y;
    return pointer;
}
BYTE CPU::ZeroPageYVal(){
    BYTE pointer = this->ZeroPageYPtr();
    BYTE value = this->mmu->ReadByte(pointer);
    return value;
}

unsigned short CPU::RelativePtr(){
    signed char pointer_offset = (signed char)this->mmu->ReadByte(this->PC + 1);
    unsigned short pointer = this->PC + pointer_offset + 2; //instruction size of 2
    return pointer;
}
BYTE CPU::RelativeVal(){
    unsigned short pointer = RelativePtr();
    BYTE value = this->mmu->ReadByte(pointer);
    return value;
}

unsigned short CPU::AbsolutePtr(){
    unsigned short pointer = this->mmu->ReadShort(this->PC + 1);
    return pointer;
}
BYTE CPU::AbsoluteVal(){
    unsigned short pointer = this->AbsolutePtr();
    BYTE value = this->mmu->ReadByte(pointer);
    return value;
}

unsigned short CPU::AbsoluteXPtr(){
    unsigned short pointer = this->mmu->ReadShort(this->PC + 1);
    pointer += this->X;
    return pointer;
}
BYTE CPU::AbsoluteXVal(){
    unsigned short pointer = this->AbsoluteXPtr();
    BYTE value = this->mmu->ReadByte(pointer);
    return value;
}

unsigned short CPU::AbsoluteYPtr(){
    unsigned short pointer = this->mmu->ReadShort(this->PC + 1);
    pointer += this->Y;
    return pointer;
}
BYTE CPU::AbsoluteYVal(){
    unsigned short pointer = this->AbsoluteYPtr();
    BYTE value = this->mmu->ReadByte(pointer);
    return value;
}

unsigned short CPU::IndirectPtr(){
    unsigned short pointer = this->mmu->ReadShort(this->PC + 1);
    return pointer;
}
unsigned short CPU::IndirectVal(){
    unsigned short pointer = this->IndirectPtr();
    unsigned short value = this->mmu->ReadShort(pointer);
    return value;
}

unsigned short CPU::IndirectZeroPagePtr(){
    unsigned short pointer = this->mmu->ReadByte(this->PC + 1);
    pointer = this->mmu->ReadShort(pointer);
    return pointer;
}
BYTE CPU::IndirectZeroPageVal(){
    unsigned short pointer = this->IndirectZeroPagePtr();
    BYTE value = this->mmu->ReadByte(pointer);
    return value;
}

unsigned short CPU::IndexedIndirectPtr(){
    unsigned short pointer = this->mmu->ReadByte(this->PC + 1);
    pointer += this->X;
    pointer = this->mmu->ReadShort(pointer);
    return pointer;
}
BYTE CPU::IndexedIndirectVal(){
    unsigned short pointer = this->IndexedIndirectPtr();
    BYTE value = this->mmu->ReadByte(pointer);
    return value;
}

unsigned short CPU::IndirectIndexedPtr(){
    unsigned short pointer = this->mmu->ReadByte(this->PC + 1);
    pointer = this->mmu->ReadShort(pointer) + this->Y;
    return pointer;
}
BYTE CPU::IndirectIndexedVal(){
    unsigned short pointer = this->IndirectIndexedPtr();
    BYTE value = this->mmu->ReadByte(pointer);
    return value;
}

unsigned short CPU::AbsoluteIndexIndirectPtr(){
    unsigned short pointer = this->mmu->ReadShort(this->PC + 1);
    pointer += this->X;
    return pointer;
}
unsigned short CPU::AbsoluteIndexIndirectVal(){
    unsigned short pointer = this->AbsoluteIndexIndirectPtr();
    BYTE value = this->mmu->ReadShort(pointer);
    return value;
}

void CPU::Push(BYTE by){
    this->mmu->StoreByte(STACK_START + this->S, by);
    this->S++;
}
void CPU::PushShort(unsigned short sh){
    BYTE low = (BYTE)(sh & 0x00FF);
    BYTE high = (BYTE)((sh & 0xFF00) >> 8);
    this->Push(low);
    this->Push(high);
}

BYTE CPU::Pop(){
    this->S--;
    return this->mmu->ReadByte(STACK_START + this->S);
}
unsigned short CPU::PopShort(){
    BYTE high = this->Pop();
    BYTE low = this->Pop();
    unsigned short result = (unsigned short)low;
    result = result | ( ((unsigned short)high) << 8);
    return result;
}

BYTE CPU::ExportFlags(){
    BYTE flags = 0;
    flags += this->n ? 1 : 0; //Negative
    flags <<= 1;
    flags += this->v ? 1 : 0; //Overflow
    flags <<= 1;
    flags += 1; //Always set
    flags <<= 1;
    flags += 1; //Clear if interrupt vectoring, set if BRK or PHP
    flags <<= 1;
    flags += this->d ? 1 : 0; //Decimal mode
    flags <<= 1;
    flags += this->i ? 1 : 0; //Interrupt disable
    flags <<= 1;
    flags += this->z ? 1 : 0; //Zero
    flags <<= 1;
    flags += this->c ? 1 : 0; //Carry
    return flags;
}

void CPU::ImportFlags(unsigned char flags){
    this->n = (flags & 0b10000000) ? true : false;
    this->v = (flags & 0b01000000) ? true : false;
    this->d = (flags & 0b00001000) ? true : false;
    this->i = (flags & 0b00000100) ? true : false;
    this->z = (flags & 0b00000010) ? true : false;
    this->c = (flags & 0b00000001) ? true : false;
}

bool CPU::Step(){
    BYTE opcode = this->mmu->ReadByte(this->PC);
    //printf("Opcode: %02X\n", (unsigned int)opcode);
    opcode_func func = OPCODES[opcode];
    if (!func){
        printf("Unknown opcode: %02X\n", (unsigned int)opcode);
        return true;
    }
    (this->*func)();
    return false;
}







void CPU::PHP(){
    BYTE flags = this->ExportFlags();
    this->Push(flags);
    this->PC += 1;
}
void CPU::ORA_I(){
    BYTE val = this->ImmediateVal();
    this->A |= val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::ORA_A(){
    BYTE val = this->AbsoluteVal();
    this->A |= val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 3;
}
void CPU::JSR(){
    unsigned short arg = this->AbsolutePtr();
    unsigned short return_addr = this->PC + 2;
    this->PushShort(return_addr);
    this->PC = arg;
}
void CPU::PLP(){
    BYTE flags = this->Pop();
    this->ImportFlags(flags);
    this->PC += 1;
}
void CPU::AND_I(){
    BYTE arg = this->ImmediateVal();
    this->A &= arg;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::PHA(){
    this->Push(this->A);
    this->PC += 1;
}
void CPU::JMP_A(){
    unsigned short arg = this->AbsolutePtr();
    this->PC = arg;
}
void CPU::CLI(){
    this->i = false;
    this->PC += 1;
}
void CPU::RTS(){
    unsigned short return_addr = this->PopShort();
    this->PC = return_addr + 1;
}
void CPU::STZ_ZP(){
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, 0);
    this->PC += 2;
}
void CPU::RMB6_ZP(){
    BYTE val = this->ZeroPageVal();
    val &= 0b10111111;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::PLA(){
    this->A = this->Pop();
    this->z = this->A == 0;
    this->z = (this->A & 0b10000000) ? true : false;
    this->PC += 1;
}
void CPU::SEI(){
    this->i = true;
    this->PC++;
}
void CPU::BRA(){
    unsigned short ptr = this->RelativePtr();
    this->PC = ptr;
}
void CPU::STA_ZP(){
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, this->A);
    this->PC += 2;
}
void CPU::SMB0_ZP(){
    BYTE val = this->ZeroPageVal();
    val |= 0b00000001;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::DEY(){
    this->Y -= 1;
    this->z = this->Y == 0;
    this->n = (this->Y & 0b10000000) ? true : false;
    this->PC += 1;
}
void CPU::STA_A(){
    unsigned short ptr = this->AbsolutePtr();
    this->mmu->StoreByte(ptr, this->A);
    this->PC += 3;
}
void CPU::STX_A(){
    unsigned short ptr = this->AbsolutePtr();
    this->mmu->StoreByte(ptr, this->X);
    this->PC += 3;
}
void CPU::BCC(){
    unsigned short arg = this->RelativePtr();
    if (!this->c) {
        this->PC = arg;
    }
    else {
        this->PC += 2;
    }
}
void CPU::STA_IZP(){
    unsigned short ptr = this->IndirectZeroPagePtr();
    this->mmu->StoreByte(ptr, this->A);
    this->PC += 2;
}
void CPU::STA_ZPX(){
    unsigned short ptr = this->ZeroPageXPtr();
    this->mmu->StoreByte(ptr, this->A);
    this->PC += 2;
}
void CPU::TYA(){
    this->A = this->Y;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 1;
}
void CPU::TXS(){
    this->S = this->X;
    this->PC++;
}
void CPU::STZ_A(){
    unsigned short ptr = this->AbsolutePtr();
    this->mmu->StoreByte(ptr, 0);
    this->PC += 3;
}
void CPU::STA_AX(){
    unsigned short ptr = this->AbsoluteXPtr();
    this->mmu->StoreByte(ptr, this->A);
    this->PC += 3;
}
void CPU::LDY_I(){
    BYTE val = this->ImmediateVal();
    this->Y = val;
    this->z = this->Y == 0;
    this->n = (this->Y & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::LDX_I(){
    BYTE val = this->ImmediateVal();
    this->X = val;
    this->z = this->X == 0;
    this->n = (this->X & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::LDA_ZP(){
    BYTE val = this->ZeroPageVal();
    this->A = val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::SMB2_ZP(){
    BYTE val = this->ZeroPageVal();
    val |= 0b00000100;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::LDA_I(){
    BYTE arg = this->ImmediateVal();
    this->A = arg;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::LDA_A(){
    BYTE val = this->AbsoluteVal();
    this->A = val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 3;
}
void CPU::LDA_AX(){
    BYTE val = this->AbsoluteXVal();
    this->A = val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 3;
}
void CPU::DEC_ZP(){
    BYTE val = this->ZeroPageVal();
    val -= 1;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::CMP_I(){
    BYTE val = this->ImmediateVal();
    this->c = this->A >= val;
    this->z = this->A == val;
    this->n = this->A < val;
    this->PC += 2;
}
void CPU::DEX(){
    this->X -= 1;
    this->z = this->X == 0;
    this->n = (this->X & 0b10000000) ? true : false;
    this->PC += 1;
}
void CPU::WAI(){ //NOT IMPLEMENTED
    this->PC++;
}
void CPU::BNE(){
    unsigned short arg = this->RelativePtr();
    if (!this->z){
        this->PC = arg;
    }
    else {
        this->PC += 2;
    }
}
void CPU::CPX_I(){
    BYTE arg = this->ImmediateVal();
    this->c = this->X >= arg;
    this->z = this->X == arg;
    this->n = this->X < arg;
    this->PC += 2;
}
void CPU::INC_ZP(){
    BYTE val = this->ZeroPageVal();
    val += 1;
    this->z = val == 0;
    this->n = (val & 0b10000000) ? true : false;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;

}
void CPU::SMB6_ZP(){
    BYTE val = this->ZeroPageVal();
    val |= 0b01000000;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::INX(){
    this->X += 1;
    this->z = this->X == 0;
    this->n = (this->X & 0b10000000) ? true : false;
    this->PC += 1;
}
void CPU::NOP(){
    this->PC += 1;
}
void CPU::BEQ(){
    unsigned short arg = this->RelativePtr();
        if (this->z){
            this->PC = arg;
        }
        else {
            this->PC += 2;
        }
}
void CPU::SMB7_ZP(){
    BYTE val = this->ZeroPageVal();
    val |= 0b10000000;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
