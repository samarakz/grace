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
 * Driver for the Grace Metafile format
 */

#include <config.h>

#include <stdio.h>
#include <string.h>

#include "defines.h"
#include "utils.h"
#include "draw.h"
#include "patterns.h"
#include "device.h"
#include "devlist.h"
#include "mfdrv.h"

static Device_entry dev_mf = {DEVICE_FILE,
          "Metafile",
          mfinitgraphics,
          NULL,
          NULL,
          "gmf",
          TRUE,
          FALSE,
          {DEFAULT_PAGE_WIDTH, DEFAULT_PAGE_HEIGHT, 72.0},
          NULL
         };

int register_mf_drv(Canvas *canvas)
{
    return register_device(canvas, &dev_mf);
}

int mfinitgraphics(Canvas *canvas)
{
    int i, j;
    Page_geometry *pg;
    
    /* device-dependent routines */
    canvas->devupdatecmap    = NULL;
    
    canvas->devdrawpixel     = mf_drawpixel;
    canvas->devdrawpolyline  = mf_drawpolyline;
    canvas->devfillpolygon   = mf_fillpolygon;
    canvas->devdrawarc       = mf_drawarc;
    canvas->devfillarc       = mf_fillarc;
    canvas->devputpixmap     = mf_putpixmap;
    canvas->devputtext       = mf_puttext;
    
    canvas->devleavegraphics = mf_leavegraphics;

    fprintf(canvas->prstream, "#GMF-%s\n", GMF_VERSION);

    fprintf(canvas->prstream, "FontResources {\n");
    for (i = 0; i < number_of_fonts(canvas); i++) {
        fprintf(canvas->prstream, "\t( %d , \"%s\" , \"%s\" )\n", 
            i, get_fontalias(canvas, i), get_fontfallback(canvas, i));
    }
    fprintf(canvas->prstream, "}\n");

    fprintf(canvas->prstream, "ColorResources {\n");
    for (i = 0; i < number_of_colors(canvas); i++) {
        RGB rgb;
        get_rgb(canvas, i, &rgb);
        fprintf(canvas->prstream, "\t( %d , \"%s\" , %d , %d , %d )\n", 
            i, get_colorname(canvas, i), rgb.red, rgb.green, rgb.blue);
    }
    fprintf(canvas->prstream, "}\n");

    fprintf(canvas->prstream, "PatternResources {\n");
    for (i = 0; i < number_of_patterns(canvas); i++) {
        fprintf(canvas->prstream, "\t( %d , ", i);
        for (j = 0; j < 32; j++) {
            fprintf(canvas->prstream, "%02x", pat_bits[i][j]);
        }
        fprintf(canvas->prstream, " )\n");
    }
    fprintf(canvas->prstream, "}\n");

    fprintf(canvas->prstream, "DashResources {\n");
    for (i = 0; i < number_of_linestyles(canvas); i++) {
        fprintf(canvas->prstream, "\t( %d , [ ", i);
        for (j = 0; j < dash_array_length[i]; j++) {
            fprintf(canvas->prstream, "%d ", dash_array[i][j]);
        }
        fprintf(canvas->prstream, "] )\n");
    }
    fprintf(canvas->prstream, "}\n");
    
    pg = get_page_geometry(canvas);
    fprintf(canvas->prstream, "InitGraphics { %.4f %ld %ld }\n",
        pg->dpi, pg->width, pg->height);
    
    return RETURN_SUCCESS;
}

void mf_setpen(const Canvas *canvas)
{
    Pen pen;
    
    getpen(canvas, &pen);
    fprintf(canvas->prstream, "SetPen { %d %d }\n", pen.color, pen.pattern);
}

void mf_setdrawbrush(const Canvas *canvas)
{
    fprintf(canvas->prstream, "SetLineWidth { %.4f }\n", getlinewidth(canvas));
    fprintf(canvas->prstream, "SetLineStyle { %d }\n", getlinestyle(canvas));
}

void mf_drawpixel(const Canvas *canvas, const VPoint *vp)
{
    mf_setpen(canvas);

    fprintf(canvas->prstream, "DrawPixel { ( %.4f , %.4f ) }\n", vp->x, vp->y);
}

void mf_drawpolyline(const Canvas *canvas, const VPoint *vps, int n, int mode)
{
    int i;
    
    mf_setpen(canvas);
    mf_setdrawbrush(canvas);
    
    fprintf(canvas->prstream, "DrawPolyline {\n");
    if (mode == POLYLINE_CLOSED) {
        fprintf(canvas->prstream, "\tClosed\n");
    } else {
        fprintf(canvas->prstream, "\tOpen\n");
    }
    for (i = 0; i < n; i++) {
        fprintf(canvas->prstream, "\t( %.4f , %.4f )\n", vps[i].x, vps[i].y);
    }
    fprintf(canvas->prstream, "}\n");
}

void mf_fillpolygon(const Canvas *canvas, const VPoint *vps, int nc)
{
    int i;
    
    mf_setpen(canvas);
    
    fprintf(canvas->prstream, "FillPolygon {\n");
    for (i = 0; i < nc; i++) {
        fprintf(canvas->prstream, "\t( %.4f , %.4f )\n", vps[i].x, vps[i].y);
    }
    fprintf(canvas->prstream, "}\n"); 
}

void mf_drawarc(const Canvas *canvas,
    const VPoint *vp1, const VPoint *vp2, int a1, int a2)
{
    mf_setpen(canvas);
    mf_setdrawbrush(canvas);
    
    fprintf(canvas->prstream,
        "DrawArc { ( %.4f , %.4f ) ( %.4f , %.4f ) %d %d }\n", 
        vp1->x, vp1->y, vp2->x, vp2->y, a1, a2);
}

void mf_fillarc(const Canvas *canvas,
    const VPoint *vp1, const VPoint *vp2, int a1, int a2, int mode)
{
    char *name;
    
    mf_setpen(canvas);
    
    /* FIXME - mode */
    if (mode == ARCFILL_CHORD) {
        name = "FillChord";
    } else {
        name = "FillPieSlice";
    }
    fprintf(canvas->prstream, "%s { ( %.4f , %.4f ) ( %.4f , %.4f ) %d %d }\n", 
        name, vp1->x, vp1->y, vp2->x, vp2->y, a1, a2);
}

void mf_putpixmap(const Canvas *canvas,
    const VPoint *vp, int width, int height, char *databits,
    int pixmap_bpp, int bitmap_pad, int pixmap_type)
{
    int i, j, k;
    long paddedW;
    int bit;
    char buf[16];
    
    if (pixmap_bpp == 1) {
        strcpy(buf, "Bitmap");
    } else {
        strcpy(buf, "Pixmap");
    }
    fprintf(canvas->prstream, "Put%s {\n", buf);
   
    if (pixmap_type == PIXMAP_TRANSPARENT) {
        strcpy(buf, "Transparent");
    } else {
        strcpy(buf, "Opaque");
    }
    
    fprintf(canvas->prstream, "\t( %.4f , %.4f ) %dx%d %s\n", 
                           vp->x, vp->y, width, height, buf);
    if (pixmap_bpp != 1) {
        for (k = 0; k < height; k++) {
            fprintf(canvas->prstream, "\t");
            for (j = 0; j < width; j++) {
                fprintf(canvas->prstream, "%02x", (databits)[k*width+j]);
            }
            fprintf(canvas->prstream, "\n");
        }
    } else {
        paddedW = PAD(width, bitmap_pad);
        for (k = 0; k < height; k++) {
            fprintf(canvas->prstream, "\t");
            for (j = 0; j < paddedW/bitmap_pad; j++) {
                for (i = 0; i < bitmap_pad; i++) {
                    bit = bin_dump(&databits[k*paddedW/bitmap_pad + j], i, bitmap_pad);
                    if (bit) {
                        fprintf(canvas->prstream, "X");
                    } else {
                        fprintf(canvas->prstream, ".");
                    }
                }
            } 
            fprintf(canvas->prstream, "\n");
        }
    }

    fprintf(canvas->prstream, "}\n"); 
}

void mf_puttext(const Canvas *canvas,
    const VPoint *vp, const char *s, int len, int font, const TextMatrix *tm,
    int underline, int overline, int kerning)
{
    int i;
    
    mf_setpen(canvas);
    
    fprintf(canvas->prstream, "PutText {\n");
    fprintf(canvas->prstream, "\t( %.4f , %.4f )\n", vp->x, vp->y); 

    fprintf(canvas->prstream, "\t %d %.4f %.4f %.4f %.4f %d %d %d %d \"", 
                        font,
                        tm->cxx, tm->cxy, tm->cyx, tm->cyy, 
                        underline, overline, kerning, len);
    for (i = 0; i < len; i++) {
        fputc(s[i], canvas->prstream);
    }
    fprintf(canvas->prstream, "\"\n");

    fprintf(canvas->prstream, "}\n"); 
}

void mf_leavegraphics(const Canvas *canvas)
{
    view v;
    
    get_bbox(canvas, BBOX_TYPE_GLOB, &v);
    fprintf(canvas->prstream, "LeaveGraphics { %.4f %.4f %.4f %.4f }\n",
        v.xv1, v.yv1, v.xv2, v.yv2);
}

