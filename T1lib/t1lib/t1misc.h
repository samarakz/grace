/*--------------------------------------------------------------------------
  ----- File:        t1misc.h
  ----- Author:      Rainer Menzner (rmz@neuroinformatik.ruhr-uni-bochum.de)
  ----- Date:        06/21/1998
  ----- Description: This file is part of the t1-library. It contains
                     some miscellaneous definitions.
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
  

#define LINE_BUF 512
#define BUF_SIZE 512
#define NULLPTR  (void *) 0


/* t1lib-glyphs are padded to this value by default: */
#define T1GLYPH_PAD   8
#define T1GLYPH_BIT   0 
#define T1GLYPH_BYTE  0 
#define PAD(bits, pad)  (((bits)+(pad)-1)&-(pad))


/* For debugging purposes: */
#define SLEEP for(j=0; j<50; j++){for (k=0; k<65536; k++);}


#define KERN_FLAG      0x01
#define LIGATURE_FLAG  0x02


/* The maximum length of a PostScript name of a font: */
#define MAXPSNAMELEN 256

/* The default resolution used by the library */
#define DEFAULT_RES           72.0
#define BIGPOINTSPERINCH      72.0

/* Defaults for configuration file searching: */
/* (Note: The directory where the global config will be located is
   determined by configure at compile time) */
#define T1_CONFIGFILENAME    ".t1librc"
#define ENV_CONF_STRING      "T1LIB_CONFIG"
#define DIRECTORY_SEP        "/"
#define GLOBAL_CONFIG_FILE   "t1lib.config"


/* Default name for t1lib-logfile: */
#define T1_LOG_FILE       "t1lib.log"
/* logfile levels: */
#define T1LOG_ERROR     1
#define T1LOG_WARNING   2
#define T1LOG_STATISTIC 3
#define T1LOG_DEBUG     4


/* def's for T1_InitLib() */
#define NO_LOGFILE          0x0
#define LOGFILE             0x1
#define IGNORE_FONTDATABASE 0x2  /* Default is to read database */
#define IGNORE_CONFIGFILE   0x4  /* Default is to read config file */


/* How may ligatures a character may have at maximum: */
#define MAX_LIGS 20


/* The maximum number of gray-levels in antialiasing */
#define AAMAXPLANES 17
#define T1_AA_LOW   2
#define T1_AA_HIGH  4


/* How much to advance the number of FONTPRIVATE structures when
   reallocating for new fonts? */
#define ADVANCE_FONTPRIVATE   10

#define NO_ANTIALIAS 0
#define ANTIALIAS    1


/* #define's for manipulating searchpaths */
#define T1_PFAB_PATH        0x01
#define T1_AFM_PATH         0x02
#define T1_ENC_PATH         0x04
#define T1_APPEND_PATH      0x00
#define T1_PREPEND_PATH     0x01


/* #define's for character attributes (not all implemented) */
#define T1_UNDERLINE           0x0001
#define T1_OVERLINE            0x0002
#define T1_OVERSTRIKE          0x0004
#define T1_DOUBLEOVERSTRIKE    0x0008
#define T1_SUBSCRIPT           0x0100
#define T1_SUPERSCRIPT         0x0200
#define T1_OUTLINE             0x1000
#define T1_KERNING             0x2000


/* Error handling: */
/* These are from scanning a font file */
#define T1ERR_SCAN_FILE_OPEN_ERR     -4
#define T1ERR_SCAN_OUT_OF_MEMORY     -3
#define T1ERR_SCAN_ERROR             -2
#define T1ERR_SCAN_FILE_EOF          -1
/* These are from generating paths */
#define T1ERR_PATH_ERROR              1
#define T1ERR_PARSE_ERROR             2
/* These are from t1lib */
#define T1ERR_INVALID_FONTID          10
#define T1ERR_INVALID_PARAMETER       11
#define T1ERR_OP_NOT_PERMITTED        12
#define T1ERR_ALLOC_MEM               13
#define T1ERR_FILE_OPEN_ERR           14
#define T1ERR_UNSPECIFIED             15


/* The info_flags meaning */
#define AFM_SUCCESS               (short)0x0001
#define AFM_SLOPPY_SUCCESS        (short)0x0002
#define AFM_SELFGEN_SUCCESS       (short)0x0004
#define USES_STANDARD_ENCODING    (short)0x0008
#define FONT_NOCACHING            (short)0x0100

extern int errno;

#ifndef PI
#define PI 3.1415927
#endif


/* Flags to control the rasterizer */
#define T1_IGNORE_FORCEBOLD           0x0001
#define T1_IGNORE_FAMILYALIGNMENT     0x0002
#define T1_IGNORE_HINTING             0x0004

#define T1_DEBUG_LINE                 0x0100
#define T1_DEBUG_REGION               0x0200
#define T1_DEBUG_PATH                 0x0400
#define T1_DEBUG_FONT                 0x0800
#define T1_DEBUG_HINT                 0x1000

