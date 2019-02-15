#include "Controller.h"
#include "CPU.h"
#include "MMU.h"
#include "Interrupt.h"

#define UP                   0b1111111111111110
#define DOWN                 0b1111111111111101
#define LEFT                 0b1111111111111011
#define RIGHT                0b1111111111110111
#define POWER                0b1111111111101111
#define MENU                 0b1111111111011111
#define UPSIDE_UP            0b1111111110111111
#define UPSIDE_DOWN          0b1111111101111111
#define SCREEN_TOP_LEFT      0b1111111011111111
#define SCREEN_TOP_RIGHT     0b1111110111111111
#define SCREEN_BOTTOM_LEFT   0b1111101111111111
#define SCREEN_BOTTOM_RIGHT  0b1111011111111111
#define ACTION               0b1110111111111111
#define MUTE                 0b1101111111111111


Controller::Controller(CPU* cpu){
    this->cpu = cpu;
    this->controls = 0xFFFF;
}

void Controller::Update(){
    const uint8_t *state = SDL_GetKeyboardState(NULL);

    unsigned short keys = 0b1111111111111111;

    if (state[SDL_SCANCODE_RIGHT]){
        keys &= RIGHT;
    }
    if (state[SDL_SCANCODE_LEFT]){
        keys &= LEFT;
    }
    if (state[SDL_SCANCODE_DOWN]){
        keys &= DOWN;
    }
    if (state[SDL_SCANCODE_UP]){
        keys &= UP;
    }
    if (state[SDL_SCANCODE_RCTRL]){
        keys &= MENU;
    }
    if (state[SDL_SCANCODE_P]){
        keys &= POWER;
    }
    if (state[SDL_SCANCODE_A]){
        keys &= ACTION;
    }
    if (state[SDL_SCANCODE_PAGEUP]){
        keys &= SCREEN_TOP_RIGHT;
    }
    if (state[SDL_SCANCODE_PAGEDOWN]){
        keys &= SCREEN_BOTTOM_RIGHT;
    }
    if (state[SDL_SCANCODE_HOME]){
        keys &= SCREEN_TOP_LEFT;
    }
    if (state[SDL_SCANCODE_END]){
        keys &= SCREEN_BOTTOM_LEFT;
    }
    if (state[SDL_SCANCODE_M]){
        keys &= MUTE;
    }

    if (this->controls != keys){
        this->cpu->ptInterrupt->Trigger();
    }

    this->controls = keys;

    this->cpu->mmu->StoreShort(0, this->controls);
}
