class MMU():
    def __init__(self, cpu):
        self.CPU = cpu

    def StoreMemory(self, address, byte):
        BRR_START = 0x2000
        BRR_LENGTH = 0x2000
        PRR_START = 0x4000
        PRR_LENGTH = 0x4000
        DRR_START = 0x8000
        DRR_LENGTH = 0x8000
        REGISTERS_START = 0x0
        REGISTERS_LENGTH = 0x80

        BTREQ = 0x2B

        
        #Implement DMA to do this properly
        if address >= 0xC000:
            position = (address - 0xC000) // 2
            self.CPU.display.pixelPosition = position
            if address % 2 == 0:
                self.CPU.display.RByte = 255-byte
            else:
                self.CPU.display.GBByte = 255-byte
                self.CPU.display.DrawCurrentPixel()
##                print('drawing')

                

        if REGISTERS_START <= address < (REGISTERS_START + REGISTERS_LENGTH):
            if address == BTREQ:
                byte = ~byte
                self.CPU.memRegisters[address] &= byte
            else:
                self.CPU.memRegisters[address] = byte

        elif BRR_START <= address < (BRR_START + BRR_LENGTH): #BRR
            brr = self.CPU.GetBRR()
            page = brr & 0x7F #Select page for OTP or flash
            paged_location = (address - BRR_START) + BRR_LENGTH*page #Calculate location for OTP or flash
            if (brr & 0b1001111000000000) == 0b1111000000000 or brr == 0: #OTP
                self.CPU.OTP[paged_location % len(self.CPU.OTP)] = byte
            elif brr & 0b1001111100000000 == 0b1100000000: #video registers
                self.CPU.WriteVideoRegister(address, byte)
            elif brr & 0b1001110000000000 == 0b10000000000: #Flash
                self.CPU.Flash[paged_location % len(self.CPU.Flash)] = byte
            elif brr & 0x8000 == 0x8000: #Internal RAM 0x2000~0x3FFF
                self.CPU.RAM[address] = byte #Fall through
            else:
                raise Exception('Invalid BRR mask')
            
        elif PRR_START <= address < (PRR_START + PRR_LENGTH):
            prr = self.CPU.GetPRR()
            page = prr & 0x7F #Select page for OTP or flash
            paged_location = (address - PRR_START) + PRR_LENGTH*page #Calculate location for OTP or flash
            if (prr & 0b1000111100000000) == 0b111100000000 or prr == 0: #OTP
                self.CPU.OTP[paged_location % len(self.CPU.OTP)] = byte
            elif prr & 0b1000111110000000 == 0b110000000: #video registers
                self.CPU.WriteVideoRegister(address, byte)
            elif prr & 0b1000111000000000 == 0b1000000000: #Flash
                self.CPU.Flash[paged_location % len(self.CPU.Flash)] = byte
            elif prr & 0x8000 == 0x8000: #Internal RAM 0x4000~0x7FFF
                self.CPU.RAM[address] = byte #Fall through
            else:
                raise Exception('Invalid PRR mask')

        elif DRR_START <= address < (DRR_START + DRR_LENGTH):
            drr = self.CPU.GetDRR()
            #drr = 0x78CD #DEBUG
            page = drr & 0x7F #Select page for OTP or flash
            paged_location = (address - DRR_START) + DRR_LENGTH*page #Calculate location for OTP or flash
            if (drr & 0b1000111100000000) == 0b111100000000 or drr == 0: #OTP
                self.CPU.OTP[paged_location % len(self.CPU.OTP)] = byte
            elif drr & 0b1000011111000000 == 0b11000000: #video registers
                self.CPU.WriteVideoRegister(address, byte)
            elif drr & 0b1000011100000000 == 0b100000000: #Flash
                self.CPU.Flash[paged_location % len(self.CPU.Flash)] = byte
            elif drr & 0x8000 == 0x8000:
                self.CPU.RAM[address-DRR_START] = byte
            else:
                raise Exception('Invalid DRR mask')
        else:
            self.CPU.RAM[address] = byte

    def ReadMemory(self, address, length=1):
        BRR_START = 0x2000
        BRR_LENGTH = 0x2000
        PRR_START = 0x4000
        PRR_LENGTH = 0x4000
        DRR_START = 0x8000
        DRR_LENGTH = 0x8000
        REGISTERS_START = 0x0
        REGISTERS_LENGTH = 0x80
        if REGISTERS_START <= address < (REGISTERS_START + REGISTERS_LENGTH):
            byte = self.CPU.memRegisters[address]
            
        elif BRR_START <= address < (BRR_START + BRR_LENGTH): #BRR
            brr = self.CPU.GetBRR()
            page = brr & 0x7F #Select page for OTP or flash
            paged_location = (address - BRR_START) + BRR_LENGTH*page #Calculate location for OTP or flash
            if (brr & 0b1001111000000000) == 0b1111000000000 or brr == 0: #OTP
                byte = self.CPU.OTP[paged_location % len(self.CPU.OTP)]
            elif brr & 0b1001111100000000 == 0b1100000000: #video registers
                byte = self.CPU.videoRegisters[address % 2]
            elif brr & 0b1001110000000000 == 0b10000000000: #Flash
                byte = self.CPU.Flash[paged_location % len(self.CPU.Flash)]
            elif brr & 0x8000 == 0x8000: #Internal RAM 0x2000~0x3FFF
                byte = self.CPU.RAM[address] #Fall through
            else:
                raise Exception('Invalid BRR mask')
            
        elif PRR_START <= address < (PRR_START + PRR_LENGTH):
            prr = self.CPU.GetPRR()
            page = prr & 0x7F #Select page for OTP or flash
            paged_location = (address - PRR_START) + PRR_LENGTH*page #Calculate location for OTP or flash
            if (prr & 0b1000111100000000) == 0b111100000000 or prr == 0: #OTP
                byte = self.CPU.OTP[paged_location % len(self.CPU.OTP)]
            elif prr & 0b1000111110000000 == 0b110000000: #video registers
                byte = self.CPU.videoRegisters[address % 2]
            elif prr & 0b1000111000000000 == 0b1000000000: #Flash
                byte = self.CPU.Flash[paged_location % len(self.CPU.Flash)]
            elif prr & 0x8000 == 0x8000: #Internal RAM 0x4000~0x7FFF
                byte = self.CPU.RAM[address] #Fall through
            else:
                raise Exception('Invalid PRR mask')

        elif DRR_START <= address < (DRR_START + DRR_LENGTH):
            drr = self.CPU.GetDRR()
            page = drr & 0x7F #Select page for OTP or flash
            paged_location = (address - DRR_START) + DRR_LENGTH*page #Calculate location for OTP or flash
            if (drr & 0b1000111100000000) == 0b111100000000 or drr == 0: #OTP
                byte = self.CPU.OTP[paged_location % len(self.CPU.OTP)]
            elif drr & 0b1000011111000000 == 0b11000000: #video registers
                byte = self.CPU.videoRegisters[address % 2]
            elif drr & 0b1000011100000000 == 0b100000000: #Flash
                byte = self.CPU.Flash[paged_location % len(self.CPU.Flash)]
            elif drr & 0x8000 == 0x8000:
                byte = self.CPU.RAM[address-DRR_START]
            else:
                raise Exception(hex(self.CPU.PC) + 'Invalid DRR mask' + hex(drr))
        else:
            byte = self.CPU.RAM[address]

        if length > 1:
            return bytes([byte]) + bytes([self.ReadMemory(address + 1, length - 1)])
        else:
            return byte
