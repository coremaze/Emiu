#include <windows.h>
#include "CPU.h"
#include "Display.h"
#include "MMU.h"
#include "DMA.h"
#include "Controller.h"
#include "Interrupt.h"
#include "Flash.h"

typedef void(CPU::*opcode_func)();
opcode_func OPCODES[] = {
        (opcode_func)nullptr, //00
        (opcode_func)nullptr, //01
        (opcode_func)nullptr, //02
        (opcode_func)nullptr, //03
        (opcode_func)nullptr, //04
        CPU::ORA_ZP, //05
        CPU::ASL_ZP, //06
        CPU::RMB0_ZP, //07
        CPU::PHP, //08
        CPU::ORA_I, //09
        CPU::ASL_ACC, //0A
        (opcode_func)nullptr, //0B
        CPU::TSB_A, //0C
        CPU::ORA_A, //0D
        CPU::ASL_A, //0E
        CPU::BBR0, //0F
        CPU::BPL, //10
        (opcode_func)nullptr, //11
        (opcode_func)nullptr, //12
        (opcode_func)nullptr, //13
        (opcode_func)nullptr, //14
        (opcode_func)nullptr, //15
        (opcode_func)nullptr, //16
        CPU::RMB1_ZP, //17
        CPU::CLC, //18
        CPU::ORA_AY, //19
        CPU::INC_ACC, //1A
        (opcode_func)nullptr, //1B
        (opcode_func)nullptr, //1C
        CPU::ORA_AX, //1D
        (opcode_func)nullptr, //1E
        CPU::BBR1, //1F
        CPU::JSR, //20
        (opcode_func)nullptr, //21
        (opcode_func)nullptr, //22
        (opcode_func)nullptr, //23
        CPU::BIT_ZP, //24
        CPU::AND_ZP, //25
        CPU::ROL_ZP, //26
        CPU::RMB2_ZP, //27
        CPU::PLP, //28
        CPU::AND_I, //29
        CPU::ROL_ACC, //2A
        (opcode_func)nullptr, //2B
        (opcode_func)nullptr, //2C
        CPU::AND_A, //2D
        CPU::ROL_A, //2E
        CPU::BBR2, //2F
        CPU::BMI, //30
        (opcode_func)nullptr, //31
        (opcode_func)nullptr, //32
        (opcode_func)nullptr, //33
        (opcode_func)nullptr, //34
        (opcode_func)nullptr, //35
        (opcode_func)nullptr, //36
        CPU::RMB3_ZP, //37
        CPU::SEC, //38
        CPU::AND_AY, //39
        CPU::DEC_ACC, //3A
        (opcode_func)nullptr, //3B
        (opcode_func)nullptr, //3C
        CPU::AND_AX, //3D
        (opcode_func)nullptr, //3E
        CPU::BBR3, //3F
        CPU::RTI, //40
        (opcode_func)nullptr, //41
        (opcode_func)nullptr, //42
        (opcode_func)nullptr, //43
        (opcode_func)nullptr, //44
        CPU::EOR_ZP, //45
        CPU::LSR_ZP, //46
        CPU::RMB4_ZP, //47
        CPU::PHA, //48
        CPU::EOR_I, //49
        CPU::LSR_ACC, //4A
        (opcode_func)nullptr, //4B
        CPU::JMP_A, //4C
        CPU::EOR_A, //4D
        CPU::LSR_A, //4E
        CPU::BBR4, //4F
        (opcode_func)nullptr, //50
        (opcode_func)nullptr, //51
        (opcode_func)nullptr, //52
        (opcode_func)nullptr, //53
        (opcode_func)nullptr, //54
        (opcode_func)nullptr, //55
        (opcode_func)nullptr, //56
        CPU::RMB5_ZP, //57
        CPU::CLI, //58
        (opcode_func)nullptr, //59
        CPU::PHY, //5A
        (opcode_func)nullptr, //5B
        (opcode_func)nullptr, //5C
        (opcode_func)nullptr, //5D
        (opcode_func)nullptr, //5E
        CPU::BBR5, //5F
        CPU::RTS, //60
        (opcode_func)nullptr, //61
        (opcode_func)nullptr, //62
        (opcode_func)nullptr, //63
        CPU::STZ_ZP, //64
        CPU::ADC_ZP, //65
        (opcode_func)nullptr, //66
        CPU::RMB6_ZP, //67
        CPU::PLA, //68
        CPU::ADC_I, //69
        CPU::ROR_ACC, //6A
        (opcode_func)nullptr, //6B
        CPU::JMP_I, //6C
        CPU::ADC_A, //6D
        CPU::ROR_A, //6E
        CPU::BBR6, //6F
        (opcode_func)nullptr, //70
        CPU::ADC_INDIRECT_INDEXED, //71
        CPU::ADC_IZP, //72
        (opcode_func)nullptr, //73
        (opcode_func)nullptr, //74
        (opcode_func)nullptr, //75
        (opcode_func)nullptr, //76
        CPU::RMB7_ZP, //77
        CPU::SEI, //78
        (opcode_func)nullptr, //79
        CPU::PLY, //7A
        (opcode_func)nullptr, //7B
        CPU::JMP_ABSOLUTE_INDEXED_INDIRECT, //7C
        (opcode_func)nullptr, //7D
        (opcode_func)nullptr, //7E
        CPU::BBR7, //7F
        CPU::BRA, //80
        (opcode_func)nullptr, //81
        (opcode_func)nullptr, //82
        (opcode_func)nullptr, //83
        CPU::STY_ZP, //84
        CPU::STA_ZP, //85
        CPU::STX_ZP, //86
        CPU::SMB0_ZP, //87
        CPU::DEY, //88
        (opcode_func)nullptr, //89
        CPU::TXA, //8A
        (opcode_func)nullptr, //8B
        CPU::STY_A, //8C
        CPU::STA_A, //8D
        CPU::STX_A, //8E
        CPU::BBS0, //8F
        CPU::BCC, //90
        CPU::STA_INDIRECT_INDEXED, //91
        CPU::STA_IZP, //92
        (opcode_func)nullptr, //93
        (opcode_func)nullptr, //94
        CPU::STA_ZPX, //95
        (opcode_func)nullptr, //96
        CPU::SMB1_ZP, //97
        CPU::TYA, //98
        CPU::STA_AY, //99
        CPU::TXS, //9A
        (opcode_func)nullptr, //9B
        CPU::STZ_A, //9C
        CPU::STA_AX, //9D
        CPU::STZ_AX, //9E
        CPU::BBS1, //9F
        CPU::LDY_I, //A0
        CPU::LDA_INDEXED_INDIRECT, //A1
        CPU::LDX_I, //A2
        (opcode_func)nullptr, //A3
        CPU::LDY_ZP, //A4
        CPU::LDA_ZP, //A5
        CPU::LDX_ZP, //A6
        CPU::SMB2_ZP, //A7
        CPU::TAY, //A8
        CPU::LDA_I, //A9
        CPU::TAX, //AA
        (opcode_func)nullptr, //AB
        CPU::LDY_A, //AC
        CPU::LDA_A, //AD
        CPU::LDX_A, //AE
        CPU::BBS2, //AF
        CPU::BCS, //B0
        CPU::LDA_INDIRECT_INDEXED, //B1
        CPU::LDA_IZP, //B2
        (opcode_func)nullptr, //B3
        CPU::LDY_ZPX, //B4
        CPU::LDA_ZPX, //B5
        (opcode_func)nullptr, //B6
        CPU::SMB3_ZP, //B7
        (opcode_func)nullptr, //B8
        CPU::LDA_AY, //B9
        (opcode_func)nullptr, //BA
        (opcode_func)nullptr, //BB
        CPU::LDY_AX, //BC
        CPU::LDA_AX, //BD
        CPU::LDX_AY, //BE
        CPU::BBS3, //BF
        CPU::CPY_I, //C0
        (opcode_func)nullptr, //C1
        (opcode_func)nullptr, //C2
        (opcode_func)nullptr, //C3
        CPU::CPY_ZP, //C4
        CPU::CMP_ZP, //C5
        CPU::DEC_ZP, //C6
        CPU::SMB4_ZP, //C7
        CPU::INY, //C8
        CPU::CMP_I, //C9
        CPU::DEX, //CA
        CPU::WAI, //CB
        CPU::CPY_A, //CC
        CPU::CMP_A, //CD
        CPU::DEC_A, //CE
        CPU::BBS4, //CF
        CPU::BNE, //D0
        (opcode_func)nullptr, //D1
        (opcode_func)nullptr, //D2
        (opcode_func)nullptr, //D3
        (opcode_func)nullptr, //D4
        (opcode_func)nullptr, //D5
        CPU::DEC_ZPX, //D6
        CPU::SMB5_ZP, //D7
        CPU::CLD, //D8
        (opcode_func)nullptr, //D9
        CPU::PHX, //DA
        (opcode_func)nullptr, //DB
        (opcode_func)nullptr, //DC
        CPU::CMP_AX, //DD
        CPU::DEC_AX, //DE
        CPU::BBS5, //DF
        CPU::CPX_I, //E0
        (opcode_func)nullptr, //E1
        (opcode_func)nullptr, //E2
        (opcode_func)nullptr, //E3
        CPU::CPX_ZP, //E4
        CPU::SBC_ZP, //E5
        CPU::INC_ZP, //E6
        CPU::SMB6_ZP, //E7
        CPU::INX, //E8
        CPU::SBC_I, //E9
        CPU::NOP, //EA
        (opcode_func)nullptr, //EB
        CPU::CPX_A, //EC
        CPU::SBC_A, //ED
        CPU::INC_A, //EE
        CPU::BBS6, //EF
        CPU::BEQ, //F0
        (opcode_func)nullptr, //F1
        CPU::SBC_IZP, //F2
        (opcode_func)nullptr, //F3
        (opcode_func)nullptr, //F4
        (opcode_func)nullptr, //F5
        CPU::INC_ZPX, //F6
        CPU::SMB7_ZP, //F7
        CPU::SED, //F8
        (opcode_func)nullptr, //F9
        CPU::PLX, //FA
        (opcode_func)nullptr, //FB
        (opcode_func)nullptr, //FC
        (opcode_func)nullptr, //FD
        CPU::INC_AX, //FE
        CPU::BBS7, //FF
};

BYTE BCDToNumber(BYTE x){
    return (10 * (x & 0xF0) >> 4) + (x & 0x0F);
}
BYTE NumberToBCD(BYTE x){
    return ((((x / 10) % 10) << 4) & 0xF0) | ((x % 10) & 0x0F);
}



CPU::CPU(char* OTPFile, char* flashFile){
    this->error = CPU_NO_ERROR;

    FILE* file;

    //Load OTP
    file = fopen(OTPFile, "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        long fsize = ftell(file);
        fseek(file, 0, SEEK_SET);
        if (fsize != OTP_SIZE) {
            printf("Error: OTP file is incorrect size.\n");
            this->error = CPU_OTP_NOT_LOADED;
            return;
        }
        fread(this->OTP, fsize, 1, file);
        fclose(file);
    }
    else {
        printf("Error: OTP file could not be read.\n");
        this->error = CPU_OTP_NOT_LOADED;
        return;
    }

    this->flash = new Flash();

    //Load Flash
    file = fopen(flashFile, "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        long fsize = ftell(file);
        fseek(file, 0, SEEK_SET);
        if (fsize != FLASH_SIZE) {
            printf("Error: Flash file is incorrect size.\n");
            this->error = CPU_FLASH_NOT_LOADED;
            return;
        }
        fread(this->flash->memory, fsize, 1, file);
        fclose(file);
    }
    else {
        printf("Error: Flash file could not be read.\n");
        this->error = CPU_FLASH_NOT_LOADED;
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

    this->controller = new Controller(this);

    this->interrupted = true;

    this->display = new Display();
    this->mmu = new MMU(this);
    this->dma = new DMA(this);

    this->btInterrupt = new BTInterrupt(this);
    this->ptInterrupt = new PTInterrupt(this);

    this->Reset();

    this->interrupted = false;


}

void CPU::PrintState(){
    printf("A:%02X X:%02X Y:%02X PC:%04X C:%s Z:%s PRR:%X Interrupted:%s\n", (unsigned int)this->A, (unsigned int)this->X, (unsigned int)this->Y, (unsigned int)this->PC, this->c ? "True" : "False",
                                                               this->z ? "True" : "False", (unsigned int)this->GetPRR(), this->interrupted ? "True" : "False");
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

unsigned short CPU::GetEffectivePRR(){
    if (this->interrupted){
        return this->GetIRR();
    }
    else {
        return this->GetPRR();
    }
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
    this->interrupted = true;
}

void CPU::EndInterrupt(){
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
    unsigned short value = this->mmu->ReadShort(pointer);
    return value;
}

void CPU::Push(BYTE by){
    this->mmu->StoreByte(STACK_START + this->S, by);
    this->S--;
}
void CPU::PushShort(unsigned short sh){
    BYTE low = (BYTE)(sh & 0x00FF);
    BYTE high = (BYTE)((sh & 0xFF00) >> 8);
    this->Push(high);
    this->Push(low);
}

BYTE CPU::Pop(){
    this->S++;
    return this->mmu->ReadByte(STACK_START + this->S);
}
unsigned short CPU::PopShort(){
    BYTE low = this->Pop();
    BYTE high = this->Pop();
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

void CPU::StartWaitTimer(){
    this->last_wait_time = timeGetTime();
}

unsigned int CPU::Wait(unsigned int loop_size){
    unsigned int cpu_speed = 6000000; //6MHz
    unsigned int average_cycles_per_instruction = 4;

    unsigned int milliseconds_per_loop = (average_cycles_per_instruction*loop_size*1000)/cpu_speed;
    unsigned int end_time = timeGetTime();
    unsigned int duration = end_time-this->last_wait_time;
    if (duration < milliseconds_per_loop){
        Sleep(milliseconds_per_loop - duration);
        if ((milliseconds_per_loop - duration) > 3){
            loop_size -= 1; //fine tune loop size in order to maximize responsiveness
        }
    }
    else {
        loop_size += 1; //fine tune loop size in order to maximize responsiveness
    }
    return loop_size;
}

bool CPU::Step(){
    this->controller->Update();
    BYTE opcode = this->mmu->ReadByte(this->PC);
    //printf("Opcode: %02X\n", (unsigned int)opcode);
    opcode_func func = OPCODES[opcode];
    if (!func){
        printf("Unknown opcode: %02X\n", (unsigned int)opcode);
        this->PrintState();
        return true;
    }
    (this->*func)();

    bool bt_interrupt_requested = this->btInterrupt->Update();
    bool pt_interrupt_requested = this->ptInterrupt->Update();
    if (!this->i){
        if (this->interrupted) {}
        else if (pt_interrupt_requested){
            this->BeginInterrupt();
            this->PushShort(this->PC);
            this->Push(this->ExportFlags());
            this->PC = this->GetPTVector();
        }
        else if (bt_interrupt_requested){
            this->BeginInterrupt();
            this->PushShort(this->PC);
            this->Push(this->ExportFlags());
            this->PC = this->GetBTVector();
        }
    }
    return false;
}





void CPU::ORA_ZP(){
    BYTE val = this->ZeroPageVal();
    this->A |= val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::ASL_ZP(){
    BYTE val = this->ZeroPageVal();
    this->c = (0b10000000 & val) ? true : false;
    val <<= 1;
    this->z = val == 0;
    this->n = (val & 0b10000000) ? true : false;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::RMB0_ZP(){
    BYTE val = this->ZeroPageVal();
    val &= 0b11111110;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
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
void CPU::ASL_ACC(){
    this->c = (0b10000000 & this->A) ? true : false;
    this->A <<= 1;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 1;
}
void CPU::TSB_A(){
    BYTE val = this->AbsoluteVal();
    this->z = (val & this->A) ? true : false;
    val = val | this->A;
    unsigned short ptr = this->AbsolutePtr();
    this->mmu->StoreByte(ptr, val);
}
void CPU::ORA_A(){
    BYTE val = this->AbsoluteVal();
    this->A |= val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 3;
}
void CPU::ASL_A(){
    BYTE val = this->AbsoluteVal();
    this->c = (0b10000000 & this->A) ? true : false;
    val <<= 1;
    this->z = val == 0;
    this->n = (val & 0b10000000) ? true : false;
    unsigned short ptr = this->AbsolutePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 3;
}
void CPU::BBR0(){
    BYTE val = this->ZeroPageVal();
    this->PC += 1;
    unsigned short ptr = this->RelativePtr();
    this->PC += 2;
    if (!(val & 0b1)){
        this->PC = ptr;
    }
}
void CPU::BPL(){
    unsigned short ptr = this->RelativePtr();
    if (!this->n){
        this->PC = ptr;
    }
    else {
        this->PC += 2;
    }
}
void CPU::RMB1_ZP(){
    BYTE val = this->ZeroPageVal();
    val &= 0b11111101;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::CLC(){
    this->c = false;
    this->PC += 1;
}
void CPU::ORA_AY(){
    BYTE val = this->AbsoluteYVal();
    this->A |= val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 3;
}
void CPU::INC_ACC(){
    this->A += 1;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 1;
}
void CPU::ORA_AX(){
    BYTE val = this->AbsoluteXVal();
    this->A |= val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 3;
}
void CPU::BBR1(){
    BYTE val = this->ZeroPageVal();
    this->PC += 1;
    unsigned short ptr = this->RelativePtr();
    this->PC += 2;
    if (!(val & 0b10)){
        this->PC = ptr;
    }
}
void CPU::JSR(){
    unsigned short arg = this->AbsolutePtr();
    unsigned short return_addr = this->PC + 2;
    this->PushShort(return_addr);
    this->PC = arg;
}
void CPU::BIT_ZP(){
    BYTE arg = this->ZeroPageVal();
    BYTE result = arg & this->A;
    this->z = result == 0;
    this->v = (arg & 0b01000000) ? true : false;
    this->n = (arg & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::AND_ZP(){
    BYTE arg = this->ZeroPageVal();
    this->A &= arg;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::ROL_ZP(){
    BYTE val = this->ZeroPageVal();
    bool old_carry = this->c;
    this->c = (val & 0b10000000) ? true : false;
    val <<= 1;
    val += (old_carry) ? 1 : 0;
    this->z = val == 0;
    this->n = (val & 0b10000000) ? true : false;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::RMB2_ZP(){
    BYTE val = this->ZeroPageVal();
    val &= 0b11111011;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
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
void CPU::ROL_ACC(){
    bool old_carry = this->c;
    this->c = (this->A & 0b10000000) ? true : false;
    this->A <<= 1;
    this->A += (old_carry) ? 1 : 0;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 1;
}
void CPU::AND_A(){
    BYTE val = this->AbsoluteVal();
    this->A &= val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 3;
}
void CPU::ROL_A(){
    BYTE val = this->AbsoluteVal();
    bool old_carry = this->c;
    this->c = (this->A & 0b10000000) ? true : false;
    val <<= 1;
    val += (old_carry) ? 1 : 0;
    this->z = val == 0;
    this->n = (val & 0b10000000) ? true : false;
    unsigned short ptr = this->AbsolutePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 3;
}
void CPU::BBR2(){
    BYTE val = this->ZeroPageVal();
    this->PC += 1;
    unsigned short ptr = this->RelativePtr();
    this->PC += 2;
    if (!(val & 0b100)){
        this->PC = ptr;
    }
}
void CPU::BMI(){
    if (this->n){
        unsigned short ptr = this->RelativePtr();
        this->PC = ptr;
    }
    else {
        this->PC += 2;
    }
}
void CPU::RMB3_ZP(){
    BYTE val = this->ZeroPageVal();
    val &= 0b11110111;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::SEC(){
    this->c = true;
    this->PC += 1;
}
void CPU::AND_AY(){
    BYTE val = this->AbsoluteYVal();
    this->A &= val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 3;
}
void CPU::DEC_ACC(){
    this->A -= 1;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 1;
}
void CPU::AND_AX(){
    BYTE val = this->AbsoluteXVal();
    this->A &= val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 3;
}
void CPU::BBR3(){
    BYTE val = this->ZeroPageVal();
    this->PC += 1;
    unsigned short ptr = this->RelativePtr();
    this->PC += 2;
    if (!(val & 0b1000)){
        this->PC = ptr;
    }
}
void CPU::RTI(){
    BYTE flags = this->Pop();
    this->ImportFlags(flags);
    unsigned short return_addr = this->PopShort();
    this->PC = return_addr;
    this->EndInterrupt();
}
void CPU::EOR_ZP(){
    BYTE val = this->ZeroPageVal();
    this->A ^= val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::LSR_ZP(){
    BYTE val = this->ZeroPageVal();
    this->c = (val & 0b00000001) ? true : false;
    val >>= 1;
    this->z = val == 0;
    this->n = (val & 0b10000000) ? true : false;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::RMB4_ZP(){
    BYTE val = this->ZeroPageVal();
    val &= 0b11101111;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::PHA(){
    this->Push(this->A);
    this->PC += 1;
}
void CPU::EOR_I(){
    BYTE val = this->ImmediateVal();
    this->A ^= val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::LSR_ACC(){
    this->c = (0b00000001 & this->A) ? true : false;
    this->A >>= 1;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 1;
}
void CPU::JMP_A(){
    unsigned short arg = this->AbsolutePtr();
    this->PC = arg;
}
void CPU::EOR_A(){
    BYTE val = this->AbsoluteVal();
    this->A ^= val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 3;
}
void CPU::LSR_A(){
    BYTE val = this->AbsoluteVal();
    this->c = (0b00000001 & val) ? true : false;
    val >>= 1;
    this->z = val == 0;
    this->n = (val & 0b10000000) ? true : false;
    unsigned short ptr = this->AbsolutePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 3;
}
void CPU::BBR4(){
    BYTE val = this->ZeroPageVal();
    this->PC += 1;
    unsigned short ptr = this->RelativePtr();
    this->PC += 2;
    if (!(val & 0b10000)){
        this->PC = ptr;
    }
}
void CPU::RMB5_ZP(){
    BYTE val = this->ZeroPageVal();
    val &= 0b11011111;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::CLI(){
    this->i = false;
    this->PC += 1;
}
void CPU::PHY(){
    this->Push(this->Y);
    this->PC += 1;
}
void CPU::RTS(){
    unsigned short return_addr = this->PopShort();
    this->PC = return_addr + 1;
}
void CPU::BBR5(){
    BYTE val = this->ZeroPageVal();
    this->PC += 1;
    unsigned short ptr = this->RelativePtr();
    this->PC += 2;
    if (!(val & 0b100000)){
        this->PC = ptr;
    }
}
void CPU::STZ_ZP(){
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, 0);
    this->PC += 2;
}
void CPU::ADC_ZP(){
    BYTE add = this->ZeroPageVal();
    unsigned short sum = this->A + add + (this->c ? 1 : 0);
    if (this->d){
        if (((this->A ^ add ^ sum) & 0x10) == 0x10){
            sum += 0x06;
        }
        if ((sum & 0xF0) > 0x90){
            sum += 0x60;
        }
    }
    bool c_6 = (((this->A&0x7F) + (add&0x7F) + (this->c ? 1 : 0)) & 0b10000000) ? true : false;
    this->A = (BYTE)sum;
    this->c = sum > 0xFF;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->v = (c_6 ^ this->c) ? true : false;
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
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 1;
}
void CPU::ADC_I(){
    BYTE add = this->ImmediateVal();
    unsigned short sum = this->A + add + (this->c ? 1 : 0);
    if (this->d){
        if (((this->A ^ add ^ sum) & 0x10) == 0x10){
            sum += 0x06;
        }
        if ((sum & 0xF0) > 0x90){
            sum += 0x60;
        }
    }
    bool c_6 = (((this->A&0x7F) + (add&0x7F) + (this->c ? 1 : 0)) & 0b10000000) ? true : false;
    this->A = (BYTE)sum;
    this->c = sum > 0xFF;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->v = (c_6 ^ this->c) ? true : false;
    this->PC += 2;
}
void CPU::ROR_ACC(){
    bool old_carry = this->c;
    this->c = (this->A & 0b00000001) ? true : false;
    this->A >>= 1;
    if (old_carry){
        this->A |= 0b10000000;
    }
    this->n = (this->A & 0b10000000) ? true : false;
    this->z = this->A == 0;
    this->PC += 1;
}
void CPU::JMP_I(){
    unsigned short arg = this->IndirectVal();
    this->PC = arg;
}
void CPU::ADC_A(){
    BYTE add = this->AbsoluteVal();
    unsigned short sum = this->A + add + (this->c ? 1 : 0);
    if (this->d){
        if (((this->A ^ add ^ sum) & 0x10) == 0x10){
            sum += 0x06;
        }
        if ((sum & 0xF0) > 0x90){
            sum += 0x60;
        }
    }
    bool c_6 = (((this->A&0x7F) + (add&0x7F) + (this->c ? 1 : 0)) & 0b10000000) ? true : false;
    this->A = (BYTE)sum;
    this->c = sum > 0xFF;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->v = (c_6 ^ this->c) ? true : false;
    this->PC += 3;
}
void CPU::ROR_A(){
    BYTE val = this->AbsoluteVal();
    bool old_carry = this->c;
    this->c = (val & 0b00000001) ? true : false;
    val >>= 1;
    if (old_carry){
        val |= 0b10000000;
    }
    this->n = (val & 0b10000000) ? true : false;
    this->z = val == 0;
    unsigned short ptr = this->AbsolutePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 3;
}
void CPU::BBR6(){
    BYTE val = this->ZeroPageVal();
    this->PC += 1;
    unsigned short ptr = this->RelativePtr();
    this->PC += 2;
    if (!(val & 0b1000000)){
        this->PC = ptr;
    }
}
void CPU::ADC_INDIRECT_INDEXED(){
    BYTE add = this->IndirectIndexedVal();
    unsigned short sum = this->A + add + (this->c ? 1 : 0);
    if (this->d){
        if (((this->A ^ add ^ sum) & 0x10) == 0x10){
            sum += 0x06;
        }
        if ((sum & 0xF0) > 0x90){
            sum += 0x60;
        }
    }
    bool c_6 = (((this->A&0x7F) + (add&0x7F) + (this->c ? 1 : 0)) & 0b10000000) ? true : false;
    this->A = (BYTE)sum;
    this->c = sum > 0xFF;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->v = (c_6 ^ this->c) ? true : false;
    this->PC += 2;
}
void CPU::ADC_IZP(){
    BYTE add = this->IndirectZeroPageVal();
    unsigned short sum = this->A + add + (this->c ? 1 : 0);
    if (this->d){
        if (((this->A ^ add ^ sum) & 0x10) == 0x10){
            sum += 0x06;
        }
        if ((sum & 0xF0) > 0x90){
            sum += 0x60;
        }
    }
    bool c_6 = (((this->A&0x7F) + (add&0x7F) + (this->c ? 1 : 0)) & 0b10000000) ? true : false;
    this->A = (BYTE)sum;
    this->c = sum > 0xFF;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->v = (c_6 ^ this->c) ? true : false;
    this->PC += 2;
}
void CPU::RMB7_ZP(){
    BYTE val = this->ZeroPageVal();
    val &= 0b01111111;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::SEI(){
    this->i = true;
    this->PC++;
}
void CPU::PLY(){
    this->Y = this->Pop();
    this->z = this->Y == 0;
    this->n = (this->Y & 0b10000000) ? true : false;
    this->PC += 1;
}
void CPU::JMP_ABSOLUTE_INDEXED_INDIRECT(){
    unsigned short arg = this->AbsoluteIndexIndirectVal();
    this->PC = arg;
}
void CPU::BBR7(){
    BYTE val = this->ZeroPageVal();
    this->PC += 1;
    unsigned short ptr = this->RelativePtr();
    this->PC += 2;
    if (!(val & 0b10000000)){
        this->PC = ptr;
    }
}
void CPU::BRA(){
    unsigned short ptr = this->RelativePtr();
    this->PC = ptr;
}
void CPU::STY_ZP(){
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, this->Y);
    this->PC += 2;
}
void CPU::STA_ZP(){
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, this->A);
    this->PC += 2;
}
void CPU::STX_ZP(){
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, this->X);
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
void CPU::TXA(){
    this->A = this->X;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 1;
}
void CPU::STY_A(){
    unsigned short ptr = this->AbsolutePtr();
    this->mmu->StoreByte(ptr, this->Y);
    this->PC += 3;
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
void CPU::BBS0(){
    BYTE val = this->ZeroPageVal();
    this->PC += 1;
    unsigned short ptr = this->RelativePtr();
    this->PC += 2;
    if (val & 0b1){
        this->PC = ptr;
    }
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
void CPU::STA_INDIRECT_INDEXED(){
    unsigned short ptr = this->IndirectIndexedPtr();
    this->mmu->StoreByte(ptr, this->A);
    this->PC += 2;
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
void CPU::SMB1_ZP(){
    BYTE val = this->ZeroPageVal();
    val |= 0b00000010;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::TYA(){
    this->A = this->Y;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 1;
}
void CPU::STA_AY(){
    unsigned short ptr = this->AbsoluteYPtr();
    this->mmu->StoreByte(ptr, this->A);
    this->PC += 3;
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
void CPU::STZ_AX(){
    unsigned short ptr = this->AbsoluteXPtr();
    this->mmu->StoreByte(ptr, 0);
    this->PC += 3;
}
void CPU::BBS1(){
    BYTE val = this->ZeroPageVal();
    this->PC += 1;
    unsigned short ptr = this->RelativePtr();
    this->PC += 2;
    if (val & 0b10){
        this->PC = ptr;
    }
}
void CPU::LDY_I(){
    BYTE val = this->ImmediateVal();
    this->Y = val;
    this->z = this->Y == 0;
    this->n = (this->Y & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::LDA_INDEXED_INDIRECT(){
    BYTE val = this->IndexedIndirectVal();
    this->A = val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::LDX_I(){
    BYTE val = this->ImmediateVal();
    this->X = val;
    this->z = this->X == 0;
    this->n = (this->X & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::LDY_ZP(){
    BYTE val = this->ZeroPageVal();
    this->Y = val;
    this->z = this->Y == 0;
    this->n = (this->Y & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::LDA_ZP(){
    BYTE val = this->ZeroPageVal();
    this->A = val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::LDX_ZP(){
    BYTE val = this->ZeroPageVal();
    this->X = val;
    this->z = this->X == 0;
    this->n = (this->X & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::SMB2_ZP(){
    BYTE val = this->ZeroPageVal();
    val |= 0b00000100;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::TAY(){
    this->Y = this->A;
    this->z = this->Y == 0;
    this->n = (this->Y & 0b10000000) ? true : false;
    this->PC += 1;
}
void CPU::LDA_I(){
    BYTE arg = this->ImmediateVal();
    this->A = arg;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::TAX(){
    this->X = this->A;
    this->z = this->X == 0;
    this->n = (this->X & 0b10000000) ? true : false;
    this->PC += 1;
}
void CPU::LDY_A(){
    BYTE val = this->AbsoluteVal();
    this->Y = val;
    this->z = this->Y == 0;
    this->n = (this->Y & 0b10000000) ? true : false;
    this->PC += 3;
}
void CPU::LDA_A(){
    BYTE val = this->AbsoluteVal();
    this->A = val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 3;
}
void CPU::LDX_A(){
    BYTE val = this->AbsoluteVal();
    this->X = val;
    this->z = this->X == 0;
    this->n = (this->X & 0b10000000) ? true : false;
    this->PC += 3;
}
void CPU::BBS2(){
    BYTE val = this->ZeroPageVal();
    this->PC += 1;
    unsigned short ptr = this->RelativePtr();
    this->PC += 2;
    if (val & 0b100){
        this->PC = ptr;
    }
}
void CPU::BCS(){
    unsigned short ptr = this->RelativePtr();
    if (this->c){
        this->PC = ptr;
    }
    else {
        this->PC += 2;
    }
}
void CPU::LDA_INDIRECT_INDEXED(){
    BYTE val = this->IndirectIndexedVal();
    this->A = val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::LDA_IZP(){
    BYTE val = this->IndirectZeroPageVal();
    this->A = val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::LDY_ZPX(){
    BYTE val = this->ZeroPageXVal();
    this->Y = val;
    this->z = this->Y == 0;
    this->n = (this->Y & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::LDA_ZPX(){
    BYTE val = this->ZeroPageXVal();
    this->A = val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::SMB3_ZP(){
    BYTE val = this->ZeroPageVal();
    val |= 0b00001000;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::LDA_AY(){
    BYTE val = this->AbsoluteYVal();
    this->A = val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 3;
}
void CPU::LDY_AX(){
    BYTE val = this->AbsoluteXVal();
    this->Y = val;
    this->z = this->Y == 0;
    this->n = (this->Y & 0b10000000) ? true : false;
    this->PC += 3;
}
void CPU::LDA_AX(){
    BYTE val = this->AbsoluteXVal();
    this->A = val;
    this->z = this->A == 0;
    this->n = (this->A & 0b10000000) ? true : false;
    this->PC += 3;
}
void CPU::LDX_AY(){
    BYTE val = this->AbsoluteYVal();
    this->X = val;
    this->z = this->X == 0;
    this->n = (this->X & 0b10000000) ? true : false;
    this->PC += 3;
}
void CPU::BBS3(){
    BYTE val = this->ZeroPageVal();
    this->PC += 1;
    unsigned short ptr = this->RelativePtr();
    this->PC += 2;
    if (val & 0b00001000){
        this->PC = ptr;
    }
}
void CPU::CPY_I(){
    BYTE arg = this->ImmediateVal();
    this->c = this->Y >= arg;
    this->z = this->Y == arg;
    BYTE result = this->Y - arg;
    this->n = (result & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::CPY_ZP(){
    BYTE arg = this->ZeroPageVal();
    this->c = this->Y >= arg;
    this->z = this->Y == arg;
    BYTE result = this->Y - arg;
    this->n = (result & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::CMP_ZP(){
    BYTE arg = this->ZeroPageVal();
    this->c = this->A >= arg;
    this->z = this->A == arg;
    BYTE result = this->A - arg;
    this->n = (result & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::DEC_ZP(){
    BYTE val = this->ZeroPageVal();
    val -= 1;
    unsigned short ptr = this->ZeroPagePtr();
    this->z = val == 0;
    this->n = (val & 0b10000000) ? true : false;
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::SMB4_ZP(){
    BYTE val = this->ZeroPageVal();
    val |= 0b00010000;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::INY(){
    this->Y += 1;
    this->z =this->Y == 0;
    this->n = (this->Y & 0b10000000) ? true : false;
    this->PC += 1;
}
void CPU::CMP_I(){
    BYTE val = this->ImmediateVal();
    this->c = this->A >= val;
    this->z = this->A == val;
    BYTE result = this->A - val;
    this->n = (result & 0b10000000) ? true : false;
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
void CPU::CPY_A(){
    BYTE val = this->AbsoluteVal();
    this->c = this->Y >= val;
    this->z = this->Y == val;
    BYTE result = this->Y - val;
    this->n = (result & 0b10000000) ? true : false;
    this->PC += 3;
}
void CPU::CMP_A(){
    BYTE val = this->AbsoluteVal();
    this->c = this->A >= val;
    this->z = this->A == val;
    BYTE result = this->A - val;
    this->n = (result & 0b10000000) ? true : false;
    this->PC += 3;
}
void CPU::DEC_A(){
    BYTE val = this->AbsoluteVal();
    val -= 1;
    this->z = val == 0;
    this->n = (val & 0b10000000) ? true : false;
    unsigned short ptr = this->AbsolutePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 3;
}
void CPU::BBS4(){
    BYTE val = this->ZeroPageVal();
    this->PC += 1;
    unsigned short ptr = this->RelativePtr();
    this->PC += 2;
    if (val & 0b00010000){
        this->PC = ptr;
    }
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
void CPU::DEC_ZPX(){
    BYTE val = this->ZeroPageXVal();
    val -= 1;
    this->z = val == 0;
    this->n = (val & 0b10000000) ? true : false;
    unsigned short ptr = this->ZeroPageXPtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::SMB5_ZP(){
    BYTE val = this->ZeroPageVal();
    val |= 0b00100000;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::CLD(){
    this->d = false;
    this->PC += 1;
}
void CPU::PHX(){
    this->Push(this->X);
    this->PC += 1;
}
void CPU::CMP_AX(){
    BYTE val = this->AbsoluteXVal();
    this->c = this->A >= val;
    this->z = this->A == val;
    BYTE result = this->A - val;
    this->n = (result & 0b10000000) ? true : false;
    this->PC += 3;
}
void CPU::DEC_AX(){
    BYTE val = this->AbsoluteXVal();
    val -= 1;
    this->z = val == 0;
    this->n = (val & 0b10000000) ? true : false;
    unsigned short ptr = this->AbsoluteXPtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 3;
}
void CPU::BBS5(){
    BYTE val = this->ZeroPageVal();
    this->PC += 1;
    unsigned short ptr = this->RelativePtr();
    this->PC += 2;
    if (val & 0b00100000){
        this->PC = ptr;
    }
}
void CPU::CPX_I(){
    BYTE arg = this->ImmediateVal();
    this->c = this->X >= arg;
    this->z = this->X == arg;
    BYTE result = this->X - arg;
    this->n = (result & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::CPX_ZP(){
    BYTE arg = this->ZeroPageVal();
    this->c = this->X >= arg;
    this->z = this->X == arg;
    BYTE result = this->X - arg;
    this->n = (result & 0b10000000) ? true : false;
    this->PC += 2;
}
void CPU::SBC_ZP(){
    BYTE val = this->ZeroPageVal();
    signed short twos = (signed char)this->A - (signed char)val - (this->c ? 0 : 1);
    BYTE c = this->A >= (val + (this->c ? 0 : 1));
    signed short result = this->A - val - (this->c ? 0 : 1);
    BYTE bin_value = result;
    this->z = bin_value == 0;
    this->n = (bin_value & 0b10000000) ? true : false;
    if (this->d){
        signed short lowresult = (this->A & 0x0F) - (val & 0x0F) - (this->c ? 0 : 1);
        if (lowresult < 0){
            lowresult = ((lowresult - 6) & 0x0F) - 16;
        }
        result = (this->A & 0xF0) - (val & 0xF0) + lowresult;
        if (result < 0){
            result = result - 0x60;
        }
        this->A = result;
    }
    else {
        this->A = bin_value;
    }

    this->v = twos < -128 || twos > 127;
    this->c = c;
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
void CPU::SBC_I(){
    BYTE val = this->ImmediateVal();
    signed short twos = (signed char)this->A - (signed char)val - (this->c ? 0 : 1);
    BYTE c = this->A >= (val + (this->c ? 0 : 1));
    signed short result = this->A - val - (this->c ? 0 : 1);
    BYTE bin_value = result;
    this->z = bin_value == 0;
    this->n = (bin_value & 0b10000000) ? true : false;
    if (this->d){
        signed short lowresult = (this->A & 0x0F) - (val & 0x0F) - (this->c ? 0 : 1);
        if (lowresult < 0){
            lowresult = ((lowresult - 6) & 0x0F) - 16;
        }
        result = (this->A & 0xF0) - (val & 0xF0) + lowresult;
        if (result < 0){
            result = result - 0x60;
        }
        this->A = result;
    }
    else {
        this->A = bin_value;
    }

    this->v = twos < -128 || twos > 127;
    this->c = c;
    this->PC += 2;
}
void CPU::NOP(){
    this->PC += 1;
}
void CPU::CPX_A(){
    BYTE val = this->AbsoluteVal();
    this->c = this->X >= val;
    this->z = this->X == val;
    BYTE result = this->X - val;
    this->n = (result & 0b10000000) ? true : false;
    this->PC += 3;
}
void CPU::SBC_A(){
    BYTE val = this->AbsoluteVal();
    signed short twos = (signed char)this->A - (signed char)val - (this->c ? 0 : 1);
    BYTE c = this->A >= (val + (this->c ? 0 : 1));
    signed short result = this->A - val - (this->c ? 0 : 1);
    BYTE bin_value = result;
    this->z = bin_value == 0;
    this->n = (bin_value & 0b10000000) ? true : false;
    if (this->d){
        signed short lowresult = (this->A & 0x0F) - (val & 0x0F) - (this->c ? 0 : 1);
        if (lowresult < 0){
            lowresult = ((lowresult - 6) & 0x0F) - 16;
        }
        result = (this->A & 0xF0) - (val & 0xF0) + lowresult;
        if (result < 0){
            result = result - 0x60;
        }
        this->A = result;
    }
    else {
        this->A = bin_value;
    }

    this->v = twos < -128 || twos > 127;
    this->c = c;
    this->PC += 3;
}
void CPU::INC_A(){
    BYTE val = this->AbsoluteVal();
    val += 1;
    this->z = val == 0;
    this->n = (val & 0b10000000) ? true : false;
    unsigned short ptr = this->AbsolutePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 3;
}
void CPU::BBS6(){
    BYTE val = this->ZeroPageVal();
    this->PC += 1;
    unsigned short ptr = this->RelativePtr();
    this->PC += 2;
    if (val & 0b01000000){
        this->PC = ptr;
    }
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
void CPU::SBC_IZP(){
    BYTE val = this->IndirectZeroPageVal();
    signed short twos = (signed char)this->A - (signed char)val - (this->c ? 0 : 1);
    BYTE c = this->A >= (val + (this->c ? 0 : 1));
    signed short result = this->A - val - (this->c ? 0 : 1);
    BYTE bin_value = result;
    this->z = bin_value == 0;
    this->n = (bin_value & 0b10000000) ? true : false;
    if (this->d){
        signed short lowresult = (this->A & 0x0F) - (val & 0x0F) - (this->c ? 0 : 1);
        if (lowresult < 0){
            lowresult = ((lowresult - 6) & 0x0F) - 16;
        }
        result = (this->A & 0xF0) - (val & 0xF0) + lowresult;
        if (result < 0){
            result = result - 0x60;
        }
        this->A = result;
    }
    else {
        this->A = bin_value;
    }

    this->v = twos < -128 || twos > 127;
    this->c = c;
    this->PC += 2;
}
void CPU::INC_ZPX(){
    BYTE val = this->ZeroPageXVal();
    val += 1;
    this->z = val == 0;
    this->n = (val & 0b10000000) ? true : false;
    unsigned short ptr = this->ZeroPageXPtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::SMB7_ZP(){
    BYTE val = this->ZeroPageVal();
    val |= 0b10000000;
    unsigned short ptr = this->ZeroPagePtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 2;
}
void CPU::SED(){
    this->d = true;
    this->PC += 1;
}
void CPU::PLX(){
    this->X = this->Pop();
    this->z = this->X == 0;
    this->n = (this->X & 0b10000000) ? true : false;
    this->PC += 1;
}
void CPU::INC_AX(){
    BYTE val = this->AbsoluteXVal();
    val += 1;
    this->z = val == 0;
    this->n = (val & 0b10000000) ? true : false;
    unsigned short ptr = this->AbsoluteXPtr();
    this->mmu->StoreByte(ptr, val);
    this->PC += 3;
}
void CPU::BBS7(){
    BYTE val = this->ZeroPageVal();
    this->PC += 1;
    unsigned short ptr = this->RelativePtr();
    this->PC += 2;
    if (val & 0b10000000){
        this->PC = ptr;
    }
}
