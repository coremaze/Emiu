import time

BTEN = 0x2A
BTREQ = 0x2B
BTC = 0x2C
IREQL = 0x3C
IENAL = 0x3E

 
class Interrupt():
    def __init__(self, CPU):
        raise NotImplemented("")
    def Update(self):
        raise NotImplemented("")

class BTInterrupt(Interrupt):
    def __init__(self, CPU):
        self.CPU = CPU
        self.clock = 0
        self.last_time = time.time()
    def Update(self):
        this_time = time.time()
        duration = (this_time - self.last_time)
        self.last_time = this_time
        cycles = int(duration * 8192)
        old_clock = self.clock
        self.clock += cycles
        
        BTEN_data = self.CPU.memRegisters[BTEN] #bits are 1 for enabled

        BTREQ_data = self.CPU.memRegisters[BTREQ]

        #Literally simulating an oscillating crystal
        #is not going to happen, so this checks if
        #an interrupt should have been triggered between
        #this update and last update.
        
        if old_clock // 4096 < self.clock // 4096: #2Hz
            BTREQ_data |= 0b1
        if old_clock // 256 < self.clock // 256: #32 Hz
            BTREQ_data |= 0b10
        if old_clock // 128 < self.clock // 128: #64 Hz
            BTREQ_data |= 0b100
        if old_clock // 64 < self.clock // 64: #128 Hz
            BTREQ_data |= 0b1000
        if old_clock // 32 < self.clock // 32: #256 Hz
            BTREQ_data |= 0b10000
        if old_clock // 16 < self.clock // 16: #512 Hz
            BTREQ_data |= 0b100000
        if old_clock // 4 < self.clock // 4: #2048 Hz
            BTREQ_data |= 0b1000000
        BTREQ_data |= 0b10000000 #8192 Hz

        BTREQ_data &= BTEN_data
        self.CPU.memRegisters[BTREQ] = BTREQ_data

        if BTREQ_data:
            #print(bin(BTEN_data), bin(BTREQ_data))
            return True
        
        return False

class PTInterrupt(Interrupt):
    def __init__(self):
        self.triggered = False
    def Update(self):
        return False #disable for now
        if self.triggered: #todo: check if enabled
            self.triggered = False
            return False
        return False
        

