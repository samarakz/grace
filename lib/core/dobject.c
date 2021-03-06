/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1996-2003 Grace Development Team
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
 * DObject stuff
 *
 */

#include <string.h>

#define ADVANCED_MEMORY_HANDLERS
#include "grace/coreP.h"

static int object_odata_size(OType type)
{
    int size;

    switch (type) {
    case DO_LINE:
        size = sizeof(DOLineData);
        break;
    case DO_BOX:
        size = sizeof(DOBoxData);
        break;
    case DO_ARC:
        size = sizeof(DOArcData);
        break;
    default:
        size = 0;
    }
    
    return size;
}

DObject *object_data_new(AMem *amem)
{
    DObject *o;

    o = amem_malloc(amem, sizeof(DObject));
    memset(o, 0, sizeof(DObject));
    if (o) {
        o->type = DO_NONE;

        o->ap.x = 0.0;
        o->ap.y = 0.0;
    
        o->offset.x = 0.0;
        o->offset.y = 0.0;
        o->angle = 0.0;
    
        o->line.pen.color = 1;
        o->line.pen.pattern = 1;
        o->line.style = 1;
        o->line.width = 1.0;
        o->fillpen.color = 1;
        o->fillpen.pattern = 0;
        
        o->odata = NULL;
        
        /* o->bb = ; */
    }
    
    return o;
}

static int object_set_data(DObject *o, void *odata)
{
    int size = object_odata_size(o->type);
    
    if (!size) {
        return RETURN_FAILURE;
    }

    memcpy(o->odata, (void *) odata, size);
    
    return RETURN_SUCCESS;
}

DObject *object_data_copy(AMem *amem, DObject *osrc)
{
    DObject *odest;
    void *odata;
    
    if (!osrc) {
        return NULL;
    }
    
    odest = object_data_new_complete(amem, osrc->type);
    if (!odest) {
        return NULL;
    }
    
    /* Save odata pointer before memcpy overwrites it */
    odata = odest->odata;
    
    memcpy(odest, osrc, sizeof(DObject));
    
    /* Restore odata */
    odest->odata = odata;
    
    if (object_set_data(odest, osrc->odata) != RETURN_SUCCESS) {
        object_data_free(amem, odest);
        return NULL;
    }
    
    return odest;
}

void object_data_free(AMem *amem, DObject *o)
{
    if (o) {
        amem_free(amem, o->odata);
        amem_free(amem, o);
    }
}

int object_qf_register(QuarkFactory *qfactory)
{
    QuarkFlavor qf = {
        QFlavorDObject,
        (Quark_data_new) object_data_new,
        (Quark_data_free) object_data_free,
        (Quark_data_copy) object_data_copy
    };

    return quark_flavor_add(qfactory, &qf);
}


void set_default_arrow(Arrow *arrowp)
{
    arrowp->type   = ARROW_TYPE_LINE;
    arrowp->length = 1.0;
    arrowp->dL_ff  = 1.0;
    arrowp->lL_ff  = 0.0;
}

void *object_odata_new(AMem *amem, OType type)
{
    void *odata;
    odata = amem_malloc(amem, object_odata_size(type));
    if (odata == NULL) {
        return NULL;
    }
    memset(odata, 0, object_odata_size(type));
    switch (type) {
    case DO_LINE:
        {
            DOLineData *l = (DOLineData *) odata;
            l->vector.x  = l->vector.y  = 0.0;
            l->arrow_end = 0; 
            set_default_arrow(&l->arrow);
        }
        break;
    case DO_BOX:
        {
            DOBoxData *b = (DOBoxData *) odata;
            b->width  = 0.0;
            b->height = 0.0;
        }
        break;
    case DO_ARC:
        {
            DOArcData *e = (DOArcData *) odata;
            e->width  = 0.0;
            e->height = 0.0;
            e->angle1 =   0.0;
            e->angle2 = 360.0;
            e->closure_type = ARCCLOSURE_CHORD;
        }
        break;
    case DO_NONE:
        break;
    }
    
    return odata;
}

Quark *object_new(Quark *gr)
{
    Quark *o; 
    o = quark_new(gr, QFlavorDObject);
    return o;
}

DObject *object_data_new_complete(AMem *amem, OType type)
{
    DObject *o;
    
    o = object_data_new(amem);
    if (o) {
        o->type   = type;
        o->odata  = object_odata_new(amem, type);
        if (o->odata == NULL) {
            amem_free(amem, o);
            return NULL;
        }
    }
    
    return o;
}

Quark *object_new_complete(Quark *gr, OType type)
{
    Quark *q;
    DObject *o;
    
    q = object_new(gr);
    o = object_get_data(q);
    if (o) {
        o->type   = type;
        o->odata  = object_odata_new(q->amem, type);
        if (o->odata == NULL) {
            amem_free(q->amem, o);
            return NULL;
        }
    }
    
    return q;
}

DObject *object_get_data(const Quark *q)
{
    DObject *o;
    
    if (q && q->fid == QFlavorDObject) {
        o = (DObject *) q->data;
    } else {
        o = NULL;
    }
    
    return o;
}

int object_get_bb(DObject *o, view *bb)
{
    if (o) {
        *bb = o->bb;
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}


int object_set_angle(Quark *q, double angle)
{
    DObject *o = object_get_data(q);
    if (o) {
        
        o->angle = angle;
        
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int object_set_offset(Quark *q, const VPoint *offset)
{
    DObject *o = object_get_data(q);
    if (o) {
        
        o->offset = *offset;
        
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int object_set_line(Quark *q, const Line *line)
{
    DObject *o = object_get_data(q);
    if (o) {
        
        o->line = *line;
        
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int object_set_fillpen(Quark *q, const Pen *pen)
{
    DObject *o = object_get_data(q);
    if (o) {
        
        o->fillpen = *pen;
        
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int object_set_location(Quark *q, const APoint *ap)
{
    DObject *o = object_get_data(q);
    if (o) {
        
        o->ap      = *ap;
        
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int object_shift(Quark *q, const VVector *vshift)
{
    DObject *o = object_get_data(q);
    if (o) {
        
        o->offset.x += vshift->x;
        o->offset.y += vshift->y;
        
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}
