/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1996-2003 Grace Development Team
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
 * Grace PDF driver
 */

#include <config.h>

#ifdef HAVE_LIBPDF

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#include <pdflib.h>

#define CANVAS_BACKEND_API
#include "grace/canvas.h"

#include "utils.h"
#include "devlist.h"
#include "pdfdrv.h"

#include "protos.h"

#ifndef NONE_GUI
#  include "motifinc.h"
#endif

typedef struct {
    PDF             *phandle;

    unsigned long    page_scale;
    float            pixel_size;
    float            page_scalef;

    int             *font_ids;
    int             *pattern_ids;

    int              color;
    int              pattern;
    double           linew;
    int              lines;
    int              linecap;
    int              linejoin;

    PDFCompatibility compat;
    PDFColorSpace    colorspace;
    int              compression;
    int              fpprec;
    
    int              kerning_supported;

#ifndef NONE_GUI
    Widget           frame;
    OptionStructure *compat_item;
    SpinStructure   *compression_item;
    SpinStructure   *fpprec_item;
    OptionStructure *colorspace_item;
#endif
} PDF_data;

static void pdf_error_handler(PDF *p, int type, const char* msg);

static PDF_data *init_pdf_data(void)
{
    PDF_data *data;

    /* we need to perform the allocations */
    data = xmalloc(sizeof(PDF_data));
    if (data == NULL) {
        return NULL;
    }

    memset(data, 0, sizeof(PDF_data));

    data->compat      = PDF_1_3;
    data->colorspace  = DEFAULT_COLORSPACE;
    data->compression = 4;
    data->fpprec      = 4;
    
    return data;
}

static void pdf_data_free(void *data)
{
    PDF_data *pdfdata = (PDF_data *) data;
    
    if (pdfdata) {
        xfree(pdfdata->font_ids);
        xfree(pdfdata->pattern_ids);
        xfree(pdfdata);
    }
}

int register_pdf_drv(Canvas *canvas)
{
    Device_entry *d;
    PDF_data *data;
    
    PDF_boot();
    
    data = init_pdf_data();
    if (!data) {
        return -1;
    }

    d = device_new("PDF", DEVICE_FILE, TRUE, data, pdf_data_free);
    if (!d) {
        xfree(data);
        return -1;
    }
    
    device_set_fext(d, "pdf");
    
    device_set_procs(d,
        pdf_initgraphics,
        pdf_leavegraphics,
        pdf_op_parser,
        pdf_gui_setup,
        NULL,
        pdf_drawpixel,
        pdf_drawpolyline,
        pdf_fillpolygon,
        pdf_drawarc,
        pdf_fillarc,
        pdf_putpixmap,
        pdf_puttext);
    
    return register_device(canvas, d);
}

static char *pdf_builtin_fonts[] = 
{
    "Times-Roman",
    "Times-Italic",
    "Times-Bold",
    "Times-BoldItalic",
    "Helvetica",
    "Helvetica-Oblique",
    "Helvetica-Bold",
    "Helvetica-BoldOblique",
    "Courier",
    "Courier-Oblique",
    "Courier-Bold",
    "Courier-BoldOblique",
    "Symbol",
    "ZapfDingbats"
};

static int number_of_pdf_builtin_fonts = sizeof(pdf_builtin_fonts)/sizeof(char *);

static int pdf_builtin_font(const char *fname)
{
    int i;
    for (i = 0; i < number_of_pdf_builtin_fonts; i++) {
        if (strcmp(pdf_builtin_fonts[i], fname) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}


int pdf_initgraphics(const Canvas *canvas, void *data, const CanvasStats *cstats)
{
    PDF_data *pdfdata = (PDF_data *) data;
    int i;
    Page_geometry *pg;
    char *s;
   
    pg = get_page_geometry(canvas);
    
    pdfdata->page_scale  = MIN2(pg->height, pg->width);
    pdfdata->pixel_size  = 1.0/pdfdata->page_scale;
    pdfdata->page_scalef = (float) pdfdata->page_scale*72.0/pg->dpi;

    /* undefine all graphics state parameters */
    pdfdata->color    = -1;
    pdfdata->pattern  = -1;
    pdfdata->linew    = -1.0;
    pdfdata->lines    = -1;
    pdfdata->linecap  = -1;
    pdfdata->linejoin = -1;

    pdfdata->phandle = PDF_new2(pdf_error_handler, NULL, NULL, NULL, NULL);
    if (pdfdata->phandle == NULL) {
        return RETURN_FAILURE;
    }

    PDF_set_parameter(pdfdata->phandle, "fontwarning", "false");
    
    /* check PDFlib capabilities */
    if (!strcmp(PDF_get_parameter(pdfdata->phandle, "pdi", 0), "true")) {
        pdfdata->kerning_supported = TRUE;
    } else {
        pdfdata->kerning_supported = FALSE;
    }

    switch (pdfdata->compat) {
    case PDF_1_3:
        s = "1.3";
        break;
    case PDF_1_4:
        s = "1.4";
        break;
    case PDF_1_5:
        s = "1.5";
        break;
    default:
        s = "1.3";
        break;
    }
    PDF_set_parameter(pdfdata->phandle, "compatibility", s);

    if (PDF_open_fp(pdfdata->phandle, canvas_get_prstream(canvas)) == -1) {
        return RETURN_FAILURE;
    }
    
    PDF_set_value(pdfdata->phandle, "compress", (float) pdfdata->compression);
    PDF_set_value(pdfdata->phandle, "floatdigits", (float) pdfdata->fpprec);

    PDF_set_info(pdfdata->phandle, "Creator", bi_version_string());
    PDF_set_info(pdfdata->phandle, "Author", canvas_get_username(canvas));
    PDF_set_info(pdfdata->phandle, "Title", canvas_get_docname(canvas));
        
    pdfdata->font_ids = xmalloc(number_of_fonts(canvas)*SIZEOF_INT);
    for (i = 0; i < number_of_fonts(canvas); i++) {
        pdfdata->font_ids[i] = -1;
    }
    for (i = 0; i < cstats->nfonts; i++) {
        int font;
        char buf[GR_MAXPATHLEN];
        char *fontname, *encscheme;
        char *pdflibenc;
        int embed;
        
        font = cstats->fonts[i].font;
        
        fontname = get_fontalias(canvas, font);
        
        if (pdf_builtin_font(fontname)) {
            embed = 0;
        } else {
            sprintf(buf, "%s==%s",
                fontname, get_afmfilename(canvas, font, TRUE));
            PDF_set_parameter(pdfdata->phandle, "FontAFM", buf);
            sprintf(buf, "%s==%s",
                fontname, get_fontfilename(canvas, font, TRUE));
            PDF_set_parameter(pdfdata->phandle, "FontOutline", buf);

            embed = 1;
        }

        encscheme = get_encodingscheme(canvas, font);
        if (strcmp(encscheme, "FontSpecific") == 0) {
            pdflibenc = "builtin";
        } else {
            pdflibenc = "winansi";
        }
        
        pdfdata->font_ids[font] =
            PDF_findfont(pdfdata->phandle, fontname, pdflibenc, embed);
        
        if (pdfdata->font_ids[font] < 0) {
            errmsg(PDF_get_errmsg(pdfdata->phandle));
        }
    }
    
    if (pdfdata->compat >= PDF_1_3) {
        pdfdata->pattern_ids = xmalloc(number_of_patterns(canvas)*SIZEOF_INT);
        for (i = 0; i < cstats->npatterns; i++) {
            int patno = cstats->patterns[i];
            Pattern *pat = canvas_get_pattern(canvas, patno);
/* Unfortunately, there is no way to open a _masked_ image from memory */
#if 0
            int im;
            pdfdata->pattern_ids[i] = PDF_begin_pattern(pdfdata->phandle,
                pat->width, pat->height, pat->width, pat->height, 2);
            im = PDF_open_image(pdfdata->phandle, "raw", "memory",
                (const char *) pat_bits[i], pat->width*pat->height/8,
                pat->width, pat->height, 1, 1, "");
            PDF_place_image(pdfdata->phandle, im, 0.0, 0.0, 1.0);
            PDF_close_image(pdfdata->phandle, im);
            PDF_end_pattern(pdfdata->phandle);
#else
            int j, k, l;
            pdfdata->pattern_ids[patno] = PDF_begin_pattern(pdfdata->phandle,
                pat->width, pat->height, pat->width, pat->height, 2);
            for (j = 0; j < 256; j++) {
                k = j%16;
                l = 15 - j/16;
                if ((pat->bits[j/8] >> (j%8)) & 0x01) {
                    /* the bit is set */
                    PDF_rect(pdfdata->phandle, (float) k, (float) l, 1.0, 1.0);
                    PDF_fill(pdfdata->phandle);
                }
            }
            PDF_end_pattern(pdfdata->phandle);
#endif
        }
    }

    PDF_begin_page(pdfdata->phandle, pg->width*72.0/pg->dpi, pg->height*72.0/pg->dpi);
    
    if (pdfdata->compat >= PDF_1_3) {
        s = canvas_get_description(canvas);

        if (!is_empty_string(s)) {
            PDF_set_border_style(pdfdata->phandle, "dashed", 3.0);
            PDF_set_border_dash(pdfdata->phandle, 5.0, 1.0);
            PDF_set_border_color(pdfdata->phandle, 1.0, 0.0, 0.0);

            PDF_add_note(pdfdata->phandle,
                20.0, 50.0, 320.0, 100.0, s, "Project description", "note", 0);
        }
    }
    
    PDF_scale(pdfdata->phandle, pdfdata->page_scalef, pdfdata->page_scalef);

    return RETURN_SUCCESS;
}

void pdf_setpen(const Canvas *canvas, const Pen *pen, PDF_data *pdfdata)
{
    if (pen->color != pdfdata->color || pen->pattern != pdfdata->pattern) {
        float c1, c2, c3, c4;
        char *cstype;
        switch (pdfdata->colorspace) {
        case COLORSPACE_GRAYSCALE:
            {
                cstype = "gray";
                
                c1 = (float) get_colorintensity(canvas, pen->color);
                c2 = c3 = c4 = 0.0;
            }
            break;
        case COLORSPACE_CMYK:
            {
                fCMYK fcmyk;
                
                cstype = "cmyk";
                
                get_fcmyk(canvas, pen->color, &fcmyk);
                c1 = (float) fcmyk.cyan;
                c2 = (float) fcmyk.magenta;
                c3 = (float) fcmyk.yellow;
                c4 = (float) fcmyk.black;
            }
            break;
        case COLORSPACE_RGB:
        default:
            {
                fRGB frgb;
                
                cstype = "rgb";
                
                get_frgb(canvas, pen->color, &frgb);
                c1 = (float) frgb.red;
                c2 = (float) frgb.green;
                c3 = (float) frgb.blue;
                c4 = 0.0;
            }
            break;
        }

        PDF_setcolor(pdfdata->phandle, "both", cstype, c1, c2, c3, c4);     
        if (pdfdata->compat >= PDF_1_3 &&
            pen->pattern > 1 && pdfdata->pattern_ids[pen->pattern] >= 0) {
            PDF_setcolor(pdfdata->phandle, "both", "pattern",
                (float) pdfdata->pattern_ids[pen->pattern], 0.0, 0.0, 0.0);     
        }
        pdfdata->color   = pen->color;
        pdfdata->pattern = pen->pattern;
    }
}

void pdf_setdrawbrush(const Canvas *canvas, PDF_data *pdfdata)
{
    int i;
    float lw;
    int ls;
    float *darray;
    Pen pen;

    getpen(canvas, &pen);
    pdf_setpen(canvas, &pen, pdfdata);
    
    ls = getlinestyle(canvas);
    lw = MAX2(getlinewidth(canvas), pdfdata->pixel_size);

    if (ls != pdfdata->lines || lw != pdfdata->linew) {    
        PDF_setlinewidth(pdfdata->phandle, lw);

        if (ls == 0 || ls == 1) {
            PDF_setpolydash(pdfdata->phandle, NULL, 0); /* length == 0,1 means solid line */
        } else {
            LineStyle *linestyle = canvas_get_linestyle(canvas, ls);
            darray = xmalloc(linestyle->length*SIZEOF_FLOAT);
            for (i = 0; i < linestyle->length; i++) {
                darray[i] = lw*linestyle->array[i];
            }
            PDF_setpolydash(pdfdata->phandle, darray, linestyle->length);
            xfree(darray);
        }
        pdfdata->linew = lw;
        pdfdata->lines = ls;
    }
}

void pdf_setlineprops(const Canvas *canvas, PDF_data *pdfdata)
{
    int lc, lj;
    
    lc = getlinecap(canvas);
    lj = getlinejoin(canvas);
    
    if (lc != pdfdata->linecap) {
        switch (lc) {
        case LINECAP_BUTT:
            PDF_setlinecap(pdfdata->phandle, 0);
            break;
        case LINECAP_ROUND:
            PDF_setlinecap(pdfdata->phandle, 1);
            break;
        case LINECAP_PROJ:
            PDF_setlinecap(pdfdata->phandle, 2);
            break;
        }
        pdfdata->linecap = lc;
    }

    if (lj != pdfdata->linejoin) {
        switch (lj) {
        case LINEJOIN_MITER:
            PDF_setlinejoin(pdfdata->phandle, 0);
            break;
        case LINEJOIN_ROUND:
            PDF_setlinejoin(pdfdata->phandle, 1);
            break;
        case LINEJOIN_BEVEL:
            PDF_setlinejoin(pdfdata->phandle, 2);
            break;
        }
        pdfdata->linejoin = lj;
    }
}

void pdf_drawpixel(const Canvas *canvas, void *data, const VPoint *vp)
{
    PDF_data *pdfdata = (PDF_data *) data;
    Pen pen;

    getpen(canvas, &pen);
    pdf_setpen(canvas, &pen, pdfdata);
    
    if (pdfdata->linew != pdfdata->pixel_size) {
        PDF_setlinewidth(pdfdata->phandle, pdfdata->pixel_size);
        pdfdata->linew = pdfdata->pixel_size;
    }
    if (pdfdata->linecap != LINECAP_ROUND) {
        PDF_setlinecap(pdfdata->phandle, 1);
        pdfdata->linecap = LINECAP_ROUND;
    }
    if (pdfdata->lines != 1) {
        PDF_setpolydash(pdfdata->phandle, NULL, 0);
        pdfdata->lines = 1;
    }

    PDF_moveto(pdfdata->phandle, (float) vp->x, (float) vp->y);
    PDF_lineto(pdfdata->phandle, (float) vp->x, (float) vp->y);
    PDF_stroke(pdfdata->phandle);
}

void pdf_poly_path(const VPoint *vps, int n, PDF_data *pdfdata)
{
    int i;
    
    PDF_moveto(pdfdata->phandle, (float) vps[0].x, (float) vps[0].y);
    for (i = 1; i < n; i++) {
        PDF_lineto(pdfdata->phandle, (float) vps[i].x, (float) vps[i].y);
    }
}

void pdf_drawpolyline(const Canvas *canvas, void *data,
    const VPoint *vps, int n, int mode)
{
    PDF_data *pdfdata = (PDF_data *) data;
    if (getlinestyle(canvas) == 0) {
        return;
    }
    
    pdf_setdrawbrush(canvas, pdfdata);
    pdf_setlineprops(canvas, pdfdata);
    
    pdf_poly_path(vps, n, pdfdata);
    
    if (mode == POLYLINE_CLOSED) {
        PDF_closepath_stroke(pdfdata->phandle);
    } else {
        PDF_stroke(pdfdata->phandle);
    }
}

void pdf_fillpolygon(const Canvas *canvas, void *data,
    const VPoint *vps, int nc)
{
    PDF_data *pdfdata = (PDF_data *) data;
    Pen pen;

    getpen(canvas, &pen);
    
    if (pen.pattern == 0) {
        return;
    }
    
    if (getfillrule(canvas) == FILLRULE_WINDING) {
        PDF_set_parameter(pdfdata->phandle, "fillrule", "winding");
    } else {
        PDF_set_parameter(pdfdata->phandle, "fillrule", "evenodd");
    }
    
    if (pdfdata->compat >= PDF_1_3 && pen.pattern > 1) {
        Pen solid_pen;
        solid_pen.color = getbgcolor(canvas);
        solid_pen.pattern = 1;
        
        pdf_setpen(canvas, &solid_pen, pdfdata);
        pdf_poly_path(vps, nc, pdfdata);
        PDF_fill(pdfdata->phandle);
    }
    
    getpen(canvas, &pen);
    pdf_setpen(canvas, &pen, pdfdata);
    pdf_poly_path(vps, nc, pdfdata);
    PDF_fill(pdfdata->phandle);
}

void pdf_arc_path(const VPoint *vp1, const VPoint *vp2,
    double a1, double a2, int mode, PDF_data *pdfdata)
{
    VPoint vpc;
    double rx, ry;

    vpc.x = (vp1->x + vp2->x)/2;
    vpc.y = (vp1->y + vp2->y)/2;
    rx = fabs(vp2->x - vp1->x)/2;
    ry = fabs(vp2->y - vp1->y)/2;
    
    if (rx == 0.0 || ry == 0.0) {
        return;
    }
    
    PDF_scale(pdfdata->phandle, 1.0, ry/rx);
    PDF_moveto(pdfdata->phandle, (float) vpc.x + rx*cos(a1*M_PI/180.0), 
                                 (float) rx/ry*vpc.y + rx*sin(a1*M_PI/180.0));
    if (a2 < 0) {
        PDF_arcn(pdfdata->phandle, (float) vpc.x, (float) rx/ry*vpc.y, rx, 
                                            (float) a1, (float) (a1 + a2));
    } else {
        PDF_arc(pdfdata->phandle, (float) vpc.x, (float) rx/ry*vpc.y, rx, 
                                            (float) a1, (float) (a1 + a2));
    }

    if (mode == ARCFILL_PIESLICE) {
        PDF_lineto(pdfdata->phandle, (float) vpc.x, (float) rx/ry*vpc.y);
    }
}

void pdf_drawarc(const Canvas *canvas, void *data,
    const VPoint *vp1, const VPoint *vp2, double a1, double a2)
{
    PDF_data *pdfdata = (PDF_data *) data;

    if (getlinestyle(canvas) == 0) {
        return;
    }
    
    pdf_setdrawbrush(canvas, pdfdata);
    PDF_save(pdfdata->phandle);
    pdf_arc_path(vp1, vp2, a1, a2, ARCFILL_CHORD, pdfdata);
    PDF_stroke(pdfdata->phandle);
    PDF_restore(pdfdata->phandle);
}

void pdf_fillarc(const Canvas *canvas, void *data,
    const VPoint *vp1, const VPoint *vp2, double a1, double a2, int mode)
{
    PDF_data *pdfdata = (PDF_data *) data;
    Pen pen;

    getpen(canvas, &pen);
    
    if (pen.pattern == 0) {
        return;
    }
    
    if (pdfdata->compat >= PDF_1_3 && pen.pattern > 1) {
        Pen solid_pen;
        solid_pen.color = getbgcolor(canvas);
        solid_pen.pattern = 1;
        
        pdf_setpen(canvas, &solid_pen, pdfdata);
        PDF_save(pdfdata->phandle);
        pdf_arc_path(vp1, vp2, a1, a2, mode, pdfdata);
        PDF_fill(pdfdata->phandle);
        PDF_restore(pdfdata->phandle);
    }

    pdf_setpen(canvas, &pen, pdfdata);
    PDF_save(pdfdata->phandle);
    pdf_arc_path(vp1, vp2, a1, a2, mode, pdfdata);
    PDF_fill(pdfdata->phandle);
    PDF_restore(pdfdata->phandle);
}

/* TODO: transparent pixmaps */
void pdf_putpixmap(const Canvas *canvas, void *data,
    const VPoint *vp, int width, int height, char *databits,
    int pixmap_bpp, int bitmap_pad, int pixmap_type)
{
    PDF_data *pdfdata = (PDF_data *) data;
    char *buf, *bp;
    int image;
    int cindex;
    RGB fg, bg;
    int	i, k, j;
    long paddedW;

    int components    = 3;
        
    buf = xmalloc(width*height*components);
    if (buf == NULL) {
        errmsg("xmalloc failed in pdf_putpixmap()");
        return;
    }
    
    bp = buf;
    if (pixmap_bpp == 1) {
        paddedW = PAD(width, bitmap_pad);
        get_rgb(canvas, getcolor(canvas), &fg);
        get_rgb(canvas, getbgcolor(canvas), &bg);
        for (k = 0; k < height; k++) {
            for (j = 0; j < paddedW/bitmap_pad; j++) {
                for (i = 0; i < bitmap_pad && j*bitmap_pad + i < width; i++) {
                    if (bin_dump(&(databits)[k*paddedW/bitmap_pad + j], i, bitmap_pad)) {
                        *bp++ = (char) fg.red;
                        *bp++ = (char) fg.green;
                        *bp++ = (char) fg.blue;
                    } else {
                        *bp++ = (char) bg.red;
                        *bp++ = (char) bg.green;
                        *bp++ = (char) bg.blue;
                    }
                }
            }
        }
    } else {
        for (k = 0; k < height; k++) {
            for (j = 0; j < width; j++) {
                cindex = (databits)[k*width + j];
                get_rgb(canvas, cindex, &fg);
                *bp++ = (char) fg.red;
                *bp++ = (char) fg.green;
                *bp++ = (char) fg.blue;
            }
        }
    }
    
    image = PDF_open_image(pdfdata->phandle, "raw", "memory",
        buf, width*height*components,
        width, height, components, GRACE_BPP, "");
    if (image == -1) {
        errmsg("Not enough memory for image!");
        xfree(buf);
        return;
    }

    PDF_place_image(pdfdata->phandle,
        image, vp->x, vp->y - height*pdfdata->pixel_size, pdfdata->pixel_size);
    PDF_close_image(pdfdata->phandle, image);
    
    xfree(buf);
}

void pdf_puttext(const Canvas *canvas, void *data,
    const VPoint *vp, const char *s, int len, int font, const TextMatrix *tm,
    int underline, int overline, int kerning)
{
    PDF_data *pdfdata = (PDF_data *) data;
    Pen pen;

    if (pdfdata->font_ids[font] < 0) {
        return;
    } 
    
    getpen(canvas, &pen);
    pdf_setpen(canvas, &pen, pdfdata);
    
    PDF_save(pdfdata->phandle);
    
    PDF_setfont(pdfdata->phandle, pdfdata->font_ids[font], 1.0);

    PDF_set_parameter(pdfdata->phandle, "underline", true_or_false(underline));
    PDF_set_parameter(pdfdata->phandle, "overline",  true_or_false(overline));
    if (pdfdata->kerning_supported) {
        PDF_set_parameter(pdfdata->phandle,
            "kerning", kerning ? "true":"false");
    }
    PDF_concat(pdfdata->phandle, (float) tm->cxx, (float) tm->cyx,
                                 (float) tm->cxy, (float) tm->cyy,
                                 vp->x, vp->y);

    PDF_show2(pdfdata->phandle, s, len);

    PDF_restore(pdfdata->phandle);
}

void pdf_leavegraphics(const Canvas *canvas, void *data,
    const CanvasStats *cstats)
{
    PDF_data *pdfdata = (PDF_data *) data;
    view v;
    v = cstats->bbox;

    PDF_set_value(pdfdata->phandle, "CropBox/llx", pdfdata->page_scalef*v.xv1);
    PDF_set_value(pdfdata->phandle, "CropBox/lly", pdfdata->page_scalef*v.yv1);
    PDF_set_value(pdfdata->phandle, "CropBox/urx", pdfdata->page_scalef*v.xv2);
    PDF_set_value(pdfdata->phandle, "CropBox/ury", pdfdata->page_scalef*v.yv2);
    
    PDF_end_page(pdfdata->phandle);
    PDF_close(pdfdata->phandle);
    PDF_delete(pdfdata->phandle);
    xfree(pdfdata->font_ids);
    XCFREE(pdfdata->pattern_ids);
}

static void pdf_error_handler(PDF *p, int type, const char *msg)
{
    char *buf, *s;

    switch (type) {
    case PDF_NonfatalError:
        /* continue on a non-fatal error */
        s = "PDFlib warning: ";
        break;
    default:
        s = "PDFlib error: ";
        /* give up in all other cases */
        break;
    }
    buf = xmalloc(strlen(msg) + strlen(s) + 1);
    if (buf) {
        sprintf(buf, "%s%s", s, msg);
        errmsg(buf);
        xfree(buf);
    }
}

int pdf_op_parser(const Canvas *canvas, void *data, const char *opstring)
{
    PDF_data *pdfdata = (PDF_data *) data;

    if (!strcmp(opstring, "compatibility:PDF-1.3")) {
        pdfdata->compat = PDF_1_3;
        return RETURN_SUCCESS;
    } else
    if (!strcmp(opstring, "compatibility:PDF-1.4")) {
        pdfdata->compat = PDF_1_4;
        return RETURN_SUCCESS;
    } else
    if (!strcmp(opstring, "compatibility:PDF-1.5")) {
        pdfdata->compat = PDF_1_5;
        return RETURN_SUCCESS;
    } else
    if (!strncmp(opstring, "compression:", 12)) {
        char *bufp;
        bufp = strchr(opstring, ':');
        bufp++;
        if (!is_empty_string(bufp)) {
            pdfdata->compression = atoi(bufp);
            return RETURN_SUCCESS;
        } else {
            return RETURN_FAILURE;
        }
    } else
    if (!strncmp(opstring, "fpprecision:", 12)) {
        char *bufp;
        bufp = strchr(opstring, ':');
        bufp++;
        if (!is_empty_string(bufp)) {
            pdfdata->fpprec = atoi(bufp);
            return RETURN_SUCCESS;
        } else {
            return RETURN_FAILURE;
        }
    } else
    if (!strcmp(opstring, "colorspace:grayscale")) {
        pdfdata->colorspace = COLORSPACE_GRAYSCALE;
        return RETURN_SUCCESS;
    } else
    if (!strcmp(opstring, "colorspace:rgb")) {
        pdfdata->colorspace = COLORSPACE_RGB;
        return RETURN_SUCCESS;
    } else
    if (!strcmp(opstring, "colorspace:cmyk")) {
        pdfdata->colorspace = COLORSPACE_CMYK;
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

#ifndef NONE_GUI

static void update_pdf_setup_frame(PDF_data *pdfdata);
static int set_pdf_setup_proc(void *data);

void pdf_gui_setup(const Canvas *canvas, void *data)
{
    PDF_data *pdfdata = (PDF_data *) data;

    set_wait_cursor();
    
    if (pdfdata->frame == NULL) {
        Widget fr, rc;
        OptionItem compat_op_items[3] = {
            {PDF_1_3, "PDF-1.3"},
            {PDF_1_4, "PDF-1.4"},
            {PDF_1_5, "PDF-1.5"}
        };
        OptionItem colorspace_op_items[3] = {
            {COLORSPACE_GRAYSCALE, "Grayscale"},
            {COLORSPACE_RGB,       "RGB"      },
            {COLORSPACE_CMYK,      "CMYK"     }
        };
    
	pdfdata->frame = CreateDialogForm(app_shell, "PDF options");

	fr = CreateFrame(pdfdata->frame, "PDF options");
        rc = CreateVContainer(fr);
	pdfdata->compat_item =
            CreateOptionChoice(rc, "Compatibility:", 1, 3, compat_op_items);
        pdfdata->colorspace_item =
            CreateOptionChoice(rc, "Colorspace:", 1, 3, colorspace_op_items);
	pdfdata->compression_item = CreateSpinChoice(rc,
            "Compression:", 1, SPIN_TYPE_INT, 0.0, 9.0, 1.0);
	pdfdata->fpprec_item = CreateSpinChoice(rc,
            "FP precision:", 1, SPIN_TYPE_INT, 4.0, 6.0, 1.0);

	CreateAACDialog(pdfdata->frame, fr, set_pdf_setup_proc, pdfdata);
    }
    update_pdf_setup_frame(pdfdata);
    RaiseWindow(GetParent(pdfdata->frame));
    unset_wait_cursor();
}

static void update_pdf_setup_frame(PDF_data *pdfdata)
{
    if (pdfdata->frame) {
        SetOptionChoice(pdfdata->compat_item, pdfdata->compat);
        SetOptionChoice(pdfdata->colorspace_item, pdfdata->colorspace);
        SetSpinChoice(pdfdata->compression_item, (double) pdfdata->compression);
        SetSpinChoice(pdfdata->fpprec_item, (double) pdfdata->fpprec);
    }
}

static int set_pdf_setup_proc(void *data)
{
    PDF_data *pdfdata = (PDF_data *) data;

    pdfdata->compat      = GetOptionChoice(pdfdata->compat_item);
    pdfdata->colorspace  = GetOptionChoice(pdfdata->colorspace_item);
    pdfdata->compression = (int) GetSpinChoice(pdfdata->compression_item);
    pdfdata->fpprec      = (int) GetSpinChoice(pdfdata->fpprec_item);
    
    return RETURN_SUCCESS;
}

#endif

#else /* No PDFlib */
void _pdfdrv_c_dummy_func(void) {}
#endif
