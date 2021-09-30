#include "Interrupt.h"
#include "CPU.h"
#include <iostream>
#include "Timer.h"
#include "types.h"


BTInterrupt::BTInterrupt(CPU* cpu){
    this->cpu = cpu;
    this->clock = 0;
    this->timer = new Timer();
    this->timer->Start();
    for (int i = 0; i<8; i++) this->BTREQ_data_buffer[i] = 0;
}

bool BTInterrupt::Update(){
    this->timer->Stop();
    float duration = this->timer->Duration();
    unsigned int cycles = (duration* 8192.0)/1000.0 ; //The number of crystal cycles that should have elapsed: 8192Hz
    if (!cycles) return false;
    this->timer->Advance();
    unsigned long long int old_clock = this->clock;
    this->clock += cycles;

    u8 BTEN_data = this->cpu->memRegisters[BTEN]; //bits are set to 1 for each clock that is enabled
    u8 BTREQ_data = this->cpu->memRegisters[BTREQ];

    //Literally simulating an oscillating crystal
    //is not going to happen, so this checks if
    //an interrupt should have been triggered between
    //this update and last update.

    this->BTREQ_data_buffer[0] += this->clock / 4096 - old_clock / 4096; // 2Hz
    this->BTREQ_data_buffer[1] += this->clock /  256 - old_clock /  256; // 32Hz
    this->BTREQ_data_buffer[2] += this->clock /  128 - old_clock /  128; // 64Hz
    this->BTREQ_data_buffer[3] += this->clock /  64 - old_clock  /  64;  // 128Hz
    this->BTREQ_data_buffer[4] += this->clock /  32 - old_clock  /  32;  // 256Hz
    this->BTREQ_data_buffer[5] += this->clock /  16 - old_clock  /  16;  // 512Hz
    this->BTREQ_data_buffer[6] += this->clock /   4 - old_clock  /   4;  // 2048Hz
    this->BTREQ_data_buffer[7] += this->clock       - old_clock;         // 8192Hz

    for (int i = 0; i<8; i++) {
        if (this->BTREQ_data_buffer[i]) {
            this->BTREQ_data_buffer[i]--;
            BTREQ_data |= 1<<i;
        }
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
    u8 IREQL_data = this->cpu->memRegisters[IREQL];
    if (this->triggered){
        IREQL_data |= 0b00100000;
    }
    else {
        IREQL_data &= 0b11011111;
    }

    if (this->triggered){
        this->triggered = false;
        u8 IENAL_data = this->cpu->memRegisters[IENAL];
        u8 IEPT = IENAL_data & 0b00100000;
        if (IEPT){
            return true;
        }
        else {
            return false;
        }
    }

    return false;
}
