/*
 * Grace - Graphics for Exploratory Data Analysis
 * 
 * Home page: http://plasma-gate.weizmann.ac.il/Grace/
 * 
 * Copyright (c) 1991-1995 Paul J Turner, Portland, OR
 * Copyright (c) 1996-2001 Grace Development Team
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

#ifndef __PATTERNS_H_
#define __PATTERNS_H_

static  unsigned char pat0_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static  unsigned char pat1_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static  unsigned char pat2_bits[] = {
   0xee, 0xee, 0xff, 0xff, 0xbb, 0xbb, 0xff, 0xff, 0xee, 0xee, 0xff, 0xff,
   0xbb, 0xbb, 0xff, 0xff, 0xee, 0xee, 0xff, 0xff, 0xbb, 0xbb, 0xff, 0xff,
   0xee, 0xee, 0xff, 0xff, 0xbb, 0xbb, 0xff, 0xff};
static  unsigned char pat3_bits[] = {
   0xee, 0xee, 0xbb, 0xbb, 0xee, 0xee, 0xbb, 0xbb, 0xee, 0xee, 0xbb, 0xbb,
   0xee, 0xee, 0xbb, 0xbb, 0xee, 0xee, 0xbb, 0xbb, 0xee, 0xee, 0xbb, 0xbb,
   0xee, 0xee, 0xbb, 0xbb, 0xee, 0xee, 0xbb, 0xbb};
static  unsigned char pat4_bits[] = {
   0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa,
   0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa,
   0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa};
static  unsigned char pat5_bits[] = {
   0x11, 0x11, 0x44, 0x44, 0x11, 0x11, 0x44, 0x44, 0x11, 0x11, 0x44, 0x44,
   0x11, 0x11, 0x44, 0x44, 0x11, 0x11, 0x44, 0x44, 0x11, 0x11, 0x44, 0x44,
   0x11, 0x11, 0x44, 0x44, 0x11, 0x11, 0x44, 0x44};
static  unsigned char pat6_bits[] = {
   0x11, 0x11, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00, 0x11, 0x11, 0x00, 0x00,
   0x44, 0x44, 0x00, 0x00, 0x11, 0x11, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00,
   0x11, 0x11, 0x00, 0x00, 0x44, 0x44, 0x00, 0x00};
static  unsigned char pat7_bits[] = {
   0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static  unsigned char pat8_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static  unsigned char pat9_bits[] = {
   0x1e, 0x1e, 0x0f, 0x0f, 0x87, 0x87, 0xc3, 0xc3, 0xe1, 0xe1, 0xf0, 0xf0,
   0x78, 0x78, 0x3c, 0x3c, 0x1e, 0x1e, 0x0f, 0x0f, 0x87, 0x87, 0xc3, 0xc3,
   0xe1, 0xe1, 0xf0, 0xf0, 0x78, 0x78, 0x3c, 0x3c};
static  unsigned char pat10_bits[] = {
   0x78, 0x78, 0xf0, 0xf0, 0xe1, 0xe1, 0xc3, 0xc3, 0x87, 0x87, 0x0f, 0x0f,
   0x1e, 0x1e, 0x3c, 0x3c, 0x78, 0x78, 0xf0, 0xf0, 0xe1, 0xe1, 0xc3, 0xc3,
   0x87, 0x87, 0x0f, 0x0f, 0x1e, 0x1e, 0x3c, 0x3c};
static  unsigned char pat11_bits[] = {
   0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
   0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
   0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33};
static  unsigned char pat12_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
   0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
   0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00};
static  unsigned char pat13_bits[] = {
   0x81, 0x81, 0x42, 0x42, 0x24, 0x24, 0x18, 0x18, 0x18, 0x18, 0x24, 0x24,
   0x42, 0x42, 0x81, 0x81, 0x81, 0x81, 0x42, 0x42, 0x24, 0x24, 0x18, 0x18,
   0x18, 0x18, 0x24, 0x24, 0x42, 0x42, 0x81, 0x81};
static  unsigned char pat14_bits[] = {
   0x80, 0x80, 0x40, 0x40, 0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x04, 0x04,
   0x02, 0x02, 0x01, 0x01, 0x80, 0x80, 0x40, 0x40, 0x20, 0x20, 0x10, 0x10,
   0x08, 0x08, 0x04, 0x04, 0x02, 0x02, 0x01, 0x01};
static  unsigned char pat15_bits[] = {
   0x01, 0x01, 0x02, 0x02, 0x04, 0x04, 0x08, 0x08, 0x10, 0x10, 0x20, 0x20,
   0x40, 0x40, 0x80, 0x80, 0x01, 0x01, 0x02, 0x02, 0x04, 0x04, 0x08, 0x08,
   0x10, 0x10, 0x20, 0x20, 0x40, 0x40, 0x80, 0x80};
static unsigned char pat16_bits[] = {
   0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
   0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
   0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22};
static unsigned char pat17_bits[] = {
   0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00};
static unsigned char pat18_bits[] = {
   0x22, 0x22, 0xff, 0xff, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0xff, 0xff,
   0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0xff, 0xff, 0x22, 0x22, 0x22, 0x22,
   0x22, 0x22, 0xff, 0xff, 0x22, 0x22, 0x22, 0x22};
static unsigned char pat19_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0x33, 0x33, 0x33, 0x33, 0xff, 0xff, 0xff, 0xff,
   0x33, 0x33, 0x33, 0x33, 0xff, 0xff, 0xff, 0xff, 0x33, 0x33, 0x33, 0x33,
   0xff, 0xff, 0xff, 0xff, 0x33, 0x33, 0x33, 0x33};
static unsigned char pat20_bits[] = {
   0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0xf0, 0xf0, 0xf0, 0xf0,
   0xf0, 0xf0, 0xf0, 0xf0, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
   0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0};
static unsigned char pat21_bits[] = {
   0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00,
   0xff, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff,
   0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff};
static unsigned char pat22_bits[] = {
   0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01,
   0x80, 0x01, 0xff, 0xff, 0xff, 0xff, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01,
   0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01};
static unsigned char pat23_bits[] = {
   0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x03,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x03, 0xc0, 0x03,
   0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x03};
static unsigned char pat24_bits[] = {
   0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0xff, 0xff, 0x40, 0x40, 0x40, 0x40,
   0x40, 0x40, 0xff, 0xff, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0xff, 0xff,
   0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xff, 0xff};
static unsigned char pat25_bits[] = {
   0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 0x18, 0x00,
   0xff, 0xff, 0xff, 0xff, 0x00, 0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 0x18,
   0x00, 0x18, 0x00, 0x18, 0xff, 0xff, 0xff, 0xff};
static unsigned char pat26_bits[] = {
   0x11, 0x11, 0xb8, 0xb8, 0x7c, 0x7c, 0x3a, 0x3a, 0x11, 0x11, 0xa3, 0xa3,
   0xc7, 0xc7, 0x8b, 0x8b, 0x11, 0x11, 0xb8, 0xb8, 0x7c, 0x7c, 0x3a, 0x3a,
   0x11, 0x11, 0xa3, 0xa3, 0xc7, 0xc7, 0x8b, 0x8b};
static unsigned char pat27_bits[] = {
   0x10, 0x10, 0x10, 0x10, 0x28, 0x28, 0xc7, 0xc7, 0x01, 0x01, 0x01, 0x01,
   0x82, 0x82, 0x7c, 0x7c, 0x10, 0x10, 0x10, 0x10, 0x28, 0x28, 0xc7, 0xc7,
   0x01, 0x01, 0x01, 0x01, 0x82, 0x82, 0x7c, 0x7c};
static unsigned char pat28_bits[] = {
   0x1c, 0x1c, 0x12, 0x12, 0x11, 0x11, 0x21, 0x21, 0xc1, 0xc1, 0x21, 0x21,
   0x11, 0x11, 0x12, 0x12, 0x1c, 0x1c, 0x12, 0x12, 0x11, 0x11, 0x21, 0x21,
   0xc1, 0xc1, 0x21, 0x21, 0x11, 0x11, 0x12, 0x12};
static unsigned char pat29_bits[] = {
   0x3e, 0x3e, 0x41, 0x41, 0x80, 0x80, 0x80, 0x80, 0xe3, 0xe3, 0x14, 0x14,
   0x08, 0x08, 0x08, 0x08, 0x3e, 0x3e, 0x41, 0x41, 0x80, 0x80, 0x80, 0x80,
   0xe3, 0xe3, 0x14, 0x14, 0x08, 0x08, 0x08, 0x08};
static unsigned char pat30_bits[] = {
   0x48, 0x48, 0x88, 0x88, 0x84, 0x84, 0x83, 0x83, 0x84, 0x84, 0x88, 0x88,
   0x48, 0x48, 0x38, 0x38, 0x48, 0x48, 0x88, 0x88, 0x84, 0x84, 0x83, 0x83,
   0x84, 0x84, 0x88, 0x88, 0x48, 0x48, 0x38, 0x38};
static unsigned char pat31_bits[] = {
   0x03, 0x03, 0x04, 0x04, 0x08, 0x08, 0x08, 0x08, 0x0c, 0x0c, 0x12, 0x12,
   0x21, 0x21, 0xc0, 0xc0, 0x03, 0x03, 0x04, 0x04, 0x08, 0x08, 0x08, 0x08,
   0x0c, 0x0c, 0x12, 0x12, 0x21, 0x21, 0xc0, 0xc0};

static Pattern patterns_init[] = {
    {16, 16, pat0_bits},
    {16, 16, pat1_bits},
    {16, 16, pat2_bits},
    {16, 16, pat3_bits},
    {16, 16, pat4_bits},
    {16, 16, pat5_bits},
    {16, 16, pat6_bits},
    {16, 16, pat7_bits},
    {16, 16, pat8_bits},
    {16, 16, pat9_bits},
    {16, 16, pat10_bits},
    {16, 16, pat11_bits},
    {16, 16, pat12_bits},
    {16, 16, pat13_bits},
    {16, 16, pat14_bits},
    {16, 16, pat15_bits},
    {16, 16, pat16_bits},
    {16, 16, pat17_bits},
    {16, 16, pat18_bits},
    {16, 16, pat19_bits},
    {16, 16, pat20_bits},
    {16, 16, pat21_bits},
    {16, 16, pat22_bits},
    {16, 16, pat23_bits},
    {16, 16, pat24_bits},
    {16, 16, pat25_bits},
    {16, 16, pat26_bits},
    {16, 16, pat27_bits},
    {16, 16, pat28_bits},
    {16, 16, pat29_bits},
    {16, 16, pat30_bits},
    {16, 16, pat31_bits}
};


static unsigned int dash_array0[2] =
{0, 1};
static unsigned int dash_array1[2] =
{1, 0};
static unsigned int dash_array2[2] =
{1, 3};
static unsigned int dash_array3[2] =
{5, 3};
static unsigned int dash_array4[2] =
{7, 3};
static unsigned int dash_array5[4] =
{1, 3, 5, 3};
static unsigned int dash_array6[4] =
{1, 3, 7, 3};
static unsigned int dash_array7[6] =
{1, 3, 5, 3, 1, 3};
static unsigned int dash_array8[6] =
{5, 3, 1, 3, 5, 3};

static LineStyle linestyles_init[] =
{
    {2, dash_array0},
    {2, dash_array1},
    {2, dash_array2},
    {2, dash_array3},
    {2, dash_array4},
    {4, dash_array5},
    {4, dash_array6},
    {6, dash_array7},
    {6, dash_array8}
};

#endif /* __PATTERNS_H_ */
