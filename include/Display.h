#ifndef DISPLAY_H
#define DISPLAY_H

#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include <iostream>

#include <types.h>

#define HEIGHT 67
#define WIDTH 98
#define SCALE 5


class Display {
    public:
    bool RedNext;
    u8 RByte;
    u8 GBByte;
    unsigned int pixelPosition;
    unsigned int width;
    unsigned int height;
    unsigned int scale;
    bool inverse;
    unsigned char command;

    unsigned int maxPixelPosition;
    unsigned int lastUpdateTime;

    //SDL
    SDL_Event event;
    SDL_Window *window;
    SDL_Renderer *renderer;

    Display();
    bool Update();
    void DrawRectangle(u8 r, u8 g, u8 b, unsigned int x, unsigned int y, unsigned int rectWidth, unsigned int rectHeight);
    void SendCommand(u8 by);
    void SendData(u8 by);
    void AddPixelData(u8 by);
    void DrawCurrentPixel();
    void Splash();
};



#endif // DISPLAY_H
