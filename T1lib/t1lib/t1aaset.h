/*--------------------------------------------------------------------------
  ----- File:        t1aaset.h
  ----- Author:      Rainer Menzner (rmz@neuroinformatik.ruhr-uni-bochum.de)
  ----- Date:        08/20/1998
  ----- Description: This file is part of the t1-library. It contains
                     definitions and declarations for t1set.c.
  ----- Copyright:   t1lib is copyrighted (c) Rainer Menzner, 1996-1998. 
                     As of version 0.5, t1lib is distributed under the
		     GNU General Public Library Lincense. The
		     conditions can be found in the files LICENSE and
		     LGPL, which should reside in the toplevel
		     directory of the distribution.  Please note that 
		     there are parts of t1lib that are subject to
		     other licenses:
		     The parseAFM-package is copyrighted by Adobe Systems
		     Inc.
		     The type1 rasterizer is copyrighted by IBM and the
		     X11-consortium.
  ----- Warranties:  Of course, there's NO WARRANTY OF ANY KIND :-)
  ----- Credits:     I want to thank IBM and the X11-consortium for making
                     their rasterizer freely available.
		     Also thanks to Piet Tutelaers for his ps2pk, from
		     which I took the rasterizer sources in a format
		     independent from X11.
                     Thanks to all people who make free software living!
--------------------------------------------------------------------------*/
  
#ifdef T1AASET_C

GLYPH *T1_AASetChar( int FontID, char charcode, 
		     float size, T1_TMATRIX *transform);
GLYPH *T1_AASetString( int FontID, char *string, int len,
		       long spaceoff, int modflag,
		       float size, T1_TMATRIX *transform);
int T1_AASetGrayValues( unsigned long white,
			unsigned long gray75,
			unsigned long gray50,
			unsigned long gray25,
			unsigned long black);
int T1_AAHSetGrayValues( unsigned long *grayvals);
int T1_AASetBitsPerPixel( int bpp);
int T1_AASetLevel( int level);
int T1_AAGetLevel( void);

#else

extern GLYPH *T1_AASetChar( int FontID, char charcode, 
			    float size, T1_TMATRIX *transform);
extern GLYPH *T1_AASetString( int FontID, char *string, int len,
			      long spaceoff, int modflag,
			      float size, T1_TMATRIX *transform);
extern int T1_AASetGrayValues( unsigned long white,
			       unsigned long gray75,
			       unsigned long gray50,
			       unsigned long gray25,
			       unsigned long black);
extern int T1_AAHSetGrayValues( unsigned long *grayvals);
extern int T1_AASetBitsPerPixel( int bpp);
extern int T1_AASetLevel( int level);
extern int T1_AAGetLevel( void);

#endif

