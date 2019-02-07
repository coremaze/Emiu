#include <windows.h>
#include "main.h"
#include "Display.h"
#include "CPU.h"
#include "MMU.h"
#include "DMA.h"

void HexDump(unsigned char* bin, unsigned int start, unsigned int end) {
    for (unsigned int i = start; i<end; i++) {
        printf("%02X ", (unsigned int)bin[i]);
    }
}


int main(int argc, char *argv[])
{
    CPU* cpu = new CPU("OTP.dat", "nova_original.dat");

    cpu->display->Update();
    Sleep(1000); //give time to hold keys if desired

    bool stepping = false;
    while (!cpu->Step() && !cpu->display->Update()){
        //cpu->PrintState();

        //if (cpu->PC == 0x70D8){
        //    cpu->PC = 0x70DB;
        //}

        //if (cpu->PC == 0x436D ){
        //    cpu->PC = 0x440E;
        //}

        //if (cpu->PC == 0x436D ){
        //    cpu->PC = 0x4668;
        //}

        //cpu->PrintState()  ;
        //if (cpu->PC == 0x4FF9){
        //    stepping = true;
        //    printf("%02x\n", cpu->mmu->ReadByte(0xC75E));
        //    //printf("%02X ", (unsigned int)cpu->mmu->ReadByte(0xC3));
        //}
        if (stepping){
            cpu->PrintState();
            std::cin.get();
            //stepping = false;
        }

    }

    //cpu->PrintState();
    Sleep(1000);
    return 0;
}
