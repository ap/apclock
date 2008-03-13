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

#ifndef ___DOCKAPP_H
#define ___DOCKAPP_H 1

void init_dockapp(Window *, Window *, int, int);
void done_dockapp(Window, Window);
void set_window_name(Window, int, char *[], char *);
void set_window_class_hints(Window, char *, char *);
Window set_window_withdrawn_state(Window, Window, Bool);
int event_pending_within_timeout(XEvent *, long, long);

#endif
