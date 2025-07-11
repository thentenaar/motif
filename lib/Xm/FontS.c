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
 *
 */
#define dbg() fprintf(stderr, "file: %s - XtWidgetToApplicationContext() on line: %d\n", __FILE__, __LINE__);
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>		/* May have to be strings.h on some systems. */

#if HAVE_STDINT_H
#include <stdint.h>
#elif HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#include <Xm/Xm.h>
#include <Xm/FontSP.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include <Xm/MenuShell.h>
#include <Xm/DropDown.h>
#include <Xm/ButtonBox.h>
#include <Xm/ExtP.h>
#include "Xm/XmI.h"

#define NUM_XLFD_DASHES 14
/* max number of fonts that can be returned (CARD16) */
#define TRY_FONTS 	(unsigned int) 65535

#define NUM_BUTTONS	30	/* The MAX number of encoding buttons. */

#define ANY_FAMILY	0	/* Don't change this, without fixing */
				/* ProcessXldfFontData */

#define GET_FONT_SIZE BUFSIZ

#define ALLOC_INC 		100
#define FAMILY_INC 		10
#define WIDGET_LIST_INC 	10

#define POINT_DIVIDE		10

#define DEFAULT_WEIGHT  	("medium")
#define BOLD_WEIGHT     	("bold")
#define DEFAULT_SLANT   	("r")
#define ITALIC_SLANT    	("i")
#define DEFAULT_FIXED_SPACING   ("c")
#define OTHER_FIXED_SPACING 	("m")
#define PROPORTIONAL_SPACING  	("p")

#define STAR_STRING	("*")
#define ANY_ENCODING	("*-*")

#define FAMILY_COLUMNS    20
#define VISIBILE_FAMILIES  11

static short GValidSizes[] = {
    2, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,22,23,24,26,28,36,48,72
};

/*
 * A font may only be scaled in one resolution, not all resolutions.
 */

#define IsScaled(fsw, fam) (XmFontS_use_scaling((fsw)) &&                     \
			    ((CheckFlag(XmFontS_user_state((fsw)), DPI_75) && \
			      CheckFlag((fam)->state, SCALED_75))          || \
			     (CheckFlag(XmFontS_user_state((fsw)), DPI_100)&& \
			      CheckFlag((fam)->state, SCALED_100))))

/************************************************************
 *       GLOBAL DECLARATIONS
 ************************************************************/

/************************************************************
 *     Private functions - for creating children widgets.
 ************************************************************/

static void CreateChildren(XmFontSelectorWidget, ArgList, Cardinal);
static void CreateTopWidgets(XmFontSelectorWidget, Widget, ArgList, Cardinal);
static void CreateMiddleArea(XmFontSelectorWidget, ArgList, Cardinal);
static void CreateSpacingBox(XmFontSelectorWidget, Widget, ArgList, Cardinal);
static void CreateFontChoiceBox(XmFontSelectorWidget, Widget,ArgList,Cardinal);
static void CreateResolutionBox(XmFontSelectorWidget, Widget,ArgList,Cardinal);
static void CreateFamilyBox(XmFontSelectorWidget, Widget, ArgList, Cardinal);
static void CreateSizesBox(XmFontSelectorWidget, Widget, ArgList, Cardinal);
static void CreateBoldItalicBox(XmFontSelectorWidget,Widget,ArgList, Cardinal);
static void CreateOtherChoiceBox(XmFontSelectorWidget,
				 Widget, ArgList, Cardinal);

/************************************************************
 *     Private functions.
 ************************************************************/

static void LoadFontData(Widget, FontData **, int *, String **, int *);
static void ProcessXlfdFontData(XmFontSelectorWidget,
				FontInfo *, FontData *, int);
static void SortOtherFontData(FontInfo *, String *, int);
static void AddToOtherList(char *, String **, int *, int *);
static void SetNonStringData(FontData *), SetFlag(Flag *, Flag, Boolean);
static void StoreString(register String, register String, register int);
static void SetLongFlag(LongFlag *, LongFlag, Boolean);
static void UpdateExistingFamily(FamilyInfo *, FontData *);
static void FillNewFamily(FamilyInfo *, FontData *);
static void SetResolution(XmFontSelectorWidget, FontData *);
static void DisplayCurrentFont(XmFontSelectorWidget, String);
static void UpdateBoldItalic(XmFontSelectorWidget);
static void UpdateFixedProportional(XmFontSelectorWidget);
static void UpdateFamilies(XmFontSelectorWidget);
static void UpdateSizes(XmFontSelectorWidget);
static void SetComboValue(Widget, String), UnsetSiblings(Widget);
static void DisplayUserError(XmFontSelectorWidget, String);
static void SetDisplayedFont(XmFontSelectorWidget, String);
static void AddToXlfdOnlyList(XmFontSelectorWidget, Widget);
static void AddToXlfdSensitiveList(XmFontSelectorWidget, Widget);
static void ChangeMode(XmFontSelectorWidget, Boolean, Boolean);
static void SetOtherList(XmFontSelectorWidget, Boolean);
static void AddToXlfdList(XmFontSelectorWidget,
			  char *, FontData **, int *, int *);

static Widget CreateEncodingMenu(XmFontSelectorWidget,
				 Widget, ArgList, Cardinal);

static FontInfo * LoadFontInfo(XmFontSelectorWidget);

static int FindResolution(Widget), CmpStrings(const void *, const void *);

static Boolean CheckFlag(Flag, Flag), CheckLongFlag(LongFlag, LongFlag);
static Boolean FillData(XmFontSelectorWidget, FontData *, char *);
static Boolean IsXlfdFont(register char *);
static Boolean CheckEncoding(XmFontSelectorWidget, FamilyInfo *);

static FamilyInfo *FindFamily(register XrmQuark, FamilyInfo *, int);

static String BuildFontString(XmFontSelectorWidget, FontData *, String, int);

static LongFlag SizeMapping(short);

/************************************************************
 *  Callbacks
 ************************************************************/

static void ToggleMiddlePane(Widget, XtPointer, XtPointer);
static void ToggleNameWindow(Widget, XtPointer, XtPointer);
static void FamilyChanged(Widget, XtPointer, XtPointer);
static void SizeChanged(Widget, XtPointer, XtPointer);
static void ChangeEncoding(Widget, XtPointer, XtPointer);
static void ToggleScaling(Widget, XtPointer, XtPointer);
static void ToggleBold(Widget, XtPointer, XtPointer);
static void ToggleItalic(Widget, XtPointer, XtPointer);
static void Toggle75DPI(Widget, XtPointer, XtPointer);
static void Toggle100DPI(Widget, XtPointer, XtPointer);
static void ToggleBothDPI(Widget, XtPointer, XtPointer);
static void ToggleProportional(Widget, XtPointer, XtPointer);
static void ToggleFixed(Widget, XtPointer, XtPointer);
static void ToggleBothSpacing(Widget, XtPointer, XtPointer);
static void XlfdMode(Widget, XtPointer, XtPointer);
static void OtherMode(Widget, XtPointer, XtPointer);
static void RemoveUserError(Widget, XtPointer, XtPointer);

/************************************************************
 *  Semi-Public functions
 ************************************************************/

static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void GetValuesHook(Widget, ArgList, Cardinal *);

static void Destroy(Widget);
static void Initialize(Widget, Widget, ArgList, Cardinal*);
static void ClassPartInitialize(WidgetClass w_class);
static void ClassInitialize();

/*  Resource definitions for Subclasses of Primitive */

#define SAMPLE \
("abcdefghijklmonpqrstuvwxyz\n\
ABCDEFGHIJKLMONPQRSTUVWXYZ\n\
1234567890\n!@#$%^&*()")

/************************************************************
 *       STATIC DECLARATIONS
 ************************************************************/

/*
 * This table is the list of resolutions of fonts that are supported.
 * most machines have only 75 and 100 dpi fonts, so we only find which
 * of those we are closest to with FindResolution().
 */

static int resolutions[] = { 75, 100 };

static XtResource resources[] =
{
  {
    XmNspacing, XmCSpacing, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(XmPanedRec, paned.internal_bw),
    XmRImmediate, (XtPointer) 2
  },

  {
    XmNseparatorOn, XmCSeparatorOn, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmPanedRec, paned.separator_on),
    XmRImmediate, (XtPointer) True
  },

  {
    XmNmarginHeight, XmCMargin, XmRVerticalDimension,
    sizeof(Dimension), XtOffsetOf(XmPanedRec, paned.margin_height),
    XmRImmediate, (XtPointer) 0
  },

  {
    XmNcurrentFont, XmCString, XmRString,
    sizeof(String), XtOffsetOf(XmFontSelectorRec, fs.current_font),
    XmRImmediate, (XtPointer)NULL
  },

  {
    XmNshowFontName, XmCBoolean, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmFontSelectorRec, fs.show_font_name),
    XmRImmediate, (XtPointer) False
  },

  {
    XmNuseScaling, XmCBoolean, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmFontSelectorRec, fs.use_scaling),
    XmRImmediate, (XtPointer) True
  },

  {
    XmNtextRows, XmCTextRows, XmRDimension,
    sizeof(Dimension), XtOffsetOf(XmFontSelectorRec, fs.text_rows),
    XmRImmediate, (XtPointer) 8
  },

  {
    XmNvalueChangedCallback, XmCCallback, XmRCallback,
    sizeof(XtCallbackList), XtOffsetOf(XmFontSelectorRec, fs.font_changed_callback),
    XmRImmediate, (XtPointer) NULL
  },

  {
    XmNsampleText, XmCSampleText, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmFontSelectorRec, fs.strings.sample_text),
    XmRString, (XtPointer) SAMPLE
  },

  {
    XmNanyString, XmCAnyString, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmFontSelectorRec, fs.strings.any),
    XmRString, (XtPointer) "Any"
  },

  {
    XmNanyLowerString, XmCAnyLowerString, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmFontSelectorRec, fs.strings.lower_any),
    XmRString, (XtPointer) "any"
  },

  {
    XmNfamilyString, XmCFamilyString, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmFontSelectorRec, fs.strings.family),
    XmRString, (XtPointer) "Family"
  },

  {
    XmNsizeString, XmCSizeString, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmFontSelectorRec, fs.strings.size),
    XmRString, (XtPointer) "Size"
  },

  {
    XmNboldString, XmCBoldString, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmFontSelectorRec, fs.strings.bold),
    XmRString, (XtPointer) "Bold"
  },

  {
    XmNitalicString, XmCItalicString, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmFontSelectorRec, fs.strings.italic),
    XmRString, (XtPointer) "Italic"
  },

  {
      XmNoptionString, XmCOptionString, XmRXmString,
      sizeof(XmString), XtOffsetOf(XmFontSelectorRec, fs.strings.option),
      XmRString, (XtPointer) "Options..."
  },

  {
    XmNbothString, XmCBothString, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmFontSelectorRec, fs.strings.both),
    XmRString, (XtPointer) "Both"
  },

  {
    XmNmonoSpaceString, XmCMonoSpaceString, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmFontSelectorRec, fs.strings.mono_space),
    XmRString, (XtPointer) "Fixed Width Fonts"
  },

  {
    XmNpropSpaceString, XmCPropSpaceString, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmFontSelectorRec, fs.strings.prop_space),
    XmRString, (XtPointer) "Proportional Fonts"
  },

  {
    XmNotherString, XmCOtherString, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmFontSelectorRec, fs.strings.other_font),
    XmRString, (XtPointer) "Other Fonts"
  },

  {
    XmNxlfdString, XmCXlfdSpaceString, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmFontSelectorRec, fs.strings.xlfd),
    XmRString, (XtPointer) "Xlfd Fonts"
  },

  {
    XmN75DPIstring, XmC75DPIString, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmFontSelectorRec, fs.strings.dpi_75),
    XmRString, (XtPointer) "75 dpi"
  },

  {
    XmN100DPIstring, XmC100DPIString, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmFontSelectorRec, fs.strings.dpi_100),
    XmRString, (XtPointer) "100 dpi"
  },

  {
    XmNscalingString, XmCScalingString, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmFontSelectorRec, fs.strings.scaling),
    XmRString, (XtPointer) "Use Font Scaling"
    },

  {
    XmNshowNameString, XmCShowNameString, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmFontSelectorRec, fs.strings.show_name),
    XmRString, (XtPointer) "Show Font Name"
  },

    /*
     * Stuff to set the valid charset/encoding pairs.
     */

  {
    XmNencodingString, XmCEncodingString, XmRXmString,
    sizeof(XmString), XtOffsetOf(XmFontSelectorRec, fs.strings.encoding_only),
    XmRString, (XtPointer) "Encoding"
  },

    /*
     * NOTE: This resource value must end in a comma because the resource
     *       converter is broken in Motif1.1.4.
     */

  {
    XmNencodingList, XmCEncodingList, XmRStringTable,
    sizeof(String *), XtOffsetOf(XmFontSelectorRec, fs.strings.encoding_list),
    XmRString, (XtPointer) "iso8859-1,"
  },

  {
    XmNdefaultEncodingString, XmCDefaultEncodingString, XmRString,
    sizeof(String), XtOffsetOf(XmFontSelectorRec, fs.encoding),
    XmRString, (XtPointer) "iso8859-1"
  }
};

static XmSyntheticResource syn_resources[] =
{
  {
    XmNspacing, sizeof(Dimension),
    XtOffsetOf(XmPanedRec, paned.internal_bw),
    _XmFromPanedPixels, (XmImportProc) _XmToPanedPixels
  },

  {
    XmNmarginHeight, sizeof(Dimension),
    XtOffsetOf(XmPanedRec, paned.margin_height),
    XmeFromVerticalPixels, (XmImportProc) XmeToVerticalPixels
  }
};

static XtResource subresources[] =
{
  {
    XmNshowSash, XmCShowSash, XmRBoolean,
    sizeof(Boolean), XtOffsetOf(XmPanedConstraintsRec, paned.show_sash),
    XmRImmediate, (XtPointer) False
  }
};

#define SuperClass ((ConstraintWidgetClass) &xmPanedClassRec)

XmFontSelectorClassRec xmFontSelectorClassRec = {
   {
/* core class fields */
    /* superclass         */   (WidgetClass) SuperClass,
    /* class name         */   "XmFontSelector",
    /* size               */   sizeof(XmFontSelectorRec),
    /* class_initialize   */   ClassInitialize,
    /* class_part init    */   ClassPartInitialize,
    /* class_inited       */   FALSE,
    /* initialize         */   Initialize,
    /* initialize_hook    */   NULL,
    /* realize            */   XtInheritRealize,
    /* actions            */   NULL,
    /* num_actions        */   0,
    /* resources          */   (XtResourceList)resources,
    /* resource_count     */   XtNumber(resources),
    /* xrm_class          */   NULLQUARK,
    /* compress_motion    */   TRUE,
    /* compress_exposure  */   TRUE,
    /* compress_enterleave*/   TRUE,
    /* visible_interest   */   FALSE,
    /* destroy            */   Destroy,
    /* resize             */   XtInheritResize,
    /* expose             */   NULL,
    /* set_values         */   SetValues,
    /* set_values_hook    */   NULL,
    /* set_values_almost  */   XtInheritSetValuesAlmost,
    /* get_values_hook    */   GetValuesHook,
    /* accept_focus       */   NULL,
    /* version            */   XtVersion,
    /* callback_private   */   NULL,
    /* tm_table           */   XtInheritTranslations,
    /* query_geometry	  */   XtInheritQueryGeometry,
    /* display_accelerator*/   XtInheritDisplayAccelerator,
    /* extension          */   NULL
   }, {
/* composite class fields */
    /* geometry_manager   */   XtInheritGeometryManager,
    /* change_managed     */   XtInheritChangeManaged,
    /* insert_child       */   XtInheritInsertChild,
    /* delete_child       */   XtInheritDeleteChild,
    /* extension          */   NULL
   }, {
/* constraint class fields */
    /* subresources       */   subresources,
    /* subresource_count  */   XtNumber(subresources),
    /* constraint_size    */   sizeof(XmFontSelectorConstraintsRec),
    /* initialize         */   NULL,
    /* destroy            */   NULL,
    /* set_values         */   NULL,
    /* extension          */   NULL
   }, {
/* manager_class fields   */
      XtInheritTranslations,                    /* translations           */
      syn_resources,	                        /* get resources          */
      XtNumber(syn_resources),    		/* num get_resources      */
      NULL,                			/* get_cont_resources     */
      0,				        /* num_get_cont_resources */
      XmInheritParentProcess,                   /* parent_process         */
      NULL,                                     /* extension              */
  }, {
      /* paned_class fields */
      NULL,                                     /* extension                 */
  }, {
      /* font selector class fields */
      NULL,                                     /* extension                 */
  }
};

WidgetClass xmFontSelectorWidgetClass = (WidgetClass) &xmFontSelectorClassRec;

/***********************************************************
 *
 * Private Functions.
 *
 ************************************************************/

/*	Function Name: LoadFontInfo
 *	Description: Loads the font information from the current screen.
 *	Arguments: fsw - the font selector widget.
 *	Returns: the font information.
 */

static FontInfo *
LoadFontInfo(XmFontSelectorWidget fsw)
{
    FontInfo *font_info = (FontInfo *) XtMalloc(sizeof(FontInfo));
    FontData *xlfd_font_data;
    String *other_fonts;
    int xlfd_num_fonts, other_num_fonts;

    font_info->resolution = FindResolution((Widget) fsw);
    LoadFontData((Widget) fsw, &xlfd_font_data, &xlfd_num_fonts,
		 &other_fonts, &other_num_fonts);

    ProcessXlfdFontData(fsw, font_info, xlfd_font_data, xlfd_num_fonts);
    SortOtherFontData(font_info, other_fonts, other_num_fonts);

    return(font_info);
}

/*	Function Name: LoadFontData
 *	Description: Loads the font data from the X Server, sorts it into
 *                   xlfd and non xlfd fonts, and sorts out the xlfd info.
 *	Arguments: w - any widget on the screen.
 * RETURNED        xlfd_data, num_xlfd - The xlfd font data.
 * RETURNED        other_fonts, num_other - The list of non xlfd fonts.
 *	Returns: none.
 */

static void
LoadFontData(Widget w, FontData **xlfd_data, int *num_xlfd,
	     String **other_fonts, int *num_other)
{
    XmFontSelectorWidget fsw = (XmFontSelectorWidget) w;
    char ** fonts, **ptr;
    int i, num, xlfd_alloc, other_alloc;

    *xlfd_data = NULL;
    *other_fonts = NULL;
    *num_xlfd = *num_other = xlfd_alloc = other_alloc = 0;

    fonts = XListFonts(XtDisplay(w), "*", TRY_FONTS, &num);

    /*
     * Loop the all the fonts and add each one to the proper list.
     */

    if (fonts != NULL)  {
	for (ptr = fonts, i = 0; i < num; i++, ptr++) {
	    if (IsXlfdFont(*ptr))
		AddToXlfdList(fsw, *ptr, xlfd_data, num_xlfd, &xlfd_alloc);
	    else
		AddToOtherList(*ptr, other_fonts, num_other, &other_alloc);
	}
	XFreeFontNames(fonts);
    }
}

/*	Function Name: ProcessXlfdFontData
 *	Description: Processes the Xlfd data, to come up with lists
 *                   of various attributes.
 *	Arguments: fsw - The Font Selector Widget.
 *                 font_info - the font information structure.
 *                 data, num - the xlfd data.
 *	Returns: none.
 *
 * This takes the raw XLFD data and builds lists of families and
 * sizes, and stores all the information in the font_info structure.
 */

static void
ProcessXlfdFontData(XmFontSelectorWidget fsw,
		    FontInfo *font_info, FontData *data, int num_data)
{
    int num, alloc, i;
    FamilyInfo *any_fam, *fam = NULL;
    FontData *top = data;
    String temp;

    /*
     * Leave space for the any family, filled in at the end of the function.
     *
     * NOTE:  This assumes that (ANY_FAMILY == 0)
     */

    num = 1;
    alloc = FAMILY_INC;
    fam = (FamilyInfo *) XtMalloc(sizeof(FamilyInfo) * alloc);
    fam->nameq = NULLQUARK;	/* Don't use this family yet. */

    /*
     * Set up the "Any" family.
     */

    for (i = 0; i < num_data; i++, data++) {
	register FamilyInfo *cur_family = FindFamily(data->familyq, fam, num);

	if (cur_family == NULL) {
	    if ( num >= alloc ) {
		alloc += FAMILY_INC;
		fam = (FamilyInfo *) XtRealloc((XtPointer) fam,
					       sizeof(FamilyInfo) * alloc);
	    }

	    FillNewFamily(fam + num, data);
	    num++;
	}
	else {
	    UpdateExistingFamily(cur_family, data);
	}
    }

    font_info->family_info = fam;
    font_info->num_families = num;

    any_fam = fam + ANY_FAMILY;
    temp = _XmGetMBStringFromXmString(ANY_STRING(fsw));
    any_fam->nameq = XrmStringToQuark(temp);
    XtFree(temp);
    any_fam->bold_nameq = XrmStringToQuark(BOLD_WEIGHT);
    any_fam->medium_nameq = XrmStringToQuark(DEFAULT_WEIGHT);
    any_fam->upright_nameq = XrmStringToQuark(DEFAULT_SLANT);
    any_fam->italic_nameq = XrmStringToQuark(ITALIC_SLANT);
    strcpy(any_fam->fixed_spacing, DEFAULT_FIXED_SPACING);

    any_fam->state = (Flag) 0;
    any_fam->sizes_75 = any_fam->sizes_100 = (LongFlag) 0;
    for (i = 1, fam++; i < num; i++, fam++) {
	any_fam->sizes_75 |= fam->sizes_75;
	any_fam->sizes_100 |= fam->sizes_100;
	any_fam->state |= fam->state;
    }
    any_fam->encodings = NULL;
    any_fam->encoding_alloc = 0;

    XtFree((XtPointer) top);
}

/*	Function Name: SortOtherFonts
 *	Description: Sorts the list of other fonts alphabetically
 *                   and stuffs them into the font data structure.
 *	Arguments: font_info - the font information.
 *                 list, num - the list of fonts.
 *	Returns: none
 */

static void
SortOtherFontData(FontInfo *font_info, String *list, int num)
{
    register int i = 0;

    qsort((void *) list, num, sizeof(String), CmpStrings);

    /*
     * Remove duplicates.
     */

    while (i < (num - 1)) {
	if (streq(list[i], list[i+1])) {
	    register int j;
	    register String *ptr;

	    XtFree((char*)list[i]);
	    ptr = list + i;
	    j = i;

	    while ( ++j < num) {
		ptr[0] = ptr[1];
		ptr++;
	    }

	    num--;	/*Something has been removed, decrement count*/
	}
	else
	    i++;
    }

    font_info->others = list;
    font_info->num_others = num;
}

/*	Function Name: CmpStrings
 *	Description: Compares two strings.
 *	Arguments: p1, p2 - two string pointers.
 *	Returns: none
 */

static int
CmpStrings(const void * p1, const void *p2)
{
    String *s1 = (String *) p1;
    String *s2 = (String *) p2;

    return(strcmp(*s1, *s2));
}

/*	Function Name: FindResolution
 *	Description: Finds the closest resolution
 *	Arguments: w - any widget on the proper screen.
 *	Returns: the closest allowable resolution to the real resolution.
 */

static int
FindResolution(Widget w)
{
    Screen *screen = XtScreen(w);
    register int i, xres, yres, min, pref;

    /*
     * there are 2.54 centimeters to an inch; so there are 25.4 millimeters.
     *
     *     dpi = N pixels / (M millimeters / (25.4 millimeters / 1 inch))
     *         = N pixels / (M inch / 25.4)
     *         = N * 25.4 pixels / M inch
     *
     *         Or in integer arithmitic
     *
     *         = (254 * N pixels / M inch + 5) / 10
     */

    /* This will get us the true pixels / inch.  That is probably
       not what we want */

    xres = (254 * WidthOfScreen(screen) / WidthMMOfScreen(screen) + 5) / 10;
    yres = (254 * HeightOfScreen(screen) / HeightMMOfScreen(screen) + 5) / 10;

    xres *= xres;
    yres *= yres;

    /*
     * Find the closest resolution by using a difference of squares.
     */

    min = 32000;
    pref = resolutions[0];	/* We have to have some default. */

    for (i = 0; i < XtNumber(resolutions); i++) {
	int diffx = (xres - resolutions[i] * resolutions[i]);
	int diffy = (yres - resolutions[i] * resolutions[i]);
	int tdiff;		/* the total difference. */

	/*
	 * Square 'em.
	 */

	tdiff = (diffx * diffx) + (diffy * diffy);
	if (min > tdiff) {
	    pref = resolutions[i];
	    min = tdiff;
	}
    }

    return(pref);
}

/*	Function Name: IsXlfdFont
 *	Description: Returns True if this is an LXFD font.
 *	Arguments: str - the font name to check.
 *	Returns: none.
 */

static Boolean
IsXlfdFont(register char * str)
{
    register int num_dashes = 0;
    register char c;

    for ( ; (c = *str) != '\0'; str++) {
	if (c == '-')
	    num_dashes++;
    }

    return(num_dashes == NUM_XLFD_DASHES);
}

/*	Function Name: AddToXlfdList
 *	Description: Adds a font to the list of xlfd fonts.
 *	Arguments: name - name of the font.
 *                 list, num - the list of xlfd fonts to add it to.
 *                 alloc - the number of allocated slots.
 *	Returns: none.
 */

static void
AddToXlfdList(XmFontSelectorWidget fsw,
	      char *name, FontData **list, int *num, int *alloc)
{
    FontData *current;
    if (*num >= *alloc) {
	*alloc += ALLOC_INC;
	*list = (FontData *) XtRealloc((XtPointer) *list,
				       sizeof(FontData) * *alloc);
    }

    current = (*list) + *num;

    /*
     * If filling the data succeeded, then accept this name, otherwise
     * just ignore it.
     */

    if (FillData(fsw, current, name))
	(*num)++;
}

/*	Function Name: AddToOtherList
 *	Description: Adds a font to the list of other fonts.
 *	Arguments: name - name of the font.
 *                 list, num - the list of other fonts to add it to.
 *                 alloc - the number of allocated slots.
 *	Returns: none.
 */

static void
AddToOtherList(char *name, String **list, int *num, int *alloc)
{
    if (*num >= *alloc) {
	*alloc += ALLOC_INC;
	*list = (String *) XtRealloc((XtPointer) *list,
				     sizeof(String) * *alloc);
    }

    (*list)[(*num)++] = XtNewString(name);
}

/*	Function Name: FillData(fsw, current, name);
 *	Description: Fills the current font data from an xlfd name.
 *	Arguments: fsw - The font selector widget.
 *                 current - the font data structure to fill.
 *                 name - an xlfd name of a font.
 *	Returns: False on failure.
 *
 * This name may have NOT *'s in it.
 */

#define GET_NEXT_FIELD(ptr)                             \
     if (((ptr) = (char *) strchr((ptr), '-')) == NULL) \
	 return(False);                                 \
     (ptr)++	/* skip hyphen */                       \

static Boolean
FillData(XmFontSelectorWidget fsw, FontData *current, char *name)
{
    char *ptr = name;
    char temp[BUFSIZ];

    current->state = 0;		/* Initialize value. */

    GET_NEXT_FIELD(ptr);	/* ptr == Foundry */
    GET_NEXT_FIELD(ptr);	/* ptr == Family */
    StoreString(ptr, temp, BUFSIZ - 1);

    if (streq(temp, STAR_STRING)) {
	String str = _XmGetMBStringFromXmString(ANY_STRING(fsw));
	strcpy(temp, str);
	XtFree((XtPointer) str);
    }

    current->familyq = XrmStringToQuark(temp);

    GET_NEXT_FIELD(ptr);	/* ptr == Weight */
    StoreString(ptr, temp, BUFSIZ - 1);
    current->weightq = XrmStringToQuark(temp);

    GET_NEXT_FIELD(ptr);	/* ptr == Slant */
    StoreString(ptr, current->slant, SLANT_LEN);

    GET_NEXT_FIELD(ptr);	/* ptr == SetWidth_Name */
    GET_NEXT_FIELD(ptr);	/* ptr == Add_Style_Name */
    GET_NEXT_FIELD(ptr);	/* ptr == Pixel Size */
    GET_NEXT_FIELD(ptr);	/* ptr == Point Size */
    StoreString(ptr, temp, BUFSIZ - 1);
    /*
     * the string '*' will default to 0 which is exactly what I want.
     */
    current->point_size = (short) atoi(temp);

    GET_NEXT_FIELD(ptr);	/* ptr == Resolution X */
    StoreString(ptr, temp, BUFSIZ - 1);
    /*
     * the string '*' will default to 0 which is exactly what I want.
     */
    current->resolution_x = (short) atoi(temp);

    GET_NEXT_FIELD(ptr);	/* ptr == Resolution Y */
    StoreString(ptr, temp, BUFSIZ - 1);
    /*
     * the string '*' will default to 0 which is exactly what I want.
     */
    current->resolution_y = (short) atoi(temp);

    GET_NEXT_FIELD(ptr);	/* ptr == Spacing */
    StoreString(ptr, current->spacing, SPACING_LEN);

    GET_NEXT_FIELD(ptr);	/* ptr == Average Width */
    GET_NEXT_FIELD(ptr);	/* ptr == Registry and Encoding */

    current->encoding = XrmStringToQuark(ptr);

    SetNonStringData(current);
    return(True);		/* success! */
}

#undef GET_NEXT_FIELD

/*	Function Name: SetNonStringData
 *	Description: Sets the non string part of the Font Data.
 *	Arguments: current - the font data we are working with.
 *	Returns: none.
 */

static void
SetNonStringData(FontData *current)
{
    char lower[BUFSIZ];

    /*
     * Now fill in the non string fields.
     */

    /*
     * If the weight_name field contains the words "bold" or "demi"
     * then this is a bold font.
     */

    XmCopyISOLatin1Lowered(lower, XrmQuarkToString(current->weightq));
    if ((strstr(lower, "bold") != NULL) || (strstr(lower, "demi") != NULL))
    	SetFlag(&(current->state), BOLD, True);

    /*
     * If the spacing is "p" then proportional, otherwize monospaced.
     */

    XmCopyISOLatin1Lowered(lower, current->spacing);
    if (strstr(lower, PROPORTIONAL_SPACING) != NULL)
    	SetFlag(&(current->state), PROPORTIONAL, True);

    /*
     * If the slant it "i" or "o" then this is an italic font.
     */

    XmCopyISOLatin1Lowered(lower, current->slant);
    if ((strchr(lower, 'i') != NULL) || (strchr(lower, 'o') != NULL))
    	SetFlag(&(current->state), ITALIC, True);

    /*
     * If the point_size is 0 then this is a scalable font.
     */

    if (current->point_size == 0) {
	if ((current->resolution_x == 75) && (current->resolution_y == 75))
	    SetFlag(&(current->state), SCALED_75, True);
	else if((current->resolution_x == 100)&&(current->resolution_y == 100))
	    SetFlag(&(current->state), SCALED_100, True);
	else if ((current->resolution_x == 0)&& (current->resolution_y == 0)) {
	    /*
	     * Sun sets all scaled fonts to have a resolution of 0x0
	     */
	    SetFlag(&(current->state), SCALED_75 | SCALED_100, True);
	}
    }
    /*
     * Check to see if resolution matches 75 or 100 dpi.
     */

    if ((current->resolution_x == 75) && (current->resolution_y == 75))
	SetFlag(&(current->state), DPI_75, True);
    else if ((current->resolution_x == 100) && (current->resolution_y == 100))
	SetFlag(&(current->state), DPI_100, True);
    else if ((current->resolution_x == 0) && (current->resolution_y == 0)) {
	/*
	 * Sun sets all scaled fonts to have a resolution of 0x0
	 */
	SetFlag(&(current->state), DPI_75 | DPI_100, True);
    }
}

/*	Function Name: StoreString
 *	Description: Stores a portion of an xlfd name into the
 *                   string varaible passed.
 *	Arguments: str - the portion of the string to store.
 *                 store - the string to store it in.
 *                 max_len - maximum length of the value.
 *	Returns: none.
 */

static void
StoreString(register String str, register String store, register int max_len)
{
    register int i;
    register int c;

    for (i = 0; i < max_len; i++, str++, store++) {
	if (((c = *str) == '-') || (c == '\0'))
	    break;
	*store = c;
    }
    *store = '\0';
}

/*	Function Name: CheckFlag
 *	Description: Checks to see if a flag value is set
 *	Arguments: state - the state variable containing the flag.
 *                 flag - the location of the flag.
 *	Returns: True if flag set, False otherwise
 */

static Boolean
CheckFlag(Flag state, Flag flag)
{
    return((state & flag) == flag);
}

/*	Function Name: CheckLongFlag
 *	Description: Checks to see if a long flag value is set
 *	Arguments: state - the state variable containing the flag.
 *                 flag - the location of the flag.
 *	Returns: True if flag set, False otherwise
 */

static Boolean
CheckLongFlag(LongFlag state, LongFlag flag)
{
    return((state & flag) == flag);
}

/*	Function Name: SetFlag
 *	Description: Sets a flag value
 *	Arguments: state - the state variable containing the flag.
 *                 flag - the location of the flag.
 *                 value - the value of the flag.
 *	Returns: none.
 */

static void
SetFlag(Flag *state, Flag flag, Boolean value)
{
    if (value)
	*state |= flag;
    else
	*state &= ~flag;
}

/*	Function Name: SetLongFlag
 *	Description: Sets a flag value that is stored in a long
 *	Arguments: state - the state variable containing the flag.
 *                 flag - the location of the flag.
 *                 value - the value of the flag.
 *	Returns: none.
 */

static void
SetLongFlag(LongFlag *state, LongFlag flag, Boolean value)
{
    if (value)
	*state |= flag;
    else
	*state &= ~flag;
}

/*	Function Name: FindFamily
 *	Description: Finds family associated with the font passed.
 *	Arguments: font - the font data.
 *                 list - the list of all families
 *                 num - the number of families.
 *	Returns: a pointer to the family this font is in or NULL.
 *
 */

static FamilyInfo *
FindFamily(register XrmQuark nameq, FamilyInfo *list, int num)
{
    register int i;

    for (i = 0; i < num; i++, list++) {
	if (list->nameq == nameq)
	    return(list);
    }

    return(NULL);
}

/*	Function Name: UpdateExistingFamily
 *	Description: Updates data, by adding another font to an
 *                   existion family.
 *	Arguments: fam - the family to update.
 *                 font - the font to add.
 *	Returns: none.
 */

static void
UpdateExistingFamily(FamilyInfo *fam, FontData *font)
{
    int i;

    if (CheckFlag(font->state, BOLD))
	fam->bold_nameq = font->weightq;
    else
	fam->medium_nameq = font->weightq;

    if (CheckFlag(font->state, ITALIC))
	fam->italic_nameq = XrmStringToQuark(font->slant);
    else
	fam->upright_nameq = XrmStringToQuark(font->slant);

    if (!streq(font->spacing, PROPORTIONAL_SPACING))
	strcpy(fam->fixed_spacing, font->spacing);

    if (font->point_size != 0) {
	if ((font->resolution_x == 75) && (font->resolution_y == 75))
	    SetLongFlag(&(fam->sizes_75), SizeMapping(font->point_size), True);
	else if ((font->resolution_x == 100) && (font->resolution_y == 100))
	    SetLongFlag(&(fam->sizes_100),SizeMapping(font->point_size), True);
    }

    fam->state |= font->state;	/* or in all bits. */

    for (i = 0; TRUE; i++)
    {
	if (i == fam->encoding_alloc)
	{
	    fam->encoding_alloc += NUM_BUTTONS;
	    fam->encodings =
		(XrmQuark *) XtRealloc((char *) fam->encodings,
				       sizeof(XrmQuark) * fam->encoding_alloc);
	    memset((char *) (fam->encodings + i), 0,
		   sizeof(XrmQuark) * NUM_BUTTONS);
	}

	if (fam->encodings[i] == font->encoding)
	    return;

	if (fam->encodings[i] == 0)
	{
	    fam->encodings[i] = font->encoding;
	    return;
	}
    }
}

/*	Function Name: FillNewFamily
 *	Description: Fills in the information for a new font family.
 *	Arguments: fam -  the family data.
 *                 font - the first font we found.
 *	Returns: none.
 */

static void
FillNewFamily(FamilyInfo *fam, FontData *font)
{
    fam->nameq = font->familyq;

    if (CheckFlag(font->state, BOLD)) {
	fam->bold_nameq = font->weightq;
    	fam->medium_nameq = NULLQUARK;
    }
    else {
    	fam->bold_nameq = NULLQUARK;
	fam->medium_nameq = font->weightq;
    }

    if (streq(font->spacing, PROPORTIONAL_SPACING))
	strcpy(fam->fixed_spacing, DEFAULT_FIXED_SPACING);
    else
	strcpy(fam->fixed_spacing, font->spacing);

    if (CheckFlag(font->state, ITALIC)) {
	fam->italic_nameq = XrmStringToQuark(font->slant);
    	fam->upright_nameq = NULLQUARK;
    }
    else {
    	fam->italic_nameq = NULLQUARK;
	fam->upright_nameq = XrmStringToQuark(font->slant);
    }

    fam->sizes_75 = fam->sizes_100 = 0;
    fam->state = font->state;

    if (font->point_size != 0) {
	if ((font->resolution_x == 75) && (font->resolution_y == 75))
	    SetLongFlag(&(fam->sizes_75), SizeMapping(font->point_size), True);
	else if ((font->resolution_x == 100) && (font->resolution_y == 100))
	    SetLongFlag(&(fam->sizes_100),SizeMapping(font->point_size), True);
    }

    fam->encoding_alloc = NUM_BUTTONS;
    fam->encodings = (XrmQuark *) XtCalloc(fam->encoding_alloc,
					   sizeof(XrmQuark));
    fam->encodings[0] = fam->encoding_alloc;
}

/*	Function Name: SizeMapping
 *	Description: Maps the actual size to the sizes bit field
 *	Arguments: size - the point size to map.
 *	Returns: 0 if the size does not map, otherwise the bit it maps to.
 */

static LongFlag
SizeMapping(short size)
{
    register int count;

    size /= POINT_DIVIDE;

    for (count = 0; count < XtNumber(GValidSizes); count++) {
	if (GValidSizes[count] == size)
	    return (((LongFlag) 1) << count);
    }

    return((LongFlag) 0);
}

/*	Function Name: DisplayCurrentFont
 *	Description: Displays the current font to the user.
 *	Arguments: fsw - the font selector widget.
 *                 font - the current font name.
 *	Returns: none.
 */

static void
DisplayCurrentFont(XmFontSelectorWidget fsw, String font)
{
    Arg largs[10];
    Cardinal num_largs;
    XmFontList fl;
    XFontStruct *fontdata;
    char buf[BUFSIZ * 2];
    Boolean	err = False;

    if ((fontdata = XLoadQueryFont(XtDisplay((Widget) fsw), font)) == NULL)
    {
	sprintf(buf, "Font '%s'\nis not availiable on this machine", font);
	DisplayUserError(fsw, buf);
	err = True;
    }
    else
    {

	/*
	 * Always clear out old errors
	 */
	RemoveUserError(NULL, (XtPointer)fsw, (XtPointer)0);

    /*
     * This is a hack to work around problems with Sun scaled fonts
     * and the Motif Text widget.  It turns out that some sun fonts
     * when a size of * is specified will return a font of height 0
     * this causes the ascent + descent to be zero and really confuses
     * the XmTextField widget.
     */

    if ((fontdata->ascent + fontdata->descent) == 0) {
	if (IsXlfdFont(font)) {
	    char *ptr, left_buf[BUFSIZ >> 2], right_buf[BUFSIZ >> 2], fbuf[BUFSIZ];
	    register int i, count;

	    /*
	     * This is a poorly formatted Sun Scaled font,
	     *
	     * Find where the size is and change it from a * to 140.
 	     */

	    for (ptr = font, i = count = 0; *ptr != 0; i++, ptr++) {
		if (*ptr != '-')
		    continue;

		count++;
		if (count == 8)
		    break;
	    }

	    ptr = (char *) strchr(ptr + 1, '-');
	    strcpy(right_buf, ptr);
	    strcpy(left_buf, font);
	    left_buf[i] = '\0';
	    snprintf(fbuf, sizeof fbuf, "%s-140%s", left_buf, right_buf);

	    if ((fontdata = XLoadQueryFont(XtDisplay((Widget) fsw),
					   fbuf)) == NULL)
	    {
		sprintf(buf,
			    "Font '%s'\nis not availiable on this machine",
			    font);
		DisplayUserError(fsw, buf);
		    err = True;
	    }
	}
	else {
		sprintf(buf,
			"Font '%s'\n%s",
			font,
		    "is is zero pixels high and cannot be displayed.");
	    DisplayUserError(fsw, buf);
		err = True;
	}
    }
    }

    if (!err) {
	fl = XmFontListCreate(fontdata, XmFONTLIST_DEFAULT_TAG);

	num_largs = 0;
	XtSetArg(largs[num_largs], XmNfontList, fl); num_largs++;
	XtSetValues(XmFontS_text(fsw), largs, num_largs);

	if (XmFontS_old_fontdata(fsw) != NULL)
	{
	    XFreeFont(XtDisplay((Widget) fsw), XmFontS_old_fontdata(fsw));
	    XmFontListFree(XmFontS_old_fontlist(fsw));
	}

	XmFontS_old_fontdata(fsw) = fontdata;
	XmFontS_old_fontlist(fsw) = fl;
    }

    if (XtIsManaged(XmFontS_name_label(fsw)))
    {
	XmString xmstr = XmStringCreateLocalized(font);
	num_largs = 0;
	XtSetArg(largs[num_largs], XmNlabelString, xmstr); num_largs++;
	XtSetValues(XmFontS_name_label(fsw), largs, num_largs);
	XmStringFree(xmstr);
    }

    /*
     * Fire off the callback for the user.
     */

    {
	FontData	*cf = XmFontS_font_info(fsw)->current_font;
	String 	str_ptr;

	if (XmFontS_xlfd_mode(fsw)) {
	    BuildFontString(fsw, cf, XmFontS_get_font(fsw), GET_FONT_SIZE);
	    str_ptr = XmFontS_get_font(fsw);
	}
	else
	    str_ptr = XrmQuarkToString(cf->familyq);

	XtCallCallbackList((Widget) fsw,
			   XmFontS_font_changed_callback(fsw), (XtPointer)str_ptr);
    }
}

/*	Function Name: BuildFontString
 *	Description: Builds a font string from the current font.
 *	Arguments:   fsw - the font selector widget.
 *                   cf - the font data to use.
 *                   buf - where to stuff the font name.
 *                   size - the size not to exceed.
 *	Returns: the font string (same as buf).
 */

/* ARGSUSED */
static String
BuildFontString(XmFontSelectorWidget fsw, FontData *cf, String buf, int size)
{
    static XrmQuark anyquark2, anyquark = NULLQUARK;
    String family, encoding;
    char res_x[BUFSIZ], res_y[BUFSIZ], point_size[BUFSIZ];

    if (anyquark == NULLQUARK) {
	String temp1 = _XmGetMBStringFromXmString(ANY_STRING(fsw));
	String temp2 = _XmGetMBStringFromXmString(LOWER_ANY_STRING(fsw));

	anyquark = XrmStringToQuark(temp1);
	anyquark2 = XrmStringToQuark(temp2);

	XtFree(temp1);
	XtFree(temp2);
    }

    if ((anyquark == cf->familyq) || (anyquark2 == cf->familyq))
	family = STAR_STRING;
    else
	family = XrmQuarkToString(cf->familyq);

    if (cf->point_size == 0)
	strcpy(point_size, STAR_STRING);
    else
	sprintf(point_size, "%d", cf->point_size);

    if (cf->resolution_x == 0)
	strcpy(res_x, STAR_STRING);
    else
	sprintf(res_x, "%d", (int) cf->resolution_x);

    if (cf->resolution_y == 0)
	strcpy(res_y, STAR_STRING);
    else
	sprintf(res_y, "%d", (int) cf->resolution_y);

    encoding = ENCODING_STRING(fsw);

    /*
     * I should really check to see that the string fits, but
     * What would I do it I failed?
     */

    sprintf(buf, "-*-%s-%s-%s-*-*-*-%s-%s-%s-%s-*-%s",
	    family, XrmQuarkToString(cf->weightq), cf->slant, point_size,
	    res_x, res_y, cf->spacing, encoding);

    return(buf);
}

/*	Function Name: UpdateBoldItalic
 *	Description: Updates the bold and italic buttons
 *	Arguments: fsw - the font selector widget.
 *	Returns: none.
 */

static void
UpdateBoldItalic(XmFontSelectorWidget fsw)
{
    Boolean set;
    FontData *cf = XmFontS_font_info(fsw)->current_font;
    FamilyInfo *fam;

    if ((fam = FindFamily(cf->familyq, XmFontS_font_info(fsw)->family_info,
			  XmFontS_font_info(fsw)->num_families)) == NULL)
    {
	String params[1];
	Cardinal num = 1;

	params[0] = XrmQuarkToString(cf->familyq);
	dbg(); _XmWarningMsg((Widget) fsw, XmNcouldNotFindFamilyData,
			XmNcouldNotFindFamilyDataMsg, params, num);
	return;
    }

    if ((fam->medium_nameq == NULLQUARK) || (fam->bold_nameq == NULLQUARK)) {
	set = (fam->medium_nameq == NULLQUARK);
	XmToggleButtonSetState(XmFontS_bold_toggle(fsw), set, False);
	SetFlag(&(XmFontS_user_state(fsw)), BOLD, set);
	XtSetSensitive(XmFontS_bold_toggle(fsw), False);
    }
    else {
	XtSetSensitive(XmFontS_bold_toggle(fsw), True);
    }

    if (CheckFlag(XmFontS_user_state(fsw), BOLD))
	cf->weightq = fam->bold_nameq;
    else
	cf->weightq = fam->medium_nameq;

    if ((fam->upright_nameq == NULLQUARK)|| (fam->italic_nameq == NULLQUARK)) {
	set = (fam->upright_nameq == NULLQUARK);
	XmToggleButtonSetState(XmFontS_italic_toggle(fsw), set, False);
	SetFlag(&(XmFontS_user_state(fsw)), ITALIC, set);
	XtSetSensitive(XmFontS_italic_toggle(fsw), False);
    }
    else {
	XtSetSensitive(XmFontS_italic_toggle(fsw), True);
    }

    if (CheckFlag(XmFontS_user_state(fsw), ITALIC))
	strcpy(cf->slant, XrmQuarkToString(fam->italic_nameq));
    else
	strcpy(cf->slant, XrmQuarkToString(fam->upright_nameq));
}

/*	Function Name:  UpdateFixedProportional
 *	Description: Updates the string stored in the fixed/proportional
 *                   part of the current font data.
 *	Arguments: fsw - the font selector widget.
 *	Returns: none.
 */

static void
UpdateFixedProportional(XmFontSelectorWidget fsw)
{
    FontData 	*cf = XmFontS_font_info(fsw)->current_font;
    FamilyInfo 	*fam;
    Boolean	setMono, setProp, setAny;

    if ((fam = FindFamily(cf->familyq, XmFontS_font_info(fsw)->family_info,
			  XmFontS_font_info(fsw)->num_families)) == NULL)
    {
	String params[1];
	Cardinal num = 1;

	params[0] = XrmQuarkToString(cf->familyq);
	dbg(); _XmWarningMsg((Widget) fsw, XmNcouldNotFindFamilyData,
		XmNcouldNotFindFamilyDataMsg, params, num);
	return;
    }

    if (CheckFlag(XmFontS_user_state(fsw), USER_PROPORTIONAL)) {
	if (CheckFlag(XmFontS_user_state(fsw), USER_FIXED))
	{
	    strcpy(cf->spacing, STAR_STRING);
	    setMono = False; setProp = False; setAny = True;
	}
	else
	{
	    strcpy(cf->spacing, PROPORTIONAL_SPACING);
	    setMono = False; setProp = True; setAny = False;
	}
    }
    else if (CheckFlag(XmFontS_user_state(fsw), USER_FIXED))
    {
	strcpy(cf->spacing, fam->fixed_spacing);
	setMono = True; setProp = False; setAny = False;
    }
    else
    {
	strcpy(cf->spacing, STAR_STRING);
	setMono = False; setProp = False; setAny = True;
    }
    if ( XmFontS_monospace(fsw) != NULL )
	XmToggleButtonSetState(XmFontS_monospace(fsw), setMono, False);
    if ( XmFontS_proportional(fsw) != NULL )
	XmToggleButtonSetState(XmFontS_proportional(fsw), setProp, False);
    if ( XmFontS_any_spacing(fsw) != NULL )
	XmToggleButtonSetState(XmFontS_any_spacing(fsw), setAny, False);
}

/*	Function Name: UpdateFamilies
 *	Description: Updates the displayed families.
 *	Arguments: fsw - the file selector widget.
 *	Returns: none
 */

static void
UpdateFamilies(XmFontSelectorWidget fsw)
{
    Arg largs[10];
    Cardinal num_largs;
    register int count;
    register int i, num;
    XmString *strs;
    LongFlag size_flag;
    FamilyInfo *fam = XmFontS_font_info(fsw)->family_info;
    FontData *cf = XmFontS_font_info(fsw)->current_font;
    Boolean str_match = False;
    String temp;

    num = XmFontS_font_info(fsw)->num_families;

    strs = (XmString *) XtMalloc(sizeof(XmString) * num);
    size_flag = SizeMapping(cf->point_size);
    for (i = count = 0; i < num; i++, fam++) {
	Boolean ok_75, ok_100, spacing_match, resolution_ok, has_sizes;

	/*
	 * Only add this family if:
	 *
	 * The size is 0 (ANY SIZE) and there is a font of the current
	 * right DPI, or
	 * This is a scaled font and scaling is turned on, or
	 * This font has current point size in 75 dpi and 75 or any selectd, or
	 * This font has current point size in 100 dpi and 100 or any selectd.
	 *
	 * And
	 * The Charset/Encodings match or any is selected.
	 * The type of spacing matches.
	 * This is a scaled font or there are some valid sizes
	 * in the current point size.
	 */

	ok_75 = ((size_flag != 0) &&
		 CheckFlag(XmFontS_user_state(fsw), DPI_75) &&
		 CheckLongFlag(fam->sizes_75, size_flag));

	ok_100 = ((size_flag != 0) &&
		  CheckFlag(XmFontS_user_state(fsw), DPI_100) &&
		  CheckLongFlag(fam->sizes_100, size_flag));

	resolution_ok = (cf->resolution_x == 0) && (cf->resolution_y == 0);

	resolution_ok |= (CheckFlag(XmFontS_user_state(fsw), DPI_75) &&
			  CheckFlag(fam->state, DPI_75));

	resolution_ok |= (CheckFlag(XmFontS_user_state(fsw), DPI_100) &&
			  CheckFlag(fam->state, DPI_100));

	if (!(has_sizes = IsScaled(fsw, fam))) {
	    has_sizes |= ((fam->sizes_75 != 0) &&
			  CheckFlag(XmFontS_user_state(fsw), DPI_75));

	    has_sizes |= ((fam->sizes_100 != 0) &&
			  CheckFlag(XmFontS_user_state(fsw), DPI_100));
	}

	/*
	 * The Any Family will match all spacings.
	 */

	spacing_match = (i == ANY_FAMILY);

	spacing_match |= (CheckFlag(XmFontS_user_state(fsw), USER_PROPORTIONAL) &&
			  CheckFlag(fam->state, PROPORTIONAL));

	spacing_match |= (CheckFlag(XmFontS_user_state(fsw), USER_FIXED) &&
			  !CheckFlag(fam->state, PROPORTIONAL));

	if (((cf->point_size == 0) || IsScaled(fsw, fam) || ok_75 || ok_100) &&
	    CheckEncoding(fsw, fam) && spacing_match &&
	    resolution_ok && has_sizes)
	{
	    strs[count++] = XmStringCreateLocalized(XrmQuarkToString(fam->nameq));
	    str_match |= (fam->nameq == cf->familyq);
	}
    }

    num_largs = 0;

    /*
     * This font is no longer in the list, reset the list to ANY_STRING
     */

    if (!str_match) {
	temp = _XmGetMBStringFromXmString(ANY_STRING(fsw));

	XtSetArg(largs[num_largs], XmNvalue, temp); num_largs++;
    }
    else
	temp = NULL;

    XtSetArg(largs[num_largs], XmNitems, strs); num_largs++;
    XtSetArg(largs[num_largs], XmNitemCount, count); num_largs++;
    XtSetValues(XmFontS_family_box(fsw), largs, num_largs);

    if (!str_match)
	FamilyChanged(XmFontS_family_box(fsw), (XtPointer) fsw, NULL);

    for (count--; count >= 0; count--)
	XmStringFree(strs[count]);
    XtFree((XtPointer) strs);
    XtFree((XtPointer) temp);
}

/*	Function Name: UpdateSizes
 *	Description: Updates the list of sizes.
 *	Arguments: fsw - the font selector widget.
 *	Returns: none.
 */

static void
UpdateSizes(XmFontSelectorWidget fsw)
{
    FontData * cf = XmFontS_font_info(fsw)->current_font;
    Arg largs[10];
    Cardinal num_largs = 0;
    register int count = 0;
    register int i, size;
    XmString *strs;
    LongFlag size_flag;
    FamilyInfo *family;
    Boolean editable, match = False;
    String temp = NULL;

    if ((family = FindFamily(XmFontS_font_info(fsw)->current_font->familyq,
			     XmFontS_font_info(fsw)->family_info,
			     XmFontS_font_info(fsw)->num_families)) == NULL)
    {
	String params[1];
	Cardinal num = 1;

	params[0] = XrmQuarkToString(cf->familyq);
	dbg(); _XmWarningMsg((Widget) fsw, XmNcouldNotFindFamilyData,
		XmNcouldNotFindFamilyDataMsg, params, num);
	return;
    }

    if (IsScaled(fsw, family)) {
	family = XmFontS_font_info(fsw)->family_info + ANY_FAMILY;
	editable = True;
    }
    else {
	editable = False;
    }

    size_flag = 0;
    if (CheckFlag(XmFontS_user_state(fsw), DPI_75))
	size_flag |= family->sizes_75;
    if (CheckFlag(XmFontS_user_state(fsw), DPI_100))
	size_flag |= family->sizes_100;

    size = sizeof(LongFlag) * 8; /* There are 8 bits in a BYTE */
    strs = (XmString *) XtMalloc(sizeof(XmString) * (size + 1));
    /*
     * DMS - fix to work around apparent Motif 1.2.4 bug concerning
     *       XmStrings containing as the empty string
     */
    if (XmStringEmpty(ANY_STRING(fsw)))
        strs[count++] = XmStringCreateLocalized(" ");
    else
        strs[count++] = XmStringCopy(ANY_STRING(fsw));

    for (i = 0; i < XtNumber(GValidSizes); i++) {
	char buf[10];
	LongFlag flag = ((LongFlag) 1) << i;

	if (!CheckLongFlag(size_flag, flag) && !IsScaled(fsw, family))
	    continue;

	sprintf(buf, "%d", GValidSizes[i]);
	strs[count++] = XmStringCreateLocalized(buf);

	match |= (SizeMapping(cf->point_size) == flag);
    }

    if (!match) {
	temp = _XmGetMBStringFromXmString(ANY_STRING(fsw));

	XtSetArg(largs[num_largs], XmNvalue, temp); num_largs++;
    }

    XtSetArg(largs[num_largs], XmNitems, strs); num_largs++;
    XtSetArg(largs[num_largs], XmNitemCount, count); num_largs++;
    XtSetArg(largs[num_largs], XmNeditable, editable); num_largs++;
    XtSetValues(XmFontS_size_box(fsw), largs, num_largs);


    if (!match)
	SizeChanged(XmFontS_size_box(fsw), (XtPointer) fsw, NULL);

    for (count--; count >= 0; count--)
	XmStringFree(strs[count]);
    XtFree((XtPointer) strs);
    XtFree((XtPointer) temp);
}

/*	Function Name: SetComboValue
 *	Description: Sets the value field of a Combination box.
 *	Arguments: box - the combo box.
 *                 value - the value to set.
 *	Returns: none.
 */

static void
SetComboValue(Widget box, String value)
{
    Arg largs[10];
    Cardinal num_largs = 0;

    XtSetArg(largs[num_largs], XmNvalue, value); num_largs++;
    XtSetValues(box, largs, num_largs);
}

/*	Function Name: UnsetSiblings
 *	Description: Unsets all sibling toggles of this toggle.
 *	Arguments: w - the toggle who's siblings we are unsetting.
 *	Returns: none.
 */

static void
UnsetSiblings(Widget w)
{
    WidgetList children;
    Cardinal num_children;
    Arg largs[10];
    Cardinal i, num_largs = 0;

    XtSetArg(largs[num_largs], XmNchildren, &children); num_largs++;
    XtSetArg(largs[num_largs], XmNnumChildren, &num_children); num_largs++;
    XtGetValues(XtParent(w), largs, num_largs);

    for (i = 0; i < num_children; i++)
	XmToggleButtonSetState(children[i], (children[i] == w), False);
}

/*	Function Name: DisplayUserError
 *	Description: Displays an error message to the user.
 *	Arguments: fsw - the font selector widget.
 *                 msg - the message to display.
 *	Returns: none
 *
 * NOTE: If (msg == NULL) then the value of current_text will be placed
 *       in the text widget.
 */

static void
DisplayUserError(XmFontSelectorWidget fsw, String msg)
{
    String temp;
    if (msg != NULL) {
	/*
	 * If the real text is already in current_text then do not
	 * attempt to get it out of the widget, just use what we have
	 * saved already, this hackery is necessary because of the
	 * wierd state machine that is in this code.
	 *
	 * The problem being solved is that this should work if we call
	 * DisplayUserError if a user error is already being displayed.
	 */

	if (XmFontS_current_text(fsw) != NULL) {
	    temp = XmFontS_current_text(fsw);
	    XmFontS_current_text(fsw) = NULL;
	}
	else
	    temp = XmTextGetString(XmFontS_text(fsw));

	XmTextSetString(XmFontS_text(fsw), msg);
	XmFontS_current_text(fsw) = temp;

	XmProcessTraversal(XmFontS_text(fsw), XmTRAVERSE_CURRENT);
    }
    else {
	if (XmFontS_current_text(fsw) == NULL)
	    return;		/* noting to do. */

	temp = XmFontS_current_text(fsw);
	XmFontS_current_text(fsw) = NULL;

	XmTextSetString(XmFontS_text(fsw), temp);
	XtFree((XtPointer) temp);
    }
}

/*	Function Name: SetResolution
 *	Description: Sets the DPI toggles based on the FontData passed.
 *	Arguments: fsw - the font selector widget.
 *                 cf - the font data to use.
 *	Returns: none.
 */

static void
SetResolution(XmFontSelectorWidget fsw, FontData *cf)
{
    Boolean	set75, set100, setAny;

    if ( cf->resolution_x != cf->resolution_y )
    {
	set75 = False;
	set100 = False;
	setAny = True;
    }
    else if ( XmFontS_font_info(fsw)->current_font->resolution_x == 75 )
    {
	set75 = True;
	set100 = False;
	setAny = False;
    }
    else if ( XmFontS_font_info(fsw)->current_font->resolution_x == 100 )
    {
	set75 = False;
	set100 = True;
	setAny = False;
    }
    else
    {
	set75 = False;
	set100 = False;
	setAny = True;
    }
    XmToggleButtonSetState(XmFontS_dpi75(fsw), set75, False);
    XmToggleButtonSetState(XmFontS_dpi100(fsw), set100, False);
    XmToggleButtonSetState(XmFontS_dpiAny(fsw), setAny, False);
}

/*	Function Name: SetDisplayedFont
 *	Description: Sets the font to be displayed.
 *	Arguments: fsw - the font selector widget.
 *                 font - the font to be displayed.
 *	Returns: none.
 */

static void
SetDisplayedFont(XmFontSelectorWidget fsw, String new_font)
{
    Boolean 	set;
    Arg 	largs[10];
    Cardinal 	num_largs;
    FontData 	*cf = XmFontS_font_info(fsw)->current_font;
    char 	buf[BUFSIZ];

    sprintf(buf, "--%s-%s-%s----0-%d-%d-*--%s",
	    STAR_STRING, DEFAULT_WEIGHT, DEFAULT_SLANT,
	    XmFontS_font_info(fsw)->resolution,
	    XmFontS_font_info(fsw)->resolution, ENCODING_STRING(fsw));

    FillData(fsw, cf, buf);		/* Put in default data. */

    if (new_font != NULL) {
	if (IsXlfdFont(new_font))
	    FillData(fsw, cf, new_font);
	else {
	    if (strchr(new_font, '-')) {
		String params[1];
		Cardinal num = 1;

		params[0] = new_font;
		dbg(); _XmWarningMsg((Widget) fsw, XmNbadXlfdFont,
			XmNbadXlfdFontMsg, params, num);
	    }
	    else {
		ChangeMode(fsw, False, False);
		SetComboValue(XmFontS_family_box(fsw), new_font);

		/*
		 * For optomization of FamilyChanged Only.
		 */

		cf->familyq = XrmStringToQuark(new_font);
		return;
	    }
	}
    }

    num_largs = 0;
    set = CheckFlag(cf->state, ITALIC);
    SetFlag(&(XmFontS_user_state(fsw)), ITALIC, set);
    XtSetArg(largs[num_largs], XmNset, set); num_largs++;
    XtSetValues(XmFontS_italic_toggle(fsw), largs, num_largs);

    num_largs = 0;
    set = CheckFlag(cf->state, BOLD);
    SetFlag(&(XmFontS_user_state(fsw)), BOLD, set);
    XtSetArg(largs[num_largs], XmNset, set); num_largs++;
    XtSetValues(XmFontS_bold_toggle(fsw), largs, num_largs);

    SetComboValue(XmFontS_family_box(fsw), XrmQuarkToString(cf->familyq));
    sprintf(buf, "%d", cf->point_size / POINT_DIVIDE);
    SetComboValue(XmFontS_size_box(fsw), buf);

    SetResolution(fsw, cf);

    XmCopyISOLatin1Lowered(buf, cf->spacing);
    if ( strstr(buf, STAR_STRING) != NULL )
    {
	SetFlag(&(XmFontS_user_state(fsw)), USER_PROPORTIONAL | USER_FIXED, True);
    }
    else if ( strstr(buf, DEFAULT_FIXED_SPACING) != NULL ||
	      strstr(buf, OTHER_FIXED_SPACING)	!= NULL )
    {
	SetFlag(&(XmFontS_user_state(fsw)), USER_FIXED, True);
	SetFlag(&(XmFontS_user_state(fsw)), USER_PROPORTIONAL, False);
    }
    else if ( strstr(buf, PROPORTIONAL_SPACING) != NULL )
    {
    	SetFlag(&(XmFontS_user_state(fsw)), USER_PROPORTIONAL, True);
	SetFlag(&(XmFontS_user_state(fsw)), USER_FIXED, False);
    }
    else
    {
	SetFlag(&(XmFontS_user_state(fsw)), USER_PROPORTIONAL | USER_FIXED, True);
    }

    UpdateFixedProportional(fsw);

    cf->familyq = NULLQUARK;	/* work around FamilyChanged Optomization. */

    FamilyChanged(XmFontS_family_box(fsw), (XtPointer) fsw, NULL);
    UpdateFamilies(fsw);
}

/************************************************************
 *
 *  Routines for handling when the change from other <--> xlfd mode
 *
 ************************************************************/

/*	Function Name: AddToXlfdOnlyList
 *	Description: Adds a widget to the xlfd only list.
 *	Arguments: fsw - the font selector.
 *                 w - the widget to add.
 *	Returns: none.
 */

static void
AddToXlfdOnlyList(XmFontSelectorWidget fsw, Widget w)
{
    int num = (int) XmFontS_num_xlfd_only(fsw);
    int alloc = (int) XmFontS_alloc_xlfd_only(fsw);

    if (num >= alloc) {
	alloc += WIDGET_LIST_INC;
	XmFontS_xlfd_only(fsw) = (WidgetList) XtRealloc((XtPointer)
						   XmFontS_xlfd_only(fsw),
						   sizeof(Widget) * alloc);
	XmFontS_alloc_xlfd_only(fsw) = alloc;
    }

    XmFontS_xlfd_only(fsw)[num] = w;
    (XmFontS_num_xlfd_only(fsw))++;
}

/*	Function Name: AddToXlfdSensitiveList
 *	Description: Adds a widget to the xlfd sensitive list.
 *	Arguments: fsw - the font selector.
 *                 w - the widget to add.
 *	Returns: none.
 */

static void
AddToXlfdSensitiveList(XmFontSelectorWidget fsw, Widget w)
{
    int num = (int) XmFontS_num_xlfd_sensitive(fsw);
    int alloc = (int) XmFontS_alloc_xlfd_sensitive(fsw);

    if (num >= alloc) {
	alloc += WIDGET_LIST_INC;
	XmFontS_xlfd_sensitive(fsw) = (WidgetList) XtRealloc((XtPointer)
							XmFontS_xlfd_sensitive(fsw),
							sizeof(Widget)* alloc);
	XmFontS_alloc_xlfd_sensitive(fsw) = alloc;
    }

    XmFontS_xlfd_sensitive(fsw)[num] = w;
    (XmFontS_num_xlfd_sensitive(fsw))++;
}

/*	Function Name: ChangeMode
 *	Description: Changes the mode of the font selector.
 *	Arguments: fsw - the file selector widget.
 *                 xlfd_mode - If True then change to xlfd mode,
 *                             otherwize change to other mode.
 *		   force - For other mode - True if we're changing modes
 *				false if we're displaying a font
 *
 *	Returns: none.
 */

static void
ChangeMode(XmFontSelectorWidget fsw, Boolean xlfd_mode, Boolean force)
{
    Arg largs[10];
    Cardinal num_largs;
    register WidgetList widgets;
    register int i, num;
    XmString family_label;

    XmFontS_xlfd_mode(fsw) = xlfd_mode; /* remember our current mode. */

    num = (int) XmFontS_num_xlfd_sensitive(fsw);
    widgets = XmFontS_xlfd_sensitive(fsw);

    for (i = 0; i < num; i++, widgets++)
	XtSetSensitive(*widgets, xlfd_mode);

    num = (int) XmFontS_num_xlfd_only(fsw);
    widgets = XmFontS_xlfd_only(fsw);

    for (i = 0; i < num; i++, widgets++) {
	if (xlfd_mode)
	    XtManageChild(*widgets);
	else
	    XtUnmanageChild(*widgets);
    }

    if (xlfd_mode) {
	char buf[BUFSIZ];
	FontData * cf = XmFontS_font_info(fsw)->current_font;
	String temp = _XmGetMBStringFromXmString(ANY_STRING(fsw));

	family_label = FAMILY_STRING(fsw);

	/*
	 * Reset the values in the family and size boxes to "any"
	 */

	SetComboValue(XmFontS_family_box(fsw), temp);
	SetComboValue(XmFontS_size_box(fsw), temp);

	cf->familyq = XrmStringToQuark(temp);
	cf->point_size = 0;

	UpdateFamilies(fsw);
	UpdateSizes(fsw);
	UpdateBoldItalic(fsw);
	UpdateFixedProportional(fsw);

	DisplayCurrentFont(fsw, BuildFontString(fsw, cf, buf, BUFSIZ));

	XtFree((XtPointer) temp);
    }
    else {
	family_label = OTHER_FONT_STRING(fsw);

	SetOtherList(fsw, force);

    }

    /*
     * Set the proper toggle buttons.
     */

    UnsetSiblings((xlfd_mode) ? XmFontS_xlfd_toggle(fsw) : XmFontS_other_toggle(fsw));

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNlabelString, family_label); num_largs++;
    XtSetArg(largs[num_largs], XmNeditable, !xlfd_mode); num_largs++;
    XtSetValues(XmFontS_family_box(fsw), largs, num_largs);
}

/*	Function Name: SetOtherList
 *	Description: Fills the other list with a complete list of
 *                   all other fonts.
 *	Arguments: fsw - the font selector widget.
 *			force - True if toggled by user, False if user
 *				is setting the current font
 *	Returns: none
 */

static void
SetOtherList(XmFontSelectorWidget fsw, Boolean force)
{
    Arg largs[10];
    Cardinal num_largs = 0;
    register int count;
    XmString *strs;
    String *others = XmFontS_font_info(fsw)->others;
    FontData * cf = XmFontS_font_info(fsw)->current_font;
    register int i, num = XmFontS_font_info(fsw)->num_others;
    String	curFont = XmFontS_current_font(fsw);

    /*
     * Set the current font to the first one on the list.
     */

    if (num > 0) {

	if (force)
	{
	XtSetArg(largs[num_largs], XmNvalue, *others); num_largs++;

	/*
	 * This only to allow FamilyChanged to be more efficient.
	 */
	cf->familyq = XrmStringToQuark(*others);
	DisplayCurrentFont(fsw, *others);
    }
	else
	{
	    XtSetArg(largs[num_largs], XmNvalue, curFont); num_largs++;
	    cf->familyq = XrmStringToQuark(curFont);
	    DisplayCurrentFont(fsw, curFont);
	}
    }
    else {
	XtSetArg(largs[num_largs], XmNvalue, " "); num_largs++;
    }

    strs = (XmString *) XtMalloc(sizeof(XmString) * num);
    for (i = count = 0; i < num; i++, others++)
	strs[count++] = XmStringCreateLocalized(*others);

    XtSetArg(largs[num_largs], XmNitems, strs); num_largs++;
    XtSetArg(largs[num_largs], XmNitemCount, count); num_largs++;
    XtSetValues(XmFontS_family_box(fsw), largs, num_largs);

    for (count--; count >= 0; count--)
	XmStringFree(strs[count]);
    XtFree((XtPointer) strs);
}

/************************************************************
 *
 *  These routines actually create the children of the font
 *  selector.
 *
 ************************************************************/

/*	Function Name: CreateChildren
 *	Description: Creates all children of the font selector.
 *	Arguments: fsw - the font selector.
 *                 args, num_args - the argument list.
 *	Returns: none.
 */

static void
CreateChildren(XmFontSelectorWidget fsw, ArgList args, Cardinal num_args)
{
    Arg *margs, largs[10];
    Cardinal num_largs;
    Widget temp, box;
    String temp_txt;

    CreateTopWidgets(fsw, (Widget) fsw, args, num_args);
    CreateMiddleArea(fsw, args, num_args);

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNfillOption, XmFillAll); num_largs++;
    XtSetArg(largs[num_largs], XmNmarginHeight, 2); num_largs++;
    margs = XtMergeArgLists(largs, num_largs, args, num_args);
    box = XtCreateManagedWidget("box", xmButtonBoxWidgetClass,
				(Widget) fsw, margs, num_args + num_largs);
    XtFree((XtPointer) margs);

    num_largs = 0;
    temp_txt = _XmGetMBStringFromXmString(SAMPLE_TEXT(fsw));
    XtSetArg(largs[num_largs], XmNvalue, temp_txt); num_largs++;
    XtSetArg(largs[num_largs], XmNeditMode, XmMULTI_LINE_EDIT); num_largs++;
    XtSetArg(largs[num_largs], XmNwordWrap, True); num_largs++;
    XtSetArg(largs[num_largs], XmNscrollHorizontal, False); num_largs++;
    XtSetArg(largs[num_largs], XmNrows, XmFontS_text_rows(fsw)); num_largs++;
    margs = XtMergeArgLists(largs, num_largs, args, num_args);
    temp = XmCreateScrolledText(box, "text", margs, num_args + num_largs);
    XtManageChild(XmFontS_text(fsw) = temp);
    XtFree((XtPointer) margs);
    XtFree((XtPointer) temp_txt);

    XtAddCallback(temp, XmNlosingFocusCallback,
		  RemoveUserError, (XtPointer) fsw);

    XtAddCallback(temp, XmNmodifyVerifyCallback,
		  RemoveUserError, (XtPointer) fsw);

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNskipAdjust, True); num_largs++;
    margs = XtMergeArgLists(largs, num_largs, args, num_args);
    temp = XtCreateWidget("nameLabel", xmLabelWidgetClass,
			  (Widget) fsw, margs, num_args + num_largs);

    if (XmFontS_show_font_name(fsw))
	XtManageChild(temp);

    XmFontS_name_label(fsw) = temp;
    XtFree((XtPointer) margs);
}

/*	Function Name: CreateTopWidgets
 *	Description: Creates the children in the middle area where
 *                   fonts are selected.
 *	Arguments: fsw - the font selector.
 *                 parent - the parent of the created widgets.
 *                 args, num_args - the argument list.
 *	Returns: none.
 */

static void
CreateTopWidgets(XmFontSelectorWidget fsw, Widget parent,
		 ArgList args, Cardinal num_args)
{
    Arg *margs, largs[10];
    Cardinal num_largs;
    Widget pane, button;

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNorientation, XmHORIZONTAL); num_largs++;
    XtSetArg(largs[num_largs], XmNspacing, 2); num_largs++;
    XtSetArg(largs[num_largs], XmNmarginWidth, 0); num_largs++;
    XtSetArg(largs[num_largs], XmNmarginHeight, 0); num_largs++;
    margs = XtMergeArgLists(largs, num_largs, args, num_args);
    pane = XtCreateManagedWidget("topPane", xmPanedWidgetClass,
				 parent, margs, num_args + num_largs);
    XtFree((XtPointer) margs);

    CreateFamilyBox(fsw, pane, args, num_args);
    CreateSizesBox(fsw, pane, args, num_args);
    CreateBoldItalicBox(fsw, pane, args, num_args);

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNlabelString, OPTION_STRING(fsw));num_largs++;
    XtSetArg(largs[num_largs], XmNshadowThickness, 2); num_largs++;
    XtSetArg(largs[num_largs], XmNindicatorOn, False); num_largs++;
    XtSetArg(largs[num_largs], XmNskipAdjust, True); num_largs++;
    margs = XtMergeArgLists(largs, num_largs, args, num_args);
    button = XtCreateManagedWidget("optionButton", xmToggleButtonWidgetClass,
				   pane, margs, num_args + num_largs);
    XtFree((XtPointer) margs);

    XtAddCallback(button, XmNvalueChangedCallback,
		  ToggleMiddlePane, (XtPointer) fsw);
}

/*	Function Name: CreateMiddleArea
 *	Description: Creates the widgets in the top left area.
 *                   These are the family, size, bold/italic and proportional
 *                   boxes.
 *	Arguments: fsw - the file selection widget.
 *                 args, num_args - arguments to the font selector (filtered).
 *	Returns: none.
 */

static void
CreateMiddleArea(XmFontSelectorWidget fsw, ArgList args, Cardinal num_args)
{
    Arg *margs, largs[10];
    Cardinal num_largs;
    Widget pane, top_pane;

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNorientation, XmVERTICAL); num_largs++;
    XtSetArg(largs[num_largs], XmNspacing, 2); num_largs++;
    XtSetArg(largs[num_largs], XmNmarginWidth, 0); num_largs++;
    XtSetArg(largs[num_largs], XmNmarginHeight, 0); num_largs++;
    margs = XtMergeArgLists(largs, num_largs, args, num_args);
    pane = XtCreateWidget("middlePane", xmPanedWidgetClass,
			  (Widget) fsw, margs, num_args + num_largs);
    XtFree((XtPointer) margs);
    XmFontS_middle_pane(fsw) = pane;

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNorientation, XmHORIZONTAL); num_largs++;
    XtSetArg(largs[num_largs], XmNspacing, 2); num_largs++;
    XtSetArg(largs[num_largs], XmNmarginWidth, 0); num_largs++;
    XtSetArg(largs[num_largs], XmNmarginHeight, 0); num_largs++;
    XtSetArg(largs[num_largs], XmNshowSash, False); num_largs++;
    margs = XtMergeArgLists(largs, num_largs, args, num_args);
    top_pane = XtCreateManagedWidget("leftPane", xmPanedWidgetClass,
				      pane, margs, num_args + num_largs);
    XtFree((XtPointer) margs);

    CreateFontChoiceBox(fsw, top_pane, args, num_args);
    CreateResolutionBox(fsw, top_pane, args, num_args);

    CreateSpacingBox(fsw, pane, args, num_args);
    CreateOtherChoiceBox(fsw, pane, args, num_args);
}

/*	Function Name: CreateFamilyBox
 *	Description: Creates the family Family box.
 *	Arguments: fsw - the file selection widget.
 *                 parent - the parent of this area.
 *                 args, num_args - arguments to the font selector (filtered).
 *	Returns: none.
 */

static void
CreateFamilyBox(XmFontSelectorWidget fsw, Widget parent,
		ArgList args, Cardinal num_args)
{
    Widget c_box;
    Cardinal num_largs;
    Arg *margs, largs[15];
    String temp = _XmGetMBStringFromXmString(ANY_STRING(fsw));

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNlabelString,FAMILY_STRING(fsw)); num_largs++;
    XtSetArg(largs[num_largs], XmNshowSash, False); num_largs++;
    XtSetArg(largs[num_largs], XmNcolumns, FAMILY_COLUMNS); num_largs++;
    XtSetArg(largs[num_largs], XmNpopupOffset, 0); num_largs++;
    XtSetArg(largs[num_largs], XmNverticalMargin, 0); num_largs++;
    XtSetArg(largs[num_largs], XmNhorizontalMargin, 0); num_largs++;
    XtSetArg(largs[num_largs], XmNverify, False); num_largs++;
    XtSetArg(largs[num_largs], XmNeditable, False); num_largs++;
    XtSetArg(largs[num_largs], XmNvisibleItemCount, VISIBILE_FAMILIES);
    num_largs++;
    /*
     * This may cause a problem in Motif I18N applications with 1.2
     */

    XtSetArg(largs[num_largs], XmNvalue, temp); num_largs++;

    margs = XtMergeArgLists(largs, num_largs, args, num_args);
    c_box = XtCreateManagedWidget("families", xmDropDownWidgetClass,
				  parent, margs, num_args + num_largs);

    XtAddCallback(c_box, XmNverifyTextCallback, FamilyChanged, (XtPointer)fsw);
    XtAddCallback(c_box, XmNupdateTextCallback, FamilyChanged, (XtPointer)fsw);

    XtFree((XtPointer) margs);
    XtFree((XtPointer) temp);
    XmFontS_family_box(fsw) = c_box;
}

/*	Function Name: CreateSizesBox
 *	Description: Creates the box for the sizes
 *	Arguments: fsw - the file selection widget.
 *                 parent - the parent of this area.
 *                 args, num_args - arguments to the font selector (filtered).
 *	Returns: none.
 *
 * NOTE: List is constructed later in UpdateSizes.
 */

static void
CreateSizesBox(XmFontSelectorWidget fsw, Widget parent,
	       ArgList args, Cardinal num_args)
{
    Widget c_box;
    Cardinal num_largs;
    Arg *margs, largs[15];
    String temp;



    num_largs = 0;
    XtSetArg(largs[num_largs], XmNlabelString, SIZE_STRING(fsw)); num_largs++;
    XtSetArg(largs[num_largs], XmNshowSash, False); num_largs++;
    XtSetArg(largs[num_largs], XmNcolumns, 4); num_largs++;
    XtSetArg(largs[num_largs], XmNmaxLength, 3); num_largs++;
    XtSetArg(largs[num_largs], XmNpopupOffset, 0); num_largs++;
    XtSetArg(largs[num_largs], XmNverticalMargin, 0); num_largs++;
    XtSetArg(largs[num_largs], XmNhorizontalMargin, 0); num_largs++;
    XtSetArg(largs[num_largs], XmNverify, False); num_largs++;
    XtSetArg(largs[num_largs], XmNskipAdjust, True); num_largs++;
    XtSetArg(largs[num_largs], XmNvisibleItemCount, VISIBILE_FAMILIES);
    num_largs++;

    temp = _XmGetMBStringFromXmString(ANY_STRING(fsw));
    XtSetArg(largs[num_largs], XmNvalue, temp); num_largs++;

    margs = XtMergeArgLists(largs, num_largs, args, num_args);
    c_box = XtCreateManagedWidget("sizes", xmDropDownWidgetClass,
				  parent, margs, num_args + num_largs);
    XtFree(temp);
    AddToXlfdOnlyList(fsw, c_box);

    XtAddCallback(c_box, XmNverifyTextCallback, SizeChanged, (XtPointer)fsw);
    XtAddCallback(c_box, XmNupdateTextCallback, SizeChanged, (XtPointer)fsw);

    XtFree((XtPointer) margs);
    XmFontS_size_box(fsw) = c_box;
}

/*	Function Name: CreateBoldItalicBox
 *	Description: Creates the bold and italic choice box.
 *	Arguments: fsw - the file selection widget.
 *                 parent - the parent of this area.
 *                 args, num_args - arguments to the font selector (filtered).
 *	Returns: none.
 */

static void
CreateBoldItalicBox(XmFontSelectorWidget fsw, Widget parent,
		    ArgList args, Cardinal num_args)
{
    Widget box, button;
    Cardinal num_largs, num_str;
    Arg *margs, largs[10];

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNorientation, XmHORIZONTAL); num_largs++;
    XtSetArg(largs[num_largs], XmNshowSash, False); num_largs++;
    XtSetArg(largs[num_largs], XmNskipAdjust, True); num_largs++;
    margs = XtMergeArgLists(largs, num_largs, args, num_args);
    box = XtCreateManagedWidget("boldItalicBox", xmButtonBoxWidgetClass,
				parent, margs, num_args + num_largs);
    AddToXlfdOnlyList(fsw, box);
    XtFree((XtPointer) margs);

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNlabelString, BOLD_STRING(fsw));
    num_str = num_largs++;
    XtSetArg(largs[num_largs], XmNindicatorType, XmN_OF_MANY); num_largs++;
    XtSetArg(largs[num_largs], XmNalignment, XmALIGNMENT_BEGINNING);
    num_largs++;
    margs = XtMergeArgLists(largs, num_largs, args, num_args);
    button = XtCreateManagedWidget("boldButton", xmToggleButtonWidgetClass,
				   box, margs, num_args + num_largs);
    XmFontS_bold_toggle(fsw) = button;
    XtAddCallback(button, XmNvalueChangedCallback, ToggleBold, (XtPointer)fsw);

    margs[num_str].value = (XtArgVal) ITALIC_STRING(fsw);
    button = XtCreateManagedWidget("italicButton", xmToggleButtonWidgetClass,
				   box, margs, num_args + num_largs);
    XmFontS_italic_toggle(fsw) = button;
    XtAddCallback(button,
		  XmNvalueChangedCallback, ToggleItalic, (XtPointer) fsw);

    XtFree((XtPointer) margs);
}

/*	Function Name: CreateSpacingBox
 *	Description: Creates the spacing choice box.
 *	Arguments: fsw - the file selection widget.
 *                 parent - the parent of this area.
 *                 args, num_args - arguments to the font selector (filtered).
 *	Returns: none.
 */

static void
CreateSpacingBox(XmFontSelectorWidget fsw, Widget parent,
		 ArgList args, Cardinal num_args)
{
    Widget box, button;
    Cardinal num_largs, num_str, num_set;
    Arg *margs, largs[10];

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNorientation, XmHORIZONTAL); num_largs++;
    XtSetArg(largs[num_largs], XmNshowSash, False); num_largs++;
    XtSetArg(largs[num_largs], XmNfillOption, XmFillMajor); num_largs++;
    XtSetArg(largs[num_largs], XmNequalSize, True); num_largs++;
    margs = XtMergeArgLists(largs, num_largs, args, num_args);
    box = XtCreateManagedWidget("spacingBox", xmButtonBoxWidgetClass,
				parent, margs, num_args + num_largs);
    AddToXlfdSensitiveList(fsw, box);
    XtFree((XtPointer) margs);

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNset, False); num_set = num_largs++;
    XtSetArg(largs[num_largs], XmNlabelString, PROPORTIONAL_STRING(fsw));
    num_str = num_largs++;
    XtSetArg(largs[num_largs], XmNindicatorType, XmONE_OF_MANY); num_largs++;
    XtSetArg(largs[num_largs], XmNalignment, XmALIGNMENT_BEGINNING);
    num_largs++;
    margs = XtMergeArgLists(largs, num_largs, args, num_args);
    button = XtCreateManagedWidget("proportionalButton",
				   xmToggleButtonWidgetClass,
				   box, margs, num_args + num_largs);
    XtAddCallback(button, XmNvalueChangedCallback, ToggleProportional,
		  (XtPointer) fsw);
    XmFontS_proportional(fsw) = button;

    margs[num_str].value = (XtArgVal) MONO_SPACE_STRING(fsw);
    button = XtCreateManagedWidget("monoButton", xmToggleButtonWidgetClass,
				   box, margs, num_args + num_largs);
    XtAddCallback(button, XmNvalueChangedCallback, ToggleFixed,
		  (XtPointer) fsw);
    XmFontS_monospace(fsw) = button;

    margs[num_str].value = (XtArgVal) BOTH_STRING(fsw);
    margs[num_set].value = (XtArgVal) True;
    button = XtCreateManagedWidget("bothButton", xmToggleButtonWidgetClass,
				   box, margs, num_args + num_largs);
    XtAddCallback(button, XmNvalueChangedCallback, ToggleBothSpacing,
		  (XtPointer) fsw);
    XmFontS_any_spacing(fsw) = button;
    SetFlag(&(XmFontS_user_state(fsw)), USER_FIXED | USER_PROPORTIONAL, True);
    XtFree((XtPointer) margs);
}

/*	Function Name: CreateFontChoiceBox
 *	Description: Create a box to choose between xlfd fonts and others.
 *	Arguments: fsw - the file selection widget.
 *                 parent - the parent of this area.
 *                 args, num_args - arguments to the font selector (filtered).
 *	Returns: none.
 */

static void
CreateFontChoiceBox(XmFontSelectorWidget fsw, Widget parent,
		    ArgList args, Cardinal num_args)
{
    Widget box, button;
    Cardinal num_largs, num_str, num_set;
    Arg *margs, largs[10];

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNorientation, XmHORIZONTAL); num_largs++;
    XtSetArg(largs[num_largs], XmNshowSash, False); num_largs++;
    XtSetArg(largs[num_largs], XmNfillOption, XmFillMajor); num_largs++;
    XtSetArg(largs[num_largs], XmNequalSize, True); num_largs++;
    margs = XtMergeArgLists(largs, num_largs, args, num_args);
    box = XtCreateManagedWidget("choiceBox", xmButtonBoxWidgetClass,
				parent, margs, num_args + num_largs);
    XtFree((XtPointer) margs);

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNlabelString, XLFD_STRING(fsw));
    num_str = num_largs++;
    XtSetArg(largs[num_largs], XmNset, True); num_set = num_largs++;
    XtSetArg(largs[num_largs], XmNindicatorType, XmONE_OF_MANY); num_largs++;
    XtSetArg(largs[num_largs], XmNalignment, XmALIGNMENT_BEGINNING);
    num_largs++;
    margs = XtMergeArgLists(largs, num_largs, args, num_args);
    button = XtCreateManagedWidget("xlfdButton",
				   xmToggleButtonWidgetClass,
				   box, margs, num_args + num_largs);
    XmFontS_xlfd_toggle(fsw) = button;

    XtAddCallback(button, XmNvalueChangedCallback, XlfdMode, (XtPointer) fsw);

    margs[num_str].value = (XtArgVal) OTHER_FONT_STRING(fsw);
    margs[num_set].value = (XtArgVal) False;
    button = XtCreateManagedWidget("otherButton", xmToggleButtonWidgetClass,
				   box, margs, num_args + num_largs);
    XtAddCallback(button, XmNvalueChangedCallback, OtherMode, (XtPointer) fsw);
    XmFontS_other_toggle(fsw) = button;

    XtFree((XtPointer) margs);
}

/*	Function Name: CreateResolution Box
 *	Description: Create the box to allow resolution choice(s).
 *	Arguments: fsw - the file selection widget.
 *                 parent - the parent of this area.
 *                 args, num_args - arguments to the font selector (filtered).
 *	Returns: none.
 */

static void
CreateResolutionBox(XmFontSelectorWidget fsw, Widget parent,
		    ArgList args, Cardinal num_args)
{
    Widget box, button;
    Cardinal num_largs, num_str, num_set;
    Arg *margs, largs[10];

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNorientation, XmHORIZONTAL); num_largs++;
    XtSetArg(largs[num_largs], XmNshowSash, False); num_largs++;
    XtSetArg(largs[num_largs], XmNfillOption, XmFillMajor); num_largs++;
    XtSetArg(largs[num_largs], XmNequalSize, True); num_largs++;
    margs = XtMergeArgLists(largs, num_largs, args, num_args);
    box = XtCreateManagedWidget("resolutionBox", xmButtonBoxWidgetClass,
				parent, margs, num_args + num_largs);
    AddToXlfdSensitiveList(fsw, box);
    XtFree((XtPointer) margs);

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNset, False); num_set = num_largs++;
    XtSetArg(largs[num_largs], XmNlabelString, DPI75_STRING(fsw));
    num_str = num_largs++;
    XtSetArg(largs[num_largs], XmNindicatorType, XmONE_OF_MANY); num_largs++;
    XtSetArg(largs[num_largs], XmNalignment, XmALIGNMENT_BEGINNING);
    num_largs++;
    margs = XtMergeArgLists(largs, num_largs, args, num_args);

    margs[num_set].value = (XtArgVal) (XmFontS_font_info(fsw)->resolution == 75);
    button = XtCreateManagedWidget("dpi75Button", xmToggleButtonWidgetClass,
				   box, margs, num_args + num_largs);
    XtAddCallback(button, XmNvalueChangedCallback, Toggle75DPI, fsw);
    XmFontS_dpi75(fsw) = button;

    margs[num_str].value = (XtArgVal) DPI100_STRING(fsw);
    margs[num_set].value = (XtArgVal) (XmFontS_font_info(fsw)->resolution == 100);
    button = XtCreateManagedWidget("dpi100Button", xmToggleButtonWidgetClass,
				   box, margs, num_args + num_largs);
    XtAddCallback(button, XmNvalueChangedCallback, Toggle100DPI, fsw);
    XmFontS_dpi100(fsw) = button;

    margs[num_str].value = (XtArgVal) BOTH_STRING(fsw);
    margs[num_set].value = (XtArgVal) False;
    button = XtCreateManagedWidget("anyButton", xmToggleButtonWidgetClass,
				   box, margs, num_args + num_largs);
    XtAddCallback(button, XmNvalueChangedCallback, ToggleBothDPI, fsw);
    XmFontS_dpiAny(fsw) = button;

    XtFree((XtPointer) margs);
}

/*	Function Name: CreateOtherChoiceBox
 *	Description: Creates the box to allow other choices to be made.
 *	Arguments: fsw - the file selection widget.
 *                 parent - the parent of this area.
 *                 args, num_args - arguments to the font selector (filtered).
 *	Returns: none.
 */

static void
CreateOtherChoiceBox(XmFontSelectorWidget fsw, Widget parent,
		     ArgList args, Cardinal num_args)
{
    Widget box, button;
    Cardinal num_largs, num_str, num_set;
    Arg *margs, largs[10];

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNorientation, XmHORIZONTAL); num_largs++;
    XtSetArg(largs[num_largs], XmNfillOption, XmFillMajor); num_largs++;
    XtSetArg(largs[num_largs], XmNequalSize, True); num_largs++;
    XtSetArg(largs[num_largs], XmNshowSash, False); num_largs++;
    margs = XtMergeArgLists(largs, num_largs, args, num_args);
    box = XtCreateManagedWidget("otherChoiceBox", xmButtonBoxWidgetClass,
				parent, margs, num_args + num_largs);
    XtFree((XtPointer) margs);

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNset, False); num_set = num_largs++;
    XtSetArg(largs[num_largs], XmNlabelString, SCALING_STRING(fsw));
    num_str = num_largs++;
    XtSetArg(largs[num_largs], XmNindicatorType, XmN_OF_MANY); num_largs++;
    XtSetArg(largs[num_largs], XmNalignment, XmALIGNMENT_BEGINNING);
    num_largs++;
    margs = XtMergeArgLists(largs, num_largs, args, num_args);

    margs[num_set].value = (XtArgVal) XmFontS_use_scaling(fsw);
    button = XtCreateManagedWidget("scalingButton", xmToggleButtonWidgetClass,
				   box, margs, num_args + num_largs);
    XmFontS_use_scaling_toggle(fsw) = button;
    AddToXlfdSensitiveList(fsw, button);
    XtAddCallback(button, XmNvalueChangedCallback,
		  ToggleScaling, (XtPointer) fsw);

    margs[num_str].value = (XtArgVal) SHOW_NAME_STRING(fsw);
    margs[num_set].value = (XtArgVal) XmFontS_show_font_name(fsw);
    button = XtCreateManagedWidget("showNameButton",
				   xmToggleButtonWidgetClass,
				   box, margs, num_args + num_largs);
    XmFontS_show_font_toggle(fsw) = button;

    XtAddCallback(button, XmNvalueChangedCallback,
		  ToggleNameWindow, (XtPointer) fsw);

    XmFontS_option_menu(fsw) = CreateEncodingMenu(fsw, box, args, num_args);
    AddToXlfdSensitiveList(fsw, XmFontS_option_menu(fsw));

    XtFree((XtPointer) margs);
}

/*	Function Name: CreateEncodingMenu
 *	Description: Creates the encoding option menu.
 *	Arguments: fsw - The font selector widget.
 *                 parent - The parent of the option menu.
 *                 args - Args used to create the font selector.
 *	Returns: The Id of the menu created.
 */

static Widget
CreateEncodingMenu(XmFontSelectorWidget fsw,
		   Widget box, ArgList args, Cardinal num_args)
{
    Widget   omenu, history = NULL, pulldownMenu, button, menuShell,
             destroy_old_menu;
    Cardinal num_largs, button_label;
    int      current, i;
    Arg      *margs, largs[10];
    String   *encodings;

    /*
     * Create the option menu only the first time through, otherwise
     * simply blow away the encoding_menu_shell and recreate that.
     */

    destroy_old_menu = XmFontS_encoding_menu_shell(fsw);

    if (destroy_old_menu == NULL)
    {
	num_largs = 0;
	XtSetArg(largs[num_largs],
		 XmNrowColumnType, XmMENU_OPTION); num_largs++;
	XtSetArg(largs[num_largs],
		 XmNlabelString, ENCODING_ONLY_STRING(fsw)); num_largs++;
	XtSetArg(largs[num_largs],
		 XmNoptionLabel, ENCODING_ONLY_STRING(fsw)); num_largs++;
	margs = XtMergeArgLists(largs, num_largs, args, num_args);
	omenu = XtCreateWidget("encodingOptionMenu", xmRowColumnWidgetClass,
			       box, margs, num_args + num_largs);
	XtFree((XtPointer) margs);
    }
    else
    {
	omenu = XmFontS_option_menu(fsw);
    }

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNwidth, 1); num_largs++;
    XtSetArg(largs[num_largs], XmNheight, 1); num_largs++;
    XtSetArg(largs[num_largs], XmNancestorSensitive, True); num_largs++;
    margs = XtMergeArgLists(largs, num_largs, args, num_args);

    menuShell = XtCreatePopupShell("menuShell", xmMenuShellWidgetClass,
				   box, margs, num_args + num_largs);
    XtFree((XtPointer) margs);

    XmFontS_encoding_menu_shell(fsw) = menuShell;

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNrowColumnType, XmMENU_PULLDOWN); num_largs++;
    margs = XtMergeArgLists(largs, num_largs, args, num_args);
    pulldownMenu = XtCreateWidget("pulldownMenu",
				  xmRowColumnWidgetClass,
				  menuShell, margs, num_args + num_largs);
    XtFree((XtPointer) margs);

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNlabelString, ANY_STRING(fsw));
    button_label = num_largs++;
    margs = XtMergeArgLists(largs, num_largs, args, num_args);

    button = XtCreateManagedWidget("button_0", xmPushButtonWidgetClass,
				   pulldownMenu, margs,
				   num_args + num_largs);

    XtAddCallback(button,
		  XmNactivateCallback, ChangeEncoding, NULL);

    current = 0;
    for (i = 1, encodings = ENCODING_LIST(fsw) ;
	 *encodings != NULL; i++, encodings++)
    {
	char name[BUFSIZ];
	XmString label = XmStringCreateLocalized(*encodings);

	margs[button_label].value = (XtArgVal) label;
	sprintf(name, "button_%d", i);
	button = XtCreateManagedWidget(name, xmPushButtonWidgetClass,
				       pulldownMenu, margs,
				       num_args + num_largs);
	XmStringFree(label);

	XtAddCallback(button,
		      XmNactivateCallback, ChangeEncoding, (XtPointer)(intptr_t)i);

	if (streq(*encodings, ENCODING_STRING(fsw)))
	{
	    current = i;
	    history = button;
	}
    }

    XtFree((XtPointer) margs);

    num_largs = 0;
    XtSetArg(largs[num_largs], XmNsubMenuId, pulldownMenu); num_largs++;

    /*
     * DMS - Fix 2/27/96
     */
    if (history)
    {
        XtSetArg(largs[num_largs], XmNmenuHistory, history); num_largs++;
    }
    XtSetValues(omenu, largs, num_largs);

    if (destroy_old_menu != NULL)
    {
	XtDestroyWidget(destroy_old_menu);
    }

    XtManageChild(omenu);

    XtFree(ENCODING_STRING(fsw));
    if (current == 0)
	ENCODING_STRING(fsw) = XtNewString(ANY_ENCODING);
    else
	ENCODING_STRING(fsw) = XtNewString(ENCODING_LIST(fsw)[current - 1]);

    return(omenu);
}

/*	Function Name: CheckEncoding
 *	Description: Checks the encoding of a font to see if it matches
 *                   the currently selected one, and returns true on a match.
 *	Arguments: fsw - The font selector widget.
 *                 fam - The family that we are checking.
 *	Returns: True if the encodings matchd, or any is selected.
 */

static Boolean
CheckEncoding(XmFontSelectorWidget fsw, FamilyInfo *fam)
{
    int i;
    XrmQuark curr;

    if (streq(ENCODING_STRING(fsw), ANY_ENCODING) || fam->encodings == NULL)
	return(TRUE);

    curr = XrmStringToQuark(ENCODING_STRING(fsw));
    for (i = 0; TRUE; i++)
    {
	if (fam->encodings[i] == curr)
	    return(True);

	if (fam->encodings[i] == 0 || i == fam->encoding_alloc)
	    return(False);
    }
}

/************************************************************
 *
 *  Callbacks.
 *
 ************************************************************/

/*	Function Name: FamilyChanged
 *	Description: Called when the user changes the family
 *	Arguments: w - *** UNUSED ***.
 *                 fsw_ptr - pointer to the font selector widget.
 *                 junk - *** UNUSED ***.
 *	Returns: none
 */

/* ARGSUSED */
static void
FamilyChanged(Widget w, XtPointer fsw_ptr, XtPointer junk)
{
    XmFontSelectorWidget fsw = (XmFontSelectorWidget) fsw_ptr;
    FontData * cf = XmFontS_font_info(fsw)->current_font;
    String str = XmDropDownGetValue(w);
    char buf[BUFSIZ];
    XrmQuark familyq = XrmStringToQuark(str);

    if (cf->familyq == familyq)
	return;			/* no change. */

    cf->familyq = familyq;

    if (XmFontS_xlfd_mode(fsw)) {
	UpdateBoldItalic(fsw);
	UpdateFixedProportional(fsw);
	UpdateSizes(fsw);

	DisplayCurrentFont(fsw, BuildFontString(fsw, cf, buf, BUFSIZ));
    }
    else {
	DisplayCurrentFont(fsw, str);
    }

    XtFree((XtPointer) str);
}

/*	Function Name: SizeChanged
 *	Description: Called when the user changes the Size
 *	Arguments: w - *** UNUSED ***.
 *                 fsw_ptr - pointer to the font selector widget.
 *                 junk - *** UNUSED ***.
 *	Returns: none
 */

/* ARGSUSED */
static void
SizeChanged(Widget w, XtPointer fsw_ptr, XtPointer junk)
{
    XmFontSelectorWidget fsw = (XmFontSelectorWidget) fsw_ptr;
    FontData * cf = XmFontS_font_info(fsw)->current_font;
    String str = XmDropDownGetValue(w);
    char buf[BUFSIZ];
    short size;

    if (cf->point_size == (size = atoi(str) * 10))
	{
	XtFree((char*)str);
	return;			/* no change. */
	}

    cf->point_size = size;

    UpdateFamilies(fsw);

    DisplayCurrentFont(fsw, BuildFontString(fsw, cf, buf, BUFSIZ));
    XtFree((XtPointer) str);
}

/*	Function Name: ChangeEncoding
 *	Description: Called when the user changes the encoding.
 *	Arguments: w - a widget child of the font sel.
 *                 data - pointer to index of the option button.
 *                 junk - ***unused
 *	Returns: none
 */

/* ARGSUSED */
static void
ChangeEncoding(Widget w, XtPointer data, XtPointer junk)
{
    XmFontSelectorWidget fsw;
    FontData *cf;
    char buf[BUFSIZ];

    for ( ; !XtIsSubclass(w, xmFontSelectorWidgetClass); w = XtParent(w)) {}
    fsw = (XmFontSelectorWidget) w;
    cf = XmFontS_font_info(fsw)->current_font;

    if (!data)
	{
	XtFree(ENCODING_STRING(fsw));
	ENCODING_STRING(fsw) = XtNewString(ANY_ENCODING);
	}
    else
	{
	XtFree(ENCODING_STRING(fsw));
	ENCODING_STRING(fsw) = XtNewString(ENCODING_LIST(fsw)[(intptr_t)data - 1]);
	}

    UpdateFamilies(fsw);
    DisplayCurrentFont(fsw, BuildFontString(fsw, cf, buf, sizeof buf));
}

/*	Function Name: ToggleScaling
 *	Description: Called when the user toggles the Font Scaling button.
 *	Arguments: w - *** UNUSED ***.
 *                 fsw_ptr - pointer to the font selector widget.
 *                 data - the toggle button data.
 *	Returns: none
 */

/* ARGSUSED */
static void
ToggleScaling(Widget w, XtPointer fsw_ptr, XtPointer data)
{
    XmFontSelectorWidget fsw = (XmFontSelectorWidget) fsw_ptr;
    FontData * cf = XmFontS_font_info(fsw)->current_font;
    FamilyInfo *family;
    XmToggleButtonCallbackStruct *info;
    LongFlag map;
    char buf[BUFSIZ];

    info = (XmToggleButtonCallbackStruct *) data;
    if ((family = FindFamily(cf->familyq, XmFontS_font_info(fsw)->family_info,
			     XmFontS_font_info(fsw)->num_families)) == NULL)
    {
	String params[1];
	Cardinal num = 1;

	params[0] = XrmQuarkToString(cf->familyq);
	dbg(); _XmWarningMsg((Widget) fsw, XmNcouldNotFindFamilyData,
		XmNcouldNotFindFamilyDataMsg, params, num);
	return;
    }

    XmFontS_use_scaling(fsw) = info->set;
    if (!info->set) {
	Boolean map_bad = (map = SizeMapping(cf->point_size)) == 0;
	Boolean bad_75 = (CheckFlag(XmFontS_user_state(fsw), DPI_75) &&
			  !CheckLongFlag(family->sizes_75, map));
	Boolean bad_100 = (CheckFlag(XmFontS_user_state(fsw), DPI_100) &&
			   !CheckLongFlag(family->sizes_100, map));

	if (map_bad || bad_75 || (bad_100 && (cf->point_size != 0))) {
	    String temp = _XmGetMBStringFromXmString(ANY_STRING(fsw));

	    cf->point_size = 0;	/* Reset to Any. */
	    SetComboValue(XmFontS_size_box(fsw), temp);
	    XtFree((XtPointer) temp);
	}
    }

    UpdateSizes(fsw);
    UpdateFamilies(fsw);
    DisplayCurrentFont(fsw, BuildFontString(fsw, cf, buf, BUFSIZ));
}

/*	Function Name: ToggleBold
 *	Description: Called when the user toggles the bold button.
 *	Arguments: w - *** UNUSED ***.
 *                 fsw_ptr - pointer to the font selector widget.
 *                 data - the toggle button data.
 *	Returns: none
 */

/* ARGSUSED */
static void
ToggleBold(Widget w, XtPointer fsw_ptr, XtPointer data)
{
    XmFontSelectorWidget fsw = (XmFontSelectorWidget) fsw_ptr;
    FontData * cf = XmFontS_font_info(fsw)->current_font;
    FamilyInfo *family;
    XmToggleButtonCallbackStruct *info;
    char buf[BUFSIZ];

    info = (XmToggleButtonCallbackStruct *) data;
    if ((family = FindFamily(cf->familyq, XmFontS_font_info(fsw)->family_info,
			     XmFontS_font_info(fsw)->num_families)) == NULL)
    {
	String params[1];
	Cardinal num = 1;

	params[0] = XrmQuarkToString(cf->familyq);
	dbg(); _XmWarningMsg((Widget) fsw, XmNcouldNotFindFamilyData,
		XmNcouldNotFindFamilyDataMsg, params, num);

	return;
    }

    if (info->set)
	cf->weightq = family->bold_nameq;
    else
	cf->weightq = family->medium_nameq;

    SetFlag(&(cf->state), BOLD, info->set);
    SetFlag(&(XmFontS_user_state(fsw)), BOLD, info->set);

    DisplayCurrentFont(fsw, BuildFontString(fsw, cf, buf, BUFSIZ));
}

/*	Function Name: ToggleItalic
 *	Description: Called when the user toggles the italic button.
 *	Arguments: w - *** UNUSED ***.
 *                 fsw_ptr - pointer to the font selector widget.
 *                 data - the toggle button data.
 *	Returns: none
 */

/* ARGSUSED */
static void
ToggleItalic(Widget w, XtPointer fsw_ptr, XtPointer data)
{
    XmFontSelectorWidget fsw = (XmFontSelectorWidget) fsw_ptr;
    FontData * cf = XmFontS_font_info(fsw)->current_font;
    FamilyInfo *family;
    XmToggleButtonCallbackStruct *info;
    char buf[BUFSIZ];

    info = (XmToggleButtonCallbackStruct *) data;
    if ((family = FindFamily(cf->familyq, XmFontS_font_info(fsw)->family_info,
			     XmFontS_font_info(fsw)->num_families)) == NULL)
    {
	String params[1];
	Cardinal num = 1;

	params[0] = XrmQuarkToString(cf->familyq);
	dbg(); _XmWarningMsg((Widget) fsw, XmNcouldNotFindFamilyData,
		XmNcouldNotFindFamilyDataMsg, params, num);
	return;
    }

    if (info->set)
	strcpy(cf->slant, XrmQuarkToString(family->italic_nameq));
    else
	strcpy(cf->slant, XrmQuarkToString(family->upright_nameq));

    SetFlag(&(cf->state), ITALIC, info->set);
    SetFlag(&(XmFontS_user_state(fsw)), ITALIC, info->set);
    DisplayCurrentFont(fsw, BuildFontString(fsw, cf, buf, BUFSIZ));
}

/*	Function Name: ToggleMiddlePane
 *	Description: toggles whether or not the middle pane is shown
 *	Arguments: w - *** UNUSED ***.
 *                 fsw_ptr - pointer to the font selector widget.
 *                 data - the toggle button data.
 *	Returns: none
 */

/* ARGSUSED */
static void
ToggleMiddlePane(Widget w, XtPointer fsw_ptr, XtPointer data)
{
    XmFontSelectorWidget fsw = (XmFontSelectorWidget) fsw_ptr;
    XmToggleButtonCallbackStruct *info;

    info = (XmToggleButtonCallbackStruct *) data;

    if (info->set)
	XtManageChild(XmFontS_middle_pane(fsw));
    else
	XtUnmanageChild(XmFontS_middle_pane(fsw));
}

/*	Function Name: ToggleNameWindow
 *	Description: toggles whether or not the middle pane is shown
 *	Arguments: w - *** UNUSED ***.
 *                 fsw_ptr - pointer to the font selector widget.
 *                 data - the toggle button data.
 *	Returns: none
 */

/* ARGSUSED */
static void
ToggleNameWindow(Widget w, XtPointer fsw_ptr, XtPointer data)
{
    XmFontSelectorWidget fsw = (XmFontSelectorWidget) fsw_ptr;
    XmToggleButtonCallbackStruct *info;
    char buf[BUFSIZ];

    info = (XmToggleButtonCallbackStruct *) data;

    XmFontS_show_font_name(fsw) = info->set;
    if (info->set) {
	XtManageChild(XmFontS_name_label(fsw));

	if (XmFontS_xlfd_mode(fsw)) {
	    DisplayCurrentFont(fsw,
			       BuildFontString(fsw,
					       XmFontS_font_info(fsw)->current_font,
					       buf, BUFSIZ));
	}
	else {
	    String str = XmDropDownGetValue(XmFontS_family_box(fsw));
	    DisplayCurrentFont(fsw, str);
	    XtFree((XtPointer) str);
	}
    }
    else
	XtUnmanageChild(XmFontS_name_label(fsw));
}

/*	Function Name: Toggle75DPI
 *	Description: Activated when the 75DPI button is toggled.
 *                 w - the toggle button widget selected.
 *                 fsw_ptr - pointer to the font selector widget.
 *                 data - the toggle button data.
 *	Returns: none
 */

static void
Toggle75DPI(Widget w, XtPointer fsw_ptr, XtPointer data)
{
    XmFontSelectorWidget fsw = (XmFontSelectorWidget) fsw_ptr;
    FontData * cf = XmFontS_font_info(fsw)->current_font;
    XmToggleButtonCallbackStruct *info;
    char buf[BUFSIZ];

    info = (XmToggleButtonCallbackStruct *) data;

    UnsetSiblings(w);

    if (!info->set)
	return;			/* Do nothing on an unset. */

    SetFlag(&(XmFontS_user_state(fsw)), DPI_75, True);
    SetFlag(&(XmFontS_user_state(fsw)), DPI_100, False);
    cf->resolution_x = cf->resolution_y = 75;

    UpdateFamilies(fsw);
    UpdateSizes(fsw);
    DisplayCurrentFont(fsw, BuildFontString(fsw, cf, buf, BUFSIZ));
}

/*	Function Name: Toggle100DPI
 *	Description: Activated when the 100DPI button is toggled.
 *                 w - the toggle button widget selected.
 *                 fsw_ptr - pointer to the font selector widget.
 *                 data - the toggle button data.
 *	Returns: none
 */

static void
Toggle100DPI(Widget w, XtPointer fsw_ptr, XtPointer data)
{
    XmFontSelectorWidget fsw = (XmFontSelectorWidget) fsw_ptr;
    FontData * cf = XmFontS_font_info(fsw)->current_font;
    XmToggleButtonCallbackStruct *info;
    char buf[BUFSIZ];

    info = (XmToggleButtonCallbackStruct *) data;

    UnsetSiblings(w);

    if (!info->set)
	return;			/* Do nothing on an unset. */

    SetFlag(&(XmFontS_user_state(fsw)), DPI_75, False);
    SetFlag(&(XmFontS_user_state(fsw)), DPI_100, True);

    cf->resolution_x = cf->resolution_y = 100;

    UpdateFamilies(fsw);
    UpdateSizes(fsw);
    DisplayCurrentFont(fsw, BuildFontString(fsw, cf, buf, BUFSIZ));
}

/*	Function Name: ToggleBothDPI
 *	Description: Activated when the Both DPI button is toggled.
 *                 w - the toggle button widget selected.
 *                 fsw_ptr - pointer to the font selector widget.
 *                 data - the toggle button data.
 *	Returns: none
 */

static void
ToggleBothDPI(Widget w, XtPointer fsw_ptr, XtPointer data)
{
    XmFontSelectorWidget fsw = (XmFontSelectorWidget) fsw_ptr;
    FontData * cf = XmFontS_font_info(fsw)->current_font;
    XmToggleButtonCallbackStruct *info;
    char buf[BUFSIZ];

    info = (XmToggleButtonCallbackStruct *) data;

    UnsetSiblings(w);

    if (!info->set)
	return;			/* Do nothing on an unset. */

    SetFlag(&(XmFontS_user_state(fsw)), DPI_75 | DPI_100, True);
    cf->resolution_x = cf->resolution_y = 0; /* 0 means any */

    UpdateFamilies(fsw);
    UpdateSizes(fsw);
    DisplayCurrentFont(fsw, BuildFontString(fsw, cf, buf, BUFSIZ));
}

/*	Function Name: ToggleProportional
 *	Description: Activated when the Proportional button is toggled.
 *                 w - the toggle button widget selected.
 *                 fsw_ptr - pointer to the font selector widget.
 *                 data - the toggle button data.
 *	Returns: none
 */

static void
ToggleProportional(Widget w, XtPointer fsw_ptr, XtPointer data)
{
    XmFontSelectorWidget fsw = (XmFontSelectorWidget) fsw_ptr;
    FontData * cf = XmFontS_font_info(fsw)->current_font;
    XmToggleButtonCallbackStruct *info;
    char buf[BUFSIZ];

    info = (XmToggleButtonCallbackStruct *) data;

    UnsetSiblings(w);

    if (!info->set)
	return;			/* Do nothing on an unset. */

    SetFlag(&(XmFontS_user_state(fsw)), USER_PROPORTIONAL, True);
    SetFlag(&(XmFontS_user_state(fsw)), USER_FIXED, False);

    UpdateFixedProportional(fsw);

    UpdateFamilies(fsw);
    UpdateSizes(fsw);
    DisplayCurrentFont(fsw, BuildFontString(fsw, cf, buf, BUFSIZ));
}

/*	Function Name: ToggleFixed
 *	Description: Activated when the Fixed button is toggled.
 *                 w - the toggle button widget selected.
 *                 fsw_ptr - pointer to the font selector widget.
 *                 data - the toggle button data.
 *	Returns: none
 */

static void
ToggleFixed(Widget w, XtPointer fsw_ptr, XtPointer data)
{
    XmFontSelectorWidget fsw = (XmFontSelectorWidget) fsw_ptr;
    FontData * cf = XmFontS_font_info(fsw)->current_font;
    XmToggleButtonCallbackStruct *info;
    char buf[BUFSIZ];

    info = (XmToggleButtonCallbackStruct *) data;

    UnsetSiblings(w);

    if (!info->set)
	return;			/* Do nothing on an unset. */

    SetFlag(&(XmFontS_user_state(fsw)), USER_FIXED, True);
    SetFlag(&(XmFontS_user_state(fsw)), USER_PROPORTIONAL, False);

    UpdateFixedProportional(fsw);
    UpdateFamilies(fsw);
    UpdateSizes(fsw);
    DisplayCurrentFont(fsw, BuildFontString(fsw, cf, buf, BUFSIZ));
}

/*	Function Name: ToggleBothSpacing
 *	Description: Activated when the Both (any) Spacing button is toggled.
 *                 w - the toggle button widget selected.
 *                 fsw_ptr - pointer to the font selector widget.
 *                 data - the toggle button data.
 *	Returns: none
 */

static void
ToggleBothSpacing(Widget w, XtPointer fsw_ptr, XtPointer data)
{
    XmFontSelectorWidget fsw = (XmFontSelectorWidget) fsw_ptr;
    FontData * cf = XmFontS_font_info(fsw)->current_font;
    XmToggleButtonCallbackStruct *info;
    char buf[BUFSIZ];

    info = (XmToggleButtonCallbackStruct *) data;

    UnsetSiblings(w);

    if (!info->set)
	return;			/* Do nothing on an unset. */

    SetFlag(&(XmFontS_user_state(fsw)), USER_FIXED | USER_PROPORTIONAL, True);

    UpdateFixedProportional(fsw);
    UpdateFamilies(fsw);
    UpdateSizes(fsw);
    DisplayCurrentFont(fsw, BuildFontString(fsw, cf, buf, BUFSIZ));
}

/*	Function Name: XlfdMode
 *	Description: Called when the Xlfd Toggle is selected.
 *                 w - the toggle button widget selected.
 *                 fsw_ptr - pointer to the font selector widget.
 *                 data - the toggle button data.
 *	Returns: none
 */

static void
XlfdMode(Widget w, XtPointer fsw_ptr, XtPointer data)
{
    XmFontSelectorWidget fsw = (XmFontSelectorWidget) fsw_ptr;
    XmToggleButtonCallbackStruct *info;

    info = (XmToggleButtonCallbackStruct *) data;

    UnsetSiblings(w);

    if (!info->set)
	return;			/* Do nothing on an unset. */

    ChangeMode(fsw, True, True);	/* Set to Xlfd Mode. */
}

/*	Function Name: OtherMode
 *	Description: Called when the Other Toggle is selected.
 *                 w - the toggle button widget selected.
 *                 fsw_ptr - pointer to the font selector widget.
 *                 data - the toggle button data.
 *	Returns: none
 */

static void
OtherMode(Widget w, XtPointer fsw_ptr, XtPointer data)
{
    XmFontSelectorWidget fsw = (XmFontSelectorWidget) fsw_ptr;
    XmToggleButtonCallbackStruct *info;

    info = (XmToggleButtonCallbackStruct *) data;

    UnsetSiblings(w);

    if (!info->set)
	return;			/* Do nothing on an unset. */

    ChangeMode(fsw, False, True);	/* Set to other Mode */
}

/*	Function Name: RemoveUserError
 *	Description: Removes a user error from the text widget.
 *                 w - the text widget.
 *                 fsw_ptr - pointer to the font selector widget.
 *                 data - *** UNUSED ***.
 *	Returns: none
 */

/* ARGSUSED */
static void
RemoveUserError(Widget w, XtPointer fsw_ptr, XtPointer data)
{
    XmFontSelectorWidget fsw = (XmFontSelectorWidget) fsw_ptr;

    if (XmFontS_current_text(fsw) == NULL)
	return;

    DisplayUserError(fsw, NULL);
}

/************************************************************
 *
 * Semi-public routines.
 *
 ************************************************************/

/*      Function Name: Class Initlialize
 *      Description:   Initializes class-specific data (offsets)
 *      Arguments:     none
 *      Returns:       nothing
 */
static void
ClassInitialize()
{
  /* do nothing */
}


/*
 * ClassPartInitialize sets up the fast subclassing for the widget.
 */
static void
#ifdef _NO_PROTO
ClassPartInitialize(w_class)
        WidgetClass w_class ;
#else
ClassPartInitialize(WidgetClass w_class)
#endif /* _NO_PROTO */
{
    _XmFastSubclassInit (w_class, XmFONTSELECTOR_BIT);
}


/*	Function Name: Initialize
 *	Description:   Called to initialize information specific
 *                     to this widget.
 *	Arguments:     req - what was originally requested.
 *                     set - what will be created (our superclassed have
 *                           already mucked with this)
 *                     args, num_args - The arguments passed to
 *                                      the creation call.
 *	Returns:       none.
 */

/* ARGSUSED */
static void
Initialize(Widget request, Widget set, ArgList args, Cardinal * num_args)
{
    XmFontSelectorWidget fsw = (XmFontSelectorWidget)set;
    ArgList f_args;
    Cardinal f_num_args;

    XmFontS_user_state(fsw) = 0;	/* Initialize user state to 0. */
    XmFontS_xlfd_mode(fsw) = True;	/* We start in Xlfd Mode. */
    XmFontS_font_info(fsw) = LoadFontInfo(fsw);
    XmFontS_current_text(fsw) = NULL;
    XmFontS_old_fontdata(fsw) = NULL;
    XmFontS_old_fontlist(fsw) = NULL;
    XmFontS_encoding_menu_shell(fsw) = NULL;

    XmFontS_xlfd_only(fsw) = XmFontS_xlfd_sensitive(fsw) = NULL;
    XmFontS_num_xlfd_only(fsw) = XmFontS_alloc_xlfd_only(fsw) = 0;
    XmFontS_num_xlfd_sensitive(fsw) = XmFontS_alloc_xlfd_sensitive(fsw) = 0;

    XmFontS_get_font(fsw) = (String) XtMalloc(sizeof(char) * GET_FONT_SIZE);

    /* duplicate strings so that user can query them later */
    ANY_STRING(fsw)		= XmStringCopy(ANY_STRING(fsw));
    BOLD_STRING(fsw)		= XmStringCopy(BOLD_STRING(fsw));
    BOTH_STRING(fsw)		= XmStringCopy(BOTH_STRING(fsw));
    DPI100_STRING(fsw)		= XmStringCopy(DPI100_STRING(fsw));
    DPI75_STRING(fsw)		= XmStringCopy(DPI75_STRING(fsw));
    ENCODING_ONLY_STRING(fsw)	= XmStringCopy(ENCODING_ONLY_STRING(fsw));
    FAMILY_STRING(fsw)		= XmStringCopy(FAMILY_STRING(fsw));
    ITALIC_STRING(fsw)		= XmStringCopy(ITALIC_STRING(fsw));
    LOWER_ANY_STRING(fsw)	= XmStringCopy(LOWER_ANY_STRING(fsw));
    MONO_SPACE_STRING(fsw)	= XmStringCopy(MONO_SPACE_STRING(fsw));
    OPTION_STRING(fsw)		= XmStringCopy(OPTION_STRING(fsw));
    OTHER_FONT_STRING(fsw)	= XmStringCopy(OTHER_FONT_STRING(fsw));
    PROPORTIONAL_STRING(fsw)	= XmStringCopy(PROPORTIONAL_STRING(fsw));
    SAMPLE_TEXT(fsw)		= XmStringCopy(SAMPLE_TEXT(fsw));
    SCALING_STRING(fsw)		= XmStringCopy(SCALING_STRING(fsw));
    SHOW_NAME_STRING(fsw)	= XmStringCopy(SHOW_NAME_STRING(fsw));
    SIZE_STRING(fsw)		= XmStringCopy(SIZE_STRING(fsw));
    XLFD_STRING(fsw)		= XmStringCopy(XLFD_STRING(fsw));

    ENCODING_STRING(fsw)	= XtNewString(ENCODING_STRING(fsw));
    /* XmFontS_current_font(fsw) handled internally */

    {
	int i;
	String *encodings = ENCODING_LIST(fsw);
	String *newList;
	for (i=0, encodings = ENCODING_LIST(fsw); *encodings != NULL; i++, encodings++)
		i++;
	i++;
	newList = (String*)XtMalloc(sizeof(String) * i);
	for (i=0, encodings = ENCODING_LIST(fsw); *encodings != NULL; i++, encodings++)
		newList[i] = XtNewString(*encodings);
	newList[i] = NULL;
	ENCODING_LIST(fsw) = newList;
    }

    _XmFilterArgs(args, *num_args,
		  xm_std_constraint_filter, &f_args, &f_num_args);
    CreateChildren(fsw, f_args, f_num_args);
    XtFree((XtPointer) f_args);

    SetFlag(&(XmFontS_user_state(fsw)), DPI_75,
	    (XmFontS_font_info(fsw)->resolution == 75));

    SetFlag(&(XmFontS_user_state(fsw)), DPI_100,
	    (XmFontS_font_info(fsw)->resolution == 100));

    /*
     * Load default information into the current font.
     */

    XmFontS_font_info(fsw)->current_font = (FontData *) XtMalloc(sizeof(FontData));
    SetDisplayedFont(fsw, XmFontS_current_font(fsw));

}

/*	Function Name: Destroy
 *	Description: Called when the widget is destroyed, cleans up.
 *	Arguments: w - the widget.
 *	Returns: none.
 */

static void
Destroy(Widget w)
{
    register int i, num;
    register String *ptr;
    XmFontSelectorWidget fsw = (XmFontSelectorWidget)w;

    if (XmFontS_old_fontdata(fsw) != NULL) {
	XFreeFont(XtDisplay(w), XmFontS_old_fontdata(fsw));
	XmFontListFree(XmFontS_old_fontlist(fsw));
    }

    num = XmFontS_font_info(fsw)->num_others;
    ptr = XmFontS_font_info(fsw)->others;
    for (i = 0; i < num; i++, ptr++)
	XtFree(*ptr);

    for (i = 0; i < XmFontS_font_info(fsw)->num_families; i++)
	XtFree((char*)XmFontS_font_info(fsw)->family_info[i].encodings);

    XtFree((XtPointer) XmFontS_get_font(fsw));
    XtFree((XtPointer) XmFontS_xlfd_only(fsw));
    XtFree((XtPointer) XmFontS_xlfd_sensitive(fsw));
    XtFree((XtPointer) XmFontS_font_info(fsw)->others);
    XtFree((XtPointer) XmFontS_font_info(fsw)->family_info);
    XtFree((XtPointer) XmFontS_font_info(fsw)->current_font);
    XtFree((XtPointer) XmFontS_font_info(fsw));


    XmStringFree(ANY_STRING(fsw));
    XmStringFree(BOLD_STRING(fsw));
    XmStringFree(BOTH_STRING(fsw));
    XmStringFree(DPI100_STRING(fsw));
    XmStringFree(DPI75_STRING(fsw));
    XmStringFree(ENCODING_ONLY_STRING(fsw));
    XmStringFree(FAMILY_STRING(fsw));
    XmStringFree(ITALIC_STRING(fsw));
    XmStringFree(LOWER_ANY_STRING(fsw));
    XmStringFree(MONO_SPACE_STRING(fsw));
    XmStringFree(OPTION_STRING(fsw));
    XmStringFree(OTHER_FONT_STRING(fsw));
    XmStringFree(PROPORTIONAL_STRING(fsw));
    XmStringFree(SAMPLE_TEXT(fsw));
    XmStringFree(SCALING_STRING(fsw));
    XmStringFree(SHOW_NAME_STRING(fsw));
    XmStringFree(SIZE_STRING(fsw));
    XmStringFree(XLFD_STRING(fsw));

    XtFree(ENCODING_STRING(fsw));
    /* current font handled internally */

    {
	String *encodings;
	for (encodings = ENCODING_LIST(fsw); *encodings != NULL; i++, encodings++)
		XtFree(*encodings);
	XtFree((char*)ENCODING_LIST(fsw));
    }
}

/*	Function Name: SetValues
 *	Description:   Called when some widget data needs to be modified on-
 *                     the-fly.
 *	Arguments:     old - the current (old) widget values.
 *                     request - before superclassed have changed things.
 *                     set - what will acutally be the new values.
 *                     args, num_args - The arguments passed to the set
 *                                      values call.
 *	Returns:       none
 */

/* ARGSUSED */
static Boolean
SetValues(Widget old, Widget request, Widget set,
	  ArgList args, Cardinal * num_args)
{
    Arg largs[10];
    Cardinal num_largs, i;
    XmFontSelectorWidget old_fsw = (XmFontSelectorWidget) old;
    XmFontSelectorWidget set_fsw = (XmFontSelectorWidget) set;
    Boolean new_encoding_list = False;

    /*
     * Pass argument list through to all children.
     */

    {
	ArgList f_args;
	Cardinal f_num_args;

	_XmFilterArgs(args, *num_args,
		      xm_std_constraint_filter, &f_args, &f_num_args);
	_XmSetValuesOnChildren(set, f_args, f_num_args);
	XtFree((XtPointer) f_args);
    }

    for (i = 0; i < *num_args; i++)
    {
	if (streq(args[i].name, XmNencodingList))
	    new_encoding_list = True;
    }

    /*
     * ||| Protect against any of the strings being changed.
     */

    if (XmFontS_current_font(old_fsw) != XmFontS_current_font(set_fsw)) {
	SetDisplayedFont(set_fsw, XmFontS_current_font(set_fsw));
    }

    if (XmFontS_show_font_name(old_fsw) != XmFontS_show_font_name(set_fsw)) {
	XmToggleButtonSetState(XmFontS_show_font_toggle(set_fsw),
			       XmFontS_show_font_name(set_fsw), True);
    }

    if (XmFontS_use_scaling(old_fsw) != XmFontS_use_scaling(set_fsw)) {
	XmToggleButtonSetState(XmFontS_use_scaling_toggle(set_fsw),
			       XmFontS_use_scaling(set_fsw), True);
    }

    if (new_encoding_list)
    {
	Widget parent = XtParent(XmFontS_option_menu(set_fsw));
	XmFontS_option_menu(set_fsw) = CreateEncodingMenu(set_fsw,
						     parent, NULL, 0);
    }

    /* if new_encoding_list is true, then CreateEncodingMenu has already
    ** freed and recreated the ENCODING_STRING, so avoid redoing it
    */
    if (new_encoding_list ||
	(((ENCODING_STRING(old_fsw) != NULL) && (ENCODING_STRING(set_fsw) != NULL))
	&& !streq(ENCODING_STRING(old_fsw), ENCODING_STRING(set_fsw)))
	)
    {
	char buf[BUFSIZ];
	int current;
	String *encodings = ENCODING_LIST(set_fsw);
	Widget button;

	if (!new_encoding_list)
	{
		XtFree(ENCODING_STRING(old_fsw));
		ENCODING_STRING(set_fsw) = XtNewString(ENCODING_STRING(set_fsw));
	}

	/* some of this code is probably a duplicate of CreateEncodingMenu */
	for (current = 0, i = 1 ; *encodings != NULL; i++, encodings++)
	{
	    /*
	     * If the default encoding matches this one then make this
	     * the current item in the option menu.
	     */
	    if (strcmp(*encodings, ENCODING_STRING(set_fsw)) == 0)
		current = i;
	}

	sprintf(buf, "*button_%d", current);
	if ((button =
	     XtNameToWidget(XtParent(XmFontS_option_menu(set_fsw)), buf)) != NULL)
	{
	    num_largs = 0;
	    XtSetArg(largs[num_largs], XmNmenuHistory, button); num_largs++;
	    XtSetValues(XmFontS_option_menu(set_fsw), largs, num_largs);
	    ChangeEncoding((Widget) set_fsw, (XtPointer)(intptr_t)current, NULL);
	}
	else
	{
	    /* ||| ERROR. */
	}
    }

    if (XmFontS_text_rows(old_fsw) != XmFontS_text_rows(set_fsw)) {
	num_largs = 0;
	XtSetArg(largs[num_largs], XmNrows, XmFontS_text_rows(set_fsw));num_largs++;
	XtSetValues(XmFontS_text(set_fsw), largs, num_largs);
    }

	{
	Boolean reset = False;
#define DoCheck(old_one,new_one)	{ reset = False; if (old_one != new_one) { XmStringFree(old_one); new_one = XmStringCopy(new_one); reset = True; } }

	DoCheck( ANY_STRING(old_fsw), ANY_STRING(set_fsw)); /* PUNT! or else we need to check if on-screen right now */
	DoCheck( BOLD_STRING(old_fsw), BOLD_STRING(set_fsw)); if (reset) XtVaSetValues( XmFontS_bold_toggle(set_fsw), XmNlabelString, BOLD_STRING(set_fsw), NULL);
	DoCheck( BOTH_STRING(old_fsw), BOTH_STRING(set_fsw)); if (reset) { XtVaSetValues( XmFontS_dpiAny(set_fsw), XmNlabelString, BOTH_STRING(set_fsw), NULL); XtVaSetValues( XmFontS_any_spacing(set_fsw), XmNlabelString, BOTH_STRING(set_fsw), NULL); }
	DoCheck( DPI100_STRING(old_fsw), DPI100_STRING(set_fsw)); if (reset) XtVaSetValues( XmFontS_dpi100(set_fsw), XmNlabelString, DPI100_STRING(set_fsw), NULL);
	DoCheck( DPI75_STRING(old_fsw), DPI75_STRING(set_fsw)); if (reset) XtVaSetValues( XmFontS_dpi75(set_fsw), XmNlabelString, DPI75_STRING(set_fsw), NULL);
	DoCheck( ENCODING_ONLY_STRING(old_fsw), ENCODING_ONLY_STRING(set_fsw)); if (reset) XtVaSetValues( XmFontS_option_menu(set_fsw), XmNlabelString, ENCODING_ONLY_STRING(set_fsw), NULL);
	DoCheck( FAMILY_STRING(old_fsw), FAMILY_STRING(set_fsw)); if (reset) XtVaSetValues( XmFontS_family_box(set_fsw), XmNlabelString, FAMILY_STRING(set_fsw), NULL);
	DoCheck( ITALIC_STRING(old_fsw), ITALIC_STRING(set_fsw)); if (reset) XtVaSetValues( XmFontS_italic_toggle(set_fsw), XmNlabelString, ITALIC_STRING(set_fsw), NULL);
	DoCheck( LOWER_ANY_STRING(old_fsw), LOWER_ANY_STRING(set_fsw));		/* PUNT! or else we need to check if on-screen right now */
	DoCheck( MONO_SPACE_STRING(old_fsw), MONO_SPACE_STRING(set_fsw)); if (reset) XtVaSetValues( XmFontS_monospace(set_fsw), XmNlabelString, MONO_SPACE_STRING(set_fsw), NULL);
	DoCheck( OPTION_STRING(old_fsw), OPTION_STRING(set_fsw)); if (reset) XtVaSetValues( XtNameToWidget((Widget)set_fsw, "*optionButton"), XmNlabelString, OPTION_STRING(set_fsw), NULL);
	DoCheck( OTHER_FONT_STRING(old_fsw), OTHER_FONT_STRING(set_fsw)); if (reset) XtVaSetValues( XmFontS_other_toggle(set_fsw), XmNlabelString, OTHER_FONT_STRING(set_fsw), NULL);
	DoCheck( PROPORTIONAL_STRING(old_fsw), PROPORTIONAL_STRING(set_fsw)); if (reset) XtVaSetValues( XmFontS_proportional(set_fsw), XmNlabelString, PROPORTIONAL_STRING(set_fsw), NULL);
	DoCheck( SAMPLE_TEXT(old_fsw), SAMPLE_TEXT(set_fsw));
		if (reset)
		{
		char * temp_txt = _XmGetMBStringFromXmString(SAMPLE_TEXT(set_fsw));
		XtVaSetValues( XmFontS_text(set_fsw), XmNlabelString, temp_txt, NULL);
		XtFree(temp_txt);
		}
	DoCheck( SCALING_STRING(old_fsw), SCALING_STRING(set_fsw)); if (reset) XtVaSetValues( XmFontS_use_scaling_toggle(set_fsw), XmNlabelString, SCALING_STRING(set_fsw), NULL);
	DoCheck( SHOW_NAME_STRING(old_fsw), SHOW_NAME_STRING(set_fsw)); if (reset) XtVaSetValues( XmFontS_show_font_toggle(set_fsw), XmNlabelString, SHOW_NAME_STRING(set_fsw), NULL);
	DoCheck( SIZE_STRING(old_fsw), SIZE_STRING(set_fsw)); if (reset) XtVaSetValues( XmFontS_size_box(set_fsw), XmNlabelString, SIZE_STRING(set_fsw), NULL);
	DoCheck( XLFD_STRING(old_fsw), XLFD_STRING(set_fsw)); if (reset) XtVaSetValues( XmFontS_xlfd_toggle(set_fsw), XmNlabelString, XLFD_STRING(set_fsw), NULL);

#undef DoCheck
	}

    return (False);
} /* SetValues */

/*	Function Name: GetValuesHook
 *	Description:   Packages up the value of the current font.
 *	Arguments:     w - The widget.
 *                     args, num_args - The arguments passed to the get
 *                                      values call.
 *	Returns:       none
 */

/* ARGSUSED */
static void
GetValuesHook(Widget w, ArgList args, Cardinal * num_args)
{
    XmFontSelectorWidget fsw = (XmFontSelectorWidget) w;
    FontData * cf;
    String *str_ptr;
    register int i;

    for (i = 0; i < *num_args; i++) {
	if (streq(args[i].name, XmNcurrentFont))
	{
		cf = XmFontS_font_info(fsw)->current_font;

		str_ptr = (String *) args[i].value;
		if (XmFontS_xlfd_mode(fsw)) {
		    BuildFontString(fsw, cf, XmFontS_get_font(fsw), GET_FONT_SIZE);
		    *str_ptr = XmFontS_get_font(fsw);
		}
		else {
		    *str_ptr = XrmQuarkToString(cf->familyq);
		}
	}
	else if (streq(args[i].name, XmNanyString))		*(XmString*)args[i].value = XmStringCopy(ANY_STRING(fsw));
	else if (streq(args[i].name, XmNbothString))		*(XmString*)args[i].value = XmStringCopy(BOLD_STRING(fsw));
	else if (streq(args[i].name, XmNboldString))		*(XmString*)args[i].value = XmStringCopy(BOTH_STRING(fsw));
	else if (streq(args[i].name, XmN100DPIstring))		*(XmString*)args[i].value = XmStringCopy(DPI100_STRING(fsw));
	else if (streq(args[i].name, XmN75DPIstring))		*(XmString*)args[i].value = XmStringCopy(DPI75_STRING(fsw));
	else if (streq(args[i].name, XmNencodingString))	*(XmString*)args[i].value = XmStringCopy(ENCODING_ONLY_STRING(fsw));
	else if (streq(args[i].name, XmNfamilyString))		*(XmString*)args[i].value = XmStringCopy(FAMILY_STRING(fsw));
	else if (streq(args[i].name, XmNitalicString))		*(XmString*)args[i].value = XmStringCopy(ITALIC_STRING(fsw));
	else if (streq(args[i].name, XmNanyLowerString))	*(XmString*)args[i].value = XmStringCopy(LOWER_ANY_STRING(fsw));
	else if (streq(args[i].name, XmNmonoSpaceString))	*(XmString*)args[i].value = XmStringCopy(MONO_SPACE_STRING(fsw));
	else if (streq(args[i].name, XmNoptionString))		*(XmString*)args[i].value = XmStringCopy(OPTION_STRING(fsw));
	else if (streq(args[i].name, XmNotherString))		*(XmString*)args[i].value = XmStringCopy(OTHER_FONT_STRING(fsw));
	else if (streq(args[i].name, XmNpropSpaceString))	*(XmString*)args[i].value = XmStringCopy(PROPORTIONAL_STRING(fsw));
	else if (streq(args[i].name, XmNsampleText))		*(XmString*)args[i].value = XmStringCopy(SAMPLE_TEXT(fsw));
	else if (streq(args[i].name, XmNscalingString))		*(XmString*)args[i].value = XmStringCopy(SCALING_STRING(fsw));
	else if (streq(args[i].name, XmNshowNameString))	*(XmString*)args[i].value = XmStringCopy(SHOW_NAME_STRING(fsw));
	else if (streq(args[i].name, XmNsizeString))		*(XmString*)args[i].value = XmStringCopy(SIZE_STRING(fsw));
	else if (streq(args[i].name, XmNxlfdString))		*(XmString*)args[i].value = XmStringCopy(XLFD_STRING(fsw));
    }
}

/************************************************************
 *
 * Public routines.
 *
 ************************************************************/

/*	Function Name: XmCreateFontSelector
 *	Description: Creation Routine for UIL and ADA.
 *	Arguments: parent - the parent widget.
 *                 name - the name of the widget.
 *                 args, num_args - the number and list of args.
 *	Returns: The created widget.
 */

Widget
XmCreateFontSelector(Widget parent, String name,
		     ArgList args, Cardinal num_args)
{
    return(XtCreateWidget(name, xmFontSelectorWidgetClass,
			  parent, args, num_args));
}
