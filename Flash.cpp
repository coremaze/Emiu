#include "Flash.h"
#include <iostream>
Flash::Flash(){
    this->mode = READ_MODE;
    this->current_command = 0;
    this->write_cycle = 1;
    this->last_operation_address = 0xFFFF;
}
unsigned int Flash::GetExpectedWriteAddress(){
    unsigned int addresses[] = {0xAAA, //1
                                  0x555, //2
                                  0xAAA, //3
                                  0xAAA, //4
                                  0x555, //5
                                  0xAAA};//6
    return addresses[this->write_cycle-1];
}
void Flash::Write(unsigned int address, BYTE by){
    this->mode = COMMAND_MODE;
    unsigned short expected_address = this->GetExpectedWriteAddress();
    this->current_command <<= 8;
    this->current_command |= by;
    if (expected_address != address){
        //Could be end of command, or could be invalid.
        switch (this->write_cycle){
            case 1: //1st bus write cycle
                if (this->current_command == 0xB0){
                    //Erase-Suspend command finished
                    //printf("Erase-Suspend not implemented.\n");
                    this->ResetWriteCycle();
                    return;
                }
                else if (this->current_command == 0x30){
                    //Erase-Resume command finished
                    //printf("Erase-Resume not implemented.\n");
                    this->ResetWriteCycle();
                    return;
                }
                else if (this->current_command == 0xF0){
                    //Software ID Exit command finished
                    this->mode = READ_MODE;
                    this->ResetWriteCycle();
                    return;
                }
                else {
                    //Invalid command
                    this->mode = READ_MODE;
                    this->ResetWriteCycle();
                    return;
                }
                break;

            case 2: //2nd bus write cycle
                //Invalid command. There are no commands that end here, there's no reason for the address to be unexpected
                this->mode = READ_MODE;
                this->ResetWriteCycle();
                return;
                break;

            case 3: //3rd bus write cycle
                //Invalid command. There are commands that end here, but all of them get sent to the expected address.
                this->mode = READ_MODE;
                this->ResetWriteCycle();
                return;
                break;

            case 4: //4th bus write cycle
                if ((this->current_command>>8) == 0xAA55A0) { //Byte-Program command finished
                    this->ByteProgram(address, by);
                    this->ResetWriteCycle();
                    return;
                }
                else if ((this->current_command>>8) == 0xAA55A5) { //User Security ID Byte-Program command finished
                    //Not implemented
                    printf("User Security ID Byte-Program not implemented.\n");
                    this->ResetWriteCycle();
                    return;
                }
                else if (((this->current_command>>8) == 0xAA5585) && (by == 0x00)) { //User Security ID Program Lock-Out command finished
                    //Not implemented
                    printf("User Security ID Program Lock-Out not implemented.\n");
                    this->ResetWriteCycle();
                    return;
                }
                else {
                    //Invalid command
                    this->mode = READ_MODE;
                    this->ResetWriteCycle();
                    return;
                }
                break;

            case 5: //5th bus write cycle
                //Invalid command. No commands end here
                this->mode = READ_MODE;
                this->ResetWriteCycle();
                return;
                break;

            case 6: //6th bus write cycle
                if ((this->current_command>>8) == 0xAA5580AA55) { //Sector-Erase and Block-Erase

                    if (this->current_command == 0xAA5580AA5550) { //Sector-Erase
                        this->SectorErase(address);
                        this->ResetWriteCycle();
                        return;
                    }
                    else if (this->current_command == 0xAA5580AA5530) { //Block-Erase
                        this->BlockErase(address);
                        this->ResetWriteCycle();
                        return;
                    }
                    else {
                        //Invalid
                        this->mode = READ_MODE;
                        this->ResetWriteCycle();
                        return;
                    }
                }
                else {

                    return;
                }
                break;

            default:
                break;
        }
    }

    else {
        //Incomplete command
        switch (this->write_cycle){
            case 1: //1st bus write cycle
                if (this->current_command == 0xAA){ //Pretty much all commands
                    //valid
                    this->write_cycle += 1;
                    return;
                }
                else {
                    //Invalid command
                    this->mode = READ_MODE;
                    this->ResetWriteCycle();
                    return;
                }
                break;

            case 2: //2nd bus write cycle
                if (this->current_command == 0xAA55){ //All commands
                    //valid
                    this->write_cycle += 1;
                    return;
                }
                else {
                    //Invalid command
                    this->mode = READ_MODE;
                    this->ResetWriteCycle();
                    return;
                }
                break;

            case 3: //3rd bus write cycle
                if (this->current_command == 0xAA55A0){ //Byte-Program
                    //valid
                    this->write_cycle += 1;
                    return;
                }
                else if (this->current_command == 0xAA5580){ //Sector-Erase, Block-Erase, or Chip-Erase
                    //valid
                    this->write_cycle += 1;
                    return;
                }
                else if (this->current_command == 0xAA5588){ //Query Sec ID command finished
                    //Not implemented
                    printf("Query Sec ID not implemented.\n");
                    this->ResetWriteCycle();
                    return;
                }
                else if (this->current_command == 0xAA55A5){ //User Security ID Byte-Program
                    //valid
                    this->write_cycle += 1;
                    return;
                }
                else if (this->current_command == 0xAA5585){ //User Security ID Program Lock-Out
                    //valid
                    this->write_cycle += 1;
                    return;
                }
                else if (this->current_command == 0xAA5590){ //Software ID Entry command finished
                    //Not implemented
                    printf("Software ID Entry not implemented.\n");
                    this->ResetWriteCycle();
                    return;
                }
                else if (this->current_command == 0xAA5598){ //CFI Query Entry command finished
                    //Not implemented
                    printf("CFI Query Entry not implemented.\n");
                    this->ResetWriteCycle();
                    return;
                }
                else if (this->current_command == 0xAA55F0){ //Software ID Exit command finished
                    this->mode = READ_MODE;
                    this->ResetWriteCycle();
                    return;
                }
                break;

            case 4: //4th bus write cycle
                if (this->current_command == 0xAA5580AA){ //All erases
                    //valid
                    this->write_cycle += 1;
                    return;
                }
                else {
                    //Invalid command
                    this->mode = READ_MODE;
                    this->ResetWriteCycle();
                    return;
                }
                break;

            case 5: //5th bus write cycle
                if (this->current_command == 0xAA5580AA55) { //All erases
                    //valid
                    this->write_cycle += 1;
                    return;
                }
                else {
                    //Invalid command
                    this->mode = READ_MODE;
                    this->ResetWriteCycle();
                    return;
                }
                break;

            case 6: //6th bus write cycle
                if (this->current_command == 0xAA5580AA5510) { //Chip-Erase
                    this->ChipErase();
                    this->ResetWriteCycle();
                    return;
                }
                else {
                    //Invalid command
                    this->mode = READ_MODE;
                    this->ResetWriteCycle();
                    return;
                }
                break;

            default:
                break;
        }
    }
}
BYTE Flash::Read(unsigned int address){
    if (this->mode == READ_MODE){
        return this->memory[address];
    }
    else {
        if (address != last_operation_address) {
            this->ResetWriteCycle();
            this->mode = READ_MODE;
            return this->memory[address];
        }
        else {
            return this->GetStatusRegister();
        }
    }
}
void Flash::ResetWriteCycle(){
    this->write_cycle = 1;
    this->current_command = 0;
}

BYTE Flash::GetStatusRegister(){
    //DQ6 would normally alternate while program or erase is in progress, but the emulator does not do this asynchronously
    //DQ7 would normally be 0 while operation is in progress, but the emulator does not do this asynchronously
    return 0b11000000;

}

void Flash::SectorErase(unsigned int address){
    this->mode = SECTOR_ERASE_MODE;
    this->last_operation_address = address;
    printf("A sector erase has been issued.\n");
    for (unsigned int i = 0; i < SECTOR_SIZE; i++){
        this->memory[address + i] = 0xFF;
    }

}
void Flash::BlockErase(unsigned int address){
    printf("A block erase has been issued.\n");
    this->mode = BLOCK_ERASE_MODE;
    this->last_operation_address = address;
    for (unsigned int i = 0; i < BLOCK_SIZE; i++){
        this->memory[address + i] = 0xFF;
    }
}
void Flash::ChipErase(){
    printf("A chip erase has been issued.\n");
    this->mode = BLOCK_ERASE_MODE;
    for (unsigned int i = 0; i < FLASH_SIZE; i++){
        this->memory[i] = 0xFF;
    }
}
void Flash::ByteProgram(unsigned int address, BYTE by){
    //printf("A byte program has been issued.\n");
    this->mode = BYTE_PROGRAM_MODE;
    this->last_operation_address = address;
    this->memory[address] = by;
}
