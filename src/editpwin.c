/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-1995 Paul J Turner, Portland, OR
 * Copyright (c) 1996-2004 Grace Development Team
 * 
 * Maintained by Evgeny Stambulchik
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
 * spreadsheet-like editing of data points
 *
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "core_utils.h"
#include "utils.h"
#include "files.h"
#include "protos.h"

#include <Xm/Xm.h>
#include <Xbae/Matrix.h>
#include "motifinc.h"


typedef struct _EditPoints {
    struct _EditPoints *next;
    Quark *pset;
    int cformat[MAX_SET_COLS];
    int cprec[MAX_SET_COLS];
    int update;
    Widget top;
    Widget mw;
    Widget label;
    OptionStructure *stype;
    Widget length;
    TextStructure *comment;
    Widget hotlink;
    OptionStructure *hotsrc;
    Widget hotfile;
} EditPoints;

static void update_cells(EditPoints *ep);

/* default cell value precision */
#define CELL_PREC 8

/* default cell value format */
#define CELL_FORMAT FORMAT_GENERAL

/* default cell width */
#define CELL_WIDTH 12

/* string cell width */
#define STRING_CELL_WIDTH 128

/* minimum size of the spreadseet matrix */
#define MIN_SS_ROWS    100
#define MIN_SS_COLS    1

#define VISIBLE_SS_ROWS     10


static void get_ep_dims(EditPoints *ep, int *nr, int *nc)
{
    XtVaGetValues(ep->mw, XmNrows, nr, XmNcolumns, nc, NULL);
}

static int get_ep_set_dims(EditPoints *ep, int *nrows, int *ncols, int *scols)
{
    if (!ep || !ep->pset) {
        return RETURN_FAILURE;
    }
    
    *nrows = set_get_length(ep->pset);
    *ncols = set_get_ncols(ep->pset);
    if (set_get_strings(ep->pset) != NULL) {
        *scols = 1;
    } else {
        *scols = 0;
    }
    
    return RETURN_SUCCESS;
}


/*
 * delete the selected row(s)
 */
static void del_rows_cb(Widget but, void *data)
{
    int i;
    int nrows, ncols, scols, nc, nr;
    int nscells, row_start, nsrows, nsrows_max, *srows;
    EditPoints *ep = (EditPoints *) data;
    char buf[64];

    if (get_ep_set_dims(ep, &nrows, &ncols, &scols) != RETURN_SUCCESS) {
        return;
    }
    
    get_ep_dims(ep, &nr, &nc);
    if (nr == 0 || nc == 0) {
        return;
    }
    
    row_start = XbaeMatrixFirstSelectedRow(ep->mw);
    if (row_start < 0 || row_start >= nrows) {
        errmsg("Nothing to delete");
        return;
    }
    
    nscells = XbaeMatrixGetNumSelected(ep->mw);
    nsrows_max = nscells/nc;
    srows = xmalloc(nsrows_max*SIZEOF_INT);
    if (!srows) {
        errmsg("Not enough memory");
        return;
    }
    
    for (nsrows = 0, i = row_start; i < nrows && nscells > 0; i++) {
        if (XbaeMatrixIsRowSelected(ep->mw, i)) {
            srows[nsrows] = i;
            nsrows++;
            nscells -= nc;
        }
    }
    
    sprintf(buf, "Delete %d selected row(s)?", nsrows);
    if (yesno(buf, NULL, NULL, NULL)) {
        for (i = nsrows - 1; i >= 0; i--) {
            del_point(ep->pset, srows[i]);
        }

        XbaeMatrixDeselectAll(ep->mw);

        update_set_lists(get_parent_graph(ep->pset));
        update_cells(ep);
    }
    
    xfree(srows);
}


/*
 * add a point to a set by copying the row containing the selected cell
 * and placing it after it
 */
void add_row_cb(Widget but, void *data)
{
    int i, j, k;
    int nrows, ncols, scols;
    char **s;
    Datapoint dpoint;
    EditPoints *ep = (EditPoints *) data;

    XbaeMatrixGetCurrentCell(ep->mw, &i, &j);
    
    if (get_ep_set_dims(ep, &nrows, &ncols, &scols) != RETURN_SUCCESS) {
        return;
    }

    if (i > nrows || i < 0){
        errmsg("Selected row out of range");
        return;
    }
    
    zero_datapoint(&dpoint);
    
    if (i < nrows) {
        for (k = 0; k < ncols; k++) {
            dpoint.ex[k] = *(set_get_col(ep->pset, k) + i);
        }
        if ((s = set_get_strings(ep->pset)) != NULL) {
            dpoint.s = s[i];
        }
        add_point_at(ep->pset, i + 1, &dpoint);
    } else {
        add_point_at(ep->pset, i, &dpoint);
    }
    
    update_set_lists(get_parent_graph(ep->pset));
    update_cells(ep);
}

static OptionStructure *editp_col_item;
static OptionStructure *editp_format_item;
static SpinStructure *editp_precision_item;

static void update_props(EditPoints *ep, int col)
{
    if (col >= 0 && col < MAX_SET_COLS) {
        SetOptionChoice(editp_col_item, col);
    } else {
        col = GetOptionChoice(editp_col_item);
    }

    SetOptionChoice(editp_format_item, ep->cformat[col]); 

    SetSpinChoice(editp_precision_item, (double) ep->cprec[col]);
}

static int do_accept_props(void *data)
{
    int col, cformat, cprec;
    EditPoints *ep = *((EditPoints **) data);

    col = GetOptionChoice(editp_col_item);
    cformat = GetOptionChoice(editp_format_item);
    cprec = (int) GetSpinChoice(editp_precision_item);
    
    if (col >= 0 && col < MAX_SET_COLS) {
        ep->cformat[col] = cformat;
        ep->cprec[col] = cprec;
        
        update_cells(ep);
        
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

void do_props(EditPoints *ep, int column)
{
    static Widget top;
    static EditPoints *sep;
    
    sep = ep;

    set_wait_cursor();
    
    if (top == NULL) {
        int i;
        Widget dialog;
        OptionItem opitems[MAX_SET_COLS];
        
	top = CreateDialogForm(app_shell, "Edit set properties");
	dialog = CreateVContainer(top);

	for (i = 0; i < MAX_SET_COLS; i++) {
            opitems[i].label = copy_string(NULL, dataset_colname(i));
            opitems[i].value = i;
        }
        editp_col_item = CreateOptionChoice(dialog,
            "Column:", 1, MAX_SET_COLS, opitems);
	for (i = 0; i < MAX_SET_COLS; i++) {
            xfree(opitems[i].label);
        }

	opitems[0].label = "Decimal";
	opitems[0].value = FORMAT_DECIMAL;
	opitems[1].label = "General";
	opitems[1].value = FORMAT_GENERAL;
	opitems[2].label = "Exponential";
	opitems[2].value = FORMAT_EXPONENTIAL;
	opitems[3].label = "Date/time";
	opitems[3].value = FORMAT_YYMMDDHMS;

        editp_format_item = CreateOptionChoice(dialog, "Format:", 1, 4, opitems);

	editp_precision_item = CreateSpinChoice(dialog, "Precision:",
            2, SPIN_TYPE_INT, (double) 0, (double) 20, (double) 1);

	CreateAACDialog(top, dialog, do_accept_props, &sep);
    }
    update_props(sep, column);
    
    RaiseWindow(GetParent(top));
    unset_wait_cursor();
}

static void do_props_cb(Widget but, void *data)
{
    EditPoints *ep = (EditPoints *) data;
    do_props(ep, -1);
}

static void leaveCB(Widget w, XtPointer client_data, XtPointer calld)
{
    int nrows, ncols, scols;
    int changed = FALSE;
    EditPoints *ep = (EditPoints *) client_data;
    XbaeMatrixLeaveCellCallbackStruct *cs =
    	    (XbaeMatrixLeaveCellCallbackStruct *) calld;

    if (get_ep_set_dims(ep, &nrows, &ncols, &scols) != RETURN_SUCCESS) {
        return;
    }
    
    if (cs->column >= ncols + scols) {
        /* should never happen */
        return;
    }

    if (cs->row >= nrows) {
        if (!is_empty_string(cs->value)) {
            set_set_length(ep->pset, cs->row + 1);
            update_set_lists(get_parent_graph(ep->pset));
        } else {
            /* empty cell */
            return;
        }
    }
    
    /* TODO: add edit_point() function to setutils.c */
    if (cs->column < ncols) {
        char *s;
        double *datap = set_get_col(ep->pset, cs->column);
        s = create_fstring(get_parent_project(ep->pset),
            ep->cformat[cs->column], ep->cprec[cs->column],
            datap[cs->row], LFORMAT_TYPE_PLAIN);
        if (strcmp(s, cs->value) != 0) {
	    double value;
            if (parse_date_or_number(get_parent_project(ep->pset),
                cs->value, FALSE, &value) == RETURN_SUCCESS) {
                datap[cs->row] = value;
                changed = TRUE;
            } else {
                errmsg("Can't parse input value");
            }
        }
    } else if (cs->column < ncols + scols) {
        char **datap = set_get_strings(ep->pset);
        if (compare_strings(datap[cs->row], cs->value) == 0) {
	    datap[cs->row] = copy_string(datap[cs->row], cs->value);
            changed = TRUE;
        }
    }
    
    if (changed) {
        quark_dirtystate_set(ep->pset, TRUE);
        
        /* don't refresh this editor */
        ep->update = FALSE;
        update_set_lists(get_parent_graph(ep->pset));
        ep->update = TRUE;
    }
}

/*
 * We use a stack of static buffers to work around asynchronous
 * refresh/redraw events
 */
#define STACKLEN    30

static char *get_cell_content(EditPoints *ep, int row, int column)
{
    static char buf[STACKLEN][32];
    static int stackp = 0;
    
    int ncols, nrows, scols;
    char *s;
    
    if (get_ep_set_dims(ep, &nrows, &ncols, &scols) != RETURN_SUCCESS) {
        s = "";
    } else if (column >= ncols + scols || row >= nrows) {
        s = "";
    } else if (column < ncols) {
        double *datap;
        datap = set_get_col(ep->pset, column);
        strcpy(buf[stackp],
            create_fstring(get_parent_project(ep->pset), ep->cformat[column],
                ep->cprec[column], datap[row], LFORMAT_TYPE_PLAIN));
        s = buf[stackp];
        stackp++;
        stackp %= STACKLEN;
    } else {
        char **datap;
        datap = set_get_strings(ep->pset);
        s = datap[row];
    }
    
    return s;
}

static void drawcellCB(Widget w, XtPointer client_data, XtPointer calld)
{
    EditPoints *ep = (EditPoints *) client_data;
    XbaeMatrixDrawCellCallbackStruct *cs =
    	    (XbaeMatrixDrawCellCallbackStruct *) calld;

    cs->type = XbaeString;
    cs->string = get_cell_content(ep, cs->row, cs->column);
}

static void labelCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    EditPoints *ep = (EditPoints *) client_data;
    XbaeMatrixLabelActivateCallbackStruct *cbs =
	(XbaeMatrixLabelActivateCallbackStruct *) call_data;

    if (cbs->row_label) {
	if (XbaeMatrixIsRowSelected(ep->mw, cbs->row)) {
	    XbaeMatrixDeselectRow(ep->mw, cbs->row);
	} else {
	    XbaeMatrixSelectRow(ep->mw, cbs->row);
        }
    } else {
        int ncols, nrows, scols;
        if (get_ep_set_dims(ep, &nrows, &ncols, &scols) != RETURN_SUCCESS) {
            return;
        } else if (cbs->column < ncols) {
            do_props(ep, cbs->column);
        }
    }
}


static EditPoints *ep_start = NULL;

void delete_ep(EditPoints *ep)
{
    EditPoints *ep_tmp = ep_start;
    
    if (ep == NULL) {
        return;
    }
    
    if (ep == ep_start) {
        ep_start = ep_start->next;
        XCFREE(ep);
        return;
    }
    
    while (ep_tmp != NULL) {
        if (ep_tmp->next == ep) {
            ep_tmp->next = ep->next;
            XCFREE(ep);
            return;
        }
        ep_tmp = ep_tmp->next;
    }
}

EditPoints *get_ep(Quark *pset)
{
    EditPoints *ep_tmp = ep_start;

    while (ep_tmp != NULL) {
        if (ep_tmp->pset == pset) {
            break;
        }
        ep_tmp = ep_tmp->next;
    }
    return ep_tmp;
}

EditPoints *get_unused_ep()
{
    EditPoints *ep_tmp = ep_start;

    while (ep_tmp != NULL) {
        if (IsManaged(GetParent(ep_tmp->top)) == False) {
            break;
        }
        ep_tmp = ep_tmp->next;
    }
    return ep_tmp;
}

void update_ss_editors(Quark *gr)
{
    EditPoints *ep = ep_start;

    while (ep != NULL) {
        if (!gr || get_parent_graph(ep->pset) == gr) {
            /* don't spend time on unmanaged SS editors */
            if (IsManaged(GetParent(ep->top))) {
                update_cells(ep);
            }
        }
        ep = ep->next;
    }
}

void close_ss_editors(Quark *pset)
{
    EditPoints *ep = ep_start;

    while (ep != NULL) {
        if (ep->pset == pset) {
            ep->pset = NULL;
            update_cells(ep);
            return;
        }
        ep = ep->next;
    }
}

/*
 * redo frame since number of data points or set type, etc.,  may change 
 */
static void update_cells(EditPoints *ep)
{
    int i, nr, nc, new_nr, new_nc, delta_nr, delta_nc;
    int c_row, c_column;
    int ncols, nrows, scols;
    short widths[MAX_SET_COLS + 1];
    int maxlengths[MAX_SET_COLS + 1];
    char *collabels[MAX_SET_COLS + 1];
    unsigned char column_label_alignments[MAX_SET_COLS + 1];
    short width;
    char buf[32];
    char **rowlabels;

    if (ep->update == FALSE) {
        return;
    }
    
    if (get_ep_set_dims(ep, &nrows, &ncols, &scols) != RETURN_SUCCESS) {
        destroy_dialog_cb(NULL, GetParent(ep->top));
        return;
    }
    
    sprintf(buf, "Dataset %s", QIDSTR(ep->pset));
    SetLabel(ep->label, buf);

    SetOptionChoice(ep->stype, set_get_type(ep->pset));
    sprintf(buf, "%d", set_get_length(ep->pset));
    xv_setstr(ep->length, buf);
    SetTextString(ep->comment, set_get_comment(ep->pset));

    SetToggleButtonState(ep->hotlink, set_is_hotlinked(ep->pset));
    SetOptionChoice(ep->hotsrc, set_get_hotlink_src(ep->pset));
    xv_setstr(ep->hotfile, set_get_hotlink_file(ep->pset));
    
    /* get current size of widget and update rows/columns as needed */
    get_ep_dims(ep, &nr, &nc);

    new_nc = MAX2(ncols + scols, MIN_SS_COLS);
    new_nr = MAX2(nrows, MIN_SS_ROWS);
    
    delta_nr = new_nr - nr;
    delta_nc = new_nc - nc;
    
    /* A bug in Xbae - the cell with focus on is NOT updated, so we do it */
    XbaeMatrixGetCurrentCell(ep->mw, &c_row, &c_column);
    XbaeMatrixSetCell(ep->mw,
        c_row, c_column, get_cell_content(ep, c_row, c_column));
    
    if (delta_nr == 0 && delta_nc == 0) {
        XbaeMatrixRefresh(ep->mw);
        return;
    }
    
    for (i = 0; i < ncols; i++) {
        widths[i] = CELL_WIDTH;
        maxlengths[i] = 2*CELL_WIDTH;
        collabels[i] = copy_string(NULL, dataset_colname(i));
        column_label_alignments[i] = XmALIGNMENT_CENTER;
    }
    if (scols) {
        widths[i] = CELL_WIDTH;
        maxlengths[i] = STRING_CELL_WIDTH;
        collabels[i] = copy_string(NULL, "String");
        column_label_alignments[i] = XmALIGNMENT_CENTER;
    }

    if (delta_nr > 0) {
        rowlabels = xmalloc(delta_nr*sizeof(char *));
        for (i = 0; i < delta_nr; i++) {
    	    sprintf(buf, "%d", nr + i);
    	    rowlabels[i] = copy_string(NULL, buf);
        }
        XbaeMatrixAddRows(ep->mw, nr, NULL, rowlabels, NULL, delta_nr);
        for (i = 0; i < delta_nr; i++) {
	    xfree(rowlabels[i]);
        }
        xfree(rowlabels);
    } else if (delta_nr < 0) {
        XbaeMatrixDeleteRows(ep->mw, nrows, -delta_nr);
        if (nrows < MIN_SS_ROWS) {
            rowlabels = xmalloc(MIN_SS_ROWS*sizeof(char *));
            for (i = 0; i < MIN_SS_ROWS; i++) {
                sprintf(buf, "%d", i);
    	        rowlabels[i] = copy_string(NULL, buf);
            }
            XtVaSetValues(ep->mw, XmNrowLabels, rowlabels, NULL);
            for (i = 0; i < delta_nr; i++) {
	        xfree(rowlabels[i]);
            }
            xfree(rowlabels);
        }
    }
    
    if (delta_nc > 0) {
        XbaeMatrixAddColumns(ep->mw, 0, NULL, NULL, widths, maxlengths, 
            NULL, NULL, NULL, delta_nc);
    } else if (delta_nc < 0) {
        XbaeMatrixDeleteColumns(ep->mw, ncols, -delta_nc);
    }
		
    /* Adjust row label width */
    width = (short) ceil(log10(new_nr)) + 1;
    
    XtVaSetValues(ep->mw,
	XmNrowLabelWidth, width,
        XmNvisibleColumns, ncols + scols,
        XmNcolumnMaxLengths, maxlengths,
        XmNcolumnLabels, collabels,
        XmNcolumnLabelAlignments, column_label_alignments,
	NULL);

    if (delta_nc != 0) {
        XtVaSetValues(ep->mw,
            XmNcolumnWidths, widths,
	    NULL);
    }

    /* free memory used to hold strings */
    for (i = 0; i < ncols + scols; i++) {
	xfree(collabels[i]);
    }
}

int ep_aac_proc(void *data)
{
    EditPoints *ep = (EditPoints *) data;
    int stype;
    char *comment;
    int cur_row, cur_col;
    int len;
    int hotlink, hotsrc;
    char *hotfile;
    
    stype = GetOptionChoice(ep->stype);
    xv_evalexpri(ep->length, &len);
    if (len < 0) {
        errmsg("Negative set length!");
        return RETURN_FAILURE;
    }
    comment = GetTextString(ep->comment);
    
    set_set_type(ep->pset, stype);
    set_set_length(ep->pset, len);
    set_set_comment(ep->pset, comment);

    hotlink = GetToggleButtonState(ep->hotlink);
    hotsrc  = GetOptionChoice(ep->hotsrc);
    hotfile = xv_getstr(ep->hotfile);
    set_set_hotlink(ep->pset, hotlink, hotfile, hotsrc);

    /* commit the last entered cell changes */
    XbaeMatrixGetCurrentCell(ep->mw, &cur_row, &cur_col);
    XbaeMatrixEditCell(ep->mw, cur_row, cur_col);

    update_set_lists(get_parent_graph(ep->pset));
    
    xfree(comment);
    
    return RETURN_SUCCESS;
}

static int do_hotlinkfile_proc(FSBStructure *fsb, char *filename, void *data)
{
    EditPoints *ep = (EditPoints *) data;
    
    xv_setstr(ep->hotfile, filename);
    
    return TRUE;
}

/*
 * create file selection pop up to choose the file for hotlink
 */
static void create_hotfiles_popup(Widget but, void *data)
{
    static FSBStructure *fsb = NULL;

    set_wait_cursor();

    if (fsb == NULL) {
        fsb = CreateFileSelectionBox(app_shell, "Hotlinked file");
	AddFileSelectionBoxCB(fsb, do_hotlinkfile_proc, data);
        ManageChild(fsb->FSB);
    }
    
    RaiseWindow(fsb->dialog);

    unset_wait_cursor();
}

static EditPoints *new_ep(void)
{
    int i;
    short widths[MIN_SS_COLS];
    char *rowlabels[MIN_SS_ROWS];
    EditPoints *ep;
    Widget fr, rc, rc1, wbut, menubar, menupane;
    
    ep = xmalloc(sizeof(EditPoints));
    ep->next = ep_start;
    ep_start = ep;
    
    ep->update = TRUE;
    
    for (i = 0; i < MAX_SET_COLS; i++) {
        ep->cprec[i] = CELL_PREC;
        ep->cformat[i] = CELL_FORMAT;
    }

    ep->top = CreateDialogForm(app_shell, "Spreadsheet dataset editor");
    SetDialogFormResizable(ep->top, TRUE);

    menubar = CreateMenuBar(ep->top);
    ManageChild(menubar);
    AddDialogFormChild(ep->top, menubar);

    menupane = CreateMenu(menubar, "File", 'F', FALSE);
    CreateMenuCloseButton(menupane, ep->top);

    menupane = CreateMenu(menubar, "Edit", 'E', FALSE);
    CreateMenuButton(menupane, "Add row", 'A', add_row_cb, ep);
    CreateMenuButton(menupane, "Delete selected rows", 'D', del_rows_cb, ep);
    CreateMenuSeparator(menupane);
    CreateMenuButton(menupane, "Column format...", 'f', do_props_cb, ep);

    menupane = CreateMenu(menubar, "Help", 'H', TRUE);
    CreateMenuHelpButton(menupane, "On dataset editor", 'e', ep->top,
        "doc/UsersGuide.html#SSEditor");

    fr = CreateFrame(ep->top, NULL);
    AddDialogFormChild(ep->top, fr);
    ep->label = CreateLabel(fr, "Dataset G*.S*");

    fr = CreateFrame(ep->top, NULL);
    AddDialogFormChild(ep->top, fr);
    rc = CreateVContainer(fr);
    rc1 = CreateHContainer(rc);
    ep->stype = CreateSetTypeChoice(rc1, "Type:");
    ep->length = CreateTextItem(rc1, 6, "Length:");
    ep->comment = CreateTextInput(rc, "Comment:");

    fr = CreateFrame(ep->top, "Hotlink");
    AddDialogFormChild(ep->top, fr);
    rc = CreateVContainer(fr);
    rc1 = CreateHContainer(rc);
    ep->hotlink = CreateToggleButton(rc1, "Enabled");
    ep->hotsrc  = CreateOptionChoiceVA(rc1, "Source type:",
        "Disk", SOURCE_DISK,
        "Pipe", SOURCE_PIPE,
        NULL);
    rc1 = CreateHContainer(rc);
    ep->hotfile = CreateTextItem(rc1, 20, "File name:");
    wbut = CreateButton(rc1, "Browse...");
    AddButtonCB(wbut, create_hotfiles_popup, ep);

    for (i = 0; i < MIN_SS_ROWS; i++) {
    	char buf[32];
        sprintf(buf, "%d", i);
    	rowlabels[i] = copy_string(NULL, buf);
    }
    for (i = 0; i < MIN_SS_COLS; i++) {
        widths[i] = CELL_WIDTH;
    }

    ep->mw = XtVaCreateManagedWidget("mw",
        xbaeMatrixWidgetClass, ep->top,
        XmNrows, MIN_SS_ROWS,
        XmNvisibleRows, VISIBLE_SS_ROWS,
        XmNbuttonLabels, True,
        XmNrowLabels, rowlabels,
        XmNcolumns, MIN_SS_COLS,
        XmNvisibleColumns, MIN_SS_COLS,
        XmNcolumnWidths, widths,
        XmNallowColumnResize, True,
        XmNgridType, XmGRID_CELL_SHADOW,
        XmNcellShadowType, XmSHADOW_ETCHED_OUT,
        XmNcellShadowThickness, 2,
        XmNaltRowCount, 0,
        XmNcalcCursorPosition, True,
        NULL);

    for (i = 0; i < MIN_SS_ROWS; i++) {
	xfree(rowlabels[i]);
    }

    XtAddCallback(ep->mw, XmNdrawCellCallback, drawcellCB, ep);	
    XtAddCallback(ep->mw, XmNleaveCellCallback, leaveCB, ep);
    XtAddCallback(ep->mw, XmNlabelActivateCallback, labelCB, ep);

    CreateAACDialog(ep->top, ep->mw, ep_aac_proc, ep);

    return ep;
}

void create_ss_frame(Quark *pset)
{
    EditPoints *ep;

    set_wait_cursor();

    /* first, try a previously opened editor with the same set */
    ep = get_ep(pset);
    if (ep == NULL) {
        /* if failed, a first unmanaged one */
        ep = get_unused_ep();
        /* if none available, create a new one */
        if (ep == NULL) {
            ep = new_ep();
        }
    }
    
    if (ep == NULL) {
        errmsg("Internal error in create_ss_frame()");
        unset_wait_cursor();
        return;
    }
    
    ep->pset = pset;
    
    update_cells(ep);
    
    RaiseWindow(GetParent(ep->top));
    
    unset_wait_cursor();

    return;   
}

/*
 * Start up external editor
 */
void do_ext_editor(Quark *pset)
{
    char *fname, ebuf[256];
    FILE *cp;
    int save_autos;
    Quark *pr = get_parent_project(pset);
    Grace *grace = grace_from_quark(pset);

    fname = tmpnam(NULL);
    cp = grace_openw(grace, fname);
    if (cp == NULL) {
        return;
    }

    write_set(pset, cp, project_get_sformat(pr));
    grace_close(cp);

    sprintf(ebuf, "%s %s", get_editor(grace), fname);
    system_wrap(ebuf);

    /* temporarily disable autoscale */
    save_autos = grace->rt->autoscale_onread;
    grace->rt->autoscale_onread = AUTOSCALE_NONE;
    if (!set_is_dataless(pset)) {
        grace->rt->curtype = set_get_type(pset);
        grace->rt->target_set = pset;
	killsetdata(pset);	
    }
    getdata(pr, fname, SOURCE_DISK, LOAD_SINGLE);
    grace->rt->autoscale_onread = save_autos;
    unlink(fname);
    update_set_lists(get_parent_graph(pset));
}
