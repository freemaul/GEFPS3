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
#ifndef GEFPS3_GEFPS3_H
	#error "Do not include this file directly, use GEFPS3/GEFPS3.h"
#endif

#ifndef GEFPS3_CONSOLE_H
#define GEFPS3_CONSOLE_H

//#include <psl1ght/types.h>
#include <ft2build.h>
#include FT_FREETYPE_H 

typedef enum {
	RGB = 0,
	ARGB = 1,
} console_mode;

typedef struct {
	int		textsize;
	int		height,width;
	int		posx,posy;
	console_mode	mode;
	char		color[4];
	char		background_color[4];
	int		background;
	FT_Library	ft_library;
	FT_Face		ft_face;
}console_attribute;

#define CONSOLE_ATTR_DEFAULT {0,0,0,0,0,RGB,{0,0,0,0},{0,0,0,0},0,NULL,NULL}

typedef struct {
	console_attribute	attr;
	char**			buffer;
	int			nb_line,nb_car_per_line;
	int			line_current;
	int			line_next;
	int			line_top;
	int			car_current;
	FT_Library		ft_library;
	FT_Face			ft_face;
}console;


console*	Create_console(console_attribute);
void		Delete_console(console*);
int		Console_get_textsize(const console*);
int		Console_get_height(const console*);
int		Console_get_width(const console*);
int		Console_get_posx(const console*);
int		Console_get_posy(const console*);
void		Console_set_posx(console*,int);
void		Console_set_posy(console*,int);
void		Console_set_color(console*,const char*,size_t);
void		Console_enable_background(console*,int);
void		Console_set_background_color(console*,const char*,size_t);


void		Console_putc(console*,char);
void		Console_print(console*,const char*);
void		Console_printf(console* cons,const char* fmt,...);
void		Console_clear(console*);
void		Console_draw(const console*,char*,int,int);



#endif

