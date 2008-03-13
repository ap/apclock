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
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "dockapp.h"

Display * dpy;

int handle_x11_error(Display * dpy)
{
	/*warnx("X error");*/ /* XXX mysteriously, this function isn't always called */
	/* we don't do anything here, we just don't want "connection lost" messages */
	exit(EXIT_SUCCESS);
}

void init_dockapp(Window * window, Window * icon_window, int width, int height)
{
	int dummy;
	XSizeHints size_hints;
	unsigned long bg;

	dpy = XOpenDisplay(NULL);
	if (dpy == NULL)
		errx(EXIT_FAILURE, "could not open display %s", XDisplayName(NULL));

	XSetIOErrorHandler(handle_x11_error);

	size_hints.flags = PMinSize | PMaxSize | PPosition;
	size_hints.min_width = size_hints.max_width = width;
	size_hints.min_height = size_hints.max_height = height;

	XWMGeometry(dpy, DefaultScreen(dpy), NULL, NULL, 0, &size_hints, &dummy, &dummy, &width, &height, &dummy);

	bg = BlackPixel(dpy, DefaultScreen(dpy));
	*window = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, width, height, 0, bg, bg);
	*icon_window = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, width, height, 0, bg, bg);

	XSetWMNormalHints(dpy, *window, &size_hints);
}


void done_dockapp(Window window, Window icon_window)
{
	XDestroyWindow(dpy, window);
	XDestroyWindow(dpy, icon_window);
	XCloseDisplay(dpy);
}


void set_window_name(Window w, int argc, char * argv[], char * app_name)
{
	XTextProperty window_name;

	if (XStringListToTextProperty(&app_name, 1, &window_name) == 0)
		errx(EXIT_FAILURE, "can't allocate window name");

	XSetCommand(dpy, w, argv, argc);
	XStoreName(dpy, w, app_name);
	XSetIconName(dpy, w, app_name);
	XSetWMName(dpy, w, &window_name);
}


void set_window_class_hints(Window w, char * app_name, char * app_class)
{
	XClassHint class_hints;

	class_hints.res_name = app_name;
	class_hints.res_class = app_class;

	XSetClassHint(dpy, w, &class_hints);
}

Window set_window_withdrawn_state(Window window, Window icon_window, Bool is_withdrawn)
{
	XWMHints wm_hints;
	Window main_window;

	wm_hints.window_group = window;

	if (is_withdrawn) {
		wm_hints.initial_state = WithdrawnState;
		wm_hints.icon_window = icon_window;
		wm_hints.icon_x = wm_hints.icon_y = 0;
		wm_hints.flags = StateHint | IconWindowHint | IconPositionHint | WindowGroupHint;
		main_window = icon_window;
	}
	else {
		wm_hints.initial_state = NormalState;
		main_window = window;
	}

	XSetWMHints(dpy, window, &wm_hints);

	return main_window;
}


/* cf. http://www.linuxquestions.org/questions/programming-9/xnextevent-select-409355/
 *
 * If no event is pending, and no data comes in on the X socket for 0.1s, return false
 * Else store the event at the provided pointer and return true
 *
 * */
int event_pending_within_timeout(XEvent * e, long sec, long usec)
{
	if (!XEventsQueued(dpy, QueuedAlready)) {
		fd_set fds;
		struct timeval tv;
		int dpyfd = ConnectionNumber(dpy);

		FD_ZERO(&fds);
		FD_SET(dpyfd, &fds);

		tv.tv_sec = sec;
		tv.tv_usec = usec;

		if (0 == select(dpyfd + 1, &fds, NULL, NULL, &tv))
			return 0;
	}

	XNextEvent(dpy, e);
	return 1;
}
