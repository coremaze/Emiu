from CPU import CPU

class Emulator():
    def __init__(self, OTPFile, flashFile):
        self.CPU = CPU(OTPFile, flashFile)
        self.breakpoints = []
    def SetBreakpoint(self, address):
        self.breakpoints.append(address)
    def RemoveBreakpoint(self, address):
        if address in self.breakpoints:
            self.breakpoints.remove(address)
    
        
    def Run(self):
        while True:
            self.CPU.Step()

            if self.CPU.PC in self.breakpoints:
                print(f'Hit breakpont {self.CPU.PC:02X} Opcode {self.CPU.MMU.ReadMemory(self.CPU.PC):X}')
                #print(hex(self.CPU.MMU.ReadMemory(0xFA)))
                #print(f'{self.CPU.GetBTVector():X}')
                self.CPU.PrintState()
                input()

if __name__ == '__main__':
    emulator = Emulator('OTP.dat', 'Cloe 1.02.dat')
    #emulator.SetBreakpoint(0x423E)
    emulator.Run()
