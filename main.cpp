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
    //cpu->mmu->StoreShort(0x32, 0x1234);

    cpu->PrintState();

    while (!cpu->Step()){
        //cpu->PrintState();
        cpu->display->Update();

    }
    cpu->PrintState();

    //Display* display = new Display();
    //display->SendCommand(0x5C);
    //display->SendData(0xFF);
    //display->SendData(0xFF);
    //while (1){
    //    if (display->Update()){
    //        break;
    //    }
    //}
    Sleep(1000);
    return 0;
}
