/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 2004 Grace Development Team
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
 * Annotating texts
 *
 */

#include <string.h>

#define ADVANCED_MEMORY_HANDLERS
#include "grace/coreP.h"

void set_default_textprops(TextProps *pstr, const defaults *grdefs)
{
    pstr->color    = grdefs->line.pen.color;
    pstr->angle    = 0.0;
    pstr->font     = grdefs->font;
    pstr->just     = JUST_LEFT|JUST_BLINE;
    pstr->charsize = 1.0;
}

AText *atext_data_new(AMem *amem)
{
    AText *at;
    at = amem_malloc(amem, sizeof(AText));
    if (at) {
        memset(at, 0, sizeof(AText));
        set_default_arrow(&at->arrow);
        at->frame.offset = 0.005;
    }
    
    return at;
}

void atext_data_free(AMem *amem, AText *at)
{
    if (at) {
        amem_free(amem, at->s);
        amem_free(amem, at);
    }
}

AText *atext_data_copy(AMem *amem, AText *at)
{
    AText *retval;
    if (at == NULL) {
        return NULL;
    } else {
        retval = atext_data_new(amem);
        if (retval) {
            memcpy(retval, at, sizeof(AText));
            retval->s = amem_strdup(amem, at->s);
        }
    }
    
    return retval;
}

Quark *atext_new(Quark *q)
{
    Quark *qnew = quark_new(q, QFlavorAText);
    AText *at = atext_get_data(qnew);

    Project *pr = project_get_data(get_parent_project(qnew));

    if (at && pr) {
        defaults grdefs = pr->grdefaults;
        set_default_textprops(&at->text_props, &grdefs);
        at->offset.x = at->offset.y = 0.0;

        at->frame.line = grdefs.line;
    }
    return qnew;
}

AText *atext_get_data(const Quark *q)
{
    if (q && q->fid == QFlavorAText) {
        return (AText *) q->data;
    } else {
        return NULL;
    }
}

int atext_qf_register(QuarkFactory *qfactory)
{
    QuarkFlavor qf = {
        QFlavorAText,
        (Quark_data_new) atext_data_new,
        (Quark_data_free) atext_data_free,
        (Quark_data_copy) atext_data_copy
    };

    return quark_flavor_add(qfactory, &qf);
}


int atext_set_string(Quark *q, const char *s)
{
    AText *at = atext_get_data(q);
    if (at) {
        
        at->s = amem_strcpy(q->amem, at->s, s);
        
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int atext_set_ap(Quark *q, const APoint *ap)
{
    AText *at = atext_get_data(q);
    if (at) {
        
        at->ap = *ap;
        
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int atext_set_offset(Quark *q, const VPoint *offset)
{
    AText *at = atext_get_data(q);
    if (at) {
        
        at->offset = *offset;
        
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int atext_set_tprops(Quark *q, const TextProps *tprops)
{
    AText *at = atext_get_data(q);
    if (at) {
        
        at->text_props = *tprops;
        
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int atext_set_font(Quark *q, int font)
{
    AText *at = atext_get_data(q);
    if (at) {
        
        at->text_props.font = font;
        
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int atext_set_char_size(Quark *q, double size)
{
    AText *at = atext_get_data(q);
    if (at) {
        
        at->text_props.charsize = size;
        
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int atext_set_color(Quark *q, int color)
{
    AText *at = atext_get_data(q);
    if (at) {
        
        at->text_props.color = color;
        
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int atext_set_just(Quark *q, int just)
{
    AText *at = atext_get_data(q);
    if (at) {
        
        at->text_props.just = just;
        
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int atext_set_angle(Quark *q, double angle)
{
    AText *at = atext_get_data(q);
    if (at) {
        
        at->text_props.angle = angle;
        
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int atext_set_pointer(Quark *q, int flag)
{
    AText *at = atext_get_data(q);
    if (at) {
        
        at->arrow_flag = flag;
        
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int atext_shift(Quark *q, const VVector *vshift)
{
    AText *at = atext_get_data(q);
    if (at) {
        
        at->offset.x += vshift->x;
        at->offset.y += vshift->y;
        
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int atext_at_shift(Quark *q, const VVector *vshift)
{
    AText *at = atext_get_data(q);
    if (at) {
        VPoint vp;
        APoint ap;
        
        if (Apoint2Vpoint(q, &at->ap, &vp) != RETURN_SUCCESS) {
            return RETURN_FAILURE;
        }
        
        vp.x += vshift->x;
        vp.y += vshift->y;
        
        if (Vpoint2Apoint(q, &vp, &ap) != RETURN_SUCCESS) {
            return RETURN_FAILURE;
        }
        
        return atext_set_ap(q, &ap);
    } else {
        return RETURN_FAILURE;
    }
}
