/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1996-2001 Grace Development Team
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

/*
 * Dummy (Null) driver for Grace
 */

#include <stdio.h>
#include <stdlib.h>
 
#include "defines.h"
#include "draw.h"
#include "device.h"
#include "devlist.h"
#include "dummydrv.h"

static Device_entry dev_dummy = {DEVICE_TERM,
          "Dummy",
          dummyinitgraphics,
          NULL,
          NULL,
          "",
          TRUE,
          FALSE,
          {DEFAULT_PAGE_WIDTH, DEFAULT_PAGE_HEIGHT, 72.0},
          NULL
         };

int register_dummy_drv(Canvas *canvas)
{
    return register_device(canvas, &dev_dummy);
}

int dummyinitgraphics(Canvas *canvas)
{
    /* device-dependent routines */
    canvas->devupdatecmap    = NULL;
    
    canvas->devdrawpixel     = dummy_drawpixel;
    canvas->devdrawpolyline  = dummy_drawpolyline;
    canvas->devfillpolygon   = dummy_fillpolygon;
    canvas->devdrawarc       = dummy_drawarc;
    canvas->devfillarc       = dummy_fillarc;
    canvas->devputpixmap     = dummy_putpixmap;
    canvas->devputtext       = dummy_puttext;
    
    canvas->devleavegraphics = dummy_leavegraphics;

    return RETURN_SUCCESS;
}

void dummy_drawpixel(const Canvas *canvas, const VPoint *vp){}
void dummy_drawpolyline(const Canvas *canvas,
    const VPoint *vps, int n, int mode){}
void dummy_fillpolygon(const Canvas *canvas, const VPoint *vps, int nc){}
void dummy_drawarc(const Canvas *canvas,
    const VPoint *vp1, const VPoint *vp2, int a1, int a2){}
void dummy_fillarc(const Canvas *canvas,
    const VPoint *vp1, const VPoint *vp2, int a1, int a2, int mode){}
void dummy_putpixmap(const Canvas *canvas,
    const VPoint *vp, int width, int height, 
    char *databits, int pixmap_bpp, int bitmap_pad, int pixmap_type){}
void dummy_puttext(const Canvas *canvas,
    const VPoint *vp, const char *s, int len, int font,
    const TextMatrix *tm, int underline, int overline, int kerning){}

void dummy_leavegraphics(const Canvas *canvas){}
