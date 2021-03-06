/*
 * Grace - GRaphing, Advanced Computation and Exploration of data
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-1995 Paul J Turner, Portland, OR
 * Copyright (c) 1996-2000 GRACE Development Team
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
 * Bitmaps for matrix fill order selector
 */

#ifndef __MBITMAPS_H_
#define __MBITMAPS_H_

#define MBITMAP_WIDTH  20
#define MBITMAP_HEIGHT 20


static unsigned char m_hv_lr_bt_bits[] = {
   0x00, 0x03, 0x00, 0x00, 0x0f, 0x00, 0xfc, 0xff, 0x03, 0x08, 0x0f, 0x00,
   0x10, 0x03, 0x00, 0x20, 0x00, 0x00, 0x40, 0x00, 0x00, 0x80, 0x00, 0x00,
   0x00, 0x01, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x06, 0x00, 0x00, 0x0a, 0x00,
   0x00, 0x10, 0x00, 0x00, 0x20, 0x00, 0x00, 0x40, 0x00, 0x00, 0x83, 0x00,
   0x00, 0x0f, 0x01, 0xfc, 0xff, 0x03, 0x00, 0x0f, 0x00, 0x00, 0x03, 0x00};
static unsigned char m_hv_lr_tb_bits[] = {
   0x00, 0x03, 0x00, 0x00, 0x0f, 0x00, 0xfc, 0xff, 0x03, 0x00, 0x0f, 0x01,
   0x00, 0x83, 0x00, 0x00, 0x40, 0x00, 0x00, 0x20, 0x00, 0x00, 0x10, 0x00,
   0x00, 0x0a, 0x00, 0x00, 0x06, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x01, 0x00,
   0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0x20, 0x00, 0x00, 0x10, 0x03, 0x00,
   0x08, 0x0f, 0x00, 0xfc, 0xff, 0x03, 0x00, 0x0f, 0x00, 0x00, 0x03, 0x00};
static unsigned char m_hv_rl_bt_bits[] = {
   0x00, 0x0c, 0x00, 0x00, 0x0f, 0x00, 0xfc, 0xff, 0x03, 0x00, 0x0f, 0x01,
   0x00, 0x8c, 0x00, 0x00, 0x40, 0x00, 0x00, 0x20, 0x00, 0x00, 0x10, 0x00,
   0x00, 0x08, 0x00, 0x00, 0x07, 0x00, 0x00, 0x06, 0x00, 0x00, 0x05, 0x00,
   0x80, 0x00, 0x00, 0x40, 0x00, 0x00, 0x20, 0x00, 0x00, 0x10, 0x0c, 0x00,
   0x08, 0x0f, 0x00, 0xfc, 0xff, 0x03, 0x00, 0x0f, 0x00, 0x00, 0x0c, 0x00};
static unsigned char m_hv_rl_tb_bits[] = {
   0x00, 0x0c, 0x00, 0x00, 0x0f, 0x00, 0xfc, 0xff, 0x03, 0x08, 0x0f, 0x00,
   0x10, 0x0c, 0x00, 0x20, 0x00, 0x00, 0x40, 0x00, 0x00, 0x80, 0x00, 0x00,
   0x00, 0x05, 0x00, 0x00, 0x06, 0x00, 0x00, 0x07, 0x00, 0x00, 0x08, 0x00,
   0x00, 0x10, 0x00, 0x00, 0x20, 0x00, 0x00, 0x40, 0x00, 0x00, 0x8c, 0x00,
   0x00, 0x0f, 0x01, 0xfc, 0xff, 0x03, 0x00, 0x0f, 0x00, 0x00, 0x0c, 0x00};
static unsigned char m_vh_lr_bt_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x02, 0x0c, 0x00, 0x02,
   0x14, 0x00, 0x02, 0x24, 0x00, 0x02, 0x44, 0x00, 0x02, 0x84, 0x00, 0x02,
   0x0e, 0x05, 0x07, 0x0e, 0x06, 0x07, 0x1f, 0x87, 0x0f, 0x1f, 0x88, 0x0f,
   0x04, 0x10, 0x02, 0x04, 0x20, 0x02, 0x04, 0x40, 0x02, 0x04, 0x80, 0x02,
   0x04, 0x00, 0x03, 0x04, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static unsigned char m_vh_lr_tb_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x02, 0x04, 0x00, 0x03,
   0x04, 0x80, 0x02, 0x04, 0x40, 0x02, 0x04, 0x20, 0x02, 0x04, 0x10, 0x02,
   0x1f, 0x88, 0x0f, 0x1f, 0x87, 0x0f, 0x0e, 0x06, 0x07, 0x0e, 0x05, 0x07,
   0x84, 0x00, 0x02, 0x44, 0x00, 0x02, 0x24, 0x00, 0x02, 0x14, 0x00, 0x02,
   0x0c, 0x00, 0x02, 0x04, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static unsigned char m_vh_rl_bt_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x02, 0x04, 0x00, 0x03,
   0x04, 0x80, 0x02, 0x04, 0x40, 0x02, 0x04, 0x20, 0x02, 0x04, 0x10, 0x02,
   0x0e, 0x0a, 0x07, 0x0e, 0x06, 0x07, 0x1f, 0x8e, 0x0f, 0x1f, 0x81, 0x0f,
   0x84, 0x00, 0x02, 0x44, 0x00, 0x02, 0x24, 0x00, 0x02, 0x14, 0x00, 0x02,
   0x0c, 0x00, 0x02, 0x04, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static unsigned char m_vh_rl_tb_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x02, 0x0c, 0x00, 0x02,
   0x14, 0x00, 0x02, 0x24, 0x00, 0x02, 0x44, 0x00, 0x02, 0x84, 0x00, 0x02,
   0x1f, 0x81, 0x0f, 0x1f, 0x8e, 0x0f, 0x0e, 0x06, 0x07, 0x0e, 0x0a, 0x07,
   0x04, 0x10, 0x02, 0x04, 0x20, 0x02, 0x04, 0x40, 0x02, 0x04, 0x80, 0x02,
   0x04, 0x00, 0x03, 0x04, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#endif /* __MBITMAPS_H_ */
