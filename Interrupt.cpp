#include "Interrupt.h"
#include <windows.h>
#include "CPU.h"
#include <iostream>


BTInterrupt::BTInterrupt(CPU* cpu){
    this->cpu = cpu;
    this->clock = 0;
    this->last_time = timeGetTime();
}
bool BTInterrupt::Update(){
    unsigned int this_time = timeGetTime();
    unsigned int duration = (this_time - this->last_time);
    this->last_time = this_time;
    unsigned int cycles = (duration * 8192) / 1000; //The number of crystal cycles that should have elapsed: 8192Hz
    unsigned long long int old_clock = this->clock;
    this->clock += cycles;

    BYTE BTEN_data = this->cpu->memRegisters[BTEN]; //bits are set to 1 for each clock that is enabled
    BYTE BTREQ_data = this->cpu->memRegisters[BTREQ];

    //Literally simulating an oscillating crystal
    //is not going to happen, so this checks if
    //an interrupt should have been triggered between
    //this update and last update.

    if ( (old_clock / 4096) < (this->clock / 4096) ){ //2Hz
        BTREQ_data |= 0b1;
    }
    if ( (old_clock / 256) < (this->clock / 256) ){ //32Hz
        BTREQ_data |= 0b10;
    }
    if ( (old_clock / 128) < (this->clock / 128) ){ //64Hz
        BTREQ_data |= 0b100;
    }
    if ( (old_clock / 64) < (this->clock / 64) ){ //128Hz
        BTREQ_data |= 0b1000;
    }
    if ( (old_clock / 32) < (this->clock / 32) ){ //256Hz
        BTREQ_data |= 0b10000;
    }
    if ( (old_clock / 16) < (this->clock / 16) ){ //512Hz
        BTREQ_data |= 0b100000;
    }
    if ( (old_clock / 4) < (this->clock / 4) ){ //2048Hz
        BTREQ_data |= 0b1000000;
    }
    if ( (old_clock) < (this->clock) ){ //8192Hz
        BTREQ_data |= 0b10000000;
    }

    BTREQ_data &= BTEN_data;
    this->cpu->memRegisters[BTREQ] = BTREQ_data;

    if (BTREQ_data){
        return true;
    }
    return false;
}

PTInterrupt::PTInterrupt(CPU* cpu){
    this->cpu = cpu;
}
void PTInterrupt::Trigger(){
    this->triggered = true;
}
bool PTInterrupt::Update(){
    BYTE IREQL_data = this->cpu->memRegisters[IREQL];
    if (this->triggered){
        IREQL_data |= 0b00100000;
    }
    else {
        IREQL_data &= 0b11011111;
    }

    if (this->triggered){
        this->triggered = false;
        BYTE IENAL_data = this->cpu->memRegisters[IENAL];
        BYTE IEPT = IENAL_data & 0b00100000;
        if (IEPT){
            return true;
        }
        else {
            return false;
        }
    }

    return false;
}
