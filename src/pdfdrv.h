/*
 * Grace - Graphics for Exploratory Data Analysis
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-95 Paul J Turner, Portland, OR
 * Copyright (c) 1996-98 GRACE Development Team
 * 
 * Maintained by Evgeny Stambulchik <fnevgeny@plasma-gate.weizmann.ac.il>
 * 
 * 
 *                           All Rights Reserved
 * 
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 * 
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "defines.h"

int pdfinitgraphics(void);

#if defined(NONE_GUI)
#  define pdf_gui_setup NULL
#else
void pdf_gui_setup(void);
#endif

void pdf_drawpolyline(VPoint *vps, int n, int mode);
void pdf_fillpolygon(VPoint *vps, int nc);
void pdf_drawarc(VPoint vp1, VPoint vp2, int a1, int a2);
void pdf_fillarc(VPoint vp1, VPoint vp2, int a1, int a2);
void pdf_putpixmap(VPoint vp, int width, int height, 
     char *databits, int pixmap_bpp, int bitmap_pad, int pixmap_type);
void pdf_puttext(VPoint start, VPoint end, double size, 
                                            CompositeString *cstring);

void pdf_leavegraphics(void);

