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
        UP = 0b11111110
        DOWN = 0b11111101
        LEFT = 0b11111011
        RIGHT = 0b11110111
        MENU = 0b11011111

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

            #Force a known good configuration when using original OTP, to get it into Flash.
            if self.CPU.PC == 0x5B8A:
                self.CPU.MMU.StoreMemory(0x2F0, 0x00) #Set up destination addr to be 0x4000
                self.CPU.MMU.StoreMemory(0x2F1, 0x40)
                self.CPU.MMU.StoreMemory(0x34, 0xCD) #Set DRR to 0x78CD
                self.CPU.MMU.StoreMemory(0x35, 0x78)
                self.CPU.Push(0x02) #Provide PRRChange with PRR 0x7202
                self.CPU.Push(0x72)
                self.CPU.display.pixelPosition = 0 #Reset display
                self.CPU.PC = 0x2E7 #Jump to PRRChange in RAM. The Original OTP should create this.
    
            if self.CPU.PC in self.breakpoints:
                print(f'Hit breakpont {self.CPU.PC:02X}')
                self.CPU.PrintState()
                input()
if __name__ == '__main__':
    emulator = Emulator('OTP.dat', 'nova_original.dat')
    #emulator.SetBreakpoint(0x2E7)
    emulator.Run()
