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
/*   $XConsortium: vis_protos.h /main/7 1995/07/14 11:51:36 drk $ */
/***********************************************************************
  	@(#)vis_protos.h	1.3.1.1	Date:1/22/91
  	Author: SJS
	History:
            09/14/90 Create
	Calls:

	Summary:
************************************************************************/
/**************************************************************************
*                               vislib/visual                             *
**************************************************************************/

extern void mvsGetXisStateHash PARAMS((char hash[], short *num_bytes));

extern void mvsPushHashBits PARAMS((char hash[], int *byte_num, int *bit_num,
                                    long bits, int num_bits));

extern Status mvsAllocColor PARAMS((Pixel pixel));

extern Status mvsAllocNamedColor PARAMS((char name[], Pixel *pixel));

extern void mvsFreeColor PARAMS((Pixel pixel));

extern Pixel mvsAllocUniqueColor PARAMS((MvsWidgetInfoRecord *widget_info,
					 int	color_ref));

extern void mvsFreeUniqueColor PARAMS((MvsWidgetInfoRecord *winfo,
				       Pixel pixel));

extern void mvsCheckNamedColors PARAMS((void));

extern int mvsColorAllocNumFromPixel PARAMS((Pixel pixel));

extern void fwrite_short PARAMS((FILE *fd, unsigned int value));

extern unsigned short fread_short PARAMS((FILE *fd));

extern void fwrite_long PARAMS((FILE *fd, unsigned long value));

extern int mvsStringCompare PARAMS((String str1, String str2));

extern Status mvsAllocColorStruct PARAMS((XColor color));

extern unsigned long fread_long PARAMS((FILE *fd));

extern int findstr PARAMS((char in_string[], char search_string[]));

extern void mvsCalcVisualHash PARAMS((MvsWidgetInfoRecord *widget_info,
                                      unsigned char hash[], short *num_bytes));

extern void mvsCheckVisual PARAMS((MvsWidgetInfoRecord *widget_info));

extern void mvsCloseVisual PARAMS((void));

extern void mvsComparePixmapInteractive PARAMS((MvsWidgetInfoRecord *wid_info));

extern void mvsComparePixmapBatch PARAMS((MvsWidgetInfoRecord *widget_info));

extern int mvsCompareIDs PARAMS((unsigned char str1[], short len1,
                                 unsigned char str2[], short len2));

extern void mvsCompressWrite PARAMS((FILE *fp, unsigned char buffer[],
                                     int size));

extern void mvsDecompressRead PARAMS((FILE *fp, unsigned char buffer[],
                                      int size));

extern XImage *mvsGetPixmapImage PARAMS((Pixmap pixmap));

extern void mvsInitFonts PARAMS((void));

extern int mvsGetVisMode PARAMS((void));

extern void mvsOpenVisual PARAMS((void));

extern void mvsSetVisualMode PARAMS((int mode));

extern XmStringCharSet mvsGetCharSet PARAMS((char *requested_char_set));

extern void mvsGeneratePixmap PARAMS((MvsWidgetInfoRecord *widget_info));

extern short mvsGetCh PARAMS((FILE *fp, unsigned char inbuf[],
                              unsigned char *ch));

extern int visLookUpColorCache PARAMS((XColor *color));

extern void visAddToColorCache PARAMS((XColor *color));

extern void visClearColorCache PARAMS((void));


/**************************************************************************
*                               vislib/enums                              *
**************************************************************************/

extern MvsArgVal EnumPixel PARAMS((int n));
extern MvsArgVal EnumPixmap PARAMS((int n));


/**************************************************************************
*                               vislib/other                              *
**************************************************************************/

visGLOBAL void (*visTraceMsg)() visINIT(NULL); /* Trace message func */

visGLOBAL void (*visWarningMsg)() visINIT(NULL); /* Warning message func */

visGLOBAL void (*visErrorMsg)() visINIT(NULL); /* Error message func */

