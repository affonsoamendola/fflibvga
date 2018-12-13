/*  Copyright 2018 Affonso Amendola
    
    This file is part of dosVGAlib.
    dosVGAlib is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    dosVGAlib is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with dosVGAlib.  If not, see <https://www.gnu.org/licenses/>
*/

#ifndef VGA_H
#define VGA_H

#define GRAPHICS_MODE13 0x13
#define GRAPHICS_MODEX 0xFD
#define TEXT_MODE 0x03
#define GRAPHICS_MODEZ 0xFF

#define ROM_CHAR_SET_SEG 0xF000
#define ROM_CHAR_SET_OFF 0XFA6E

#define VRETRACE 0x08

#define SEQUENCER 0x3C4
#define CRT_CONTROLLER 0x3D4
#define GFX_CONTROLLER 0x3CE

#define DAC_WRITE 0x3C8
#define DAC_READ 0x3C7
#define DAC_DATA 0x3C9

#define ATTR_CONTROLLER_FF 0x3DA
#define ATTR_CONTROLLER_DATA 0x3C0

#define MISC_OUTPUT 0x3C2

#define CRT_MAX_SCANLINE 0x09
#define CRT_ADDR_MODE 0x14
#define CRT_MODE_CONTROL 0x17

#define GFX_WRITE_MODE 0x05
#define GFX_MISC 0x06

#define SEQ_PLANE_ENABLE 0x02
#define SEQ_MEMORY_MODE 0x04

#define O_RDONLY        0x01

#define TRANSPARENT_INDEX   0

#define SET_BITS(x,bits) (x|bits)
#define RESET_BITS(x,bits) (x&~bits)

extern int current_frame_buffer_page;
extern int current_draw_buffer_page;

int get_res_x();
int get_res_y();

unsigned char far* get_draw_buffer();
unsigned char far* get_frame_buffer();

void set_color(unsigned char color_index, unsigned char red, unsigned char green, unsigned char blue);
void set_pallette(unsigned char* pallette, int start_index, int end_index);
void write_pallette(char * filename, unsigned char* pallette, int start_index, int end_index);
void get_pallette(unsigned char* pallette, int start_index ,int end_index);
unsigned char get_color(unsigned char color_index);

void set_graphics_mode(int mode);

void fill_screen(int color);
void set_pixel(int x, int y, int color);
int get_pixel(int x, int y);

void draw_line_h(int x1, int x2, int y, int color);
void draw_line_v(int x, int y1, int y2, int color);

void frame_page(int page);
void draw_page(int page);

void draw_message_box(char *line1, char *line2, char *line3, int line_spacing, int text_color, int back_color, int line_color);

void fill_rectangle(int x1, int x2, int y1, int y2, int color);

void print_char(int xc, int yc, char c, int color, int transparent);
void print_string(int x, int y, int color, char *string, int transparent);

void load_pgm(char* filename, unsigned char far * allocated_mem, int x_size, int y_size);

void copy_vmem_to_dbuffer(  unsigned char far * location, 
                            int x_pos_fbuffer, 
                            int y_pos_fbuffer, 
                            int x_offset_start_vmem, 
                            int y_offset_start_vmem,
                            int x_offset_end_vmem,
                            int y_offset_end_vmem,
                            int x_vmem_size);

void copy_vmem_to_dbuffer_latched ( unsigned char far * source,
                                    int bytes,
                                    int dest_offset);

void copy_vmem_to_location_latched ( unsigned char far* source,
                                     unsigned char far* destination,
                                     int bytes,
                                     int dest_offset);

void draw_vmem_to_dbuffer_latched ( unsigned char far* source,
                                    int latchedColumns,
                                    int lines,
                                    int dest_offest );
#endif /* VGA_H */