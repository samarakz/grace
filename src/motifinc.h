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
 * for Motif specific items
 */

#ifndef __MOTIFINC_H_
#define __MOTIFINC_H_

#if (XmVersion < 1002)
#  define XmStringCreateLocalized XmStringCreateSimple
#endif

/* 
 * Accept/Apply/Close for aac_cb callbacks
 */
#define AAC_ACCEPT  0
#define AAC_APPLY   1
#define AAC_CLOSE   2

extern Widget app_shell;        /* defined in xmgrace.c */
extern XmStringCharSet charset; /* defined in xmgrace.c */

/* set selection gadget */
typedef struct _SetChoiceItem {
    int type;
    int display;
    int gno;
    int spolicy;
    int fflag; /* if 0, no filter gadgets */
    int indx;
    Widget list;
    Widget rb;
    Widget but[8];
} SetChoiceItem;

typedef struct {
    int value;
    char *label;
} OptionItem;

typedef struct {
    int value;
    unsigned char *bitmap;
} BitmapOptionItem;

typedef struct {
    int value;
    Widget widget;
} OptionWidgetItem;

typedef struct {
    int nchoices;
    Widget rc;
    Widget menu;
    Widget pulldown;
    OptionWidgetItem *options;
} OptionStructure;

typedef struct {
    int nchoices;
    int *values;
    void *anydata;
    Widget rc;
    Widget list;
    Widget popup;
} ListStructure;

Widget CreateFrame(Widget parent, char *s);

Widget CreateTab(Widget parent);
Widget CreateTabPage(Widget parent, char *s);
void SelectTabPage(Widget tab, Widget w);

Widget CreateCharSizeChoice(Widget parent, char *s);
double GetCharSizeChoice(Widget w);
void SetCharSizeChoice(Widget w, double size);

Widget CreateAngleChoice(Widget parent, char *s);
int GetAngleChoice(Widget w);
void SetAngleChoice(Widget w, int angle);

Widget CreateToggleButton(Widget parent, char *s);
int GetToggleButtonState(Widget w);
void SetToggleButtonState(Widget w, int value);

Widget CreateAACButtons(Widget parent, Widget form, XtCallbackProc aac_cb);

OptionStructure CreateOptionChoice(Widget parent, char *labelstr, int ncols,
                                                int nchoices, OptionItem *items);
OptionStructure CreateBitmapOptionChoice(Widget parent, char *labelstr, int ncols,
                int nchoices, int width, int height, BitmapOptionItem *items);
void SetOptionChoice(OptionStructure opt, int value);
int GetOptionChoice(OptionStructure opt);

void AddOptionChoiceCB(OptionStructure opt, XtCallbackProc cb);

ListStructure CreateListChoice(Widget parent, char *labelstr,
                                              int nchoices, OptionItem *items);
void SelectListChoice(ListStructure list, int choice);
void SelectListChoices(ListStructure list, int nchoices, int *choices);
void UpdateListChoice(ListStructure *listp, int nchoices, OptionItem *items);
int GetListChoices(ListStructure *listp, int **values);

void list_choice_selectall(Widget w, XEvent *e, String *par, Cardinal *npar);

OptionStructure CreateFontChoice(Widget parent, char *s);
OptionStructure CreatePatternChoice(Widget parent, char *s);
OptionStructure CreateLineStyleChoice(Widget parent, char *s);

ListStructure CreateNewGraphChoice(Widget parent, char *labelstr, XtCallbackProc cbproc);

Widget *CreateSetChoice(Widget parent, char *labelstr, int nsets, int type);
SetChoiceItem CreateSetSelector(Widget parent, char *label, int type, int ff, int gtype, int stype);
int GetSelectedSet(SetChoiceItem l);
int GetSelectedSets(SetChoiceItem l, int **sets);
void update_set_list(int gno, SetChoiceItem l);
int save_set_list(SetChoiceItem l);
void update_save_set_list( SetChoiceItem l, int newgr );

void SetSelectorFilterCB(Widget parent, XtPointer cld, XtPointer calld);
void DefineSetSelectorFilter(SetChoiceItem *s);
int SetSelectedSet(int gno, int setno, SetChoiceItem l);

Widget *CreatePanelChoice(Widget parent, char *labstr, int nchoices, ...);
Widget *CreatePanelChoice0(Widget parent, char *labstr, int ncols, int nchoices, ...);
void SetChoice(Widget * w, int value);
int GetChoice(Widget * w);

Widget *CreateGraphChoice(Widget parent, char *labelstr, int ngraphs, int type);
Widget *CreateColorChoice(Widget parent, char *s);
Widget *CreateLineWidthChoice(Widget parent, char *s);

Widget *CreateFormatChoice(Widget parent, char *s);
Widget *CreatePrecisionChoice(Widget parent, char *s);

Widget CreateTextItem2(Widget parent, int len, char *s);
Widget CreateTextItem4(Widget parent, int len, char *s);
Widget CreateScrollTextItem2(Widget parent, int len, int hgt, char *s);

Widget CreateCommandButtons(Widget parent, int n, Widget * buts, char **l);
Widget CreateCommandButtonsNoDefault(Widget parent, int n, Widget * buts, char **l);

Widget CreateMenuBar(Widget parent, char *name, char *help_anchor);
Widget CreateMenu(Widget parent, char *name, char *label, char mnemonic,
	Widget *cascade, char *help_anchor);
Widget CreateMenuButton(Widget parent, char *name, char *label, char mnemonic,
	XtCallbackProc cb, XtPointer data, char *help_anchor);
Widget CreateMenuToggle(Widget parent, char *name, char *label, char mnemonic,
	XtCallbackProc cb, XtPointer data, char *help_anchor);
Widget CreateMenuSeparator(Widget parent, char *name);
Widget CreateMenuLabel(Widget parent, char *name);

#endif /* __MOTIFINC_H_ */
