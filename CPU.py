import struct
from Display import Display
from MMU import MMU
import Interrupt
class CPU():
    STACK_START = 0x100
    IRR = 0x30 #PRR when interrupting
    PRR = 0x32 #controls 4000~7FFF
    DRR = 0x34 #controls 8000~FFFF
    BRR = 0x36 #controls 2000~3FFF
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

        self.memRegisters = bytearray(0x80)
        self.RAM = bytearray(0x10000)
        self.videoRegisters = bytearray(0x2)

        self.interrupted = True

        self.MMU = MMU(self)


        self.BTInterrupt = Interrupt.BTInterrupt(self)

        self.Reset()

        self.interrupted = False

    def PrintState(self):
        print(f'A:{self.A} X:{self.X} Y:{self.Y} PC:{self.PC:X} C:{self.c} Z:{self.z}')
        
    def GetIRR(self):
        return struct.unpack('<H', self.MMU.ReadMemory(self.IRR, length=2))[0]
    def GetPRR(self):
        if not self.interrupted:
            return struct.unpack('<H', self.MMU.ReadMemory(self.PRR, length=2))[0]
        else:
            return self.GetIRR()
    def GetDRR(self):
        return struct.unpack('<H', self.MMU.ReadMemory(self.DRR, length=2))[0]
    def GetBRR(self):
        return struct.unpack('<H', self.MMU.ReadMemory(self.BRR, length=2))[0]

    def GetResetVector(self):
        if not self.interrupted:
            raise Exception("Can't get vector unless interrupted")
        return struct.unpack('<H', self.MMU.ReadMemory(0x7FFC, length=2))[0]
    
    def Reset(self):
        self.i = True
        self.d = False
        self.PC = self.GetResetVector()
        self.display = Display()

    def GetBTVector(self):
        if not self.interrupted:
            raise Exception("Can't get vector unless interrupted")
        return struct.unpack('<H', self.MMU.ReadMemory(0x7FEC, length=2))[0]

    def WriteVideoRegister(self, address, byte):
##        print(f'Video write coming from {hex(self.PC)}')
        address %= 2
        self.videoRegisters[address] = byte
        if address == 0:
##            print(f'Command coming from {hex(self.PC)}: ')
            self.display.SendCommand(byte)
        else:
            #print(f'Data coming from {hex(self.PC)}: ', end='')
            self.display.SendData(byte)       
        

    def ImmediateVal(self):
        value = self.MMU.ReadMemory(self.PC + 1) #1 byte arg
        return value
    
    def ZeroPagePtr(self):
        pointer = self.MMU.ReadMemory(self.PC + 1) #1 byte arg
        return pointer
    def ZeroPageVal(self):
        pointer = self.ZeroPagePtr()
        value = self.MMU.ReadMemory(pointer)
        return value
    
    def ZeroPageXPtr(self):
        pointer = self.MMU.ReadMemory(self.PC + 1) #1 byte arg
        pointer += self.X
        pointer &= 0xFF
        return pointer
    def ZeroPageXVal(self):
        pointer = self.ZeroPageXPtr()
        value = self.MMU.ReadMemory(pointer)
        return value
    
    def ZeroPageYPtr(self):
        pointer = self.MMU.ReadMemory(self.PC + 1) #1 byte arg
        pointer += self.Y
        pointer &= 0xFF
        return pointer
    def ZeroPageYVal(self):
        pointer = self.ZeroPageYPtr()
        value = self.MMU.ReadMemory(pointer)
        return value

    def RelativePtr(self):
        ptr = struct.unpack('<b', bytes([self.MMU.ReadMemory(self.PC + 1)]))[0] #1 byte signed arg
        ptr += self.PC + 2 #instruction size of 2
        return ptr
    def RelativeVal(self):
        ptr = self.RelativePtr()
        value = self.MMU.ReadMemory(ptr)
        return value

    def AbsolutePtr(self):
        ptr = struct.unpack('<H', self.MMU.ReadMemory(self.PC + 1, length=2))[0] #2 byte arg
        return ptr
    def AbsoluteVal(self):
        ptr = self.AbsolutePtr()
        value = self.MMU.ReadMemory(ptr)
        return value

    def AbsoluteXPtr(self):
        ptr = struct.unpack('<H', self.MMU.ReadMemory(self.PC + 1, length=2))[0] #2 byte arg
        ptr += self.X
        return ptr
    def AbsoluteXVal(self):
        ptr = self.AbsoluteXPtr()
        val = self.MMU.ReadMemory(ptr)
        return val
    
    def AbsoluteYPtr(self):
        ptr = struct.unpack('<H', self.MMU.ReadMemory(self.PC + 1, length=2))[0] #2 byte arg
        ptr += self.Y
        return ptr
    def AbsoluteYVal(self):
        ptr = self.AbsoluteYPtr()
        val = self.MMU.ReadMemory(ptr)
        return val

    def IndirectPtr(self):
        pointer = struct.unpack('<H', self.MMU.ReadMemory(self.PC + 1, length=2))[0] #2 byte arg
        return pointer
    def IndirectVal(self):
        pointer = self.IndirectPtr()
        val = struct.unpack('<H', self.MMU.ReadMemory(pointer, length=2))[0] #2 byte value
        return val
    
    def IndirectZeroPagePtr(self):
        pointer = self.MMU.ReadMemory(self.PC + 1)
        pointer = struct.unpack('<H', self.MMU.ReadMemory(pointer, length=2))[0] #2 byte pointer
        return pointer
    def IndirectZeroPageVal(self):
        pointer = self.IndirectZeroPagePtr()
        value = self.MMU.ReadMemory(pointer)
        return value
        
    def IndexedIndirectPtr(self):
        pointer = self.MMU.ReadMemory(self.PC + 1)
        pointer += self.X
        pointer &= 0xFF
        pointer = struct.unpack('<H', self.MMU.ReadMemory(pointer, length=2))[0] #2 byte pointer
        return pointer
    def IndexedIndirectVal(self):
        pointer = self.IndexedIndirectPtr()
        value = self.MMU.ReadMemory(pointer)
        return value
    
    def IndirectIndexedPtr(self):
        pointer = self.MMU.ReadMemory(self.PC + 1)
        pointer = self.MMU.ReadMemory(pointer) + self.Y
        return pointer
    def IndirectIndexedVal(self):
        pointer = self.IndirectIndexedPtr()
        value = self.MMU.ReadMemory(pointer)
        return value
    
    def AbsoluteIndexIndirectPtr(self):
        pointer = struct.unpack('<H', self.MMU.ReadMemory(self.PC + 1, length=2))[0] #2 byte arg
        pointer += self.X
        return pointer
    def AbsoluteIndexIndirectVal(self):
        pointer = self.AbsoluteIndexIndirectPtr()
        value = struct.unpack('<H', self.MMU.ReadMemory(pointer, length=2))[0] #2 byte value
        return value
        
    def Push(self, byte):
        self.MMU.StoreMemory(self.STACK_START + self.S, byte)
        self.S += 1

    def PushShort(self, short):
        low = short & 0x00FF
        high = short & 0xFF00
        high >>= 8
        self.Push(low)
        self.Push(high)

    def Pop(self):
        self.S -= 1
        return self.MMU.ReadMemory(self.STACK_START + self.S)

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
        opcode = self.MMU.ReadMemory(self.PC)
        if verbose:
            if opcode in OPCODES:
                print(f'{hex(self.PC)} {OPCODES[opcode].__name__}')
            else:
                print(f'{hex(self.PC)} {hex(opcode)}')
                
        if opcode not in OPCODES:
            print(f'{self.PC:X}: opcode {opcode:X}')
        func = OPCODES[opcode]
        func(self)
        if verbose:
            print('State after execution:')
            self.PrintState()
            print()

        #interupts
        interrupt_requested = self.BTInterrupt.Update()
        if not self.i:
            if not self.interrupted and interrupt_requested:
                self.interrupted = True
                self.PushShort(self.PC)
                self.Push(self.ExportFlags())
                self.PC = self.GetBTVector()
            

    def ASL_ZP(self):
        val = self.ZeroPageVal()
        self.c = bool(0b10000000 & val)
        val <<= 1
        val &= 0xFF
        self.z = val == 0
        self.n = bool(val & 0b10000000)
        ptr = self.ZeroPagePtr()
        self.MMU.StoreMemory(ptr, val)
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
        self.MMU.StoreMemory(ptr, val)
        self.PC += 3

    def ORA_A(self):
        val = self.AbsoluteVal()
        self.A |= val
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 3

    def BBR0(self):
        val = self.ZeroPageVal()
        self.PC += 1
        ptr = self.RelativePtr()
        self.PC += 2
        if not val & 0b1:
            self.PC = ptr

    def BPL(self):
        ptr = self.RelativePtr()
        if not self.n:
            self.PC = ptr
        else:
            self.PC += 2

    def CLC(self):
        self.c = False
        self.PC += 1

    def BBR1(self):
        val = self.ZeroPageVal()
        self.PC += 1
        ptr = self.RelativePtr()
        self.PC += 2
        if not val & 0b10:
            self.PC = ptr

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
        self.MMU.StoreMemory(ptr, val)
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

    def BBR2(self):
        val = self.ZeroPageVal()
        self.PC += 1
        ptr = self.RelativePtr()
        self.PC += 2
        if not val & 0b100:
            self.PC = ptr

    def ROL_A(self):
        val = self.AbsoluteVal()
        old_carry = self.c
        self.c = bool(val & 0b10000000)
        val <<= 1
        val &= 0xFF
        val += int(old_carry)
        ptr = self.AbsolutePtr()
        self.MMU.StoreMemory(ptr, val)
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

    def BBR3(self):
        val = self.ZeroPageVal()
        self.PC += 1
        ptr = self.RelativePtr()
        self.PC += 2
        if not val & 0b1000:
            self.PC = ptr

    def RTI(self):
        flags = self.Pop()
        self.ImportFlags(flags)
        return_addr = self.PopShort()
        self.PC = return_addr
        self.interrupted = False

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
        self.MMU.StoreMemory(ptr, val)
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
##        if self.PC<0x500:
##            print(f'JMPing to {arg:X}')
        self.PC = arg

    def EOR_A(self):
        val = self.AbsoluteVal()
        self.A ^= val
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 3

    def LSR_A(self):
        val = self.AbsoluteVal()
        self.c = bool(0b00000001 & val)
        val >>= 1
        self.z = val == 0
        self.n = bool(val & 0b10000000)
        ptr = self.AbsolutePtr()
        self.MMU.StoreMemory(ptr, val)
        self.PC += 3

    def BBR4(self):
        val = self.ZeroPageVal()
        self.PC += 1
        ptr = self.RelativePtr()
        self.PC += 2
        if not val & 0b10000:
            self.PC = ptr

    def CLI(self):
        self.i = False
        self.PC += 1

    def PHY(self):
        self.Push(self.Y)
        self.PC += 1

    def BBR5(self):
        val = self.ZeroPageVal()
        self.PC += 1
        ptr = self.RelativePtr()
        self.PC += 2
        if not val & 0b100000:
            self.PC = ptr

    def RTS(self):
        return_addr = self.PopShort()
        self.PC = return_addr + 1

    def STZ_ZP(self):
        ptr = self.ZeroPagePtr()
        self.MMU.StoreMemory(ptr, 0)
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
        self.MMU.StoreMemory(ptr, val)
        self.PC += 2

    def RMB6_ZP(self):
        val = self.ZeroPageVal()
        val &= 0b10111111
        ptr = self.ZeroPagePtr()
        self.MMU.StoreMemory(ptr, val)
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

    def ROR_ACC(self):
        old_carry = self.c
        self.c = bool(self.A & 0b00000001)
        self.A >>= 1
        if old_carry:
            self.A |= 0b10000000
        else:
            self.A &= 0b01111111
        self.n = bool(self.A & 0b10000000)
        self.PC += 1
        
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
        self.MMU.StoreMemory(ptr, val)
        self.PC += 3
        
    def ADC_A(self):
        add = self.AbsoluteVal()
        self.A += add + int(self.c)
        self.c = self.A > 0xFF
        self.A &= 0xFF
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 3

    def BBR6(self):
        val = self.ZeroPageVal()
        self.PC += 1
        ptr = self.RelativePtr()
        self.PC += 2
        if not val & 0b1000000:
            self.PC = ptr

    def SEI(self):
        self.i = True
        self.PC += 1

    def PLY(self):
        self.Y = self.Pop()
        self.z = self.Y == 0 #reference says A
        self.n = bool(self.Y & 0b10000000) #reference says A
        self.PC += 1

    def JMP_ABSOLUTE_INDEXED_INDIRECT(self):
        arg = self.AbsoluteIndexIndirectVal()
        self.PC = arg

    def BBR7(self):
        val = self.ZeroPageVal()
        self.PC += 1
        ptr = self.RelativePtr()
        self.PC += 2
        if not val & 0b10000000:
            self.PC = ptr

    def BRA(self):
        ptr = self.RelativePtr()
        self.PC = ptr

    def STA_A(self):
        arg = self.AbsolutePtr()
        self.MMU.StoreMemory(arg, self.A)
        self.PC += 3

    def STA_ZP(self):
        ptr = self.ZeroPagePtr()
        self.MMU.StoreMemory(ptr, self.A)
        self.PC += 2

    def SMB0_ZP(self):
        val = self.ZeroPageVal()
        val |= 0b00000001
        ptr = self.ZeroPagePtr()
        self.MMU.StoreMemory(ptr, val)
        self.PC += 2

    def DEY(self):
        self.Y -= 1
        self.Y &= 0xFF
        self.z = self.Y == 0
        self.n = bool(self.Y & 0b10000000)
        self.PC += 1

    def STX_A(self):
        ptr = self.AbsolutePtr()
        self.MMU.StoreMemory(ptr, self.X)
        self.PC += 3

    def BBS0(self):
        val = self.ZeroPageVal()
        self.PC += 1
        ptr = self.RelativePtr()
        self.PC += 2
        if val & 0b1:
            self.PC = ptr

    def BCC(self):
        arg = self.RelativePtr()
        if not self.c:
            self.PC = arg
        else:
            self.PC += 2

    def STA_INDIRECT_INDEXED(self):
        ptr = self.IndirectIndexedPtr()
        self.MMU.StoreMemory(ptr, self.A)
        self.PC += 2

    def STA_IZP(self):
        ptr = self.IndirectZeroPagePtr()
        self.MMU.StoreMemory(ptr, self.A)
        self.PC += 2

    def STA_ZPX(self):
        ptr = self.ZeroPageXPtr()
        self.MMU.StoreMemory(ptr, self.A)
        self.PC += 2

    def TYA(self):
        self.A = self.Y
        self.z = self.A == 0
        self.n = bool(self.A & 0b10000000)
        self.PC += 1

    def STA_AY(self):
        ptr = self.AbsoluteYPtr()
        self.MMU.StoreMemory(ptr, self.A)
        self.PC += 3

    def TXS(self):
        self.S = self.X
        self.PC += 1

    def STZ_A(self):
        ptr = self.AbsolutePtr()
        self.MMU.StoreMemory(ptr, 0)
        self.PC += 3
        
    def STA_AX(self):
        ptr = self.AbsoluteXPtr()
        self.MMU.StoreMemory(ptr, self.A)
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

    def SMB2_ZP(self):
        val = self.ZeroPageVal()
        val |= 0b00000100
        ptr = self.ZeroPagePtr()
        self.MMU.StoreMemory(ptr, val)
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
        self.MMU.StoreMemory(ptr, val)
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

    def WAI(self):#NOT IMPLEMENTED AT ALL
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
        self.MMU.StoreMemory(ptr, val)
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
        self.MMU.StoreMemory(ptr, val)
        self.PC += 2

    def PHX(self):
        self.Push(self.X)
        self.PC += 1

    def DEC_AX(self):
        val = self.AbsoluteXVal()
        val -= 1
        val &= 0xFF
        self.z = val == 0
        self.n = bool(val & 0b10000000)
        ptr = self.AbsoluteXPtr()
        self.MMU.StoreMemory(ptr, val)
        self.PC += 3

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
        self.MMU.StoreMemory(ptr, val)
        self.PC += 2

    def SMB6_ZP(self):
        val = self.ZeroPageVal()
        val |= 0b01000000
        ptr = self.ZeroPagePtr()
        self.MMU.StoreMemory(ptr, val)
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
        self.MMU.StoreMemory(ptr, val)
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
        self.MMU.StoreMemory(ptr, val)
        self.PC += 2

    def SMB7_ZP(self):
        val = self.ZeroPageVal()
        val |= 0b10000000
        ptr = self.ZeroPagePtr()
        self.MMU.StoreMemory(ptr, val)
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
        0x0F: CPU.BBR0,
        0x10: CPU.BPL,
        0x18: CPU.CLC,
        0x1F: CPU.BBR1,
        0x20: CPU.JSR,
        0x24: CPU.BIT_ZP,
        0x26: CPU.ROL_ZP,
        0x28: CPU.PLP,
        0x29: CPU.AND_I,
        0x2A: CPU.ROL_ACC,
        0x2E: CPU.ROL_A,
        0x2F: CPU.BBR2,
        0x2D: CPU.AND_A,
        0x38: CPU.SEC,
        0x3F: CPU.BBR3,
        0x40: CPU.RTI,
        0x45: CPU.EOR_ZP,
        0x46: CPU.LSR_ZP,
        0x48: CPU.PHA,
        0x49: CPU.EOR_I,
        0x4A: CPU.LSR_ACC,
        0x4C: CPU.JMP_A,
        0x4D: CPU.EOR_A,
        0x4E: CPU.LSR_A,
        0x4F: CPU.BBR4,
        0x58: CPU.CLI,
        0x5A: CPU.PHY,
        0x5F: CPU.BBR5,
        0x60: CPU.RTS,
        0x64: CPU.STZ_ZP,
        0x65: CPU.ADC_ZP,
        0x66: CPU.ROR_ZP,
        0x67: CPU.RMB6_ZP,
        0x68: CPU.PLA,
        0x69: CPU.ADC_I,
        0x6A: CPU.ROR_ACC,
        0x6C: CPU.JMP_I,
        0x6E: CPU.ROR_A,
        0x6D: CPU.ADC_A,
        0x6F: CPU.BBR6,
        0x78: CPU.SEI,
        0x7A: CPU.PLY,
        0x7C: CPU.JMP_ABSOLUTE_INDEXED_INDIRECT,
        0x7F: CPU.BBR7,
        0x80: CPU.BRA,
        0x8D: CPU.STA_A,
        0x85: CPU.STA_ZP,
        0x87: CPU.SMB0_ZP,
        0x88: CPU.DEY,
        0x8E: CPU.STX_A,
        0x8F: CPU.BBS0,
        0x90: CPU.BCC,
        0x91: CPU.STA_INDIRECT_INDEXED,
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
        0xA7: CPU.SMB2_ZP,
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
        0xCB: CPU.WAI,
        0xCC: CPU.CPY_A,
        0xCE: CPU.DEC_A,
        0xD0: CPU.BNE,
        0xD6: CPU.DEC_ZPX,
        0xDA: CPU.PHX,
        0xDE: CPU.DEC_AX,
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
        
