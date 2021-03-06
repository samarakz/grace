/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 2000-2005 Grace Development Team
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
 * Object drawing
 */

#include <config.h>

#include <stdlib.h>

#include "grace/plotP.h"

void draw_object(Canvas *canvas, Quark *q)
{
    VPoint anchor;
    DObject *o = object_get_data(q);

    if (o == NULL) {
        return;
    }
    
    if (Apoint2Vpoint(q, &o->ap, &anchor) != RETURN_SUCCESS) {
        return;
    }
    anchor.x += o->offset.x;
    anchor.y += o->offset.y;
    
    setclipping(canvas, FALSE);
    
    activate_bbox(canvas, BBOX_TYPE_TEMP, TRUE);
    reset_bbox(canvas, BBOX_TYPE_TEMP);

    switch (o->type) {
    case DO_LINE:
        {
            DOLineData *l = (DOLineData *) o->odata;

            VPoint vp1;
            double x, y, co, si;

            x = l->vector.x;
            y = l->vector.y;

            co = cos(M_PI/180.0*o->angle);
            si = sin(M_PI/180.0*o->angle);

            vp1.x = anchor.x + x*co - y*si;
            vp1.y = anchor.y + x*si + y*co;

            setline(canvas, &o->line);
            DrawLine(canvas, &anchor, &vp1);

            switch (l->arrow_end) {
            case ARROW_AT_NONE:
                break;
            case ARROW_AT_BEGINNING:
                draw_arrowhead(canvas, &vp1, &anchor, &l->arrow,
                    &o->line.pen, &o->fillpen);
                break;
            case ARROW_AT_END:
                draw_arrowhead(canvas, &anchor, &vp1, &l->arrow,
                    &o->line.pen, &o->fillpen);
                break;
            case ARROW_AT_BOTH:
                draw_arrowhead(canvas, &vp1, &anchor, &l->arrow,
                    &o->line.pen, &o->fillpen);
                draw_arrowhead(canvas, &anchor, &vp1, &l->arrow,
                    &o->line.pen, &o->fillpen);
                break;
            }
        }
        break;
    case DO_BOX:
        {
            DOBoxData *b = (DOBoxData *) o->odata;
            if (o->angle == 0.0) {
                VPoint vp1, vp2;

                vp1.x = anchor.x - b->width/2;
                vp2.x = anchor.x + b->width/2;
                vp1.y = anchor.y - b->height/2;
                vp2.y = anchor.y + b->height/2;

                setpen(canvas, &o->fillpen);
                DrawFilledRect(canvas, &vp1, &vp2);

                setline(canvas, &o->line);
                DrawRect(canvas, &vp1, &vp2);
            } else {
                VPoint vps[4];
                double x, y, co, si;

                x = b->width/2;
                y = b->height/2;

                co = cos(M_PI/180.0*o->angle);
                si = sin(M_PI/180.0*o->angle);
                
                vps[0].x = anchor.x + x*co - y*si;
                vps[0].y = anchor.y + x*si + y*co;
                vps[1].x = anchor.x - x*co - y*si;
                vps[1].y = anchor.y - x*si + y*co;
                vps[2].x = anchor.x - x*co + y*si;
                vps[2].y = anchor.y - x*si - y*co;
                vps[3].x = anchor.x + x*co + y*si;
                vps[3].y = anchor.y + x*si - y*co;

                setpen(canvas, &o->fillpen);
                DrawPolygon(canvas, vps, 4);

                setline(canvas, &o->line);
                DrawPolyline(canvas, vps, 4, POLYLINE_CLOSED);
            }
        }
        break;
    case DO_ARC:
        {
            VPoint vp1, vp2;
            DOArcData *e = (DOArcData *) o->odata;
            
            vp1.x = anchor.x - e->width/2;
            vp2.x = anchor.x + e->width/2;
            vp1.y = anchor.y - e->height/2;
            vp2.y = anchor.y + e->height/2;

            setpen(canvas, &o->fillpen);
            /* FIXME: implement true ellipse rotation! */
            DrawFilledArc(canvas, &vp1, &vp2, e->angle1 + o->angle, e->angle2,
                e->closure_type);

            setline(canvas, &o->line);
            DrawArc(canvas, &vp1, &vp2, e->angle1 + o->angle, e->angle2,
                e->closure_type, e->draw_closure);
        }
        break;
    case DO_NONE:
        break;
    }

    get_bbox(canvas, BBOX_TYPE_TEMP, &o->bb);
}



void draw_atext(Canvas *canvas, Quark *q)
{
    VPoint anchor;
    AText *at = atext_get_data(q);
    TextFrame *tf;
    view bbox;
    
    if (!at || string_is_empty(at->s)) {
        return;
    }

    if (Apoint2Vpoint(q, &at->ap, &anchor) != RETURN_SUCCESS) {
        return;
    }
    anchor.x += at->offset.x;
    anchor.y += at->offset.y;
    
    setclipping(canvas, FALSE);
    
    tf = &at->frame;

    drawtext(canvas, &anchor, &at->text_props, tf, at->s, &bbox);
    
    at->bb = bbox;
    
    if ((tf->line.pen.pattern || tf->fillpen.pattern) &&
        at->arrow_flag && tf->line.style) {
        VPoint vp1, vp2;

        vp2.x = anchor.x - at->offset.x;
        vp2.y = anchor.y - at->offset.y;

        switch (tf->decor) {
        case FRAME_DECOR_LINE:
            if (vp2.x < bbox.xv1) {
                vp1.x = bbox.xv1;
            } else
            if (vp2.x > bbox.xv2) {
                vp1.x = bbox.xv2;
            } else {
                vp1.x = vp2.x;
            }
            vp1.y = bbox.yv1;
            break;
        case FRAME_DECOR_OVAL:
            if (vp2.x < bbox.xv1) {
                vp1.x = bbox.xv1;
            } else
            if (vp2.x > bbox.xv2) {
                vp1.x = bbox.xv2;
            } else {
                vp1.x = (bbox.xv1 + bbox.xv2)/2;
            }
            if (vp2.y < bbox.yv1) {
                vp1.y = bbox.yv1;
            } else
            if (vp2.y > bbox.yv2) {
                vp1.y = bbox.yv2;
            } else {
                vp1.y = (bbox.yv1 + bbox.yv2)/2;
            }
            break;
        default:
            if (vp2.x < bbox.xv1) {
                vp1.x = bbox.xv1;
            } else
            if (vp2.x > bbox.xv2) {
                vp1.x = bbox.xv2;
            } else {
                vp1.x = vp2.x;
            }
            if (vp2.y < bbox.yv1) {
                vp1.y = bbox.yv1;
            } else
            if (vp2.y > bbox.yv2) {
                vp1.y = bbox.yv2;
            } else {
                vp1.y = vp2.y;
            }
        }

        setline(canvas, &tf->line);
        DrawLine(canvas, &vp1, &vp2);
        draw_arrowhead(canvas, &vp1, &vp2, &at->arrow,
            &tf->line.pen, &tf->line.pen);
    }
}
