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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: SelectioB.c /main/19 1997/06/18 17:42:11 samborn $"
#endif
#endif

/*-------------------------------------------------------------------------
**
**	include files
**
**-------------------------------------------------------------------------
*/
#include <stdlib.h>
#include <Xm/AccTextT.h>
#include <Xm/ActivatableT.h>
#include <Xm/ArrowB.h>
#include <Xm/DialogS.h>
#include <Xm/GadgetP.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumnP.h>
#include <Xm/SeparatoG.h>
#include <Xm/TextF.h>
#include <Xm/TraitP.h>
#include <Xm/TransltnsP.h>
#include <Xm/VaSimpleP.h>
#include "BulletinBI.h"
#include "GeoUtilsI.h"
#include "GMUtilsI.h"
#include "ManagerI.h"
#include "MessagesI.h"
#include "RepTypeI.h"
#include "SelectioBI.h"
#include "TransferI.h"
#include "XmI.h"
#include "XmStringI.h"

#define WARN_DIALOG_TYPE_CHANGE _XmMMsgSelectioB_0001
#define WARN_CHILD_TYPE		_XmMMsgSelectioB_0002
#define MESSAGE0     		_XmMMsgMotif_0001

#define defaultTextAccelerators		_XmSelectioB_defaultTextAccelerators

#define IsButton(w) \
(((XtPointer) XmeTraitGet((XtPointer) XtClass((w)), XmQTactivatable) != NULL))

#define IsAutoButton(sb, w) ( \
      w == SB_OkButton(sb)     || \
      w == SB_ApplyButton(sb)  || \
      w == SB_CancelButton(sb) || \
      w == SB_HelpButton(sb))

#define SetupWorkArea(sb) \
    if (_XmGeoSetupKid (boxPtr, SB_WorkArea(sb)))     \
    {                                                 \
      layoutPtr->space_above = vspace;                \
      vspace = BB_MarginHeight(sb);                   \
      boxPtr += 2 ;                                 \
      ++layoutPtr ;                                 \
    }



/********    Static Function Declarations    ********/

static void ClassInitialize( void ) ;
static void ClassPartInitialize(
                        WidgetClass w_class) ;
static void Initialize(
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void InsertChild(
                        Widget child) ;
static void DeleteChild(
                        Widget child) ;
static void _XmDialogTypeDefault(
                        Widget widget,
                        int offset,
                        XrmValue *value) ;
static XmImportOperator _XmSetSyntheticResForChild(
                        Widget widget,
                        int offset,
                        XtArgVal *value) ;
static void SelectionBoxCallback(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void ListCallback(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
static void UpdateString(
                        Widget w,
                        XmString string,
                        XmStringDirection direction) ;
static Boolean SetValues(
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;

/********    End Static Function Declarations    ********/


static XtAccelerators defaultTextAcceleratorsParsed;

/*  Action list  */

static XtActionsRec actionsList[] =
{
    { "UpOrDown",               _XmSelectionBoxUpOrDown }, /* Motif 1.0 */
    { "SelectionBoxUpOrDown",   _XmSelectionBoxUpOrDown },
    { "SelectionBoxRestore",    _XmSelectionBoxRestore },
    };


/*  Resource definitions for SelectionBox
*/

static XmSyntheticResource syn_resources[] =
{
	{	XmNselectionLabelString,
		sizeof (XmString),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.selection_label_string),
		_XmSelectionBoxGetSelectionLabelString,
		_XmSetSyntheticResForChild
	},

	{	XmNlistLabelString,
		sizeof (XmString),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.list_label_string),
		_XmSelectionBoxGetListLabelString,
		_XmSetSyntheticResForChild
	},

	{	XmNtextColumns,
		sizeof(short),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.text_columns),
		_XmSelectionBoxGetTextColumns,
		NULL
	},

	{	XmNtextString,
		sizeof (XmString),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.text_string),
		_XmSelectionBoxGetTextString,
		_XmSetSyntheticResForChild
	},

	{	XmNlistItems,
		sizeof (XmStringTable),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.list_items),
		_XmSelectionBoxGetListItems,
		_XmSetSyntheticResForChild
	},

	{	XmNlistItemCount,
		sizeof(int),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.list_item_count),
		_XmSelectionBoxGetListItemCount,
		_XmSetSyntheticResForChild
	},

	{	XmNlistVisibleItemCount,
		sizeof(int),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.list_visible_item_count),
		_XmSelectionBoxGetListVisibleItemCount,
		_XmSetSyntheticResForChild
	},

	{	XmNokLabelString,
		sizeof (XmString),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.ok_label_string),
		_XmSelectionBoxGetOkLabelString,
		NULL
	},

	{	XmNapplyLabelString,
		sizeof (XmString),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.apply_label_string),
		_XmSelectionBoxGetApplyLabelString,
		NULL
	},

	{	XmNcancelLabelString,
		sizeof (XmString),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.cancel_label_string),
		_XmSelectionBoxGetCancelLabelString,
		NULL
	},

	{	XmNhelpLabelString,
		sizeof (XmString),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.help_label_string),
		_XmSelectionBoxGetHelpLabelString,
		NULL
	},
};



static XtResource resources[] =
{
	{	XmNtextAccelerators,
		XmCAccelerators, XmRAcceleratorTable, sizeof (XtAccelerators),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.text_accelerators),
		XmRImmediate, NULL
	},

	{	XmNselectionLabelString,
		XmCSelectionLabelString, XmRXmString, sizeof (XmString),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.selection_label_string),
		XmRString, NULL
	},

	{	XmNlistLabelString,
		XmCListLabelString, XmRXmString, sizeof (XmString),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.list_label_string),
		XmRString, NULL
	},

	{	XmNtextColumns,
		XmCColumns, XmRShort, sizeof(short),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.text_columns),
		XmRImmediate, (XtPointer) 20
	},

	{	XmNtextString,
		XmCTextString, XmRXmString, sizeof (XmString),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.text_string),
		XmRImmediate, (XtPointer) XmUNSPECIFIED
	},

	{	XmNlistItems,
		XmCItems, XmRXmStringTable, sizeof (XmStringTable),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.list_items),
		XmRImmediate, NULL
	},

	{	XmNlistItemCount,
		XmCItemCount, XmRInt, sizeof(int),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.list_item_count),
		XmRImmediate, (XtPointer) XmUNSPECIFIED_COUNT
	},

	{	XmNlistVisibleItemCount,
		XmCVisibleItemCount, XmRInt, sizeof(int),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.list_visible_item_count),
		XmRImmediate, (XtPointer) 8
	},

	{	XmNokLabelString,
		XmCOkLabelString, XmRXmString, sizeof (XmString),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.ok_label_string),
		XmRString, NULL
	},

	{	XmNapplyLabelString,
		XmCApplyLabelString, XmRXmString, sizeof (XmString),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.apply_label_string),
		XmRString, NULL
	},

	{	XmNcancelLabelString,
		XmCCancelLabelString, XmRXmString, sizeof (XmString),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.cancel_label_string),
		XmRString, NULL
	},

	{	XmNhelpLabelString,
		XmCHelpLabelString, XmRXmString, sizeof (XmString),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.help_label_string),
		XmRString, NULL
	},

	{	XmNnoMatchCallback,
		XmCCallback, XmRCallback, sizeof (XtCallbackList),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.no_match_callback),
		XmRImmediate, (XtPointer) NULL
	},

	{	XmNmustMatch,
		XmCMustMatch, XmRBoolean, sizeof(Boolean),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.must_match),
		XmRImmediate, (XtPointer) False
	},

	{	XmNminimizeButtons,
		XmCMinimizeButtons, XmRBoolean, sizeof(Boolean),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.minimize_buttons),
		XmRImmediate, (XtPointer) False
	},

	{	XmNokCallback,
		XmCCallback, XmRCallback, sizeof (XtCallbackList),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.ok_callback),
		XmRImmediate, (XtPointer) NULL
	},

	{	XmNapplyCallback,
		XmCCallback, XmRCallback, sizeof (XtCallbackList),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.apply_callback),
		XmRImmediate, (XtPointer) NULL
	},

	{	XmNcancelCallback,
		XmCCallback, XmRCallback, sizeof (XtCallbackList),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.cancel_callback),
		XmRImmediate, (XtPointer) NULL
	},

	{	XmNdialogType,
		XmCDialogType, XmRSelectionType, sizeof (unsigned char),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.dialog_type),
		XmRCallProc, (XtPointer) _XmDialogTypeDefault
        },

        {       XmNchildPlacement,
                XmCChildPlacement, XmRChildPlacement, sizeof (unsigned char),
                XtOffsetOf( struct _XmSelectionBoxRec, selection_box.child_placement),
                XmRImmediate, (XtPointer) XmPLACE_ABOVE_SELECTION
	},
};



externaldef( xmselectionboxclassrec) XmSelectionBoxClassRec
                                                       xmSelectionBoxClassRec =
{
    {
    	/* superclass	      */	(WidgetClass) &xmBulletinBoardClassRec,
    	/* class_name	      */	"XmSelectionBox",
    	/* widget_size	      */	sizeof(XmSelectionBoxRec),
    	/* class_initialize   */    	ClassInitialize,
    	/* chained class init */	ClassPartInitialize,
    	/* class_inited       */	FALSE,
    	/* initialize	      */	Initialize,
    	/* initialize hook    */	NULL,
    	/* realize	      */	XtInheritRealize,
    	/* actions	      */	actionsList,
    	/* num_actions	      */	XtNumber(actionsList),
    	/* resources	      */	resources,
    	/* num_resources      */	XtNumber(resources),
    	/* xrm_class	      */	NULLQUARK,
    	/* compress_motion    */	TRUE,
    	/* compress_exposure  */	XtExposeCompressMaximal,
    	/* compress enter/exit*/	TRUE,
    	/* visible_interest   */	FALSE,
    	/* destroy	      */	NULL,
    	/* resize	      */	XtInheritResize,
    	/* expose	      */	XtInheritExpose,
    	/* set_values	      */	SetValues,
	/* set_values_hook    */	NULL,
	/* set_values_almost  */	XtInheritSetValuesAlmost,
	/* get_values_hook    */	NULL,
    	/* accept_focus	      */	NULL,
	/* version	      */	XtVersion,
        /* callback_offsets   */        NULL,
        /* tm_table           */        XtInheritTranslations,
    	/* query_geometry     */	XtInheritGeometryManager,
    	/* display_accelerator*/	NULL,
	/* extension	      */	NULL,
    },

    {	/* composite class record */

	/* childrens geo mgr proc   */  XtInheritGeometryManager,
	/* set changed proc	    */  XtInheritChangeManaged,
	/* insert_child		    */	InsertChild,
	/* delete_child 	    */	DeleteChild,
	/* extension		    */	NULL,
    },

    {	/* constraint class record */

	/* no additional resources  */	NULL,
	/* num additional resources */	0,
	/* size of constraint rec   */	0,
	/* constraint_initialize    */	NULL,
	/* constraint_destroy	    */	NULL,
	/* constraint_setvalue	    */	NULL,
	/* extension		    */	NULL,
    },

    {	/* manager class record */
      XmInheritTranslations, 		        /* default translations   */
      syn_resources, 				/* syn_resources      	  */
      XtNumber (syn_resources), 		/* num_syn_resources 	  */
      NULL, 					/* syn_cont_resources     */
      0, 					/* num_syn_cont_resources */
      XmInheritParentProcess,                   /* parent_process         */
      NULL, 					/* extension		  */
    },

    {	/* bulletin board class record */
        TRUE,                                   /*always_install_accelerators*/
        _XmSelectionBoxGeoMatrixCreate,         /* geo_matrix_create */
        XmInheritFocusMovedProc,                /* focus_moved_proc */
	NULL, 					/* extension */
    },

    {	/* selection box class record */
        ListCallback,                           /* list_callback */
	NULL, 					/* extension  */
    },
};

externaldef( xmselectionboxwidgetclass) WidgetClass xmSelectionBoxWidgetClass
                                      = (WidgetClass) &xmSelectionBoxClassRec ;


/****************************************************************/
static void
ClassInitialize( void )
{
/****************/

    /* parse the default translation and accelerator tables
    */
    defaultTextAcceleratorsParsed =
                            XtParseAcceleratorTable( defaultTextAccelerators) ;
    return ;
    }
/****************************************************************/
static void
ClassPartInitialize(
        WidgetClass w_class )
{
/****************/
    XmSelectionBoxWidgetClass wc = (XmSelectionBoxWidgetClass) w_class;
    XmSelectionBoxWidgetClass super =
        (XmSelectionBoxWidgetClass) wc->core_class.superclass;

    if (wc->selection_box_class.list_callback == XmInheritCallbackProc)
        wc->selection_box_class.list_callback =
            super->selection_box_class.list_callback;

    _XmFastSubclassInit(w_class, XmSELECTION_BOX_BIT) ;

    return ;
}


/****************************************************************
 * Create a SelectionBox instance.
 ****************/
static void
Initialize(
        Widget rw,		/* unused */
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
    XmSelectionBoxWidget new_w = (XmSelectionBoxWidget) nw ;
/****************/

    new_w->selection_box.work_area = NULL;

    if ( new_w->selection_box.text_accelerators == NULL ) {
	new_w->selection_box.text_accelerators =
	    defaultTextAcceleratorsParsed;
    }

    /*	Validate dialog type.
     */
    if(    !XmRepTypeValidValue( XmRID_SELECTION_TYPE,
				new_w->selection_box.dialog_type,
				(Widget) new_w)    )
        {
            new_w->selection_box.dialog_type =
		XmIsDialogShell( XtParent( new_w))
		    ? XmDIALOG_SELECTION : XmDIALOG_WORK_AREA ;
	}
    /*      Validate child placement.
     */
    if(    !XmRepTypeValidValue( XmRID_CHILD_PLACEMENT,
				new_w->selection_box.child_placement,
				(Widget) new_w)    )
        {
	    new_w->selection_box.child_placement = XmPLACE_ABOVE_SELECTION;
	}

    /*	Create child widgets.
	Here we have now to take care of XmUNSPECIFIED (CR 4856).
	*/
    new_w->selection_box.adding_sel_widgets = True;

    if ( (new_w->selection_box.dialog_type != XmDIALOG_PROMPT)
	&& (new_w->selection_box.dialog_type != XmDIALOG_COMMAND) )
	{
            if (new_w->selection_box.list_label_string ==
		(XmString) XmUNSPECIFIED) {
		new_w->selection_box.list_label_string = NULL ;
		_XmSelectionBoxCreateListLabel( new_w) ;
		new_w->selection_box.list_label_string =
		    (XmString) XmUNSPECIFIED ;
	    } else
		_XmSelectionBoxCreateListLabel( new_w) ;

	}
    else
        {   SB_ListLabel (new_w) = NULL;
	}

    if (new_w->selection_box.list_label_string !=
	(XmString) XmUNSPECIFIED)
	new_w->selection_box.list_label_string = NULL ;


    if (new_w->selection_box.dialog_type != XmDIALOG_PROMPT)
	{
            _XmSelectionBoxCreateList( new_w) ;
	}
    else
        {   SB_List (new_w) = NULL;
	}
    new_w->selection_box.list_items = NULL ;
    new_w->selection_box.list_item_count = XmUNSPECIFIED_COUNT ;


    if (new_w->selection_box.selection_label_string ==
	(XmString) XmUNSPECIFIED) {
	new_w->selection_box.selection_label_string = NULL ;
	_XmSelectionBoxCreateSelectionLabel(new_w);
	new_w->selection_box.selection_label_string =
	    (XmString) XmUNSPECIFIED ;
    } else {
	_XmSelectionBoxCreateSelectionLabel(new_w);
	new_w->selection_box.selection_label_string = NULL ;
    }


    _XmSelectionBoxCreateText(new_w);

    /* Do not reset text_string to XmUNSPECIFIED until after calls
     *   to CreateList and CreateText.
     */
    new_w->selection_box.text_string = (XmString) XmUNSPECIFIED ;

    if(    new_w->manager.initial_focus == NULL    )
        {
            new_w->manager.initial_focus = SB_Text( new_w) ;
	}

    if (new_w->selection_box.dialog_type != XmDIALOG_COMMAND)
	{
	    _XmSelectionBoxCreateSeparator (new_w);
	    _XmSelectionBoxCreateOkButton (new_w);
	    if (new_w->selection_box.apply_label_string ==
		(XmString) XmUNSPECIFIED) {
		new_w->selection_box.apply_label_string = NULL ;
		_XmSelectionBoxCreateApplyButton (new_w);
		new_w->selection_box.apply_label_string =
		    (XmString) XmUNSPECIFIED ;
	    } else
		_XmSelectionBoxCreateApplyButton (new_w);
	    _XmSelectionBoxCreateCancelButton (new_w);
	    _XmSelectionBoxCreateHelpButton (new_w);

	    BB_DefaultButton( new_w) = SB_OkButton( new_w) ;
	    _XmBulletinBoardSetDynDefaultButton( (Widget) new_w,
						BB_DefaultButton( new_w)) ;
	}
    else
	{
	    SB_Separator (new_w) = NULL;
	    SB_OkButton (new_w) = NULL;
	    SB_ApplyButton (new_w) = NULL;
	    SB_CancelButton (new_w) = NULL;
	    SB_HelpButton (new_w) = NULL;
	}
    new_w->selection_box.ok_label_string = NULL ;
    if (new_w->selection_box.apply_label_string !=
	(XmString) XmUNSPECIFIED)
	new_w->selection_box.apply_label_string = NULL ;
    new_w->selection_box.cancel_label_string = NULL ;
    new_w->selection_box.help_label_string = NULL ;

    new_w->selection_box.adding_sel_widgets = False;


    XtManageChildren (new_w->composite.children,
		      new_w->composite.num_children) ;

    if (new_w->selection_box.dialog_type == XmDIALOG_PROMPT ||
	new_w->selection_box.dialog_type == XmDIALOG_WORK_AREA)
	{
	    XtUnmanageChild (SB_ApplyButton (new_w));
	}

}

/****************************************************************
 * Selection widget supports ONE child.  This routine
 *   handles adding a child to selection widget
 ****************/
static void
InsertChild(
        Widget child )
{
            XmSelectionBoxWidget sb ;
/****************/
    XtWidgetProc insert_child;

    /* Use the dialog class insert proc to do all the dirty work
    */
    _XmProcessLock();
    insert_child = ((XmBulletinBoardWidgetClass) xmBulletinBoardWidgetClass)
					  ->composite_class.insert_child;
    _XmProcessUnlock();

    (*insert_child)(child) ;

    if(    !XtIsRectObj( child)    )
    {   return ;
        }
    sb = (XmSelectionBoxWidget) XtParent( child) ;

    /* check if this child is to be the selection widget's work area widget
    */
    if(    !sb->selection_box.adding_sel_widgets
        && !XtIsShell( child)    )
    {
        if(    !sb->selection_box.work_area    )
        {   sb->selection_box.work_area = child ;
            }
        }
    return ;
    }

/****************************************************************
 * Remove child from selection widget
 ****************/
static void
DeleteChild(
        Widget child )
{
            XmSelectionBoxWidget sel ;
	    XtWidgetProc delete_child;
/****************/

    if(    XtIsRectObj( child)    )
    {
        sel = (XmSelectionBoxWidget) XtParent( child) ;
        /*	Clear widget fields (BulletinBoard does default and cancel).
        */
        if(    child == SB_ListLabel (sel)    )
        {   SB_ListLabel( sel) = NULL ;
            }
        else
        {   if(    SB_List( sel)  &&  (child == XtParent( SB_List (sel)))    )
            {   SB_List( sel) = NULL ;
                }
            else
            {   if(    child == SB_SelectionLabel (sel)    )
                {   SB_SelectionLabel( sel) = NULL ;
                    }
                else
                {   if(    child == SB_Text (sel)    )
                    {   SB_Text( sel) = NULL ;
                        }
                    else
                    {   if(    child == SB_WorkArea (sel)    )
                        {   SB_WorkArea( sel) = NULL ;
                            }
                        else
                        {   if(    child == SB_Separator (sel)    )
                            {   SB_Separator( sel) = NULL ;
                                }
                            else
                            {   if(    child == SB_OkButton (sel)    )
                                {   SB_OkButton( sel) = NULL ;
                                    }
                                else
                                {   if(    child == SB_ApplyButton (sel)    )
                                    {   SB_ApplyButton( sel) = NULL ;
                                        }
                                    else
                                    {   if(    child == SB_HelpButton (sel)   )
                                        {   SB_HelpButton( sel) = NULL ;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    _XmProcessLock();
    delete_child = ((XmBulletinBoardWidgetClass) xmBulletinBoardWidgetClass)
					  ->composite_class.delete_child;
    _XmProcessUnlock();
    (*delete_child)( child) ;
    return ;
    }

/****************************************************************
 * Set the default type (selection or workarea) based on parent.
 ****************/
static void
_XmDialogTypeDefault(
        Widget widget,
        int offset,		/* unused */
        XrmValue *value )
{
    static unsigned char	type;
/****************/

    /*
     * Set the default type.  To do this, we check the dialog
     * box's parent.  If it is a DialogShell widget, then this
     * is a "pop-up" dialog box, and the default type is selection.
     * Else the default type is workarea.
     */
    type = XmDIALOG_WORK_AREA;
    if (XmIsDialogShell (XtParent (widget)))
	type = XmDIALOG_SELECTION;
    value->addr = (XPointer)(&type);
    return ;
}

static XmImportOperator
_XmSetSyntheticResForChild(
        Widget widget,		/* unused */
        int offset,		/* unused */
        XtArgVal *value)	/* unused */
{
    return XmSYNTHETIC_LOAD;
    }

/****************************************************************
 * Create the Label displayed above the List widget.
 ****************/
void
_XmSelectionBoxCreateListLabel(
        XmSelectionBoxWidget sel )
{
/****************/

    SB_ListLabel( sel) = _XmBB_CreateLabelG( (Widget) sel,
                               sel->selection_box.list_label_string, "Items",
			       XmItemsStringLoc) ;
    return ;
    }
/****************************************************************
 * Create the Label displayed above the Text widget.
 ****************/
void
_XmSelectionBoxCreateSelectionLabel(
        XmSelectionBoxWidget sel )
{
/****************/



  if (sel->selection_box.dialog_type == XmDIALOG_COMMAND)
      SB_SelectionLabel(sel) = _XmBB_CreateLabelG((Widget)sel,
				     sel->selection_box.selection_label_string,
				     "Selection",
				     XmPromptStringLoc);
  else
      SB_SelectionLabel( sel) = _XmBB_CreateLabelG( (Widget) sel,
		                      sel->selection_box.selection_label_string,
				      "Selection",
				      XmSelectionStringLoc) ;
}


/****************************************************************
 * Create the List widget.
 ****************/
void
_XmSelectionBoxCreateList(
        XmSelectionBoxWidget sel )
{
            Arg		al[20] ;
    register int	ac = 0 ;
            int *       position ;
            int         pos_count ;
            XtCallbackProc callbackProc ;
/****************/

    if(    sel->selection_box.list_items    )
    {
        XtSetArg( al[ac], XmNitems, sel->selection_box.list_items) ; ac++ ;
        }
    if(    sel->selection_box.list_item_count != XmUNSPECIFIED    )
    {
        XtSetArg( al[ac], XmNitemCount,
                                  sel->selection_box.list_item_count) ;  ac++ ;
        }
    XtSetArg( al[ac], XmNvisibleItemCount,
		sel->selection_box.list_visible_item_count) ;  ac++ ;

    sel->selection_box.list_selected_item_position = 0 ;

    XtSetArg( al[ac], XmNstringDirection, SB_StringDirection (sel)) ;  ac++ ;
    XtSetArg( al[ac], XmNselectionPolicy, XmBROWSE_SELECT) ;  ac++ ;
    XtSetArg( al[ac], XmNlistSizePolicy, XmCONSTANT) ;  ac++ ;
    XtSetArg( al[ac], XmNnavigationType, XmSTICKY_TAB_GROUP) ; ++ac ;

    SB_List( sel) = XmCreateScrolledList( (Widget) sel, "ItemsList", al, ac) ;

    if(    sel->selection_box.text_string != (XmString) XmUNSPECIFIED    )
    {
        if(    sel->selection_box.text_string
            && XmListGetMatchPos( SB_List( sel),
                    sel->selection_box.text_string, &position, &pos_count)    )
        {   if(    pos_count    )
            {
                sel->selection_box.list_selected_item_position = position[0] ;
                XmListSelectPos( SB_List( sel), position[0], FALSE) ;
                }
            XtFree( (char *) position) ;
            }
        }
    callbackProc = ((XmSelectionBoxWidgetClass) sel->core.widget_class)
                                          ->selection_box_class.list_callback ;
    if(    callbackProc    )
    {
        XtAddCallback( SB_List( sel), XmNsingleSelectionCallback,
                                               callbackProc, (XtPointer) sel) ;
        XtAddCallback( SB_List( sel), XmNbrowseSelectionCallback,
                                               callbackProc, (XtPointer) sel) ;
        XtAddCallback( SB_List( sel), XmNdefaultActionCallback,
                                               callbackProc, (XtPointer) sel) ;
        }
    XtManageChild( SB_List( sel)) ;

    return ;
    }

/****************************************************************
 * Create the Text widget.
 ****************/
void
_XmSelectionBoxCreateText(
        XmSelectionBoxWidget sel )
{
	Arg		al[10];
	register int	ac = 0;
	String		text_value ;
	XtAccelerators	temp_accelerators ;
/****************/

	XtSetArg (al[ac], XmNcolumns, sel->selection_box.text_columns);  ac++;
	XtSetArg (al[ac], XmNresizeWidth, False);  ac++;
	XtSetArg (al[ac], XmNeditMode, XmSINGLE_LINE_EDIT);  ac++;
        XtSetArg( al[ac], XmNnavigationType, XmSTICKY_TAB_GROUP) ; ++ac ;

	SB_Text( sel) = XmCreateTextField( (Widget) sel, "Text", al, ac);
	if(    (sel->selection_box.text_string != (XmString) XmUNSPECIFIED)    )
        {
            text_value = _XmStringGetTextConcat(
                                              sel->selection_box.text_string) ;
            XmTextFieldSetString( SB_Text (sel), text_value) ;
            if(    text_value    )
            {   XmTextFieldSetInsertionPosition( SB_Text( sel),
			          XmTextFieldGetLastPosition( SB_Text( sel))) ;
                }
            XtFree( (char *) text_value) ;
            }

	/*	Install text accelerators.
	*/
        temp_accelerators = sel->core.accelerators;
	sel->core.accelerators = sel->selection_box.text_accelerators;
	XtInstallAccelerators( SB_Text( sel), (Widget) sel) ;
	sel->core.accelerators = temp_accelerators;
        return ;
}

/****************************************************************
 * Create the Separator displayed above the buttons.
 ****************/
void
_XmSelectionBoxCreateSeparator(
        XmSelectionBoxWidget sel )
{
	Arg		al[10];
	register int	ac = 0;
/****************/

	XtSetArg (al[ac], XmNhighlightThickness, 0);  ac++;
	SB_Separator (sel) =
		XmCreateSeparatorGadget( (Widget) sel, "Separator", al, ac);
        return ;
}
/****************************************************************
 * Create the "OK" PushButton.
 ****************/
void
_XmSelectionBoxCreateOkButton(
        XmSelectionBoxWidget sel )
{
/****************/

    SB_OkButton( sel) = _XmBB_CreateButtonG( (Widget) sel,
                                    sel->selection_box.ok_label_string, "OK",
				    XmOkStringLoc) ;
    XtAddCallback (SB_OkButton (sel), XmNactivateCallback,
                        SelectionBoxCallback, (XtPointer) XmDIALOG_OK_BUTTON) ;
    return ;
    }

/****************************************************************
 * Create the "Apply" PushButton.
 ****************/
void
_XmSelectionBoxCreateApplyButton(
        XmSelectionBoxWidget sel )
{
/****************/

    SB_ApplyButton( sel) = _XmBB_CreateButtonG( (Widget) sel,
                              sel->selection_box.apply_label_string, "Apply",
                              XmApplyStringLoc) ;
    /* Remove BulletinBoard Unmanage callback from apply and help buttons.
    */
    XtRemoveAllCallbacks( SB_ApplyButton( sel), XmNactivateCallback) ;
    XtAddCallback (SB_ApplyButton (sel), XmNactivateCallback,
                     SelectionBoxCallback, (XtPointer) XmDIALOG_APPLY_BUTTON) ;
    return ;
    }
/****************************************************************
 * Create the "Cancel" PushButton.
 ****************/
void
_XmSelectionBoxCreateCancelButton(
        XmSelectionBoxWidget sel )
{
/****************/

    SB_CancelButton( sel) = _XmBB_CreateButtonG( (Widget) sel,
                            sel->selection_box.cancel_label_string, "Cancel",
                            XmCancelStringLoc) ;
    XtAddCallback( SB_CancelButton( sel), XmNactivateCallback,
                    SelectionBoxCallback, (XtPointer) XmDIALOG_CANCEL_BUTTON) ;
    return ;
    }
/****************************************************************
 * Create the "Help" PushButton.
 ****************/
void
_XmSelectionBoxCreateHelpButton(
        XmSelectionBoxWidget sel )
{
/****************/

    SB_HelpButton( sel) = _XmBB_CreateButtonG( (Widget) sel,
                                sel->selection_box.help_label_string, "Help",
                                XmHelpStringLoc) ;
    /* Remove BulletinBoard Unmanage callback from apply and help buttons.
    */
    XtRemoveAllCallbacks( SB_HelpButton( sel), XmNactivateCallback) ;
    XtAddCallback (SB_HelpButton (sel), XmNactivateCallback,
                      SelectionBoxCallback, (XtPointer) XmDIALOG_HELP_BUTTON) ;
    return ;
    }

/****************************************************************/

XmGeoMatrix
_XmSelectionBoxGeoMatrixCreate(
        Widget wid,
        Widget instigator,
        XtWidgetGeometry *desired )
{
    XmSelectionBoxWidget sb = (XmSelectionBoxWidget) wid ;
    XmGeoMatrix     geoSpec ;
    register XmGeoRowLayout  layoutPtr ;
    register XmKidGeometry   boxPtr ;
    XmKidGeometry   firstButtonBox ;
    XmListWidget    list ;
    Boolean         listLabelBox ;
    Boolean         selLabelBox ;
    Dimension       vspace = BB_MarginHeight(sb);
    int             i;

/*
 * Layout SelectionBox XmGeoMatrix.
 * Each row is terminated by leaving an empty XmKidGeometry and
 * moving to the next XmGeoRowLayout.
 */

    geoSpec = _XmGeoMatrixAlloc( XmSB_MAX_WIDGETS_VERT,
                              sb->composite.num_children, 0) ;
    geoSpec->composite = (Widget) sb ;
    geoSpec->instigator = (Widget) instigator ;
    if(    desired    )
    {   geoSpec->instig_request = *desired ;
        }
    geoSpec->margin_w = BB_MarginWidth( sb) + sb->manager.shadow_thickness ;
    geoSpec->margin_h = BB_MarginHeight( sb) + sb->manager.shadow_thickness ;
    geoSpec->no_geo_request = _XmSelectionBoxNoGeoRequest ;

    layoutPtr = &(geoSpec->layouts->row) ;
    boxPtr = geoSpec->boxes ;

    /* menu bar */

    for (i = 0; i < sb->composite.num_children; i++)
    {	Widget w = sb->composite.children[i];

        if(    XmIsRowColumn(w)
            && ((XmRowColumnWidget)w)->row_column.type == XmMENU_BAR
            && w != SB_WorkArea(sb)
            && _XmGeoSetupKid( boxPtr, w)    )
	{   layoutPtr->fix_up = _XmMenuBarFix ;
            boxPtr += 2;
            ++layoutPtr;
            vspace = 0;		/* fixup space_above of next row. */
            break;
            }
        }

    /* work area, XmPLACE_TOP */

    if (sb->selection_box.child_placement == XmPLACE_TOP)
      SetupWorkArea(sb);

    /* list box label */

    listLabelBox = FALSE ;
    if(    _XmGeoSetupKid( boxPtr, SB_ListLabel( sb))    )
    {
        listLabelBox = TRUE ;
        layoutPtr->space_above = vspace;
        vspace = BB_MarginHeight(sb);
        boxPtr += 2 ;
        ++layoutPtr ;
        }

    /* list box */

    list = (XmListWidget) SB_List( sb) ;
    if(    list  &&  XtIsManaged((Widget)list)
        && _XmGeoSetupKid( boxPtr, XtParent( list))    )
    {
        if(    !listLabelBox    )
        {   layoutPtr->space_above = vspace;
            vspace = BB_MarginHeight(sb);
            }
        layoutPtr->stretch_height = TRUE ;
        layoutPtr->min_height = 70 ;
        boxPtr += 2 ;
        ++layoutPtr ;
        }

    /* work area, XmPLACE_ABOVE_SELECTION */

    if (sb->selection_box.child_placement == XmPLACE_ABOVE_SELECTION)
      SetupWorkArea(sb)

    /* selection label */

    selLabelBox = FALSE ;
    if(    _XmGeoSetupKid( boxPtr, SB_SelectionLabel( sb))    )
    {   selLabelBox = TRUE ;
        layoutPtr->space_above = vspace;
        vspace = BB_MarginHeight(sb);
        boxPtr += 2 ;
        ++layoutPtr ;
        }

    /* selection text */

    if(    _XmGeoSetupKid( boxPtr, SB_Text( sb))    )
    {
        if(    !selLabelBox    )
        {   layoutPtr->space_above = vspace;
            vspace = BB_MarginHeight(sb);
            }
        boxPtr += 2 ;
        ++layoutPtr ;
        }

    /* work area, XmPLACE_BELOW_SELECTION */

    if (sb->selection_box.child_placement == XmPLACE_BELOW_SELECTION)
      SetupWorkArea(sb)

    /* separator */

    if(    _XmGeoSetupKid( boxPtr, SB_Separator( sb))    )
    {   layoutPtr->fix_up = _XmSeparatorFix ;
        layoutPtr->space_above = vspace;
        vspace = BB_MarginHeight(sb);
        boxPtr += 2 ;
        ++layoutPtr ;
        }

    /* button row */

    firstButtonBox = boxPtr ;

    if (LayoutIsRtoLM(sb))
    {
       if(    _XmGeoSetupKid( boxPtr, SB_HelpButton( sb))    )
       {   ++boxPtr ;
           }
       if(    _XmGeoSetupKid( boxPtr, SB_CancelButton( sb))    )
       {   ++boxPtr ;
           }
       if(    _XmGeoSetupKid( boxPtr, SB_ApplyButton( sb))    )
       {   ++boxPtr ;
           }

       for (i = 0; i < sb->composite.num_children; i++)
       {
	    Widget w = sb->composite.children[sb->composite.num_children-i-1];
	    if (IsButton(w) && !IsAutoButton(sb,w) && w != SB_WorkArea(sb))
	    {
		if (_XmGeoSetupKid( boxPtr, w))
		{   ++boxPtr ;
		}
	    }
       }

       if(    _XmGeoSetupKid( boxPtr, SB_OkButton( sb))    )
       {   ++boxPtr ;
           }
    }
    else
    {
      if(    _XmGeoSetupKid( boxPtr, SB_OkButton( sb))    )
	{   ++boxPtr ;
	  }
      for (i = 0; i < sb->composite.num_children; i++)
	{
	  Widget w = sb->composite.children[i];
	  if (IsButton(w) && !IsAutoButton(sb,w) && w != SB_WorkArea(sb))
	    {
	      if (_XmGeoSetupKid( boxPtr, w))
		{   ++boxPtr ;
		  }
	    }
	}
      if(    _XmGeoSetupKid( boxPtr, SB_ApplyButton( sb))    )
	{   ++boxPtr ;
	  }
      if(    _XmGeoSetupKid( boxPtr, SB_CancelButton( sb))    )
	{   ++boxPtr ;
	  }
      if(    _XmGeoSetupKid( boxPtr, SB_HelpButton( sb))    )
	{   ++boxPtr ;
	  }
    }

    if(    boxPtr != firstButtonBox    )
    {   layoutPtr->fill_mode = XmGEO_CENTER ;
        layoutPtr->fit_mode = XmGEO_WRAP ;
        layoutPtr->space_above = vspace;
        vspace = BB_MarginHeight(sb);
        if(    !(sb->selection_box.minimize_buttons)    )
        {   layoutPtr->even_width = 1 ;
            }
        layoutPtr->even_height = 1 ;
	++layoutPtr ;
        }

    /* the end. */

    layoutPtr->space_above = vspace;
    layoutPtr->end = TRUE ;
    return( geoSpec) ;
    }

/****************************************************************/
Boolean
_XmSelectionBoxNoGeoRequest(
        XmGeoMatrix geoSpec )
{
/****************/

    if(    BB_InSetValues( geoSpec->composite)
        && (XtClass( geoSpec->composite) == xmSelectionBoxWidgetClass)    )
    {
        return( TRUE) ;
        }
    return( FALSE) ;
    }

/****************************************************************
 * Call the callbacks for a SelectionBox button.
 ****************/
static void
SelectionBoxCallback(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
	unsigned char		which_button = (unsigned char)(long) client_data;
	XmSelectionBoxWidget	sel = (XmSelectionBoxWidget) XtParent (w);
	XmAnyCallbackStruct	*callback = (XmAnyCallbackStruct *) call_data;
	XmSelectionBoxCallbackStruct	temp;
	Boolean			match = True;
	String			text_value;
/****************/

	text_value = XmTextFieldGetString (SB_Text (sel));
	temp.event = callback->event;
	temp.value = XmStringGenerate(text_value, XmFONTLIST_DEFAULT_TAG,
				      XmCHARSET_TEXT, NULL);
	temp.length = XmStringLength (temp.value);
	XtFree (text_value);

	switch (which_button)
	{
		case XmDIALOG_OK_BUTTON:
                        if (sel->selection_box.list != NULL)
			   if (sel->selection_box.must_match)
			   {
				match = XmListItemExists (SB_List (sel), temp.value);
			   }
			if (!match)
			{
				temp.reason = XmCR_NO_MATCH;
				XtCallCallbackList ((Widget) sel, sel->
					selection_box.no_match_callback, &temp);
			}
			else
			{
				temp.reason = XmCR_OK;
				XtCallCallbackList ((Widget) sel, sel->
					selection_box.ok_callback, &temp);
			}
			break;

		case XmDIALOG_APPLY_BUTTON:
			temp.reason = XmCR_APPLY;
			XtCallCallbackList ((Widget) sel, sel->selection_box.apply_callback, &temp);
			break;

		case XmDIALOG_CANCEL_BUTTON:
			temp.reason = XmCR_CANCEL;
			XtCallCallbackList ((Widget) sel, sel->selection_box.cancel_callback, &temp);
			break;

		case XmDIALOG_HELP_BUTTON:
			/* Invoke the help system. */
                        _XmManagerHelp((Widget)sel, callback->event, NULL, NULL) ;
			break;
	}
        XmStringFree( temp.value) ;
        return ;
}

/****************************************************************
 * Process callback from the List in a SelectionBox.
 ****************/
static void
ListCallback(
        Widget w,
        XtPointer client_data,
        XtPointer call_data )
{
	enum { XmA_MOTIF_SELECTION_TEXT, XmA_MOTIF_COMPOUND_STRING,
	       XmACOMPOUND_TEXT, NUM_ATOMS };
	static char *atom_names[] = {
	  XmI_MOTIF_SELECTION_TEXT, XmS_MOTIF_COMPOUND_STRING,
	  XmSCOMPOUND_TEXT };

	XmListCallbackStruct	*callback = (XmListCallbackStruct *) call_data;
	XmSelectionBoxWidget	sel = (XmSelectionBoxWidget) client_data ;
	XmGadgetClass		gadget_class;
        XmGadget                dbutton = (XmGadget)
                                                BB_DynamicDefaultButton( sel) ;
	XmAccessTextualTrait	textTrait;
	Atom		atoms[XtNumber(atom_names)];
	Atom		PRIMARY = XA_PRIMARY;
	XtPointer	value;
	Atom		type;
	unsigned long	size;
	int		format;
	Boolean		success;


	textTrait = (XmAccessTextualTrait)
	  XmeTraitGet((XtPointer) XtClass(SB_Text(sel)), XmQTaccessTextual);
	if (textTrait == NULL) return; /* Abort if text child is broken */

	assert(XtNumber(atom_names) == NUM_ATOMS);
	XInternAtoms(XtDisplay(w), atom_names, XtNumber(atom_names),
		     False, atoms);

	/*	Update the text widget to relect the latest list selection.
	 *	If list default action (double click), activate default button.
	 */
	sel->selection_box.list_selected_item_position =
                                                       callback->item_position;

	success = False;
	/*
	 * Obtain item.
	 *
	 * We do this via the selection mechanism by calling
	 * _XmConvertHandler directly.  First we call with
	 * the target _MOTIF_SELECTION_TEXT,  then we fallback
	 * to using _MOTIF_COMPOUND_STRING if no reply is obtained
	 */
	_XmConvertHandlerSetLocal();
	success = _XmConvertHandler(w, &PRIMARY,
				    &atoms[XmA_MOTIF_SELECTION_TEXT],
				    &type, &value, &size, &format);
	if (! success) {
	  _XmConvertHandlerSetLocal();
	  success = _XmConvertHandler(w, &PRIMARY,
				      &atoms[XmA_MOTIF_COMPOUND_STRING],
				      &type, &value, &size, &format);
	}
	if (success) {
	  if (type == atoms[XmA_MOTIF_COMPOUND_STRING]) {
	    XmString temp;
	    temp = XmCvtByteStreamToXmString((unsigned char *) value);

	    textTrait -> setValue(SB_Text(sel), temp, XmFORMAT_XmSTRING);
	    XmStringFree(temp);
	  } else if (type == atoms[XmACOMPOUND_TEXT]) {
	    XmString temp;
	    temp = XmCvtCTToXmString((char*) value);
	    textTrait -> setValue(SB_Text(sel), value, XmFORMAT_XmSTRING);
	    XmStringFree(temp);
	  } else if (type == XA_STRING) {
	    textTrait -> setValue(SB_Text(sel), value, XmFORMAT_MBYTE);
	  }
	}

	if(success)
	{
	  XmTextFieldSetInsertionPosition(SB_Text (sel),
				       XmTextFieldGetLastPosition( SB_Text( sel))) ;
	  XtFree((char*) value);
	}
        /* Catch only double-click default action here.
        *  Key press events are handled through the ParentProcess routine.
        */
	if(    (callback->reason == XmCR_DEFAULT_ACTION)
            && (callback->event->type != KeyPress)
            && dbutton  &&  XtIsManaged((Widget)dbutton)
            && XtIsSensitive((Widget)dbutton)  &&  XmIsGadget( dbutton)    )
	 {
            gadget_class = (XmGadgetClass) dbutton->object.widget_class ;
            if (gadget_class->gadget_class.arm_and_activate)
		{
		/* pass the event so that the button can pass it on to its
		** callbacks, even though the event isn't within the button
		*/
		(*(gadget_class->gadget_class.arm_and_activate))
			  ((Widget) dbutton, callback->event, NULL, NULL) ;
		}
	 }
        return ;
        }


/****************************************************************
 * Set the label string of a label or button
 ****************/
static void
UpdateString(
        Widget w,
        XmString string,
        XmStringDirection direction )
{
	Arg		al[3];
    	register int	ac = 0;
/****************/

	if (w)
	{
		XtSetArg (al[ac], XmNstringDirection, direction);  ac++;
		XtSetArg (al[ac], XmNlabelString, string);  ac++;
		XtSetValues (w, al, ac);
	}
        return ;
}

/****************************************************************
 * Update widget when values change.
 ****************/
static Boolean
SetValues(
        Widget cw,
        Widget rw,
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
            XmSelectionBoxWidget current = (XmSelectionBoxWidget) cw ;
            XmSelectionBoxWidget request = (XmSelectionBoxWidget) rw ;
            XmSelectionBoxWidget new_w = (XmSelectionBoxWidget) nw ;
	Arg		al[10];
	register int	ac;

	String		text_value ;
/****************/

	BB_InSetValues (new_w) = True;


      /*      Validate child placement.
      */
      if(    (new_w->selection_box.child_placement
                      != current->selection_box.child_placement)
              && !XmRepTypeValidValue( XmRID_CHILD_PLACEMENT,
                      new_w->selection_box.child_placement, (Widget) new_w)    )
      {
          new_w->selection_box.child_placement =
                      current->selection_box.child_placement;
          }

	/*	Update label strings.
	*/
	if(    new_w->selection_box.selection_label_string
                          != current->selection_box.selection_label_string    )
	{   UpdateString( SB_SelectionLabel (new_w),
                                     new_w->selection_box.selection_label_string,
                                                    SB_StringDirection (new_w)) ;
            new_w->selection_box.selection_label_string = NULL ;
            }

	if(    new_w->selection_box.list_label_string
                               != current->selection_box.list_label_string    )
	{   UpdateString( SB_ListLabel (new_w),
                                          new_w->selection_box.list_label_string,
                                                    SB_StringDirection (new_w)) ;
            new_w->selection_box.list_label_string = NULL ;
            }

	if(    new_w->selection_box.ok_label_string
                                 != current->selection_box.ok_label_string    )
	{   UpdateString( SB_OkButton (new_w),
                                            new_w->selection_box.ok_label_string,
                                                    SB_StringDirection (new_w)) ;
            new_w->selection_box.ok_label_string = NULL ;
            }

	if(    new_w->selection_box.apply_label_string
                              != current->selection_box.apply_label_string    )
	{   UpdateString( SB_ApplyButton (new_w),
                                         new_w->selection_box.apply_label_string,
                                                    SB_StringDirection (new_w)) ;
            new_w->selection_box.apply_label_string = NULL ;
            }

	if(    new_w->selection_box.cancel_label_string
                             != current->selection_box.cancel_label_string    )
	{   UpdateString( SB_CancelButton (new_w),
                                        new_w->selection_box.cancel_label_string,
                                                    SB_StringDirection (new_w)) ;
            new_w->selection_box.cancel_label_string = NULL ;
            }

	if(    new_w->selection_box.help_label_string
                               != current->selection_box.help_label_string    )
	{   UpdateString( SB_HelpButton (new_w),
                                          new_w->selection_box.help_label_string,
                                                    SB_StringDirection (new_w)) ;
            new_w->selection_box.help_label_string = NULL ;
            }

	/*	Update List widget.
	*/
	ac = 0;
	if(    new_w->selection_box.list_items    ) {
	  XtSetArg( al[ac], XmNitems,
		   new_w->selection_box.list_items) ;  ac++ ;
	}
	if(    new_w->selection_box.list_item_count != XmUNSPECIFIED_COUNT ) {
	  XtSetArg( al[ac], XmNitemCount,
		   new_w->selection_box.list_item_count) ;  ac++ ;
	  new_w->selection_box.list_item_count = XmUNSPECIFIED_COUNT ;
	}
	if (new_w->selection_box.list_visible_item_count !=
	    current->selection_box.list_visible_item_count) {
	  XtSetArg (al[ac], XmNvisibleItemCount,
		    new_w->selection_box.list_visible_item_count);  ac++;
	}
	if (ac)	{
	  if (SB_List (new_w))
	    XtSetValues (SB_List (new_w), al, ac);
	  new_w->selection_box.list_items = NULL ;
	}

	/*	Update Text widget.
	*/
        text_value = NULL ;
	ac = 0;
	if(    new_w->selection_box.text_string
                                     != current->selection_box.text_string    )
	{
            text_value = _XmStringGetTextConcat(
                                              new_w->selection_box.text_string) ;
            XtSetArg( al[ac], XmNvalue, text_value) ;  ac++ ;
            new_w->selection_box.text_string = (XmString) XmUNSPECIFIED ;
            }
	if (new_w->selection_box.text_columns !=
		current->selection_box.text_columns)
	{
		XtSetArg (al[ac], XmNcolumns,
			new_w->selection_box.text_columns);  ac++;
	}
	if (ac)
	{
		if (SB_Text (new_w))
			XtSetValues (SB_Text (new_w), al, ac);
	}
	if (text_value)
	{
		if (SB_Text (new_w))
			XmTextFieldSetInsertionPosition (SB_Text (new_w),
			          XmTextFieldGetLastPosition( SB_Text( new_w))) ;
		XtFree (text_value);
	}

	/*	Validate dialog type.
	*/
	if (request->selection_box.dialog_type !=
			 current->selection_box.dialog_type)
	{
		XmeWarning( (Widget) new_w, WARN_DIALOG_TYPE_CHANGE);
		new_w->selection_box.dialog_type =
			current->selection_box.dialog_type;
	}
	BB_InSetValues (new_w) = False;

	/*	If this is the instantiated class then do layout.
	*/
	if(    XtClass( new_w) == xmSelectionBoxWidgetClass    )
	{
            _XmBulletinBoardSizeUpdate( (Widget) new_w) ;
	    }
	return (Boolean) (FALSE);
        }

/****************************************************************/
void
_XmSelectionBoxGetSelectionLabelString(
        Widget wid,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            XmSelectionBoxWidget sel = (XmSelectionBoxWidget) wid ;
            XmString        data ;
            Arg             al[1] ;
/****************/

    if(    SB_SelectionLabel (sel)    )
    {
        XtSetArg( al[0], XmNlabelString, &data) ;
        XtGetValues( SB_SelectionLabel( sel), al, 1) ;
        *value = (XtArgVal) data ;
        }
    else
    {   *value = (XtArgVal) NULL ;
        }
    return ;
    }
/****************************************************************/
void
_XmSelectionBoxGetListLabelString(
        Widget wid,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            XmSelectionBoxWidget sel = (XmSelectionBoxWidget) wid ;
            XmString        data ;
            Arg             al[1] ;
/****************/

    if(    SB_ListLabel( sel)    )
    {
        XtSetArg( al[0], XmNlabelString, &data) ;
        XtGetValues( SB_ListLabel( sel), al, 1) ;
        *value = (XtArgVal) data ;
        }
    else
    {   *value = (XtArgVal) NULL ;
        }
    return ;
    }
/****************************************************************/
void
_XmSelectionBoxGetTextColumns(
        Widget wid,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            XmSelectionBoxWidget sel = (XmSelectionBoxWidget) wid ;
            short           data ;
            Arg             al[1] ;
/****************/

    if(    SB_Text( sel)    )
    {
        XtSetArg( al[0], XmNcolumns, &data) ;
        XtGetValues( SB_Text( sel), al, 1) ;
        *value = (XtArgVal) data ;
        }
    else
    {   *value = (XtArgVal) 0 ;
        }
    return ;
    }
/****************************************************************/
void
_XmSelectionBoxGetTextString(
        Widget wid,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            XmSelectionBoxWidget sel = (XmSelectionBoxWidget) wid ;
            String          data = NULL ;
            XmString        text_string ;
            Arg             al[1] ;
/****************/

    if(    SB_Text( sel)    )
    {
        XtSetArg( al[0], XmNvalue, &data) ;
        XtGetValues( SB_Text( sel), al, 1) ;
        text_string = XmStringGenerate(data, XmFONTLIST_DEFAULT_TAG,
				       XmCHARSET_TEXT, NULL) ;
        *value = (XtArgVal) text_string ;
	XtFree((char *)data);
        }
    else
    {   *value = (XtArgVal) NULL ;
        }
    return ;
    }
/****************************************************************/
void
_XmSelectionBoxGetListItems(
        Widget wid,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            XmSelectionBoxWidget sel = (XmSelectionBoxWidget) wid ;
            Arg             al[1] ;
            XmString        data ;
/****************/

    if(    SB_List( sel)    )
    {
        XtSetArg( al[0], XmNitems, &data) ;
        XtGetValues( SB_List( sel), al, 1) ;
        *value = (XtArgVal) data ;
        }
    else
    {   *value = (XtArgVal) NULL ;
        }
    return ;
    }
/****************************************************************/
void
_XmSelectionBoxGetListItemCount(
        Widget wid,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            XmSelectionBoxWidget sel = (XmSelectionBoxWidget) wid ;
            int             data ;
            Arg             al[1] ;
/****************/

    if(    SB_List( sel)    )
    {
        XtSetArg( al[0], XmNitemCount, &data) ;
        XtGetValues( SB_List( sel), al, 1) ;
        *value = (XtArgVal) data ;
        }
    else
    {   *value = (XtArgVal) 0 ;
        }
    return ;
    }
/****************************************************************/
void
_XmSelectionBoxGetListVisibleItemCount(
        Widget wid,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            XmSelectionBoxWidget sel = (XmSelectionBoxWidget) wid ;
            int             data ;
            Arg             al[1] ;
/****************/

    if(    SB_List( sel)    )
    {
        XtSetArg( al[0], XmNvisibleItemCount, &data) ;
        XtGetValues( SB_List( sel), al, 1) ;
        *value = (XtArgVal) data ;
        }
    else
    {   *value = (XtArgVal) 0 ;
        }
    return ;
    }
/****************************************************************/
void
_XmSelectionBoxGetOkLabelString(
        Widget wid,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            XmSelectionBoxWidget sel = (XmSelectionBoxWidget) wid ;
            XmString        data ;
            Arg             al[1] ;
/****************/

    if(    SB_OkButton( sel)    )
    {
        XtSetArg( al[0], XmNlabelString, &data) ;
        XtGetValues( SB_OkButton( sel), al, 1) ;
        *value = (XtArgVal) data ;
        }
    else
    {   *value = (XtArgVal) NULL ;
        }
    return ;
    }
/****************************************************************/
void
_XmSelectionBoxGetApplyLabelString(
        Widget wid,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            XmSelectionBoxWidget sel = (XmSelectionBoxWidget) wid ;
            XmString        data ;
            Arg             al[1] ;
/****************/

    if(    SB_ApplyButton( sel)    )
    {
        XtSetArg( al[0], XmNlabelString, &data) ;
        XtGetValues( SB_ApplyButton( sel), al, 1) ;
        *value = (XtArgVal) data ;
        }
    else
    {   *value = (XtArgVal) NULL ;
        }
    return ;
    }
/****************************************************************/
void
_XmSelectionBoxGetCancelLabelString(
        Widget wid,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            XmSelectionBoxWidget sel = (XmSelectionBoxWidget) wid ;
            XmString        data ;
            Arg             al[1] ;
/****************/

    if(    SB_CancelButton( sel)    )
    {
        XtSetArg( al[0], XmNlabelString, &data) ;
        XtGetValues( SB_CancelButton( sel), al, 1) ;
        *value = (XtArgVal) data ;
        }
    else
    {   *value = (XtArgVal) NULL ;
        }
    return ;
    }
/****************************************************************/
void
_XmSelectionBoxGetHelpLabelString(
        Widget wid,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            XmSelectionBoxWidget sel = (XmSelectionBoxWidget) wid ;
            XmString        data ;
            Arg             al[1] ;
/****************/

    if(    SB_HelpButton( sel)    )
    {
        XtSetArg( al[0], XmNlabelString, &data) ;
        XtGetValues( SB_HelpButton( sel), al, 1) ;
        *value = (XtArgVal) data ;
        }
    else
    {   *value = (XtArgVal) NULL ;
        }
    return ;
    }

/****************************************************************/
void
_XmSelectionBoxUpOrDown(
        Widget wid,
        XEvent *event,		/* unused */
        String *argv,
        Cardinal *argc )
{
            XmSelectionBoxWidget sel = (XmSelectionBoxWidget) wid ;
            int	            visible ;
            int	            top ;
            int	            key_pressed ;
            Widget	    list ;
            int	*           position ;
            int	            count ;
            Arg             av[5] ;
            Cardinal        ac ;
/****************/

    if (!argc || (*argc != 1) || !argv)
    {
        XmeWarning(wid, MESSAGE0);
        return;
    }

    if(    !(list = sel->selection_box.list)    )
    {   return ;
        }
    ac = 0 ;
    XtSetArg( av[ac], XmNitemCount, &count) ; ++ac ;
    XtSetArg( av[ac], XmNtopItemPosition, &top) ; ++ac ;
    XtSetArg( av[ac], XmNvisibleItemCount, &visible) ; ++ac ;
    XtGetValues( list, av, ac) ;

    if(    !count    )
    {   return ;
        }

    if (_XmConvertActionParamToRepTypeId((Widget) sel,
			 XmRID_SELECTION_BOX_UP_OR_DOWN_ACTION_PARAMS,
			 argv[0], True, &key_pressed) == False)
    {
	/* We couldn't convert the value. Just assume a value of 0. */
	key_pressed = 0;
    }

    position = &(sel->selection_box.list_selected_item_position) ;

    if(    *position == 0    )
    {   /*  No selection, so select first item or last if key_pressed == end.
        */
        if(    key_pressed == 3    )
        {   *position = count ;
            XmListSelectPos( list, *position, True) ;
            }
        else
        {   XmListSelectPos( list, ++*position, True) ;
            }
        }
    else
    {   if(    !key_pressed && (*position > 1)    )
        {   /*  up  */
            XmListDeselectPos( list, *position) ;
            XmListSelectPos( list, --*position, True) ;
            }
        else
        {   if(    (key_pressed == 1) && (*position < count)    )
            {   /*  down  */
                XmListDeselectPos( list, *position) ;
                XmListSelectPos( list, ++*position, True) ;
                }
            else
            {   if(    key_pressed == 2    )
                {   /*  home  */
                    XmListDeselectPos( list, *position) ;
                    *position = 1 ;
                    XmListSelectPos( list, *position, True) ;
                    }
                else
                {   if(    key_pressed == 3    )
                    {   /*  end  */
                        XmListDeselectPos( list, *position) ;
                        *position = count ;
                        XmListSelectPos( list, *position, True) ;
                        }
                    }
                }
            }
        }
    if(    top > *position    )
    {   XmListSetPos( list, *position) ;
        }
    else
    {   if(    (top + visible) <= *position    )
        {   XmListSetBottomPos( list, *position) ;
            }
        }
    return ;
    }

/****************************************************************/
void
_XmSelectionBoxRestore(
        Widget wid,
        XEvent *event,		/* unused */
        String *argv,		/* unused */
        Cardinal *argc )	/* unused */
{
            XmSelectionBoxWidget sel = (XmSelectionBoxWidget) wid ;
            Widget          list ;
            int	            count ;
            XmString *      items ;
            Arg             al[5] ;
            int             ac ;
            String          textItem ;
/****************/

    list = SB_List( sel) ;

    if(    list
        && SB_Text( sel)    )
    {
        ac = 0 ;
        XtSetArg( al[ac], XmNselectedItems, &items) ; ++ac ;
        XtSetArg( al[ac], XmNselectedItemCount, &count) ; ++ac ;
        XtGetValues( list, al, ac) ;
        if(    count    )
        {
            textItem = _XmStringGetTextConcat( *items) ;
            XmTextFieldSetString( SB_Text( sel), textItem) ;
            XmTextFieldSetInsertionPosition( SB_Text( sel),
			          XmTextFieldGetLastPosition( SB_Text( sel))) ;
            XtFree( textItem) ;
            }
        else
        {
            XmTextFieldSetString( SB_Text( sel), NULL) ;
            }
        }
    return ;
    }

/****************************************************************
 * This function returns the widget id of a SelectionBox child widget.
 ****************/
Widget
XmSelectionBoxGetChild(
        Widget sb,
        unsigned char which )
{
/****************/
	Widget	child = NULL;
	_XmWidgetToAppContext(sb);
	_XmAppLock(app);

	switch (which)
	{
		case XmDIALOG_LIST:
			child = SB_List (sb);
			break;

		case XmDIALOG_LIST_LABEL:
			child = SB_ListLabel (sb);
			break;

		case XmDIALOG_SELECTION_LABEL:
			child = SB_SelectionLabel (sb);
			break;

		case XmDIALOG_WORK_AREA:
			child = SB_WorkArea (sb);
			break;

		case XmDIALOG_TEXT:
			child = SB_Text (sb);
			break;

		case XmDIALOG_SEPARATOR:
			child = SB_Separator (sb);
			break;

		case XmDIALOG_OK_BUTTON:
			child = SB_OkButton (sb);
			break;

		case XmDIALOG_APPLY_BUTTON:
			child = SB_ApplyButton (sb);
			break;

		case XmDIALOG_CANCEL_BUTTON:
			child = SB_CancelButton (sb);
			break;

		case XmDIALOG_HELP_BUTTON:
			child = SB_HelpButton (sb);
			break;

		case XmDIALOG_DEFAULT_BUTTON:
			child = SB_DefaultButton (sb);
			break;

		default:
			XmeWarning( (Widget) sb, WARN_CHILD_TYPE);
			break;
	}
	_XmAppUnlock(app);
	return (child);
}

/****************************************************************
 * This function creates and returns a SelectionBox widget.
 ****************/
Widget
XmCreateSelectionBox(
        Widget p,
        String name,
        ArgList args,
        Cardinal n )
{
    return (XtCreateWidget (name, xmSelectionBoxWidgetClass, p, args, n));
}

Widget
XmVaCreateSelectionBox(
        Widget parent,
        char *name,
        ...)
{
    register Widget w;
    va_list var;
    int count;

    Va_start(var,name);
    count = XmeCountVaListSimple(var);
    va_end(var);


    Va_start(var, name);
    w = XmeVLCreateWidget(name,
                         xmSelectionBoxWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedSelectionBox(
        Widget parent,
        char *name,
        ...)
{
    Widget w = NULL;
    va_list var;
    int count;

    Va_start(var, name);
    count = XmeCountVaListSimple(var);
    va_end(var);

    Va_start(var, name);
    w = XmeVLCreateWidget(name,
                         xmSelectionBoxWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}

/****************************************************************
 * This convenience function creates a DialogShell and a SelectionBox
 *   child of the shell; returns the SelectionBox widget.
 ****************/
Widget
XmCreateSelectionDialog(
        Widget ds_p,
        String name,
        ArgList sb_args,
        Cardinal sb_n )
{
    ArgList		_sb_args;	/*  arglist for sb	*/
    Widget w;

    /*  allocate arglist, copy args, add dialog type arg
     */
    _sb_args = (ArgList) XtMalloc (sizeof (Arg) * (sb_n + 1));
    memcpy(_sb_args, sb_args, sizeof (Arg) * sb_n);
    XtSetArg (_sb_args[sb_n], XmNdialogType, XmDIALOG_SELECTION);  sb_n++;

    /*  create SelectionBoxDialog, free args, return */
    w = XmeCreateClassDialog (xmSelectionBoxWidgetClass,
			      ds_p, name, _sb_args, sb_n) ;
    XtFree((char *)_sb_args);
    return w ;
}


/****************************************************************
 * This convenience function creates a DialogShell and a SelectionBox
 *   child of the shell; returns the SelectionBox widget.
 ****************/
Widget
XmCreatePromptDialog(
        Widget ds_p,
        String name,
        ArgList sb_args,
        Cardinal sb_n )
{
    ArgList		_sb_args;	/*  arglist for sb	*/
    Widget w;

    /*  allocate arglist, copy args, add dialog type arg
     */
    _sb_args = (ArgList) XtMalloc (sizeof (Arg) * (sb_n + 1));
    memcpy(_sb_args, sb_args, sizeof (Arg) * sb_n);
    XtSetArg (_sb_args[sb_n], XmNdialogType, XmDIALOG_PROMPT);  sb_n++;

    /*  create SelectionBoxDialog, free args, return */
    w = XmeCreateClassDialog (xmSelectionBoxWidgetClass,
			      ds_p, name, _sb_args, sb_n) ;
    XtFree((char*)_sb_args);
    return w ;
}
