# Emiu

Emiu is an emulator for the Miuchiz handheld console.

Microprocessor documentation: http://meuk.spritesserver.nl/st2205um.pdf

LCD controller documentation: https://www.crystalfontz.com/controllers/Sitronix/ST7626/

Flash documentation: http://ww1.microchip.com/downloads/en/devicedoc/25040a.pdf

## Compiling

### Compiler flags

Have g++ follow the C++14 ISO C++ language standard: `-std=c++14`

### Linker libraries

`Emiu\SDL2\lib\libSDL2.dll.a`

### Other linker options

`-lwinmm`
