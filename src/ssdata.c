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
 * spreadsheet data stuff
 *
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "globals.h"
#include "utils.h"
#include "graphs.h"
#include "graphutils.h"
#include "files.h"
#include "ssdata.h"

#include "protos.h"

double *copy_data_column(double *src, int nrows)
{
    double *dest;
    
    dest = malloc(nrows*SIZEOF_DOUBLE);
    if (dest != NULL) {
        memcpy(dest, src, nrows*SIZEOF_DOUBLE);
    }
    return dest;
}

/* TODO: index_shift */
double *allocate_index_data(int nrows)
{
    int i;
    double *retval;
    
    retval = malloc(nrows*SIZEOF_DOUBLE);
    if (retval != NULL) {
        for (i = 0; i < nrows; i++) {
            retval[i] = i;
        }
    }
    return retval;
}

static ss_data blockdata = {0, 0, NULL, NULL};

void set_blockdata(ss_data *ssd)
{
    free_ss_data(&blockdata);
    memcpy(&blockdata, ssd, sizeof(ss_data));
}

int get_blockncols(void)
{
    return blockdata.ncols;
}

int get_blocknrows(void)
{
    return blockdata.nrows;
}

int *get_blockformats(void)
{
    return blockdata.formats;
}

int realloc_ss_data(ss_data *ssd, int nrows)
{
    int i, j;
    char  **sp;
    
    for (i = 0; i < ssd->ncols; i++) {
        if (ssd->formats[i] == FFORMAT_STRING) {
            sp = (char **) ssd->data[i];
            for (j = nrows; j < ssd->nrows; j++) {
                cxfree(sp[j]);
            }
            ssd->data[i] = xrealloc(ssd->data[i], nrows*sizeof(char *));
            sp = (char **) ssd->data[i];
            for (j = ssd->nrows; j < nrows; j++) {
                sp[j] = NULL;
            }
        } else {
            ssd->data[i] = xrealloc(ssd->data[i], nrows*SIZEOF_DOUBLE);
        }
    }
    ssd->nrows = nrows;
    
    return GRACE_EXIT_SUCCESS;
}

void free_ss_data(ss_data *ssd)
{
    int i, j;
    char  **sp;
    
    for (i = 0; i < ssd->ncols; i++) {
        if (ssd->formats[i] == FFORMAT_STRING) {
            sp = (char **) ssd->data[i];
            for (j = 0; j < ssd->nrows; j++) {
                cxfree(sp[j]);
            }
        }
        cxfree(ssd->data[i]);
    }
    cxfree(ssd->data);
    cxfree(ssd->formats);
    ssd->nrows = 0;
    ssd->ncols = 0;
}

int init_ss_data(ss_data *ssd, int ncols, int *formats)
{
    int i;
    
    ssd->data = malloc(ncols*SIZEOF_VOID_P);
    for (i = 0; i < ncols; i++) {
        ssd->data[i] = NULL;
    }
    ssd->formats = formats;
    ssd->ncols = ncols;
    ssd->nrows = 0;

    return GRACE_EXIT_SUCCESS;
}

static char *next_token(char *s, char **token, int *quoted)
{
    *quoted = FALSE;
    *token = NULL;
    
    if (s == NULL) {
        return NULL;
    }
    
    while (*s == ' ' || *s == '\t') {
        s++;
    }
    if (*s == '"') {
        s++;
        *token = s;
        while (*s != '\0' && (*s != '"' || (*s == '"' && *(s - 1) == '\\'))) {
            s++;
        }
        if (*s == '"') {
            /* successfully identified a quoted string */
            *quoted = TRUE;
        }
    } else {
        *token = s;
        if (**token == '\n') {
            /* EOL reached */
            return NULL;
        }
        while (*s != '\n' && *s != '\0' && *s != ' ' && *s != '\t') {
            s++;
        }
    }
    
    if (*s != '\0') {
        *s = '\0';
        s++;
        return s;
    } else {
        return NULL;
    }
}

int parse_ss_row(const char *s, int *nncols, int *nscols, int **formats)
{
    int ncols;
    int quoted;
    char *buf, *s1, *token;
    double value;
    Dates_format df_pref, df_rec;

    *nscols = 0;
    *nncols = 0;
    *formats = NULL;
    df_pref = get_date_hint();
    buf = copy_string(NULL, s);
    s1 = buf;
    while ((s1 = next_token(s1, &token, &quoted)) != NULL) {
        if (token == NULL) {
            *nscols = 0;
            *nncols = 0;
            cxfree(*formats);
            free(buf);
            return GRACE_EXIT_FAILURE;
        }
        
        ncols = *nncols + *nscols;
        /* reallocate the formats array */
        if (ncols % 10 == 0) {
            *formats = xrealloc(*formats, (ncols + 10)*SIZEOF_INT);
        }

        if (quoted) {
            (*formats)[ncols] = FFORMAT_STRING;
            (*nscols)++;
        } else if (parse_date(token, df_pref, &value, &df_rec) ==
            GRACE_EXIT_SUCCESS) {
            (*formats)[ncols] = FFORMAT_DATE;
            (*nncols)++;
        } else {
            (*formats)[ncols] = FFORMAT_NUMBER;
            (*nncols)++;
        }
    }
    free(buf);
    
    return GRACE_EXIT_SUCCESS;
}


/* NOTE: the input string will be corrupted! */
int insert_data_row(ss_data *ssd, int row, char *s)
{
    int i, j;
    int ncols = ssd->ncols;
    char *token;
    int quoted;
    char  **sp;
    double *np;
    Dates_format df_pref, df_rec;
    
    df_pref = get_date_hint();
    for (i = 0; i < ncols; i++) {
        s = next_token(s, &token, &quoted);
        if (s == NULL || token == NULL) {
            /* invalid line: free the already allocated string fields */
            for (j = 0; j < i; j++) {
                if (ssd->formats[j] == FFORMAT_STRING) {
                    sp = (char **) ssd->data[j];
                    cxfree(sp[row]);
                }
            }
            return GRACE_EXIT_FAILURE;
        } else {
            if (ssd->formats[i] == FFORMAT_STRING) {
                sp = (char **) ssd->data[i];
                sp[row] = copy_string(NULL, token);
            } else if (ssd->formats[i] == FFORMAT_DATE) {
                np = (double *) ssd->data[i];
                parse_date(token, df_pref, &np[row], &df_rec);
            } else {
                np = (double *) ssd->data[i];
                np[row] = atof(token);
            }
        }
    }
    
    return GRACE_EXIT_SUCCESS;
}


int store_data(ss_data *ssd, int load_type, char *label)
{
    int ncols, nncols, nncols_req, nscols, nrows;
    int i, j;
    double *xdata;
    int gno, setno;
    int x_from_index;
    
    if (ssd == NULL) {
        return GRACE_EXIT_FAILURE;
    }
    ncols = ssd->ncols;
    nrows = ssd->nrows;
    if (ncols <= 0 || nrows <= 0) {
        return GRACE_EXIT_FAILURE;
    }

    nncols = 0;
    for (j = 0; j < ncols; j++) {
        if (ssd->formats[j] != FFORMAT_STRING) {
            nncols++;
        }
    }
    nscols = ncols - nncols;
    
    gno = get_parser_gno();
    
    switch (load_type) {
    case LOAD_SINGLE:
        if (nncols > MAX_SET_COLS || nscols > 1) {
            errmsg("Internal error");
            free_ss_data(ssd);
            return GRACE_EXIT_FAILURE;
        }

        nncols_req = settype_cols(curtype);
        x_from_index = FALSE;
        if (nncols_req == nncols + 1) {
            x_from_index = TRUE;
        } else if (nncols_req != nncols) {
	    errmsg("Column count incorrect");
	    return GRACE_EXIT_FAILURE;
        }

        setno = nextset(gno);
        set_set_hidden(gno, setno, FALSE);
        set_dataset_type(gno, setno, curtype);

        nncols = 0;
        if (x_from_index) {
            xdata = allocate_index_data(nrows);
            if (xdata == NULL) {
                errmsg("Can't allocate more sets!");
                free_ss_data(ssd);
            }
            setcol(gno, setno, nncols, xdata, nrows);
            nncols++;
        }
        for (j = 0; j < ncols; j++) {
            if (ssd->formats[j] == FFORMAT_STRING) {
                set_set_strings(gno, setno, nrows, (char **) ssd->data[j]);
            } else {
                setcol(gno, setno, nncols, (double *) ssd->data[j], nrows);
                nncols++;
            }
        }
        if (!strlen(getcomment(gno, setno))) {
            setcomment(gno, setno, label);
        }
        log_results(label);
        
        cxfree(ssd->data);
        break;
    case LOAD_NXY:
        if (nscols != 0) {
            errmsg("Internal error");
            free_ss_data(ssd);
            return GRACE_EXIT_FAILURE;
        }
        
        for (i = 0; i < ncols - 1; i++) {
            setno = nextset(gno);
            if (setno == -1) {
                errmsg("Can't allocate more sets!");
                free_ss_data(ssd);
                return GRACE_EXIT_FAILURE;
            }
            if (i > 0) {
                xdata = copy_data_column((double *) ssd->data[0], nrows);
                if (xdata == NULL) {
                    errmsg("Can't allocate more sets!");
                    free_ss_data(ssd);
                }
            } else {
                xdata = (double *) ssd->data[0];
            }
            set_set_hidden(gno, setno, FALSE);
            set_dataset_type(gno, setno, SET_XY);
            setcol(gno, setno, DATA_X, xdata, nrows);
            setcol(gno, setno, DATA_Y, (double *) ssd->data[i + 1], nrows);
            setcomment(gno, setno, label);
            log_results(label);
        }
    
        cxfree(ssd->data);
        break;
    case LOAD_BLOCK:
        set_blockdata(ssd);
        log_results(label);
        break;
    default:
        errmsg("Internal error");
        free_ss_data(ssd);
        return GRACE_EXIT_FAILURE;
    }
    
    return GRACE_EXIT_SUCCESS;
}

#define OLD_COL_INDICES

void field_string_to_cols(char *fs, int *nc, int **cols)
{
    int col, coli[MAX_SET_COLS];
    char *s, buf[256];

    strcpy(buf, fs);
    s = buf;
    *nc = 0;
    while ((s = strtok(s, ":")) != NULL) {
#ifdef OLD_COL_INDICES
        col = atoi(s);
        col--;
#else
	if (!strcmp(s, "i")) {
            col = -1;
        } else {
            col = atoi(s);
        }
#endif
	if (col < -1) {
	    errmsg("Column index out of range");
	}
        coli[*nc] = col;
	(*nc)++;
        if (*nc > MAX_SET_COLS) {
            errmsg("Too many columns scanned in column string");
        }
	s = NULL;
    }
    
    *cols = coli;
}

char *cols_to_field_string(int nc, int *cols)
{
    int i;
    char *s, buf[32];
    
    s = NULL;
    for (i = 0; i < nc; i++) {
#ifdef OLD_COL_INDICES
        sprintf(buf, "%d", cols[i] + 1);
#else
        if (cols[i] == -1) {
            strcpy(buf, "i");
        } else {
            sprintf(buf, "%d", cols[i]);
        }
#endif
        if (i != 0) {
            s = concat_strings(s, ":");
        }
        s = concat_strings(s, buf);
    }
    
    return s;
}

void create_set_fromblock(int gno, int type, int nc, int *coli, int scol)
{
    int i, ncols, blockncols, blocklen, column;
    int setno;
    double *cdata;
    char buf[256];

    blockncols = get_blockncols();
    if (blockncols <= 0) {
        errmsg("No block data read");
        return;
    }

    blocklen = get_blocknrows();
    
    ncols = settype_cols(type);
    if (nc > ncols) {
        errmsg("Too many columns scanned in column string");
        return;
    }
    if (nc < ncols) {
	errmsg("Too few columns scanned in column string");
	return;
    }
    
    for (i = 0; i < nc; i++) {
	if (coli[i] < -1 || coli[i] >= blockncols) {
	    errmsg("Column index out of range");
	    return;
	}
    }
    
    if (scol >= blockncols) {
	errmsg("String column index out of range");
	return;
    }
    
    setno = nextset(gno);
    if (setno == -1) {
	return;
    }
    
    activateset(gno, setno);
    set_dataset_type(gno, setno, type);

    for (i = 0; i < nc; i++) {
        column = coli[i];
        if (column == -1) {
            cdata = allocate_index_data(blocklen);
        } else {
            if (blockdata.formats[column] != FFORMAT_STRING) {
                cdata = copy_data_column((double *) blockdata.data[column], blocklen);
            } else {
                errmsg("Tried to read doubles from strings!");
                killsetdata(gno, setno);
                return;
            }
        }
        if (cdata == NULL) {
            errmsg("Can't allocate more sets!");
            killsetdata(gno, setno);
            return;
        }
        setcol(gno, setno, i, cdata, blocklen);
    }

    /* strings, if any */
    if (scol >= 0) {
        if (blockdata.formats[scol] != FFORMAT_STRING) {
            errmsg("Tried to read strings from doubles!");
            killsetdata(gno, setno);
            return;
        } else {
            set_set_strings(gno, setno, blocklen, (char **) blockdata.data[scol]);
        }
    }

    sprintf(buf, "Cols %s", cols_to_field_string(nc, coli));
    setcomment(gno, setno, buf);

    autoscale_graph(gno, autoscale_onread);

    log_results(buf);   
}
