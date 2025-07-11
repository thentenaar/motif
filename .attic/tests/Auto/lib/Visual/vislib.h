/* 
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
*/ 
/* 
 * HISTORY
*/ 
/*   $XConsortium: vislib.h /main/9 1996/10/30 11:34:49 drk $ */
/***********************************************************************
  	@(#)vislib.h	1.4.1.1	Date:1/22/91
  	Author: SJS
	History:
            09/14/90 Create, split libraries into maangeable size
	Calls:

	Summary:
************************************************************************/
/******************************************************************************
                VISUAL GENERATE / CHECKING UTILITY DECLARATIONS
******************************************************************************/

#ifndef VISLIB

#define VISLIB

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <stdio.h>
#include "mvs_defines.h"
#include "mvs_types.h"

/* Visual utilities mode of operation */

#define VISUAL_MODE_DONT_CHECK           0
#define VISUAL_MODE_BATCH_COMPARE        1
#define VISUAL_MODE_INTERACTIVE_COMPARE  2
#define VISUAL_MODE_GENERATE_PIXMAPS     3

/* Define mvsGLOBAL to cause this file to conditionally generate global code */

#ifdef DECLARE_VISLIB_GLOBALS_HERE
#    define visGLOBAL
#    define visINIT(x) = x
#else
#    define visGLOBAL extern
#    define visINIT(x)
#endif

  visGLOBAL Display *visDisplay visINIT(NULL);

  visGLOBAL int mvsVisualMode visINIT(VISUAL_MODE_DONT_CHECK);


/* Pixmap cache declarations */

#define MAX_PIXMAPS 100

  visGLOBAL Pixmap         mvsAvailPixmap[MAX_PIXMAPS];
  visGLOBAL XImage        *mvsAvailImage[MAX_PIXMAPS];
  visGLOBAL int            mvsNumAvailPixmaps;


/* Image File Info */

  visGLOBAL FILE *mvsImageFd;               /* Image file descriptor          */
  visGLOBAL long mvsBeginningOfImageFile;   /* File location where images star*/
  visGLOBAL long mvsBeginningOfRecIds;      /* File location of rec id table  */


/* Internal Table of Record IDs, Record Length, File Position, and Checksum */

# define MAX_RECORD_IDS 1500
# define MAX_RECORD_ID_LENGTH 5000

  visGLOBAL unsigned char   mvsRecIdTable[MAX_RECORD_IDS][MAX_RECORD_ID_LENGTH];
  visGLOBAL long            mvsRecLength[MAX_RECORD_IDS];
  visGLOBAL short           mvsRecIdLength[MAX_RECORD_IDS];
  visGLOBAL long            mvsFilePos[MAX_RECORD_IDS];
  visGLOBAL unsigned int    mvsVisualChecksum[MAX_RECORD_IDS];
  visGLOBAL short           mvsNumRecIds;


/* The BIG Image Buffer Statically Declared Here */

# define MAX_IMAGE_DIMENSION 1024
  visGLOBAL unsigned char mvsImageBuffer[MAX_IMAGE_DIMENSION*MAX_IMAGE_DIMENSION];

/* Structure that holds the temp images images used to compare
   against during compare run.
 */

  struct mvsTempImageRec  {
    unsigned char mvsTempImageBuffer[MAX_IMAGE_DIMENSION*MAX_IMAGE_DIMENSION];
    char mvsTempImageName[50];
    unsigned int mvsTempImageWidth;
    unsigned int mvsTempImageHeight;
    struct mvsTempImageRec *next;
  }; 

extern  struct mvsTempImageRec *mvsTempImageRecHead, *mvsTempImageRecTail;


/* Byte Order Test Macro */
 
  visGLOBAL short mvsByteOrderTestWord visINIT(1);
# define LEAST_SIGNIFICANT_BYTE_FIRST (*(char *)&mvsByteOrderTestWord)


/* Arbitrary Start Codes */

/* These are used to recognize with high confidence the beginning of various */
/* sections of the file stream (provideds additional error detection).       */

  visGLOBAL unsigned int   mvsImageFileStartCode   visINIT(0x12345678);
  visGLOBAL unsigned int   mvsImageRecordStartCode visINIT(0x87654321);
  visGLOBAL unsigned short mvsRecIdRecordStartCode visINIT(0xFEED);


/**************************************************************************
*                     GLOBAL COLOR STRUCTURES
**************************************************************************/

typedef struct _MvsVisualInfoRecord {
    int            class;
    int            mvs_color_mode;
    Visual         *visual;
    Colormap       colormap_id;
    int            colormap_size;
    int            bits_per_rgb;
    unsigned long  red_mask,green_mask,blue_mask; /* for Direct or True color */
    unsigned int   depth;
    unsigned long  depth_mask;
} MvsVisualInfoRecord;

#define ColorModeNone           0
#define ColorModeBlackAndWhite  1
#define ColorModeGreyScale      2
#define ColorModePseudoColor    3

visGLOBAL MvsVisualInfoRecord mvsVisualInfo;

typedef struct _MvsColorTableRecord {
    unsigned short  red,green,blue;
    String name1,name2;
} MvsColorTableRecord;

typedef struct _MvsPixmapTableRecord {
    String name;
    Pixmap pixmap;
    short width,height;
    char content[16];
} MvsPixmapTableRecord;

#include "mvscolor.h" /* GLOBAL color_table_record color_table[] = {...}; */

/*** Color Alloc Info ***/

/*
 This array is managed as a lazy sparse array - that is, all entries are
 initialized with a special code initially (alloc cnt = 0).  Then when a color
 is allocated the first entry in the list starting from the beginning that
 is free (contains the special code) is used.  The entry is never moved during
 its entire life cycle so that the color can be referenced by its location in
 this array (color_alloc_num) rather than by its pixel value.  Each time a new
 allocation of the color is made, its alloc_cnt is incremented.  Each time
 an allocation is freed, alloc_cnt is decremented.  When alloc_cnt reaches
 0 than it is placed on the available list (by virtue of its alloc_cnt equalling
 0) and its cell may later be reused for another color allocation.
*/

typedef struct _MvsColorAllocInfoRecord {
    Pixel pixel;
    unsigned short red,green,blue;
    int alloc_cnt;
} MvsColorAllocInfoRecord;

typedef struct _MvsUniqueColorAllocInfoRecord {
    short in_use;
    MvsWidgetInfo widget_info;
    short color_ref;
} MvsUniqueColorAllocInfoRecord;

visGLOBAL MvsColorAllocInfoRecord mvsColorAllocInfo[MAX_COLORS];

visGLOBAL MvsUniqueColorAllocInfoRecord mvsUniqueColorAllocInfo[MAX_COLORS]
                                                            visINIT({ 0 });

visGLOBAL XColor color_cache[MAX_COLORS] visINIT({ 0 });
visGLOBAL int cache_pointer visINIT({ 0 });

#include "vis_protos.h"

#endif
/* VISLIB - don't put anything after this! */
