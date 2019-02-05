#ifndef DISPLAY_H
#define DISPLAY_H

#define SDL_MAIN_HANDLED
#include "SDL2/include/SDL2/SDL.h"
#include <windows.h>
#define BYTE unsigned char
#include <iostream>
#define HEIGHT 67
#define WIDTH 98
#define SCALE 4


class Display {
    public:
    bool RedNext;
    BYTE RByte;
    BYTE GBByte;
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
    void DrawRectangle(BYTE r, BYTE g, BYTE b, unsigned int x, unsigned int y, unsigned int rectWidth, unsigned int rectHeight);
    void SendCommand(BYTE by);
    void SendData(BYTE by);
    void AddPixelData(BYTE by);
    void DrawCurrentPixel();
};



#endif // DISPLAY_H
