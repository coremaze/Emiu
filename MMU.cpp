#include "MMU.h"
#include "CPU.h"
#include "DMA.h"
#include <iostream>
MMU::MMU(CPU* cpu){
    this->cpu = cpu;
}

void MMU::StoreByte(unsigned short address, BYTE by){

    //if (address== 0x8000){
    //    printf("0x8000 from %04X\n writing %02X\n", this->cpu->PC, (unsigned int)by);
    //}
    if ((address >= REGISTERS_START) && (address < (REGISTERS_START + REGISTERS_LENGTH))){ //Register?
        if (address == BTREQ){
            by = ~by;
            this->cpu->memRegisters[address] &= by;
        }
        else if (address == DPTR){
            this->cpu->dma->SetDPTRL(by);
        }
        else if (address == DPTR+1){
            this->cpu->dma->SetDPTRH(by);
        }
        else if (address == DBKR){
            this->cpu->dma->SetDBKRL(by);
        }
        else if (address == DBKR+1){
            this->cpu->dma->SetDBKRH(by);
        }
        else if (address == DCNT){
            this->cpu->dma->SetDCNTL(by);
        }
        else if (address == DCNT+1){
            this->cpu->dma->SetDCNTH(by);
        }
        else {
            this->cpu->memRegisters[address] = by;
        }
    }
    else if ((address >= BRR_START) && (address < (BRR_START + BRR_LENGTH))){ //BRR?
        unsigned short brr = this->cpu->GetBRR();
        unsigned char page = brr & 0x7F; //Select page for OTP or flash
        unsigned short paged_location = (address - BRR_START) + BRR_LENGTH * page; //Calculate location for OTP or flash
        if ( ((brr & 0b1001111000000000) == 0b1111000000000) || (brr == 0) ){ //OTP?
            this->cpu->OTP[paged_location % OTP_SIZE] = by;
        }
        else if ( (brr & 0b1001111100000000) == 0b1100000000 ){ //Video Registers?
            this->cpu->WriteVideoRegister(address, by);
        }
        else if ( (brr & 0b1001110000000000) == 0b10000000000 ){ //Flash?
            this->cpu->Flash[paged_location % FLASH_SIZE] = by;
        }
        else if ( (brr & 0x8000) == 0x8000 ){ //Internal RAM 0x2000 ~ 0x3FFF
            this->cpu->RAM[address] = by; //Fall through
        }
        else {
            printf("Invalid BRR mask: %04X", this->cpu->GetBRR());
        }
    }
    else if ((address >= PRR_START) && (address < (PRR_START + PRR_LENGTH))){ //PRR?
        unsigned short prr = this->cpu->GetPRR();
        unsigned char page = prr & 0x7F; //Select page for OTP or flash
        unsigned short paged_location = (address - PRR_START) + PRR_LENGTH * page; //Calculate location for OTP or flash
        if ( ((prr & 0b1000111100000000) == 0b111100000000) || (prr == 0) ){ //OTP?
            this->cpu->OTP[paged_location % OTP_SIZE] = by;
        }
        else if ( (prr & 0b1000111110000000) == 0b110000000 ){ //Video Registers?
            this->cpu->WriteVideoRegister(address, by);
        }
        else if ( (prr & 0b1000111000000000) == 0b1000000000 ){ //Flash?
            this->cpu->Flash[paged_location % FLASH_SIZE] = by;
        }
        else if ( (prr & 0x8000) == 0x8000 ){ //Internal RAM 0x2000 ~ 0x3FFF
            this->cpu->RAM[address] = by; //Fall through
        }
        else {
            printf("Invalid PRR mask: %04X", this->cpu->GetPRR());
        }
    }
    else if ((address >= DRR_START) && (address < (DRR_START + DRR_LENGTH))){ //PRR?
        unsigned short drr = this->cpu->GetDRR();
        unsigned char page = drr & 0x7F; //Select page for OTP or flash
        unsigned short paged_location = (address - DRR_START) + DRR_LENGTH * page; //Calculate location for OTP or flash
        if ( ((drr & 0b1000111100000000) == 0b111100000000) || (drr == 0) ){ //OTP?
            this->cpu->OTP[paged_location % OTP_SIZE] = by;
        }
        else if ( (drr & 0b1000011111000000) == 0b11000000 ){ //Video Registers?
            this->cpu->WriteVideoRegister(address, by);
        }
        else if ( (drr & 0b1000011100000000) == 0b100000000 ){ //Flash?
            this->cpu->Flash[paged_location % FLASH_SIZE] = by;
        }
        else if ( (drr & 0x8000) == 0x8000 ){ //Internal RAM 0x2000 ~ 0x3FFF
            this->cpu->RAM[address] = by; //Fall through
        }
        else {
            printf("Invalid DRR mask: %04X", this->cpu->GetDRR());
        }
    }
    else {
        this->cpu->RAM[address] = by;
    }
}

BYTE MMU::ReadByte(unsigned short address){
    if ((address >= REGISTERS_START) && (address < (REGISTERS_START + REGISTERS_LENGTH))){ //Register?
        if (address == DPTR){
            return this->cpu->dma->GetDPTRL();
        }
        else if (address == DPTR+1){
            return this->cpu->dma->GetDPTRH();
        }
        else if (address == DBKR){
            return this->cpu->dma->GetDBKRL();
        }
        else if (address == DBKR+1){
            return this->cpu->dma->GetDBKRH();
        }
        else if (address == DCNT){
            return this->cpu->dma->GetDCNTL();
        }
        else if (address == DCNT+1){
            return this->cpu->dma->GetDCNTH();
        }
        else {
            return this->cpu->memRegisters[address];
        }
    }
    else if ((address >= BRR_START) && (address < (BRR_START + BRR_LENGTH))){ //BRR?
        unsigned short brr = this->cpu->GetBRR();
        unsigned char page = brr & 0x7F; //Select page for OTP or flash
        unsigned short paged_location = (address - BRR_START) + BRR_LENGTH * page; //Calculate location for OTP or flash
        if ( ((brr & 0b1001111000000000) == 0b1111000000000) || (brr == 0) ){ //OTP?
            return this->cpu->OTP[paged_location % OTP_SIZE];
        }
        else if ( (brr & 0b1001111100000000) == 0b1100000000 ){ //Video Registers?
            return this->cpu->videoRegisters[address % 2];
        }
        else if ( (brr & 0b1001110000000000) == 0b10000000000 ){ //Flash?
            return this->cpu->Flash[paged_location % FLASH_SIZE];
        }
        else if ( (brr & 0x8000) == 0x8000 ){ //Internal RAM 0x2000 ~ 0x3FFF
            return this->cpu->RAM[address];
        }
        else {
            printf("Invalid BRR mask: %04X", this->cpu->GetBRR());
        }
    }
    else if ((address >= PRR_START) && (address < (PRR_START + PRR_LENGTH))){ //PRR?
        unsigned short prr = this->cpu->GetPRR();
        unsigned char page = prr & 0x7F; //Select page for OTP or flash
        unsigned short paged_location = (address - PRR_START) + PRR_LENGTH * page; //Calculate location for OTP or flash
        if ( ((prr & 0b1000111100000000) == 0b111100000000) || (prr == 0) ){ //OTP?
            return this->cpu->OTP[paged_location % OTP_SIZE];
        }
        else if ( (prr & 0b1000111110000000) == 0b110000000 ){ //Video Registers?
            return this->cpu->videoRegisters[address % 2];
        }
        else if ( (prr & 0b1000111000000000) == 0b1000000000 ){ //Flash?
            return this->cpu->Flash[paged_location % FLASH_SIZE];
        }
        else if ( (prr & 0x8000) == 0x8000 ){ //Internal RAM 0x2000 ~ 0x3FFF
            return this->cpu->RAM[address]; //Fall through
        }
        else {
            printf("Invalid PRR mask: %04X", this->cpu->GetPRR());
        }
    }
    else if ((address >= DRR_START) && (address < (DRR_START + DRR_LENGTH))){ //PRR?
        unsigned short drr = this->cpu->GetDRR();
        unsigned char page = drr & 0x7F; //Select page for OTP or flash
        unsigned short paged_location = (address - DRR_START) + DRR_LENGTH * page; //Calculate location for OTP or flash
        if ( ((drr & 0b1000111100000000) == 0b111100000000) || (drr == 0) ){ //OTP?
            return this->cpu->OTP[paged_location % OTP_SIZE];
        }
        else if ( (drr & 0b1000011111000000) == 0b11000000 ){ //Video Registers?
            return this->cpu->videoRegisters[address % 2];
        }
        else if ( (drr & 0b1000011100000000) == 0b100000000 ){ //Flash?
            return this->cpu->Flash[paged_location % FLASH_SIZE];
        }
        else if ( (drr & 0x8000) == 0x8000 ){ //Internal RAM 0x2000 ~ 0x3FFF
            return this->cpu->RAM[address]; //Fall through
        }
        else {
            printf("Invalid DRR mask: %04X", this->cpu->GetDRR());
        }
    }
    else {
        return this->cpu->RAM[address];
    }
}

void MMU::StoreShort(unsigned short address, unsigned short sh){
    BYTE low;
    BYTE high;
    low = (BYTE)(sh & 0xFF);
    high = (BYTE)((sh & 0xFF00) >> 8);
    this->StoreByte(address, low);
    this->StoreByte(address+1, high);
}

unsigned short MMU::ReadShort(unsigned short address){
    BYTE low = this->ReadByte(address);
    BYTE high = this->ReadByte(address+1);
    unsigned short sh = (((unsigned short)high) << 8 ) | ((unsigned short)low);
    return sh;
}
