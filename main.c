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

#include <err.h>
#include <stdlib.h>
#include <getopt.h>
#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include "main.h"
#include "dockapp.h"
#include "hand.h"
#include "cairoutil.h"
#include "default_bg.png_c"

#define NAME "apclock"
#define CLASS "APClock"


void init_hands(Hand * hours, Hand * mins, Hand * secs, int size, cairo_t * cr)
{
	double scale = size/64.0;

	{
		double length     = 14;
		double base_width =  5;
		double tip_width  =  1.5;
		hours->value = -1;
		hours->r = hours->g = hours->b = 4.0/5.0;
		hours->path = create_rect_hand_path(cr, scale, length + base_width/2.0, base_width, tip_width, base_width/2.0);
	}

	{
		double length     = 20;
		double base_width =  3.5;
		double tip_width  =  1.75;
		mins->value = -1;
		mins->r = mins->g = mins->b = 5.0/9.0;
		mins->path = create_rect_hand_path(cr, scale, length + base_width/2.0, base_width, tip_width, base_width/2.0);
	}

	{
		double length     =   5;
		double base_width =   0;
		double tip_width  =   6;
		double offset     = -21;
		secs->value = -1;
		secs->r = secs->g = secs->b = 8.0/9.0;
		secs->path = create_rect_hand_path(cr, scale, length, base_width, tip_width, offset);
	}
}


int main(int argc, char *argv[])
{
	int opt_under_windowmaker = True;
	int opt_show_seconds = True;

	{
		int opt;

		while ((opt = getopt(argc, argv, "sw")) != -1) {
			switch (opt) {
				case 's': opt_show_seconds = False;      break;
				case 'w': opt_under_windowmaker = False; break;
				default: errx(EXIT_FAILURE, "error parsing command line arguments");
			}
		}

		if (optind < argc)
			errx(EXIT_FAILURE, "extraneous command line arguments");
	}

	{
		unsigned char * default_bg_png_ptr = default_bg_png;
		cairo_surface_t * clockpane_surface = cairo_image_surface_create_from_png_stream(read_memory_buffer, &default_bg_png_ptr);
		int sw = cairo_image_surface_get_width(clockpane_surface);
		int sh = cairo_image_surface_get_height(clockpane_surface);

		{
			Window window, icon_window;
			Window main_window;
			Atom wm_delete_window;

			init_dockapp(&window, &icon_window, sw, sh);
			set_window_name(window, argc, argv, NAME);
			set_window_class_hints(window, NAME, CLASS);
			set_window_class_hints(icon_window, NAME, CLASS);
			main_window = set_window_withdrawn_state(window, icon_window, opt_under_windowmaker);

			XSelectInput(dpy, main_window, ExposureMask | SubstructureNotifyMask | StructureNotifyMask);

			wm_delete_window = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
			XSetWMProtocols(dpy, main_window, &wm_delete_window, 1);

			XMapWindow(dpy, window);

			{
				cairo_surface_t * buffer_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, sw, sh);
				cairo_t * buffer_cr = cairo_create(buffer_surface);

				cairo_surface_t * main_window_surface = cairo_xlib_surface_create(dpy, main_window, DefaultVisual(dpy, DefaultScreen(dpy)), sw, sh);
				cairo_t * main_window_cr = cairo_create(main_window_surface);

				Pixmap mask_pixmap = XCreatePixmap(dpy, DefaultRootWindow(dpy), sw, sh, 1);
				cairo_surface_t * mask_surface = cairo_xlib_surface_create_for_bitmap(dpy, mask_pixmap, DefaultScreenOfDisplay(dpy), sw, sh);
				cairo_t * mask_cr = cairo_create(mask_surface);

				cairo_set_source_surface(main_window_cr, buffer_surface, 0, 0);

				{
					Hand hours, mins, secs;
					init_hands(&hours, &mins, &secs, sw > sh ? sw : sh, buffer_cr);

					while (1) {
						XEvent e;
						int need_repaint = False;

						if (event_pending_within_timeout(&e, 0, 100000L)) /* 0.1s timeout */
							switch (e.type) {
								case Expose:
									need_repaint = e.xexpose.count == 0;
									break;
								case ClientMessage: /* XXX somehow we never ever actually receive this */
									if (e.xclient.data.l[0] == wm_delete_window) goto done;
									break;
								default:
									break;
							}

						if (move_hands(&hours, &mins, &secs)) {
							clear_with_surface(buffer_cr, clockpane_surface);
							render_hand(buffer_cr, hours);
							render_hand(buffer_cr, mins);
							if (opt_show_seconds) render_hand(buffer_cr, secs);
							need_repaint = True;
						}

						if (need_repaint) {
							cairo_paint(main_window_cr);
							bitmap_from_alpha(mask_cr, buffer_surface);
							XShapeCombineMask(dpy, main_window, ShapeBounding, 0, 0, mask_pixmap, ShapeSet);
							XSync(dpy, False);

							/* no point painting multiple times if Expose events have come
							 * in faster than we manage to obey them, so discard the lot */
							while (XCheckTypedWindowEvent(dpy, main_window, Expose, &e));
						}
					}
				}

				done:
				cairo_destroy(buffer_cr);
				cairo_surface_destroy(buffer_surface);

				cairo_destroy(main_window_cr);
				cairo_surface_destroy(main_window_surface);

				cairo_destroy(mask_cr);
				cairo_surface_destroy(mask_surface);
				XFreePixmap(dpy, mask_pixmap);
			}

			done_dockapp(window, icon_window);
		}

		cairo_surface_destroy(clockpane_surface);
	}

	return EXIT_SUCCESS;
}
