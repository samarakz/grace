/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 2002-2005 Grace Development Team
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

#include <config.h>

#include "globals.h"
#include "utils.h"
#include "dicts.h"

#include "explorer.h"
#include "qbitmaps.h"

#if defined(HAVE_XPM_H)
#  include <xpm.h>
#else
#  include <X11/xpm.h>
#endif

#include <Xm/ScrolledW.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#if XmVersion >= 2000
# define USE_PANEDW 1
#  include <Xm/PanedW.h>
#else
# define USE_PANEDW 0
#endif

#include "protos.h"

/* Quark labeling procedure */
typedef char * (*Quark_LabelingProc)(
    Quark *q
);

/* Quark creating procedure */
typedef ListTreeItem * (*Quark_CreateProc)(
    Widget w,
    ListTreeItem *parent,
    char *string,
    void *udata
);

typedef struct {
    Quark *q;
    Widget tree;
    unsigned int nchoices;
    Quark_LabelingProc labeling_proc;
} TreeItemData;

static void highlight_selected(Widget w, ListTreeItem *parent,
    int nsquarks, Quark **squarks);

static ListTreeItem *q_create(Widget w,
    ListTreeItem *parent, char *string, void *udata);

static char *default_quark_labeling_proc(Quark *q)
{
    char buf[128];
    
    sprintf(buf, "Quark \"%s\"", QIDSTR(q));
    
    return copy_string(NULL, buf);
}

static int traverse_hook(unsigned int step, void *data, void *udata)
{
    char *s;
    ListTreeItem *ti = (ListTreeItem *) udata;
    TreeItemData *ti_data = (TreeItemData *) ti->user_data;
    Quark *q = (Quark *) data;
    
    s = ti_data->labeling_proc(q);
    if (s) {
        if (step >= ti_data->nchoices) {
            q_create(ti_data->tree, ti, s, data);
            ti_data->nchoices++;
        } else {
            ListTreeRenameItem(ti_data->tree, ti, s);
        }
        xfree(s);
    }
    
    return TRUE;
}

void UpdateQuarkTree(ListTreeItem *ti)
{
    TreeItemData *ti_data = (TreeItemData *) ti->user_data;
    if (!ti_data->q || quark_count_children(ti_data->q) <= 0) {
        ListTreeDeleteChildren(ti_data->tree, ti);
        ti_data->nchoices = 0;
    }
    if (ti_data->q) {
        storage_traverse(quark_get_children(ti_data->q), traverse_hook, ti);
    }
}   

ListTreeItem *CreateQuarkTree(Widget tree, ListTreeItem *parent,
    Quark *q, const char *label, Quark_LabelingProc labeling_proc)
{
    ListTreeItem *item;
    TreeItemData *data;
    char *s;
    GUI *gui;
    
    if (!q) {
        return NULL;
    }
    
    gui = gui_from_quark(q);
    
    data = xmalloc(sizeof(TreeItemData));
    data->q = q;
    data->tree = tree;
    data->nchoices = 0;
    
    if (labeling_proc) {
        data->labeling_proc = labeling_proc;
    } else {
        data->labeling_proc = default_quark_labeling_proc;
    }
    
    if (label) {
        s = copy_string(NULL, label);
    } else {
        s = data->labeling_proc(q);
    }
    
    item = ListTreeAdd(tree, parent, s);
    xfree(s);
    item->user_data = data;

    if (quark_is_active(q) && quark_count_children(q) > 0) {
        item->open = True;
    }
    
    if (quark_is_active(q)) {
        ListTreeSetItemPixmaps(tree, item, gui->eui->a_icon, gui->eui->a_icon);
    } else {
        ListTreeSetItemPixmaps(tree, item, gui->eui->h_icon, gui->eui->h_icon);
    }
    
    UpdateQuarkTree(item);
    
    return item;
}

void SelectQuarkTreeItem(Widget w, ListTreeItem *parent, Quark *q)
{
    ListTreeMultiReturnStruct ret;

    ListTreeClearHighlighted(w);
    highlight_selected(w, parent, 1, &q);
    
    ListTreeGetHighlighted(w, &ret);
    XtCallCallbacks(w, XtNhighlightCallback, (XtPointer) &ret);

    if (ret.count > 0) {
        ListTreeSetPos(w, ret.items[0]);
    }
}

static int explorer_apply(ExplorerUI *ui, void *caller);

static ListTreeItem *q_create(Widget w,
    ListTreeItem *parent, char *string, void *udata)
{
    ListTreeItem *item;
    Quark *q = (Quark *) udata;
    TreeItemData *p_data = (TreeItemData *) parent->user_data;
    
    item = CreateQuarkTree(w, parent, q, string, p_data->labeling_proc);
    
    return item;
}

static void manage_plugin(ExplorerUI *ui, Widget managed_top)
{
    if (managed_top == ui->project_ui->top) {
        ManageChild(ui->project_ui->top);
    } else {
        UnmanageChild(ui->project_ui->top);
    }
    if (managed_top == ui->ssd_ui->top) {
        ManageChild(ui->ssd_ui->top);
    } else {
        UnmanageChild(ui->ssd_ui->top);
    }
    if (managed_top == ui->frame_ui->top) {
        ManageChild(ui->frame_ui->top);
    } else {
        UnmanageChild(ui->frame_ui->top);
    }
    if (managed_top == ui->graph_ui->top) {
        ManageChild(ui->graph_ui->top);
    } else {
        UnmanageChild(ui->graph_ui->top);
    }
    if (managed_top == ui->set_ui->top) {
        ManageChild(ui->set_ui->top);
    } else {
        UnmanageChild(ui->set_ui->top);
    }
    if (managed_top == ui->axisgrid_ui->top) {
        ManageChild(ui->axisgrid_ui->top);
    } else {
        UnmanageChild(ui->axisgrid_ui->top);
    }
    if (managed_top == ui->axis_ui->top) {
        ManageChild(ui->axis_ui->top);
    } else {
        UnmanageChild(ui->axis_ui->top);
    }
    if (managed_top == ui->object_ui->top) {
        ManageChild(ui->object_ui->top);
    } else {
        UnmanageChild(ui->object_ui->top);
    }
    if (managed_top == ui->atext_ui->top) {
        ManageChild(ui->atext_ui->top);
    } else {
        UnmanageChild(ui->atext_ui->top);
    }
    if (managed_top == ui->region_ui->top) {
        ManageChild(ui->region_ui->top);
    } else {
        UnmanageChild(ui->region_ui->top);
    }
}

static void highlight_cb(Widget w, XtPointer client, XtPointer call)
{
    ExplorerUI *ui = (ExplorerUI *) client;
    ListTreeMultiReturnStruct *ret;
    int count;
    Quark *q = NULL;
    int fid = -1;
    int all_shown = TRUE;
    int all_hidden = TRUE;

    ret = (ListTreeMultiReturnStruct *) call;
    count = ret->count;
    
    ui->homogeneous_selection = TRUE;
    ui->all_siblings = TRUE;

    if (count > 0) {
        int i;
        ListTreeItem *item = ret->items[0];
        TreeItemData *ti_data = (TreeItemData *) item->user_data;
        Quark *parent;
        
        q = ti_data->q;
        fid = quark_fid_get(q);
        parent = quark_parent_get(q);
        all_shown  = quark_is_active(q);
        all_hidden = !all_shown;
        
        for (i = 1; i < count; i++) {
            item = ret->items[i];
            ti_data = (TreeItemData *) item->user_data;
            
            if ((int) quark_fid_get(ti_data->q) != fid) {
                ui->homogeneous_selection = FALSE;
            }
            if (quark_parent_get(ti_data->q) != parent) {
                ui->all_siblings = FALSE;
            }
            if (quark_is_active(ti_data->q)) {
                all_hidden = FALSE;
            } else {
                all_shown = FALSE;
            }
        }
    }
    
    if (!count || !ui->homogeneous_selection) {
        SetSensitive(ui->aacbuts[0], FALSE);
        SetSensitive(ui->aacbuts[1], FALSE);
        
        manage_plugin(ui, NULL);
    } else {
        SetSensitive(ui->aacbuts[0], TRUE);
        SetSensitive(ui->aacbuts[1], TRUE);
        
        if (count == 1) {
            Widget managed_top;
            switch (fid) {
            case QFlavorProject:
                update_project_ui(ui->project_ui, q);
                managed_top = ui->project_ui->top;
                break;
            case QFlavorSSD:
                update_ssd_ui(ui->ssd_ui, q);
                managed_top = ui->ssd_ui->top;
                break;
            case QFlavorFrame:
                update_frame_ui(ui->frame_ui, q);
                managed_top = ui->frame_ui->top;
                break;
            case QFlavorGraph:
                update_graph_ui(ui->graph_ui, q);
                managed_top = ui->graph_ui->top;
                break;
            case QFlavorSet:
                update_set_ui(ui->set_ui, q);
                managed_top = ui->set_ui->top;
                break;
            case QFlavorAGrid:
                update_axisgrid_ui(ui->axisgrid_ui, q);
                managed_top = ui->axisgrid_ui->top;
                break;
            case QFlavorAxis:
                update_axis_ui(ui->axis_ui, q);
                managed_top = ui->axis_ui->top;
                break;
            case QFlavorDObject:
                update_object_ui(ui->object_ui, q);
                managed_top = ui->object_ui->top;
                break;
            case QFlavorAText:
                update_atext_ui(ui->atext_ui, q);
                managed_top = ui->atext_ui->top;
                break;
            case QFlavorRegion:
                update_region_ui(ui->region_ui, q);
                managed_top = ui->region_ui->top;
                break;
            default:
                managed_top = NULL;
                break;
            }
            
            manage_plugin(ui, managed_top);
        }
    }

    if (!count || fid == QFlavorProject) {
        SetSensitive(ui->popup_hide_bt,           FALSE);
        SetSensitive(ui->popup_show_bt,           FALSE);
    } else {
        SetSensitive(ui->popup_hide_bt, !all_hidden);
        SetSensitive(ui->popup_show_bt, !all_shown);
    }
        
    if (!count || !ui->all_siblings || fid == QFlavorProject) {
        SetSensitive(ui->popup_delete_bt,         FALSE);
        SetSensitive(ui->popup_duplicate_bt,      FALSE);
        SetSensitive(ui->popup_bring_to_front_bt, FALSE);
        SetSensitive(ui->popup_send_to_back_bt,   FALSE);
        SetSensitive(ui->popup_move_up_bt,        FALSE);
        SetSensitive(ui->popup_move_down_bt,      FALSE);
    } else {
        SetSensitive(ui->popup_delete_bt,         TRUE);
        SetSensitive(ui->popup_duplicate_bt,      TRUE);
        SetSensitive(ui->popup_bring_to_front_bt, TRUE);
        SetSensitive(ui->popup_send_to_back_bt,   TRUE);
        SetSensitive(ui->popup_move_up_bt,        TRUE);
        SetSensitive(ui->popup_move_down_bt,      TRUE);
    }
    
    SetSensitive(ui->insert_frame_bt,    FALSE);
    SetSensitive(ui->insert_graph_bt,    FALSE);
    SetSensitive(ui->insert_set_bt,      FALSE);
    SetSensitive(ui->insert_ssd_bt,      FALSE);
    SetSensitive(ui->insert_axisgrid_bt, FALSE);
    SetSensitive(ui->insert_axis_bt,     FALSE);
    SetSensitive(ui->insert_object_pane, FALSE);
    if (count == 1) {
        SetSensitive(ui->idstr->form, TRUE);
        SetTextString(ui->idstr, QIDSTR(q));
        switch (fid) {
        case QFlavorProject:
            SetSensitive(ui->insert_frame_bt,    TRUE);
            SetSensitive(ui->insert_object_pane, TRUE);
            break;
        case QFlavorFrame:
            SetSensitive(ui->insert_graph_bt,    TRUE);
            SetSensitive(ui->insert_object_pane, TRUE);
            break;
        case QFlavorGraph:
            SetSensitive(ui->insert_ssd_bt, TRUE);
            SetSensitive(ui->insert_axisgrid_bt, TRUE);
            SetSensitive(ui->insert_object_pane, TRUE);
            break;
        case QFlavorAGrid:
            SetSensitive(ui->insert_axis_bt,     TRUE);
            break;
        case QFlavorAxis:
            SetSensitive(ui->insert_object_pane, TRUE);
            break;
        }
        
        if (get_parent_ssd(q) &&
            (fid == QFlavorGraph || fid == QFlavorSSD)) {
            SetSensitive(ui->insert_set_bt,      TRUE);
        }
    } else {
        SetSensitive(ui->idstr->form, FALSE);
        SetTextString(ui->idstr, NULL);
    }
}

static void menu_cb(Widget w, XtPointer client, XtPointer call)
{
    ListTreeItemReturnStruct *ret = (ListTreeItemReturnStruct *) call;
    XButtonEvent *xbe = (XButtonEvent *) ret->event;
    ExplorerUI *ui = (ExplorerUI *) client;

    XmMenuPosition(ui->popup, xbe);
    XtManageChild(ui->popup);
}

static void destroy_cb(Widget w, XtPointer client, XtPointer call)
{
    ListTreeItemReturnStruct *ret;

    ret = (ListTreeItemReturnStruct *) call;
    
    xfree(ret->item->user_data);
}

static void drop_cb(Widget w, XtPointer client, XtPointer call)
{
    ExplorerUI *ui = (ExplorerUI *) client;
    ListTreeDropStruct *cbs = (ListTreeDropStruct *) call;
    TreeItemData *ti_data = (TreeItemData *) cbs->item->user_data;
    Quark *drop_q = ti_data->q;

    if (ui->all_siblings && ui->homogeneous_selection) {
        int count;
        ListTreeMultiReturnStruct ret;
        
        ListTreeGetHighlighted(ui->tree, &ret);
        count = ret.count;
        if (count > 0) {
            int i;
            Quark *parent;
            ListTreeItem *item = ret.items[0];
            ti_data = (TreeItemData *) item->user_data;
            parent = quark_parent_get(ti_data->q);
            
            if (parent && parent != drop_q &&
                quark_fid_get(parent) == quark_fid_get(drop_q)) {
                for (i = 0; i < count; i++) {
                    Quark *q;
                    item = ret.items[i];
                    ti_data = (TreeItemData *) item->user_data;
                    q = ti_data->q;
                    if (cbs->operation == XmDROP_COPY) {
                        quark_copy2(drop_q, q);
                    } else {
                        quark_reparent(q, drop_q);
                    }
                }
                cbs->ok = True;
                snapshot_and_update(drop_q, TRUE);
            }
        }
    }
}

static int explorer_apply(ExplorerUI *ui, void *caller)
{
    ListTreeMultiReturnStruct ret;
    int count, i, res = RETURN_SUCCESS;
    
    if (caller && !GetToggleButtonState(ui->instantupdate)) {
        return RETURN_FAILURE;
    }
    
    ListTreeGetHighlighted(ui->tree, &ret);
    count = ret.count;

    for (i = 0; i < count && res == RETURN_SUCCESS; i++) {
        ListTreeItem *item = ret.items[i];
        TreeItemData *ti_data = (TreeItemData *) item->user_data;
        Quark *q = ti_data->q;

        if (count == 1 && (!caller || caller == ui->idstr)) {
            char *s = GetTextString(ui->idstr);
            quark_idstr_set(q, s);
            xfree(s);
        }

        switch (quark_fid_get(q)) {
        case QFlavorProject:
            if (set_project_data(ui->project_ui, q, caller) != RETURN_SUCCESS) {
                res = RETURN_FAILURE;
            }
            break;
        case QFlavorSSD:
            if (set_ssd_data(ui->ssd_ui, q, caller) != RETURN_SUCCESS) {
                res = RETURN_FAILURE;
            }
            break;
        case QFlavorFrame:
            if (set_frame_data(ui->frame_ui, q, caller) != RETURN_SUCCESS) {
                res = RETURN_FAILURE;
            }
            break;
        case QFlavorGraph:
            if (graph_set_data(ui->graph_ui, q, caller) != RETURN_SUCCESS) {
                res = RETURN_FAILURE;
            }
            break;
        case QFlavorSet:
            if (set_set_data(ui->set_ui, q, caller) != RETURN_SUCCESS) {
                res = RETURN_FAILURE;
            }
            break;
        case QFlavorAGrid:
            if (set_axisgrid_data(ui->axisgrid_ui, q, caller) != RETURN_SUCCESS) {
                res = RETURN_FAILURE;
            }
            break;
        case QFlavorAxis:
            if (set_axis_data(ui->axis_ui, q, caller) != RETURN_SUCCESS) {
                res = RETURN_FAILURE;
            }
            break;
        case QFlavorDObject:
            if (set_object_data(ui->object_ui, q, caller) != RETURN_SUCCESS) {
                res = RETURN_FAILURE;
            }
            break;
        case QFlavorAText:
            if (set_atext_data(ui->atext_ui, q, caller) != RETURN_SUCCESS) {
                res = RETURN_FAILURE;
            }
            break;
        case QFlavorRegion:
            if (set_region_data(ui->region_ui, q, caller) != RETURN_SUCCESS) {
                res = RETURN_FAILURE;
            }
            break;
        default:
            res = RETURN_FAILURE;
            break;
        }
    }
    
    snapshot_and_update(grace->project, FALSE);

    return res;
}

static int explorer_aac(void *data)
{
    ExplorerUI *ui = (ExplorerUI *) data;
    
    return explorer_apply(ui, NULL);
}

static void highlight_selected(Widget w, ListTreeItem *parent,
    int nsquarks, Quark **squarks)
{
    ListTreeItem *item;

    item = parent;
    while (item) {
        TreeItemData *ti_data = (TreeItemData *) item->user_data;
        int i;
        for (i = 0; i < nsquarks; i++) {
            if (ti_data->q == squarks[i]) {
                ListTreeHighlightItemMultiple(w, item);
            }
        }
        if (item->firstchild) {
            highlight_selected(w, item->firstchild, nsquarks, squarks);
        }
        item = item->nextsibling;
    }
}

void update_explorer(ExplorerUI *ui, int reselect)
{
    ListTreeMultiReturnStruct ret;
    int i, nsquarks;
    Quark **squarks;
    
    if (!ui) {
        return;
    }
    
    ListTreeGetHighlighted(ui->tree, &ret);
    nsquarks = ret.count;
    
    squarks = xmalloc(nsquarks*SIZEOF_VOID_P);
    for (i = 0; i < nsquarks; i++) {
        ListTreeItem *item = ret.items[i];
        TreeItemData *ti_data = (TreeItemData *) item->user_data;
        squarks[i] = ti_data->q;
    }
    
    ListTreeRefreshOff(ui->tree);
    if (ui->project) {
        ListTreeDelete(ui->tree, ui->project);
    }
    ui->project = CreateQuarkTree(ui->tree, NULL,
        grace->project, NULL, q_labeling);

    highlight_selected(ui->tree, ui->project, nsquarks, squarks);
    xfree(squarks);

    ListTreeRefreshOn(ui->tree);
    ListTreeRefresh(ui->tree);

    if (reselect) {
        ListTreeGetHighlighted(ui->tree, &ret);
        XtCallCallbacks(ui->tree, XtNhighlightCallback, (XtPointer) &ret);
    } else {
        ui->homogeneous_selection = FALSE;
        ui->all_siblings = FALSE;
    }
}

static void update_explorer_cb(Widget but, void *data)
{
    ExplorerUI *ui = (ExplorerUI *) data;
    update_explorer(ui, TRUE);
}


#define HIDE_CB           0
#define SHOW_CB           1
#define DELETE_CB         2
#define DUPLICATE_CB      3
#define BRING_TO_FRONT_CB 4
#define SEND_TO_BACK_CB   5
#define MOVE_UP_CB        6
#define MOVE_DOWN_CB      7
#define ADD_FRAME_CB      8
#define ADD_GRAPH_CB      9
#define ADD_SSD_CB       10
#define ADD_SET_CB       11
#define ADD_AXISGRID_CB  12
#define ADD_AXIS_CB      13
#define ADD_LINE_CB      14
#define ADD_BOX_CB       15
#define ADD_ARC_CB       16
#define ADD_TEXT_CB      17

static void popup_any_cb(ExplorerUI *eui, int type)
{
    ListTreeMultiReturnStruct ret;
    int count, i;
    Quark *qnew = NULL;
    
    ListTreeGetHighlighted(eui->tree, &ret);
    count = ret.count;
    
    for (i = 0; i < count; i ++) {
        ListTreeItem *item;
        TreeItemData *ti_data;
        Quark *q;
        
        switch (type) {
        case SEND_TO_BACK_CB:
        case MOVE_UP_CB:
            item = ret.items[count - i - 1];
            break;
        default:
            item = ret.items[i];
            break;
        }

        ti_data = (TreeItemData *) item->user_data;
        q = ti_data->q;
        
        switch (type) {
        case HIDE_CB:
            quark_set_active(q, FALSE);
            break;
        case SHOW_CB:
            quark_set_active(q, TRUE);
            break;
        case DELETE_CB:
            quark_free(q);
            break;
        case BRING_TO_FRONT_CB:
            quark_push(q, TRUE);
            break;
        case SEND_TO_BACK_CB:
            quark_push(q, FALSE);
            break;
        case MOVE_UP_CB:
            quark_move(q, TRUE);
            break;
        case MOVE_DOWN_CB:
            quark_move(q, FALSE);
            break;
        case DUPLICATE_CB:
            quark_copy(q);
            break;
        case ADD_FRAME_CB:
            qnew = frame_new(q);
            break;
        case ADD_GRAPH_CB:
            qnew = graph_new(q);
            break;
        case ADD_SSD_CB:
            qnew = grace_ssd_new(q);
            break;
        case ADD_SET_CB:
            qnew = grace_set_new(q);
            break;
        case ADD_AXISGRID_CB:
            qnew = axisgrid_new(q);
            break;
        case ADD_AXIS_CB:
            qnew = axis_new(q);
            break;
        case ADD_LINE_CB:
            qnew = object_new_complete(q, DO_LINE);
            break;
        case ADD_BOX_CB:
            qnew = object_new_complete(q, DO_BOX);
            break;
        case ADD_ARC_CB:
            qnew = object_new_complete(q, DO_ARC);
            break;
        case ADD_TEXT_CB:
            qnew = atext_new(q);
            break;
        }
    }
    
    snapshot_and_update(grace->project, TRUE);
    
    if (qnew) {
        SelectQuarkTreeItem(eui->tree, eui->project, qnew);
    }
}

static void hide_cb(Widget but, void *udata)
{
    popup_any_cb((ExplorerUI *) udata, HIDE_CB);
}

static void show_cb(Widget but, void *udata)
{
    popup_any_cb((ExplorerUI *) udata, SHOW_CB);
}

static void delete_cb(Widget but, void *udata)
{
    if (yesno("Really delete selected item(s)?", NULL, NULL, NULL)) {
        popup_any_cb((ExplorerUI *) udata, DELETE_CB);
    }
}

static void duplicate_cb(Widget but, void *udata)
{
    popup_any_cb((ExplorerUI *) udata, DUPLICATE_CB);
}

static void bring_to_front_cb(Widget but, void *udata)
{
    popup_any_cb((ExplorerUI *) udata, BRING_TO_FRONT_CB);
}

static void send_to_back_cb(Widget but, void *udata)
{
    popup_any_cb((ExplorerUI *) udata, SEND_TO_BACK_CB);
}

static void move_up_cb(Widget but, void *udata)
{
    popup_any_cb((ExplorerUI *) udata, MOVE_UP_CB);
}

static void move_down_cb(Widget but, void *udata)
{
    popup_any_cb((ExplorerUI *) udata, MOVE_DOWN_CB);
}

static void add_frame_cb(Widget but, void *udata)
{
    popup_any_cb((ExplorerUI *) udata, ADD_FRAME_CB);
}

static void add_graph_cb(Widget but, void *udata)
{
    popup_any_cb((ExplorerUI *) udata, ADD_GRAPH_CB);
}

static void add_ssd_cb(Widget but, void *udata)
{
    popup_any_cb((ExplorerUI *) udata, ADD_SSD_CB);
}

static void add_set_cb(Widget but, void *udata)
{
    popup_any_cb((ExplorerUI *) udata, ADD_SET_CB);
}

static void add_axisgrid_cb(Widget but, void *udata)
{
    popup_any_cb((ExplorerUI *) udata, ADD_AXISGRID_CB);
}

static void add_axis_cb(Widget but, void *udata)
{
    popup_any_cb((ExplorerUI *) udata, ADD_AXIS_CB);
}

static void add_object_cb(Widget but, void *udata)
{
    ExplorerUI *eui = (ExplorerUI *) udata;
    if (but == eui->insert_line_bt) {
        popup_any_cb(eui, ADD_LINE_CB);
    } else
    if (but == eui->insert_box_bt) {
        popup_any_cb(eui, ADD_BOX_CB);
    } else
    if (but == eui->insert_arc_bt) {
        popup_any_cb(eui, ADD_ARC_CB);
    } else
    if (but == eui->insert_text_bt) {
        popup_any_cb(eui, ADD_TEXT_CB);
    }
}

void raise_explorer(GUI *gui, Quark *q)
{
    Grace *grace = gui->P;

    set_wait_cursor();
    
    if (!gui->eui) {
        ExplorerUI *eui;
        Widget menubar, menupane, panel, form, fr;
        X11Stuff *xstuff = grace->gui->xstuff;
        Pixel bg;
        XpmColorSymbol transparent;
        XpmAttributes attrib;

        eui = xmalloc(sizeof(ExplorerUI));
        gui->eui = eui;

        /* Create pixmaps */
        XtVaGetValues(app_shell, XtNbackground, &bg, NULL);
        transparent.name  = NULL;
        transparent.value = "None";
        transparent.pixel = bg;
        attrib.colorsymbols = &transparent;
        attrib.valuemask    = XpmColorSymbols;
        attrib.numsymbols   = 1;
        XpmCreatePixmapFromData(xstuff->disp, xstuff->root,
            active_xpm, &eui->a_icon, NULL, &attrib);
        XpmCreatePixmapFromData(xstuff->disp, xstuff->root,
            hidden_xpm, &eui->h_icon, NULL, &attrib);
        
        
        eui->top = CreateDialogForm(app_shell, "Explorer");
        menubar = CreateMenuBar(eui->top);
        ManageChild(menubar);
        AddDialogFormChild(eui->top, menubar);

        menupane = CreateMenu(menubar, "File", 'F', FALSE);
        CreateMenuButton(menupane,
            "Close", 'C', destroy_dialog_cb, GetParent(eui->top));

        menupane = CreateMenu(menubar, "Edit", 'E', FALSE);
        eui->edit_undo_bt = CreateMenuButton(menupane, "Undo", 'U',
            undo_cb, grace);
        eui->edit_redo_bt = CreateMenuButton(menupane, "Redo", 'R',
            redo_cb, grace);

        menupane = CreateMenu(menubar, "Insert", 'I', FALSE);
        eui->insert_frame_bt = CreateMenuButton(menupane,
            "Frame", '\0', add_frame_cb, eui);
        SetSensitive(eui->insert_frame_bt,  FALSE);
        eui->insert_graph_bt = CreateMenuButton(menupane,
            "Graph", '\0', add_graph_cb, eui);
        SetSensitive(eui->insert_graph_bt,  FALSE);
        eui->insert_ssd_bt = CreateMenuButton(menupane,
            "SSData", '\0', add_ssd_cb, eui);
        SetSensitive(eui->insert_ssd_bt,    FALSE);
        eui->insert_set_bt = CreateMenuButton(menupane,
            "Set", '\0', add_set_cb, eui);
        SetSensitive(eui->insert_set_bt,    FALSE);
        eui->insert_axisgrid_bt = CreateMenuButton(menupane,
            "Axis grid", '\0', add_axisgrid_cb, eui);
        SetSensitive(eui->insert_axisgrid_bt,   FALSE);
        eui->insert_axis_bt = CreateMenuButton(menupane,
            "Axis", '\0', add_axis_cb, eui);
        SetSensitive(eui->insert_axis_bt,   FALSE);
        eui->insert_object_pane = CreateMenu(menupane, "Annotating objects", 'o', FALSE);
        SetSensitive(eui->insert_object_pane, FALSE);
        eui->insert_text_bt = CreateMenuButton(eui->insert_object_pane,
            "Text", '\0', add_object_cb, eui);
        eui->insert_line_bt = CreateMenuButton(eui->insert_object_pane,
            "Line", '\0', add_object_cb, eui);
        eui->insert_box_bt = CreateMenuButton(eui->insert_object_pane,
            "Box", '\0', add_object_cb, eui);
        eui->insert_arc_bt = CreateMenuButton(eui->insert_object_pane,
            "Arc", '\0', add_object_cb, eui);

        menupane = CreateMenu(menubar, "Options", 'O', FALSE);
        eui->instantupdate = CreateMenuToggle(menupane, "Instantaneous update",
                            'u', NULL, NULL);
        SetToggleButtonState(eui->instantupdate, gui->instant_update);

        menupane = CreateMenu(menubar, "Help", 'H', TRUE);
        CreateMenuHelpButton(menupane, "On explorer", 'e',
            eui->top, "doc/UsersGuide.html#explorer");

#if USE_PANEDW
        panel = XtVaCreateWidget("panedWindow",
            xmPanedWindowWidgetClass, eui->top,
            XmNorientation, XmHORIZONTAL,
            NULL);
#else
        panel = CreateGrid(eui->top, 2, 1);
#endif

        form = XmCreateForm(panel, "form", NULL, 0);

        eui->tree = XmCreateScrolledListTree(form, "tree", NULL, 0);
        XtAddCallback(eui->tree, XtNhighlightCallback, highlight_cb, eui);
        XtAddCallback(eui->tree, XtNmenuCallback, menu_cb, eui);
        XtAddCallback(eui->tree, XtNdestroyItemCallback, destroy_cb, eui);
        XtAddCallback(eui->tree, XtNdropCallback, drop_cb, eui);

        fr = CreateFrame(form, NULL);
        eui->idstr = CreateTextInput(fr, "ID string:");
        AddTextInputCB(eui->idstr, text_explorer_cb, eui);

        XtVaSetValues(GetParent(eui->tree),
            XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNtopAttachment, XmATTACH_FORM,
            XmNbottomAttachment, XmATTACH_WIDGET,
            XmNbottomWidget, fr,
            NULL);
        XtVaSetValues(fr,
            XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNtopAttachment, XmATTACH_NONE,
            XmNbottomAttachment, XmATTACH_FORM,
            NULL);
        
	ManageChild(form);
        
#if !USE_PANEDW
        PlaceGridChild(panel, form, 0, 0);
#endif

        eui->scrolled_window = XtVaCreateManagedWidget("scrolled_window",
            xmScrolledWindowWidgetClass, panel,
            XmNscrollingPolicy, XmAUTOMATIC,
	    NULL);

#if USE_PANEDW
	ManageChild(panel);
        XtVaSetValues(form,
            XmNpaneMinimum, 150,
            XmNwidth, 250,
            NULL);
#else
        PlaceGridChild(panel, eui->scrolled_window, 1, 0);
#endif

#ifdef HAVE_LESSTIF
# if !defined(SF_BUG_993209_FIXED) && !defined(SF_BUG_993209_NOT_FIXED)
#  error "You should check whether SF bug #993209 is fixed in your version of LessTif."
#  error "Then define either SF_BUG_993209_FIXED or SF_BUG_993209_NOT_FIXED, accordingly."
#  error "See http://sourceforge.net/tracker/index.php?func=detail&aid=993209&group_id=8596&atid=108596."
# endif
# ifdef SF_BUG_993209_NOT_FIXED
        /* A dirty workaround */
        eui->scrolled_window = CreateVContainer(eui->scrolled_window);
# endif
#endif

        eui->project_ui = create_project_ui(eui);
        UnmanageChild(eui->project_ui->top);

	eui->ssd_ui = create_ssd_ui(eui);
        UnmanageChild(eui->ssd_ui->top);

	eui->frame_ui = create_frame_ui(eui);
        UnmanageChild(eui->frame_ui->top);

	eui->graph_ui = create_graph_ui(eui);
        UnmanageChild(eui->graph_ui->top);

	eui->set_ui = create_set_ui(eui);
        UnmanageChild(eui->set_ui->top);

	eui->axisgrid_ui = create_axisgrid_ui(eui);
        UnmanageChild(eui->axisgrid_ui->top);

	eui->axis_ui = create_axis_ui(eui);
        UnmanageChild(eui->axis_ui->top);

	eui->object_ui = create_object_ui(eui);
        UnmanageChild(eui->object_ui->top);

	eui->atext_ui = create_atext_ui(eui);
        UnmanageChild(eui->atext_ui->top);

	eui->region_ui = create_region_ui(eui);
        UnmanageChild(eui->region_ui->top);

        eui->aacbuts = CreateAACDialog(eui->top, panel, explorer_aac, eui);

        eui->project = CreateQuarkTree(eui->tree, NULL,
            grace->project, NULL, q_labeling);
        
        ManageChild(eui->tree);
        ListTreeRefreshOn(eui->tree);
        ListTreeRefresh(eui->tree);

        /* Menu popup */
        eui->popup = XmCreatePopupMenu(eui->tree, "explorerPopupMenu", NULL, 0);
        eui->popup_hide_bt = CreateMenuButton(eui->popup,
            "Hide", '\0', hide_cb, eui);
        eui->popup_show_bt = CreateMenuButton(eui->popup,
            "Show", '\0', show_cb, eui);

        CreateMenuSeparator(eui->popup);

        eui->popup_delete_bt = CreateMenuButton(eui->popup,
            "Delete", '\0', delete_cb, eui);
        eui->popup_duplicate_bt = CreateMenuButton(eui->popup,
            "Duplicate", '\0', duplicate_cb, eui);

        CreateMenuSeparator(eui->popup);

        eui->popup_bring_to_front_bt = CreateMenuButton(eui->popup,
            "Bring to front", '\0', bring_to_front_cb, eui);
        eui->popup_move_up_bt = CreateMenuButton(eui->popup,
            "Move up", '\0', move_up_cb, eui);
        eui->popup_move_down_bt = CreateMenuButton(eui->popup,
            "Move down", '\0', move_down_cb, eui);
        eui->popup_send_to_back_bt = CreateMenuButton(eui->popup,
            "Send to back", '\0', send_to_back_cb, eui);

        CreateMenuSeparator(eui->popup);

        CreateMenuButton(eui->popup,
            "Update tree", '\0', update_explorer_cb, eui);
    }

    RaiseWindow(GetParent(gui->eui->top));
    
    if (q) {
        SelectQuarkTreeItem(gui->eui->tree, gui->eui->project, q);
    }

    update_undo_buttons(grace->project);

    unset_wait_cursor();
}

void define_explorer_popup(Widget but, void *data)
{
    GUI *gui = (GUI *) data;
    raise_explorer(gui, NULL);
}


void oc_explorer_cb(OptionStructure *opt, int a, void *data)
{
    ExplorerUI *eui = (ExplorerUI *) data;
    explorer_apply(eui, opt);
}
void tb_explorer_cb(Widget but, int a, void *data)
{
    ExplorerUI *eui = (ExplorerUI *) data;
    explorer_apply(eui, but);
}
void scale_explorer_cb(Widget scale, int a, void *data)
{
    ExplorerUI *eui = (ExplorerUI *) data;
    explorer_apply(eui, scale);
}
void sp_explorer_cb(SpinStructure *spinp, double a, void *data)
{
    ExplorerUI *eui = (ExplorerUI *) data;
    explorer_apply(eui, spinp);
}
void text_explorer_cb(TextStructure *cst, char *s, void *data)
{
    ExplorerUI *eui = (ExplorerUI *) data;
    explorer_apply(eui, cst);
}
void titem_explorer_cb(Widget w, char *s, void *data)
{
    ExplorerUI *eui = (ExplorerUI *) data;
    explorer_apply(eui, w);
}
void pen_explorer_cb(Widget but, const Pen *pen, void *data)
{
    ExplorerUI *eui = (ExplorerUI *) data;
    explorer_apply(eui, but);
}
