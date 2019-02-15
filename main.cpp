#include <windows.h>
#include "main.h"
#include "Display.h"
#include "CPU.h"
#include "MMU.h"
#include "DMA.h"
#include "Timer.h"
#include <fstream>

void HexDump(unsigned char* bin, unsigned int start, unsigned int end) {
    for (unsigned int i = start; i<end; i++) {
        printf("%02X ", (unsigned int)bin[i]);
    }
}

void DumpData(CPU* cpu){
    BYTE data[0x10000] = {0};
    for (int i = 0; i<0x10000; i++){
        data[i] = cpu->mmu->ReadByte(i);
    }
    std::ofstream file("dump.dat", std::ios::out | std::ios::binary);
    file.write((const char*)data, 0x10000);
}


int main(int argc, char *argv[])
{
    CPU* cpu = new CPU("OTP.dat", "Spike 1.09.03.dat");

    if (cpu->error){
        printf("Emiu was not able to start.\n");
        Sleep(1000);
        return 1;
    }

    cpu->display->Splash();

    cpu->display->Update();
    Sleep(1000); //give time to hold keys if desired

    bool stepping = false;
    bool stop = false;
    unsigned int loop_size = 100;

    while (!cpu->display->Update() && !stop){
        cpu->StartWaitTimer();
        for (int i = 0; i<loop_size; i++){
            //if (((cpu->GetPRR()==0x207) && cpu->PC==0x6718) || stepping) {
                //cpu->PrintState();
                //printf("%02X\n", cpu->mmu->ReadByte(0x805));
                //stepping = true;
                //std::cin.get();
                //DumpData(cpu);
                //cpu->mmu->StoreByte(0x884, 0);
            //}
            //cpu->PrintState();
            if (cpu->Step()){
                stop = true;
                break;
            }


        }
        cpu->Wait(loop_size);
    }



    //while (!cpu->Step() && !cpu->display->Update()){
        //cpu->PrintState();


        /*if (((cpu->GetPRR()&0xFFFF) == 0x207) && cpu->PC == 0x63FD ){
            //stepping = true;
            DumpData(cpu);
        }*/


        //if (stepping && !cpu->interrupted){
            //cpu->PrintState();
            //std::cin.get();
            //stepping = false;
        //}

   // }

    printf("End state: ");
    cpu->PrintState();
    printf("IRR: %04X\n", cpu->GetIRR());
    Sleep(1000);
    return 0;
}
