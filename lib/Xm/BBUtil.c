/* $TOG: BBUtil.c /main/8 1997/04/15 10:23:34 dbl $ */
/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option)
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
/*
 * HISTORY
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <Xm/Xm.h>
#include <Xm/PushBG.h>
#include <Xm/LabelG.h>
#include <Xm/TraitP.h>
#include <Xm/TakesDefT.h>
#include "BulletinBI.h"
#include "MessagesI.h"

/* defines for label string resources coming from message catalog */
#define OK_STRING         _XmMMsgResource_0001
#define CANCEL_STRING     _XmMMsgResource_0002
#define SELECTION_STRING  _XmMMsgResource_0003
#define APPLY_STRING      _XmMMsgResource_0004
#define HELP_STRING       _XmMMsgResource_0005
#define FILTER_STRING     _XmMMsgResource_0006
#define DIRLIST_STRING    _XmMMsgResource_0008
#define ITEMS_STRING      _XmMMsgResource_0009
#define DIRTEXT_STRING    _XmMMsgResource_0011
#define PROMPT_STRING     _XmMMsgResource_0012



/****************************************************************/
static char *
GetLabelString(
       XmLabelStringLoc l_loc )
{
    char *retval = (char*)NULL;
    switch (l_loc)
	{
	case XmOkStringLoc:
	    retval = OK_STRING;
	    break;
	case XmCancelStringLoc:
	    retval = CANCEL_STRING;
	    break;
	case XmSelectionStringLoc:
	    retval = SELECTION_STRING;
	    break;
	case XmApplyStringLoc:
	    retval = APPLY_STRING;
	    break;
	case XmHelpStringLoc:
	    retval = HELP_STRING;
	    break;
	case XmFilterStringLoc:
	    retval = FILTER_STRING;
	    break;
	case XmDirListStringLoc:
	    retval = DIRLIST_STRING;
	    break;
	case XmItemsStringLoc:
	    retval = ITEMS_STRING;
	    break;
	case XmDirTextStringLoc:
	    retval = DIRTEXT_STRING;
	    break;
	case XmPromptStringLoc:
	    retval = PROMPT_STRING;
	    break;
	default:
		/* internal error */
		break;
	}
    return retval;
}


/****************************************************************/
Widget 
_XmBB_CreateButtonG(
        Widget bb,
        XmString l_string,
	char *name,
        XmLabelStringLoc l_loc )
{
    Arg		        al[10] ;
    register Cardinal   ac = 0 ;
    Widget              button ;
    XmTakesDefaultTrait trait_default ;
    XmString            default_label_string_loc = NULL;
/****************/

    if(    l_string    )
	{
	    XtSetArg( al[ac], XmNlabelString, l_string) ; ac++ ;
        }
    else
	{
	    default_label_string_loc = XmStringCreate(GetLabelString(l_loc),
						      XmFONTLIST_DEFAULT_TAG);
	    XtSetArg( al[ac], XmNlabelString, default_label_string_loc); ac++;
	}
	    
    XtSetArg( al[ac], XmNstringDirection, BB_StringDirection( bb)) ; ac++ ;

    button = XmCreatePushButtonGadget( (Widget) bb, name, al, ac) ;

    trait_default = (XmTakesDefaultTrait) XmeTraitGet((XtPointer)
						      XtClass(button), 
						      XmQTtakesDefault) ;
    if (trait_default) 
	trait_default->showAsDefault  (button, XmDEFAULT_READY);

    if (default_label_string_loc)
	XmStringFree(default_label_string_loc);

    return( button ) ;
}

/****************************************************************/
Widget 
_XmBB_CreateLabelG(
        Widget bb,
        XmString l_string,
        char *name,
        XmLabelStringLoc l_loc )
{
            Arg		    al[10] ;
    register int            ac = 0 ;
    Widget                  label ;
    XmString                default_label_string_loc = NULL;
/****************/

    if(    l_string    )
	{
	    XtSetArg( al[ac], XmNlabelString, l_string) ; ac++ ;
        }
    else
	{
	    default_label_string_loc = XmStringCreate(GetLabelString(l_loc),
						      XmFONTLIST_DEFAULT_TAG);
	    XtSetArg( al[ac], XmNlabelString, default_label_string_loc); ac++;
	}

    XtSetArg( al[ac], XmNstringDirection, BB_StringDirection( bb)) ; ac++ ;
    XtSetArg( al[ac], XmNhighlightThickness, 0) ; ac++ ;
    XtSetArg( al[ac], XmNtraversalOn, False) ; ac++ ;
    XtSetArg( al[ac], XmNalignment, XmALIGNMENT_BEGINNING) ; ac++ ;

    label = XmCreateLabelGadget( bb, name, al, ac);

    if (default_label_string_loc)
	XmStringFree(default_label_string_loc);

    return( label ) ;
	
}

