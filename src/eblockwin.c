/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-95 Paul J Turner, Portland, OR
 * Copyright (c) 1996-99 Grace Development Team
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
 * Edit block data Panel
 *
 *
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>

#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>

#include "globals.h"
#include "graphs.h"
#include "plotone.h"
#include "utils.h"
#include "files.h"
#include "ssdata.h"
#include "motifinc.h"
#include "protos.h"

static char ncolsbuf[128];

static int block_curtype = SET_XY;

static Widget eblock_frame;
static Widget eblock_panel;

/*
 * Panel item declarations
 */
static OptionStructure *eblock_nchoice_items[MAX_SET_COLS];
static OptionStructure *eblock_schoice_item;
static Widget eblock_ncols_item;
static OptionStructure *eblock_type_choice_item;
static ListStructure *eblock_graph_choice_item;

/*
 * Event and Notify proc declarations
 */
static void eblock_type_notify_proc(int value, void *data);
static void eblock_accept_notify_proc(Widget w, XtPointer client_data, XtPointer call_data);
static void update_eblock(int gno);

/*
 * Create the block data panel
 */
void create_eblock_frame(int gno)
{
    int i, blockncols;
    char buf[32];
    Widget rc, buts[2];

    blockncols = get_blockncols();
    if (blockncols == 0) {
	errmsg("Need to read block data first");
	return;
    }
    set_wait_cursor();
    if (eblock_frame == NULL) {
        OptionItem blockitem;
	char *label1[2];
	label1[0] = "Accept";
	label1[1] = "Close";
        blockitem.value = 0;
        blockitem.label = "Index";
	eblock_frame = XmCreateDialogShell(app_shell, "Edit block data", NULL, 0);
	handle_close(eblock_frame);
	eblock_panel = XmCreateRowColumn(eblock_frame, "eblock_rc", NULL, 0);

	eblock_ncols_item = XtVaCreateManagedWidget("tmp", xmLabelWidgetClass, eblock_panel,
						    NULL);

        rc = XtVaCreateWidget("rc", xmRowColumnWidgetClass, eblock_panel, NULL);

	eblock_type_choice_item = CreateSetTypeChoice(rc, "Set type:");
        AddOptionChoiceCB(eblock_type_choice_item, eblock_type_notify_proc, NULL);

	for (i = 0; i < MAX_SET_COLS; i++) {
            sprintf(buf, "%s from column:", dataset_colname(i));
            eblock_nchoice_items[i] = CreateOptionChoice(rc,
                buf, 3, 1, &blockitem);
        }
        eblock_schoice_item = CreateOptionChoice(rc, "Strings from column:",
            1, 1, &blockitem);

	XtManageChild(rc);

	eblock_graph_choice_item = CreateGraphChoice(eblock_panel,
                                    "Load to a new set in graph:", LIST_TYPE_SINGLE);

	CreateSeparator(eblock_panel);

	CreateCommandButtons(eblock_panel, 2, buts, label1);
	XtAddCallback(buts[0], XmNactivateCallback,
		 (XtCallbackProc) eblock_accept_notify_proc, (XtPointer) 0);
	XtAddCallback(buts[1], XmNactivateCallback,
		 (XtCallbackProc) destroy_dialog, (XtPointer) eblock_frame);

	XtManageChild(eblock_panel);
    }
    XtRaise(eblock_frame);
    update_eblock(gno);
    unset_wait_cursor();
}

/*
 * Notify and event procs
 */

static void update_eblock(int gno)
{
    int blocklen, blockncols;
    int *blockformats;
    int i, ncols, nncols, nscols;
    char buf[16];
    OptionItem *blockitems, *sblockitems;
    
    if (eblock_frame == NULL) {
	return;
    }
    blockncols = get_blockncols();
    if (blockncols == 0) {
	errmsg("Need to read block data first");
	return;
    }
    blocklen = get_blocknrows();
    blockformats = get_blockformats();
    if (is_valid_gno(gno)) {
        SelectListChoice(eblock_graph_choice_item, gno);
    }
    sprintf(ncolsbuf, "%d column(s) of length %d", blockncols, blocklen);
    SetLabel(eblock_ncols_item, ncolsbuf);
    
    /* TODO: check if new data arrived */
    if (1) {
        blockitems  = malloc((blockncols + 1)*sizeof(OptionItem));
        sblockitems = malloc((blockncols + 1)*sizeof(OptionItem));
        blockitems[0].value = -1;
        blockitems[0].label = copy_string(NULL, "Index");
        sblockitems[0].value = -1;
        sblockitems[0].label = copy_string(NULL, "None");
        nncols = 0;
        nscols = 0;
        for (i = 0; i < blockncols; i++) {
            sprintf(buf, "%d", i + 1);
            if (blockformats[i] != FFORMAT_STRING) {
                nncols++;
                blockitems[nncols].value = i;
                blockitems[nncols].label = copy_string(NULL, buf);
            } else {
                nscols++;
                sblockitems[nscols].value = i;
                sblockitems[nscols].label = copy_string(NULL, buf);
            }
        }
        for (i = 0; i < MAX_SET_COLS; i++) {
            UpdateOptionChoice(eblock_nchoice_items[i],
                nncols + 1, blockitems);
            if (i < blockncols) {
                SetOptionChoice(eblock_nchoice_items[i], i);
            }
        }
        UpdateOptionChoice(eblock_schoice_item, nscols + 1, sblockitems);

        for (i = 0; i < nncols + 1; i++) {
            free(blockitems[i].label);
        }
        free(blockitems);
        for (i = 0; i < nscols + 1; i++) {
            free(sblockitems[i].label);
        }
        free(sblockitems);
    }

    ncols = settype_cols(block_curtype);
    for (i = 0; i < MAX_SET_COLS; i++) {
        XtSetSensitive(eblock_nchoice_items[i]->menu, (i < ncols));
    }
}

static void eblock_type_notify_proc(int value, void *data)
{
    block_curtype = value;

    update_eblock(-1);
}

static void eblock_accept_notify_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
    int i, gno;
    int cs[MAX_SET_COLS], nncols, scol;

    if (GetSingleListChoice(eblock_graph_choice_item, &gno)
        != GRACE_EXIT_SUCCESS) {
        errmsg("Please select a single graph");
        return;
    }
    
    nncols = settype_cols(block_curtype);
    for (i = 0; i < nncols; i++) {
        cs[i] = GetOptionChoice(eblock_nchoice_items[i]);
    }
    scol = GetOptionChoice(eblock_schoice_item);
    
    create_set_fromblock(gno, block_curtype, nncols, cs, scol);
    update_all();
    drawgraph();
}
