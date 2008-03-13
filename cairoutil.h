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

#ifndef ___CAIROUTIL_H
#define ___CAIROUTIL_H 1

cairo_status_t read_memory_buffer(void *, unsigned char *, unsigned int);
void render_hand(cairo_t *, Hand);
cairo_path_t * create_rect_hand_path(cairo_t *, double, double, double, double, double);
cairo_path_t * create_disc_hand_path(cairo_t *, double, double, double);
void clear_with_surface(cairo_t *, cairo_surface_t *);
void bitmap_from_alpha(cairo_t *, cairo_surface_t *);

#endif
