/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1996-2000 Grace Development Team
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
 * Driver for the Scalable Vector Graphics Format from W3C
 */

#include <config.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "defines.h"
#include "utils.h"
#include "cmath.h"
#include "draw.h"
#include "graphs.h"
#include "device.h"
#include "devlist.h"
#include "patterns.h"
#include "svgdrv.h"

extern FILE *prstream;

static Device_entry dev_svg = {
    DEVICE_FILE,
    "SVG",
    svginitgraphics,
    NULL,
    NULL,
    "svg",
    TRUE,
    FALSE,
    {DEFAULT_PAGE_WIDTH, DEFAULT_PAGE_HEIGHT, 72.0},
    NULL
};

typedef struct {
    double side;
    int   *pattern_defined;
    int   *pattern_empty;
    int   *pattern_full;
    int    group_is_open;
    double line_width;
    Pen    pen;
    int    fillrule;
    int    linecap;
    int    linejoin;
    int    linestyle;
    int    draw;
    int    fill;
} Svg_data;

static int init_svg_data(void)
{
    Svg_data *data;
    int i;

    data = (Svg_data *) get_curdevice_data();
    if (data == NULL) {
        /* we need to perform the allocations */
        data = (Svg_data *) xrealloc(NULL, sizeof(Svg_data));
        if (data == NULL) {
            return RETURN_FAILURE;
        }

        data->pattern_defined = NULL;
        data->pattern_empty = NULL;
        data->pattern_full = NULL;

        set_curdevice_data((void *) data);
    }

    data->side = MIN2(page_width_pp, page_height_pp);

    data->pattern_defined = 
        xrealloc(data->pattern_defined, number_of_patterns()*SIZEOF_INT);
    data->pattern_empty   =
        xrealloc(data->pattern_empty,   number_of_patterns()*SIZEOF_INT);
    data->pattern_full    =
        xrealloc(data->pattern_full,    number_of_patterns()*SIZEOF_INT);
    for (i = 0; i < number_of_patterns(); i++) {
        data->pattern_defined[i] = FALSE;
        data->pattern_empty[i]   = FALSE;
        data->pattern_full[i]    = FALSE;
    }

    data->group_is_open = FALSE;
    data->line_width    = 0.0;
    data->pen.color     = 0;
    data->pen.pattern   = 0;
    data->fillrule      = 0;
    data->linecap       = 0;
    data->linejoin      = 0;
    data->linestyle     = 0;
    data->draw          = FALSE;
    data->fill          = FALSE;

    return RETURN_SUCCESS;
}

/*
 * SVG conventions :
 *   Y coordinates increase downwards
 *   angles increase counterclockwise
 */

/*
 * convert coordinate system
 */
static double convertX (double x)
{
    Svg_data *data;
    data = get_curdevice_data();
    return x*data->side;
}

static double convertY (double y)
{
    Svg_data *data;
    data = get_curdevice_data();
    return (1.0 - y)*data->side;
}

static double convertW (double width)
{
    Svg_data *data;
    data = get_curdevice_data();
    return width*data->side;
}

static double convertH (double height)
{
    Svg_data *data;
    data = get_curdevice_data();
    return height*data->side;
}

int register_svg_drv(void)
{
    return register_device(dev_svg);
}

static void define_pattern(int i, Svg_data *data)
{
#ifndef EXPERIMENTAL_SVG_PATTERNS
    data->pattern_full[i]  = TRUE;
    data->pattern_defined[i] = TRUE;
    return;
#else
    int j, k, l;

    if (data->pattern_defined[i] == TRUE) {
        return;
    }

    /* testing if the pattern is either empty or full */
    data->pattern_empty[i] = TRUE;
    data->pattern_full[i]  = TRUE;
    for (j = 0; j < 32; j++) {
        if (pat_bits[i][j] != 0x00) {
            data->pattern_empty[i] = FALSE;
        }
        if (pat_bits[i][j] != 0xff) {
            data->pattern_full[i] = FALSE;
        }
    }

    if (data->pattern_empty[i] != TRUE && data->pattern_full[i] != TRUE) {
        /* this is an horrible hack ! */
        /* we define pixels as squares in vector graphics */
        fprintf(prstream,
                "  <defs>\n   <pattern id=\"pattern%d\""
                " width=\"%d\" height=\"%d\"\">\n",
                i, 16, 16);
        for (j = 0; j < 256; j++) {
            k = j/16;
            l = j%16;
            if ((pat_bits[i][j/8] >> (j%8)) & 0x01) {
                /* the bit is set */
                fprintf(prstream,
                        "     <rect x=\"%d\" y=\"%d\""
                        " width=\"1\" height=\"1\"/>\n",
                        l, 15 - k);
            }
        }
        fprintf(prstream, "   </pattern>\n  </defs>\n");
    }

    data->pattern_defined[i] = TRUE;
#endif
}

/*
 * escape special characters
 */
static char *escape_specials(unsigned char *s, int len)
{
    static char *es = NULL;
    int i, elen = 0;

    elen = 0;
    for (i = 0; i < len; i++) {
        if (s[i] == '&') {
            elen += 4;
        } else if (s[i] == '<' || s[i] == '>') {
            elen += 3;
        }
        elen++;
    }

    es = xrealloc(es, (elen + 1)*SIZEOF_CHAR);

    elen = 0;
    for (i = 0; i < len; i++) {
        if (s[i] == '&') {
            es[elen++] = '&';
            es[elen++] = 'a';
            es[elen++] = 'm';
            es[elen++] = 'p';
            es[elen++] = ';';
        } else if (s[i] == '<') {
            es[elen++] = '&';
            es[elen++] = 'l';
            es[elen++] = 't';
            es[elen++] = ';';
        } else if (s[i] == '>') {
            es[elen++] = '&';
            es[elen++] = 'g';
            es[elen++] = 't';
            es[elen++] = ';';
        } else {
            es[elen++] = (char) s[i];
        }
    }
    es[elen] = '\0';

    return (es);
}

int svginitgraphics(void)
{
    /* device-dependent routines */
    devupdatecmap   = NULL;
    
    devdrawpixel    = svg_drawpixel;
    devdrawpolyline = svg_drawpolyline;
    devfillpolygon  = svg_fillpolygon;
    devdrawarc      = svg_drawarc;
    devfillarc      = svg_fillarc;
    devputpixmap    = svg_putpixmap;
    devputtext      = svg_puttext;

    devleavegraphics = svg_leavegraphics;

    if (init_svg_data() != RETURN_SUCCESS) {
        return RETURN_FAILURE;
    }

    fprintf(prstream, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
    fprintf(prstream, "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG December 1999//EN\"");
    fprintf(prstream, " \"http://www.w3.org/Graphics/SVG/SVG-19991203.dtd\">\n");
    fprintf(prstream, "<!-- generated by %s -->\n", bi_version_string());
    fprintf(prstream, "<svg xml:space=\"preserve\" ");
    fprintf(prstream, "width=\"%.4fin\" height=\"%.4fin\" viewBox=\"%.4f %.4f %.4f %.4f\">\n",
            page_width_in, page_height_in,
            0.0, 0.0, page_width_pp, page_height_pp);

    /* project description */
    if (get_project_description() != NULL) {
        fprintf(prstream, " <desc>%s</desc>\n", get_project_description());
    }
    
    return RETURN_SUCCESS;
}

static void svg_group_props (int draw, int fill)
{
    int i, needs_group;
    double lw;
    Pen pen;
    int fillrule, linecap, linejoin, linestyle;
    RGB *prgb;
    int red, green, blue;
    Svg_data *data;

    data = (Svg_data *) get_curdevice_data();

    /* do we need to redefine a group with new properties ? */
    needs_group = (data->group_is_open == TRUE) ? FALSE : TRUE;
    lw        = getlinewidth();
    fillrule  = getfillrule();
    linecap   = getlinecap();
    linejoin  = getlinejoin();
    linestyle = getlinestyle();
    if (fabs(lw - data->line_width) >= 1.0e-6*(1.0 + fabs(data->line_width))) {
        needs_group = TRUE;
    }
    pen = getpen();
    if ((pen.color != data->pen.color) || (pen.pattern != data->pen.pattern)) {
        needs_group = TRUE;
    }
    if (fillrule != data->fillrule) {
        needs_group = TRUE;
    }
    if (linecap != data->linecap) {
        needs_group = TRUE;
    }
    if (linejoin != data->linejoin) {
        needs_group = TRUE;
    }
    if (linestyle != data->linestyle) {
        needs_group = TRUE;
    }
    if ((draw != data->draw) || (fill != data->fill)) {
        needs_group = TRUE;
    }

    if (needs_group == TRUE) {
        /* we need to write the characteristics of the group */

        if (data->group_is_open == TRUE) {
            /* first, we should close the preceding group */
            fprintf(prstream, "  </g>\n");
            data->group_is_open = FALSE;
        }

        define_pattern(pen.pattern, data);
        prgb = get_rgb(pen.color);
        if (prgb != NULL) {
            red   = prgb->red   >> (GRACE_BPP - 8);
            green = prgb->green >> (GRACE_BPP - 8);
            blue  = prgb->blue  >> (GRACE_BPP - 8);
        } else {
            red   = 0;
            green = 0;
            blue  = 0;
        }

        if (fill && data->pattern_empty[pen.pattern] != TRUE) {
            if (data->pattern_full[pen.pattern] == TRUE) {
                fprintf(prstream, "  <g style=\"fill:#%2.2X%2.2X%2.2X",
                        red, green, blue);
            } else {
                fprintf(prstream, "  <g style=\"color:#%2.2X%2.2X%2.2X",
                        red, green, blue);
                fprintf(prstream, "; fill:url(#pattern%d)", pen.pattern);
            }
            if (getfillrule() == FILLRULE_WINDING) {
                fprintf(prstream, "; fill-rule:nonzero");
            } else {
                fprintf(prstream, "; fill-rule:evenodd");
            }
        } else {
            fprintf(prstream, "  <g style=\"fill:none");
        }

        if (draw) {

            fprintf(prstream, "; stroke:#%2.2X%2.2X%2.2X", red, green, blue);

            fprintf(prstream, "; stroke-width:%8.4f", lw*data->side);

            switch (linecap) {
            case LINECAP_BUTT :
                fprintf(prstream, "; stroke-linecap:butt");
                break;
            case LINECAP_ROUND :
                fprintf(prstream, "; stroke-linecap:round");
                break;
            case LINECAP_PROJ :
                fprintf(prstream, "; stroke-linecap:square");
                break;
            default :
                fprintf(prstream, "; stroke-linecap:inherit");
                break;
            }

            switch (linejoin) {
            case LINEJOIN_MITER :
                fprintf(prstream, "; stroke-linejoin:miter");
                break;
            case LINEJOIN_ROUND :
                fprintf(prstream, "; stroke-linejoin:round");
                break;
            case LINEJOIN_BEVEL :
                fprintf(prstream, "; stroke-linejoin:bevel");
                break;
            default :
                fprintf(prstream, "; stroke-linejoin:inherit");
                break;
            }

            if (linestyle <= 1) {
                fprintf(prstream, "; stroke-dasharray:none");
            } else {
                fprintf(prstream, "; stroke-dasharray:");
                for (i = 0; i < dash_array_length[linestyle]; i++) {
                    fprintf(prstream, " %d", dash_array[linestyle][i]);
                }
            }
        }

        fprintf(prstream, "\">\n");


        data->group_is_open = TRUE;
        data->line_width    = lw;
        data->pen           = pen;
        data->fillrule      = fillrule;
        data->linecap       = linecap;
        data->linejoin      = linejoin;
        data->linestyle     = linestyle;
        data->draw          = draw;
        data->fill          = fill;
    }
}

void svg_drawpixel(VPoint vp)
{
    svg_group_props(FALSE, TRUE);
    fprintf(prstream,
            "   <rect x=\"%.4f\" y=\"%.4f\" width=\"%.4f\" height=\"%.4f\"/>\n",
            convertX(vp.x), convertY (vp.y),
            convertW(1.0), convertH(1.0));
}

void svg_drawpolyline(VPoint *vps, int n, int mode)
{
    int i;

    if (n <= 0) {
        return;
    }

    svg_group_props(TRUE, FALSE);
    fprintf(prstream, "   <path d=\"M%.4f,%.4f",
            convertX(vps[0].x), convertY(vps[0].y));
    for (i = 1; i < n; i++) {
        if (i%10 == 0) {
            fprintf(prstream, "\n            ");
        }
        fprintf(prstream, "L%.4f,%.4f", convertX(vps[i].x), convertY(vps[i].y));
    }
    if (mode == POLYLINE_CLOSED) {
        fprintf(prstream, "z\"/>\n");
    } else {
        fprintf(prstream, "\"/>\n");
    }

}

void svg_fillpolygon(VPoint *vps, int nc)
{
    int i;

    if (nc <= 0) {
        return;
    }

    svg_group_props(FALSE, TRUE);
    fprintf(prstream, "   <path  d=\"M%.4f,%.4f",
            convertX(vps[0].x), convertY(vps[0].y));
    for (i = 1; i < nc; i++) {
        if (i%10 == 0) {
            fprintf(prstream, "\n             ");
        }
        fprintf(prstream, "L%.4f,%.4f", convertX(vps[i].x), convertY(vps[i].y));
    }
    fprintf(prstream, "z\"/>\n");
}

void svg_drawarc(VPoint vp1, VPoint vp2, int a1, int a2)
{
    VPoint center;
    double rx, ry;

    if (a1 == a2) {
        return;
    }
    
    center.x = 0.5*(vp1.x + vp2.x);
    center.y = 0.5*(vp1.y + vp2.y);
    rx       = 0.5*fabs(vp2.x - vp1.x);
    ry       = 0.5*fabs(vp2.y - vp1.y);

    svg_group_props(TRUE, FALSE);

    if ((a1 - a2)%360 == 0) {
        fprintf(prstream,
            "   <ellipse  rx=\"%.4f\" ry=\"%.4f\" cx=\"%.4f\" cy=\"%.4f\"/>\n",
            convertW(rx), convertH(ry),
            convertX(center.x), convertY(center.y));
    } else {
        VPoint start, end;
        
        start.x = center.x + rx*cos((M_PI/180.0)*a1);
        start.y = center.y + ry*sin((M_PI/180.0)*a1);
        end.x   = center.x + rx*cos((M_PI/180.0)*a2);
        end.y   = center.y + ry*sin((M_PI/180.0)*a2);

        fprintf(prstream,
            "   <path  d=\"M%.4f, %.4fA%.4f, %.4f %d %d %d %.4f, %.4f\"/>\n",
            convertX(start.x), convertY(start.y),
            convertW(rx), convertH(ry),
            0,
            (abs(a2 - a1) > 180) ? 1 : 0,
            (a2 > a1) ? 0 : 1,
            convertX(end.x), convertY(end.y));
    }
}

void svg_fillarc(VPoint vp1, VPoint vp2, int a1, int a2, int mode)
{
    VPoint center;
    double rx, ry;

    if (a1 == a2) {
        return;
    }

    center.x = 0.5*(vp1.x + vp2.x);
    center.y = 0.5*(vp1.y + vp2.y);
    rx       = 0.5*fabs(vp2.x - vp1.x);
    ry       = 0.5*fabs(vp2.y - vp1.y);

    svg_group_props(FALSE, TRUE);

    if ((a1 - a2)%360 == 0) {
        fprintf(prstream,
            "   <ellipse  rx=\"%.4f\" ry=\"%.4f\" cx=\"%.4f\" cy=\"%.4f\"/>\n",
            convertW(rx), convertH(ry),
            convertX(center.x), convertY(center.y));
    } else {
        VPoint start, end;
        
        start.x = center.x + rx*cos((M_PI/180.0)*a1);
        start.y = center.y + ry*sin((M_PI/180.0)*a1);
        end.x   = center.x + rx*cos((M_PI/180.0)*a2);
        end.y   = center.y + ry*sin((M_PI/180.0)*a2);

        if (mode == ARCFILL_CHORD) {
            fprintf(prstream,
                "   <path  d=\"M%.4f, %.4fA%.4f, %.4f %d %d %d %.4f, %.4fz\"/>\n",
                convertX(start.x), convertY(start.y),
                convertW(rx), convertH(ry),
                0,
                (abs(a2 - a1) > 180) ? 1 : 0,
                (a2 > a1) ? 0 : 1,
                convertX(end.x), convertY(end.y));
        } else {
            fprintf(prstream,
                "   <path  d=\"M%.4f,%.4fL%.4f,%.4fA%.4f,%.4f %d %d %d %.4f,%.4fz\"/>\n",
                convertX(center.x), convertY(center.y),
                convertX(start.x), convertY(start.y),
                convertW(rx), convertH(ry),
                0,
                (abs(a2 - a1) > 180) ? 1 : 0,
                (a2 > a1) ? 0 : 1,
                convertX(end.x), convertY(end.y));
        }
    }
}

void svg_putpixmap(VPoint vp, int width, int height, char *databits, 
                   int pixmap_bpp, int bitmap_pad, int pixmap_type)
{
    /* not implemented yet */
}

void svg_puttext(VPoint vp, char *s, int len, int font,
                 TextMatrix *tm, int underline, int overline, int kerning)
{
    char *fontalias, *dash, *family;
    Svg_data *data = get_curdevice_data();
    double fsize = data->side;

    svg_group_props(FALSE, TRUE);
    
    fprintf(prstream, "   <text  ");

    family  = NULL;
    fontalias = get_fontalias(font);
    if ((dash = strchr(fontalias, '-')) == NULL) {
        family = copy_string(family, fontalias);
    } else {
        family    = xrealloc(family, dash - fontalias + 1);
        strncpy(family, fontalias, dash - fontalias);
        family[dash - fontalias] = '\0';
    }
    fprintf(prstream, " style=\"font-family:%s", family);
    copy_string(family, NULL);

    if (strstr(fontalias, "Italic") != NULL) {
        fprintf(prstream, "; font-style:italic");
    } else {
        if (strstr(fontalias, "Oblique") != NULL) {
            fprintf(prstream, "; font-style:oblique");
        } else {
            fprintf(prstream, "; font-style:normal");
        }
    }

    if (strstr(fontalias, "Bold") != NULL) {
        fprintf(prstream, "; font-weight:bold");
    } else {
        fprintf(prstream, "; font-weight:normal");
    }

    fprintf(prstream, "; font-size:%.4f", fsize);

    if (underline == TRUE) {
        if (overline == TRUE) {
            fprintf(prstream, "; text-decoration=underline|overline");
        } else {
            fprintf(prstream, "; text-decoration=underline");
        }
    } else {
        if (overline == TRUE) {
            fprintf(prstream, "; text-decoration=overline");
        }
    }

    fprintf(prstream, "\" transform=\"matrix(%.4f,%.4f,%.4f,%.4f,%.4f,%.4f)\">",
            tm->cxx, -tm->cyx,
            -tm->cxy, tm->cyy,
            convertX(vp.x), convertY(vp.y));

    fprintf(prstream, escape_specials((unsigned char *) s, len));

    fprintf(prstream, "</text>\n");
}

void svg_leavegraphics(void)
{
    Svg_data *data;
    data = (Svg_data *) get_curdevice_data();
    if (data->group_is_open == TRUE) {
        fprintf(prstream, "  </g>\n");
        data->group_is_open = FALSE;
    }
    fprintf(prstream, "</svg>\n");
}
