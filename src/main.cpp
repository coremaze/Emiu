#define SAVE_FLASH

#include "main.h"
#include "Display.h"
#include "CPU.h"
#include "MMU.h"
#include "DMA.h"
#include "Timer.h"
#include "Flash.h"
#include <fstream>
#include "types.h"

void HexDump(unsigned char* bin, unsigned int start, unsigned int end) {
    for (unsigned int i = start; i<end; i++) {
        printf("%02X ", (unsigned int)bin[i]);
    }
}

void DumpData(CPU* cpu){
    u8 data[0x10000] = {0};
    for (int i = 0; i<0x10000; i++){
        data[i] = cpu->mmu->ReadByte(i);
    }
    std::ofstream file("dump.dat", std::ios::out | std::ios::binary);
    file.write((const char*)data, 0x10000);
}


int main(int argc, char *argv[])
{
    #ifdef SAVE_FLASH
    printf("Controls:\n"
           "D-PAD: Arrow keys\n"
           "MENU: Right CTRL\n"
           "POWER: P\n"
           "ACTION: A\n"
           "Screen: Home, Page Up, End, Page Down\n");
    printf("\nNote:\n"
           "1) To take a device out of DEMO MODE, press DOWN and MENU while booting.\n"
           "2) To ensure that flash saves, press POWER before shutting down Emiu.\n\n");
    CPU* cpu = new CPU("OTP.dat", "Flash.dat");
    #else
    CPU* cpu = new CPU("OTP.dat", "Spike 1.09.03.dat");
    #endif


    if (cpu->error){
        printf("Emiu was not able to start.\n");
        Timer::USleep(1000);
        return 1;
    }

    cpu->display->Splash();

    cpu->display->Update();
    Timer::USleep(1000); //give time to hold keys if desired

    bool stepping = false;
    bool stop = false;
    unsigned int loop_size = 100;

    while (!cpu->display->Update() && !stop){
        cpu->StartWaitTimer();
        for (int i = 0; i<loop_size; i++){
            //if (((cpu->GetPRR()==0x23B) && cpu->PC==0x5456) || stepping) {
                //cpu->PrintState();
                //printf("%02X\n", cpu->mmu->ReadByte(0x925));
                //stepping = true;
                //std::cin.get();
                //DumpData(cpu);
                //cpu->mmu->StoreByte(0x884, 0);
            //}
            if (cpu->Step()){
                stop = true;
                break;
            }
        }

        //printf("loop_size: %u\n", loop_size);
        //fflush(stdout);
        loop_size = cpu->Wait(loop_size);
    }

    printf("End state: ");
    cpu->PrintState();
    printf("IRR: %04X\n", cpu->GetIRR());

    #ifdef SAVE_FLASH
    printf("Saving flash.\n");
    cpu->flash->Save("Flash.dat");
    #endif // SAVE_FLASH

    Timer::USleep(1000);
    return 0;
}
