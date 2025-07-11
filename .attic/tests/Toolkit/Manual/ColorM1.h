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

#include <string.h>

#include <Mrm/MrmAppl.h>


#define NULLW ((Widget)0)
#define NULLCB ((XmAnyCallbackStruct *)0)

#define MAX_ENTRY_SIZE 500
/* scales show between 0 and 255, color values are between 0 and 65535 */
#define COLOR_PCT_MULTIPLIER 256

#define XtNfontText "fontText"
#define XtNrgbTextFile "rgbTextFile"
#define XtNmreTypeFile "mreTypeFile"
#define XtNprintHierarchy "printHierarchy"
#define XtNmergeOnSave "mergeOnSave"
#define XtNinternalPixmaps "internalPixmaps"
#define XtCFontText "FontText"
#define XtCRgbTextFile "RgbTextFile"
#define XtCMreTypeFile "MreTypeFile"
#define XtCPrintHierarchy "PrintHierarchy"
#define XtCMergeOnSave "MergeOnSave"
#define XtCInternalPixmaps "InternalPixmaps"
#define XmStringDefCreate(s) XmStringLtoRCreate((s),XmSTRING_DEFAULT_CHARSET)

typedef struct {
  Pixel fg;
  char *font_text;
  char *rgb_text_file;
  char *mre_type_file;
  Boolean print_hierarchy;
  Boolean merge_on_save;
  Boolean internal_pixmaps;
  } ApplicationData, *ApplicationDataPtr;

Colormap cmap;
Display *dpy;
Widget main_selection_box_id;
Widget popped_by;   
MrmHierarchy	s_MrmHierarchy;		
ApplicationData data;
Boolean no_colors_available;

