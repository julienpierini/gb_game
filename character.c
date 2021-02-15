#include <gb/gb.h>


struct character {
    UBYTE spritesNumber[2];
    UBYTE leftSprite[7];
    UBYTE rightSprite[7];
    UINT8 x;
    UINT8 y;
    UINT8 width;
    UINT8 height;
    UINT8 direction;
};