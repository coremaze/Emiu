class DMA():
    DPTR = 0x58
    DBKR = 0x5A
    DCNT = 0x5C
    DSEL = 0x5E
    DMOD = 0x5F
    def __init__(self, cpu):
        self.CPU = cpu
        self.sourcePtr = 0
        self.destPtr = 0
        self.sourceBank = 0
        self.destBank = 0
        self.count = 0
        
    def SetDPTRL(self, byte):
        DSEL_value = self.CPU.MMU.ReadMemory(self.DSEL)

        if DSEL_value & 1: #select destination
            self.destPtr = (self.destPtr & 0xFF00) | byte
        else: #select source
            self.sourcePtr = (self.sourcePtr & 0xFF00) | byte

    def GetDPTRL(self):
        DSEL_value = self.CPU.MMU.ReadMemory(self.DSEL)
        
        if DSEL_value & 1: #select destination
            return self.destPtr & 0xFF
        else: #select source
            return self.sourcePtr & 0xFF

    def SetDPTRH(self, byte):
        DSEL_value = self.CPU.MMU.ReadMemory(self.DSEL)

        if DSEL_value & 1: #select destination
            self.destPtr = (self.destPtr & 0x00FF) | (byte << 8) | 0x8000 #highest bit always set
        else: #select source
            self.sourcePtr = (self.sourcePtr & 0x00FF) | (byte << 8) | 0x8000
        
    def GetDPTRH(self):
        DSEL_value = self.CPU.MMU.ReadMemory(self.DSEL)
        
        if DSEL_value & 1: #select destination
            return (self.destPtr & 0xFF00) >> 8
        else: #select source
            return (self.sourcePtr & 0xFF00) >> 8

    def SetDBKRL(self, byte):
        DSEL_value = self.CPU.MMU.ReadMemory(self.DSEL)

        if DSEL_value & 1: #select destination
            self.destBank = (self.destBank & 0xFF00) | byte
        else: #select source
            self.sourceBank = (self.sourceBank & 0xFF00) | byte

    def GetDBKRL(self):
        DSEL_value = self.CPU.MMU.ReadMemory(self.DSEL)

        if DSEL_value & 1: #select destination
            return self.destBank & 0xFF
        else: #select source
            return self.sourceBank & 0xFF

    def SetDBKRH(self, byte):
        DSEL_value = self.CPU.MMU.ReadMemory(self.DSEL)

        if DSEL_value & 1: #select destination
            self.destBank = (self.destBank & 0x00FF) | (byte << 8)
        else: #select source
            self.sourceBank = (self.sourceBank & 0x00FF) | (byte << 8)

    def GetDBKRH(self):
        DSEL_value = self.CPU.MMU.ReadMemory(self.DSEL)

        if DSEL_value & 1: #select destination
            return (self.destBank & 0xFF00) >> 8
        else: #select source
            return (self.sourceBank & 0xFF00) >> 8

    def SetDCNTL(self, byte):
        self.count = (self.count & 0xFF00) | byte

    def GetDCNTL(self):
        return self.count & 0xFF
    
    def SetDCNTH(self, byte):
        self.count = (self.count & 0x00FF) | (byte << 8)
        self.Execute()

    def GetDCNTH(self):
        return (self.count & 0xFF00) >> 8

    def Execute(self):
        old_DRR = self.CPU.GetDRR()
        DMOD_value = self.CPU.MMU.ReadMemory(self.DMOD)
        source_mode = DMOD_value & 0b000011
        dest_mode = (DMOD_value & 0b001100) >> 2
        func = (DMOD_value & 0b110000) >> 4

        DCNTL_value = self.CPU.MMU.ReadMemory(self.DCNT)
        DCNTH_value = self.CPU.MMU.ReadMemory(self.DCNT+1)
        count = self.count

        old_sourcePtr = self.sourcePtr
        old_destPtr = self.destPtr

        for _ in range(count+1):
            source_DRRL = self.sourceBank & 0xFF
            source_DRRH = (self.sourceBank & 0xFF00) >> 8
            self.CPU.MMU.StoreMemory(self.CPU.DRR, source_DRRL)
            self.CPU.MMU.StoreMemory(self.CPU.DRR+1, source_DRRH)

            source_byte = self.CPU.MMU.ReadMemory(self.sourcePtr)

            dest_DRRL = self.destBank & 0xFF
            dest_DRRH = (self.destBank & 0xFF00) >> 8
            self.CPU.MMU.StoreMemory(self.CPU.DRR, dest_DRRL)
            self.CPU.MMU.StoreMemory(self.CPU.DRR+1, dest_DRRH)

            dest_byte = self.CPU.MMU.ReadMemory(self.destPtr)

            #FUNC
            if func == 0b00: #normal mode
                new_byte = source_byte
            elif func == 0b01: #XOR mode
                new_byte = source_byte ^ dest_byte
            elif func == 0b10: #OR mode
                new_byte = source_byte | dest_byte
            elif func == 0b11: #AND mode
                new_byte = source_byte & dest_byte

            self.CPU.MMU.StoreMemory(self.destPtr, new_byte)

            #print(f'{source_mode:X} {dest_mode:X} {count:X} {func:X}')
            #Pointer modes
            if source_mode == 0b00: #Continue mode
                self.sourcePtr += 1
                self.sourcePtr = (self.sourcePtr & 0xFFFF) | 0x8000
            elif source_mode == 0b01: #Reload mode
                self.sourcePtr += 1
                self.sourcePtr = (self.sourcePtr & 0xFFFF) | 0x8000
            else: #Fixed mode
                pass

            if dest_mode == 0b00: #Continue mode
                self.destPtr += 1
                self.destPtr = (self.destPtr & 0xFFFF) | 0x8000
            elif dest_mode == 0b01: #Reload mode
                self.destPtr += 1
                self.destPtr = (self.destPtr & 0xFFFF) | 0x8000
            else: #Fixed mode
                pass

        old_DRRL = old_DRR & 0xFF
        old_DRRH = (old_DRR & 0x00FF) >> 8
        self.CPU.MMU.StoreMemory(self.CPU.DRR, old_DRRL)
        self.CPU.MMU.StoreMemory(self.CPU.DRR+1, old_DRRH)

        if source_mode == 0b01:
            self.sourcePtr = old_sourcePtr
        if dest_mode == 0b01: #Reload mode
            self.destPtr = old_destPtr
