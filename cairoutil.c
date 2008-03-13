/*
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <string.h>
#include <X11/Xlib.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include <math.h>
#include "hand.h"

cairo_status_t read_memory_buffer(void * srcptr, unsigned char * data, unsigned int length)
{
	memcpy(data, *(void**)srcptr, length);
	*(unsigned char**)srcptr += length;
	return CAIRO_STATUS_SUCCESS;
}


void render_hand(cairo_t * cr, Hand h)
{
	cairo_surface_t * t = cairo_get_target(cr);
	int sw = cairo_image_surface_get_width(t);
	int sh = cairo_image_surface_get_height(t);

	cairo_save(cr);
	cairo_translate(cr, sw/2, sh/2);
	cairo_rotate(cr, h.angle);
	cairo_append_path(cr, h.path);
	cairo_set_source_rgb(cr, h.r, h.g, h.b);
	cairo_fill(cr);
	cairo_new_path(cr);
	cairo_restore(cr);
}


cairo_path_t * create_rect_hand_path(cairo_t * cr, double scale, double length, double base_width, double tip_width, double offset)
{
	cairo_path_t * path;

	cairo_save(cr);
	cairo_new_path(cr);
	cairo_move_to(cr, scale * -tip_width/2.0,  scale * (-length + offset) );
	cairo_line_to(cr, scale *  tip_width/2.0,  scale * (-length + offset) );
	cairo_line_to(cr, scale *  base_width/2.0, scale * offset);
	cairo_line_to(cr, scale * -base_width/2.0, scale * offset);
	cairo_close_path(cr);
	path = cairo_copy_path(cr);
	cairo_new_path(cr);
	cairo_restore(cr);

	return path;
}


cairo_path_t * create_disc_hand_path(cairo_t * cr, double scale, double offset, double radius)
{
	cairo_path_t * path;

	cairo_save(cr);
	cairo_new_path(cr);
	cairo_arc(cr, 0, scale * -offset, scale * radius, 0, 2*M_PI);
	cairo_close_path(cr);
	path = cairo_copy_path(cr);
	cairo_new_path(cr);
	cairo_restore(cr);

	return path;
}


void clear_with_surface(cairo_t * cr, cairo_surface_t * source)
{
	cairo_save(cr);
	cairo_set_source_surface(cr, source, 0, 0);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint(cr);
	cairo_restore(cr);
}


void bitmap_from_alpha(cairo_t * cr, cairo_surface_t * mask)
{
	cairo_save(cr);
	cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
	cairo_paint(cr);
	cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
	cairo_mask_surface(cr, mask, 0, 0);
	cairo_restore(cr);
}
