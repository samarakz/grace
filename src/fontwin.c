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
 * Font tool
 *
 */

#include <config.h>

#include <X11/X.h>

#ifdef WITH_EDITRES
#  include <X11/Xmu/Editres.h>
#endif

#include <Xm/Xm.h>
#include <Xm/DrawingA.h>
#include <Xm/Frame.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/Separator.h>
#include <Xm/ScrolledW.h>
#include <Xm/DialogS.h>
#include <Xm/Text.h>
#include <Xm/XmosP.h>

#ifdef HAVE_LIBXBAE
#  include <Xbae/Matrix.h>
#endif

#include <t1lib.h>

#include "utils.h"
#include "motifinc.h"
#include "protos.h"

#ifdef HAVE_LIBXBAE

/* used globally */
extern XtAppContext app_con;
extern Widget app_shell;
extern Display *disp;
extern Window xwin;
extern Window root;

extern unsigned long xvlibcolors[];

static Widget fonttool_frame = NULL;
static Widget font_table;
static OptionStructure font_select_item;
static Widget string_item;
static Widget glyph_item;

static int FontID = 0;

static void DrawCB(Widget w,XtPointer cd, XbaeMatrixDrawCellCallbackStruct *cbs);
static void EnterCB(Widget w, XtPointer cd, XbaeMatrixEnterCellCallbackStruct *cbs);
static void update_fonttool(Widget w, XtPointer client_data, XtPointer call_data);
static void EditStringCB(Widget w, XtPointer client_data, XmAnyCallbackStruct *cbs);

void create_fonttool(Widget w, XtPointer client_data, XtPointer call_data)
{
    int i;
    short widths[16];
    Widget fonttool_panel, scrolled_window;
    
    if (fonttool_frame == NULL) {
	fonttool_frame = XmCreateDialogShell(app_shell, "Font tool", NULL, 0);
	handle_close(fonttool_frame);
        fonttool_panel = XtVaCreateWidget("fonttool_panel", xmFormWidgetClass, 
                                        fonttool_frame, NULL, 0);

        font_select_item = CreateFontChoice(fonttool_panel, "Font:");
        AddOptionChoiceCB(font_select_item, update_fonttool);
        XtVaSetValues(font_select_item.rc,
            XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNtopAttachment, XmATTACH_FORM,
            NULL);
        
        for (i = 0; i < 16; i++) {
            widths[i] = 2;
        }
        font_table = XtVaCreateManagedWidget(
            "fontTable", xbaeMatrixWidgetClass, fonttool_panel,
            XmNrows, 16,
            XmNcolumns, 16,
            XmNvisibleRows, 8,
            XmNvisibleColumns, 16,
            XmNfill, True,
            XmNcolumnWidths, widths,
	    XmNgridType, XmGRID_SHADOW_IN,
	    XmNcellShadowType, XmSHADOW_ETCHED_OUT,
	    XmNcellShadowThickness, 2,
            XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNtopAttachment, XmATTACH_WIDGET,
            XmNtopWidget, font_select_item.rc,
            NULL);
            
        XtAddCallback(font_table, XmNdrawCellCallback, (XtCallbackProc) DrawCB, NULL);
        XtAddCallback(font_table, XmNenterCellCallback, (XtCallbackProc) EnterCB, NULL);

        string_item = CreateTextItem2(fonttool_panel, 50, "String:");
        XtVaSetValues(XtParent(string_item),
            XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNtopAttachment, XmATTACH_WIDGET,
            XmNtopWidget, font_table,
            NULL);

/*
 *         XtAddCallback(string_item, XmNvalueChangedCallback, (XtCallbackProc) EditStringCB, NULL);
 */
        XtAddCallback(string_item, XmNmodifyVerifyCallback, (XtCallbackProc) EditStringCB, NULL);
        
        scrolled_window = XtVaCreateManagedWidget("scrolled_window",
	    xmScrolledWindowWidgetClass, fonttool_panel,
	    XmNscrollingPolicy, XmAUTOMATIC,
	    XmNvisualPolicy, XmVARIABLE,
            XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNtopAttachment, XmATTACH_WIDGET,
            XmNtopWidget, XtParent(string_item),
            XmNbottomAttachment, XmATTACH_FORM,
	    NULL);

        glyph_item = XtVaCreateManagedWidget("glyph",
            xmDrawingAreaWidgetClass, scrolled_window,
	    XmNheight, (Dimension) 100,
	    XmNwidth, (Dimension) 600,
	    XmNresizePolicy, XmRESIZE_ANY,
            XmNbackground,
	    xvlibcolors[0],
	    NULL);

        XtManageChild(fonttool_panel);
    }
    
    XtRaise(fonttool_frame);
}

static T1_TMATRIX UNITY_MATRIX = {1.0, 0.0, 0.0, 1.0};

static void DrawCB(Widget w, XtPointer cd, XbaeMatrixDrawCellCallbackStruct *cbs)
{
    unsigned char c;
    GLYPH *glyph;
    int height, width;
    Pixmap pixmap;
    float Size = 20.0;
    char dummy_bits[1] = {0};
    int valid_char;
        
    c = 16*cbs->row + cbs->column;
    
    glyph = T1_SetChar(FontID, c, Size, &UNITY_MATRIX);
       
    if (glyph != NULL) {
        valid_char = TRUE;
        height = glyph->metrics.ascent - glyph->metrics.descent;
        width = glyph->metrics.rightSideBearing - glyph->metrics.leftSideBearing;
        pixmap = XCreateBitmapFromData(disp, root,
             (char *) glyph->bits, width, height);
    } else {
        if (c == ' ') {
            valid_char = TRUE;
        } else {
            valid_char = FALSE;
        }
        pixmap = XCreateBitmapFromData(disp, root,
             dummy_bits, 1, 1);
    }
    
    /* Assign it a pixmap */
    cbs->pixmap = pixmap;
    cbs->type = XbaePixmap;
    XbaeMatrixSetCellUserData(w, cbs->row, cbs->column, (XtPointer) valid_char);  
   
/*
 *     cbs->mask = mask;
 */

    /* Set height and width to that of the bitmap's to ensure correct
       positioning and drawing */

/*  This is no longer necessary as it is calculated on the fly */
/*  cbs->height = xbae_height; */
/*  cbs->width = xbae_width; */
    return;
}

static void insert_into_string(char *s)
{
    int pos;
    
    pos = XmTextGetInsertionPosition(string_item);
    XmTextInsert(string_item, pos, s);
}

static void EnterCB(Widget w, XtPointer cd, XbaeMatrixEnterCellCallbackStruct *cbs)
{
    int valid_char;
    char s[2];
    
    valid_char = (int) XbaeMatrixGetCellUserData(w, cbs->row, cbs->column);
    if (valid_char == TRUE) {
        s[0] = 16*cbs->row + cbs->column;
        s[1] = '\0';
        insert_into_string(s);
    } else {
        XBell(disp, 25);
    }
}


static void update_fonttool(Widget w, XtPointer client_data, XtPointer call_data)
{
    char buf[32];
    
    FontID = (int) client_data;
    
    XbaeMatrixRefresh(font_table);
    sprintf(buf, "\\f{%d}", FontID);
    insert_into_string(buf);
}

static void EditStringCB(Widget w, XtPointer client_data, XmAnyCallbackStruct *cbs)
{
    int i;
    unsigned char c;
    int valid_char;
    char string[512];
    static int column = 0, row = 0;
    XmTextVerifyCallbackStruct *tcbs;
    XmTextBlock text;
/*
 *     VPoint vp;
 */
    
    tcbs = (XmTextVerifyCallbackStruct *) cbs;
    text = tcbs->text;
    for (i = 0; i < text->length; i++) {
        XbaeMatrixDeselectCell(font_table, row, column);
        
        c = text->ptr[i];
        row = c/16;
        column = c % 16;

        valid_char = (int) XbaeMatrixGetCellUserData(font_table, row, column);
        if (valid_char == TRUE) {
            XbaeMatrixSelectCell(font_table, row, column);
        } else {
            tcbs->doit = False;
            break;
        }
    }
    
    strcpy(string, xv_getstr(string_item));
/*
 *     vp.x = 0.5;
 *     vp.y = 0.5;
 *     WriteString(vp, 0, 0, string);
 */
}

#else /* No Xbae */

void create_fonttool(Widget w, XtPointer client_data, XtPointer call_data)
{
    errmsg("No fonttool for this build");
}

#endif
