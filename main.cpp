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
    CPU* cpu = new CPU("OTP.dat", "Dash 1.02.dat");

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


        //if (cpu->PC == 0x7427){
        //    printf("a\n");
        //}

        //cpu->PrintState()  ;
        //if (cpu->PC == 0x4FF9){
        //    stepping = true;
        //    printf("%02x\n", cpu->mmu->ReadByte(0xC75E));
        //    //printf("%02X ", (unsigned int)cpu->mmu->ReadByte(0xC3));
        //}

        //if ((cpu->GetPRR()&0xFFFF) == 0 && cpu->PC == 0x5A54 ){
        //    cpu->PrintState();
        //    //stepping = true;
        //}
        if (stepping){
            cpu->PrintState();
            std::cin.get();
            //stepping = false;
        }

    }

    printf("End state: ");
    cpu->PrintState();
    printf("IRR: %04X\n", cpu->GetIRR());
    Sleep(1000);
    return 0;
}
