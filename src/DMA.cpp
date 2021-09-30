#include "CPU.h"
#include "DMA.h"
#include "MMU.h"
#include <iostream>

DMA::DMA(CPU* cpu){
    this->cpu = cpu;
}

void DMA::SetDPTRL(u8 by){
    u8 DSEL_value = this->cpu->mmu->ReadByte(DSEL);

    if (DSEL_value & 1) { //Select destination
        this->destPtr = (this->destPtr & 0xFF00) | (unsigned short)by;
    }
    else { //Select source
        this->sourcePtr = (this->sourcePtr & 0xFF00) | (unsigned short)by;
    }
}

u8 DMA::GetDPTRL(){
    u8 DSEL_value = this->cpu->mmu->ReadByte(DSEL);

    if (DSEL_value & 1) { //Select destination
        return this->destPtr & 0xFF;
    }
    else { //Select source
        return this->sourcePtr & 0xFF;
    }
}

void DMA::SetDPTRH(u8 by){
    u8 DSEL_value = this->cpu->mmu->ReadByte(DSEL);

    if (DSEL_value & 1) { //Select destination
        this->destPtr = (this->destPtr & 0x00FF) | (((unsigned short)by) << 8) | 0x8000; //highest bit always set
    }
    else { //Select source
        this->sourcePtr = (this->sourcePtr & 0x00FF) | (((unsigned short)by) << 8) | 0x8000;
    }
}

u8 DMA::GetDPTRH(){
    u8 DSEL_value = this->cpu->mmu->ReadByte(DSEL);

    if (DSEL_value & 1) { //Select destination
        return (this->destPtr & 0xFF00) >> 8;
    }
    else { //Select source
        return (this->sourcePtr & 0xFF00) >> 8;
    }
}

void DMA::SetDBKRL(u8 by){
    u8 DSEL_value = this->cpu->mmu->ReadByte(DSEL);

    if (DSEL_value & 1) { //Select destination
        this->destBank = (this->destBank & 0xFF00) | by;
    }
    else { //Select source
        this->sourceBank = (this->sourceBank & 0xFF00) | by;
    }
}

u8 DMA::GetDBKRL(){
    u8 DSEL_value = this->cpu->mmu->ReadByte(DSEL);

    if (DSEL_value & 1) { //Select destination
        return this->destBank & 0xFF;
    }
    else { //Select source
        return this->sourceBank & 0xFF;
    }
}

void DMA::SetDBKRH(u8 by){
    u8 DSEL_value = this->cpu->mmu->ReadByte(DSEL);

    if (DSEL_value & 1) { //Select destination
        this->destBank = (this->destBank & 0x00FF) | (((unsigned short)by) << 8);
    }
    else { //Select source
        this->sourceBank = (this->sourceBank & 0x00FF) | (((unsigned short)by) << 8);
    }
}

u8 DMA::GetDBKRH(){
    u8 DSEL_value = this->cpu->mmu->ReadByte(DSEL);

    if (DSEL_value & 1) { //Select destination
        return (this->destBank & 0xFF00) >> 8;
    }
    else { //Select source
        return (this->sourceBank & 0xFF00) >> 8;
    }
}

void DMA::SetDCNTL(u8 by){
    this->number = (this->number & 0xFF00) | by;
}

u8 DMA::GetDCNTL(){
    return this->number & 0xFF;
}

void DMA::SetDCNTH(u8 by){
    this->number = (this->number & 0x00FF) | (((unsigned short)by) << 8);
    this->Execute();
}

u8 DMA::GetDCNTH(){
    return (this->number & 0xFF00) >> 8;
}

void DMA::Execute(){
    unsigned short old_DRR = this->cpu->GetDRR();
    u8 DMOD_value = this->cpu->mmu->ReadByte(DMOD);
    u8 source_mode = DMOD_value & 0b000011;
    u8 dest_mode = (DMOD_value & 0b001100) >> 2;
    u8 func = (DMOD_value & 0b110000) >> 4;

    //printf("PC:%04X %02X DRR:%04X\n", this->cpu->PC, dest_mode, this->cpu->GetDRR());

    unsigned short old_sourcePtr = this->sourcePtr;
    unsigned short old_destPtr = this->destPtr;

    for (int i = 0; i<(this->number+1); i++){
        this->cpu->mmu->StoreShort(DRR, this->sourceBank);

        u8 source_byte = this->cpu->mmu->ReadByte(this->sourcePtr);

        this->cpu->mmu->StoreShort(DRR, this->destBank);

        u8 dest_byte = this->cpu->mmu->ReadByte(this->destPtr);

        u8 new_byte;
        //FUNC
        if (func == 0b00){ //Normal mode
            new_byte = source_byte;
        }
        else if (func == 0b01){ //XOR mode
            new_byte = source_byte ^ dest_byte;
        }
        else if (func == 0b10){ //OR mode
            new_byte = source_byte | dest_byte;
        }
        else if (func == 0b11){
            new_byte = source_byte & dest_byte;
        }
        this->cpu->mmu->StoreByte(this->destPtr, new_byte);

        //Pointer modes
        if (source_mode == 0b00){ //Continue mode
            if (this->sourcePtr == 0xFFFF){
                this->sourceBank++;
            }
            this->sourcePtr++;
            this->sourcePtr = this->sourcePtr | 0x8000;
        }
        else if (source_mode == 0b01) { //Reload mode
            if (this->sourcePtr == 0xFFFF){
                this->sourceBank++;
            }
            this->sourcePtr++;
            this->sourcePtr = this->sourcePtr | 0x8000;
        }
        else { //Fixed mode

        }

        if (dest_mode == 0b00){ //Continue mode
            if (this->destPtr == 0xFFFF){
                this->destBank++;
            }
            this->destPtr++;
            this->destPtr = this->destPtr | 0x8000;
        }
        else if (dest_mode == 0b01) { //Reload mode
            if (this->destPtr == 0xFFFF){
                this->destBank++;
            }
            this->destPtr++;
            this->destPtr = this->destPtr | 0x8000;
        }
        else { //Fixed mode

        }
    }

    this->cpu->mmu->StoreShort(DRR, old_DRR);

    if (source_mode == 0b01) {
        this->sourcePtr = old_sourcePtr;
    }
    if (dest_mode == 0b01) { //Reload mode
        this->destPtr = old_destPtr;
    }
}
