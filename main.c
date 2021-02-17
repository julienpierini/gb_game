#include <gb/gb.h>
#include <bcd.h>
#include <gb/font.h>
#include <stdio.h>
#include "character.c"
#include "assets/map.c"
#include "assets/tiles.c"
#include "assets/player3.c"

/* screen size */
#define MIN_SX		  0U		        /* min x (char) */
#define MAX_SX		  mapWidth          /* max x (char) */
#define MIN_SY		  0U		        /* min y (char) */
#define MAX_SY		  mapHeight	        /* max y (char) */

/* sprites */
#define SP_H_DEF	  0U		        /* sprite horizontal 0° rotation */
#define SP_H_FLIP	  32U		        /* sprite horizontal 180° rotation */
#define TL_SIZE       8U		        /* sprite size length = height = 8 */

/* tiles */
#define TL_PL_NB       24U               /* number of player tiles to load in VRAM */
#define TL_BKG_NB      32U              /* number of background tiles to load in VRAM */

/* screen */
#define MID_X		   90U		        /* mid x pos (dot) */
#define MID_Y		   70U              /* mid y pos (dot) */

/* scrolling */
#define PXL_SC          1U              /* nb of pixels to scroll when player is moving */

extern clock_t clock();

struct character player;

font_t min_font;

UBYTE character_direct;
UBYTE tile_size, tile_init;
UBYTE bkg_x_pos, bkg_y_pos;
UBYTE scrolling_speed;
UBYTE key, last_key = 0;
UINT16 s;
BOOLEAN tile_flip, reverse_anime = FALSE;
int n = 0, frame = 0, anime = 0, last_anime = 0;

UINT16 get_s(){
    return (clock()/60);
}

void print_time(){
    printf("%d", s);
}

void init_background(){
    set_bkg_data(37, TL_BKG_NB, background);
    set_bkg_tiles(0, 0, mapWidth, mapHeight, map);
    scrolling_speed = PXL_SC;
}

void init_player(struct character* gameCharacter, int firstTileLoaded, int tileRangeLoaded, int x, int y){
    set_sprite_data(firstTileLoaded, tileRangeLoaded, player3);

    // right
    gameCharacter->leftSprite[0] = 0;  // 0-1
    gameCharacter->rightSprite[0] = 2; // 2-3
    // left
    gameCharacter->leftSprite[1] = 0;  // 0-1
    gameCharacter->rightSprite[1] = 2; // 2-3
    // front
    gameCharacter->leftSprite[2] = 4;  // 4-5
    gameCharacter->rightSprite[2] = 6; // 6-7
    // back
    gameCharacter->leftSprite[3] = 8;  // 8-9
    gameCharacter->rightSprite[3] = 10; // 10-11

    // rigth/left anime
    gameCharacter->leftSprite[4] = 12;  // 12-13
    gameCharacter->rightSprite[4] = 14; // 14-15
    // front anime
    gameCharacter->leftSprite[5] = 16;  // 16-17
    gameCharacter->rightSprite[5] = 18; // 18-19
    // back anime
    gameCharacter->leftSprite[6] = 20;  // 20-21
    gameCharacter->rightSprite[6] = 22; // 22-23

    gameCharacter->x = x;
    gameCharacter->y = y;
    
    // init front direction
    gameCharacter->spritesNumber[0] = 0;
    gameCharacter->spritesNumber[1] = 1;
    set_sprite_tile(gameCharacter->spritesNumber[0], gameCharacter->leftSprite[2]);
    set_sprite_tile(gameCharacter->spritesNumber[1], gameCharacter->rightSprite[2]);
    move_sprite(gameCharacter->spritesNumber[0], gameCharacter->x, gameCharacter->y );
    move_sprite(gameCharacter->spritesNumber[1], gameCharacter->x + TL_SIZE, gameCharacter->y );
}

void rotate_character_tiles(struct character* gameCharacter, int character_animation, BOOLEAN tile_flip){
    // flip tiles
    if(tile_flip){
        set_sprite_tile(gameCharacter->spritesNumber[1], gameCharacter->leftSprite[character_animation]);
        set_sprite_tile(gameCharacter->spritesNumber[0], gameCharacter->rightSprite[character_animation]);
        set_sprite_prop(gameCharacter->spritesNumber[0], SP_H_FLIP);
        set_sprite_prop(gameCharacter->spritesNumber[1], SP_H_FLIP);
    }
    else {
        set_sprite_tile(gameCharacter->spritesNumber[0], gameCharacter->leftSprite[character_animation]);
        set_sprite_tile(gameCharacter->spritesNumber[1], gameCharacter->rightSprite[character_animation]);
        set_sprite_prop(gameCharacter->spritesNumber[0], SP_H_DEF);
        set_sprite_prop(gameCharacter->spritesNumber[1], SP_H_DEF);    
    }
}

void reset_character_pos(UBYTE key, UBYTE last_key, struct character* gameCharacter){
    // if character stop moving
    // right  = 1
    if(key == 0 && last_key == 1){
        character_direct = 0;
        tile_flip = FALSE;
    }
    // left = 2
    else if(key == 0 && last_key == 2){
        character_direct = 0;
        tile_flip = TRUE;
    }
    // up    = 4 (back)
    else if(key == 0 && last_key == 4){
        character_direct = 3;
        tile_flip = FALSE;
    }
    // down  = 8 (front)
    else if(key == 0 && last_key == 8){
        character_direct = 2;
        tile_flip = FALSE;
    }
    // if the character change his direction
    // right  = 1
    else if(key == 1 && last_key != key){
        character_direct = 0 + 4;
        tile_flip = FALSE;
    }
    // left = 2
    else if(key == 2 && last_key != key){
        character_direct = 0 + 4;
        tile_flip = TRUE;
    }
    // up    = 4 (back)
    else if(key == 4 && last_key != key){
        character_direct = 3 + 3;
        tile_flip = FALSE;
    }
    // down  = 8 (front)
    else if(key == 8 && last_key != key){
        character_direct = 2 + 3;
        tile_flip = FALSE;
    }
    rotate_character_tiles(gameCharacter, character_direct, tile_flip);
}

void update_character_sprites(struct character* gameCharacter, UBYTE character_direct, UBYTE tile_flip){
    // set left and right sprites depending of the character direction
    if((frame % 8) == 0){
        if(last_anime == 0 && character_direct == 0){
            anime = 4;
        } else if(last_anime == 0){
            anime = 3;
        } else if((character_direct == 2 || character_direct == 3) && (reverse_anime == FALSE)){
            anime = 3;
            tile_flip = TRUE;
            reverse_anime = TRUE;
        } else {
            anime = 0;
            reverse_anime = FALSE;
        }
        rotate_character_tiles(gameCharacter, character_direct + anime, tile_flip);
        last_anime = anime;
    }
}

void update_bkg(UBYTE key, UBYTE bkg_x_pos, UBYTE bkg_y_pos){
    if((key & J_RIGHT) || (key & J_LEFT)){
        scroll_bkg(bkg_x_pos, 0);
    }
    else {
        scroll_bkg(0, bkg_y_pos);
    }
}

void update_display(UBYTE key, struct character* gameCharacter){
    tile_size = TL_SIZE;

    if(key){
        /* set scrolling variables, player direction  */
        if(key & J_RIGHT) {
        // right
            bkg_x_pos = scrolling_speed;
            character_direct = 0;
            // flip horizontally the sprites
            tile_flip = FALSE;
        } else if(key & J_LEFT) {
        // left
            bkg_x_pos = -scrolling_speed;
            character_direct = 0;
            // flip back horizontally the sprites
            tile_flip = TRUE;
        } else if(key & J_DOWN) {
        // front
            bkg_y_pos = scrolling_speed;
            character_direct = 2;
            // flip back horizontally the sprites
            tile_flip = FALSE;
        } else if(key & J_UP) {
        // back
            bkg_y_pos =- scrolling_speed;
            character_direct = 3;
            // flip back horizontally the sprites
            tile_flip = FALSE;
        }
        update_bkg(key, bkg_x_pos, bkg_y_pos);
        update_character_sprites(gameCharacter, character_direct, tile_flip);
    }
}

void updateSwitches() {
    HIDE_WIN;
	SHOW_SPRITES;
	SHOW_BKG;
}

void main(){
    disable_interrupts();
    DISPLAY_OFF;
    SPRITES_8x16;
    font_init();
    min_font = font_load(font_min); // 36 tiles
    font_set(min_font);

    init_player(&player, 0, TL_PL_NB, MID_X, MID_Y);
    init_background();
    
    DISPLAY_ON;
    enable_interrupts();

    while (1){
        key = joypad();
        if(key == 0 && last_key != 0){
            reset_character_pos(key, last_key, &player);
        }
        else if(key != 0 && last_key != key){
            reset_character_pos(key, last_key, &player);
        }
        last_key = key;
        s = get_s();
        if((s%1) == 0){
            frame += 1;
            if(frame == 32){
                frame = 0;
            }
        }
        update_display(key, &player);
        updateSwitches();
        wait_vbl_done();
    }
    
}