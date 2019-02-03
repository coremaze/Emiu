import pygame
pygame.init()
class Controller():
    def __init__(self, CPU):
        self.CPU = CPU
        self.controls1 = 0xFF
        self.controls2 = 0xFF
        
    def Update(self):
        controls1 = 0b11111111
        UP =    0b11111110
        DOWN =  0b11111101
        LEFT =  0b11111011
        RIGHT = 0b11110111
        POWER = 0b11101111
        MENU =  0b11011111

        keys=pygame.key.get_pressed()
        if keys[pygame.K_RIGHT]:
            controls1 &= RIGHT
        if keys[pygame.K_LEFT]:
            controls1 &= LEFT
        if keys[pygame.K_DOWN]:
            controls1 &= DOWN
        if keys[pygame.K_UP]:
            controls1 &= UP
        if keys[pygame.K_RCTRL]:
            controls1 &= MENU
        if keys[pygame.K_p]:
            controls1 &= POWER

        
        controls2 = 0b11111111
        SCREEN_TOP_LEFT =      0b11111110
        SCREEN_TOP_RIGHT =     0b11111101
        SCREEN_BOTTOM_LEFT =   0b11111011
        SCREEN_BOTTOM_RIGHTT = 0b11110111
        
        if keys[pygame.K_q]:
            controls2 &= SCREEN_TOP_LEFT
        if keys[pygame.K_w]:
            controls2 &= SCREEN_TOP_RIGHT
        if keys[pygame.K_s]:
            controls2 &= SCREEN_BOTTOM_LEFT
        if keys[pygame.K_a]:
            controls2 &= SCREEN_BOTTOM_RIGHTT
        


        if (controls1 != self.controls1) or (controls2 != self.controls2):
            self.CPU.PTInterrupt.Trigger()

        self.controls1 = controls1
        self.controls2 = controls2

        self.CPU.MMU.StoreMemory(0, self.controls1)
        self.CPU.MMU.StoreMemory(1, self.controls2)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                exit()
