/*
    This file is part of GEFPS3.

    GEFPS3 is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GEFPS3 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with GEFPS3.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <GEFPS3/GEFPS3.h>

#include <stdlib.h>
#include <string.h>

console* Create_console(console_attribute attr)
{
	console* cons;
	int i;

	cons = (console*) malloc(sizeof(console));
	if(cons==NULL)
		return NULL;

	cons->attr = attr;	// make a copy of attributes
	// Init of members
	cons->line_current = 0;
	cons->line_next = 0;
	cons->car_current = 0;
	cons->nb_line = attr.height / attr.textsize;
	cons->nb_car_per_line = attr.width / attr.textsize;
	cons->line_top = -1;
	cons->ft_library	= NULL;
	cons->ft_face		= NULL;

	// Construction of buffer :
	// -> buffer[line][row]
	cons->buffer = (char**) malloc(sizeof(char*) * cons->nb_line);
	if(cons->buffer == NULL)
	{
		Delete_console(cons);
		return NULL;
	}
	for(i=0; i<(cons->nb_line) ; i++)
	{
		cons->buffer[i] = (char*) malloc(sizeof(char) * (cons->nb_car_per_line + 1));	// +1 for '\0'
		if(cons->buffer[i] == NULL)
		{
			Delete_console(cons);
			return NULL;
		}
		cons->buffer[i][0]='\0'; // first caracter NULL for no printing at the begining
		cons->buffer[i][cons->nb_car_per_line]='\0'; // should always be NULL
	}

	if ( attr.ft_library != NULL && attr.ft_face != NULL)	// library and face specifed in attributes
	{
		// use library and face from attributes
		cons->ft_library = attr.ft_library;
		cons->ft_face = attr.ft_face;
	}
	else
	{
		if ( FT_Init_FreeType( &(cons->ft_library) ) )
		{
			Delete_console(cons);
			return NULL;
		}
		// Load Face "/dev_hdd0/tmp/arial.ttf"
		// Not a good way, this default Face will be on memory
		if ( FT_New_Face( cons->ft_library, "/dev_hdd0/tmp/arial.ttf", 0, &(cons->ft_face) ) )
		{
			Delete_console(cons);
			return NULL;
		}
	}
	// Set font size
	if( FT_Set_Pixel_Sizes( cons->ft_face, 0, cons->attr.textsize) )
	{
		Delete_console(cons);
		return NULL;
	}

	return cons;
}

void Delete_console(console* cons)
{
	int i;

	if(cons->attr.ft_library == NULL || cons->attr.ft_face == NULL) // library and face not specifed by attributes
	{
		// library and face alloc by Make_console
		// so we delete them
		FT_Done_FreeType(cons->ft_library);
		FT_Done_Face(cons->ft_face);
	}
	
	// free buffer
	for(i=0;i<(cons->nb_line);i++)
		free(cons->buffer[i]);
	free(cons->buffer);
	// free console
	free(cons);
}

int Console_get_textsize(const console* cons)
{	return cons->attr.textsize;}

int Console_get_height(const console* cons)
{	return cons->attr.height;}

int Console_get_width(const console* cons)
{	return cons->attr.width;}

int Console_get_posx(const console* cons)
{	return cons->attr.posx;}

int Console_get_posy(const console* cons)
{	return cons->attr.posy;}

void Console_set_posx(console* cons, int posx)
{	cons->attr.posx = posx;}

void Console_set_posy(console* cons, int posy)
{	cons->attr.posy = posy;}

void Console_set_color(console* cons,const char* color,size_t size)
{
	int i;
	for(i=0;i<size;i++)
		cons->attr.color[i]=color[i];
}

void Console_set_background_color(console* cons, const char* color, size_t size)
{
	int i;
	for(i=0;i<size;i++)
		cons->attr.background_color[i]=color[i];
}

void Console_enable_background(console* cons, int enable)
{
	cons->attr.background = enable;
}

void Console_putc(console* cons,char c)
{
	if( (cons->car_current) >= (cons->nb_car_per_line) )
	{
		cons->car_current = 0;
		++(cons->line_next);
	}
	cons->line_next %= cons->nb_line;

	if(cons->line_current != cons->line_next)
	{
		if(cons->line_top >= 0)
			cons->line_top++;
		else
		{
			if(cons->line_next == 0)
				cons->line_top=1;
		}

		cons->line_current = cons->line_next;
		cons->car_current = 0;
	}

	switch(c)
	{
	case '\n':
		cons->buffer[cons->line_current][cons->car_current] = '\0';
		++(cons->line_next);
//		cons->car_current = 0;
		break;
	default:
		cons->buffer[cons->line_current][cons->car_current] = c;
		cons->buffer[cons->line_current][++(cons->car_current)] = '\0';
	}
}

void Console_print(console* cons,const char* msg)
{
	int i;
	int size;

	size=strlen(msg);
	// easy way, but not optimized to print message
	for(i=0;i<size;i++)
		Console_putc(cons,msg[i]);
}

char* make_message_for_printf(const char *fmt, ...) // from http://linux.about.com/library/cmd/blcmdl3_vsnprintf.htm
{
	/* Guess we need no more than 100 bytes. */
	int n, size = 100;
	char *p;
	va_list ap;
	if ((p = malloc (size)) == NULL)
		return NULL;
	while (1) {
		/* Try to print in the allocated space. */
		va_start(ap, fmt);
		n = vsnprintf (p, size, fmt, ap);
		va_end(ap);
		/* If that worked, return the string. */
		if (n > -1 && n < size)
			return p;
		/* Else try again with more space. */
		if (n > -1)    /* glibc 2.1 */
			size = n+1; /* precisely what is needed */
		else           /* glibc 2.0 */
			size *= 2;  /* twice the old size */
		if ((p = realloc (p, size)) == NULL)
			return NULL;
	}
}

void Console_printf(console* cons,const char* fmt,...)
{
	char* buffer;
	va_list ap;

	va_start(ap, fmt);
	buffer = make_message_for_printf(fmt,ap);
	va_end(ap);

	if(buffer==NULL)
		return;

	Console_print(cons,buffer);

	free(buffer);
}


void Console_clear(console* cons)
{
	int i;
	for(i=0;i<cons->nb_line;i++)
		cons->buffer[i][0]=0;
	cons->line_current = 0;
	cons->line_next = 0;
	cons->line_top = -1;
	cons->car_current = 0;
}

void my_draw_bitmap(char* buffer,int width,int height,FT_Bitmap* bitmap,int posx,int posy,int buffer_depth,const char color[4])
{
	int i,j,c;

	for(i=0;i<(bitmap->rows);i++)
	{
		for(j=0;j<(bitmap->width);j++)
		{
			for(c=0;c<buffer_depth;c++)
			{
				if(bitmap->buffer[j + i*(bitmap->width)] != 0)
					buffer[ (posx+j)*(buffer_depth) + (posy+i)*(buffer_depth)*width + c] = color[c];
			}
		}
	}
}


void Console_draw(const console* cons, char* buffer_out,int width,int height)
{
	int buffer_depth;

	int num_line;
	int num_car;
	int real_line;

	int i,j,c;

	int cur_posx = cons->attr.posx;
	int cur_posy = cons->attr.posy;

	const char* buffer_current = NULL;

	FT_GlyphSlot slot = cons->ft_face->glyph;

	switch(cons->attr.mode)
	{
	case RGB:
		buffer_depth = 3;
		break;
	case ARGB:
		buffer_depth = 4;
		break;
	default:
		return;
	}

	if(cons->attr.background!=0)
	{
		for(i=0;i<cons->attr.height;i++)
		{
			for(j=0;j<cons->attr.width;j++)
			{
				for(c=0;c<buffer_depth;c++)
				{
					buffer_out[ (cur_posx+j)*(buffer_depth) + (cur_posy+i)*(buffer_depth)*width + c] = cons->attr.background_color[c];
				}
			}
		}
	}

	for(num_line = 0 ; num_line < cons->nb_line ; num_line ++)
	{
		if(cons->line_top>0)
			real_line = (num_line + cons->line_top) % (cons->nb_line);
		else
			real_line = num_line;

		for(num_car = 0 ; cons->buffer[real_line][num_car]!=0;num_car++)
		{
			if(FT_Load_Char(cons->ft_face,cons->buffer[real_line][num_car], FT_LOAD_RENDER))
				continue;
			my_draw_bitmap( buffer_out,
					width,
					height,
					&slot->bitmap,
					cur_posx + slot->bitmap_left,
					cur_posy - slot->bitmap_top + cons->attr.textsize,
					buffer_depth,
					cons->attr.color);
			cur_posx += slot->advance.x >> 6;
//			cur_posx += cons->ft_face->max_advance_width;
		}
		cur_posx = cons->attr.posx;
		cur_posy += cons->attr.textsize;
//		cur_posy += cons->ft_face->max_advance_height;
	}
}


