import pygame
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
