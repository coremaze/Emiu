#include "CPU.h"
#include "DMA.h"
#include "MMU.h"
#include <iostream>

DMA::DMA(CPU* cpu){
    this->cpu = cpu;
}

void DMA::SetDPTRL(BYTE by){
    BYTE DSEL_value = this->cpu->mmu->ReadByte(DSEL);

    if (DSEL_value & 1) { //Select destination
        this->destPtr = (this->destPtr & 0xFF00) | (unsigned short)by;
    }
    else { //Select source
        this->sourcePtr = (this->sourcePtr & 0xFF00) | (unsigned short)by;
    }
}

BYTE DMA::GetDPTRL(){
    BYTE DSEL_value = this->cpu->mmu->ReadByte(DSEL);

    if (DSEL_value & 1) { //Select destination
        return this->destPtr & 0xFF;
    }
    else { //Select source
        return this->sourcePtr & 0xFF;
    }
}

void DMA::SetDPTRH(BYTE by){
    BYTE DSEL_value = this->cpu->mmu->ReadByte(DSEL);

    if (DSEL_value & 1) { //Select destination
        this->destPtr = (this->destPtr & 0x00FF) | (((unsigned short)by) << 8) | 0x8000; //highest bit always set
    }
    else { //Select source
        this->sourcePtr = (this->sourcePtr & 0x00FF) | (((unsigned short)by) << 8) | 0x8000;
    }
}

BYTE DMA::GetDPTRH(){
    BYTE DSEL_value = this->cpu->mmu->ReadByte(DSEL);

    if (DSEL_value & 1) { //Select destination
        return (this->destPtr & 0xFF00) >> 8;
    }
    else { //Select source
        return (this->sourcePtr & 0xFF00) >> 8;
    }
}

void DMA::SetDBKRL(BYTE by){
    BYTE DSEL_value = this->cpu->mmu->ReadByte(DSEL);

    if (DSEL_value & 1) { //Select destination
        this->destBank = (this->destBank & 0xFF00) | by;
    }
    else { //Select source
        this->sourceBank = (this->sourceBank & 0xFF00) | by;
    }
}

BYTE DMA::GetDBKRL(){
    BYTE DSEL_value = this->cpu->mmu->ReadByte(DSEL);

    if (DSEL_value & 1) { //Select destination
        return this->destBank & 0xFF;
    }
    else { //Select source
        return this->sourceBank & 0xFF;
    }
}

void DMA::SetDBKRH(BYTE by){
    BYTE DSEL_value = this->cpu->mmu->ReadByte(DSEL);

    if (DSEL_value & 1) { //Select destination
        this->destBank = (this->destBank & 0x00FF) | (((unsigned short)by) << 8);
    }
    else { //Select source
        this->sourceBank = (this->sourceBank & 0x00FF) | (((unsigned short)by) << 8);
    }
}

BYTE DMA::GetDBKRH(){
    BYTE DSEL_value = this->cpu->mmu->ReadByte(DSEL);

    if (DSEL_value & 1) { //Select destination
        return (this->destBank & 0xFF00) >> 8;
    }
    else { //Select source
        return (this->sourceBank & 0xFF00) >> 8;
    }
}

void DMA::SetDCNTL(BYTE by){
    this->number = (this->number & 0xFF00) | by;
}

BYTE DMA::GetDCNTL(){
    return this->number & 0xFF;
}

void DMA::SetDCNTH(BYTE by){
    this->number = (this->number & 0x00FF) | (((unsigned short)by) << 8);
    this->Execute();
}

BYTE DMA::GetDCNTH(){
    return (this->number & 0xFF00) >> 8;
}

void DMA::Execute(){
    unsigned short old_DRR = this->cpu->GetDRR();
    BYTE DMOD_value = this->cpu->mmu->ReadByte(DMOD);
    BYTE source_mode = DMOD_value & 0b000011;
    BYTE dest_mode = (DMOD_value & 0b001100) >> 2;
    BYTE func = (DMOD_value & 0b110000) >> 4;

    //printf("PC:%04X %02X DRR:%04X\n", this->cpu->PC, dest_mode, this->cpu->GetDRR());

    unsigned short old_sourcePtr = this->sourcePtr;
    unsigned short old_destPtr = this->destPtr;

    for (int i = 0; i<(this->number+1); i++){
        this->cpu->mmu->StoreShort(DRR, this->sourceBank);

        BYTE source_byte = this->cpu->mmu->ReadByte(this->sourcePtr);

        this->cpu->mmu->StoreShort(DRR, this->destBank);

        BYTE dest_byte = this->cpu->mmu->ReadByte(this->destPtr);

        BYTE new_byte;
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
