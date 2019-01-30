import struct
from Display import Display
from MMU import MMU
from cpython cimport bool
cdef class CPU:
    cdef public unsigned short STACK_START
    cdef public unsigned short PRR
    cdef public unsigned short DRR
    cdef public unsigned short BRR

    cdef public bytearray OTP
    cdef public bytearray Flash
    
    cdef public bool c
    cdef public bool z
    cdef public bool i
    cdef public bool d
    cdef public bool b
    cdef public bool v
    cdef public bool n

    cdef public unsigned char A
    cdef public unsigned char X
    cdef public unsigned char Y
    cdef public unsigned char S
    cdef public unsigned short PC

    cdef public bytearray memRegisters
    cdef public bytearray RAM
    cdef public bytearray videoRegisters

    cdef public object MMU
    cdef public object display
    def __init__(self, OTPFile, flashFile):

        
        with open(OTPFile, 'rb') as f:
            self.OTP = bytearray(f.read())

        with open(flashFile, 'rb') as f:
            self.Flash = bytearray(f.read())

        self.STACK_START = 0x100
        self.PRR = 0x32 #controls 4000~7FFF
        self.DRR = 0x34 #controls 8000~FFFF
        self.BRR = 0x36 #controls 2000~3FFF

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

        self.MMU = MMU(self)

        self.Reset()

    def PrintState(self):
        print(f'A:{self.A} X:{self.X} Y:{self.Y} PC:{hex(self.PC)}')

    cpdef public unsigned short GetPRR(self):
        return self.MMU.ReadShort(self.PRR)
    cpdef public unsigned short GetDRR(self):
        return self.MMU.ReadShort(self.DRR)
    cpdef public unsigned short GetBRR(self):
        return self.MMU.ReadShort(self.BRR)

    cpdef public unsigned short GetResetVector(self):
        return self.MMU.ReadShort(0x7FFC)
    
    cpdef public void Reset(self):
        self.i = True
        self.d = False
        self.PC = self.GetResetVector()
        self.display = Display()

    cpdef public unsigned short GetBTVector(self):
        struct.unpack('<H', self.OTP[0x7FEC : 0x7FEF])[0]

    cpdef public void WriteVideoRegister(self, unsigned short address, unsigned char byte):
##        print(f'Video write coming from {hex(self.PC)}')
        address %= 2
        self.videoRegisters[address] = byte
        if address == 0:
            print(f'Command coming from {hex(self.PC)}: ')
            self.display.SendCommand(byte)
        else:
            #print(f'Data coming from {hex(self.PC)}: ', end='')
            self.display.SendData(byte)       
        

    cpdef public unsigned char ImmediateVal(self):
        cpdef unsigned char value = self.MMU.ReadByte(self.PC + 1) #1 byte arg
        return value
    
    cpdef public unsigned char ZeroPagePtr(self):
        cpdef unsigned char pointer = self.MMU.ReadByte(self.PC + 1) #1 byte arg
        return pointer
    cpdef public unsigned char ZeroPageVal(self):
        cpdef unsigned char pointer = self.ZeroPagePtr()
        cpdef unsigned char value = self.MMU.ReadByte(pointer)
        return value
    
    cpdef public unsigned char ZeroPageXPtr(self):
        cpdef unsigned char pointer = self.MMU.ReadByte(self.PC + 1) #1 byte arg
        pointer += self.X
        return pointer
    cpdef public unsigned char ZeroPageXVal(self):
        cpdef unsigned char pointer = self.ZeroPageXPtr()
        cpdef unsigned char value = self.MMU.ReadByte(pointer)
        return value
    
    cpdef public unsigned char ZeroPageYPtr(self):
        cpdef unsigned char pointer = self.MMU.ReadByte(self.PC + 1) #1 byte arg
        pointer += self.Y
        return pointer
    cpdef public unsigned char ZeroPageYVal(self):
        cpdef unsigned char pointer = self.ZeroPageYPtr()
        cpdef unsigned char value = self.MMU.ReadByte(pointer)
        return value

    cpdef public unsigned short RelativePtr(self):
        cpdef signed char offset = struct.unpack('<b', bytes([self.MMU.ReadByte(self.PC + 1)]))[0] #1 byte signed arg
        cpdef unsigned short ptr = offset + self.PC + 2 #instruction size of 2
        return ptr
    cpdef public unsigned char RelativeVal(self):
        cpdef unsigned short ptr = self.RelativePtr()
        cpdef unsigned char value = self.MMU.ReadByte(ptr)
        return value

    cpdef public unsigned short AbsolutePtr(self):
        cpdef unsigned short ptr = self.MMU.ReadShort(self.PC + 1) #2 byte arg
        return ptr
    cpdef public unsigned char AbsoluteVal(self):
        cpdef unsigned short ptr = self.AbsolutePtr()
        cpdef unsigned char value = self.MMU.ReadByte(ptr)
        return value

    cpdef public unsigned short AbsoluteXPtr(self):
        cpdef unsigned short ptr = self.MMU.ReadShort(self.PC + 1) #2 byte arg
        ptr += self.X
        return ptr
    cpdef public unsigned char AbsoluteXVal(self):
        cpdef unsigned short ptr = self.AbsoluteXPtr()
        cpdef unsigned char val = self.MMU.ReadByte(ptr)
        return val
    
    cpdef public unsigned short AbsoluteYPtr(self):
        cpdef unsigned short ptr = self.MMU.ReadShort(self.PC + 1) #2 byte arg
        ptr += self.Y
        return ptr
    cpdef public unsigned char AbsoluteYVal(self):
        cpdef unsigned short ptr = self.AbsoluteYPtr()
        cpdef unsigned char val = self.MMU.ReadByte(ptr)
        return val

    cpdef public unsigned short IndirectPtr(self):
        pointer = self.MMU.ReadShort(self.PC + 1) #2 byte arg
        return pointer
    cpdef public unsigned short IndirectVal(self):
        pointer = self.IndirectPtr()
        val = self.MMU.ReadShort(pointer) #2 byte value
        return val
    
    cpdef public unsigned short IndirectZeroPagePtr(self):
        cpdef unsigned short pointer = self.MMU.ReadByte(self.PC + 1)
        pointer = self.MMU.ReadShort(pointer) #2 byte pointer
        return pointer
    cpdef public unsigned char IndirectZeroPageVal(self):
        cpdef unsigned short pointer = self.IndirectZeroPagePtr()
        cpdef unsigned char value = self.MMU.ReadByte(pointer)
        return value
        
    cpdef public unsigned short IndexedIndirectPtr(self):
        cpdef unsigned char zp_pointer = self.MMU.ReadByte(self.PC + 1)
        zp_pointer += self.X
        
        cpdef unsigned short pointer = self.MMU.ReadShort(pointer) #2 byte pointer
        return pointer
    cpdef public unsigned char IndexedIndirectVal(self):
        cpdef unsigned short pointer = self.IndexedIndirectPtr()
        cpdef unsigned char value = self.MMU.ReadByte(pointer)
        return value
    
    cpdef public unsigned short IndirectIndexedPtr(self):
        cpdef unsigned short pointer = self.MMU.ReadByte(self.PC + 1)
        pointer = self.MMU.ReadByte(pointer) + self.Y
        return pointer
    cpdef public unsigned char IndirectIndexedVal(self):
        cpdef unsigned short pointer = self.IndirectIndexedPtr()
        cpdef unsigned char value = self.MMU.ReadByte(pointer)
        return value
    
    cpdef public unsigned short AbsoluteIndexIndirectPtr(self):
        cpdef unsigned short pointer = self.MMU.ReadShort(self.PC + 1) #2 byte arg
        pointer += self.X
        return pointer
    cpdef public unsigned short AbsoluteIndexIndirectVal(self):
        cpdef unsigned short pointer = self.AbsoluteIndexIndirectPtr()
        cpdef unsigned short value = self.MMU.ReadShort(pointer) #2 byte value
        return value
        
    cpdef public void Push(self, byte):
        self.MMU.StoreMemory(self.STACK_START + self.S, byte)
        self.S += 1

    cpdef public void PushShort(self, unsigned short _short):
        cpdef unsigned char low = _short & 0x00FF
        cpdef unsigned char high = (_short & 0xFF00) >> 8
        self.Push(low)
        self.Push(high)

    cpdef public unsigned char Pop(self):
        self.S -= 1
        return self.MMU.ReadByte(self.STACK_START + self.S)

    cpdef public unsigned short PopShort(self):
        cpdef unsigned char high = self.Pop()
        cpdef unsigned char low = self.Pop()
        cpdef unsigned short _short = low + (high << 8)
        return _short

    cpdef public unsigned char ExportFlags(self):
        cpdef unsigned char flags = 0
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

    cpdef public void ImportFlags(self, flags):
        self.n = (flags & 0b10000000) != 0
        self.v = (flags & 0b01000000) != 0
        self.d = (flags & 0b00001000) != 0
        self.i = (flags & 0b00000100) != 0
        self.z = (flags & 0b00000010) != 0
        self.c = (flags & 0b00000001) != 0

    def Step(self, verbose = False):
        opcode = self.MMU.ReadByte(self.PC)
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

    cpdef public void ASL_ZP(self):
        cpdef unsigned char val = self.ZeroPageVal()
        self.c = (0b10000000 & val) != 0
        val <<= 1
        self.z = val == 0
        self.n = (val & 0b10000000) != 0
        cpdef unsigned short ptr = self.ZeroPagePtr()
        self.MMU.StoreMemory(ptr, val)
        self.PC += 2

    cpdef public void PHP(self):
        cpdef unsigned char flags = self.ExportFlags()
        self.Push(flags)
        self.PC += 1

    cpdef public void ORA_I(self):
        cpdef unsigned char val = self.ImmediateVal()
        self.A |= val
        self.z = self.A == 0
        self.n = (self.A & 0b10000000) != 0
        self.PC += 2

    cpdef public void ASL_ACC(self):
        self.c = (0b10000000 & self.A) != 0
        self.A <<= 1
        self.z = self.A == 0
        self.n = (self.A & 0b10000000) != 0
        self.PC += 1

    cpdef public void ASL_A(self):
        cpdef unsigned char val = self.AbsoluteVal()
        val <<= 1
        self.z = val == 0
        self.n = (val & 0b10000000) != 0
        cpdef unsigned ptr = self.AbsolutePtr()
        self.MMU.StoreMemory(ptr, val)
        self.PC += 3

    cpdef public void ORA_A(self):
        cpdef unsigned char val = self.AbsoluteVal()
        self.A |= val
        self.z = self.A == 0
        self.n = (self.A & 0b10000000) != 0
        self.PC += 3

    cpdef public void BPL(self):
        cpdef unsigned short ptr = self.RelativePtr()
        if not self.n:
            self.PC = ptr
        else:
            self.PC += 2

    cpdef public void CLC(self):
        self.c = False
        self.PC += 1

    cpdef public void JSR(self):
        cpdef unsigned short arg = self.AbsolutePtr()
        cpdef unsigned short return_addr = self.PC + 2
        self.PushShort(return_addr)
        self.PC = arg

    cpdef public void BIT_ZP(self):
        cpdef unsigned char arg = self.ZeroPageVal()
        result = arg & self.A
        self.z = result == 0
        self.v = (arg & 0b01000000) != 0
        self.n = (arg & 0b10000000) != 0
        self.PC += 2

    cpdef public void ROL_ZP(self):
        cpdef unsigned char val = self.ZeroPageVal()
        cpdef bool old_carry = self.c
        self.c = (val & 0b10000000) != 0
        val <<= 1
        val += int(old_carry)
        cpdef unsigned short ptr = self.ZeroPagePtr()
        self.MMU.StoreMemory(ptr, val)
        self.PC += 2

    cpdef public void PLP(self):
        cpdef unsigned char flags = self.Pop()
        self.ImportFlags(flags)
        self.PC += 1

    cpdef public void AND_I(self):
        cpdef unsigned char arg = self.ImmediateVal()
        self.A &= arg
        self.z = self.A == 0
        self.n = (self.A & 0b10000000) != 0
        self.PC += 2

    cpdef public void ROL_ACC(self):
        cpdef bool old_carry = self.c
        self.c = (self.A & 0b10000000) != 0
        self.A <<= 1
        self.A += int(old_carry)
        self.PC += 1

    cpdef public void ROL_A(self):
        cpdef unsigned char val = self.AbsoluteVal()
        cpdef bool old_carry = self.c
        self.c = (val & 0b10000000) != 0
        val <<= 1
        val += int(old_carry)
        cpdef unsigned short ptr = self.AbsolutePtr()
        self.MMU.StoreMemory(ptr, val)
        self.PC += 3 

    cpdef public void AND_A(self):
        cpdef unsigned char val = self.AbsoluteVal()
        self.A &= val
        self.z = self.A == 0
        self.n = (self.A & 0b10000000) != 0
        self.PC += 3
        
    cpdef public void SEC(self):
        self.c = True
        self.PC += 1

    cpdef public void EOR_ZP(self):
        cpdef unsigned char val = self.ZeroPageVal()
        self.A ^= val
        self.z = self.A == 0
        self.n = (self.A & 0b10000000) != 0
        self.PC += 2

    def LSR_ZP(self):
        val = self.ZeroPageVal()
        self.c = (val & 0b00000001) != 0
        val >>= 1
        self.z = val == 0
        self.n = (val & 0b10000000) != 0
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
        self.n = (self.A & 0b10000000) != 0
        self.PC += 2

    def LSR_ACC(self):
        self.c = (0b00000001 & self.A) != 0
        self.A >>= 1
        self.z = self.A == 0
        self.n = (self.A & 0b10000000) != 0
        self.PC += 1

    def JMP_A(self):
        arg = self.AbsolutePtr()
        self.PC = arg

    def EOR_A(self):
        val = self.AbsoluteVal()
        self.A ^= val
        self.z = self.A == 0
        self.n = (self.A & 0b10000000) != 0
        self.PC += 3

    def LSR_A(self):
        val = self.AbsoluteVal()
        self.c = (0b00000001 & val) != 0
        val >>= 1
        self.z = val == 0
        self.n = (val & 0b10000000) != 0
        ptr = self.AbsolutePtr()
        self.MMU.StoreMemory(ptr, val)
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
        self.MMU.StoreMemory(ptr, 0)
        self.PC += 2

    def ADC_ZP(self):
        add = self.ZeroPageVal()
        self.A += add + int(self.c)
        self.c = self.A > 0xFF
        self.A &= 0xFF
        self.z = self.A == 0
        self.n = (self.A & 0b10000000) != 0
        self.PC += 2

    def ROR_ZP(self):
        val = self.ZeroPageVal()
        old_carry = self.c
        self.c = (val & 0b00000001) != 0
        val >>= 1
        if old_carry:
            val |= 0b10000000
        else:
            val &= 0b01111111
        self.n = (val & 0b10000000) != 0
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
        self.n = (self.A & 0b10000000) != 0
        self.PC += 1

    def ADC_I(self):
        add = self.ImmediateVal()
        self.A += add + int(self.c)
        self.c = self.A > 0xFF
        self.A &= 0xFF
        self.z = self.A == 0
        self.n = (self.A & 0b10000000) != 0
        self.PC += 2

    def ROR_ACC(self):
        old_carry = self.c
        self.c = (self.A & 0b00000001) != 0
        self.A >>= 1
        if old_carry:
            self.A |= 0b10000000
        else:
            self.A &= 0b01111111
        self.n = (self.A & 0b10000000) != 0
        self.PC += 1
        
    def JMP_I(self):
        arg = self.IndirectVal()
        self.PC = arg

    def ROR_A(self):
        val = self.AbsoluteVal()
        old_carry = self.c
        self.c = (val & 0b00000001) != 0
        val >>= 1
        if old_carry:
            val |= 0b10000000
        else:
            val &= 0b01111111
        self.n = (val & 0b10000000) != 0
        ptr = self.AbsolutePtr()
        self.MMU.StoreMemory(ptr, val)
        self.PC += 3
        
    def ADC_A(self):
        add = self.AbsoluteVal()
        self.A += add + int(self.c)
        self.c = self.A > 0xFF
        self.A &= 0xFF
        self.z = self.A == 0
        self.n = (self.A & 0b10000000) != 0
        self.PC += 3

    def SEI(self):
        self.i = True
        self.PC += 1

    def PLY(self):
        self.Y = self.Pop()
        self.z = self.Y == 0 #reference says A
        self.n = (self.Y & 0b10000000) != 0 #reference says A
        self.PC += 1

    def JMP_ABSOLUTE_INDEXED_INDIRECT(self):
        arg = self.AbsoluteIndexIndirectVal()
        self.PC = arg

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
        self.n = (self.Y & 0b10000000) != 0
        self.PC += 1

    def STX_A(self):
        ptr = self.AbsolutePtr()
        self.MMU.StoreMemory(ptr, self.X)
        self.PC += 3

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
        self.n = (self.A & 0b10000000) != 0
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
        self.n = (self.Y & 0b10000000) != 0
        self.PC += 2

    def LDA_INDEXED_INDIRECT(self):
        val = self.IndexedIndirectVal()
        self.A = val
        self.z = self.A == 0
        self.n = (self.A & 0b10000000) != 0
        self.PC += 2

    def LDX_I(self):
        val = self.ImmediateVal()
        self.X = val
        self.z = self.X == 0
        self.n = (self.X & 0b10000000) != 0
        self.PC += 2

    def LDA_ZP(self):
        val = self.ZeroPageVal()
        self.A = val
        self.z = self.A == 0
        self.n = (self.A & 0b10000000) != 0
        self.PC += 2

    def LDX_ZP(self):
        val = self.ZeroPageVal()
        self.X = val
        self.z = self.X == 0
        self.n = (self.X & 0b10000000) != 0
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
        self.n = (self.Y & 0b10000000) != 0
        self.PC += 1

    def LDA_I(self):
        arg = self.ImmediateVal()
        self.A = arg
        self.z = self.A == 0
        self.n = (self.A & 0b10000000) != 0
        self.PC += 2

    def LDA_A(self):
        val = self.AbsoluteVal()
        self.A = val
        self.z = self.A == 0
        self.n = (self.A & 0b10000000) != 0
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
        self.n = (self.A & 0b10000000) != 0
        self.PC += 2

    def LDA_IZP(self):
        val = self.IndirectZeroPageVal()
        ptr = self.IndirectZeroPagePtr()
        self.A = val
        self.z = self.A == 0
        self.n = (self.A & 0b10000000) != 0
        self.PC += 2

    def LDA_ZPX(self):
        val = self.ZeroPageXVal()
        self.A = val
        self.z = self.A == 0
        self.n = (self.A & 0b10000000) != 0
        self.PC += 2

    def LDA_AY(self):
        val = self.AbsoluteYVal()
        self.A = val
        self.z = self.A == 0
        self.n = (self.A & 0b10000000) != 0
        self.PC += 3

    def LDY_AX(self):
        val = self.AbsoluteXVal()
        self.Y = val
        self.z = self.Y == 0
        self.n = (self.Y & 0b10000000) != 0
        self.PC += 3

    def LDA_AX(self):
        val = self.AbsoluteXVal()
        self.A = val
        self.z = self.Y == 0
        self.n = (self.A & 0b10000000) != 0
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
        self.n = (self.Y & 0b10000000) != 0
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
        self.n = (self.X & 0b10000000) != 0
        self.PC += 1

    def WAI(self):#NOT IMPLEMENTED AT ALL
        self.PC += 1

    def TAX(self):
        self.X = self.A
        self.z = self.X == 0
        self.n = (self.X & 0b10000000) != 0
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
        self.n = (val & 0b10000000) != 0
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
        self.n = (val & 0b10000000) != 0
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
        self.n = (val & 0b10000000) != 0
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
        self.n = (val & 0b10000000) != 0
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
        self.n = (self.X & 0b10000000) != 0
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
        self.n = (val & 0b10000000) != 0
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
        self.n = (val & 0b10000000) != 0
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
        self.n = (self.X & 0b10000000) != 0 #Reference says A
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
        0x4D: CPU.EOR_A,
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
        0x6A: CPU.ROR_ACC,
        0x6C: CPU.JMP_I,
        0x6E: CPU.ROR_A,
        0x6D: CPU.ADC_A,
        0x78: CPU.SEI,
        0x7A: CPU.PLY,
        0x7C: CPU.JMP_ABSOLUTE_INDEXED_INDIRECT,
        0x80: CPU.BRA,
        0x8D: CPU.STA_A,
        0x85: CPU.STA_ZP,
        0x87: CPU.SMB0_ZP,
        0x88: CPU.DEY,
        0x8E: CPU.STX_A,
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
        
