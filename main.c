#include <gb/gb.h>
#include <bcd.h>
#include <gb/font.h>
#include <stdio.h>
#include "character.c"
#include "assets/map.c"
#include "assets/tiles.c"
#include "assets/player3.c"

/* screen size */
#define MIN_SX          0U                  /* min x (char) */
#define MAX_SX          mapWidth            /* max x (char) */
#define MIN_SY          0U		              /* min y (char) */
#define MAX_SY          mapHeight           /* max y (char) */

/* sprites */
#define SP_H_DEF        0U                  /* sprite horizontal 0° rotation */
#define SP_H_FLIP       32U                 /* sprite horizontal 180° rotation */
#define TL_SIZE         8U                  /* sprite size length = height = 8 */

/* tiles */
#define TL_PL_NB        24U                 /* number of player tiles to load in VRAM */
#define TL_BKG_NB       32U                 /* number of background tiles to load in VRAM */

/* direction */
#define RIGHT           0U                  /* setup right logic value */
#define LEFT            1U                  /* setup left logic value */
#define DOWN            2U                  /* setup down logic value */
#define UP              3U                  /* setup up logic value */

/* screen */
#define MID_X           90U                 /* mid x pos (dot) */
#define MID_Y           70U                 /* mid y pos (dot) */

/* scrolling */
#define PXL_SC          1U                  /* nb of pixels to scroll when the player is moving */

extern clock_t clock();

font_t min_font;

UBYTE character_direct;
UBYTE tile_size, tile_init;
UBYTE bkg_x_pos, bkg_y_pos;
UBYTE scrolling_speed;
UBYTE key, last_key = 0;
UINT16 s;
BOOLEAN reverse_anime = FALSE;
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

int convert_direction_to_tile_number(int direction){
    int tile_number = direction;
    if(direction != RIGHT)
        tile_number = direction - 1;
    return tile_number;
}

void set_character_tiles(struct character* gameCharacter, int character_tile, BOOLEAN tile_flip){
    // flip tiles
    if(tile_flip){
        set_sprite_tile(gameCharacter->spriteId + 1, gameCharacter->leftSprite[character_tile]);
        set_sprite_tile(gameCharacter->spriteId, gameCharacter->rightSprite[character_tile]);
        set_sprite_prop(gameCharacter->spriteId, SP_H_FLIP);
        set_sprite_prop(gameCharacter->spriteId + 1, SP_H_FLIP);
    }
    else {
        set_sprite_tile(gameCharacter->spriteId, gameCharacter->leftSprite[character_tile]);
        set_sprite_tile(gameCharacter->spriteId + 1, gameCharacter->rightSprite[character_tile]);
        set_sprite_prop(gameCharacter->spriteId, SP_H_DEF);
        set_sprite_prop(gameCharacter->spriteId + 1, SP_H_DEF);    
    }
}

void init_character(struct character* gameCharacter, int firstTileLoaded, int tileRangeLoaded, int x, int y){
    BOOLEAN tile_flip = FALSE;
    int init_tile_number = convert_direction_to_tile_number(gameCharacter->direction);

    if(gameCharacter->direction == LEFT)
        tile_flip = TRUE;

    set_sprite_data(firstTileLoaded, tileRangeLoaded, player3);

    for(int i = 0; i < (tileRangeLoaded/4); i++){
        gameCharacter->leftSprite[i] = i * 4; 
        gameCharacter->rightSprite[i] = i * 4 + 2; 
    }

    gameCharacter->x = x;
    gameCharacter->y = y;
    
    set_character_tiles(gameCharacter, init_tile_number, tile_flip);
    move_sprite(gameCharacter->spriteId, gameCharacter->x, gameCharacter->y );
    move_sprite(gameCharacter->spriteId + 1, gameCharacter->x + TL_SIZE, gameCharacter->y );
}

void reset_character_pos(struct character* gameCharacter){
    BOOLEAN tile_flip = FALSE;
    int tile_number = convert_direction_to_tile_number(gameCharacter->direction);

    if(gameCharacter->direction == LEFT)
        tile_flip = TRUE;
    set_character_tiles(gameCharacter, tile_number, tile_flip);
}

BOOLEAN toggle_bool(BOOLEAN bool){
    if(bool == TRUE)
        return FALSE;
    else
        return TRUE;
}

void update_character_sprites(struct character* gameCharacter){
    int tile_number = convert_direction_to_tile_number(gameCharacter->direction);
    // left/right anime
    if((frame % 7) == 0 && (gameCharacter->direction == RIGHT || gameCharacter->direction == LEFT)){
        reverse_anime = FALSE;
        // reverse tile when left direction
        if(gameCharacter->direction == LEFT)
            reverse_anime = TRUE;
        if(last_anime == 0)
            anime = 3;
        else 
            anime = 0;
    } // back/front anime  
    else if((frame % 7) == 0 && (gameCharacter->direction == DOWN || gameCharacter->direction == UP)){
        // reverse or not anime
        if(last_anime == 0){
            anime = 3;
            reverse_anime = toggle_bool(reverse_anime);
        } // no anime 
        else if(last_anime == 3){
            anime = 0;
        }
    }
    set_character_tiles(gameCharacter, tile_number + anime, reverse_anime);
    last_anime = anime;
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
    // (KEY) right  = 1 / left = 2 / up = 4 (back) / down = 8 (front)
    if(!key && last_key < 9 || key < 9 && last_key != key)
        reset_character_pos(gameCharacter);

    if(key != 0 && key < 9){
        /* set scrolling variables and player direction */
        if(key & J_RIGHT) {
            bkg_x_pos = scrolling_speed;
            gameCharacter->direction = RIGHT;
        } else if(key & J_LEFT) {
            bkg_x_pos = -scrolling_speed;
            gameCharacter->direction = LEFT;
        } else if(key & J_DOWN) {
            bkg_y_pos = scrolling_speed;
            gameCharacter->direction = DOWN;
        } else if(key & J_UP) {
            bkg_y_pos =- scrolling_speed;
            gameCharacter->direction = UP;
        }
        update_bkg(key, bkg_x_pos, bkg_y_pos);
        update_character_sprites(gameCharacter);
    }
    last_key = key;
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

    init_background();

    struct character player;
    player.spriteId = 0;
    player.direction = RIGHT;
    init_character(&player, 0, TL_PL_NB, MID_X, MID_Y);
    
    DISPLAY_ON;
    enable_interrupts();

    while (1){
        key = joypad();
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