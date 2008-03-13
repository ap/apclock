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

#include <time.h>
#include <math.h>
#include "hand.h"

int move_hands(Hand * hours, Hand * mins, Hand * secs)
{
	int h, m, s;
	struct tm *time_struct;
	time_t curtime;

	curtime = time(NULL);
	time_struct = localtime(&curtime);

	h = time_struct->tm_hour;
	m = time_struct->tm_min;
	s = time_struct->tm_sec;

	if (h == hours->value && m == mins->value && s == secs->value)
		return 0;

	hours->value = h;
	mins->value = m;
	secs->value = s;

	hours->angle = (M_PI / 6.0) * (double)h + (M_PI / 360.0) * (double)m;
	mins->angle = (M_PI / 30.0) * (double)m;
	secs->angle = (M_PI / 30.0) * (double)s;

	return 1;
}


void init_hands(Hand * hours, Hand * mins, Hand * secs, int size)
{
	double scale = size/64.0;

	hours->value      = -1;
	hours->base_width = scale * 5.5;
	hours->tip_width  = scale * 2;
	hours->length     = scale * 14;
	hours->r = hours->g = hours->b = 2.0/3.0;

	mins->value      = -1;
	mins->base_width = scale * 4.5;
	mins->tip_width  = scale * 1.5;
	mins->length     = scale * 20;
	mins->r = mins->g = mins->b = 4.0/7.0;

	secs->value       = -1;
	secs->base_width = scale * 2;
	secs->tip_width  = scale * 1.25;
	secs->length     = scale * 24.5;
	secs->r = secs->g = secs->b = 1.0;
}
