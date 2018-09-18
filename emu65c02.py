import struct
import pygame
import time
pygame.init()

COMMANDS = {
    0xAF : ('Display ON (DISON)', ''),
    0xAE : ('Display OFF (DISOFF)', ''),
    0xA6 : ('Normal display (DISNOR)', ''),
    0xA7 : ('Inverse display (DISINV)', ''),
    0xBB : ('Common scan (COMSCAN)', ''),
    0xCA : ('Display control (DISCTL)', ''),
    0x04 : ('Sleep In/Out Preparation (SLPP)', ''),
    0x95 : ('Sleep In (SLPIN)', ''),
    0x94 : ('Sleep Out (SLPOUT)', ''),
    0x75 : ('Page address set (PASET)', ''),
    0x15 : ('Column Address Set (CASET)', ''),
    0xBC : ('Data Scan Direction (DATCTL)', ''),
    0x5C : ('Memory write (RAMWR)', ''),
    0xA8 : ('Partial display in (PLTIN)', ''),
    0xA9 : ('Partial display out (PTLOUT)', ''),
    0xE0 : ('Read modify write in (RMWIN)', ''),
    0xEE : ('Read modify write out (RMWOUT)', ''),
    0xAA : ('Area scroll set (ASCSET)', ''),
    0xAB : ('Scroll start address set (SCSTART)', ''),
    0xD1 : ('Internal OSC on (OSCON)', ''),
    0xD2 : ('Internal OSC off (oSCOFF)', ''),
    0x20 : ('Power Control (PWRCTL)', 'FRAME 1 PWM Set'),
    0x81 : ('Electronic volume control (VOLCTR)', ''),
    0xD6 : ('EC increase 1 (VOLUP)', ''),
    0xD7 : ('EC decrease 1 (VOLDOWN)', ''),
    0x7C : ('Read Register 1 (EPSRRD1)', ''),
    0x7D : ('Read Register 2 (EPSRRD2)', ''),
    0x25 : ('NOP Instruction', ''),
    0x07 : ('EEPROM Function Start (EEOK)', ''),
    0x82 : ('Not Use (RESERVED)', ''),
    0x30 : ('Ext = 0', 'Ext = 0'),
    0x31 : ('Ext = 1', 'Ext = 1'),
    0x23 : ('', 'Palette FRC4 Setup'),
    0x21 : ('', 'Palette FRC2 Setup'),
    0x22 : ('', 'Palette FRC3 Setup'),
    0x32 : ('', 'Analog set (ANASET)'),
    0xCD : ('', 'Control EEPROM'),
    0xCC : ('', 'Cancel EEPROM'),
    0xFC : ('', 'Write data to EEPROM (EPMWR)'),
    0xFD : ('', 'Read data from EEPROM (EPMWR)'),
    0xFA : ('', 'Display performance adjustment (DISPADJ)'),
    0xF4 : ('', 'Internal Initialize Preparation (IIPP)')
    }

def HexView(data):
    return ' '.join('%02x' % x for x in data)

class Display():
    def __init__(self):
        self.RedNext = True
        self.RByte = 0
        self.GBByte = 0
        self.pixelPosition = 0
        self.width = 98
        self.height = 67
        self.scale = 2
        
        self.command = 0
        self.drawStartX = 0
        self.drawStartY = 0

        self.maxPixelPosition = self.width*self.height
        self.screen = pygame.display.set_mode((self.width*self.scale, self.height*self.scale))
        pygame.display.set_caption("Miuchiz")
        
    def SendCommand(self, byte):
        self.command = byte
        if byte in COMMANDS:
            command_name = COMMANDS[byte]
        else:
            command_name = 'Unknown'
        print(f'Display command {hex(byte)} ({command_name})')

    def SendData(self, byte):
        if self.command == 0x5C: #Memory Write (RAMWR)
            self.AddPixelData(byte)
        else:
            print(f'Unused data for command {hex(self.command)}: {hex(byte)}')
            
    def AddPixelData(self, byte):
        if self.RedNext:
            self.RByte = byte
        else:
            self.GBByte = byte
            self.DrawCurrentPixel()
       
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                exit()
        self.RedNext = not self.RedNext

    def DrawCurrentPixel(self):
        x = self.pixelPosition % self.width
        y = (self.pixelPosition - x) // self.width
        
        r = self.RByte & 0x0F
        r *= 17
        g = ((self.GBByte & 0xF0) >> 4) * 17
        b = (self.GBByte & 0x0F) * 17

        pygame.draw.rect(self.screen, (r,g,b), (x*self.scale, y*self.scale, self.scale, self.scale))

        self.pixelPosition += 1
        
        if self.pixelPosition >= self.maxPixelPosition:
            self.pixelPosition = 0
            
        pygame.display.flip()
        

class CPU():
    STACK_START = 0x100
    PRR = 0x32 #4000~7FFF
    DRR = 0x34 #8000~FFFF
    BRR = 0x36 #2000~3FFF
    def __init__(self, OTPFile, flashFile):
        with open(OTPFile, 'rb') as f:
            self.OTP = bytearray(f.read())

        with open(flashFile, 'rb') as f:
            self.Flash = bytearray(f.read())

        

        self.c = False #Carry
        self.z = False #Zero
        self.i = False #Interrupt Disable
        self.d = False #Decimal Mode
        self.b = False #Break Command
        self.v = False #Overflow
        self.n = False #Negative

        self.A = 0 #Accumulator
        self.X = 0 #X
        self.Y = 0 #Y
        self.S = 0 #Stack
        self.PC = 0 #Program Counter

        self.Reset()


    def PrintState(self):
        print(f'A:{self.A} X:{self.X} Y:{self.Y} PC:{hex(self.PC)}')

    def GetPRR(self):
        return struct.unpack('<H', self.ReadMemory(self.PRR, length=2))[0]
    def GetDRR(self):
        return struct.unpack('<H', self.ReadMemory(self.DRR, length=2))[0]
    def GetBRR(self):
        return struct.unpack('<H', self.ReadMemory(self.BRR, length=2))[0]

    def GetResetVector(self):
        return struct.unpack('<H', self.OTP[0x7FFC : 0x7FFE])[0]
    def Reset(self):
        self.i = True
        self.d = False
        self.PC = self.GetResetVector()

        self.memRegisters = bytearray(0x80)
        self.memRegisters[0x32] = 1 #Set PRR to OTP second page
        self.memRegisters[0x33] = 0
        self.RAM = bytearray(0x10000)
        self.videoRegisters = bytearray(0x2)
        self.display = Display()

    def GetBTVector(self):
        struct.unpack('<H', self.OTP[0x7FEC : 0x7FEF])[0]

    def WriteVideoRegister(self, address, byte):
        address %= 2
        self.videoRegisters[address] = byte
        if address == 0:
            self.display.SendCommand(byte)
        else:
            self.display.SendData(byte)       
        
    def StoreMemory(self, address, byte):
        PAGE_SIZE = 0x4000
        BRR_START = 0x2000
        BRR_LENGTH = 0x2000
        PRR_START = 0x4000
        PRR_LENGTH = 0x4000
        DRR_START = 0x8000
        DRR_LENGTH = 0x8000
        REGISTERS_START = 0x0
        REGISTERS_LENGTH = 0x80
        
        if address in range(REGISTERS_START, REGISTERS_START + REGISTERS_LENGTH):
            self.memRegisters[address] = byte

        elif address in range(BRR_START, BRR_START + BRR_LENGTH): #BRR
            brr = self.GetBRR()
            page = brr & 0x7F #Select page for OTP or flash
            paged_location = (address - BRR_START) + PAGE_SIZE*page #Calculate location for OTP or flash
            if (brr & 0b1001111000000000) in (0, 0b1111000000000) : #OTP
                self.OTP[paged_location % len(self.OTP)] = byte
            elif brr & 0b1001111100000000 == 0b1100000000: #video registers
                self.WriteVideoRegister(address, byte)
            elif brr & 0b1001110000000000 == 0b10000000000: #Flash
                self.Flash[paged_location % len(self.Flash)] = byte
            elif brr & 0x8000 == 0x8000: #Internal RAM 0x2000~0x3FFF
                self.RAM[address] = byte #Fall through
            else:
                raise Exception('Invalid BRR mask')
            
        elif address in range(PRR_START, PRR_START + PRR_LENGTH):
            prr = self.GetPRR()
            page = prr & 0x7F #Select page for OTP or flash
            paged_location = (address - PRR_START) + PAGE_SIZE*page #Calculate location for OTP or flash
            if (prr & 0b1000111100000000) in (0, 0b111100000000): #OTP
                self.OTP[paged_location % len(self.OTP)] = byte
            elif prr & 0b1000111110000000 == 0b110000000: #video registers
                self.WriteVideoRegister(address, byte)
            elif prr & 0b1000111000000000 == 0b1000000000: #Flash
                self.Flash[paged_location % len(self.Flash)] = byte
            elif prr & 0x8000 == 0x8000: #Internal RAM 0x4000~0x7FFF
                self.RAM[address] = byte #Fall through
            else:
                raise Exception('Invalid PRR mask')

        elif address in range(DRR_START, PRR_START + DRR_LENGTH):
            drr = self.GetDRR()
            page = drr & 0x7F #Select page for OTP or flash
            paged_location = (address - DRR_START) + PAGE_SIZE*page #Calculate location for OTP or flash
            if (drr & 0b1000111100000000) in (0, 0b111100000000): #OTP
                self.OTP[paged_location % len(self.OTP)] = byte
            elif drr & 0b1000011111000000 == 0b11000000: #video registers
                self.WriteVideoRegister(address, byte)
            elif drr & 0b1000011100000000 == 0b100000000: #Flash
                self.Flash[paged_location % len(self.Flash)] = byte
            elif drr & 0x8000 == 0x8000:
                self.RAM[address-DRR_START] = byte
            else:
                raise Exception('Invalid DRR mask')
        else:
            self.RAM[address] = byte


    def ReadMemory(self, address, length=1):
        PAGE_SIZE = 0x4000
        BRR_START = 0x2000
        BRR_LENGTH = 0x2000
        PRR_START = 0x4000
        PRR_LENGTH = 0x4000
        DRR_START = 0x8000
        DRR_LENGTH = 0x8000
        REGISTERS_START = 0x0
        REGISTERS_LENGTH = 0x80
        if address in range(REGISTERS_START, REGISTERS_START + REGISTERS_LENGTH):
            byte = self.memRegisters[address]
            
        elif address in range(BRR_START, BRR_START + BRR_LENGTH): #BRR
            brr = self.GetBRR()
            page = brr & 0x7F #Select page for OTP or flash
            paged_location = (address - BRR_START) + PAGE_SIZE*page #Calculate location for OTP or flash
            if (brr & 0b1001111000000000) in (0, 0b1111000000000) : #OTP
                byte = self.OTP[paged_location % len(self.OTP)]
            elif brr & 0b1001111100000000 == 0b1100000000: #video registers
                byte = self.videoRegisters[address % 2]
            elif brr & 0b1001110000000000 == 0b10000000000: #Flash
                byte = self.Flash[paged_location % len(self.Flash)]
            elif brr & 0x8000 == 0x8000: #Internal RAM 0x2000~0x3FFF
                byte = self.RAM[address] #Fall through
            else:
                print('Invalid BRR mask')
            
        elif address in range(PRR_START, PRR_START + PRR_LENGTH):
            prr = self.GetPRR()
            page = prr & 0x7F #Select page for OTP or flash
            paged_location = (address - PRR_START) + PAGE_SIZE*page #Calculate location for OTP or flash
            if (prr & 0b1000111100000000) in (0, 0b111100000000): #OTP
                byte = self.OTP[paged_location % len(self.OTP)]
            elif prr & 0b1000111110000000 == 0b110000000: #video registers
                byte = self.videoRegisters[address % 2]
            elif prr & 0b1000111000000000 == 0b1000000000: #Flash
                byte = self.Flash[paged_location % len(self.Flash)]
            elif prr & 0x8000 == 0x8000: #Internal RAM 0x4000~0x7FFF
                byte = self.RAM[address] #Fall through
            else:
                print('Invalid PRR mask')

        elif address in range(DRR_START, PRR_START + DRR_LENGTH):
            drr = self.GetDRR()
            page = drr & 0x7F #Select page for OTP or flash
            paged_location = (address - DRR_START) + PAGE_SIZE*page #Calculate location for OTP or flash
            if (drr & 0b1000111100000000) in (0, 0b111100000000): #OTP
                byte = self.OTP[paged_location % len(self.OTP)]
            elif drr & 0b1000011111000000 == 0b11000000: #video registers
                byte = self.videoRegisters[address % 2]
            elif drr & 0b1000011100000000 == 0b100000000: #Flash
                byte = self.Flash[paged_location % len(self.Flash)]
            elif drr & 0x8000 == 0x8000:
                byte = self.RAM[address-DRR_START]
            else:
                print(hex(self.PC), 'Invalid DRR mask', hex(drr))
        else:
            byte = self.RAM[address]

        if length > 1:
            return bytes([byte]) + bytes([self.ReadMemory(address + 1, length - 1)])
        else:
            return byte
        

    def ImmediateVal(self):
        value = self.ReadMemory(self.PC + 1) #1 byte arg
        return value
    
    def ZeroPagePtr(self):
        pointer = self.ReadMemory(self.PC + 1) #1 byte arg
        return pointer
    def ZeroPageVal(self):
        pointer = self.ZeroPagePtr()
        value = self.ReadMemory(pointer)
        return value
    
    def ZeroPageXPtr(self):
        pointer = self.ReadMemory(self.PC + 1) #1 byte arg
        pointer += self.X
        pointer &= 0xFF
        return pointer
    def ZeroPageXVal(self):
        pointer = self.ZeroPageXPtr()
        value = self.ReadMemory(pointer)
        return value
    
    def ZeroPageYPtr(self):
        pointer = self.ReadMemory(self.PC + 1) #1 byte arg
        pointer += self.Y
        pointer &= 0xFF
        return pointer
    def ZeroPageYVal(self):
        pointer = self.ZeroPageYPtr()
        value = self.ReadMemory(pointer)
        return value

    def RelativePtr(self):
        ptr = struct.unpack('<b', bytes([self.ReadMemory(self.PC + 1)]))[0] #1 byte signed arg
        ptr += self.PC + 2 #instruction size of 2
        return ptr
    def RelativeVal(self):
        ptr = self.RelativePtr()
        value = self.ReadMemory(ptr)
        return value

    def AbsolutePtr(self):
        ptr = struct.unpack('<H', self.ReadMemory(self.PC + 1, length=2))[0] #2 byte arg
        return ptr
    def AbsoluteVal(self):
        ptr = self.AbsolutePtr()
        value = self.ReadMemory(ptr)
        return value

    def AbsoluteXPtr(self):
        ptr = struct.unpack('<H', self.ReadMemory(self.PC + 1, length=2))[0] #2 byte arg
        ptr += self.X
        return ptr
    def AbsoluteXVal(self):
        ptr = self.AbsoluteXPtr()
        val = self.ReadMemory(ptr)
        return val
    
    def AbsoluteYPtr(self):
        ptr = struct.unpack('<H', self.ReadMemory(self.PC + 1, length=2))[0] #2 byte arg
        ptr += self.Y
        return ptr
    def AbsoluteYVal(self):
        ptr = self.AbsoluteYPtr()
        val = self.ReadMemory(ptr)
        return val

    def IndirectPtr(self):
        pointer = struct.unpack('<H', self.ReadMemory(self.PC + 1, length=2))[0] #2 byte arg
        return pointer
    def IndirectVal(self):
        pointer = self.IndirectPtr()
        val = struct.unpack('<H', self.ReadMemory(pointer, length=2))[0] #2 byte value
        return val
    
    def IndirectZeroPagePtr(self):
        pointer = self.ReadMemory(self.PC + 1)
        pointer = struct.unpack('<H', self.ReadMemory(pointer, length=2))[0] #2 byte pointer
        return pointer
    def IndirectZeroPageVal(self):
        pointer = self.IndirectZeroPagePtr()
        value = self.ReadMemory(pointer)
        return value
        
    def IndexedIndirectPtr(self):
        pointer = self.ReadMemory(self.PC + 1)
        pointer += self.X
        pointer &= 0xFF
        pointer = struct.unpack('<H', self.ReadMemory(pointer, length=2))[0] #2 byte pointer
        return pointer
    def IndexedIndirectVal(self):
        pointer = self.IndexedIndirectPtr()
        value = self.ReadMemory(pointer)
        return value
    
    def IndirectIndexedPtr(self):
        pointer = self.ReadMemory(self.PC + 1)
        pointer = self.ReadMemory(pointer) + self.Y
        return pointer
    def IndirectIndexedVal(self):
        pointer = self.IndirectIndexedPtr()
        value = self.ReadMemory(pointer)
        return value
    
    def AbsoluteIndexIndirectPtr(self):
        pointer = struct.unpack('<H', self.ReadMemory(self.PC + 1, length=2))[0] #2 byte arg
        pointer += self.X
        return pointer
    def AbsoluteIndexIndirectVal(self):
        pointer = self.AbsoluteIndexIndirectPtr()
        value = struct.unpack('<H', self.ReadMemory(pointer))[0] #2 byte value
        return value
        
    def Push(self, byte):
        self.StoreMemory(self.STACK_START + self.S, byte)
        self.S += 1

    def PushShort(self, short):
        low = short & 0x00FF
        high = short & 0xFF00
        high >>= 8
        self.Push(low)
        self.Push(high)

    def Pop(self):
        self.S -= 1
        return self.ReadMemory(self.STACK_START + self.S)

    def PopShort(self):
        high = self.Pop()
        low = self.Pop()
        high <<= 8
        short = low + high
        return short

    def ExportFlags(self):
        flags = 0
        flags += int(self.n) #Negative
        flags <<= 1
        flags += int(self.v) #Overflow
        flags <<= 1
        flags += int(True) #Always set
        flags <<= 1
        flags += int(True) #Clear if interrupt vectoring, set if BRK or PHP
        flags <<= 1
        flags += int(self.d) #Decimal mode
        flags <<= 1
        flags += int(self.i) #Interrupt disable
        flags <<= 1
        flags += int(self.z) #Zero
        flags <<= 1
        flags += int(self.c) #Carry
        return flags

    def ImportFlags(self, flags):
        self.n = bool(flags & 0b10000000)
        self.v = bool(flags & 0b01000000)
        self.d = bool(flags & 0b00001000)
        self.i = bool(flags & 0b00000100)
        self.z = bool(flags & 0b00000010)
        self.c = bool(flags & 0b00000001)

    def Step(self, verbose = False):
        opcode = self.ReadMemory(self.PC)
        if verbose:
            if opcode in OPCODES:
                print(f'{hex(self.PC)} {OPCODES[opcode].__name__}')
            else:
                print(f'{hex(self.PC)} {hex(opcode)}')
        func = OPCODES[opcode]
        func(self)
        if verbose:
            print('State after execution:')
            self.PrintState()
            print()

    def ASL_ZP(self):
        val = self.ZeroPageVal()
        self.c = bool(0b10000000 & val)
        val <<= 1
        val &= 0xFF
        self.z = val == 0
        self.n = bool(val & 0b10000000)
        ptr = self.ZeroPagePtr()
        self.StoreMemory(ptr, val)
        self.PC += 2

    def PHP(self):
        flags = self.ExportFlags()
        self.Push(flags)
        self.PC += 1

    def ORA_I(self):
        val = self.ImmediateVal()
        self.A |= val
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 2

    def ASL_ACC(self):
        self.c = bool(0b10000000 & self.A)
        self.A <<= 1
        self.A &= 0xFF
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 1

    def ASL_A(self):
        val = self.AbsoluteVal()
        val <<= 1
        val &= 0xFF
        self.z = val == 0
        self.n = bool(val & 0b10000000)
        ptr = self.AbsolutePtr()
        self.StoreMemory(ptr, val)
        self.PC += 3

    def ORA_A(self):
        val = self.AbsoluteVal()
        self.A |= val
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 3

    def BPL(self):
        ptr = self.RelativePtr()
        if not self.n:
            self.PC = ptr
        else:
            self.PC += 2

    def CLC(self):
        self.c = False
        self.PC += 1

    def JSR(self):
        arg = self.AbsolutePtr()
        return_addr = self.PC + 2
        self.PushShort(return_addr)
        self.PC = arg

    def BIT_ZP(self):
        arg = self.ZeroPageVal()
        result = arg & self.A
        self.z = result == 0
        self.v = bool(arg & 0b01000000)
        self.n = bool(arg & 0b10000000)
        self.PC += 2

    def ROL_ZP(self):
        val = self.ZeroPageVal()
        old_carry = self.c
        self.c = bool(val & 0b10000000)
        val <<= 1
        val &= 0xFF
        val += int(old_carry)
        ptr = self.ZeroPagePtr()
        self.StoreMemory(ptr, val)
        self.PC += 2

    def PLP(self):
        flags = self.Pop()
        self.ImportFlags(flags)
        self.PC += 1

    def AND_I(self):
        arg = self.ImmediateVal()
        self.A &= arg
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 2

    def ROL_ACC(self):
        old_carry = self.c
        self.c = bool(self.A & 0b10000000)
        self.A <<= 1
        self.A &= 0xFF
        self.A += int(old_carry)
        self.PC += 1

    def ROL_A(self):
        val = self.AbsoluteVal()
        old_carry = self.c
        self.c = bool(val & 0b10000000)
        val <<= 1
        val &= 0xFF
        val += int(old_carry)
        ptr = self.AbsolutePtr()
        self.StoreMemory(ptr, val)
        self.PC += 3 

    def AND_A(self):
        val = self.AbsoluteVal()
        self.A &= val
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 3
        
    def SEC(self):
        self.c = True
        self.PC += 1

    def EOR_ZP(self):
        val = self.ZeroPageVal()
        self.A ^= val
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 2

    def LSR_ZP(self):
        val = self.ZeroPageVal()
        self.c = bool(val & 0b00000001)
        val >>= 1
        self.z = val == 0
        self.n = bool(val & 0b10000000)
        ptr = self.ZeroPagePtr()
        self.StoreMemory(ptr, val)
        self.PC += 2

    def PHA(self):
        self.Push(self.A)
        self.PC += 1

    def EOR_I(self):
        val = self.ImmediateVal()
        self.A ^= val
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 2

    def LSR_ACC(self):
        self.c = bool(0b00000001 & self.A)
        self.A >>= 1
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 1

    def JMP_A(self):
        arg = self.AbsolutePtr()
        self.PC = arg

    def LSR_A(self):
        val = self.AbsoluteVal()
        self.c = bool(0b00000001 & val)
        val >>= 1
        self.z = val == 0
        self.n = bool(val & 0b10000000)
        ptr = self.AbsolutePtr()
        self.StoreMemory(ptr, val)
        self.PC += 3

    def CLI(self):
        self.i = False
        self.PC += 1

    def PHY(self):
        self.Push(self.Y)
        self.PC += 1
    
    def RTS(self):
        return_addr = self.PopShort()
        self.PC = return_addr + 1

    def STZ_ZP(self):
        ptr = self.ZeroPagePtr()
        self.StoreMemory(ptr, 0)
        self.PC += 2

    def ADC_ZP(self):
        add = self.ZeroPageVal()
        self.A += add + int(self.c)
        self.c = self.A > 0xFF
        self.A &= 0xFF
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 2

    def ROR_ZP(self):
        val = self.ZeroPageVal()
        old_carry = self.c
        self.c = bool(val & 0b00000001)
        val >>= 1
        if old_carry:
            val |= 0b10000000
        else:
            val &= 0b01111111
        self.n = bool(val & 0b10000000)
        ptr = self.ZeroPagePtr()
        self.StoreMemory(ptr, val)
        self.PC += 2

    def RMB6_ZP(self):
        val = self.ZeroPageVal()
        val &= 0b10111111
        ptr = self.ZeroPagePtr()
        self.StoreMemory(ptr, val)
        self.PC += 2

    def PLA(self):
        self.A = self.Pop()
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 1

    def ADC_I(self):
        add = self.ImmediateVal()
        self.A += add + int(self.c)
        self.c = self.A > 0xFF
        self.A &= 0xFF
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 2

    def JMP_I(self):
        arg = self.IndirectVal()
        self.PC = arg

    def ROR_A(self):
        val = self.AbsoluteVal()
        old_carry = self.c
        self.c = bool(val & 0b00000001)
        val >>= 1
        if old_carry:
            val |= 0b10000000
        else:
            val &= 0b01111111
        self.n = bool(val & 0b10000000)
        ptr = self.AbsolutePtr()
        self.StoreMemory(ptr, val)
        self.PC += 3
        
    def ADC_A(self):
        add = self.AbsoluteVal()
        self.A += add + int(self.c)
        self.c = self.A > 0xFF
        self.A &= 0xFF
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 3

    def SEI(self):
        self.i = True
        self.PC += 1

    def PLY(self):
        self.Y = self.Pop()
        self.z = self.Y == 0 #reference says A
        self.n = bool(self.Y & 0b10000000) #reference says A
        self.PC += 1

    def BRA(self):
        ptr = self.RelativePtr()
        self.PC = ptr

    def STA_A(self):
        arg = self.AbsolutePtr()
        self.StoreMemory(arg, self.A)
        self.PC += 3

    def STA_ZP(self):
        ptr = self.ZeroPagePtr()
        self.StoreMemory(ptr, self.A)
        self.PC += 2

    def SMB0_ZP(self):
        val = self.ZeroPageVal()
        val |= 0b00000001
        ptr = self.ZeroPagePtr()
        self.StoreMemory(ptr, val)
        self.PC += 2

    def DEY(self):
        self.Y -= 1
        self.Y &= 0xFF
        self.z = self.Y == 0
        self.n = bool(self.Y & 0b10000000)
        self.PC += 1

    def STX_A(self):
        ptr = self.AbsolutePtr()
        self.StoreMemory(ptr, self.X)
        self.PC += 3

    def BCC(self):
        arg = self.RelativePtr()
        if not self.c:
            self.PC = arg
        else:
            self.PC += 2

    def STA_IZP(self):
        ptr = self.IndirectZeroPagePtr()
        self.StoreMemory(ptr, self.A)
        self.PC += 2

    def STA_ZPX(self):
        ptr = self.ZeroPageXPtr()
        self.StoreMemory(ptr, self.A)
        self.PC += 2

    def TYA(self):
        self.A = self.Y
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 1

    def STA_AY(self):
        ptr = self.AbsoluteYPtr()
        self.StoreMemory(ptr, self.A)
        self.PC += 3

    def TXS(self):
        self.S = self.X
        self.PC += 1

    def STZ_A(self):
        ptr = self.AbsolutePtr()
        self.StoreMemory(ptr, 0)
        self.PC += 3
        
    def STA_AX(self):
        ptr = self.AbsoluteXPtr()
        self.StoreMemory(ptr, self.A)
        self.PC += 3

    def LDY_I(self):
        val = self.ImmediateVal()
        self.Y = val
        self.z = self.Y == 0
        self.n = bool(self.Y & 0b10000000)
        self.PC += 2

    def LDA_INDEXED_INDIRECT(self):
        val = self.IndexedIndirectVal()
        self.A = val
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 2

    def LDX_I(self):
        val = self.ImmediateVal()
        self.X = val
        self.z = self.X == 0
        self.n = bool(self.X & 0b10000000)
        self.PC += 2

    def LDA_ZP(self):
        val = self.ZeroPageVal()
        self.A = val
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 2

    def LDX_ZP(self):
        val = self.ZeroPageVal()
        self.X = val
        self.z = self.X == 0
        self.n = bool(self.X & 0b10000000)
        self.PC += 2

    def TAY(self):
        self.Y = self.A
        self.z = self.Y == 0
        self.n = bool(self.Y & 0b10000000)
        self.PC += 1

    def LDA_I(self):
        arg = self.ImmediateVal()
        self.A = arg
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 2

    def LDA_A(self):
        val = self.AbsoluteVal()
        self.A = val
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 3

    def BCS(self):
        ptr = self.RelativePtr()
        if self.c:
            self.PC = ptr
        else:
            self.PC += 2

    def LDA_INDIRECT_INDEXED(self):
        val = self.IndirectIndexedVal()
        self.A = val
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 2

    def LDA_IZP(self):
        val = self.IndirectZeroPageVal()
        ptr = self.IndirectZeroPagePtr()
        self.A = val
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 2

    def LDA_ZPX(self):
        val = self.ZeroPageXVal()
        self.A = val
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 2

    def LDA_AY(self):
        val = self.AbsoluteYVal()
        self.A = val
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 3

    def LDY_AX(self):
        val = self.AbsoluteXVal()
        self.Y = val
        self.z = self.Y == 0
        self.n = bool(self.Y & 0b10000000)
        self.PC += 3

    def LDA_AX(self):
        val = self.AbsoluteXVal()
        self.A = val
        self.z = self.Y == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 3
        
    def CPY_I(self):
        arg = self.ImmediateVal()
        self.c = self.Y >= arg
        self.z = self.Y == arg
        self.n = self.Y < arg
        self.PC += 2

    def CPY_ZP(self):
        arg = self.ZeroPageVal()
        self.c = self.Y >= arg
        self.z = self.Y == arg
        self.n = self.Y < arg
        self.PC += 2

    def DEC_ZP(self):
        val = self.ZeroPageVal()
        val -= 1
        val &= 0xFF
        ptr = self.ZeroPagePtr()
        self.StoreMemory(ptr, val)
        self.PC += 2

    def INY(self):
        self.Y += 1
        self.Y &= 0xFF
        self.z = self.Y == 0
        self.n = bool(self.Y & 0b10000000)
        self.PC += 1

    def CMP_I(self):
        val = self.ImmediateVal()
        self.c = self.A >= val
        self.z = self.A == val
        self.n = self.A < val
        self.PC += 2

    def DEX(self):
        self.X -= 1
        self.X &= 0xFF
        self.z = self.X == 0
        self.n = bool(self.X & 0b10000000)
        self.PC += 1

    def TAX(self):
        self.X = self.A
        self.z = self.X == 0
        self.n = bool(self.X & 0b10000000)
        self.PC += 1

    def CPY_A(self):
        val = self.AbsoluteVal()
        self.c = self.Y >= val
        self.z = self.Y == val
        self.n = self.Y < val
        self.PC += 3

    def DEC_A(self):
        val = self.AbsoluteVal()
        val -= 1
        val &= 0xFF
        self.z = val == 0
        self.n = bool(val & 0b10000000)
        ptr = self.AbsolutePtr()
        self.StoreMemory(ptr, val)
        self.PC += 3

    def BNE(self):
        arg = self.RelativePtr()
        if not self.z:
            self.PC = arg
        else:
            self.PC += 2

    def DEC_ZPX(self):
        val = self.ZeroPageXVal()
        val -= 1
        val &= 0xFF
        self.z = val == 0
        self.n = bool(val & 0b10000000)
        ptr = self.ZeroPageXPtr()
        self.StoreMemory(ptr, val)
        self.PC += 2

    def PHX(self):
        self.Push(self.X)
        self.PC += 1

    def CPX_I(self):
        arg = self.ImmediateVal()
        self.c = self.X >= arg
        self.z = self.X == arg
        self.n = self.X < arg
        self.PC += 2

    def CPX_ZP(self):
        arg = self.ZeroPageVal()
        self.c = self.X >= arg
        self.z = self.X == arg
        self.n = self.X < arg
        self.PC += 2

    def SBC_ZP(self):
        val = self.ZeroPageVal()
        self.A -= val
        if not self.c:
            self.A -= 1
        if self.A < -127:
            self.v = True
        else:
            self.v = False
        self.A &= 0xFF
        self.z = self.A == 0
        self.PC += 2

    def INC_ZP(self):
        val = self.ZeroPageVal()
        val += 1
        val &= 0xFF
        self.z = val == 0
        self.n = bool(val & 0b10000000)
        ptr = self.ZeroPagePtr()
        self.StoreMemory(ptr, val)
        self.PC += 2

    def SMB6_ZP(self):
        val = self.ZeroPageVal()
        val |= 0b01000000
        ptr = self.ZeroPagePtr()
        self.StoreMemory(ptr, val)
        self.PC += 2
        
    def INX(self):
        self.X += 1
        self.X &= 0xFF
        self.z = self.X == 0
        self.n = bool(self.X & 0b10000000)
        self.PC += 1

    def SBC_I(self):
        val = self.ImmediateVal()
        self.A -= val
        if not self.c:
            self.A -= 1
        if self.A < -127:
            self.v = True
        else:
            self.v = False
        self.A &= 0xFF
        self.z = self.A == 0
        self.PC += 2

    def NOP(self):
        self.PC += 1
        
    def CPX_A(self):
        val = self.AbsoluteVal()
        self.c = self.X >= val
        self.z = self.X == val
        self.n = self.X < val
        self.PC += 3

    def SBC_A(self):
        val = self.AbsoluteVal()
        self.A -= val
        if not self.c:
            self.A -= 1
        if self.A < -127:
            self.v = True
        else:
            self.v = False
        self.A &= 0xFF
        self.z = self.A == 0
        self.PC += 3

    def INC_A(self):
        val = self.AbsoluteVal()
        val += 1
        val &= 0xFF
        self.z = val == 0
        self.n = bool(val & 0b10000000)
        ptr = self.AbsolutePtr()
        self.StoreMemory(ptr, val)
        self.PC += 3

    def BEQ(self):
        arg = self.RelativePtr()
        if self.z:
            self.PC = arg
        else:
            self.PC += 2

    def INC_ZPX(self):
        val = self.ZeroPageXVal()
        val += 1
        val &= 0xFF
        self.z = val == 0
        self.n = bool(val & 0b10000000)
        ptr = self.ZeroPageXPtr()
        self.StoreMemory(ptr, val)
        self.PC += 2

    def SMB7_ZP(self):
        val = self.ZeroPageVal()
        val |= 0b10000000
        ptr = self.ZeroPagePtr()
        self.StoreMemory(ptr, val)
        self.PC += 2

    def PLX(self):
        self.X = self.Pop()
        self.z = self.X == 0 #Reference says A
        self.n = bool(self.X & 0b10000000) #Reference says A
        self.PC += 1
        
    
        


OPCODES = {
        0x06: CPU.ASL_ZP,
        0x08: CPU.PHP,
        0x09: CPU.ORA_I,
        0x0A: CPU.ASL_ACC,
        0x0E: CPU.ASL_A,
        0x0D: CPU.ORA_A,
        0x10: CPU.BPL,
        0x18: CPU.CLC,
        0x20: CPU.JSR,
        0x24: CPU.BIT_ZP,
        0x26: CPU.ROL_ZP,
        0x28: CPU.PLP,
        0x29: CPU.AND_I,
        0x2A: CPU.ROL_ACC,
        0x2E: CPU.ROL_A,
        0x2D: CPU.AND_A,
        0x38: CPU.SEC,
        0x45: CPU.EOR_ZP,
        0x46: CPU.LSR_ZP,
        0x48: CPU.PHA,
        0x49: CPU.EOR_I,
        0x4A: CPU.LSR_ACC,
        0x4C: CPU.JMP_A,
        0x4E: CPU.LSR_A,
        0x58: CPU.CLI,
        0x5A: CPU.PHY,
        0x60: CPU.RTS,
        0x64: CPU.STZ_ZP,
        0x65: CPU.ADC_ZP,
        0x66: CPU.ROR_ZP,
        0x67: CPU.RMB6_ZP,
        0x68: CPU.PLA,
        0x69: CPU.ADC_I,
        0x6C: CPU.JMP_I,
        0x6E: CPU.ROR_A,
        0x6D: CPU.ADC_A,
        0x78: CPU.SEI,
        0x7A: CPU.PLY,
        0x80: CPU.BRA,
        0x8D: CPU.STA_A,
        0x85: CPU.STA_ZP,
        0x87: CPU.SMB0_ZP,
        0x88: CPU.DEY,
        0x8E: CPU.STX_A,
        0x90: CPU.BCC,
        0x92: CPU.STA_IZP,
        0x95: CPU.STA_ZPX,
        0x98: CPU.TYA,
        0x99: CPU.STA_AY,
        0x9A: CPU.TXS,
        0x9C: CPU.STZ_A,
        0x9D: CPU.STA_AX,
        0xA0: CPU.LDY_I,
        0xA1: CPU.LDA_INDEXED_INDIRECT,
        0xA2: CPU.LDX_I,
        0xA5: CPU.LDA_ZP,
        0xA6: CPU.LDX_ZP,
        0xA8: CPU.TAY,
        0xA9: CPU.LDA_I,
        0xAA: CPU.TAX,
        0xAD: CPU.LDA_A,
        0xB0: CPU.BCS,
        0xB1: CPU.LDA_INDIRECT_INDEXED,
        0xB2: CPU.LDA_IZP,
        0xB5: CPU.LDA_ZPX,
        0xB9: CPU.LDA_AY,
        0xBC: CPU.LDY_AX,
        0xBD: CPU.LDA_AX,
        0xC0: CPU.CPY_I,
        0xC4: CPU.CPY_ZP,
        0xC6: CPU.DEC_ZP,
        0xC8: CPU.INY,
        0xC9: CPU.CMP_I,
        0xCA: CPU.DEX,
        0xCC: CPU.CPY_A,
        0xCE: CPU.DEC_A,
        0xD0: CPU.BNE,
        0xD6: CPU.DEC_ZPX,
        0xDA: CPU.PHX,
        0xE0: CPU.CPX_I,
        0xE4: CPU.CPX_ZP,
        0xE5: CPU.SBC_ZP, #check
        0xE6: CPU.INC_ZP,
        0xE7: CPU.SMB6_ZP,
        0xE8: CPU.INX,
        0xE9: CPU.SBC_I, #check
        0xEA: CPU.NOP,
        0xEC: CPU.CPX_A,
        0xED: CPU.SBC_A, #check
        0xEE: CPU.INC_A,
        0xF0: CPU.BEQ,
        0xF6: CPU.INC_ZPX,
        0xF7: CPU.SMB7_ZP,
        0xFA: CPU.PLX
        
        }
        

def Controls():
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
##    controls &= MENU
##    controls &= LEFT
    return controls

from threading import Thread

cpu = CPU('SimpleOTP.dat', 'nova_original.dat')
##cpu = CPU('SimpleOTP.dat', 'Spike 1.09.03.dat')

##cpu = CPU('fixed_otp.bin', 'nova_original.dat')
##cpu.memory[0x42D] = 1
##cpu.memory[0x435] = 0x2A


def run():
##    cpu.PC = 0x5DDC
##    cpu.PC = 0x5D0E
    breaking = False
    verbose = False
    while True:
        cpu.StoreMemory(0, Controls())
        cpu.StoreMemory(1, 0xFF)

##        if cpu.PC == 0x422f:
##        breaking = True
##        verbose = True


        if breaking:
            input()

        try:
            cpu.Step(verbose)
        except KeyError as e:
            print('Opcode', hex(e.args[0]), 'Address', hex(cpu.PC))
            break


Thread(target=run).start()
