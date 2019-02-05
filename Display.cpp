#include "Display.h"

Display::Display(){
    this->RedNext = true;
    this->RByte = 0;
    this->GBByte = 0;
    this->pixelPosition = 0;
    this->width = WIDTH;
    this->height = HEIGHT;
    this->scale = SCALE;
    this->inverse = false;

    this->command = 0;

    this->maxPixelPosition = this->width * this->height;


    SDL_Init(SDL_INIT_EVERYTHING);
    this->window = SDL_CreateWindow("Miuchiz", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->width*this->scale, this->height*this->scale, SDL_WINDOW_SHOWN);
    this->renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(this->renderer, 0, 255, 0, 255);
    SDL_RenderClear(this->renderer);
}

bool Display::Update(){
    SDL_RenderPresent(this->renderer);
    if (SDL_PollEvent(&this->event) && this->event.type == SDL_QUIT){
        return true;
    }
    return false;
}

void Display::DrawRectangle(BYTE r, BYTE g, BYTE b, unsigned int x, unsigned int y, unsigned int rectWidth, unsigned int rectHeight){
    SDL_SetRenderDrawColor(this->renderer, 0, r, g, b);
    for (unsigned int i=x; i<(x+rectWidth); i++){
        for (unsigned int j=y; j<(y+rectHeight); j++){
            SDL_RenderDrawPoint(this->renderer, i, j);
        }
    }
}

void Display::SendCommand(BYTE by){
    this->command = by;
    if (this->command == 0x75) { //PASET
        this->pixelPosition = 0;
    }
    else if (this->command == 0x15) { //CASET
        this->pixelPosition = 0;
    }
    else if (this->command == 0xA6) { //Normal Display
        this->inverse = false;
    }
    else if (this->command == 0xA7) { //Inverse display
        this->inverse = true;
    }
}

void Display::SendData(BYTE by) {
    if (this->command == 0x5C) { //Memory Write (RAMWR)
        this->AddPixelData(by);
    }
    else {
        printf("Unused data for command %02X: %02X\n", (unsigned int)this->command, (unsigned int)by);
    }
}

void Display::AddPixelData(BYTE by){
    if (this->RedNext) {
        this->RByte = by;
    }
    else {
        this->GBByte = by;
        this->DrawCurrentPixel();
    }
    this->RedNext = !this->RedNext;
}

void Display::DrawCurrentPixel() {
    unsigned int x = this->pixelPosition % this->width;
    unsigned int y = (this->pixelPosition - x) / this->width;

    BYTE r = this->RByte & 0x0F;
    r = r * 17;
    BYTE g = ((this->GBByte & 0xF0) >> 4) * 17;
    BYTE b = (this->GBByte & 0x0F) * 17;

    if (!this->inverse) {
        r = 255 - r;
        g = 255 - g;
        b = 255 - b;
    }

    this->DrawRectangle(r, g, b, x*this->scale, y*this->scale, this->scale, this->scale);

    this->pixelPosition++;

    if (this->pixelPosition >= this->maxPixelPosition) {
        this->pixelPosition = 0;
    }
}

