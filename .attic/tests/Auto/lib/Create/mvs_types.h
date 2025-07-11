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
/*   $XConsortium: mvs_types.h /main/7 1995/07/14 11:26:27 drk $ */
/***********************************************************************
  	@(#)mvs_types.h	1.2.1.1	Date:1/22/91
  	Author: SJS
	History:
            09/14/90 Create, move types to separate file(help break up)
	Calls:

	Summary:
************************************************************************/
/**************************************************************************
*                       RESOURCE TYPES
**************************************************************************/

#ifndef MVS_TYPES
#define MVS_TYPES
/**** Define the ubiquitous MvsArgVal ****/

/* 
This is the fundamental type of each resource. It is the longest of the
following types: unsigned long, function pointer, char *.  This is defined
by XtArgVal in Intrinsic.h.  Basically we use this type to hold all the
values and pointers to values and structures that are returned by XtGetValues.
XtSetValues reads the 4+ byte value properly as is while XtGetValues returns
a value the length of the type of the resource and does not properly zero out
the rest of the bytes. Thus we must call a conversion routine to type cast
each value returned by XGetValues to XtArgVal.  

Note also that we (unlike with real resources) store the list of callback
records assigned to the resource at creation time and subsequently in
calls to mvsSetValues().  We had to store these somewhere, why not here? 
By making mvsSetValues() call XAddCallback(s) for us, we can test callbacks
just like any other resource in the resource info array (using the appropriate
enum function to enumerate a series of valid values which can be stored in
the resource). We cannot however read back the callbacks but can still check
them via a few tricks in the mvsCompareResources code.
*/

typedef XtArgVal MvsArgVal;

/*
 Note: MvsArgVal is defined merely to distinguish resource value usage intent.
*/



/***** Clarify a Few Resource Types (for enum classification, etc.)       *****/
/***** XtOrderProc is in the AES, but in (CompositeP.h)PRIVATE Xt include *****/


typedef unsigned char unsigned_char;
typedef unsigned int unsigned_int;
typedef int Integer;
typedef Widget Widget_ref;
typedef Screen *Pointer;
#ifdef DECLARE_MVSLIB_GLOBALS_HERE
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/CompositeP.h>
#endif


/************** Define the Type_info Array **************/

/*
This is used to map the type code (usually referenced from the
resource_info[] table) to the reference type (gives some clue how
to handle the resource for purposes of copying, comparing, etc.), the
size in bytes (could be used in the resource converstion function in
place of the switched type casts possibly, though I don't think this
would be quite as portable), and the enumeration function which is
a general function with the following formal definition:

     MvsArgVal func(int value)

such that when you pass it a -1 it returns the number of valid enumerations
you can do on it.  Then when you pass it any value from 0 to the maximum value,
it returns a valid MvsArgVal value which you can use to try out on the
resource.
*/

typedef struct _MvsTypeInfoRecord {
    int ref_type;
    int size;
    MvsArgVal (*enum_func)();
    int nbits;
}  MvsTypeInfoRecord;


/**************************************************************************
*                     ENUMERATION TYPE DEFINE
**************************************************************************/

typedef struct {
    MvsArgVal value;
    char str_val[MAX_STRING_VAL];
} MvsEnumType;


/**************************************************************************
*                     BEHAVIOR TABLE INFO
**************************************************************************/


typedef struct {
    char names[MAX_RESOURCE_NAME_LENGTH];
    int num_enums;
    int mod_num;
    int div_num;
} MvsBehavior_Table;


/**************************************************************************
*                       RESOURCE INFO RECORD
**************************************************************************/

/*
This record is used to define the resource_info array in the widget specific
module. It is indexed by the resource number starting with the highest 
inheritence class down to the lowest most local class listed in the same
order they are listed in the AES.

It contains the name of each resource used by this widget, a type code
which corresponds to the types listed previously in this file (i.e., 
T_xxx), a boolean code indicating the meaning of the next field, the
default value which if needs_converting == True may be set instead to
one of the codes define above,
*/

typedef struct _MvsResourceInfoRecord {
    char       *name;
    int        type_code;
    char       needs_converting;
    MvsArgVal  default_value;
    char       affects_visual;
    MvsArgVal  (*enum_func)();
    int        access;
    int        is_related;
} MvsResourceInfoRecord;

typedef struct _MvsIntResInfoRecord {
    char       *name;
    int        type_code;
    char       needs_converting;
    MvsArgVal  default_value;
    int        visual;
} MvsIntResInfoRecord;



/**************************************************************************
*                     WIDGET CLASS INFO
**************************************************************************/

typedef struct _MvsWidgetClassInfo {
    void            (*proc_SetExpectedActions)();/* Ptr to S.E.A. func      */
    Widget          (*proc_CreateWidget)();      /* Ptr to create func      */
    void            (*proc_TestBehavior)();      /* Ptr to test behaviorfunc*/
    char            *class_name;                 /* Name of class ->debug   */
    MvsResourceInfoRecord      *resource_info;   /* Ptr to static resrc info*/
    short                      num_resources;    /* Number of resources     */
    MvsIntResInfoRecord        *intres_info;     /* static info on intres   */
    short                      num_intres;       /* Number of internal res  */
    Arg             common_args[MAX_COMM_RES];   /* Common start args       */
    short           num_common_args;             /* number of arguments     */
    short           res_color_ref[MAX_COLOR_REF];/* Array of color res nums */
    short           res_pixmap_ref[MAX_COLOR_REF];/* Array of pixmap res nums*/
    short           num_res_colors;              /* Number of color resourcs*/
} MvsWidgetClassInfo;




/**************************************************************************
*                     WIDGET INFO 
**************************************************************************/
 
  typedef struct _MvsWidgetInfoRecord {
    int                         in_use;      /* 0=free, 1=used               */
    int                         being_destroyed; /* 0=no, 1=yes              */
    int                         c_parent;    /* 0=normal, 1=convenience func */
    MvsWidgetClassInfo          *widget_class_info;
                                             /* Static widget class info     */
    MvsArgVal                   mvs_resources[MAX_RESOURCES];
                                             /* Local copy of all resources  */
    MvsArgVal			int_resources[MAX_INTRES];
					     /* Internal resources */
    XImage                      *res_image[MAX_COLORS];
                                             /* Pntr to image assoc w/pixmap */
    Widget                       widget;     /* Xt widget id                 */
    Window                       window;     /* X window id                  */
    struct _MvsWidgetInfoRecord *parent;     /* parent widget's info         */
    struct _MvsWidgetInfoRecord *first_child;/* first child                  */
    struct _MvsWidgetInfoRecord *next_sibling;/* next sibl top-bot stack order*/
    short                        unique_color_ref[10];
    short                        x,y,width,height; /* Geometry of widget     */
    short                        border_width;
    short                        callbacks_setup; /* True or False           */
    short                        callback_ref[MAX_CALLBACK_REF];
                                             /* Reference callback_info[]    */
                                             /* in order of cbs in res_info. */

/* Automation Added */

    struct _MvsUniqueColorAllocInfoRecord2 {
				 short in_use;
				 struct _MvsWidgetInfoRecord *widget_info;
				 short color_ref;
				 }
				 widgetUniqueColorInfo[MAX_UNIQUE_COLORS];
					     /* added for Automation - unique
						color per widget basis instead
						of whole hierarchy */

/* End Automation Added */

  } MvsWidgetInfoRecord;

  typedef MvsWidgetInfoRecord *MvsWidgetInfo;


/******************************************************************************
                CALLBACK MANAGEMENT DECLARATIONS
******************************************************************************/

typedef struct _MvsCallbackInfoRecord {
  int                 called_cnt;
  int                 reason;
  int                 flag;
  int                 in_use;
  int                 resource_num;
  MvsWidgetInfo       widget_info;
  Widget              widget_id_returned;
  int                 client_data_returned;
  int                 client_data_expected;
  int                 call_data_returned;
} MvsCallbackInfoRecord;

/**************************************************************************
*                     GLOBAL FONT INFO STRUCTURE
**************************************************************************/

typedef struct _MvsFontInfoRecord {
    XFontStruct     *font_struct;  /* Returned from XmLoadQueryFont */
    XmStringCharSet char_set_name; /* For use with XmFontListCreate */
    char            bits_per_char; /* 8 or 16 */
    char            mono_spaced;   /* True=Mono spaced font, False = variable */
    char            left_to_right; /* True= Direction in left to right */
    char            iso_latin1;    /* True= Roman character set */
    char            *font_name;    /* For use with XmLoadQueryFont */
} MvsFontInfoRecord;


typedef struct _ClassRef {
    WidgetClass         motif_class;	/* xm*Class type of references */
    int                 mvs_class;	/* mvsXm*Class type of references */
    MvsWidgetClassInfo  *inst_class;	/* iXm*Class type of references */
} ClassRef;

/*
ClassRef        MotifClasses[MAXCLASS];
*/

typedef union _ClassRefEle {
    WidgetClass         motif_class;
    int                 mvs_class;
    MvsWidgetClassInfo  *inst_class;
} ClassRefEle;

#endif
/* TYPES - don't put anything after this! */
