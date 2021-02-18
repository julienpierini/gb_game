#include <gb/gb.h>


struct character {
    UBYTE spriteId;
    UBYTE leftSprite[6];
    UBYTE rightSprite[6];
    UINT8 x;
    UINT8 y;
    UINT8 width;
    UINT8 height;
    UINT8 direction;
};

// right/left
// gameCharacter->leftSprite[0] = 0;  // 0-1
// gameCharacter->rightSprite[0] = 2; // 2-3
// front
// gameCharacter->leftSprite[1] = 4;  // 4-5
// gameCharacter->rightSprite[1] = 6; // 6-7
// back
// gameCharacter->leftSprite[2] = 8;  // 8-9
// gameCharacter->rightSprite[2] = 10; // 10-11

// rigth/left anime
// gameCharacter->leftSprite[3] = 12;  // 12-13
// gameCharacter->rightSprite[3] = 14; // 14-15
// front anime
// gameCharacter->leftSprite[4] = 16;  // 16-17
// gameCharacter->rightSprite[4] = 18; // 18-19
// back anime
// gameCharacter->leftSprite[5] = 20;  // 20-21
// gameCharacter->rightSprite[5] = 22; // 22-23