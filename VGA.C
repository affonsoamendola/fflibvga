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

#include <bios.h>
#include <dos.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include <VGA.H>
#include <FONFLIB.H>

unsigned char far* frame_buffer = 0xA0000000;
unsigned char far* draw_buffer = 0xA0004B00L;

unsigned char far* rom_char_set = 0xF000FA6EL;

int current_video_mode = TEXT_MODE;

void set_color(unsigned char color_index, unsigned char red, unsigned char green, unsigned char blue)
{
	outportb(DAC_WRITE, color_index);
	outportb(DAC_DATA, red);
	outportb(DAC_DATA, green);
	outportb(DAC_DATA, blue);
}

void set_pallette(unsigned char* pallette, int start_index, int end_index)
{
	int i;
	int j;
	outportb(DAC_WRITE, start_index);
	for(i = 0; i <= end_index-start_index; i++)
	{
		for(j=0; j<3; j++)
		{
			outportb(DAC_DATA,*(pallette+(i*3)+j));
		}
	}
}

void write_pallette(char * filename, unsigned char* pallette, int start_index, int end_index)
{
	FILE * file;
	int i;
	int j;

	unsigned char current_char;

	int middle_of_number;

	file = fopen(filename, "w");

	for (i = 0; i <= (end_index-start_index); i++)
	{
		fputi(start_index + i, 3, file);
		fputc(' ', file);

		for(j = 0; j<3; j++)
		{
			fputi((*(pallette+i*3+j)), 3, file);
			if(j < 2)
				fputc(' ',file);
			else
				fputc('\n',file);
		}
	}
}

void load_pallette(char * filename, int size)
{
	FILE * file;
	int i;
	int j;

	unsigned char r, g, b;

	unsigned char index = 0;

	int format_error = 0;

	unsigned char current_char;

	r = 0;
	g = 0;
	b = 0;

	file = fopen(filename, "r");
	for(j= 0; j<size; j++)
	{	
		for(i= 0; i<4;i++)
		{	
			current_char = fgetc(file);

			if(current_char >= 48 && current_char <= 58)
			{
				index = index*10 + current_char - 48;
				
			}
			else if(current_char == 32)
			{
				break;
			}
			else
			{
				format_error = 1;
			}
		}

		for(i= 0; i<4;i++)
		{	
			current_char = fgetc(file);

			if(current_char >= 48 && current_char <= 58)
			{
				r = r*10 + current_char - 48;
				
			}
			else if(current_char == 32)
			{
				break;
			}
			else
			{
				format_error = 1;
			}
		}

		for(i= 0; i<4;i++)
		{	
			current_char = fgetc(file);

			if(current_char >= 48 && current_char <= 58)
			{
				g = g*10 + current_char - 48;
				
			}
			else if(current_char == 32)
			{
				break;
			}
			else
			{
				format_error = 1;
			}
		}

		for(i= 0; i<4;i++)
		{	
			current_char = fgetc(file);
			
			if(current_char >= 48 && current_char <= 58)
			{
				b = b*10 + current_char - 48;
				
			}
			else if(current_char == '\n')
			{
				break;
			}
			else
			{
				format_error = 1;
			}
		}

		if(format_error == 1)
		{
			set_graphics_mode(TEXT_MODE);
			printf("FILE IS NOT PLT, OR IS CORRUPTED SOMEHOW, CHECK YO FILES YO, filename: %s", filename);
			exit(EXIT_FAILURE);
		}

		outportb(DAC_WRITE, index);
		outportb(DAC_DATA, r);
		outportb(DAC_DATA, g);
		outportb(DAC_DATA, b);

		index = 0;
		r = 0;
		g = 0;
		b = 0;
	}
}

void get_pallette(unsigned char* pallette, int start_index ,int end_index)
{
	int i;
	int j;

	outportb(DAC_READ, start_index);
	for(i = 0; i <= end_index-start_index; i++)
	{
		for(j=0; j<3; j++)
		{
			*(pallette+(i*3)+j) = inportb(DAC_DATA);
		}
	}
}

unsigned char get_color(unsigned char color_index)
{
	outportb(DAC_WRITE, color_index);
	return inportb(DAC_DATA);
}

void set_graphics_mode(int mode)
{
	unsigned char data;
	if(mode == TEXT_MODE)
	{
		_asm	{
			mov ax,0003h;
			int 10h;
			}
	}

	if(mode == GRAPHICS_MODE13)
	{
		_asm	{
			mov ax,0013h;
			int 10h;
			}
	}
	if(mode == GRAPHICS_MODEX)
	{
		_asm 	{
				mov ax,0013h;
				int 10h;
				}

		outport(SEQUENCER, 0x0604);
		outport(SEQUENCER, 0x0100);
		outportb(MISC_OUTPUT,0xe3);
		outport(SEQUENCER, 0x0300);

		outportb(CRT_CONTROLLER,0x11);
		data = inportb(CRT_CONTROLLER+1);
		data = data & 0x7f;
		outportb(CRT_CONTROLLER+1,data);
		outport(CRT_CONTROLLER, 0x0d06);
		outport(CRT_CONTROLLER, 0x3e07);
		outport(CRT_CONTROLLER, 0x4109);
		outport(CRT_CONTROLLER, 0xea10);
		outport(CRT_CONTROLLER, 0xac11);
		outport(CRT_CONTROLLER, 0xdf12);
		outport(CRT_CONTROLLER, 0x0014);
		outport(CRT_CONTROLLER, 0xe715);
		outport(CRT_CONTROLLER, 0x0616);
		outport(CRT_CONTROLLER, 0xe317);

		outport(SEQUENCER, 0x0f02);

		_asm {
			les di,draw_buffer;
			sub ax,ax
			mov cx,320*240/4;
			rep stosw;
		}
	}
	if(mode == GRAPHICS_MODEZ)
	{
		int data;
		_asm	{
			mov ax,0013h;
			int 10h;
			}

		outportb(CRT_CONTROLLER,CRT_MAX_SCANLINE);
		data = inportb(CRT_CONTROLLER+1);
		outportb(CRT_CONTROLLER+1,RESET_BITS(data,0x0f));

		outportb(CRT_CONTROLLER,CRT_ADDR_MODE);
		data=inportb(CRT_CONTROLLER+1);
		outportb(GFX_CONTROLLER+1,RESET_BITS(data,0x40));

		outportb(CRT_CONTROLLER,CRT_MODE_CONTROL);
		data=inportb(CRT_CONTROLLER+1);
		outportb(CRT_CONTROLLER+1,SET_BITS(data,0x40));

		outportb(GFX_CONTROLLER,GFX_WRITE_MODE);
		data=inportb(GFX_CONTROLLER+1);
		outportb(GFX_CONTROLLER+1,RESET_BITS(data,0x10));

		outportb(GFX_CONTROLLER,GFX_MISC);
		data=inportb(GFX_CONTROLLER+1);
		outportb(GFX_CONTROLLER+1,RESET_BITS(data,0x02));

		outportb(SEQUENCER,SEQ_MEMORY_MODE);
		data=inportb(SEQUENCER+1);
		data=RESET_BITS(data,0x08);
		data=SET_BITS(data,0x04);
		outportb(SEQUENCER+1,data);
		outportb(SEQUENCER,SEQ_PLANE_ENABLE);
		outportb(SEQUENCER+1,0x0f);

		_asm	{
			les di, draw_buffer;
			xor ax,ax;
			mov cx,320*400/8;
			rep stosw;
			}
	}
	current_video_mode = mode;
}

void fill_screen(int color)
{
	if(current_video_mode == GRAPHICS_MODE13)
	{
	_asm	{
		les di,draw_buffer;
		mov al,BYTE PTR color;
		mov ah,al;
		mov cx,320*200/2;
		rep stosw;
		}
	}
	if(current_video_mode == GRAPHICS_MODEX)
	{
	_asm	{
		mov dx,SEQUENCER;
		mov al,SEQ_PLANE_ENABLE;
		mov ah,0fh;
		out dx,ax;
		les di,draw_buffer;
		mov al,BYTE PTR color;
		mov ah,al;
		mov cx,(320*240/8);
		rep stosw;
		}
	}

	if(current_video_mode == GRAPHICS_MODEZ)
	{
	_asm	{
		mov dx,SEQUENCER;
		mov al,SEQ_PLANE_ENABLE;
		mov ah,0fh;
		out dx,ax;
		les di,draw_buffer;
		mov al,BYTE PTR color;
		mov ah,al;
		mov cx,320*400/8;
		rep stosw;
		}
	}
}

void set_pixel(int x, int y, int color)
{
	if(current_video_mode == GRAPHICS_MODE13)
	{
		draw_buffer[(y<<8)+(y<<6)+x] = (unsigned char)color;
	}
	if(current_video_mode == GRAPHICS_MODEX)
	{
		outport(SEQUENCER, 0x02+(1<<((x%4)+8)));
		draw_buffer[(y<<6)+(y<<4)+(x>>2)] = (unsigned char)color;
	}

	if(current_video_mode == GRAPHICS_MODEZ)
	{
		_asm	{
			mov dx,SEQUENCER;
			mov al,SEQ_PLANE_ENABLE;
			mov cl,BYTE PTR x;
			and cl,03h;
			mov ah,1;
			shl ah,cl;
			out dx,ax;
			}
		draw_buffer[(y<<6)+(y<<4)+(x>>2)] = (unsigned char)color;
	}
}

int get_pixel(int x, int y)
{
	if(current_video_mode == GRAPHICS_MODE13)
	{
		return((int)(draw_buffer[(y<<8)+(y<<6)+x]));
	}

	if(current_video_mode == GRAPHICS_MODEZ)
	{
		_asm	{
			mov dx,SEQUENCER;
			mov al,SEQ_PLANE_ENABLE;
			mov cl,BYTE PTR x;
			and cl,03h;
			mov ah,1;
			shl ah,cl;
			out dx,ax;
			}
		return((int)(draw_buffer[(y<<6)+(y<<4)+(x>>2)]));
	}
	return -1;
}

void draw_line_h(int x1, int x2, int y, int color)
{
	if(current_video_mode == GRAPHICS_MODE13)
	{
		int temp;

		if(x1>x2)
		{
			temp = x1;
			x1 = x2;
			x2 = temp;
		}

		_fmemset((unsigned char far *)(draw_buffer + ((y<<8) +(y<<6))+x1),(unsigned char)color, x2-x1+1);
	}
	if(current_video_mode == GRAPHICS_MODEX)
	{
		int temp;
		unsigned int startSequence =0;
		unsigned int endSequence=0;
		
		if(x1>x2)
		{
			temp=x1;
			x1=x2;
			x2=temp;
		}

		if(x2>>2 != x1>>2)
		{
			_asm 	{
						mov dx, SEQUENCER
						mov al, 02h
						mov cl, BYTE PTR x1
						and cl, 03h
						mov bl, 0fh
						shl bl, cl
						mov ah, bl
						out dx, ax
					}

			draw_buffer[(y<<6)+(y<<4)+(x1>>2)] = (unsigned char)color;

			_asm 	{
						mov dx, SEQUENCER
						mov ax, 0f02h
						out dx, ax
					}
			
			_fmemset((unsigned char far *)(draw_buffer + ((y<<6) +(y<<4)+(x1>>2)+1)),(unsigned char)color, (x2-x2%4-x1-4+x1%4)>>2);
			
			_asm 	{ 
						mov dx, SEQUENCER
						mov al, 02h
						mov bl, BYTE PTR x2
						and bl, 03h
						mov cl, 03h
						sub cl, bl
						mov bl, 0fh
						shr bl, cl
						mov ah, bl
						out dx, ax
					}

			draw_buffer[(y<<6)+(y<<4)+(x2>>2)] = (unsigned char)color;
		}
		else
		{
			int i = 0;
			int sequence = 0x0;
			for(i = x1%4; i<= x2%4;i++)
			{
				sequence += 0x01<<i;
			}
			sequence = sequence<<8;
			outport(SEQUENCER, sequence+0x02);
			draw_buffer[(y<<6)+(y<<4)+(x1>>2)] = (unsigned char)color;
		}
	}
}

void draw_line_v(int x, int y1, int y2, int color)
{
	if(current_video_mode == GRAPHICS_MODE13)
	{
		int temp;
		int length;
		int i;
		unsigned char far *start_offset;

		if(y1>y2)
		{
			temp = y1;
			y1 = y2;
			y2 = temp;
		}

		start_offset = draw_buffer + ((y1<<8) + (y1<<6)) + x;

		length = y2-y1;

		for(i=0;i<=length;i++)
		{
			*start_offset = (unsigned char)color;
			start_offset+=320;
		}
	}
	if(current_video_mode == GRAPHICS_MODEX)
	{
		int temp;
		int length;
		int i;
		unsigned char far *start_offset;

		if(y1>y2)
		{
			temp = y1;
			y1 = y2;
			y2 = temp;
		}

		start_offset = draw_buffer + ((y1<<6) + (y1<<4) + (x>>2));

		length = y2-y1;
		outport(SEQUENCER, 0x02+(1<<((x%4)+8)));

		for(i=0;i<=length;i++)
		{
			*start_offset = (unsigned char)color;
			start_offset+=80;
		}
	}
}

void flip_front_page()
{	
	copy_vmem_to_dbuffer_latched(draw_buffer, frame_buffer, (SCREEN_WIDTH/4)*SCREEN_HEIGHT);
}

void fill_rectangle(int x1, int x2, int y1, int y2, int color)
{
	if(current_video_mode == GRAPHICS_MODE13)
	{
		int temp;
		int i;
		int height;
		unsigned char far * start_offset;

		if(x1>x2)
		{
			temp = x1;
			x1 = x2;
			x2 = temp;
		}
		if(y1>y2)
		{
			temp = y1;
			y1 = y2;
			y2 = temp;
		}

		start_offset = draw_buffer + ((y1<<8) + (y1<<6)) + x1;
		height = y2-y1;

		for(i=0;i<=height;i++)
		{
				_fmemset(start_offset,(unsigned char)color, x2-x1+1);
				start_offset+=320;
		}
	}
	if(current_video_mode == GRAPHICS_MODEX)
	{
		int temp;
		int i;
		int height;
		unsigned char far * start_offset;

		if(x1>x2)
		{
			temp = x1;
			x1 = x2;
			x2 = temp;
		}
		if(y1>y2)
		{
			temp = y1;
			y1 = y2;
			y2 = temp;
		}

		start_offset = draw_buffer + ((y1<<6) + (y1<<4));
		height = y2-y1;

		for(i=0;i<=height;i++)
		{
			if(x2>>2 != x1>>2)
			{
				_asm 	{
							mov dx, SEQUENCER
							mov al, 02h
							mov cl, BYTE PTR x1
							and cl, 03h
							mov bl, 0fh
							shl bl, cl
							mov ah, bl
							out dx, ax
						}
						
				*(start_offset+(x1>>2)) = (unsigned char)color;

				_asm 	{
							mov dx, SEQUENCER
							mov ax, 0f02h
							out dx, ax
						}
				
				_fmemset((unsigned char far *)(start_offset+(x1>>2)+1),(unsigned char)color, (x2-x2%4-x1-4+x1%4)>>2);
				
				_asm 	{ 
							mov dx, SEQUENCER
							mov al, 02h
							mov bl, BYTE PTR x2
							and bl, 03h
							mov cl, 03h
							sub cl, bl
							mov bl, 0fh
							shr bl, cl
							mov ah, bl
							out dx, ax
						}

				*(start_offset+(x2>>2)) = (unsigned char)color;
				start_offset+=80;
			}
			else
			{
				int i = 0;
				int sequence = 0x0;
				for(i = x1%4; i<= x2%4;i++)
				{
					sequence += 0x01<<i;
				}
				sequence = sequence<<8;
				outport(SEQUENCER, sequence+0x02);
				*start_offset = (unsigned char)color;
				start_offset +=80;
			}
		}
	}
}

void print_char(int xc, int yc, char c, int color, int transparent)
{
	if(current_video_mode == GRAPHICS_MODE13)
	{
		int offset, x, y;

		unsigned char far *work_char;

		unsigned char bit_mask;

		work_char = rom_char_set + c*8;

		offset = (yc<<8)+(yc<<6)+xc;

		for(y=0; y<8; y++)
		{
			bit_mask = 0x80;
			for(x=0; x<8; x++)
			{
				if((*work_char & bit_mask))
					draw_buffer[offset+x] = (unsigned char)color;
				else
				if(!transparent)
					draw_buffer[offset+x] = 0;

				bit_mask = (bit_mask>>1);
			}
			offset+=320;
			work_char++;
		}
	}

	if(current_video_mode == GRAPHICS_MODEX)
	{
		int offset, x, y;

		unsigned char far *work_char;

		unsigned char bit_mask;
		unsigned char sequence;

		work_char = rom_char_set + c*8;

		offset = (yc<<6)+(yc<<4);
		
		for(y=0; y<8; y++)
		{
			bit_mask = 0x88;
			for(x=0; x<4; x++)
			{
				sequence = (xc+x)%4;
				outport(SEQUENCER, ((0x1<<sequence)<<8)+0x02);
				if((*work_char & (bit_mask & 0xf0)))
					draw_buffer[offset+((xc+x)>>2)] = (unsigned char)color;
				else
				if(!transparent)
					draw_buffer[offset+((xc+x)>>2)] = 0;
		
				if((*work_char & (bit_mask & 0x0f)))
					draw_buffer[offset+((xc+x)>>2)+1] = (unsigned char)color;
				else
				if(!transparent)
					draw_buffer[offset+((xc+x)>>2)+1] = 0;
				
				bit_mask = (bit_mask>>1);
			}
			offset+=80;
			work_char++;
		}
	}
}

void print_string(int x, int y, int color, char *string, int transparent)
{
	if(current_video_mode == GRAPHICS_MODE13 || current_video_mode == GRAPHICS_MODEX)
	{
		int i, length;

		length = strlen(string);

		for(i=0; i<length; i++)
		{
			print_char(x+(i<<3), y, string[i], color, transparent);
		}
	}
}

void load_pgm(char* filename, unsigned char far * allocated_mem, int x_size, int y_size)
{	
	FILE * file;
	int current_char;
	int i;
	int j;

	int x;
	int y;

	int current_number = 0;

	int format_error = 0;

	int file_x = 0;
	int file_y = 0;

	file = fopen(filename, "r");

	if(file == NULL)
	{
		set_graphics_mode(TEXT_MODE);
		printf("ERROR OPENING FILE: %s", filename);
		exit(EXIT_FAILURE);
	}

	current_char = fgetc(file);
	if(current_char != 'P')format_error = 1;
	current_char = fgetc(file);
	if(current_char != '2')format_error = 1;
	current_char = fgetc(file);
	if(current_char != '\n')format_error = 1;

	for(i= 0; i<4;i++)
	{	
		current_char = fgetc(file);
		if(current_char >= 48 && current_char <= 58)
		{
			file_x = file_x*10 + current_char - 48;
			
		}
		else if(current_char == 32)
		{
			break;
		}
		else
		{
			format_error = 1;
		}
	}

	if(file_x != x_size)
	{
		set_graphics_mode(TEXT_MODE);
		printf("WRONG X SIZE FOR %s", filename);
		printf("EXPECTING %d", x_size);
		printf("GOT %d", file_x);
		exit(EXIT_FAILURE);
	}
	for(i= 0; i<4;i++)
	{	
		current_char = fgetc(file);
		if(current_char >= 48 && current_char <= 58)
		{
			file_y = file_y*10 + current_char - 48;
		}
		else if(current_char == '\n')
		{
			break;
		}
		else
		{
			format_error = 1;
		}
	}

	if(file_y != y_size)
	{
		set_graphics_mode(TEXT_MODE);
		printf("WRONG Y SIZE FOR %s", filename);
		printf("EXPECTING %d", y_size);
		printf("GOT %d", file_y);
		exit(EXIT_FAILURE);
	}

	current_number = 0;

	for(i= 0; i<4;i++)
	{	
		current_char = fgetc(file);
		if(current_char >= 48 && current_char <= 58)
		{
			current_number = current_number*10 + current_char - 48;
		}
		else if(current_char == '\n')
		{
			break;
		}
		else
		{
			format_error = 1;
		}
	}

	current_number = 0;

	for(y = 0; y< y_size; y++)
	{
		for(x = 0; x < x_size; x++)
		{
			outport(SEQUENCER, ((0x1<<(x%4))<<8)+0x02);
			for(j = 0; j<4; j++)
			{	
				current_char = fgetc(file);

				if(current_char >= 48 && current_char <= 58)
				{
					current_number = current_number*10 + current_char - 48;
				}
				else if(current_char == '\n')
				{
					break;
				}
				else
				{
					format_error = 1;
				}
			}
			*(allocated_mem+(y*x_size>>2)+(x>>2)) = (unsigned char)current_number;		
			current_number = 0;
		}
	}

	if(format_error == 1)
	{
		set_graphics_mode(TEXT_MODE);
		printf("FILE IS NOT PGM, OR IS CORRUPTED SOMEHOW, CHECK YO FILES YO, filename: ", filename);
		exit(EXIT_FAILURE);
	}
}

void copy_vmem_to_dbuffer_latched (	unsigned char far* source,
									unsigned char far* destination,
									int bytes)
{
	unsigned char current_pixel = 0;

	int i;

	outport(GFX_CONTROLLER, 0x08);
	outport(SEQUENCER, (0xff<<8)+0x02);

	for(i=0; i<bytes; i++)
	{	
		current_pixel = *(source+i);		
		*(destination+i) = 0;	
	}

	outport(GFX_CONTROLLER + 1, 0x0ff);	
}

void copy_vmem_to_dbuffer(	unsigned char far * location, 
							int x_pos_fbuffer, 
							int y_pos_fbuffer, 
							int x_offset_start_vmem, 
							int x_offset_end_vmem,
							int y_offset_start_vmem,
							int y_offset_end_vmem,
							int x_vmem_size)
{
	unsigned char current_pixel = 0;

	int src_plane = 0;
	int dest_plane = 0;

	int dest_offset = 0;
	int src_offset = 0;

	int x;
	int y;

	int i;
	int j;

	src_plane = x_offset_start_vmem%4;
	dest_plane = x_pos_fbuffer%4;

	for(i = 0; i<4; i++, src_plane++, dest_plane++)
	{
		if(src_plane >=4)
		{
			src_plane = 0;
			src_offset += 1;
		}
		if(dest_plane >=4)
		{
			dest_plane = 0;
			dest_offset += 1;
		}

		outport(GFX_CONTROLLER, (src_plane<<8)+0x04);
		outport(SEQUENCER, ((0x1<<dest_plane)<<8)+0x02);

		for(x=0; x<=(x_offset_end_vmem-x_offset_start_vmem)-i; x+=4)	
		{
			for(y=0; y<=(y_offset_end_vmem-y_offset_start_vmem); y++)
			{		
				current_pixel = *(	location +
									src_offset +
									((y+y_offset_start_vmem)*(x_vmem_size>>2)) +
									((x+x_offset_start_vmem)>>2)   );
				
				if(current_pixel != TRANSPARENT_INDEX)
				{
					if(x<x_offset_end_vmem-x_offset_start_vmem)
					{
						
						*(	draw_buffer + 
							(x_pos_fbuffer>>2) + 
							dest_offset +
							y_pos_fbuffer*(SCREEN_WIDTH>>2) + 
							y*(SCREEN_WIDTH>>2) + 
							(x>>2))
							 = current_pixel;
					}
				}
			}	
		}
	}
}
