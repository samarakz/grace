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

#include <config.h>

#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "globals.h"
#include "utils.h"
#include "device.h"

static unsigned int ndevices = 0;
static int curdevice = 0;
static Device_entry *device_table = NULL;

int set_page_geometry(Page_geometry pg)
{
    if (pg.width  > 0 &&
	pg.height > 0 &&
        pg.dpi_x  > 0 &&
        pg.dpi_y  > 0) {
        device_table[curdevice].pg = pg;
	return GRACE_EXIT_SUCCESS;
    } else {
        return GRACE_EXIT_FAILURE;
    }
}

Page_geometry get_page_geometry(void)
{
    return (device_table[curdevice].pg);
}

int register_device(Device_entry device)
{
    int dindex;
    
    ndevices++;
    dindex = ndevices - 1;
    device_table = xrealloc(device_table, ndevices*sizeof(Device_entry));

    device_table[dindex] = device;

    device_table[dindex].name = NULL;
    device_table[dindex].name = 
                        copy_string(device_table[dindex].name, device.name);
    device_table[dindex].fext = NULL;
    device_table[dindex].fext = 
                        copy_string(device_table[dindex].fext, device.fext);
    
    return dindex;
}

int select_device(int dindex)
{
    if (dindex >= ndevices || dindex < 0) {
        return GRACE_EXIT_FAILURE;
    } else {
        curdevice = dindex;
	return GRACE_EXIT_SUCCESS;
    }
}

/*
 * set the current print device
 */
void set_printer(int device)
{
    hdevice = device;
}

int get_device_by_name(char *dname)
{
    int i;
    
    i = 0;
    while (i < ndevices) {
        if (strncmp(device_table[i].name, dname, strlen(dname)) == 0) {
            break;
        } else {
            i++;
        }
    }
    if (i >= ndevices) {
        return -1;
    } else {
	return i;
    }
}

int set_printer_by_name(char *dname)
{
    int id;
    
    id = get_device_by_name(dname);
    
    if (id >= ndevices || id < 0 || device_table[id].type == DEVICE_TERM) {
        return GRACE_EXIT_FAILURE;
    } else {
        hdevice = id;
	return GRACE_EXIT_SUCCESS;
    }
}

void set_ptofile(int flag)
{
    ptofile = flag;
}

int initgraphics(void)
{
    return ((*device_table[curdevice].init)());
}

Device_entry get_device_props(int device)
{
    return (device_table[device]);
}

Device_entry get_curdevice_props()
{
    return (device_table[curdevice]);
}

void set_device_props(int deviceid, Device_entry device)
{
    device_table[deviceid].type = device.type;
/*
 *     device_table[deviceid].init = device.init;
 *     device_table[deviceid].parser = device.parser;
 *     device_table[deviceid].setup = device.setup;
 */
    device_table[deviceid].devfonts = device.devfonts;
    device_table[deviceid].fontaa = device.fontaa;
    device_table[deviceid].pg = device.pg;
}

void set_curdevice_props(Device_entry device)
{
    set_device_props(curdevice, device);
}

int parse_device_options(int dindex, char *options)
{
    char *p, *oldp, opstring[64];
    int n;
        
    if (dindex >= ndevices || dindex < 0 || 
            device_table[dindex].parser == NULL) {
        return GRACE_EXIT_FAILURE;
    } else {
        oldp = options;
        while ((p = strchr(oldp, ',')) != NULL) {
	    n = MIN2((p - oldp), 64 - 1);
            strncpy(opstring, oldp, n);
            opstring[n] = '\0';
            if (device_table[dindex].parser(opstring) != GRACE_EXIT_SUCCESS) {
                return GRACE_EXIT_FAILURE;
            }
            oldp = p + 1;
        }
        return device_table[dindex].parser(oldp);
    }
}

int number_of_devices(void)
{
    return (ndevices);
}

void get_page_viewport(double *vx, double *vy)
{
    *vx = device_table[curdevice].pg.width/device_table[curdevice].pg.dpi_x;
    *vy = device_table[curdevice].pg.height/device_table[curdevice].pg.dpi_y;
    if (*vx < *vy) {
        *vy /= *vx;
        *vx = 1.0;
    } else {
        *vx /= *vy;
        *vy = 1.0;
    }
}
