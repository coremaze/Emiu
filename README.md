# Emiu

Emiu is an emulator for the Miuchiz handheld console.

Microprocessor documentation: http://meuk.spritesserver.nl/st2205um.pdf

LCD controller documentation: https://www.crystalfontz.com/controllers/Sitronix/ST7626/

Flash documentation: http://ww1.microchip.com/downloads/en/devicedoc/25040a.pdf

## Compiling

```
mkdir build && cd build
cmake [-DCMAKE_TOOLCHAIN_FILE="../mingw32.cmake"] .. && make
```
