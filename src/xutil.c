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

#include <config.h>

#include <stdlib.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/cursorfont.h>

#include "defines.h"
#include "globals.h"

#include "utils.h"
#include "files.h"
#include "graphs.h"
#include "graphutils.h"
#include "plotone.h"
#include "device.h"

#include "x11drv.h"

#include "protos.h"

extern Window root, xwin;
extern Display *disp;
extern Widget app_shell;
extern XtAppContext app_con;
extern GC gc, gcxor;
extern int depth;

static Pixmap bufpixmap = (Pixmap) NULL;

extern int win_h, win_w;	/* declared in x11drv.c */

extern int inpipe;
extern char batchfile[];

extern Input_buffer *ib_tbl;
extern int ib_tblsize;

/*
 * cursors
 */

static Cursor wait_cursor;
static Cursor line_cursor;
static Cursor find_cursor;
static Cursor move_cursor;
static Cursor text_cursor;
static Cursor kill_cursor;
static int cur_cursor = -1;

static void xmonitor_rti(XtPointer ib, int *ptrFd, XtInputId *ptrId);

void DefineDialogCursor(Cursor c);
void UndefineDialogCursor();

void set_wait_cursor()
{
    if (disp == NULL) {
        return;
    }
    
    DefineDialogCursor(wait_cursor);
}

void unset_wait_cursor()
{
    if (disp == NULL) {
        return;
    }
    
    UndefineDialogCursor();
    if (cur_cursor >= 0) {
        set_cursor(cur_cursor);
    }
}

void set_cursor(int c)
{
    if (disp == NULL) {
        return;
    }

    XUndefineCursor(disp, xwin);
    cur_cursor = c;
    switch (c) {
    case 0:
        XDefineCursor(disp, xwin, line_cursor);
        break;
    case 1:
        XDefineCursor(disp, xwin, find_cursor);
        break;
    case 2:
        XDefineCursor(disp, xwin, text_cursor);
        break;
    case 3:
        XDefineCursor(disp, xwin, kill_cursor);
        break;
    case 4:
        XDefineCursor(disp, xwin, move_cursor);
        break;
    default:
        cur_cursor = -1;
        break;
    }
    XFlush(disp);
}

void init_cursors(void)
{
    wait_cursor = XCreateFontCursor(disp, XC_watch);
    line_cursor = XCreateFontCursor(disp, XC_crosshair);
    find_cursor = XCreateFontCursor(disp, XC_hand2);
    text_cursor = XCreateFontCursor(disp, XC_xterm);
    kill_cursor = XCreateFontCursor(disp, XC_pirate);
    move_cursor = XCreateFontCursor(disp, XC_fleur);
    cur_cursor = -1;
}


/*
 * put a string in the title bar
 */
void set_title(char *ts)
{
    static char *buf = NULL;
    
    if (ts == NULL) {
        return;
    } else if (buf == NULL || strcmp(buf, ts) != 0) {
        buf = copy_string(buf, ts);
        XtVaSetValues(app_shell, XtNtitle, ts, NULL);
    }
}

/*
 * draw the graph focus indicators
 */
void draw_focus(int gno)
{
    int ix1, iy1, ix2, iy2;
    view v;
    VPoint vp;
    
    if (draw_focus_flag == TRUE) {
	get_graph_viewport(gno, &v);
        vp.x = v.xv1;
        vp.y = v.yv1;
        xlibVPoint2dev(vp, &ix1, &iy1);
        vp.x = v.xv2;
        vp.y = v.yv2;
        xlibVPoint2dev(vp, &ix2, &iy2);
	if (bufpixmap != (Pixmap) NULL) {
            XFillRectangle(disp, bufpixmap, gcxor, ix1 - 5, iy1 - 5, 10, 10);
	    XFillRectangle(disp, bufpixmap, gcxor, ix1 - 5, iy2 - 5, 10, 10);
	    XFillRectangle(disp, bufpixmap, gcxor, ix2 - 5, iy2 - 5, 10, 10);
	    XFillRectangle(disp, bufpixmap, gcxor, ix2 - 5, iy1 - 5, 10, 10);
        }
        XFillRectangle(disp, xwin, gcxor, ix1 - 5, iy1 - 5, 10, 10);
	XFillRectangle(disp, xwin, gcxor, ix1 - 5, iy2 - 5, 10, 10);
	XFillRectangle(disp, xwin, gcxor, ix2 - 5, iy2 - 5, 10, 10);
	XFillRectangle(disp, xwin, gcxor, ix2 - 5, iy1 - 5, 10, 10);
    }
}

/*
 * rubber band line (optionally erasing previous one)
 */
void select_line(int x1, int y1, int x2, int y2, int erase)
{
    static int x1_old, y1_old, x2_old, y2_old;

    if (erase) {
        XDrawLine(disp, xwin, gcxor, x1_old, y1_old, x2_old, y2_old);
    }
    x1_old = x1;
    y1_old = y1;
    x2_old = x2;
    y2_old = y2;
    XDrawLine(disp, xwin, gcxor, x1, y1, x2, y2);
}


/*
 * draw an xor'ed box (optionally erasing previous one)
 */
void select_region(int x1, int y1, int x2, int y2, int erase)
{
    static int x1_old, y1_old, dx_old, dy_old;
    int dx = x2 - x1;
    int dy = y2 - y1;

    if (dx < 0) {
	iswap(&x1, &x2);
	dx = -dx;
    }
    if (dy < 0) {
	iswap(&y1, &y2);
	dy = -dy;
    }
    if (erase) {
        XDrawRectangle(disp, xwin, gcxor, x1_old, y1_old, dx_old, dy_old);
    }
    x1_old = x1;
    y1_old = y1;
    dx_old = dx;
    dy_old = dy;
    XDrawRectangle(disp, xwin, gcxor, x1, y1, dx, dy);
}

/*
 * slide an xor'ed bbox shifted by shift_*, (optionally erasing previous one)
 */
void slide_region(view bb, int shift_x, int shift_y, int erase)
{
    int x1, x2;
    int y1, y2;
    VPoint vp;

    vp.x = bb.xv1;
    vp.y = bb.yv1;
    xlibVPoint2dev(vp, &x1, &y1);
    x1 += shift_x;
    y1 += shift_y;
    
    vp.x = bb.xv2;
    vp.y = bb.yv2;
    xlibVPoint2dev(vp, &x2, &y2);
    x2 += shift_x;
    y2 += shift_y;
    
    select_region(x1, y1, x2, y2, erase);
}


/*
 * draw a crosshair cursor
 */
void crosshair_motion(int x, int y)
{
    static int cursor_oldx = -1, cursor_oldy = -1;
    
    /* Erase the previous crosshair */
    XDrawLine(disp, xwin, gcxor, 0, cursor_oldy, win_w, cursor_oldy);
    XDrawLine(disp, xwin, gcxor, cursor_oldx, 0, cursor_oldx, win_h);

    /* Draw the new crosshair */
    XDrawLine(disp, xwin, gcxor, 0, y, win_w, y);
    XDrawLine(disp, xwin, gcxor, x, 0, x, win_h);
    cursor_oldx = x;
    cursor_oldy = y;
}


/*
 * expose/resize proc
 */
void expose_resize(Widget w, XtPointer client_data,
                        XmDrawingAreaCallbackStruct *cbs)
{
    Dimension ww, wh;
    Page_geometry pg;
    static int inc = 0;
    Input_buffer *ib;

#if defined(DEBUG)
    if (debuglevel == 7) {
	printf("Call to expose_resize(); reason == %d\n", cbs->reason);
    }
#endif
    
    /* HACK */
    if (xwin == 0) {
        return;
    }
    
    if (!inc) {
	inwin = TRUE;
	inc++;
        
        if (page_layout == PAGE_FREE) {
            get_canvas_size(&ww, &wh);
            pg = get_page_geometry();
            pg.width = (long) ww;
            pg.height = (long) wh;
            set_page_geometry(pg);
        }
        
	
	if (batchfile[0]) {
            getparms(batchfile);
	}
	
	if (inpipe == TRUE) {
	    getdata(get_cg(), "stdin", SOURCE_DISK, curtype);
	    inpipe = FALSE;
	}

        if (ib_tblsize > 0) {
            for (ib = ib_tbl; ib < ib_tbl + ib_tblsize; ib++) {
                if (ib->fd >= 0) {
                    xregister_rti(ib);
                }
            }
        }

        update_all();
        drawgraph();

        return;
    }
    
    if (cbs->reason == XmCR_EXPOSE) {
  	xlibredraw(cbs->window, cbs->event->xexpose.x,
                                cbs->event->xexpose.y,
                                cbs->event->xexpose.width,
                                cbs->event->xexpose.height);
        return;
    }
    
    
    if (page_layout == PAGE_FREE) {
        get_canvas_size(&ww, &wh);
        pg = get_page_geometry();
        if (wh*pg.width - ww*pg.height != 0) {
            /* aspect ratio changed */
            double ext_x, ext_y;
            double old_aspectr, new_aspectr;
            old_aspectr = (double) pg.width/pg.height;
            new_aspectr = (double) ww/wh;
            if (old_aspectr >= 1.0 && new_aspectr >= 1.0) {
                ext_x = new_aspectr/old_aspectr;
                ext_y = 1.0;
            } else if (old_aspectr <= 1.0 && new_aspectr <= 1.0) {
                ext_x = 1.0;
                ext_y = old_aspectr/new_aspectr;
            } else if (old_aspectr >= 1.0 && new_aspectr <= 1.0) {
                ext_x = 1.0/old_aspectr;
                ext_y = 1.0/new_aspectr;
            } else {
                ext_x = new_aspectr;
                ext_y = old_aspectr;
            }

            rescale_viewport(ext_x, ext_y);
        } 
        pg.width = (long) ww;
        pg.height = (long) wh;
        set_page_geometry(pg);

        drawgraph();
    }
}

/* 
 * redraw all
 */
void xdrawgraph(void)
{
    if (inwin && (auto_redraw)) {
	set_wait_cursor();
	drawgraph();
	unset_wait_cursor();
    }
}


void xlibredraw(Window window, int x, int y, int width, int height)
{
    if (inwin == TRUE && bufpixmap != (Pixmap) NULL) {
        XCopyArea(disp, bufpixmap, window, gc, x, y, width, height, x, y);
    }
}

Pixmap resize_bufpixmap(unsigned int w, unsigned int h)
{
    static unsigned int pixmap_w = 0, pixmap_h = 0;
    
    if (w == 0 || h == 0) {
        return (bufpixmap);
    }
    
    if (bufpixmap == (Pixmap) NULL) {
        bufpixmap = XCreatePixmap(disp, root, w, h, depth);
    } else if (pixmap_w != w || pixmap_h != h) {
        XFreePixmap(disp, bufpixmap);
        bufpixmap = XCreatePixmap(disp, root, w, h, depth);
    }
    
    if (bufpixmap == (Pixmap) NULL) {
        errmsg("Can't allocate buffer pixmap");
        pixmap_w = 0;
        pixmap_h = 0;
        return (xwin);
    } else {
        pixmap_w = w;
        pixmap_h = h;
        return (bufpixmap);
    }
}

void switch_current_graph(int gno)
{
    int saveg = get_cg();
    
    if (is_graph_hidden(gno) == FALSE) {
        select_graph(gno);
        draw_focus(saveg);
        draw_focus(gno);
        make_format(gno);
        update_all();
        set_graph_selectors(gno);
    }
}

static void xmonitor_rti(XtPointer ib, int *ptrFd, XtInputId *ptrId)
{
    monitor_input((Input_buffer *) ib, 1, 1);
}

void xunregister_rti(XtInputId id)
{
  if (disp != (Display *) NULL) {
      /* the screen has been initialized : we can remove the buffer */
      XtRemoveInput(id);
  }
}

void xregister_rti(Input_buffer *ib)
{
  if (disp != (Display *) NULL) {
      /* the screen has been initialized : we can register the buffer */
      ib->id = (unsigned long) XtAppAddInput(app_con,
                                             ib->fd,
                                             (XtPointer) XtInputReadMask,
                                             xmonitor_rti,
                                             (XtPointer) ib);
  }
}

/*
 * for the goto point feature
 */
void setpointer(VPoint vp)
{
    int x, y;
    
    xlibVPoint2dev(vp, &x, &y);
    XWarpPointer(disp, None, xwin, 0, None, win_w, win_h, x, y);
}

char *display_name(void)
{
    return DisplayString(disp);
}
