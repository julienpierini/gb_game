#include <gb/gb.h>
#include <stdio.h>

#include <gb/bgb_emu.h>

const UBYTE scanline_offsets_tbl[] = {0, 1, 2, 3, 3, 2, 1, 0, 0, 1, 2, 3, 3, 2, 1, 0};
const UBYTE * scanline_offsets = scanline_offsets_tbl;

void scanline_isr() {
    SCX_REG = scanline_offsets[LY_REG & (UBYTE)7];
}


void main() {
    printf("This is\na wobble\ntest\nfor DMG\n|\n|\n|\n|\n|");
    
    CRITICAL {
        STAT_REG = 0x18;
        add_LCD(scanline_isr);
    }
    set_interrupts(VBL_IFLAG | LCD_IFLAG);

    while (1) {
        wait_vbl_done();        
        scanline_offsets = &scanline_offsets_tbl[(UBYTE)(sys_time >> 2) & 0x07u];
    }
}