/*  Copyright 2018-2019 Affonso Amendola
    
    This file is part of fflibvga.
    fflibvga is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    fflibvga is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with fflibvga.  If not, see <https://www.gnu.org/licenses/>
*/

#ifndef VGA_H
#define VGA_H

#define VGA_GRAPHICS_MODE_X     0x20
#define VGA_TEXT_MODE           0x03

#define SEQ_ADDRESS             0x03C4
#define SEQ_DATA                0x03C5

#define SEQ_RESET               0x00
#define SEQ_CLOCKING_MODE       0x01
#define SEQ_MAP_MASK            0x02
#define SEQ_CHAR_MAP_SEL        0x03
#define SEQ_MEMORY_MODE         0x04

#define CRTC_ADDRESS            0x03D4 //MIGHT CHANGE DEPENDING ON MONOCHROMATIC STUFF
#define CRTC_DATA               0x03D5 //MIGHT CHANGE DEPENDING ON MONOCHROMATIC STUFF

#define CRTC_H_TOTAL            0x00
#define CRTC_H_ENABLE_END       0x01
#define CRTC_START_H_BLANK      0x02
#define CRTC_END_H_BLANK        0x03
#define CRTC_START_H_RETRACE    0x04
#define CRTC_END_H_RETRACE      0x05
#define CRTC_V_TOTAL            0x06
#define CRTC_OVERFLOW           0x07
#define CRTC_PRESET_ROW_SCAN    0x08
#define CRTC_MAX_SCAN_LINE      0x09
#define CRTC_CURSOR_START       0x0A
#define CRTC_CURSOR_END         0x0B
#define CRTC_START_ADDR_HIGH    0x0C
#define CRTC_START_ADDR_LOW     0x0D
#define CRTC_CURSOR_LOC_HIGH    0x0E
#define CRTC_CURSOR_LOC_LOW     0x0F
#define CRTC_V_RETRACE_START    0x10
#define CRTC_V_RETRACE_END      0x11
#define CRTC_V_ENABLE_END       0x12
#define CRTC_OFFSET             0x13
#define CRTC_UNDERLINE_LOC      0x14
#define CRTC_START_V_BLANK      0x15
#define CRTC_END_V_BLANK        0x16
#define CRTC_CRT_MODE_CTRL      0x17
#define CRTC_LINE_COMPARE       0x18

#define GFXC_ADDRESS            0x03CE
#define GFXC_DATA               0x03CF

#define GFXC_SET_RESET          0x00
#define GFXC_ENABLE_SET_RESET   0x01
#define GFXC_COLOR_COMPARE      0x02
#define GFXC_DATA_ROTATE        0x03
#define GFXC_READ_MAP_SEL       0x04
#define GFXC_GRAPHICS_MODE      0x05
#define GFXC_MISC               0x06
#define GFXC_COLOR_DONT_CARE    0x07
#define GFXC_BIT_MASK           0x08

#define ATRC_ADDRESS            0x03C0
#define ATRC_DATA_WRITE         0x03C0
#define ATRC_DATA_READ          0x03C1

#define ATRC_INTERNAL_PALETTE   0x00 //MULTIPLE INDEXES, GOES TO INDEX 0x0F
#define ATRC_MODE_CONTROL       0x10
#define ATRC_OVERSCAN_COLOR     0x11
#define ATRC_COLOR_PLANE_ENABLE 0x12
#define ATRC_H_PEL_PANNING      0x13
#define ATRC_COLOR_SELECT       0x14

#define DAC_PALETTE_W_ADDRESS   0x03C8
#define DAC_PALETTE_R_ADDRESS   0x03C7
#define DAC_STATE               0x03C7
#define DAC_PALETTE_DATA        0x03C9
#define DAC_PALETTE_PEL_MASK    0x03C6

#define MISC_OUTPUT_READ        0x03CC
#define MISC_OUTPUT_WRITE       0x03C2

#define INPUT_STATUS_0          0x03C2
#define INPUT_STATUS_1          0x03DA //MIGHT CHANGE DEPENDING ON MONOCHROMATIC STUFF

#define FEATURE_CONTROL_READ    0x03CA
#define FEATURE_CONTROL_WRITE   0x03DA

#define VIDEO_SUBSYS_ENABLE     0x03C3
#define VIDEO_SUBSYS_ENABLE     0x03C3

#define TRANSPARENT_INDEX   0

#define COLOR_BLACK             0
#define COLOR_DARK_BLUE         1
#define COLOR_DARK_GREEN        2
#define COLOR_DARK_CYAN         3
#define COLOR_DARK_RED          4
#define COLOR_DARK_PURPLE       5
#define COLOR_DARK_YELLOW       6
#define COLOR_LIGHT_GRAY        7
#define COLOR_DARK_GRAY         8
#define COLOR_LIGHT_BLUE        9
#define COLOR_LIGHT_GREEN       10
#define COLOR_LIGHT_CYAN        11
#define COLOR_LIGHT_RED         12
#define COLOR_LIGHT_PURPLE      13
#define COLOR_LIGHT_YELLOW      14
#define COLOR_WHITE             15

#define COLOR_GRAY_0            16
#define COLOR_GRAY_1            17
#define COLOR_GRAY_2            18
#define COLOR_GRAY_3            19
#define COLOR_GRAY_4            20
#define COLOR_GRAY_5            21
#define COLOR_GRAY_6            22
#define COLOR_GRAY_7            23
#define COLOR_GRAY_8            24
#define COLOR_GRAY_9            25
#define COLOR_GRAY_10           26
#define COLOR_GRAY_11           27
#define COLOR_GRAY_12           28
#define COLOR_GRAY_13           29
#define COLOR_GRAY_14           30
#define COLOR_GRAY_15           31

#define COLOR_BLUE              32
#define COLOR_BLUE_1            33
#define COLOR_PURPLE_1          34
#define COLOR_PURPLE            35
#define COLOR_PINK              36
#define COLOR_PINK_1            37
#define COLOR_PINK_2            38
#define COLOR_RED_1             39
#define COLOR_RED               40
#define COLOR_ORANGE_1          41
#define COLOR_ORANGE            42
#define COLOR_YELLOW_1          43
#define COLOR_YELLOW            44
#define COLOR_LIME              45
#define COLOR_GREEN_1           46
#define COLOR_GREEN             47

#define VMEM       unsigned char far

#define VMEM_START 0xA0009600
#define VMEM_END   0xA000FFFF

#define SCREEN_SIZE 0x4B00

#define PAGE_0 (unsigned char far *)0xA0000000
#define PAGE_1 (unsigned char far *)0xA0004B00

#define SET_BITS(x,bits) (x|bits)
#define RESET_BITS(x,bits) (x&~bits)

extern int current_frame_buffer_page;
extern int current_draw_buffer_page;

typedef struct VMEM_ALLOCATION_
{
    unsigned char far * mem_start;
    unsigned char far * mem_end;
    struct VMEM_ALLOCATION_ * next_block;
} 
VMEM_ALLOCATION;

typedef struct POINT2_
{
    int x;
    int y;
}
POINT2;

typedef struct POINT3_
{
    int x;
    int y;
    int z;
}
POINT3;

typedef struct RECT_
{
    POINT2 start_point;
    int size_x;
    int size_y;    
}
RECT;

typedef struct COLOR_
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
}
COLOR;

typedef struct IMAGE_
{
    int in_vmem;

    unsigned long size_x;
    unsigned long size_y;

    COLOR indexed_color[256];
    unsigned char last_index_used;
    unsigned char palette_start_location;

    unsigned char far * data;
} 
IMAGE;

void init_vga();

unsigned char far * valloc(unsigned long size);
int vfree(VMEM* location);

int get_res_x();
int get_res_y();

void set_double_buffer(int value);

VMEM* get_draw_buffer();
VMEM* get_frame_buffer();

void flip_front_page();

void set_graphics_mode(int mode);
int get_graphics_mode();

void fill_screen(char color);

void set_pixel(POINT2 pos, int color);

void draw_line_h(POINT2 start_point, int length, char color);
void draw_line_v(POINT2 start_point, int height, char color);
void draw_rectangle_filled(RECT rectangle, char color);

void print_char(    POINT2 position, char c, 
                    char fore_color, char back_color, int fill_background);

void print_string(  POINT2 position, char *string, 
                    char fore_color, char back_color, int fill_background);

void print_int( POINT2 position, int integer, 
                char fore_color, char back_color, int fill_background);

void print_string_centralized(  int y, char *string, 
                                char fore_color, char back_color, int fill_background);


IMAGE * load_fis_image(char* filename, int in_vmem, int do_load_palette, unsigned char palette_start_location);
void unload_image(IMAGE * image);

void load_palette(IMAGE * image);
void set_palette(unsigned char color_index, COLOR color);


void copy_vmem_to_vmem      (   VMEM* source,
                                VMEM* destination,
                                int bytes,
                                int dest_offset_bytes);

void copy_smem_to_vmem      (   unsigned char far* source,
                                VMEM* destination,
                                int bytes,
                                int dest_offset_bytes);

void copy_vmem_to_db        (   VMEM* source,
                                int dest_offset_bytes,
                                int size_x_bytes,
                                int size_y_pels);

void copy_smem_to_db        (   unsigned char far* source,
                                int dest_offset_bytes,
                                int size_x_pels,
                                int size_y_pels);

void copy_image_to_db       (   IMAGE* image,
                                POINT2 position);


#endif /* VGA_H */