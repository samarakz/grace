/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 2001-2003 Grace Development Team
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

#include <string.h>
/* for time_t */
#ifdef TIME_WITH_SYS_TIME
#  include <sys/time.h>
#  include <time.h>
#else
#  ifdef HAVE_SYS_TIME_H
#    include <sys/time.h>
#  else
#    include <time.h>
#  endif
#endif

#define ADVANCED_MEMORY_HANDLERS
#include "grace/coreP.h"

static defaults d_d =
{{{1, 1}, 1, 1.0}, {0, 0}, 0, 1.0};

Project *project_data_new(AMem *amem)
{
    Project *pr;
    
    pr = amem_malloc(amem, sizeof(Project));
    if (!pr) {
        return NULL;
    }
    memset(pr, 0, sizeof(Project));
    
    pr->description = NULL;
    
    pr->sformat = amem_strdup(amem, "%.8g");

    pr->timestamp = amem_strdup(amem, "");

    /* FIXME: #defines */
    pr->page_wpp = 792;
    pr->page_hpp = 612;

    pr->ref_date = 0.0;
    pr->wrap_year = 1950;
    pr->two_digits_years = FALSE;
    
    pr->bgcolor  = 0;
    pr->bgfill   = TRUE;

    pr->fscale   = 0.025;
    pr->lscale   = 0.0015;

    pr->grdefaults = d_d;
    
    return pr;
}

void project_data_free(AMem *amem, Project *pr)
{
    unsigned int i;
    
    if (!pr) {
        return;
    }
    
    amem_free(amem, pr->description);
    
    amem_free(amem, pr->sformat);
    amem_free(amem, pr->docname);
    
    amem_free(amem, pr->timestamp);
    
    for (i = 0; i < pr->nfonts; i++) {
        Fontdef *f = &pr->fontmap[i];
        amem_free(amem, f->fontname);
        amem_free(amem, f->fallback);
    }
    amem_free(amem, pr->fontmap);
    
    for (i = 0; i < pr->ncolors; i++) {
        Colordef *c = &pr->colormap[i];
        amem_free(amem, c->cname);
    }
    amem_free(amem, pr->colormap);
    
    amem_free(amem, pr);
}

Project *project_get_data(const Quark *q)
{
    if (q && q->fid == QFlavorProject) {
        return (Project *) q->data;
    } else {
        return NULL;
    }
}

int project_qf_register(QuarkFactory *qfactory)
{
    QuarkFlavor qf = {
        QFlavorProject,
        (Quark_data_new) project_data_new,
        (Quark_data_free) project_data_free,
        NULL
    };

    return quark_flavor_add(qfactory, &qf);
}

int project_get_version_id(const Quark *q)
{
    Project *pr = project_get_data(q);
    if (pr) {
        return pr->version_id;
    } else {
        return 0;
    }
}

int project_set_version_id(Quark *q, int version_id)
{
    Project *pr = project_get_data(q);
    if (pr) {
        pr->version_id = version_id;
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

char *project_get_description(const Quark *q)
{
    Project *pr = project_get_data(q);
    if (pr) {
        return pr->description;
    } else {
        return NULL;
    }
}

int project_set_description(Quark *q, char *descr)
{
    Project *pr = project_get_data(q);
    if (pr) {
        pr->description = amem_strcpy(q->amem, pr->description, descr);
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

char *project_get_sformat(const Quark *q)
{
    Project *pr = project_get_data(q);
    if (pr) {
        return pr->sformat;
    } else {
        return NULL;
    }
}

int project_set_sformat(Quark *q, const char *s)
{
    Project *pr = project_get_data(q);
    if (pr) {
        pr->sformat = amem_strcpy(q->amem, pr->sformat, s);;
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

char *project_get_docname(const Quark *q)
{
    Project *pr = project_get_data(q);
    if (pr) {
        return pr->docname;
    } else {
        return NULL;
    }
}

int project_set_docname(Quark *q, char *s)
{
    Project *pr = project_get_data(q);
    if (pr) {
        pr->docname = amem_strcpy(q->amem, pr->docname, s);
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int project_get_page_dimensions(const Quark *q, int *wpp, int *hpp)
{
    Project *pr = project_get_data(q);
    if (pr) {
        *wpp = pr->page_wpp;
	*hpp = pr->page_hpp;
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int project_set_page_dimensions(Quark *q, int wpp, int hpp)
{
    Project *pr = project_get_data(q);
    if (pr) {
        pr->page_wpp = wpp;
	pr->page_hpp = hpp;
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int project_set_fontsize_scale(Quark *q, double fscale)
{
    Project *pr = project_get_data(q);
    if (pr) {
        pr->fscale = fscale;
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int project_set_linewidth_scale(Quark *q, double lscale)
{
    Project *pr = project_get_data(q);
    if (pr) {
        pr->lscale = lscale;
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int project_set_ref_date(Quark *q, double ref)
{
    Project *pr = project_get_data(q);
    
    if (pr) {
        pr->ref_date = ref;
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}

int project_allow_two_digits_years(Quark *q, int flag)
{
    Project *pr = project_get_data(q);
    
    if (pr) {
        pr->two_digits_years = flag;
        quark_dirtystate_set(q, TRUE);
        return RETURN_SUCCESS;
    } else {
        return RETURN_FAILURE;
    }
}


int project_update_timestamp(Quark *q, time_t *t)
{
    Project *pr = project_get_data(q);
    struct tm tm;
    time_t time_value;
    char *str;

    if (!pr) {
        return RETURN_FAILURE;
    }
    
    if (!t) {
        (void) time(&time_value);
    } else {
        time_value = *t;
    }
    tm = *localtime(&time_value);
    str = asctime(&tm);
    if (str[strlen(str) - 1] == '\n') {
        str[strlen(str) - 1]= '\0';
    }
    pr->timestamp = amem_strcpy(q->amem, pr->timestamp, str);
    
    return RETURN_SUCCESS;
}

char *project_get_timestamp(Quark *q)
{
    Project *pr = project_get_data(q);
    if (pr) {
        return pr->timestamp;
    } else {
        return NULL;
    }
}


int project_add_font(Quark *project, const Fontdef *f)
{
    Project *pr = project_get_data(project);
    Fontdef *fnew;
    pr->fontmap = amem_realloc(project->amem,
        pr->fontmap, (pr->nfonts + 1)*sizeof(Fontdef));
    fnew = &pr->fontmap[pr->nfonts];
    fnew->id = f->id;
    fnew->fontname = amem_strdup(project->amem, f->fontname);
    fnew->fallback = amem_strdup(project->amem, f->fallback);
    pr->nfonts++;
    
    return RETURN_SUCCESS;
}

int project_add_color(Quark *project, const Colordef *c)
{
    Project *pr = project_get_data(project);
    Colordef *cbuf;
    unsigned int i;
    
    /* Check whether a color with this id exists */
    for (i = 0; i < pr->ncolors; i++) {
        cbuf = &pr->colormap[i];
        if (cbuf->id == c->id) {
            cbuf->rgb = c->rgb;
            cbuf->cname = amem_strcpy(project->amem, cbuf->cname, c->cname);
            
            return RETURN_SUCCESS;
        }
    }
    
    /* If id not found */
    pr->colormap = amem_realloc(project->amem,
        pr->colormap, (pr->ncolors + 1)*sizeof(Colordef));
    cbuf = &pr->colormap[pr->ncolors];
    cbuf->id = c->id;
    cbuf->rgb = c->rgb;
    cbuf->cname = amem_strdup(project->amem, c->cname);
    pr->ncolors++;
    
    return RETURN_SUCCESS;
}

Quark *get_parent_project(const Quark *q)
{
    Quark *p = (Quark *) q;
    
    while (p) {
        if (p->fid == QFlavorProject) {
            return p;
        }
        p = quark_parent_get(p);
    }
    
    return NULL;
}
