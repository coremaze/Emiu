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
    CPU* cpu = new CPU("OTP.dat", "Cloe 1.04.03.dat");

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
        //if (cpu->PC == 0x62F5){
        //    //stepping = true;
        //    printf("%04x\n", cpu->mmu->ReadShort(0xCA));
        //    //printf("%02X ", (unsigned int)cpu->mmu->ReadByte(0xC3));
        //}

        if (((cpu->GetPRR()&0xFFFF) == 0x207) && cpu->PC == 0x6255  ){
            //cpu->mmu->StoreByte(0x885, cpu->mmu->ReadByte(0x885)+1);
            //cpu->A = cpu->mmu->ReadByte(0x885);
            //cpu->PC = 0x6199;
                //cpu->mmu->StoreByte(0x820, 0);
            //cpu->PC=0x64A5 ;
            //cpu->PrintState();
            //cpu->A -= 0x10;
            //printf("height: %02X line index: %02X ptr: %04X\n", cpu->mmu->ReadByte(0x81F), cpu->mmu->ReadByte(0x885), cpu->mmu->ReadShort(0xBF));


            //cpu->A = 10;
            //if (cpu->mmu->ReadShort(0xBF) > 0xF000){
            //    cpu->PC = 0x6258;
            //}
            //printf("%04X DRR:%04X\n", cpu->mmu->ReadShort(0xCA), cpu->GetDRR());
            //stepping = true;
            //cpu->PC = 0x618E;
            //cpu->mmu->StoreByte(0x81D, 3);
            //cpu->A = 67;

            //unsigned short addr1 = cpu->PopShort();
            //unsigned short addr2 = cpu->PopShort();
            //unsigned short addr3 = cpu->PopShort();
            //cpu->PushShort(addr3);
            //cpu->PushShort(addr2);
            //cpu->PushShort(addr1);
            //printf("%04X\n", addr3);
            //stepping = true;
        }

        //if (cpu->PC == 0x634B){
        //    cpu->PC = 0x635A;
        //    cpu->Y = 0;
        //    cpu->mmu->StoreShort(0xBF, 0xC000);
        //}
        if (stepping && !cpu->interrupted){
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
