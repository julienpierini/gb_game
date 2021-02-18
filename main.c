#include "gb/gb.h"
#include "bcd.h"
#include "assets/map.c"
#include "assets/tiles.c"
#include "assets/player_tiles.c"

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
#define TL_PL_HEAD_NB  0U		        /* tile player head nb */
#define TL_PL_LEGS_NB  1U		        /* tile player legs nb */
#define TL_PL_NB       8U               /* number of player tiles to load in VRAM */
#define TL_BKG_NB      32U              /* number of background tiles to load in VRAM */

/* player */
#define PL_X		   90U		        /* player x pos (dot) */
#define PL_Y		   70U              /* player y pos (dot) */

/* scrolling */
#define PXL_SC          1U              /* nb of pixels to scroll when player is moving */

/* Gameboy */
#define CPU_FREQ        4190000U        /* CPU frequency in Hertz */

extern clock_t clock();


UBYTE player_direct;
UBYTE tile_size, tile_flip;
UBYTE bkg_x_pos, bkg_y_pos;
UBYTE scrolling_speed;
UBYTE key;
BCD   bcd;

UINT32 get_ms(){
    return clock()/CPU_FREQ*1000;
}

void init_background(){
    set_bkg_data(0, TL_BKG_NB, background);
    set_bkg_tiles(0, 0, mapWidth, mapHeight, map);
    scrolling_speed = PXL_SC;
}

<<<<<<< Updated upstream
void init_player(){
    set_sprite_data( 0, TL_PL_NB, player_tiles );
    set_sprite_tile( TL_PL_HEAD_NB, TL_PL_HEAD_NB );
    set_sprite_tile( TL_PL_LEGS_NB, TL_PL_LEGS_NB );
    move_sprite( TL_PL_HEAD_NB, PL_X, PL_Y );
    move_sprite( TL_PL_LEGS_NB, PL_X, PL_Y+TL_SIZE );
}

void update_player_sprites(UBYTE key, UBYTE player_direct, UBYTE tile_flip){
    // set head and legs sprite depending of the player direction
    set_sprite_tile(TL_PL_HEAD_NB, player_direct*2);
    move_sprite(TL_PL_HEAD_NB, PL_X, PL_Y); move_sprite(TL_PL_LEGS_NB, PL_X, PL_Y+TL_SIZE);

    if((key&J_RIGHT) || (key&J_LEFT)){
        // is flip
        set_sprite_prop(TL_PL_HEAD_NB, tile_flip);
        set_sprite_prop(TL_PL_LEGS_NB, tile_flip);
        // legs anim lef/right
        set_sprite_tile( TL_PL_LEGS_NB, 7 );
        move_sprite(TL_PL_LEGS_NB, PL_X, PL_Y+TL_SIZE);
=======

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
>>>>>>> Stashed changes

        set_sprite_tile(TL_PL_LEGS_NB, 6);
        move_sprite(TL_PL_LEGS_NB, PL_X, PL_Y+TL_SIZE);

        set_sprite_tile( TL_PL_LEGS_NB, TL_PL_LEGS_NB);
        move_sprite(TL_PL_LEGS_NB, PL_X, PL_Y+TL_SIZE);

<<<<<<< Updated upstream
    }
    else {
        // unflip
        set_sprite_prop(TL_PL_HEAD_NB, tile_flip);
        set_sprite_prop(TL_PL_LEGS_NB, tile_flip);
        // set up and down sprites
        set_sprite_tile(TL_PL_LEGS_NB, player_direct*2+1);
        move_sprite(TL_PL_LEGS_NB, PL_X, PL_Y+TL_SIZE);
    }
=======
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
>>>>>>> Stashed changes
}

void update_bkg(UBYTE key, UBYTE bkg_x_pos, UBYTE bkg_y_pos){
    if((key&J_RIGHT) || (key&J_LEFT)){
        scroll_bkg(bkg_x_pos, 0);
    }
    else {
        scroll_bkg(0, bkg_y_pos);
    }
}

void update_display(UBYTE key){
    tile_size = TL_SIZE;
<<<<<<< Updated upstream

    if(key){
        /* set scrolling variables, player direction  */
        if(key&J_LEFT) {
            bkg_x_pos=-scrolling_speed;
            player_direct=0;
            // flip horizontally the sprites
            tile_flip=SP_H_FLIP;
        } else if(key&J_RIGHT) {
            bkg_x_pos=scrolling_speed;
            player_direct=0;
            // flip back horizontally the sprites
            tile_flip=SP_H_DEF;
        } else if(key&J_UP) {
            bkg_y_pos=-scrolling_speed;
            player_direct=2;
            // flip back horizontally the sprites
            tile_flip=SP_H_DEF;
        } else if(key&J_DOWN) {
            bkg_y_pos=scrolling_speed;
            player_direct=1;
            // flip back horizontally the sprites
            tile_flip=SP_H_DEF;
=======
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
>>>>>>> Stashed changes
        }
        update_bkg(key, bkg_x_pos, bkg_y_pos);
        update_player_sprites(key, player_direct, tile_flip);
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
    init_background();
<<<<<<< Updated upstream
    init_player();
=======

    struct character player;
    player.spriteId = 0;
    player.direction = RIGHT;
    init_character(&player, 0, TL_PL_NB, MID_X, MID_Y);
    
>>>>>>> Stashed changes
    DISPLAY_ON;
    enable_interrupts();

    while (1){
        key = joypad();
        // int ms = get_ms();
        // bcd = MAKE_BCD(ms);
        update_display(key);
        updateSwitches();
        wait_vbl_done();
    }
    
}