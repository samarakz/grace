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

/*
 *
 * Locator Panel
 *
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Xm/DialogS.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>

#include "globals.h"
#include "graphs.h"
#include "graphutils.h"
#include "protos.h"
#include "motifinc.h"

static Widget locator_frame;
static Widget locator_panel;

/*
 * Panel item declarations
 */
static Widget *locator_onoff_item;
static Widget *delta_item;
static Widget *loc_formatx;
static Widget *loc_formaty;
static Widget *loc_precx;
static Widget *loc_precy;
static Widget locx_item;
static Widget locy_item;
static Widget *fixedp_item;

/*
 * Event and Notify proc declarations
 */

static void locator_define_notify_proc(Widget w, XtPointer client_data, XtPointer call_data);
static void locator_reset_notify_proc(Widget w, XtPointer client_data, XtPointer call_data);

extern int go_locateflag;

static int locfx = 2, locfy = 2, locpx = 6, locpy = 6;

void update_locator_items(int gno)
{
    char buf[32];
    
    GLocator locator;
    
    get_graph_locator(gno, &locator);
    
    if (locator_frame) {
	SetChoice(locator_onoff_item, go_locateflag == FALSE);
	SetChoice(fixedp_item, locator.pointset == TRUE);
	SetChoice(delta_item, locator.pt_type);
	SetChoice(loc_formatx, get_format_index(locator.fx));
	SetChoice(loc_formaty, get_format_index(locator.fy));
	SetChoice(loc_precx, locator.px);
	SetChoice(loc_precy, locator.py);
	if (locator.pointset) {
	    sprintf(buf, "%f", locator.dsx);
	    xv_setstr(locx_item, buf);
	    sprintf(buf, "%f", locator.dsy);
	    xv_setstr(locy_item, buf);
	}
    }
}

/*
 * Create the locator Panel
 */
void create_locator_frame(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget rc, fr, rc2;
    Widget buts[3];
    set_wait_cursor();
    if (locator_frame == NULL) {
	char *label1[3];
	label1[0] = "Accept";
	label1[1] = "Reset";
	label1[2] = "Cancel";
	locator_frame = XmCreateDialogShell(app_shell, "Locator props", NULL, 0);
	handle_close(locator_frame);
	locator_panel = XmCreateRowColumn(locator_frame, "ticks_rc", NULL, 0);

	locator_onoff_item = (Widget *) CreatePanelChoice(locator_panel, "Locator:",
							  3,
							  "ON",
							  "OFF",
							  NULL,
							  NULL);
	delta_item = (Widget *) CreatePanelChoice(locator_panel, "Locator display type:",
						  7,
						  "[X, Y]",
						  "[DX, DY]",
						  "[DISTANCE]",
						  "[R, Theta]",
						  "[VX, VY]",
						  "[SX, SY]",
						  NULL,
						  NULL);
	fixedp_item = CreatePanelChoice(locator_panel, "Fixed point:",
					3, "OFF", "ON", NULL,
					NULL);

	rc2 = XmCreateRowColumn(locator_panel, "rc2", NULL, 0);
/*
	XtVaSetValues(rc2, XmNorientation, XmHORIZONTAL, NULL);
*/
	fr = XmCreateFrame(rc2, "fr", NULL, 0);
	rc = XmCreateRowColumn(fr, "rc", NULL, 0);

	loc_formatx = CreateFormatChoice(rc, "Format X:");
	loc_precx = CreatePrecisionChoice(rc, "Precision X:");
	locx_item = (Widget) CreateTextItem2(rc, 10, "Fixed point X:");
	XtManageChild(rc);
	XtManageChild(fr);

	fr = XmCreateFrame(rc2, "fr", NULL, 0);
	rc = XmCreateRowColumn(fr, "rc", NULL, 0);
	loc_formaty = CreateFormatChoice(rc, "Format Y:");
	loc_precy = CreatePrecisionChoice(rc, "Precision Y:");

	locy_item = (Widget) CreateTextItem2(rc, 10, "Fixed point Y:");
	XtManageChild(rc);
	XtManageChild(fr);
	XtManageChild(rc2);

	XtVaCreateManagedWidget("sep", xmSeparatorWidgetClass, locator_panel, NULL);

	CreateCommandButtons(locator_panel, 3, buts, label1);
	XtAddCallback(buts[0], XmNactivateCallback,
		(XtCallbackProc) locator_define_notify_proc, (XtPointer) 0);
	XtAddCallback(buts[1], XmNactivateCallback,
		 (XtCallbackProc) locator_reset_notify_proc, (XtPointer) 0);
	XtAddCallback(buts[2], XmNactivateCallback,
		(XtCallbackProc) destroy_dialog, (XtPointer) locator_frame);

	XtManageChild(locator_panel);
    }
    XtRaise(locator_frame);
    update_locator_items(get_cg());
    unset_wait_cursor();
}				/* end create_locator_panel */

/*
 * Notify and event procs
 */

static void locator_define_notify_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    GLocator locator;
    
    get_graph_locator(get_cg(), &locator);
    
    go_locateflag = (int) GetChoice(locator_onoff_item) == 0;
    locator.pt_type = (int) GetChoice(delta_item);
    locfx = locator.fx = format_types[(int) GetChoice(loc_formatx)];
    locfy = locator.fy = format_types[(int) GetChoice(loc_formaty)];
    locpx = locator.px = (int) GetChoice(loc_precx);
    locpy = locator.py = (int) GetChoice(loc_precy);
    locator.pointset = (int) GetChoice(fixedp_item);
    if (locator.pointset) {
	xv_evalexpr(locx_item, &locator.dsx ); 
	xv_evalexpr(locy_item, &locator.dsy ); 
    }
    make_format(get_cg());
    set_graph_locator(get_cg(), &locator);
    
    XtUnmanageChild(locator_frame);
}

static void locator_reset_notify_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    GLocator locator;
    
    locator.dsx = locator.dsy = 0.0;/* locator props */
    locator.pointset = FALSE;
    locator.pt_type = 0;
    locator.fx = FORMAT_GENERAL;
    locator.fy = FORMAT_GENERAL;
    locator.px = 6;
    locator.py = 6;
    set_graph_locator(get_cg(), &locator);
    update_locator_items(get_cg());
}
