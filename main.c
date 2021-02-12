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

        set_sprite_tile(TL_PL_LEGS_NB, 6);
        move_sprite(TL_PL_LEGS_NB, PL_X, PL_Y+TL_SIZE);

        set_sprite_tile( TL_PL_LEGS_NB, TL_PL_LEGS_NB);
        move_sprite(TL_PL_LEGS_NB, PL_X, PL_Y+TL_SIZE);

    }
    else {
        // unflip
        set_sprite_prop(TL_PL_HEAD_NB, tile_flip);
        set_sprite_prop(TL_PL_LEGS_NB, tile_flip);
        // set up and down sprites
        set_sprite_tile(TL_PL_LEGS_NB, player_direct*2+1);
        move_sprite(TL_PL_LEGS_NB, PL_X, PL_Y+TL_SIZE);
    }
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
    init_player();
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