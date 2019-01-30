import time
##from threading import Thread

BTEN = 0x2A
BTREQ = 0x2B
BTC = 0x2C
IREQL = 0x3C
IENAL = 0x3E


##def Start_BT(CPU):
##    Thread(target=BT, args=[CPU]).start()
##    
##def BT(CPU):
##    wait_time = 1/8192 #8192 Hz
##    timer = 0
##    while True:
##        time.sleep(0.5)
##        BTEN_data = CPU.memRegisters[BTEN] #bits are 1 for enabled
##        BTREQ_data = CPU.memRegisters[BTREQ]
##        BTREQ_data &= 1 #2Hz
##        
##        BTREQ_data &= ~BTEN_data
##
##        CPU.memRegisters[BTREQ] = BTREQ_data
##
##        CPU.BTInterrupt = True
##        initial_time = time.time()
##        while True:
##            final_time = time.time()
##            if final_time-initial_time >= wait_time:
##                break
##        timer += 1
##        timer %= 8192
##
##        BTEN_data = CPU.memRegisters[BTEN] #bits are 1 for enabled
##
##        BTREQ_data = CPU.memRegisters[BTREQ]
##        if timer % 4096 == 0: #2Hz
##            BTREQ_data &= 0b1
##        if timer % 256 == 0: #32 Hz
##            BTREQ_data &= 0b10
##        if timer % 256 == 0: #64 Hz
##            BTREQ_data &= 0b100
##        if timer % 64 == 0: #128 Hz
##            BTREQ_data &= 0b1000
##        if timer % 32 == 0: #256 Hz
##            BTREQ_data &= 0b10000
##        if timer % 16 == 0: #512 Hz
##            BTREQ_data &= 0b100000
##        if timer % 4 == 0: #2048 Hz
##            BTREQ_data &= 0b1000000
##        BTREQ_data &= 0b10000000 #8192 Hz
##
##        BTREQ_data &= ~BTEN_data
##
##        CPU.BT_interrupt = True
        

        
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
        duration = this_time - self.last_time
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
#These are not implemented because they seem to not be used.
##        if old_clock // 256 < self.clock // 256: #32 Hz
##            BTREQ_data |= 0b10
##        if old_clock // 128 < self.clock // 128: #64 Hz
##            BTREQ_data |= 0b100
##        if old_clock // 64 < self.clock // 64: #128 Hz
##            BTREQ_data |= 0b1000
##        if old_clock // 32 < self.clock // 32: #256 Hz
##            BTREQ_data |= 0b10000
##        if old_clock // 16 < self.clock // 16: #512 Hz
##            BTREQ_data |= 0b100000
##        if old_clock // 4 < self.clock // 4: #2048 Hz
##            BTREQ_data |= 0b1000000
##        BTREQ_data |= 0b10000000 #8192 Hz

        BTREQ_data &= BTEN_data
        self.CPU.memRegisters[BTREQ] = BTREQ_data

        if BTREQ_data:
            #print(bin(BTEN_data), bin(BTREQ_data))
            return True
        
        return False

