/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-1995 Paul J Turner, Portland, OR
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
 *
 * Prototypes involving X
 *
 */
#ifndef __XPROTOS_H_
#define __XPROTOS_H_

#include <Xm/Xm.h>

#include "defines.h"
#include "core_utils.h"

struct _X11Stuff {
    Display *disp;
    int screennumber;
    
    Window root;
    Window xwin;

    Widget canvas;

    GC gc;
    int depth;
    Colormap cmap;

    Pixmap bufpixmap;

    unsigned int win_h;
    unsigned int win_w;
    unsigned int win_scale;
};

void x11_VPoint2dev(const VPoint *vp, short *x, short *y);
void x11_dev2VPoint(short x, short y, VPoint *vp);

long x11_allocate_color(GUI *gui, const RGB *rgb);
void x11_redraw(Window window, int x, int y, int widht, int height);

int x11_init(Grace *grace);

int initialize_gui(int *argc, char **argv);
void startup_gui(Grace *grace);

void xdrawgraph(const Quark *q, int force);
void expose_resize(Widget w, XtPointer client_data, XtPointer call_data);

void setpointer(VPoint vp);

void select_line(GUI *gui, int x1, int y1, int x2, int y2, int erase);
void select_region(GUI *gui, int x1, int y1, int x2, int y2, int erase);
void slide_region(GUI *gui, view bbox, int shift_x, int shift_y, int erase);
void reset_crosshair(GUI *gui, int clear);
void crosshair_motion(GUI *gui, int x, int y);

void draw_focus(Quark *gr);
void switch_current_graph(Quark *gr);

char *display_name(GUI *gui);

void xunregister_rti(XtInputId);
void xregister_rti(Input_buffer *ib);

void errwin(const char *s);
int yesnowin(char *msg1, char *msg2, char *s1, char *help_anchor);
void stufftextwin(char *s);

void do_hotupdate_proc(void *data);

void create_file_popup(Widget but, void *data);
void create_netcdfs_popup(Widget but, void *data);
void create_saveproject_popup(void);
void create_openproject_popup(void);
void create_write_popup(Widget but, void *data);

void create_eblock_frame(Quark *gr);

void create_printer_setup(Widget but, void *data);

void create_eval_frame(Widget but, void *data);
void create_load_frame(Widget but, void *data);
void create_histo_frame(Widget but, void *data);
void create_fourier_frame(Widget but, void *data);
void create_run_frame(Widget but, void *data);
void create_reg_frame(Widget but, void *data);
void create_diff_frame(Widget but, void *data);
void create_interp_frame(Widget but, void *data);
void create_int_frame(Widget but, void *data);
void create_xcor_frame(Widget but, void *data);
void create_samp_frame(Widget but, void *data);
void create_prune_frame(Widget but, void *data);
void create_lconv_frame(Widget but, void *data);
void create_leval_frame(Widget but, void *data);

void create_points_frame(Widget but, void *data);

void create_arrange_frame(Widget but, void *data);
void create_autos_frame(Widget but, void *data);

void define_explorer_popup(Widget but, void *data);
void update_explorer(ExplorerUI *ui, int reselect);

void create_about_grtool(Widget but, void *data);

void create_monitor_frame_cb(Widget but, void *data);

void update_set_lists(Quark *gr);
void update_ticks(Quark *gr);
void update_props_items(void);
void update_all(void);
void update_all_cb(Widget but, void *data);

void update_set_selectors(Quark *gr);

void graph_set_selectors(Quark *gr);
int clean_graph_selectors(Quark *pr, int etype, void *data);
int clean_set_selectors(Quark *gr, int etype, void *data);

void HelpCB(Widget w, void *data);

void create_nonl_frame(Widget but, void *data);

void create_props_frame(Widget but, void *data);

void create_fonttool(Widget w);
void create_fonttool_cb(Widget but, void *data);

void create_datasetprop_popup(Widget but, void *data);
void create_datasetop_popup(Widget but, void *data);

void create_featext_frame(Widget but, void *data);
void create_cumulative_frame(Widget but, void *data);

void create_ss_frame(Quark *pset);
void update_ss_editors(Quark *gr);
void do_ext_editor(Quark *pset);

void init_cursors(GUI *gui);
void set_cursor(GUI *gui, int c);
void set_wait_cursor(void);
void unset_wait_cursor(void);

int init_option_menus(void);

void set_title(const Quark *pr);
void set_left_footer(char *s);
void set_tracker_string(char *s);

void sync_canvas_size(Grace *grace);

void installXErrorHandler(void);

#endif /* __XPROTOS_H_ */
