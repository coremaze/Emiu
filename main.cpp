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
        cpu->mmu->StoreShort(0, 0xFFFF);
        //cpu->PrintState();
        if (stepping){
                cpu->PrintState();
                std::cin.get();
        }


        /*if (cpu->PC == 0x5A41){
                stepping = true;
                cpu->PrintState();
                printf("%04X ", (unsigned int)cpu->mmu->ReadShort(0xAC));
        }*/

    }
    //cpu->PrintState();

    return 0;
}
