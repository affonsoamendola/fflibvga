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

#include <bios.h>
#include <dos.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>

#include "fflibvga.h"

unsigned char far* rom_char_set = (unsigned char far*)0xF000FA6EL;

int CURRENT_RES_X = 320;
int CURRENT_RES_Y = 240;

int CURRENT_VIDEO_MODE = VGA_TEXT_MODE;

int DOUBLE_BUFFER_ENABLE = 1;

VMEM_ALLOCATION video_allocation_head;

POINT2 point2(int x, int y)
{
	POINT2 to_return;

	to_return.x = x;
	to_return.y = y;

	return to_return;
}

int sign(int value)
{
	if(value >= 0)
	{
		return +1;
	}
	else
	{
		return -1;
	}
}

void init_vga()
{
	//Initializes the VGA for use, run this before using any function here.

	video_allocation_head.mem_start = (VMEM*)VMEM_END;
	video_allocation_head.mem_end = (VMEM*)VMEM_END;
	video_allocation_head.next_block = &video_allocation_head;
}

void wait_vblank()
{
	asm {
			mov dx, INPUT_STATUS_1
		}
	loop:;
	asm {
			
			in al, dx
			test al, 8
			jz loop:
		}
}

unsigned char far * valloc(unsigned long size)
{
	//Allocates memory for use in VMEM.
	//Receives size of the block to be allocated in bytes.
	//Returns the address of the start of the allocated area.

	int done = 0;
	int failed = 0;

	VMEM* current_location;

	VMEM_ALLOCATION * new_block;
	VMEM_ALLOCATION * current_block = &video_allocation_head;

	current_location = (VMEM*)VMEM_START;

	while(!done)
	{

		if(current_location + size < (VMEM*)(current_block->next_block->mem_start))
		{
			done = 1;
		}
		else
		{
			current_block = current_block->next_block;
			current_location = (current_block->mem_end)+1;
		}

		if(current_location + size > (VMEM *)VMEM_END)
		{
			printf("FAILURE ALLOCATING VMEMORY\n");
			done = 1;
			failed = 1;
		}
	}

	if(failed)
	{
		return NULL;
	}
	else
	{
		new_block = malloc(sizeof(VMEM_ALLOCATION));
		new_block->mem_start = current_location;
		new_block->mem_end = current_location + size;
		new_block->next_block = current_block->next_block;
		current_block->next_block = new_block;
		return current_location;
	}
}

int vfree(VMEM* location)
{
	//Clears the memory for use at VMEM location specified.
	//Returns -1 on failure and 0 on success.

	int done = 0;
	int failed = 0;

	VMEM_ALLOCATION * current_allocation = &video_allocation_head;

	while(!done)
	{
		if((VMEM*)(current_allocation->next_block)->mem_start == (VMEM*)location)
		{
			done = 1;
		}
		else
		{
			current_allocation = current_allocation->next_block;
			if(current_allocation == &video_allocation_head)
			{
				done = 1;
				failed = 1;
			}
		}
	}

	if(failed)
	{
		return -1;
	}
	else
	{
		current_allocation->next_block = (current_allocation->next_block)->next_block;
		return 0;
	}
}

int get_res_x()
{

	return CURRENT_RES_X;
}

int get_res_y()
{

	return CURRENT_RES_Y;
}

void set_double_buffer(int value)
{

	DOUBLE_BUFFER_ENABLE = value;
}

VMEM* get_draw_buffer()
{
	if(DOUBLE_BUFFER_ENABLE == 0)
	{
		return PAGE_0;
	}
	else
	{
		return PAGE_1;
	}
}

VMEM* get_frame_buffer()
{

	return PAGE_0;
}

void flip_front_page()
{	
	copy_vmem_to_vmem		(	get_draw_buffer(),
								get_frame_buffer(),
								(CURRENT_RES_X>>2)*CURRENT_RES_Y,
								0);
}

void set_graphics_mode(int mode)
{
	unsigned char data;
	VMEM* draw_buffer;

	draw_buffer = get_draw_buffer();

	if(mode == VGA_TEXT_MODE)
	{
		_asm	{
				mov ax,0003h
				int 10h
				}
	}

	if(mode == VGA_GRAPHICS_MODE_X)
	{
		_asm 	{
				mov ax,0013h;
				int 10h;
				}

		outport(SEQ_ADDRESS,		0x0600 | SEQ_MEMORY_MODE);
		outport(SEQ_ADDRESS, 		0x0100 | SEQ_RESET);
		outportb(MISC_OUTPUT_WRITE, 0xE3);
		outport(SEQ_ADDRESS, 		0x0300 | SEQ_RESET); 

		outportb(CRTC_ADDRESS,	CRTC_V_RETRACE_END);
		data = inportb(CRTC_DATA);
		outportb(CRTC_DATA,	data &(~0x80));

		outport(CRTC_ADDRESS, 		0x0D00 | CRTC_V_TOTAL);
		outport(CRTC_ADDRESS, 		0x3E00 | CRTC_OVERFLOW);
		outport(CRTC_ADDRESS, 		0x4100 | CRTC_MAX_SCAN_LINE);
		outport(CRTC_ADDRESS,		0xEA00 | CRTC_V_RETRACE_START);
		outport(CRTC_ADDRESS, 		0xAC00 | CRTC_V_RETRACE_END);
		outport(CRTC_ADDRESS, 		0xDF00 | CRTC_V_ENABLE_END);
		outport(CRTC_ADDRESS, 		0x0000 | CRTC_UNDERLINE_LOC);
		outport(CRTC_ADDRESS, 		0xE700 | CRTC_START_V_BLANK);
		outport(CRTC_ADDRESS, 		0x0600 | CRTC_END_V_BLANK );
		outport(CRTC_ADDRESS, 		0xE300 | CRTC_CRT_MODE_CTRL);

		outport(SEQ_ADDRESS, 		0x0F00 | SEQ_MAP_MASK);

		_asm 	{
				les di,draw_buffer
				sub ax,ax
				mov cx,320*240/4
				rep stosw
				}

		CURRENT_RES_X = 320;
		CURRENT_RES_Y = 240;
	}

	CURRENT_VIDEO_MODE = mode;
}

int get_graphics_mode()
{

	return CURRENT_VIDEO_MODE;
}

void fill_screen(char color)
{
	//Fills the draw_buffer with a certain color

	VMEM* draw_buffer;

	draw_buffer = get_draw_buffer();
 
	_asm	{
			mov dx,	SEQ_ADDRESS
			mov al,	SEQ_MAP_MASK
			mov ah,	0x0F
			out dx,	ax
			les di,	draw_buffer
			mov al,	BYTE PTR color
			mov ah,	al
			mov cx,	(320*240/8)
			rep stosw
			}
}

void set_pixel(POINT2 pos, int color)
{
	char bitmask;
	int offset;

	bitmask = 0x01 << (pos.x & 0x03);
	offset = pos.y*(CURRENT_RES_X>>2) + (pos.x>>2);

	_asm	{
			mov dx,	SEQ_ADDRESS
			mov al,	SEQ_MAP_MASK
			mov ah,	bitmask
			out dx, ax
			}
	
	get_draw_buffer()[offset] = (unsigned char)color;
}

void draw_line_h(POINT2 start_point, int length, char color)
{
	//Draws a horizontal line starting at start_point with the specified length. and color.
	//Goes from left to right. 

	char bitmask_left;
	char bitmask_right;
	char bitmask_center;

	int y_offset;

	int i;

	POINT2 left_point;

	if(start_point.y < 0 || start_point.y >= get_res_x())
	{
		return;
	}

	if(length < 0)
	{
		left_point.x = start_point.x + length;
		left_point.y = start_point.y;
	}
	else
	{
		left_point = start_point;
	}

	y_offset = left_point.y*(CURRENT_RES_X>>2);

	bitmask_left = 0x0F << (left_point.x & 0x03);
	bitmask_right = ~(0x0F << ((left_point.x + abs(length)) & 0x03));
	
	bitmask_left &= 0x0F;
	bitmask_right &= 0x0F;

	bitmask_center = bitmask_left & bitmask_right;

	if(left_point.x>>2 != (left_point.x + abs(length))>>2)
	{
		_asm	{
				mov dx,	SEQ_ADDRESS
				mov al,	SEQ_MAP_MASK
				mov ah,	bitmask_left
				out dx, ax
				}

		if((left_point.x>>2) >= 0 && (left_point.x>>2) < (get_res_x()>>2))
		{
			get_draw_buffer()[y_offset + (left_point.x>>2)] = color;
		}
		
		_asm	{
				mov dx,	SEQ_ADDRESS
				mov al,	SEQ_MAP_MASK
				mov ah,	bitmask_right
				out dx, ax
				}
		
		if(((left_point.x + abs(length))>>2) >= 0 && ((left_point.x + abs(length))>>2) < (get_res_x()>>2))
		{
			get_draw_buffer()[y_offset + ((left_point.x + abs(length))>>2)] = color;
		}

		_asm	{
				mov dx,	SEQ_ADDRESS
				mov al,	SEQ_MAP_MASK
				mov ah,	0x0F
				out dx, ax
				}

		for(i = (left_point.x>>2)+1; i < ((left_point.x + abs(length))>>2); i++)
		{
			if(i >= 0 && i < (get_res_x()>>2))
			{
				get_draw_buffer()[y_offset + i] = color;
			}
		}
	}
	else
	{
		asm	{
				mov dx,	SEQ_ADDRESS
				mov al,	SEQ_MAP_MASK
				mov ah,	bitmask_center
				out dx, ax
				}

		if((left_point.x>>2) >= 0 && (left_point.x>>2) < (get_res_x()>>2))
		{		
			get_draw_buffer()[y_offset + (left_point.x>>2)] = color;
		}
	}
}

void draw_line_v(POINT2 start_point, int height, char color)
{
	char bitmask;

	int start_offset;
	int i;

	start_offset = start_point.y*(get_res_x()>>2) + (start_point.x>>2);

	bitmask = 0x01 << (start_point.x & 3);

	_asm	{
			mov dx,	SEQ_ADDRESS
			mov al,	SEQ_MAP_MASK
			mov ah,	bitmask
			out dx, ax
			}


	for(i = 0; i < abs(height); i++)
	{
		if(	start_point.y + (sign(height) * i) >= 0 && start_point.y + (sign(height) * i) < get_res_y() && 
			start_point.x >= 0 && start_point.x < get_res_x())
		{
			get_draw_buffer()[start_offset + (sign(height)) * i * (get_res_x()>>2)] = color;
		}
	}
}

void draw_line(POINT2 start_point, POINT2 end_point, char color)
{
	float delta_x;
	float delta_y;
	float delta_error;
	float error;

	int y;
	int x;
	int h_line_start;
	int v_line_start;

	if(start_point.x == end_point.x)
	{
		draw_line_v(start_point, end_point.y - start_point.y, color);
		return;
	}
	else if(start_point.y == end_point.y)
	{
		draw_line_h(start_point, end_point.x - start_point.x, color);
		return;
	}

	delta_x = (float)(end_point.x - start_point.x);
	delta_y = (float)(end_point.y - start_point.y);

	if(fabs(delta_x) > fabs(delta_y))
	{
		delta_error = fabs(delta_y/delta_x);

		error = 0.0f;

		y = start_point.y;
		h_line_start = 0;

		for(x = 0; x < abs(end_point.x - start_point.x); x++)
		{
			error = error + delta_error;
			if(error >= 0.5f)
			{
				draw_line_h(point2(start_point.x + sign(delta_x)*h_line_start, y), sign(delta_x)*(x - h_line_start), color);
				h_line_start += x - h_line_start;
				y = y + sign(delta_y);
				error -= 1.0f;
			}
		} 	
		draw_line_h(point2(start_point.x + sign(delta_x)*h_line_start, y), sign(delta_x)*(x - h_line_start), color);
	}
	else
	{
		delta_error = fabs(delta_x/delta_y);

		error = 0.0f;

		x = start_point.x;
		v_line_start = 0;

		for(y = 0; y < abs(end_point.y - start_point.y); y++)
		{
			error = error + delta_error;
			if(error >= 0.5f)
			{
				draw_line_v(point2(x, start_point.y + sign(delta_y)*v_line_start), sign(delta_y)*(y - v_line_start), color);
				v_line_start += y - v_line_start;
				x = x + sign(delta_x);
				error -= 1.0f;
			}
		} 	
		draw_line_v(point2(x, start_point.y + sign(delta_y)*v_line_start), sign(delta_y)*(y - v_line_start), color);
	}
}

void draw_rectangle_filled(RECT rectangle, char color)
{
	int i;
	POINT2 current_line_start;
	
	current_line_start.x = rectangle.start_point.x;
	current_line_start.y = rectangle.start_point.y;

	for(i = 0; i < rectangle.size_y; i++)
	{
		draw_line_h(current_line_start, rectangle.size_x, color);
		current_line_start.y += 1;
	}
}

void print_char(	POINT2 position, char c, 
					char fore_color, char back_color, int fill_background)
{
	int offset, x, y;

	unsigned char far *work_char;

	unsigned char bit_mask;
	unsigned char sequence;

	work_char = rom_char_set + c*8;

	offset = position.y*(CURRENT_RES_X>>2);
	
	for(y=0; y<8; y++)
	{
		bit_mask = 0x88;
		for(x=0; x<4; x++)
		{
			sequence = 0x01<<((position.x+x) & 3);

			_asm	{
					mov dx,	SEQ_ADDRESS
					mov al,	SEQ_MAP_MASK
					mov ah,	sequence
					out dx, ax
					}

			if((*work_char & (bit_mask & 0xf0)))
				get_draw_buffer()[offset+((position.x+x)>>2)] = (unsigned char)fore_color;

			else if(fill_background)
				get_draw_buffer()[offset+((position.x+x)>>2)] = (unsigned char)back_color;
	
			
			if((*work_char & (bit_mask & 0x0f)))
				get_draw_buffer()[offset+((position.x+x)>>2)+1] = (unsigned char)fore_color;
			
			else if(fill_background)
				get_draw_buffer()[offset+((position.x+x)>>2)+1] = (unsigned char)back_color;
			
			bit_mask = (bit_mask>>1);
		}

		offset += (CURRENT_RES_X>>2);
		work_char++;
	}
}

void print_string(	POINT2 position, char *string, 
					char fore_color, char back_color, int fill_background)
{
	int i, length;
	POINT2 current_position;

	current_position = position;

	length = strlen(string);

	for(i=0; i<length; i++)
	{
		print_char(current_position, string[i], fore_color, back_color, fill_background);
		current_position.x += 8;
	}
}

void print_int(	POINT2 position, int integer, 
				char fore_color, char back_color, int fill_background)
{
	char char_buffer[256];

	print_string(position, (char *)itoa(integer, char_buffer, 10), fore_color, back_color, fill_background);
}

void print_string_centralized(	int y, char *string, 
								char fore_color, char back_color, int fill_background)
{
	int len;
	POINT2 position;

	len = strlen(string);

	position.y = y;
	position.x = CURRENT_RES_X/2-len*4;

    print_string(position, string, fore_color, back_color, fill_background);
}

IMAGE * load_fis_image(char* filename, int in_vmem, int do_load_palette, unsigned char palette_start_location)
{
	unsigned int magic_number;
	unsigned char adapter_type;

	unsigned char sequence_indicator;

	unsigned char sequence_length;
	unsigned char sequence_index;

	int i;

	int bitmask;

	long count;

	char buffer;

	IMAGE * image;	
	FILE * file;

	file = fopen(filename, "rb");

	magic_number = 0;

	if(file == NULL)
	{
		set_graphics_mode(VGA_TEXT_MODE);
		printf("FIS - Couldn't find file : %s\n", filename);
		exit(-1);
	} 

	magic_number |= ((int)getc(file))<<8;
	magic_number |= ((int)getc(file));

	if(magic_number != 0xF0F0) 
	{
		set_graphics_mode(VGA_TEXT_MODE);
		printf("FIS - Magic number is wrong, are you sure this is a FIS file? File: %s\n", filename);
		exit(-1);
	}

	adapter_type = getc(file);

	if(adapter_type != 0) 
	{
		set_graphics_mode(VGA_TEXT_MODE);
		printf("FIS - This file is not for the VGA board, so this function cant load it properly. File: %s\n", filename);
		exit(-1);
	}

	image = malloc(sizeof(IMAGE));

	image->size_x |= ((int)getc(file))<<8;
	image->size_x |= ((int)getc(file));

	image->size_y |= ((int)getc(file))<<8;
	image->size_y |= ((int)getc(file));

	image->in_vmem = in_vmem;

	image->last_index_used = (unsigned char)getc(file);

	if((int)(image->last_index_used)+(int)palette_start_location > 255)
	{
		set_graphics_mode(VGA_TEXT_MODE);
		printf("FIS - Not enough palette indexes to allocate for this image, change the palette start index for file: %s\n", filename);
		exit(-1);
	} 

	image->palette_start_location = palette_start_location;

	sequence_indicator = (unsigned char)getc(file);

	for(i = 0; i < 23; i++)
	{
		buffer = (unsigned char)getc(file);
	}

	count = 0;

	if(in_vmem)
	{
		image->data = valloc(((image->size_x>>2)*(image->size_y)));
		if(image->data == NULL) 
		{
			set_graphics_mode(VGA_TEXT_MODE);
			printf("FIS - Couldn't allocate video memory for image file: %s\n", filename);
			exit(-1);
		}
	}
	else
	{
		image->data = malloc((image->size_x)*(image->size_y));
		if(image->data == NULL) 
		{
			set_graphics_mode(VGA_TEXT_MODE);
			printf("FIS - Couldn't allocate standard memory for image file: %s\n", filename);
			exit(-1);
		}
	}

	while(count < ((unsigned long)image->size_x * (unsigned long)image->size_y))
	{
		buffer = (unsigned char)getc(file);

		if(buffer == sequence_indicator)
		{
			sequence_index = (unsigned char)getc(file);
			sequence_length = (unsigned char)getc(file);

			for(i = 0; i < sequence_length; i++)
			{
				if(in_vmem)
				{
					bitmask = 0x0100 << (count & 3);

					outport(SEQ_ADDRESS, bitmask | SEQ_MAP_MASK);
					*((image->data) + (count>>2)) = sequence_index + palette_start_location;
				}
				else
				{
					*((image->data) + count) = sequence_index + palette_start_location;
				}
				count++;
			}
		}
		else
		{
			if(in_vmem)
			{
				bitmask = 0x0100 << (count & 3);

				outport(SEQ_ADDRESS, bitmask | SEQ_MAP_MASK);
				*((image->data) + (count>>2)) = buffer + palette_start_location;
			}
			else
			{
				*((image->data) + count) = buffer + palette_start_location;
			}
			count++;	
		}
	}

	for(i = 0; i <= image->last_index_used; i++)
	{
		image->indexed_color[i].r = (unsigned char)getc(file);
		image->indexed_color[i].g = (unsigned char)getc(file);
		image->indexed_color[i].b = (unsigned char)getc(file);
	}

	if(do_load_palette)
	{
		load_palette(image);
	}

	return (IMAGE *)image;
}

void unload_image(IMAGE * image)
{
	if(image->in_vmem)
	{
		vfree((VMEM*)image->data);
	}
	else
	{
		free((void *)image->data);
	}

	free(image);
}

void load_palette(IMAGE * image)
{
	unsigned char last_index_used;
	unsigned char palette_start_location;

	int i;

	last_index_used = image->last_index_used;
	palette_start_location = image->palette_start_location;

	for(i = 0; i <= last_index_used; i++)
	{
		set_palette(i + palette_start_location, image->indexed_color[i]);
	}
}

void set_palette(unsigned char color_index, COLOR color)
{
	outportb(DAC_PALETTE_W_ADDRESS, color_index);
	outportb(DAC_PALETTE_DATA, color.r>>2);
	outportb(DAC_PALETTE_DATA, color.g>>2);
	outportb(DAC_PALETTE_DATA, color.b>>2);
}

void copy_vmem_to_vmem		(	VMEM* source,
								VMEM* destination,
								int bytes,
								int dest_offset_bytes)
{
	volatile unsigned char current_pixel = 0;

	int i;

	outport(GFXC_ADDRESS, 0x0000 | GFXC_BIT_MASK);
	outport(SEQ_ADDRESS, 0x0F00 | SEQ_MAP_MASK);

	for(i=0; i < bytes; i++)
	{	
		current_pixel = *(source + i);		
		*(dest_offset_bytes + destination + i) = 0;	
	}

	outport(GFXC_ADDRESS, 0xFF00 | GFXC_BIT_MASK);	
}

void copy_smem_to_vmem		(	unsigned char far* source,
								VMEM* destination,
								int bytes,
								int dest_offset_bytes)
{
	volatile unsigned char current_pixel = 0;

	int i, p;

	char bitmask;

	bitmask = 0x00;

	for(p = 0; p < 4; p++)
	{
		bitmask = (0x01 << p);

		_asm	{
				mov dx,	SEQ_ADDRESS
				mov al,	SEQ_MAP_MASK
				mov ah,	bitmask
				out dx, ax
				}

		for(i = 0; i < (bytes>>2); i++)
		{
			current_pixel = *(source + (i*4) + p);
			*(destination + dest_offset_bytes + i) = current_pixel;
		}
	}	
}

void copy_vmem_to_db 		(	VMEM* source,
								int dest_offset_bytes,
								int size_x_bytes,
								int size_y_pels)
{
	volatile unsigned char current_pixel = 0;

	int x, y;

	outport(GFXC_ADDRESS, 	0x0000 | GFXC_BIT_MASK);
	outport(SEQ_ADDRESS, 	0xFF00 | SEQ_MAP_MASK);

	for(y = 0; y < size_y_pels; y++)
	{
		for(x = 0; x < size_x_bytes; x++)
		{	
			current_pixel = *(source + y*size_x_bytes + x);		
			*(get_draw_buffer() + dest_offset_bytes + y*(CURRENT_RES_X>>2) + x) = 0;	
		}
	}
	outport(GFXC_ADDRESS, 	0xFF00 | GFXC_BIT_MASK);
}

void copy_smem_to_db		(	unsigned char far* source,
								int dest_offset_bytes,
								int size_x_pels,
								int size_y_pels)
{
	volatile unsigned char current_pixel = 0;

	int x, y, p;

	char bitmask;

	bitmask = 0x00;

	for(p = 0; p < 4; p++)
	{
		bitmask = (0x01 << p);

		_asm	{
					mov dx,	SEQ_ADDRESS
					mov al,	SEQ_MAP_MASK
					mov ah,	bitmask
					out dx, ax
					}

		for(y = 0; y < size_y_pels; y++)
		{
			for(x = 0; x < (size_x_pels>>2); x++)
			{	
				current_pixel = *(source + x*4 + y*size_x_pels + p);
				*(get_draw_buffer() + dest_offset_bytes + x + y*(CURRENT_RES_X>>2)) = current_pixel;
			}
		}
	}	
}

void copy_image_to_db		(   IMAGE* image,
								POINT2 position  )
{
	int dest_offset_bytes;

	dest_offset_bytes = (position.x>>2) + position.y*(CURRENT_RES_X>>2);

	if(image->in_vmem)
	{
		copy_vmem_to_db(image->data, dest_offset_bytes, (image->size_x)>>2, image->size_y);
	}
	else
	{
		copy_smem_to_db(image->data, dest_offset_bytes, image->size_x, image->size_y);
	}
}

void main()
{
	set_graphics_mode(VGA_GRAPHICS_MODE_X);
	set_double_buffer(1);
	init_vga();

	draw_line(point2(0, 200), point2(10, 0), COLOR_ORANGE_1);

	flip_front_page();
	getch();
}
