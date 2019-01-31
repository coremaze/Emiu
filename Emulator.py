from CPU import CPU
import pygame
pygame.init()
class Emulator():
    def __init__(self, OTPFile, flashFile):
        self.CPU = CPU(OTPFile, flashFile)
        self.breakpoints = []
    def SetBreakpoint(self, address):
        self.breakpoints.append(address)
    def RemoveBreakpoint(self, address):
        if address in self.breakpoints:
            self.breakpoints.remove(address)
    def UpdateControls(self):
        controls = 0b11111111
        UP =    0b11111110
        DOWN =  0b11111101
        LEFT =  0b11111011
        RIGHT = 0b11110111
        POWER = 0b11101111
        MENU =  0b11011111

        keys=pygame.key.get_pressed()
        if keys[pygame.K_RIGHT]:
            controls &= RIGHT
        if keys[pygame.K_LEFT]:
            controls &= LEFT
        if keys[pygame.K_DOWN]:
            controls &= DOWN
        if keys[pygame.K_UP]:
            controls &= UP
        if keys[pygame.K_RCTRL]:
            controls &= MENU
        if keys[pygame.K_p]:
            controls &= POWER


        self.CPU.MMU.StoreMemory(0, controls)
        self.CPU.MMU.StoreMemory(1, 0xFF)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                exit()
        
    def Run(self):
        while True:
            self.UpdateControls()
            self.CPU.Step()

            if self.CPU.PC == 0x5972 and self.CPU.GetPRR() == 0: #bypass checks
                self.CPU.PC = 0x597F
            
            if self.CPU.PC in self.breakpoints:
                print(f'Hit breakpont {self.CPU.PC:02X}')
                print(hex(self.CPU.MMU.ReadMemory(0xFA)))
                self.CPU.PrintState()
                input()

                    
if __name__ == '__main__':
    emulator = Emulator('OTP.dat', 'nova_original.dat')
    #emulator.SetBreakpoint(0x4000)
    emulator.Run()
