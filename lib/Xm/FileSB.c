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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: FileSB.c /main/21 1997/09/26 13:38:52 bill $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <Xm/XmosP.h>
#include "XmI.h"
#include "RepTypeI.h"
#include <Xm/FileSBP.h>
#include <Xm/GadgetP.h>
#include <Xm/AtomMgr.h>

#include <Xm/List.h>
#include <Xm/LabelG.h>
#include <Xm/RowColumnP.h>
#include <Xm/ArrowB.h>
#include <Xm/TextF.h>
#include <Xm/DialogS.h>
#include <Xm/VendorSEP.h>
#include <Xm/DragC.h>
#include <Xm/DropSMgr.h>
#include <Xm/Protocols.h>
#include <Xm/VaSimpleP.h>
#include <Xm/TraitP.h>
#include <Xm/ActivatableT.h>
#include "BulletinBI.h"
#include "GeoUtilsI.h"
#include "ManagerI.h"
#include "MessagesI.h"
#include "SelectioBI.h"
#include "TraversalI.h"
#include "VendorSEI.h"
#include "XmosI.h"
#include "XmStringI.h"
#include <sys/stat.h>

#define MESSAGE0     _XmMMsgMotif_0001

#define FILES_STRING          _XmMMsgResource_0007
#define FILTER_APPLY_STRING   _XmMMsgResource_0010

#define IsButton(w) \
(((XtPointer) XmeTraitGet((XtPointer) XtClass((w)), XmQTactivatable) != NULL))

#define IsAutoButton(fsb, w) (		\
      w == SB_OkButton(fsb) ||		\
      w == SB_ApplyButton(fsb) ||	\
      w == SB_CancelButton(fsb) ||	\
      w == SB_HelpButton(fsb))

#define SetupWorkArea(fsb) \
    if (_XmGeoSetupKid (boxPtr, SB_WorkArea(fsb)))    \
    {                                                 \
        layoutPtr->space_above = vspace;              \
        vspace = BB_MarginHeight(fsb);                \
        boxPtr += 2 ;                                 \
        ++layoutPtr ;                                 \
    }

typedef struct
    {   XmKidGeometry filter_label ;
        XmKidGeometry filter_text ;
        XmKidGeometry dir_list_label ;
        XmKidGeometry file_list_label ;
        Dimension   prefer_width ;
        Dimension   delta_width ;
        } FS_GeoExtensionRec, *FS_GeoExtension ;


/********    Static Function Declarations    ********/

static void ClassPartInitialize(
                        WidgetClass fsc) ;
static void Initialize(
                        Widget rw,
                        Widget nw,
                        ArgList args_in,
                        Cardinal *num_args) ;
static void Destroy(
                        Widget fsb) ;
static void DeleteChild(
                        Widget w) ;
static XtGeometryResult GeometryManager(
                        Widget w,
                        XtWidgetGeometry *req,
                        XtWidgetGeometry *reply) ;
static void ChangeManaged(
                        Widget wid) ;
static void FSBCreateFilterLabel(
                        XmFileSelectionBoxWidget fsb) ;
static void FSBCreateDirListLabel(
                        XmFileSelectionBoxWidget fsb) ;
static void FSBCreateDirList(
                        XmFileSelectionBoxWidget fsb) ;
static void FSBCreateFilterText(
                        XmFileSelectionBoxWidget fs) ;
static void FSBCreateDirText(
                        XmFileSelectionBoxWidget fs) ;
static void FSBCreateDirTextLabel(
                        XmFileSelectionBoxWidget fs) ;
static XmGeoMatrix FileSBGeoMatrixCreate(
                        Widget wid,
                        Widget instigator,
                        XtWidgetGeometry *desired) ;
static Boolean FileSelectionBoxNoGeoRequest(
                        XmGeoMatrix geoSpec) ;
static void ListLabelFix(
                        XmGeoMatrix geoSpec,
                        int action,
                        XmGeoMajorLayout layoutPtr,
                        XmKidGeometry rowPtr) ;
static void ListFix(
                        XmGeoMatrix geoSpec,
                        int action,
                        XmGeoMajorLayout layoutPtr,
                        XmKidGeometry rowPtr) ;
static void UpdateHorizPos(
                        Widget wid) ;
static void FileSearchProc(
                        Widget w,
                        XtPointer sd) ;
static void QualifySearchDataProc(
                        Widget w,
                        XtPointer sd,
                        XtPointer qsd) ;
static void FileSelectionBoxUpdate(
                        XmFileSelectionBoxWidget fs,
                        XmFileSelectionBoxCallbackStruct *searchData) ;
static void DirSearchProc(
                        Widget w,
                        XtPointer sd) ;
static void ListCallback(
                        Widget wid,
                        XtPointer client_data,
                        XtPointer call_data) ;
static Boolean SetValues(
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args_in,
                        Cardinal *num_args) ;
static void FSBGetDirectory(
                        Widget fs,
                        int resource,
                        XtArgVal *value) ;
static void FSBGetDirSpec(
                        Widget fs,
                        int resource,
                        XtArgVal *value) ;
static void FSBGetNoMatchString(
                        Widget fs,
                        int resource,
                        XtArgVal *value) ;
static void FSBGetPattern(
                        Widget fs,
                        int resource,
                        XtArgVal *value) ;
static void FSBGetFilterLabelString(
                        Widget fs,
                        int resource_offset,
                        XtArgVal *value) ;
static void FSBGetDirListLabelString(
                        Widget fs,
                        int resource_offset,
                        XtArgVal *value) ;
static void FSBGetDirListItems(
                        Widget fs,
                        int resource_offset,
                        XtArgVal *value) ;
static void FSBGetDirListItemCount(
                        Widget fs,
                        int resource_offset,
                        XtArgVal *value) ;
static void FSBGetListItems(
                        Widget fs,
                        int resource_offset,
                        XtArgVal *value) ;
static void FSBGetListItemCount(
                        Widget fs,
                        int resource_offset,
                        XtArgVal *value) ;
static void FSBGetDirMask(
                        Widget fs,
                        int resource_offset,
                        XtArgVal *value) ;
static Widget GetActiveText(
                        XmFileSelectionBoxWidget fsb,
                        XEvent *event) ;
static void FileSelectionBoxUpOrDown(
                        Widget wid,
                        XEvent *event,
                        String *argv,
                        Cardinal *argc) ;
static void FileSelectionBoxRestore(
                        Widget wid,
                        XEvent *event,
                        String *argv,
                        Cardinal *argc) ;
static void FileSelectionBoxFocusMoved(
                        Widget wid,
                        XtPointer client_data,
                        XtPointer data) ;
static void FileSelectionPB(
                        Widget wid,
                        XtPointer which_button,
                        XtPointer call_data) ;
static void FSBConvert(
		       Widget wid,
		       XtPointer client_data,
		       XtPointer cb_struct);
static void FilterFix(
                        XmGeoMatrix geoSpec,
                        int action,
                        XmGeoMajorLayout layoutPtr,
                        XmKidGeometry rowPtr) ;

/********    End Static Function Declarations    ********/


/*
 * transfer vector from translation manager action names to
 * address of routines
 */

static XtActionsRec ActionsTable[] =
{
    { "UpOrDown", FileSelectionBoxUpOrDown }, /* Motif 1.0 */
    { "SelectionBoxUpOrDown", FileSelectionBoxUpOrDown },
    { "SelectionBoxRestore", FileSelectionBoxRestore },
    };


/*---------------------------------------------------*/
/* widget resources                                  */
/*---------------------------------------------------*/
static XtResource resources[] =
{
    /* fileselection specific resources */

	{	XmNdirectory,
		XmCDirectory,
		XmRXmString,
		sizeof( XmString),
		XtOffsetOf( struct _XmFileSelectionBoxRec,
                                                 file_selection_box.directory),
		XmRXmString,
		(XtPointer) NULL    /* This will initialize to the current   */
	},                          /*   directory, because of XmNdirMask.   */
	{	XmNpattern,
		XmCPattern,
		XmRXmString,
		sizeof( XmString),
		XtOffsetOf( struct _XmFileSelectionBoxRec,
                                                   file_selection_box.pattern),
                XmRImmediate,
                (XtPointer) NULL  /* This really initializes to "*", because */
	},                        /*   of interaction with "XmNdirMask".     */
	{	XmNdirListLabelString,
		XmCDirListLabelString,
		XmRXmString,
		sizeof (XmString),
		XtOffsetOf( struct _XmFileSelectionBoxRec,
                                     file_selection_box.dir_list_label_string),
		XmRImmediate,
                (XtPointer) XmUNSPECIFIED
	},
        {       XmNdirListItems,
                XmCDirListItems,
                XmRXmStringTable,
                sizeof( XmStringTable),
		XtOffsetOf( struct _XmFileSelectionBoxRec,
                                            file_selection_box.dir_list_items),
                XmRImmediate,
                (XtPointer) NULL
        },
        {       XmNdirListItemCount,
                XmCDirListItemCount,
                XmRInt,
                sizeof( int),
		XtOffsetOf( struct _XmFileSelectionBoxRec,
                                       file_selection_box.dir_list_item_count),
                XmRImmediate,
                (XtPointer) XmUNSPECIFIED_COUNT
        },
	{	XmNfilterLabelString,
		XmCFilterLabelString,
		XmRXmString,
		sizeof (XmString),
		XtOffsetOf( struct _XmFileSelectionBoxRec,
                                       file_selection_box.filter_label_string),
		XmRImmediate,
                (XtPointer) XmUNSPECIFIED
	},
	{	XmNdirMask,
		XmCDirMask,
		XmRXmString,
		sizeof (XmString),
		XtOffsetOf( struct _XmFileSelectionBoxRec,
                                                  file_selection_box.dir_mask),
		XmRImmediate,
                (XtPointer) XmUNSPECIFIED
	},
	{	XmNnoMatchString,
		XmCNoMatchString,
		XmRXmString,
		sizeof (XmString),
		XtOffsetOf( struct _XmFileSelectionBoxRec,
                                           file_selection_box.no_match_string),
		XmRImmediate,
                (XtPointer) XmUNSPECIFIED
	},
	{	XmNqualifySearchDataProc,
		XmCQualifySearchDataProc,
		XmRProc,
		sizeof(XtProc),
		XtOffsetOf( struct _XmFileSelectionBoxRec,
                                  file_selection_box.qualify_search_data_proc),
		XmRImmediate,
		(XtPointer) QualifySearchDataProc
	},
	{	XmNdirSearchProc,
		XmCDirSearchProc,
		XmRProc,
		sizeof(XtProc),
		XtOffsetOf( struct _XmFileSelectionBoxRec,
                                           file_selection_box.dir_search_proc),
		XmRImmediate,
		(XtPointer) DirSearchProc
	},
	{	XmNfileSearchProc,
		XmCFileSearchProc,
		XmRProc,
		sizeof(XtProc),
		XtOffsetOf( struct _XmFileSelectionBoxRec,
                                          file_selection_box.file_search_proc),
		XmRImmediate,
		(XtPointer) FileSearchProc
	},
	{	XmNfileTypeMask,
		XmCFileTypeMask,
		XmRFileTypeMask,
		sizeof( unsigned char),
		XtOffsetOf( struct _XmFileSelectionBoxRec,
                                            file_selection_box.file_type_mask),
		XmRImmediate,
		(XtPointer) XmFILE_REGULAR
	},
	{	XmNlistUpdated,
		XmCListUpdated,
		XmRBoolean,
		sizeof(Boolean),
		XtOffsetOf( struct _XmFileSelectionBoxRec,
                                              file_selection_box.list_updated),
		XmRImmediate,
		(XtPointer) TRUE
	},
	{	XmNdirectoryValid,
		XmCDirectoryValid,
		XmRBoolean,
		sizeof(Boolean),
		XtOffsetOf( struct _XmFileSelectionBoxRec,
                                           file_selection_box.directory_valid),
		XmRImmediate,
		(XtPointer) TRUE
	},
	/* superclass resource default overrides */

	{	XmNdirSpec,
		XmCDirSpec,
		XmRXmString,
		sizeof( XmString),
		XtOffsetOf( struct _XmFileSelectionBoxRec, selection_box.text_string),
		XmRImmediate,
		(XtPointer) XmUNSPECIFIED
	},
	{	XmNautoUnmanage,
		XmCAutoUnmanage,
		XmRBoolean,
		sizeof(Boolean),
		XtOffsetOf( struct _XmFileSelectionBoxRec,
                                                 bulletin_board.auto_unmanage),
		XmRImmediate,
		(XtPointer) FALSE
	},
	{	XmNfileListLabelString,
		XmCFileListLabelString,
		XmRXmString,
		sizeof(XmString),
		XtOffsetOf( struct _XmFileSelectionBoxRec,
                                              selection_box.list_label_string),
		XmRImmediate,
		(XtPointer) XmUNSPECIFIED
	},
	{	XmNapplyLabelString,
		XmCApplyLabelString,
		XmRXmString,
		sizeof(XmString),
		XtOffsetOf( struct _XmFileSelectionBoxRec,
                                             selection_box.apply_label_string),
		XmRImmediate,
		(XtPointer) XmUNSPECIFIED
	},
	{	XmNdialogType,
		XmCDialogType,
		XmRSelectionType,
		sizeof(unsigned char),
		XtOffsetOf( struct _XmFileSelectionBoxRec, selection_box.dialog_type),
		XmRImmediate,
		(XtPointer) XmDIALOG_FILE_SELECTION
	},
	{	XmNfileListItems,
		XmCItems, XmRXmStringTable, sizeof (XmString *),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.list_items),
		XmRImmediate, NULL
	},
	{	XmNfileListItemCount,
		XmCItemCount, XmRInt, sizeof(int),
		XtOffsetOf( struct _XmSelectionBoxRec, selection_box.list_item_count),
		XmRImmediate, (XtPointer) XmUNSPECIFIED_COUNT
	},
	{
	    XmNpathMode,
	    XmCPathMode, XmRPathMode, sizeof(XtEnum),
	    XtOffsetOf(XmFileSelectionBoxRec,
		       file_selection_box.path_mode),
	    XmRImmediate, (XtPointer) XmPATH_MODE_FULL
	},
	{
	    XmNfileFilterStyle,
	    XmCFileFilterStyle, XmRFileFilterStyle, sizeof(XtEnum),
	    XtOffsetOf(XmFileSelectionBoxRec,
		       file_selection_box.file_filter_style),
	    XmRImmediate, (XtPointer) XmFILTER_NONE
	},
      {
	XmNdirTextLabelString,
        XmCDirTextLabelString,
        XmRXmString,
        sizeof(XmString),
        XtOffsetOf(XmFileSelectionBoxRec,
		       file_selection_box.dir_text_label_string),
        XmRImmediate,
        (XtPointer) NULL
    },
};

static XmSyntheticResource syn_resources[] =
{
  {	XmNdirectory,
	sizeof (XmString),
	XtOffsetOf( struct _XmFileSelectionBoxRec, file_selection_box.directory),
	FSBGetDirectory,
	(XmImportProc)NULL
  },
  {	XmNdirListLabelString,
	sizeof (XmString),
	XtOffsetOf( struct _XmFileSelectionBoxRec,
		 file_selection_box.dir_list_label_string),
	FSBGetDirListLabelString,
	(XmImportProc)NULL
  },
  {     XmNdirListItems,
        sizeof( XmString *),
	XtOffsetOf( struct _XmFileSelectionBoxRec, file_selection_box.dir_list_items),
        FSBGetDirListItems,
        (XmImportProc)NULL
  },
  {    XmNdirListItemCount,
        sizeof( int),
	XtOffsetOf( struct _XmFileSelectionBoxRec,
		 file_selection_box.dir_list_item_count),
        FSBGetDirListItemCount,
        (XmImportProc)NULL
  },
  {	XmNfilterLabelString,
	sizeof (XmString),
	XtOffsetOf( struct _XmFileSelectionBoxRec,
		 file_selection_box.filter_label_string),
	FSBGetFilterLabelString,
	(XmImportProc)NULL
  },
  {	XmNdirMask,
	sizeof( XmString),
	XtOffsetOf( struct _XmFileSelectionBoxRec, file_selection_box.dir_mask),
	FSBGetDirMask,
	(XmImportProc)NULL
  },
  {	XmNdirSpec,
	sizeof (XmString),
	XtOffsetOf( struct _XmFileSelectionBoxRec, selection_box.text_string),
	FSBGetDirSpec,
	(XmImportProc)NULL
  },
  {	XmNfileListLabelString,
	sizeof (XmString),
	XtOffsetOf( struct _XmFileSelectionBoxRec, selection_box.list_label_string),
	_XmSelectionBoxGetListLabelString,
	(XmImportProc)NULL
  },
  {	XmNfileListItems,
	sizeof (XmString *),
	XtOffsetOf( struct _XmSelectionBoxRec, selection_box.list_items),
	FSBGetListItems,
	(XmImportProc)NULL
  },
  {	XmNfileListItemCount,
	sizeof(int),
	XtOffsetOf( struct _XmSelectionBoxRec, selection_box.list_item_count),
	FSBGetListItemCount,
	(XmImportProc)NULL
  },
  {	XmNnoMatchString,
	sizeof (XmString),
	XtOffsetOf( struct _XmFileSelectionBoxRec,
		 file_selection_box.no_match_string),
	FSBGetNoMatchString,
	(XmImportProc)NULL
  },
  {	XmNpattern,
	sizeof( XmString),
	XtOffsetOf( struct _XmFileSelectionBoxRec,
		 file_selection_box.pattern),
	FSBGetPattern,
	(XmImportProc)NULL
  },
};

externaldef( xmfileselectionboxclassrec) XmFileSelectionBoxClassRec
                                                   xmFileSelectionBoxClassRec =
{
    {   /* core class record        */
	/* superclass	            */	(WidgetClass) &xmSelectionBoxClassRec,
	/* class_name		    */	"XmFileSelectionBox",
	/* widget_size		    */	sizeof(XmFileSelectionBoxRec),
	/* class_initialize	    */	(XtProc)NULL,
	/* class part init          */	ClassPartInitialize,
	/* class_inited		    */	FALSE,
	/* initialize		    */	Initialize,
	/* initialize hook	    */	(XtArgsProc)NULL,
	/* realize		    */	XtInheritRealize,
	/* actions		    */	ActionsTable,
	/* num_actions		    */	XtNumber(ActionsTable),
	/* resources		    */	resources,
	/* num_resources	    */	XtNumber(resources),
	/* xrm_class		    */	NULLQUARK,
	/* compress_motion	    */	TRUE,
	/* compress_exposure        */	XtExposeCompressMaximal,
	/* compress crossing        */	FALSE,
	/* visible_interest	    */	FALSE,
	/* destroy		    */	Destroy,
	/* resize		    */	XtInheritResize,
	/* expose		    */	XtInheritExpose,
	/* set_values		    */	SetValues,
	/* set_values_hook	    */	(XtArgsFunc)NULL,
	/* set_values_almost        */	XtInheritSetValuesAlmost,
	/* get_values_hook	    */	(XtArgsProc)NULL,
	/* accept_focus		    */	(XtAcceptFocusProc)NULL,
	/* version		    */	XtVersion,
	/* callback_private         */	(XtPointer)NULL,
	/* tm_table                 */	XtInheritTranslations,
	/* query_geometry	    */	XtInheritQueryGeometry,
	/* display_accelerator	    */	(XtStringProc)NULL,
	/* extension		    */	(XtPointer)NULL,
	},
    {   /* composite class record   */
	/* geometry manager         */	GeometryManager,
	/* set changed proc	    */	ChangeManaged,
	/* insert_child		    */	XtInheritInsertChild,
	/* delete_child 	    */	DeleteChild,
	/* extension		    */	(XtPointer)NULL,
	},
    {   /* constraint class record  */
	/* no additional resources  */	(XtResourceList)NULL,
	/* num additional resources */	0,
	/* size of constraint rec   */	0,
	/* constraint_initialize    */	(XtInitProc)NULL,
	/* constraint_destroy	    */  (XtWidgetProc)NULL,
	/* constraint_setvalue      */	(XtSetValuesFunc)NULL,
	/* extension                */	(XtPointer)NULL,
	},
    {   /* manager class record     */
	/* translations             */	XtInheritTranslations,
	/* get_resources            */	syn_resources,
	/* num_syn_resources        */	XtNumber(syn_resources),
	/* constraint_syn_resources */	(XmSyntheticResource *)NULL,
	/* num_constraint_syn_resources*/ 0,
        /* parent_process<           */  XmInheritParentProcess,
	/* extension		    */	(XtPointer)NULL,
	},
    {	/* bulletinBoard class record*/
	/* always_install_accelerators*/TRUE,
	/* geo_matrix_create        */	FileSBGeoMatrixCreate,
	/* focus_moved_proc         */	FileSelectionBoxFocusMoved,
	/* extension                */	(XtPointer)NULL,
	},
    {	/*selectionbox class record */
        /* list_callback            */  ListCallback,
	/* extension		    */	(XtPointer)NULL,
	},
    {	/* fileselection class record*/
	/* extension		    */	(XtPointer)NULL,
	}
};

externaldef( xmfileselectionboxwidgetclass) WidgetClass
     xmFileSelectionBoxWidgetClass = (WidgetClass)&xmFileSelectionBoxClassRec ;


/****************************************************************
 * Class Initialization.  Sets up accelerators and fast subclassing.
 ****************/
static void
ClassPartInitialize(
        WidgetClass fsc )
{
/****************/

    _XmFastSubclassInit( fsc, XmFILE_SELECTION_BOX_BIT) ;

    return ;
    }

/****************************************************************
 * This routine initializes an instance of the file selection widget.
 * Instance record fields which are shadow resources for child widgets and
 *   which are of an allocated type are set to NULL after they are used, since
 *   the memory identified by them is not owned by the File Selection Box.
 ****************/
static void
Initialize(
        Widget rw,		/* unused */
        Widget nw,
        ArgList args_in,	/* unused */
        Cardinal *num_args )	/* unused */
{
    XmFileSelectionBoxWidget new_w = (XmFileSelectionBoxWidget) nw ;
    Arg             args[16] ;
    int             numArgs ;
    XmFileSelectionBoxCallbackStruct searchData ;
    XmString local_xmstring ;
/****************/

    FS_StateFlags( new_w) = 0 ;
    FS_PrevDirModTime( new_w) = 0;

    /*	Here we have now to take care of XmUNSPECIFIED (CR 4856).
     */
    if (new_w->selection_box.list_label_string ==
	(XmString) XmUNSPECIFIED) {

	local_xmstring = XmStringCreate(FILES_STRING,
					XmFONTLIST_DEFAULT_TAG);
	numArgs = 0 ;
	XtSetArg( args[numArgs], XmNlabelString, local_xmstring) ; ++numArgs ;
	XtSetValues( SB_ListLabel( new_w), args, numArgs) ;
	XmStringFree(local_xmstring);

	new_w->selection_box.list_label_string = NULL ;
    }

    if (new_w->selection_box.apply_label_string ==
	(XmString) XmUNSPECIFIED) {

	local_xmstring = XmStringCreate(FILTER_APPLY_STRING,
					XmFONTLIST_DEFAULT_TAG);
	numArgs = 0 ;
	XtSetArg( args[numArgs], XmNlabelString, local_xmstring) ; ++numArgs ;
	XtSetValues( SB_ApplyButton( new_w), args, numArgs) ;
	XmStringFree(local_xmstring);

	new_w->selection_box.list_label_string = NULL ;
    }


    /* must set adding_sel_widgets to avoid adding these widgets to
     * selection work area
     */
    SB_AddingSelWidgets( new_w) = TRUE ;

    if(    !(SB_ListLabel( new_w))    )
    {   _XmSelectionBoxCreateListLabel( (XmSelectionBoxWidget) new_w) ;
        }
    if(    !(SB_List( new_w))    )
    {   _XmSelectionBoxCreateList( (XmSelectionBoxWidget) new_w) ;
        }
    if(    !(SB_SelectionLabel( new_w))    )
    {   _XmSelectionBoxCreateSelectionLabel( (XmSelectionBoxWidget) new_w) ;
        }
    if(    !(SB_Text( new_w))    )
    {   _XmSelectionBoxCreateText( (XmSelectionBoxWidget) new_w) ;
        }
    if(    !(SB_ApplyButton( new_w))    )
    {   _XmSelectionBoxCreateApplyButton( (XmSelectionBoxWidget) new_w) ;
        }
    if(    !(SB_OkButton( new_w))    )
    {   _XmSelectionBoxCreateOkButton( (XmSelectionBoxWidget) new_w) ;
        }
    if(    !(SB_CancelButton( new_w))    )
    {   _XmSelectionBoxCreateCancelButton( (XmSelectionBoxWidget) new_w) ;
        }
    if(    !(SB_HelpButton( new_w))    )
    {   _XmSelectionBoxCreateHelpButton( (XmSelectionBoxWidget) new_w) ;
        }


    FSBCreateFilterLabel( new_w) ;
    FS_FilterLabelString( new_w) = NULL ;

    FSBCreateDirListLabel( new_w) ;
    FS_DirListLabelString( new_w) = NULL ;

    FSBCreateFilterText( new_w);

    FSBCreateDirList( new_w) ;

    if(    FS_PathMode( new_w) ==  XmPATH_MODE_RELATIVE    )
      {
        FSBCreateDirTextLabel( new_w) ;
        FSBCreateDirText( new_w) ;
      } else {
	  FS_DirTextLabel( new_w) = NULL ;
	  FS_DirText( new_w) = NULL;
      }

    /* Since the DirSearchProc is going to be run during initialize,
    *   and since it has the responsibility to manage the directory list and
    *   the filter text, any initial values of the following resources can
    *   be ignored, since they will be immediately over-written.
    */
    FS_DirListItems( new_w) = NULL ;  /* Set/Get Values only.*/
    FS_DirListItemCount( new_w) = XmUNSPECIFIED_COUNT ; /* Set/Get Values only.*/

    SB_AddingSelWidgets( new_w) = FALSE;

    /* Remove the activate callbacks that our superclass
    *   may have attached to these buttons
    */
    XtRemoveAllCallbacks( SB_ApplyButton( new_w), XmNactivateCallback) ;
    XtRemoveAllCallbacks( SB_OkButton( new_w), XmNactivateCallback) ;
    XtRemoveAllCallbacks( SB_CancelButton( new_w), XmNactivateCallback) ;
    XtRemoveAllCallbacks( SB_HelpButton( new_w), XmNactivateCallback) ;

    XtAddCallback( SB_ApplyButton( new_w), XmNactivateCallback,
                          FileSelectionPB, (XtPointer) XmDIALOG_APPLY_BUTTON) ;
    XtAddCallback( SB_OkButton( new_w), XmNactivateCallback,
                             FileSelectionPB, (XtPointer) XmDIALOG_OK_BUTTON) ;
    XtAddCallback( SB_CancelButton( new_w), XmNactivateCallback,
                         FileSelectionPB, (XtPointer) XmDIALOG_CANCEL_BUTTON) ;
    XtAddCallback( SB_HelpButton( new_w), XmNactivateCallback,
                           FileSelectionPB, (XtPointer) XmDIALOG_HELP_BUTTON) ;


    if( FS_NoMatchString( new_w) == (XmString) XmUNSPECIFIED) {
	FS_NoMatchString( new_w) = XmStringConcatAndFree
	  (XmStringDirectionCreate(XmSTRING_DIRECTION_L_TO_R),
	   XmStringCreate(" [    ] ", XmFONTLIST_DEFAULT_TAG));
    }
    else {
	FS_NoMatchString( new_w) = XmStringCopy( FS_NoMatchString( new_w)) ;
    }

    searchData.reason = XmCR_NONE ;
    searchData.event = NULL ;
    searchData.value = NULL ;
    searchData.length = 0 ;
    searchData.mask = NULL ;
    searchData.mask_length = 0 ;
    searchData.dir = NULL ;
    searchData.dir_length = 0 ;
    searchData.pattern = NULL ;
    searchData.pattern_length = 0 ;

    /* The XmNdirSpec resource will be loaded into the Text widget by
    *   the Selection Box (superclass) Initialize routine.  It will be
    *   picked-up there by the XmNqualifySearchDataProc routine to fill
    *   in the value field of the search data.
    */

    if(FS_DirMask( new_w) != (XmString) XmUNSPECIFIED    )
    {
        searchData.mask = XmStringCopy(FS_DirMask( new_w)) ;
    } else {
	searchData.mask = XmStringCreate("*", XmFONTLIST_DEFAULT_TAG);
    }

    searchData.mask_length = XmStringLength( searchData.mask) ;

        /* The DirMask field will be set after subsequent call to
        *   the DirSearchProc.  Set field to NULL to prevent freeing of
        *   memory owned by request.
        */
    FS_DirMask( new_w) = (XmString) XmUNSPECIFIED ;

    if(    FS_Directory( new_w)    )
    {
        searchData.dir = XmStringCopy( FS_Directory( new_w)) ;
        searchData.dir_length = XmStringLength( searchData.dir) ;

        /* The Directory field will be set after subsequent call to
        *   the DirSearchProc.  Set field to NULL to prevent freeing of
        *   memory owned by request.
        */
        FS_Directory( new_w) = NULL ;
        }
    if(    FS_Pattern( new_w)    )
    {
        searchData.pattern = XmStringCopy( FS_Pattern( new_w)) ;
        searchData.pattern_length = XmStringLength( searchData.pattern) ;

        /* The Pattern field will be set after subsequent call to
        *   the DirSearchProc.  Set field to NULL to prevent freeing of
        *   memory owned by request.
        */
        FS_Pattern( new_w) = NULL ;
        }

    if(    !FS_QualifySearchDataProc( new_w)    )
    {   FS_QualifySearchDataProc( new_w) = QualifySearchDataProc ;
        }
    if(    !FS_DirSearchProc( new_w)    )
    {   FS_DirSearchProc( new_w) = DirSearchProc ;
        }
    if(    !FS_FileSearchProc( new_w)    )
    {   FS_FileSearchProc( new_w) = FileSearchProc ;
        }

    FileSelectionBoxUpdate( new_w, &searchData) ;

    XmStringFree( searchData.mask) ;
    XmStringFree( searchData.pattern) ;
    XmStringFree( searchData.dir) ;

    /* Add Convert callbacks to handle the FILE and FILENAME
       targets */
    XtAddCallback(FS_DirList(new_w), XmNconvertCallback,
		  FSBConvert, (XtPointer) new_w);
    XtAddCallback(SB_List(new_w), XmNconvertCallback,
		  FSBConvert, (XtPointer) new_w);

    /* Mark everybody as managed because no one else will.
    *   Only need to do this if we are the instantiated class.
    */
    if(    XtClass( new_w) == xmFileSelectionBoxWidgetClass    )
    {   XtManageChildren( new_w->composite.children,
                                                 new_w->composite.num_children) ;
        }
    return ;
    }

/****************************************************************/
static void
Destroy(
        Widget fsb )
{
/****************/

    XmStringFree( FS_NoMatchString( fsb)) ;
    XmStringFree( FS_Pattern( fsb)) ;
    XmStringFree( FS_Directory( fsb)) ;

    return ;
    }

/****************************************************************
 * This procedure is called to remove the child from
 *   the child list, and to allow the parent to do any
 *   neccessary clean up.
 ****************/
static void
DeleteChild(
        Widget w )
{
            XmFileSelectionBoxWidget fs ;
	    XtWidgetProc delete_child;
/****************/

    if(    XtIsRectObj( w)    )
    {
        fs = (XmFileSelectionBoxWidget) XtParent( w) ;

        if(    w == FS_FilterLabel( fs)    )
        {   FS_FilterLabel( fs) = NULL ;
            }
        else
        {   if(    w == FS_FilterText( fs)    )
            {   FS_FilterText( fs) = NULL ;
                }
            else
            {   if(   FS_DirList( fs)  &&  (w == XtParent( FS_DirList( fs)))  )
                {   FS_DirList( fs) = NULL ;
                    }
                else
                {   if(    w == FS_DirListLabel( fs)    )
                    {   FS_DirListLabel( fs) = NULL ;
                        }
                    }
                }
            }
        }
    _XmProcessLock();
    delete_child = ((XmSelectionBoxWidgetClass) xmSelectionBoxWidgetClass) ->
			    composite_class.delete_child;
    _XmProcessUnlock();

    (*delete_child)( w) ;
    return ;
    }

static XtGeometryResult
GeometryManager(
        Widget w,
        XtWidgetGeometry *req,
        XtWidgetGeometry *reply )
{
  XtGeometryResult rtnVal ;
  XtGeometryHandler geometry_manager;

  _XmProcessLock();
  geometry_manager = xmSelectionBoxClassRec.composite_class.geometry_manager;
  _XmProcessUnlock();

  rtnVal = (*geometry_manager)( w, req, reply) ;
  UpdateHorizPos( XtParent( w)) ;

  return rtnVal ;
}

static void
ChangeManaged(
        Widget wid )
{
  XtWidgetProc change_managed;

  _XmProcessLock();
  change_managed = xmSelectionBoxClassRec.composite_class.change_managed;
  _XmProcessUnlock();

  (*change_managed)( wid) ;

  UpdateHorizPos( wid) ;
}


/****************************************************************/
static void
FSBCreateFilterLabel(
        XmFileSelectionBoxWidget fsb )
{
/****************/

    if (FS_FilterLabelString( fsb) == (XmString) XmUNSPECIFIED)
	FS_FilterLabelString( fsb) = NULL;

    FS_FilterLabel( fsb) = _XmBB_CreateLabelG( (Widget) fsb,
					      FS_FilterLabelString( fsb),
					      "FilterLabel",
					      XmFilterStringLoc) ;
    return ;
    }
/****************************************************************/
static void
FSBCreateDirListLabel(
        XmFileSelectionBoxWidget fsb )
{
/****************/

    if (FS_DirListLabelString( fsb) == (XmString) XmUNSPECIFIED)
	FS_DirListLabelString( fsb) = NULL;

    FS_DirListLabel( fsb) = _XmBB_CreateLabelG( (Widget) fsb,
					       FS_DirListLabelString( fsb),
					       "Dir",
					       XmDirListStringLoc) ;
    return ;
    }

/****************************************************************
 * Create the directory List widget.
 ****************/
static void
FSBCreateDirList(
        XmFileSelectionBoxWidget fsb )
{
	Arg		al[20];
	register int	ac = 0;
            XtCallbackProc callbackProc ;
/****************/

    FS_DirListSelectedItemPosition( fsb) = 0 ;

    XtSetArg( al[ac], XmNvisibleItemCount,
                                        SB_ListVisibleItemCount( fsb)) ; ac++ ;
    XtSetArg( al[ac], XmNstringDirection, SB_StringDirection( fsb));  ac++;
    XtSetArg( al[ac], XmNselectionPolicy, XmBROWSE_SELECT);  ac++;
    XtSetArg( al[ac], XmNlistSizePolicy, XmCONSTANT);  ac++;
    XtSetArg( al[ac], XmNnavigationType, XmSTICKY_TAB_GROUP) ; ++ac ;

    FS_DirList( fsb) = XmCreateScrolledList( (Widget) fsb, "DirList", al, ac);

    callbackProc = ((XmSelectionBoxWidgetClass) fsb->core.widget_class)
                                          ->selection_box_class.list_callback ;
    if(    callbackProc    )
    {
        XtAddCallback( FS_DirList( fsb), XmNsingleSelectionCallback,
                                               callbackProc, (XtPointer) fsb) ;
        XtAddCallback( FS_DirList( fsb), XmNbrowseSelectionCallback,
                                               callbackProc, (XtPointer) fsb) ;
        XtAddCallback( FS_DirList( fsb), XmNdefaultActionCallback,
                                               callbackProc, (XtPointer) fsb) ;
        }
    XtManageChild( FS_DirList( fsb)) ;

    return ;
    }

/****************************************************************
 * Creates fs dir search filter text entry field.
 ****************/
static void
FSBCreateFilterText(
        XmFileSelectionBoxWidget fs )
{
            Arg             arglist[10] ;
            int             argCount ;
            char *          stext_value ;
            XtAccelerators  temp_accelerators ;
/****************/

    /* Get text portion from Compound String, and set
    *   fs_stext_charset and fs_stext_direction bits...
    */
    /* Should do this stuff entirely with XmStrings when the text
    *   widget supports it.
    */
    if(    !(stext_value = _XmStringGetTextConcat( FS_Pattern( fs)))    )
    {   stext_value = (char *) XtMalloc( 1) ;
        stext_value[0] = '\0' ;
        }
    argCount = 0 ;
    XtSetArg( arglist[argCount], XmNcolumns,
                                            SB_TextColumns( fs)) ; argCount++ ;
    XtSetArg( arglist[argCount], XmNresizeWidth, FALSE) ; argCount++ ;
    XtSetArg( arglist[argCount], XmNvalue, stext_value) ; argCount++ ;
    XtSetArg( arglist[argCount], XmNnavigationType,
                                             XmSTICKY_TAB_GROUP) ; argCount++ ;
    FS_FilterText( fs) = XmCreateTextField( (Widget) fs, "FilterText",
                                                           arglist, argCount) ;
    /*	Install text accelerators.
    */
    temp_accelerators = fs->core.accelerators ;
    fs->core.accelerators = SB_TextAccelerators( fs) ;
    XtInstallAccelerators( FS_FilterText( fs), (Widget) fs) ;
    fs->core.accelerators = temp_accelerators ;

    XtFree( stext_value) ;
    return ;
    }


static void
FSBCreateDirText(
        XmFileSelectionBoxWidget fs)
{
            Arg             arglist[10] ;
            int             argCount ;
            char *          stext_value ;
            XtAccelerators  temp_accelerators ;
/****************/

    /* Get text portion from Compound String, and set
    *   fs_stext_charset and fs_stext_direction bits...
    */
    /* Should do this stuff entirely with XmStrings when the text
    *   widget supports it.
    */
    if(    !(stext_value = _XmStringGetTextConcat( FS_Directory( fs)))    )
    {   stext_value = (char *) XtMalloc( 1) ;
        stext_value[0] = '\0' ;
        }
    argCount = 0 ;
    XtSetArg( arglist[argCount], XmNcolumns,
                                            SB_TextColumns( fs)) ; argCount++ ;
    XtSetArg( arglist[argCount], XmNresizeWidth, FALSE) ; argCount++ ;
    XtSetArg( arglist[argCount], XmNvalue, stext_value) ; argCount++ ;
    XtSetArg( arglist[argCount], XmNnavigationType,
                                             XmSTICKY_TAB_GROUP) ; argCount++ ;
    FS_DirText( fs) = XmCreateTextField( (Widget) fs, "DirText",
                                                           arglist, argCount) ;
    /*	Install text accelerators.
    */
    temp_accelerators = fs->core.accelerators ;
    fs->core.accelerators = SB_TextAccelerators( fs) ;
    XtInstallAccelerators(FS_DirText( fs), (Widget) fs) ;
    fs->core.accelerators = temp_accelerators ;

    XtFree( stext_value) ;
    return ;
    }

static void
FSBCreateDirTextLabel(
        XmFileSelectionBoxWidget fs)
{

  FS_DirTextLabel(fs) = _XmBB_CreateLabelG( (Widget) fs,
					   FS_DirTextLabelString(fs),
					   "DirL",
					   XmDirTextStringLoc) ;

}

/****************************************************************/
static void
FilterFix(
        XmGeoMatrix geoSpec,
        int action,		/* unused */
        XmGeoMajorLayout layoutPtr, /* unused */
        XmKidGeometry rowPtr )
{
            FS_GeoExtension extension ;
/****************/

    extension = (FS_GeoExtension) geoSpec->extension ;
    extension->filter_label = rowPtr ;
    rowPtr += 2 ;
    extension->filter_text = rowPtr ;
}

/****************************************************************
 * Get Geo matrix filled with kid widgets.
 ****************/
static XmGeoMatrix
FileSBGeoMatrixCreate(
        Widget wid,
        Widget instigator,
        XtWidgetGeometry *desired )
{
    XmFileSelectionBoxWidget fsb = (XmFileSelectionBoxWidget) wid ;
    XmGeoMatrix     geoSpec ;
    register XmGeoRowLayout  layoutPtr ;
    register XmKidGeometry   boxPtr ;
    XmKidGeometry   firstButtonBox ;
    Boolean         dirListLabelBox ;
    Boolean         listLabelBox ;
    Boolean         dirListBox ;
    Boolean         listBox ;
    Boolean         selLabelBox ;
    Boolean         filterLabelBox ;
    Dimension       vspace = BB_MarginHeight(fsb);
    int             i;

/*
 * Layout FileSelectionBox XmGeoMatrix.
 * Each row is terminated by leaving an empty XmKidGeometry and
 * moving to the next XmGeoRowLayout.
 */

    geoSpec = _XmGeoMatrixAlloc( XmFSB_MAX_WIDGETS_VERT,
                              fsb->composite.num_children,
                              sizeof( FS_GeoExtensionRec)) ;
    geoSpec->composite = (Widget) fsb ;
    geoSpec->instigator = (Widget) instigator ;
    if(    desired    )
    {   geoSpec->instig_request = *desired ;
        }
    geoSpec->margin_w = BB_MarginWidth( fsb) + fsb->manager.shadow_thickness ;
    geoSpec->margin_h = BB_MarginHeight( fsb) + fsb->manager.shadow_thickness ;
    geoSpec->no_geo_request = FileSelectionBoxNoGeoRequest ;

    layoutPtr = &(geoSpec->layouts->row) ;
    boxPtr = geoSpec->boxes ;

    /* menu bar */

    for (i = 0; i < fsb->composite.num_children; i++)
    {   Widget w = fsb->composite.children[i];

        if(    XmIsRowColumn(w)
            && ((XmRowColumnWidget)w)->row_column.type == XmMENU_BAR
            && w != SB_WorkArea(fsb)
            && _XmGeoSetupKid( boxPtr, w)    )
        {   layoutPtr->fix_up = _XmMenuBarFix ;
            boxPtr += 2;
            ++layoutPtr;
            vspace = 0;		/* fixup space_above of next row. */
            break;
            }
        }

    /* work area, XmPLACE_TOP */

    if (fsb->selection_box.child_placement == XmPLACE_TOP)
      SetupWorkArea(fsb);

    if(    _XmGeoSetupKid( boxPtr, FS_DirTextLabel( fsb))    )
    {
        layoutPtr->space_above = vspace;
        vspace = BB_MarginHeight(fsb);
        boxPtr += 2 ;
        ++layoutPtr ;
        }
    if(    _XmGeoSetupKid( boxPtr, FS_DirText( fsb))    )
    {
        boxPtr += 2 ;
        ++layoutPtr ;
        }

    /* filter label */

    filterLabelBox = FALSE ;
    if(    _XmGeoSetupKid( boxPtr, FS_FilterLabel( fsb))    )
    {
        filterLabelBox = TRUE ;
        layoutPtr->space_above = vspace;
        vspace = BB_MarginHeight(fsb);
        if(    FS_PathMode( fsb) ==  XmPATH_MODE_RELATIVE    )
          {
            layoutPtr->fix_up = FilterFix ;
          }
        boxPtr += 2 ;
        ++layoutPtr ;
        }

    /* filter text */

    if(    _XmGeoSetupKid( boxPtr, FS_FilterText( fsb))    )
    {
        if(    !filterLabelBox    )
        {   layoutPtr->space_above = vspace;
            vspace = BB_MarginHeight(fsb);
            }
	boxPtr += 2 ;
        ++layoutPtr ;
        }

    /* dir list and file list labels */

    if (LayoutIsRtoLM(fsb))
    {
        listLabelBox = FALSE ;
        if(    _XmGeoSetupKid( boxPtr, SB_ListLabel( fsb))    )
        {
           listLabelBox = TRUE ;
           ++boxPtr ;
           }
        dirListLabelBox = FALSE ;
        if(    _XmGeoSetupKid( boxPtr, FS_DirListLabel( fsb))    )
        {
           dirListLabelBox = TRUE ;
           ++boxPtr ;
           }
    }
    else
    {
      dirListLabelBox = FALSE ;
      if(    _XmGeoSetupKid( boxPtr, FS_DirListLabel( fsb))    )
	{
	  dirListLabelBox = TRUE ;
	  ++boxPtr ;
        }
      listLabelBox = FALSE ;
      if(    _XmGeoSetupKid( boxPtr, SB_ListLabel( fsb))    )
	{
	  listLabelBox = TRUE ;
	  ++boxPtr ;
        }
    }

    if(    dirListLabelBox  ||  listLabelBox    )
    {   layoutPtr->fix_up = ListLabelFix ;
        layoutPtr->space_above = vspace;
        vspace = BB_MarginHeight(fsb);
        layoutPtr->space_between = BB_MarginWidth( fsb) ;

        if(    dirListLabelBox && listLabelBox    )
        {   layoutPtr->sticky_end = TRUE ;
            }
        layoutPtr->fill_mode = XmGEO_PACK ;
        ++boxPtr ;
        ++layoutPtr ;
        }

    if (LayoutIsRtoLM(fsb))
    {
        listBox = FALSE ;
        if(    SB_List( fsb) &&  XtIsManaged( SB_List( fsb))
           && _XmGeoSetupKid( boxPtr, XtParent( SB_List( fsb)))    )
        {
           listBox = TRUE ;
           ++boxPtr ;
           }
        dirListBox = FALSE ;
        if(     FS_DirList( fsb) && XtIsManaged( FS_DirList(fsb))
           &&  _XmGeoSetupKid( boxPtr, XtParent( FS_DirList( fsb)))    )
        {
           dirListBox = TRUE ;
           ++boxPtr ;
           }
    }
    else
    {
      /* dir list and file list */

      dirListBox = FALSE ;
      if(     FS_DirList( fsb)  &&  XtIsManaged( FS_DirList( fsb))
	 &&  _XmGeoSetupKid( boxPtr, XtParent( FS_DirList( fsb)))    )
	{
	  dirListBox = TRUE ;
	  ++boxPtr ;
        }
      listBox = FALSE ;
      if(    SB_List( fsb)  &&  XtIsManaged( SB_List( fsb))
	 && _XmGeoSetupKid( boxPtr, XtParent( SB_List( fsb)))    )
	{
	  listBox = TRUE ;
	  ++boxPtr ;
        }
    }

    if(    dirListBox  || listBox    )
    {   layoutPtr->fix_up = ListFix ;
        layoutPtr->fit_mode = XmGEO_AVERAGING ;
        layoutPtr->space_between = BB_MarginWidth( fsb) ;
        layoutPtr->stretch_height = TRUE ;
        layoutPtr->min_height = 70 ;
        layoutPtr->even_height = 1 ;
        if(    !listLabelBox  &&  !dirListLabelBox    )
        {   layoutPtr->space_above = vspace;
            vspace = BB_MarginHeight(fsb);
            }
        ++boxPtr ;
        ++layoutPtr ;
        }

    /* work area, XmPLACE_ABOVE_SELECTION */

    if (fsb->selection_box.child_placement == XmPLACE_ABOVE_SELECTION)
      SetupWorkArea(fsb)

    /* selection label */

    selLabelBox = FALSE ;
    if(    _XmGeoSetupKid( boxPtr, SB_SelectionLabel( fsb))    )
    {   selLabelBox = TRUE ;
        layoutPtr->space_above = vspace;
        vspace = BB_MarginHeight(fsb);
        boxPtr += 2 ;
        ++layoutPtr ;
        }

    /* selection text */

    if(    _XmGeoSetupKid( boxPtr, SB_Text( fsb))    )
    {
        if(    !selLabelBox    )
        {   layoutPtr->space_above = vspace;
            vspace = BB_MarginHeight(fsb);
            }
        boxPtr += 2 ;
        ++layoutPtr ;
        }

    /* work area, XmPLACE_BELOW_SELECTION */

    if (fsb->selection_box.child_placement == XmPLACE_BELOW_SELECTION)
      SetupWorkArea(fsb)

    /* separator */

    if(    _XmGeoSetupKid( boxPtr, SB_Separator( fsb))    )
    {   layoutPtr->fix_up = _XmSeparatorFix ;
        layoutPtr->space_above = vspace;
        vspace = BB_MarginHeight(fsb);
        boxPtr += 2 ;
        ++layoutPtr ;
        }

    /* button row */

    firstButtonBox = boxPtr ;

    if (LayoutIsRtoLM(fsb))
    {
        if(    _XmGeoSetupKid( boxPtr, SB_HelpButton( fsb))    )
        {   ++boxPtr ;
            }
        if(    _XmGeoSetupKid( boxPtr, SB_CancelButton( fsb))    )
        {   ++boxPtr ;
            }
        if(    _XmGeoSetupKid( boxPtr, SB_ApplyButton( fsb))    )
        {   ++boxPtr ;
            }

	for (i = 0; i < fsb->composite.num_children; i++)
	{
	    Widget w = fsb->composite.children[fsb->composite.num_children-i-1];
	    if (IsButton(w) && !IsAutoButton(fsb,w) && w != SB_WorkArea(fsb))
	    {
		if (_XmGeoSetupKid( boxPtr, w))
		{   ++boxPtr ;
		    }
	    }
	}

        if(    _XmGeoSetupKid( boxPtr, SB_OkButton( fsb))    )
        {   ++boxPtr ;
            }
    }
    else
    {
      if(    _XmGeoSetupKid( boxPtr, SB_OkButton( fsb))    )
	{   ++boxPtr ;
	  }

      for (i = 0; i < fsb->composite.num_children; i++)
	{
	  Widget w = fsb->composite.children[i];
	  if (IsButton(w) && !IsAutoButton(fsb,w) && w != SB_WorkArea(fsb))
	    {
	      if (_XmGeoSetupKid( boxPtr, w))
		{   ++boxPtr ;
		  }
	    }
	}

      if(    _XmGeoSetupKid( boxPtr, SB_ApplyButton( fsb))    )
	{   ++boxPtr ;
	  }
      if(    _XmGeoSetupKid( boxPtr, SB_CancelButton( fsb))    )
	{   ++boxPtr ;
	  }
      if(    _XmGeoSetupKid( boxPtr, SB_HelpButton( fsb))    )
	{   ++boxPtr ;
	  }
    }

    if(    boxPtr != firstButtonBox    )
    {
        layoutPtr->fill_mode = XmGEO_CENTER ;
        layoutPtr->fit_mode = XmGEO_WRAP ;
        if(    !(SB_MinimizeButtons( fsb))    )
        {   layoutPtr->even_width = 1 ;
            }
        layoutPtr->space_above = vspace ;
        vspace = BB_MarginHeight(fsb) ;
        layoutPtr->even_height = 1 ;
	++layoutPtr ;
        }

    /* the end. */

    layoutPtr->space_above = vspace ;
    layoutPtr->end = TRUE ;
    return( geoSpec) ;
    }
/****************************************************************/
static Boolean
FileSelectionBoxNoGeoRequest(
        XmGeoMatrix geoSpec )
{
/****************/

    if(    BB_InSetValues( geoSpec->composite)
        && (XtClass( geoSpec->composite) == xmFileSelectionBoxWidgetClass)    )
    {
        return( TRUE) ;
        }
    return( FALSE) ;
    }

/****************************************************************
 * This routine saves the geometry pointers of the list labels so that they
 *   can be altered as appropriate by the ListFix routine.
 ****************/
static void
ListLabelFix(
        XmGeoMatrix geoSpec,
        int action,		/* unused */
        XmGeoMajorLayout layoutPtr, /* unused */
        XmKidGeometry rowPtr )
{
            FS_GeoExtension extension ;
/****************/

    extension = (FS_GeoExtension) geoSpec->extension ;
    if (LayoutIsRtoLM(geoSpec->composite))
    {
	extension->file_list_label = rowPtr++ ;
	extension->dir_list_label = rowPtr ;
    }
    else
    {
      extension->dir_list_label = rowPtr++ ;
      extension->file_list_label = rowPtr ;
    }

    return ;
    }

/****************************************************************
 * Geometry layout fixup routine for the directory and file lists.  This
 *   routine reduces the preferred width of the file list widget according
 *   to the length of the directory  path.
 * This algorithm assumes that each row has at least one box.
 ****************/
static void
ListFix(
        XmGeoMatrix geoSpec,
        int action,
        XmGeoMajorLayout layoutPtr,
        XmKidGeometry rowPtr )
{
    Dimension       listPathWidth ;
    XmListWidget    fileList ;
    XmKidGeometry   fileListGeo ;
    XmKidGeometry   dirListGeo ;
    Arg             argv[2] ;
    Cardinal        argc ;
    XmFontList      listFonts ;
    FS_GeoExtension extension ;
    int             listLabelsOffset ;
    /****************/

    if (LayoutIsRtoLM(geoSpec->composite))
	{
	    fileListGeo = rowPtr++;
	    dirListGeo = rowPtr ;
	}
    else
	{
	    dirListGeo = rowPtr++ ;
	    fileListGeo = rowPtr ;
	}

    if(    !fileListGeo->kid    )
	{   /* Only one list widget in this row, so do nothing.
	     */
	    return ;
        }
    extension = (FS_GeoExtension) geoSpec->extension ;
    fileList = (XmListWidget) SB_List( geoSpec->composite) ;
    switch(    action    )
    {
        case XmGET_PREFERRED_SIZE:
        {
            if(    FS_PathMode( geoSpec->composite) ==  XmPATH_MODE_FULL  )
              {
                argc = 0 ;
                XtSetArg( argv[argc], XmNfontList, &listFonts) ; ++argc ;
                XtGetValues( (Widget) fileList, argv, argc) ;

                listPathWidth = XmStringWidth( listFonts, FS_Directory(
                                                         geoSpec->composite)) ;

                if(    !(FS_StateFlags( geoSpec->composite) & XmFS_NO_MATCH)    )
                {
                    if(    listPathWidth < fileListGeo->box.width    )
                    {   fileListGeo->box.width -= listPathWidth ;
                        }
                    }
                if(    listPathWidth < dirListGeo->box.width    )
                {   dirListGeo->box.width -= listPathWidth ;
                    }
                if(    extension->dir_list_label
                    && (extension->dir_list_label->box.width
                                                  < dirListGeo->box.width)    )
                {   extension->dir_list_label->box.width = dirListGeo->box.width ;
                    }
                /* Drop through to pick up extension record field for either
                *   type of geometry request.
                */
              }
            else
              {
                if(    extension->dir_list_label
                    && (extension->dir_list_label->box.width
                                                  > dirListGeo->box.width)    )
                {   dirListGeo->box.width = extension->dir_list_label->box.width ;
                    }
                if(    extension->filter_label
                    && (extension->filter_label->box.width
                                                  > dirListGeo->box.width)    )
                {   dirListGeo->box.width = extension->filter_label->box.width ;
                    }
                if(    extension->file_list_label
                    && (extension->file_list_label->box.width
                                                 > fileListGeo->box.width)    )
                {   fileListGeo->box.width
                                          = extension->file_list_label->box.width ;
                    }
                if(    extension->filter_label
                    && extension->filter_text
                    && (fileListGeo->box.height >=
                          ((extension->filter_label->box.height
                              + extension->filter_text->box.height) << 1))    )
                {
                    dirListGeo->box.height = (fileListGeo->box.height -=
                                    (extension->filter_label->box.height
                                           + extension->filter_text->box.height
                                             + (layoutPtr - 1)->row.space_above
                                               + layoutPtr->row.space_above)) ;
                    }

                break ;
              }
            }
        case XmGET_ACTUAL_SIZE:
        {
            if(    FS_PathMode( geoSpec->composite) ==  XmPATH_MODE_FULL  )
              {
                extension->prefer_width = fileListGeo->box.width ;
              }
            break ;
            }
        case XmGEO_PRE_SET:
        {
            if(    FS_PathMode( geoSpec->composite) ==  XmPATH_MODE_FULL    )
              {
                if(    fileListGeo->box.width > extension->prefer_width    )
                {
                    /* Add extra space designated for file list to dir list
                    *   instead, assuring that file list only shows the file name
                    *   and not a segment of the path.
                    */
                    extension->delta_width = fileListGeo->box.width
                                                    - extension->prefer_width ;
                    fileListGeo->box.width -= extension->delta_width ;
		    if (LayoutIsRtoLM(geoSpec->composite))
		      dirListGeo->box.x -= extension->delta_width ;
		    else
		      fileListGeo->box.x += extension->delta_width ;
		    dirListGeo->box.width += extension->delta_width ;
                    }
                else
                {   extension->delta_width = 0 ;
                    }
                /* Set label boxes to be the same width and x dimension as the
                *   lists below them.
                */
                if(    extension->file_list_label    )
                {
                    if(    extension->file_list_label->box.width
                                                  < fileListGeo->box.width    )
                    {   extension->file_list_label->box.width
                                                     = fileListGeo->box.width ;
			extension->file_list_label->box.x = fileListGeo->box.x ;
                        }
		    else if (LayoutIsRtoLM(geoSpec->composite) &&
			     extension->file_list_label->box.width
                                                  > fileListGeo->box.width) {
		      extension->file_list_label->box.width
                                                     = fileListGeo->box.width ;
		    }
                    if(    extension->dir_list_label    )
                    {
			if (LayoutIsRtoLM(geoSpec->composite)) {
			  extension->dir_list_label->box.x = dirListGeo->box.x;
			  extension->dir_list_label->box.width =
			    dirListGeo->box.width;
			} else {
			  listLabelsOffset = extension->file_list_label->box.x
                                           - extension->dir_list_label->box.x ;
			  if(    listLabelsOffset
                                      > (int) layoutPtr->row.space_between    )
			    {   extension->dir_list_label->box.width =
                                            (Dimension) listLabelsOffset
                                               - layoutPtr->row.space_between ;
			      }
			}
                        }
                    }
              }
            else
              {
                /* Set label boxes to be the same width and x dimension as the
                *   lists below them.
                */
                if(    extension->file_list_label    )
                {
                    extension->file_list_label->box.width
                                                     = fileListGeo->box.width ;
                    extension->file_list_label->box.x = fileListGeo->box.x ;
                    }
                if(    extension->dir_list_label    )
                {
                    extension->dir_list_label->box.width = dirListGeo->box.width ;
                    extension->dir_list_label->box.x = dirListGeo->box.x ;
                    }
                if(    extension->filter_label
                    && extension->filter_text
                    && extension->file_list_label
                    && extension->dir_list_label    )
                {
                    Position dirListDelta = fileListGeo->box.y
                                              - extension->filter_text->box.y ;
                    extension->filter_label->box.width
                                      = extension->filter_text->box.width
                                      = extension->dir_list_label->box.width ;
		    extension->filter_label->box.x
		                      = extension->filter_text->box.x
                                      = extension->dir_list_label->box.x;
                    extension->file_list_label->box.y
                                             = extension->filter_label->box.y ;
                    fileListGeo->box.y -= dirListDelta ;
                    fileListGeo->box.height += dirListDelta ;
                    }
              }
            break ;
            }
        case XmGEO_POST_SET:
        {
            if(    FS_PathMode( geoSpec->composite)  ==  XmPATH_MODE_FULL   )
              {
                if(    extension->delta_width    )
                {   /* Undo the changes of PRE_SET, so subsequent re-layout
                    *   attempts will yield correct results.
                    */
                    fileListGeo->box.width += extension->delta_width ;
		    if (LayoutIsRtoLM(geoSpec->composite))
		      dirListGeo->box.x += extension->delta_width ;
		    else
		      fileListGeo->box.x -= extension->delta_width ;
                    dirListGeo->box.width -= extension->delta_width ;
                    }
              }
            break ;
            }
        }
    return ;
}


static void
UpdateHorizPos(
        Widget wid)
{
  Dimension listPathWidth ;
  Arg argv[2] ;
  Cardinal argc ;
  XmFontList listFonts ;
  XmString dirString = FS_Directory( wid) ;

  if(    FS_PathMode( wid)  ==  XmPATH_MODE_RELATIVE   )
    {
      return ;
    }

  if(    !(FS_StateFlags( wid) & XmFS_NO_MATCH)    )
    {
      /* Move horizontal position so path does not show in file list.
       */
      argc = 0 ;
      XtSetArg( argv[argc], XmNfontList, &listFonts) ; ++argc ;
      XtGetValues( SB_List( wid), argv, argc) ;
      listPathWidth = XmStringWidth( listFonts, dirString) ;
      XmListSetHorizPos( SB_List( wid), listPathWidth) ;
    }
  /* Move horizontal scroll position of directory list as far to the
   *   right as it will go, so that the right end of the list is
   *   never hidden.
   */
  argc = 0 ;
  XtSetArg( argv[argc], XmNfontList, &listFonts) ; ++argc ;
  XtGetValues( FS_DirList( wid), argv, argc) ;

  listPathWidth = XmStringWidth( listFonts, dirString) ;
  XmListSetHorizPos( FS_DirList( wid), listPathWidth) ;

  return ;
}


/****************************************************************/
static void
FileSearchProc(
        Widget w,
        XtPointer sd )
{
            XmFileSelectionBoxWidget fs = (XmFileSelectionBoxWidget) w ;
            XmFileSelectionBoxCallbackStruct * searchData
                                    = (XmFileSelectionBoxCallbackStruct *) sd ;
            String          dir ;
            String          pattern ;
            Arg             args[3] ;
            int             Index ;
            String *        fileList ;
            unsigned int    numFiles ;
            unsigned int    numItems = 0 ;
            unsigned int    numAlloc ;
            XmString *      XmStringFileList ;
            unsigned        dirLen ;
/****************/

    if(   !(dir = _XmStringGetTextConcat( searchData->dir))    )
    {   return ;
        }
    if(    !(pattern = _XmStringGetTextConcat( searchData->pattern))    )
    {   XtFree( dir) ;
        return ;
        }
    fileList = NULL ;
    _XmOSBuildFileList( dir, pattern, FS_FileTypeMask( fs),
                                            &fileList,  &numFiles, &numAlloc) ;
    if(    fileList  &&  numFiles    ) {
	Boolean showDotFiles = (FS_FileFilterStyle( fs) == XmFILTER_NONE) ;

	if(    numFiles > 1    )
	    qsort( (void *)fileList, numFiles, sizeof( char *),
		  _XmOSFileCompare) ;

        XmStringFileList = (XmString *) XtMalloc(numFiles * sizeof( XmString)) ;

        Index = 0 ;
	dirLen = strlen( dir) ;

	while(    Index < numFiles    ) {
	    if(    showDotFiles
	       || ((fileList[Index])[dirLen] != '.')    ) {
                  if(    FS_PathMode( fs) ==  XmPATH_MODE_FULL    )
		      XmStringFileList[numItems++] =
			XmStringGenerate(fileList[Index],
					 XmFONTLIST_DEFAULT_TAG,
					 XmCHARSET_TEXT, NULL);
		  else
		      XmStringFileList[numItems++] =
			XmStringGenerate(&(fileList[Index])[dirLen],
					 XmFONTLIST_DEFAULT_TAG,
					 XmCHARSET_TEXT, NULL) ;
	      }
	    ++Index ;
	}

	/* Update the list.
        */
        Index = 0 ;
        XtSetArg( args[Index], XmNitems, XmStringFileList) ; Index++ ;
        XtSetArg( args[Index], XmNitemCount, numItems) ; Index++ ;
        XtSetValues( SB_List( fs), args, Index) ;

        Index = numFiles ;
        while(    Index--    )
        {   XtFree( fileList[Index]) ;
            }
        while(    numItems--    )
        {   XmStringFree( XmStringFileList[numItems]) ;
            }
        XtFree( (char *) XmStringFileList) ;
        }
    else
    {   XtSetArg( args[0], XmNitemCount, 0) ;
        XtSetValues( SB_List( fs), args, 1) ;
        }
    FS_ListUpdated( fs) = TRUE ;

    XtFree( (char *) fileList) ;
    XtFree( pattern) ;
    XtFree( dir) ;
    return ;
    }


/****************************************************************
 * This routine validates and allocates new copies of all searchData
 *   fields that are required by the DirSearchProc and the FileSearchProc
 *   routines.  The default routines require only the "dir" and "pattern"
 *   fields to be filled with appropriate qualified non-null XmStrings.
 * Any of the fields of the searchData passed into this routine may be NULL.
 *   Generally, only those fields which signify changes due to a user action
 *   will be passed into this routine.  This data should always override
 *   data derived from other sources.
 * The caller is responsible to free the XmStrings of all (non-null) fields
 *   of the qualifiedSearchData record.
 ****************/
static void
QualifySearchDataProc(
        Widget w,
        XtPointer sd,
        XtPointer qsd )
{
            XmFileSelectionBoxWidget fs = (XmFileSelectionBoxWidget) w ;
            XmFileSelectionBoxCallbackStruct * searchData
                                    = (XmFileSelectionBoxCallbackStruct *) sd ;
            XmFileSelectionBoxCallbackStruct * qualifiedSearchData
                                   = (XmFileSelectionBoxCallbackStruct *) qsd ;
            String          valueString ;
            String          patternString ;
            String          dirString ;
            String          maskString ;
            String          qualifiedDir ;
            String          qualifiedPattern ;
            String          qualifiedMask ;
            char *          dirPartPtr ;
            char *          patternPartPtr ;
            unsigned int    qDirLen ;
/****************/

    maskString = _XmStringGetTextConcat( searchData->mask) ;
    dirString = _XmStringGetTextConcat( searchData->dir) ;
    patternString = _XmStringGetTextConcat( searchData->pattern) ;

    if(    !maskString
        || (dirString  &&  patternString)
        || (dirString  &&  maskString  &&  (maskString[0] != '/'))    )
    {
        if(    !dirString    )
        {   dirString = _XmStringGetTextConcat( FS_Directory( fs)) ;
            }
        if(    !patternString    )
        {
            if(    maskString  &&  (maskString[0] != '/')    )
            {
                patternString = maskString ;
                maskString = NULL ;
                }
            else
            {   patternString = _XmStringGetTextConcat( FS_Pattern( fs)) ;
                }
            }
        _XmOSQualifyFileSpec( dirString, patternString,
                                            &qualifiedDir, &qualifiedPattern) ;
        }
    else
    {   patternPartPtr = _XmOSFindPatternPart( maskString) ;

        if(    patternPartPtr != maskString    )
        {
	    /*** This need to be re-think with Xmos.c in mind. dd */

            /* To avoid allocating memory and copying part of the mask string,
            *   just stuff '\0' at the '/' which is between the directory part
            *   and the pattern part.  The QualifyFileSpec below does not
            *   require the trailing '/', and it will assure that the resulting
            *   qualifiedDir will have the required trailing '/'.
            * Must check to see if the directory part of the mask
            *   string is "//", so that this information is not lost when
            *   deleting the '/' before the pattern part.  Embedded "//"
            *   sequences are not protected, but root specifications are.
            */
            *(patternPartPtr - 1) = '\0' ;

            if(    !*maskString
                || ((*maskString == '/')  &&  !maskString[1])    )
            {
                if(    !*maskString    )
                {   /* The '/' that was replaced with '\0' above was the only
                    *    character in the directory specification (root
                    *    directory "/"), so simply restore it.
                    */
                    dirPartPtr = "/" ;
                    }
                else
                {   /* The directory specification was "//" before the
                    *   trailing '/' was deleted, so restore original.
                    */
                    dirPartPtr = "//" ;
                    }
                }
            else
            {   /* Is non-root directory specification, so its ok to have
                *   deleted the '/', since we are not protecting embedded
                *   "//" path specifications from reduction to a single slash.
                */
                dirPartPtr = maskString ;
                }
            }
        else
        {   dirPartPtr = NULL ;
            }
        if(    dirString    )
        {   dirPartPtr = dirString ;
            }
        if(    patternString    )
        {   patternPartPtr = patternString ;
            }
        _XmOSQualifyFileSpec( dirPartPtr, patternPartPtr,
                                            &qualifiedDir, &qualifiedPattern) ;
        }
    qDirLen = strlen( qualifiedDir) ;
    qualifiedMask = XtMalloc( 1 + qDirLen + strlen( qualifiedPattern)) ;
    strcpy( qualifiedMask, qualifiedDir) ;
    strcpy( &qualifiedMask[qDirLen], qualifiedPattern) ;

    qualifiedSearchData->reason = searchData->reason ;
    qualifiedSearchData->event = searchData->event ;

    if(    searchData->value    )
    {   qualifiedSearchData->value = XmStringCopy( searchData->value) ;
        valueString = NULL ;
        }
    else
    {
	if(    FS_PathMode( fs)  ==  XmPATH_MODE_FULL   )
          {
	      valueString = XmTextFieldGetString( SB_Text( fs)) ;
	  }else
          {
	      String fileStr = XmTextFieldGetString( SB_Text( fs)) ;

            if(    (fileStr == NULL)
                || (*fileStr == '\0')
                || (*fileStr == '/')
                || (FS_Directory( fs) == NULL)    )
              {
                valueString = fileStr ;
              }
            else
              {
                String dirStr = _XmStringGetTextConcat( FS_Directory( fs)) ;
                unsigned dirLen = strlen( dirStr) ;

                valueString = XtMalloc( dirLen + strlen( fileStr) + 1) ;
                strcpy( valueString, dirStr) ;
                strcpy( &valueString[dirLen], fileStr) ;
                XtFree( fileStr) ;
                XtFree( dirStr) ;
              }
          }
        qualifiedSearchData->value =
	  XmStringGenerate(valueString, XmFONTLIST_DEFAULT_TAG,
			   XmCHARSET_TEXT, NULL);
        }
    qualifiedSearchData->length = XmStringLength( qualifiedSearchData->value) ;

    qualifiedSearchData->mask =
      XmStringGenerate(qualifiedMask, XmFONTLIST_DEFAULT_TAG,
		       XmCHARSET_TEXT, NULL) ;
    qualifiedSearchData->mask_length = XmStringLength(
                                                   qualifiedSearchData->mask) ;

    qualifiedSearchData->dir =
      XmStringGenerate(qualifiedDir, XmFONTLIST_DEFAULT_TAG,
		       XmCHARSET_TEXT, NULL) ;
    qualifiedSearchData->dir_length = XmStringLength(
                                                    qualifiedSearchData->dir) ;

    qualifiedSearchData->pattern =
      XmStringGenerate(qualifiedPattern, XmFONTLIST_DEFAULT_TAG,
		       XmCHARSET_TEXT, NULL) ;
    qualifiedSearchData->pattern_length = XmStringLength(
                                                qualifiedSearchData->pattern) ;
    XtFree( valueString) ;
    XtFree( qualifiedMask) ;
    XtFree( qualifiedPattern) ;
    XtFree( qualifiedDir) ;
    XtFree( patternString) ;
    XtFree( dirString) ;
    XtFree( maskString) ;
    return ;
    }

/****************************************************************/
static void
FileSelectionBoxUpdate(
        XmFileSelectionBoxWidget fs,
        XmFileSelectionBoxCallbackStruct *searchData )
{
            Arg             ac[5] ;
            Cardinal        al ;
            int             itemCount ;
            XmString        item ;
            String          textValue ;
            String          dirString ;
            String          maskString ;
            String          patternString ;
            int             len ;
            XmFileSelectionBoxCallbackStruct qualifiedSearchData ;
/****************/

    /* Unmap file list, so if it takes a long time to generate the
    *   list items, the user doesn't wonder what is going on.
    */
    XtSetMappedWhenManaged( SB_List( fs), FALSE) ;
    XFlush( XtDisplay( fs)) ;

    if(    FS_StateFlags( fs) & XmFS_NO_MATCH    )
    {   XmListDeleteAllItems( SB_List( fs)) ;
        }
    FS_StateFlags( fs) |= XmFS_IN_FILE_SEARCH ;

    (*FS_QualifySearchDataProc( fs))( (Widget) fs, (XtPointer) searchData,
                                            (XtPointer) &qualifiedSearchData) ;
    FS_ListUpdated( fs) = FALSE ;
    FS_DirectoryValid( fs) = FALSE ;

    (*FS_DirSearchProc( fs))( (Widget) fs, (XtPointer) &qualifiedSearchData) ;

    if(    FS_DirectoryValid( fs)    )
    {
        (*FS_FileSearchProc( fs))( (Widget) fs,
                                            (XtPointer) &qualifiedSearchData) ;
        /* Now update the Directory and Pattern resources.
        */
        if(    !XmStringCompare( qualifiedSearchData.dir, FS_Directory( fs))  )
        {   if(    FS_Directory( fs)    )
            {   XmStringFree( FS_Directory( fs)) ;
                }
            FS_Directory( fs) = XmStringCopy( qualifiedSearchData.dir) ;
            }

        if(   !XmStringCompare( qualifiedSearchData.pattern, FS_Pattern( fs)) )
        {   if(    FS_Pattern( fs)    )
            {   XmStringFree( FS_Pattern( fs)) ;
                }
            FS_Pattern( fs) = XmStringCopy( qualifiedSearchData.pattern) ;
            }
        /* Also update the filter text.
        */
        if(    FS_PathMode( fs)  ==  XmPATH_MODE_FULL   )
          {
            if ((dirString = _XmStringGetTextConcat( FS_Directory(fs))) != NULL)
            {
                if((patternString=_XmStringGetTextConcat(FS_Pattern(fs)))!=NULL)
                  {
                    len = strlen( dirString) ;
                    maskString = XtMalloc( len + strlen( patternString) + 1) ;
                    strcpy( maskString, dirString) ;
                    strcpy( &maskString[len], patternString) ;

		    XmTextFieldSetString( FS_FilterText( fs), maskString) ;
		    XmTextFieldSetInsertionPosition( FS_FilterText( fs),
			     XmTextFieldGetLastPosition( FS_FilterText( fs))) ;
                    XtFree( maskString) ;
                    XtFree( patternString) ;
                  }
                XtFree( dirString) ;
              }
          }
        else
          {
            if ((dirString = _XmStringGetTextConcat( FS_Directory(fs))) != NULL)
              {
                XmTextFieldSetString( FS_DirText( fs), dirString) ;
                XmTextFieldSetInsertionPosition( FS_DirText( fs),
                                XmTextFieldGetLastPosition( FS_DirText( fs))) ;
                XtFree( dirString) ;
              }
            if((patternString=_XmStringGetTextConcat(FS_Pattern(fs)))!=NULL)
              {
                XmTextFieldSetString( FS_FilterText( fs), patternString) ;
                XmTextFieldSetInsertionPosition( FS_FilterText( fs),
                             XmTextFieldGetLastPosition( FS_FilterText( fs))) ;
                XtFree( patternString) ;
              }
          }
        }
    FS_StateFlags( fs) &= ~XmFS_IN_FILE_SEARCH ;

    al = 0 ;
    XtSetArg( ac[al], XmNitemCount, &itemCount) ; ++al ;
    XtGetValues( SB_List( fs), ac, al) ;

    if(    itemCount    )
    {   FS_StateFlags( fs) &= ~XmFS_NO_MATCH ;
        }
    else
    {   FS_StateFlags( fs) |= XmFS_NO_MATCH ;

        if(    (item = FS_NoMatchString( fs)) != NULL    )
        {   al = 0 ;
            XtSetArg( ac[al], XmNitems, &item) ; ++al ;
            XtSetArg( ac[al], XmNitemCount, 1) ; ++al ;
            XtSetValues( SB_List( fs), ac, al) ;
            }
        }
    if(    FS_ListUpdated( fs)    )
    {
        if(    FS_PathMode( fs)  ==  XmPATH_MODE_FULL   )
          {
            if ((textValue = _XmStringGetTextConcat(FS_Directory(fs))) != NULL)
              {
		  XmTextFieldSetString( SB_Text( fs), textValue) ;
		  XmTextFieldSetInsertionPosition( SB_Text( fs),
			     XmTextFieldGetLastPosition( SB_Text( fs))) ;
                XtFree( textValue) ;
              }
          }
        else
          {
            XmTextFieldSetString( SB_Text( fs), NULL) ;
          }

        _XmBulletinBoardSizeUpdate( (Widget) fs) ;

        UpdateHorizPos( (Widget) fs) ;
        }
    XtSetMappedWhenManaged( SB_List( fs), TRUE) ;

    XmStringFree( qualifiedSearchData.value) ;
    XmStringFree( qualifiedSearchData.mask) ;
    XmStringFree( qualifiedSearchData.dir) ;
    XmStringFree( qualifiedSearchData.pattern) ;
    return ;
    }

/****************************************************************
 * This loads the list widget with a directory list based
 *   on the directory specification.
 ****************/
static void
DirSearchProc(
        Widget w,
        XtPointer sd )
{
            XmFileSelectionBoxWidget fs = (XmFileSelectionBoxWidget) w ;
            XmFileSelectionBoxCallbackStruct * searchData
                                    = (XmFileSelectionBoxCallbackStruct *) sd ;
            String          qualifiedDir ;
            Arg             args[10] ;
            int             Index ;
            String *        dirList ;
            unsigned int    numDirs ;
            unsigned int    numAlloc ;
            XmString *      XmStringDirList ;
            struct stat     curDirStats ;
            time_t          curDirModTime = 0 ;
	    unsigned        numItems = 0 ;
	    unsigned        dirLen ;
	    Boolean showDotFiles = (FS_FileFilterStyle( fs) == XmFILTER_NONE) ;
/****************/

    /* Sometimes a directory has changed contents even though
     *   the FileSB has not navigated to a different directory;
     *   the directory list needs to be updated in this case.
     * A simple "one level cache" saves the modification time of the
     *   most recently accessed FileSB directory.  This is used to
     *   avoid completely re-creating the directory list when the
     *   directory contents haven't changed.
     * While not perfect, this simple implementation will improve
     *   performance for 99 percent of the cases when only the filter
     *   is being changed and the directory list need not be touched.
     * An interface for the "stat" functionality used here should
     *   re-implemented in Xmos.c.
     */

    if(    (qualifiedDir = _XmStringGetTextConcat( searchData->dir))
                                                                   == NULL    )
      {
        if(    _XmGetAudibleWarning((Widget) fs) == XmBELL    )
          {
            XBell( XtDisplay( fs), 0) ;
          }
        return ;
      }
    if( !stat( qualifiedDir, &curDirStats)    )
      {
        curDirModTime = curDirStats.st_mtime ;
      }
    if(    (FS_StateFlags( fs) & XmFS_DIR_SEARCH_PROC)
        || (curDirModTime != FS_PrevDirModTime(fs))
        || !XmStringCompare( searchData->dir, FS_Directory( fs))    )
    {
        FS_StateFlags( fs) &= ~XmFS_DIR_SEARCH_PROC ;

        /* Directory is different than current, so update dir list.
        */
        dirList = NULL ;
        _XmOSGetDirEntries( qualifiedDir, "*", XmFILE_DIRECTORY, FALSE, TRUE,
                                               &dirList, &numDirs, &numAlloc) ;
        if(    !numDirs    )
        {
            /* Directory list is empty, so have attempted to go
            *   into a directory without permissions.  Don't do it!
            */
            if(    _XmGetAudibleWarning((Widget) fs) == XmBELL    )
            {   XBell( XtDisplay( fs), 0) ;
                }
            XtFree( (char *) qualifiedDir) ;
	    XtFree((char *) dirList) ;
            return ;
            }
        if(    numDirs > 1    )
        {   qsort( (void *)dirList, numDirs, sizeof( char *), _XmOSFileCompare) ;
            }
        XmStringDirList = (XmString *) XtMalloc( numDirs * sizeof( XmString)) ;

	Index = 0 ;
	dirLen = strlen( qualifiedDir) ;

	while(    Index < numDirs    ) {
	    /* Assume first entry is "." and second is "..".
	     */
	    if( showDotFiles
	       || (Index == 1)
	       || ((dirList[Index])[dirLen] != '.')) {

		if(    FS_PathMode( fs)  ==  XmPATH_MODE_FULL   )
		    XmStringDirList[numItems++] =
			XmStringGenerate(dirList[Index],
					 XmFONTLIST_DEFAULT_TAG,
					 XmCHARSET_TEXT, NULL) ;
		else
		    XmStringDirList[numItems++] =
			XmStringGenerate(&(dirList[Index])[dirLen],
					 XmFONTLIST_DEFAULT_TAG,
					 XmCHARSET_TEXT, NULL) ;
	    }
	    ++Index ;
	}

        /* Update the list.  */
        Index = 0;
        XtSetArg( args[Index], XmNitems, XmStringDirList) ; Index++ ;
        XtSetArg( args[Index], XmNitemCount, numItems) ; Index++ ;
        XtSetArg( args[Index], XmNtopItemPosition, 1) ; Index++ ;
        XtSetValues( FS_DirList( fs), args, Index);

        XmListSelectPos( FS_DirList( fs), 1, FALSE) ;
        FS_DirListSelectedItemPosition( fs) = 1 ;

        Index = numDirs ;
        while(    Index--    )
        {   XtFree( dirList[Index]) ;
            }
        XtFree( (char *) dirList) ;

        while(    numItems--    )
        {
            XmStringFree( XmStringDirList[numItems]) ;
            }
        XtFree( (char *) XmStringDirList) ;
        FS_ListUpdated( fs) = TRUE ;
        FS_PrevDirModTime( fs) = curDirModTime ;
        }
    XtFree( (char *) qualifiedDir) ;

    FS_DirectoryValid( fs) = TRUE ;
    return ;
    }

/****************************************************************
 * Process callback from either List of the File Selection Box.
 ****************/
static void
ListCallback(
        Widget wid,
        XtPointer client_data,
        XtPointer call_data )
{
            XmListCallbackStruct * callback ;
            XmFileSelectionBoxWidget fsb ;
            XmGadgetClass   gadget_class ;
            XmGadget        dbutton ;
            XmFileSelectionBoxCallbackStruct change_data ;
            XmFileSelectionBoxCallbackStruct qualified_change_data ;
            String          textValue ;
            String          dirString ;
            String          maskString ;
            String          patternString ;
            int             len ;
/****************/

    callback = (XmListCallbackStruct *) call_data ;
    fsb = (XmFileSelectionBoxWidget) client_data ;

    switch(    callback->reason    )
    {
        case XmCR_BROWSE_SELECT:
        case XmCR_SINGLE_SELECT:
        {
            if(    wid == FS_DirList( fsb)    )
            {
                FS_DirListSelectedItemPosition( fsb)
                                                    = callback->item_position ;
                change_data.event  = NULL ;
                change_data.reason = XmCR_NONE ;
                change_data.value = NULL ;
                change_data.length = 0 ;
                textValue = XmTextFieldGetString( FS_FilterText( fsb)) ;
                change_data.mask =
		  XmStringGenerate(textValue, XmFONTLIST_DEFAULT_TAG,
				   XmCHARSET_TEXT, NULL) ;
                change_data.mask_length = XmStringLength( change_data.mask) ;
                if(    FS_PathMode( fsb)  ==  XmPATH_MODE_FULL   )
                  {
                    change_data.dir = XmStringCopy( callback->item) ;
                  }
                else
                  {
                    change_data.dir = XmStringConcat( FS_Directory( fsb),
                                                              callback->item) ;
                  }
                change_data.dir_length = XmStringLength( change_data.dir) ;
                change_data.pattern = NULL ;
                change_data.pattern_length = 0 ;

                /* Qualify and then update the filter text.
                */
                (*FS_QualifySearchDataProc( fsb))( (Widget) fsb,
                                     (XtPointer) &change_data,
                                          (XtPointer) &qualified_change_data) ;

                if(    FS_PathMode( fsb)  ==  XmPATH_MODE_FULL   )
                  {
                    if ((dirString =
		     _XmStringGetTextConcat(qualified_change_data.dir)) != NULL)
                      {   if ((patternString =
			 _XmStringGetTextConcat(qualified_change_data.pattern))
			 != NULL)
                          {
                            len = strlen( dirString) ;
                            maskString = XtMalloc( len
                                                 + strlen( patternString) + 1) ;
                            strcpy( maskString, dirString) ;
                            strcpy( &maskString[len], patternString) ;
                            XmTextFieldSetString( FS_FilterText( fsb),
                                                                  maskString) ;
                            XmTextFieldSetInsertionPosition( FS_FilterText( fsb),
                                                    XmTextFieldGetLastPosition(
                                                        FS_FilterText( fsb))) ;
                            XtFree( maskString) ;
                            XtFree( patternString) ;
                          }
                        XtFree( dirString) ;
                      }
                  }
                else
                  {
                    if ((dirString =
		     _XmStringGetTextConcat(qualified_change_data.dir)) != NULL)
                      {
                        XmTextFieldSetString( FS_DirText( fsb), dirString) ;
                        XmTextFieldSetInsertionPosition( FS_DirText( fsb),
                               XmTextFieldGetLastPosition( FS_DirText( fsb))) ;
                         XtFree( dirString) ;
                      }
                    if ((patternString =
			 _XmStringGetTextConcat(qualified_change_data.pattern))
			 != NULL)
                      {
                        XmTextFieldSetString( FS_FilterText( fsb), patternString) ;
                        XmTextFieldSetInsertionPosition( FS_FilterText( fsb),
                            XmTextFieldGetLastPosition( FS_FilterText( fsb))) ;
                        XtFree( patternString) ;
                      }
                  }
		XmStringFree( qualified_change_data.pattern) ;
                XmStringFree( qualified_change_data.dir) ;
                XmStringFree( qualified_change_data.mask) ;
                XmStringFree( qualified_change_data.value) ;
                XmStringFree( change_data.mask) ;
                XmStringFree( change_data.dir) ;
                XtFree( textValue) ;
                }
            else    /* wid is File List. */
            {
                if(    FS_StateFlags( fsb) & XmFS_NO_MATCH    )
                {
                    XmListDeselectPos( SB_List( fsb), 1) ;
                    break ;
                    }
                SB_ListSelectedItemPosition( fsb) = callback->item_position ;
                if ((textValue =
		     _XmStringGetTextConcat(callback->item)) != NULL)
                {
                    XmTextFieldSetString( SB_Text( fsb), textValue) ;
                    XmTextFieldSetInsertionPosition( SB_Text( fsb),
			     XmTextFieldGetLastPosition( SB_Text( fsb))) ;
                 XtFree(textValue);
                    }
                }
            break ;
            }
        case XmCR_DEFAULT_ACTION:
        {
            dbutton = (XmGadget) BB_DynamicDefaultButton( fsb) ;
            /* Catch only double-click default action here.
            *  Key press events are handled through the ParentProcess routine.
            */
            if(    (callback->event->type != KeyPress)
                && dbutton  &&  XtIsManaged((Widget)dbutton)
                && XtIsSensitive((Widget)dbutton)  &&  XmIsGadget( dbutton)
	        && (    !(FS_StateFlags(fsb) & XmFS_NO_MATCH)
		    || (wid == FS_DirList( fsb)))    )
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
            break ;
            }
        default:
        {   break ;
            }
        }
    return ;
    }

/****************************************************************
 * This routine detects differences in two versions
 *   of a widget, when a difference is found the
 *   appropriate action is taken.
 ****************/
static Boolean
SetValues(
        Widget cw,
        Widget rw,
        Widget nw,
        ArgList args_in,	/* unused */
        Cardinal *num_args )	/* unused */
{
            XmFileSelectionBoxWidget current = (XmFileSelectionBoxWidget) cw ;
            XmFileSelectionBoxWidget request = (XmFileSelectionBoxWidget) rw ;
            XmFileSelectionBoxWidget new_w = (XmFileSelectionBoxWidget) nw ;
            Arg             args[10] ;
            int             n ;
            String          newString ;
            Boolean         doSearch = FALSE ;
            XmFileSelectionBoxCallbackStruct searchData ;
/****************/

    BB_InSetValues( new_w) = TRUE ;

    if(    FS_DirListLabelString( current) != FS_DirListLabelString( new_w)    )
    {
        n = 0 ;
        XtSetArg( args[n], XmNlabelString, FS_DirListLabelString( new_w)) ; n++ ;
        XtSetArg( args[n], XmNlabelType, XmSTRING) ; n++ ;
        XtSetValues( FS_DirListLabel( new_w), args, n) ;
        FS_DirListLabelString( new_w) = NULL ;
        }
    if(    FS_FilterLabelString( current) != FS_FilterLabelString( new_w)    )
    {
        n = 0 ;
        XtSetArg( args[n], XmNlabelString, FS_FilterLabelString( new_w)) ; n++ ;
        XtSetArg( args[n], XmNlabelType, XmSTRING) ; n++ ;
        XtSetValues( FS_FilterLabel( new_w), args, n) ;
        FS_FilterLabelString( new_w) = NULL ;
        }
    n = 0 ;
    if(    SB_ListVisibleItemCount( current)
                                          != SB_ListVisibleItemCount( new_w)    )
    {   XtSetArg( args[n], XmNvisibleItemCount,
                                         SB_ListVisibleItemCount( new_w)) ; ++n ;
        }
    if(    FS_DirListItems( new_w)    )
    {
        XtSetArg( args[n], XmNitems, FS_DirListItems( new_w)) ; ++n ;
        FS_DirListItems( new_w) = NULL ;
        }
    if(    FS_DirListItemCount( new_w) != XmUNSPECIFIED_COUNT    )
    {
        XtSetArg( args[n], XmNitemCount, FS_DirListItemCount( new_w)) ; ++n ;
        FS_DirListItemCount( new_w) = XmUNSPECIFIED_COUNT ;
        }

    if(    n    )
    {   XtSetValues( FS_DirList( new_w), args, n) ;
        }

    if(    (SB_TextColumns( new_w) != SB_TextColumns( current))
        && FS_FilterText( new_w)    )
    {
        n = 0 ;
        XtSetArg( args[n], XmNcolumns, SB_TextColumns( new_w)) ; ++n ;
        XtSetValues( FS_FilterText( new_w), args, n) ;
        }
    if(    FS_NoMatchString( new_w) != FS_NoMatchString( current)    )
    {   XmStringFree( FS_NoMatchString( current)) ;
        FS_NoMatchString( new_w) = XmStringCopy( FS_NoMatchString( new_w)) ;
        }
    if(    !FS_QualifySearchDataProc( new_w)    )
    {   FS_QualifySearchDataProc( new_w) = QualifySearchDataProc ;
        }
    if(    FS_DirSearchProc( new_w)  != FS_DirSearchProc( current)
	|| FS_FileFilterStyle( new_w) != FS_FileFilterStyle( current)  )
    {  doSearch = TRUE ;
       FS_StateFlags(new_w) |= XmFS_DIR_SEARCH_PROC ;
      /* in order to track the case where the directory does not
         change but the dirsearch proc does so we have to regenerate
         the dir list from scratch */
    }
    if(    !FS_DirSearchProc( new_w)    )
    {   FS_DirSearchProc( new_w) = DirSearchProc ;
    }
    if(    !FS_FileSearchProc( new_w)    )
    {   FS_FileSearchProc( new_w) = FileSearchProc ;
        }
    /* The XmNdirSpec resource will be loaded into the Text widget by
    *   the Selection Box (superclass) SetValues routine.  It will be
    *   picked-up there by the XmNqualifySearchDataProc routine to fill
    *   in the value field of the search data.
    */
    bzero( (char*)&searchData, sizeof( XmFileSelectionBoxCallbackStruct)) ;

    if(    FS_DirMask( new_w) != FS_DirMask( current)    )
    {
        if(    FS_StateFlags( new_w) & XmFS_IN_FILE_SEARCH    )
        {
            if(    FS_FilterText( new_w)    )
            {
                newString = _XmStringGetTextConcat( FS_DirMask( new_w)) ;

                /* Should do this stuff entirely with XmStrings when the text
                *   widget supports it.
                */
                XmTextFieldSetString( FS_FilterText( new_w), newString) ;
                if(    newString    )
                {   XmTextFieldSetInsertionPosition( FS_FilterText( new_w),
			    XmTextFieldGetLastPosition( FS_FilterText( new_w))) ;
                    }
                XtFree( newString) ;
                }
            }
        else
        {   doSearch = TRUE ;
            searchData.mask = XmStringCopy( FS_DirMask( request)) ;
            searchData.mask_length = XmStringLength( searchData.mask) ;
            }
        FS_DirMask( new_w) = (XmString) XmUNSPECIFIED ;
        }
    if(    FS_Directory( current) != FS_Directory( new_w)    )
    {
        if(    FS_StateFlags( new_w) & XmFS_IN_FILE_SEARCH    )
        {
            FS_Directory( new_w) = XmStringCopy( FS_Directory( request)) ;
            XmStringFree( FS_Directory( current)) ;
            }
        else
        {   doSearch = TRUE ;
            searchData.dir = XmStringCopy( FS_Directory( request)) ;
            searchData.dir_length = XmStringLength( searchData.dir) ;

            /* The resource will be set to the new value after the Search
            *   routines have been called for validation.
            */
            FS_Directory( new_w) = FS_Directory( current) ;
            }
        }
    if(    FS_Pattern( current) != FS_Pattern( new_w)    )
    {
        if(    FS_StateFlags( new_w) & XmFS_IN_FILE_SEARCH    )
        {
            FS_Pattern( new_w) = XmStringCopy( FS_Pattern( request)) ;
            XmStringFree( FS_Pattern( current)) ;
            }
        else
        {   doSearch = TRUE ;
            searchData.pattern = XmStringCopy( FS_Pattern( request)) ;
            searchData.pattern_length = XmStringLength( searchData.pattern) ;

            /* The resource will be set to the new value after the Search
            *   routines have been called for validation.
            */
            FS_Pattern( new_w) = FS_Pattern( current) ;
            }
        }
    if(    FS_FileTypeMask( new_w) != FS_FileTypeMask( current)    )
    {
        if(    !(FS_StateFlags( new_w) & XmFS_IN_FILE_SEARCH)    )
        {   doSearch = TRUE ;
            }
        }
    if(    doSearch    )
    {
        FileSelectionBoxUpdate( new_w, &searchData) ;

        XmStringFree( searchData.value) ;
        XmStringFree( searchData.mask) ;
        XmStringFree( searchData.dir) ;
        XmStringFree( searchData.pattern) ;
        }
    BB_InSetValues( new_w) = FALSE ;

    if(    XtClass( new_w) == xmFileSelectionBoxWidgetClass    )
    {
        _XmBulletinBoardSizeUpdate( (Widget) new_w) ;

        UpdateHorizPos( (Widget) new_w) ;
        }
    return( FALSE) ;
    }

/****************************************************************/
static void
FSBGetDirSpec(
            Widget fs,
            int resource,
            XtArgVal *value)
{
    if (FS_PathMode(fs) == XmPATH_MODE_RELATIVE)
    {
	XtArgVal	filename;

        _XmSelectionBoxGetTextString(fs, resource, &filename);

        *value = (XtArgVal)XmStringConcat(FS_Directory(fs),
	        (XmString)filename);
        XmStringFree((XmString)filename);
    }
    else
    {
        _XmSelectionBoxGetTextString(fs, resource, value);
    }

    return;
}
/****************************************************************/
static void
FSBGetDirectory(
            Widget fs,
            int resource,	/* unused */
            XtArgVal *value)
/****************
 * This does get values hook magic to keep the
 * user happy.
 ****************/
{
    XmString        data ;
/****************/

    data = XmStringCopy(FS_Directory(fs));
    *value = (XtArgVal) data ;

    return ;
    }
/****************************************************************/
static void
FSBGetNoMatchString(
            Widget fs,
            int resource,	/* unused */
            XtArgVal *value)
/****************
 * This does get values hook magic to keep the
 * user happy.
 ****************/
{
    XmString        data ;
/****************/

    data = XmStringCopy(FS_NoMatchString(fs));
    *value = (XtArgVal) data ;

    return ;
    }
/****************************************************************/
static void
FSBGetPattern(
            Widget fs,
            int resource,	/* unused */
            XtArgVal *value)
/****************
 * This does get values hook magic to keep the
 * user happy.
 ****************/
{
    XmString        data ;
/****************/

    data = XmStringCopy(FS_Pattern(fs));
    *value = (XtArgVal) data ;

    return ;
    }
/****************************************************************
 * This does get values hook magic to keep the user happy.
 ****************/
static void
FSBGetFilterLabelString(
        Widget fs,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            XmString        data ;
            Arg             al[1] ;
/****************/

    XtSetArg( al[0], XmNlabelString, &data) ;
    XtGetValues( FS_FilterLabel( fs), al, 1) ;
    *value = (XtArgVal) data ;

    return ;
    }
/****************************************************************
 * This does get values hook magic to keep the user happy.
 ****************/
static void
FSBGetDirListLabelString(
        Widget fs,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            XmString        data ;
            Arg             al[1] ;
/****************/

    XtSetArg( al[0], XmNlabelString, &data) ;
    XtGetValues( FS_DirListLabel( fs), al, 1) ;
    *value = (XtArgVal) data ;

    return ;
    }
/****************************************************************
 * This does get values hook magic to keep the user happy.
 ****************/
static void
FSBGetDirListItems(
        Widget fs,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            XmStringTable   data ;
            Arg             al[1] ;
/****************/

    XtSetArg( al[0], XmNitems, &data) ;
    XtGetValues( FS_DirList( fs), al, 1) ;
    *value = (XtArgVal) data ;

    return ;
    }
/****************************************************************
 * This does get values hook magic to keep the user happy.
 ****************/
static void
FSBGetDirListItemCount(
        Widget fs,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            int             data ;
            Arg             al[1] ;
/****************/

    XtSetArg( al[0], XmNitemCount, &data) ;
    XtGetValues( FS_DirList( fs), al, 1) ;
    *value = (XtArgVal) data ;

    return ;
    }
/****************************************************************
 * This does get values hook magic to keep the user happy.
 ****************/
static void
FSBGetListItems(
        Widget fs,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            XmStringTable   data ;
            Arg             al[1] ;
/****************/

    if(    FS_StateFlags( fs) & XmFS_NO_MATCH    )
    {
        *value = (XtArgVal) NULL ;
        }
    else
    {   XtSetArg( al[0], XmNitems, &data) ;
        XtGetValues( SB_List( fs), al, 1) ;
        *value = (XtArgVal) data ;
        }
    return ;
    }
/****************************************************************
 * This does get values hook magic to keep the user happy.
 ****************/
static void
FSBGetListItemCount(
        Widget fs,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            int             data ;
            Arg             al[1] ;
/****************/

    if(    FS_StateFlags( fs) & XmFS_NO_MATCH    )
    {
        *value = (XtArgVal) 0 ;
        }
    else
    {   XtSetArg( al[0], XmNitemCount, &data) ;
        XtGetValues( SB_List( fs), al, 1) ;
        *value = (XtArgVal) data ;
        }

    return ;
    }
/****************************************************************
 * This does get values hook magic to keep the
 * user happy.
 ****************/
static void
FSBGetDirMask(
        Widget fs,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            String          filterText ;
            XmString        data ;
/****************/

    filterText = XmTextFieldGetString( FS_FilterText(fs)) ;
    data = XmStringGenerate(filterText, XmFONTLIST_DEFAULT_TAG,
			    XmCHARSET_TEXT, NULL);
    *value = (XtArgVal) data ;
    XtFree( filterText) ;

    return ;
    }

/****************************************************************/
static Widget
GetActiveText(
        XmFileSelectionBoxWidget fsb,
        XEvent *event )
{
    Widget          activeChild = NULL ;
/****************/

    if(    _XmGetFocusPolicy( (Widget) fsb) == XmEXPLICIT    )
    {
        if(    (fsb->manager.active_child == SB_Text( fsb))
            || (fsb->manager.active_child == FS_FilterText( fsb))
	   || (fsb->manager.active_child == FS_DirText( fsb))   )
        {
            activeChild = fsb->manager.active_child ;
            }
    }
    else
    {
        if(    SB_Text( fsb)
            && (XtWindow( SB_Text( fsb))
		== ((XKeyPressedEvent *) event)->window)   )
        {   activeChild = SB_Text( fsb) ;
	}
        else
        {   if(    FS_FilterText( fsb)
                && (XtWindow( FS_FilterText( fsb))
		    ==  ((XKeyPressedEvent *) event)->window)   )
            {   activeChild = FS_FilterText( fsb) ;
	    }
	else {   if(    FS_DirText( fsb)
		    && (XtWindow( FS_DirText( fsb))
			==  ((XKeyPressedEvent *) event)->window)   )
		     {   activeChild = FS_DirText( fsb) ;
		     }
	     }
        }
    }
    return( activeChild) ;
}


/****************************************************************/
static void
FileSelectionBoxUpOrDown(
        Widget wid,
        XEvent *event,
        String *argv,
        Cardinal *argc )
{
            XmFileSelectionBoxWidget fsb = (XmFileSelectionBoxWidget) wid ;
            int	            visible ;
            int	            top ;
            int	            key_pressed ;
            Widget	    list ;
            int	*           position ;
            int	            count ;
            Widget          activeChild ;
            Arg             av[5] ;
            Cardinal        ac ;
/****************/

    if (!argc || (*argc != 1) || !argv)
    {
        XmeWarning(wid, MESSAGE0);
        return;
    }

    if(    !(activeChild = GetActiveText( fsb, event))    )
    {   return ;
        }
    if(    activeChild == SB_Text( fsb)    )
    {
        if(    FS_StateFlags( fsb) & XmFS_NO_MATCH    )
        {   return ;
            }
        list = SB_List( fsb) ;
        position = &SB_ListSelectedItemPosition( fsb) ;
        }
    else /* activeChild == FS_FilterText( fsb) */
    {   list = fsb->file_selection_box.dir_list ;
        position = &FS_DirListSelectedItemPosition( fsb) ;
        }
    if(    !list    )
    {   return ;
        }
    ac = 0 ;
    XtSetArg( av[ac], XmNitemCount, &count) ; ++ac ;
    XtSetArg( av[ac], XmNtopItemPosition, &top) ; ++ac ;
    XtSetArg( av[ac], XmNvisibleItemCount, &visible) ; ++ac ;
    XtGetValues( (Widget) list, av, ac) ;

    if(    !count    )
    {   return ;
        }

    if (_XmConvertActionParamToRepTypeId((Widget) fsb,
			 XmRID_FILE_SELECTION_BOX_UP_OR_DOWN_ACTION_PARAMS,
			 argv[0], True, &key_pressed) == False)
    {
	/* We couldn't convert the value. Just assume a value of 0. */
	key_pressed = 0;
    }

    if(    *position == 0    )
    {   /*  No selection, so select first item.
        */
        XmListSelectPos( list, ++*position, True) ;
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
static void
FileSelectionBoxRestore(
        Widget wid,
        XEvent *event,
        String *argv,
        Cardinal *argc )
{
            XmFileSelectionBoxWidget fsb = (XmFileSelectionBoxWidget) wid ;
            String          itemString ;
            String          dir ;
            String          mask ;
            int             dirLen ;
            int             maskLen ;
            Widget          activeChild ;
/****************/

    if(    !(activeChild = GetActiveText( fsb, event))    )
    {   return ;
        }
    if(    activeChild == SB_Text( fsb)    )
    {   _XmSelectionBoxRestore( (Widget) fsb, event, argv, argc) ;
        }
    else
    {
        if(    FS_PathMode( fsb)  ==  XmPATH_MODE_FULL    )
          {
            if ((dir = _XmStringGetTextConcat( FS_Directory( fsb))) != NULL)
              {
                dirLen = strlen( dir) ;

                if ((mask = _XmStringGetTextConcat( FS_Pattern( fsb))) != NULL)
                  {
                    maskLen = strlen( mask) ;
                    itemString = XtMalloc( dirLen + maskLen + 1) ;
                    strcpy( itemString, dir) ;
                    strcpy( &itemString[dirLen], mask) ;
                    XmTextFieldSetString( FS_FilterText( fsb), itemString) ;
                    XmTextFieldSetInsertionPosition( FS_FilterText( fsb),
			    XmTextFieldGetLastPosition( FS_FilterText( fsb))) ;
                    XtFree( itemString) ;
                    XtFree( mask) ;
                  }
                XtFree( dir) ;
              }
          }
        else
          {
            if(    activeChild == FS_FilterText( fsb)    )
            {
                if ((mask = _XmStringGetTextConcat(FS_Pattern(fsb))) != NULL)
                {
                    XmTextFieldSetString( FS_FilterText( fsb), mask) ;
                    XmTextFieldSetInsertionPosition( FS_FilterText( fsb),
                            XmTextFieldGetLastPosition( FS_FilterText( fsb))) ;
                    XtFree( mask) ;
                    }
                }
            else /* activeChild == FS_DirText( fsb) */
            {
                if ((dir = _XmStringGetTextConcat(FS_Directory(fsb))) != NULL)
                {
                    XmTextFieldSetString( FS_DirText( fsb), dir) ;
                    XmTextFieldSetInsertionPosition( FS_DirText( fsb),
                               XmTextFieldGetLastPosition( FS_DirText( fsb))) ;
                     XtFree( dir) ;
                    }
                }
          }
        }
    return ;
    }
/****************************************************************/
static void
FileSelectionBoxFocusMoved(
        Widget wid,
        XtPointer client_data,
        XtPointer data )
{
            XmFocusMovedCallbackStruct * call_data
                                        = (XmFocusMovedCallbackStruct *) data ;
            Widget          ancestor ;
/****************/

    if(    !call_data->cont    )
    {   /* Preceding callback routine wants focus-moved processing
        *   to be discontinued.
        */
        return ;
        }

    if(    call_data->new_focus
        && (   (call_data->new_focus == FS_FilterText( client_data))
            || (call_data->new_focus == FS_DirText( client_data))
            || (call_data->new_focus == FS_DirList( client_data)))
        && XtIsManaged( SB_ApplyButton( client_data))    )
    {
        BB_DefaultButton( client_data) = SB_ApplyButton( client_data) ;
        }

 /*
  * Fix for 4110 - Check to see if the new_focus is NULL.  If it is, check
  *                to see if the default button has been set.  If not, set
  *                it to the OkButton.  Then, check if the new_focus is
  *                either the File list or the File name text field.  If
  *                they are, set the default button to the OkButton.
  *                Otherwise, leave the default button alone.
  */
     else if (!call_data->new_focus && (BB_DefaultButton(client_data)) == NULL)
      {   BB_DefaultButton( client_data) = SB_OkButton( client_data) ;
          }
     else if (call_data->new_focus
              && ((call_data->new_focus == SB_Text(client_data))
              || (call_data->new_focus == SB_List(client_data))))
     {   BB_DefaultButton( client_data) = SB_OkButton( client_data) ;
         }
 /*
  * End Fix 4110
  */
      else
    {   BB_DefaultButton( client_data) = SB_OkButton( client_data) ;
        }

    _XmBulletinBoardFocusMoved( wid, client_data, (XtPointer) call_data) ;

    /* Since the focus-moved callback of an ancestor bulletin board may
    *   have already been called, we must make sure that it knows that
    *   we have changed our default button.  So, walk the hierarchy and
    *   synchronize the dynamic default button of all ancestor bulletin
    *   board widgets.
    */
    if(    call_data->cont    )
    {
        ancestor = XtParent( (Widget) client_data) ;

        while(    ancestor  &&  !XtIsShell( ancestor)    )
        {
            if(    XmIsBulletinBoard( ancestor)    )
            {
                if(    BB_DynamicDefaultButton( ancestor)
                    && BB_DynamicDefaultButton( client_data)    )
                {
                    _XmBulletinBoardSetDynDefaultButton( ancestor,
                                       BB_DynamicDefaultButton( client_data)) ;
                    }
                }
            ancestor = XtParent( ancestor) ;
            }
        }
    return ;
    }

/****************************************************************
 * This is the procedure which does all of the button
 *   callback magic.
 ****************/
static void
FileSelectionPB(
        Widget wid,
        XtPointer which_button,
        XtPointer call_data )
{
            XmAnyCallbackStruct * callback = (XmAnyCallbackStruct *) call_data;
            XmFileSelectionBoxWidget fs ;
            XmFileSelectionBoxCallbackStruct searchData ;
            XmFileSelectionBoxCallbackStruct qualifiedSearchData ;
            Boolean         match = True ;
            String          text_value ;
            Boolean         allowUnmanage = FALSE ;
/****************/

    fs = (XmFileSelectionBoxWidget) XtParent( wid) ;

    searchData.reason = XmCR_NONE ;
    searchData.event = callback->event ;
    searchData.value = NULL ;
    searchData.length = 0 ;
    searchData.mask = NULL ;
    searchData.mask_length = 0 ;
    searchData.dir = NULL ;
    searchData.dir_length = 0 ;
    searchData.pattern = NULL ;
    searchData.pattern_length = 0 ;

    if(    ((long) which_button) == XmDIALOG_APPLY_BUTTON    )
    {
        if(    FS_FilterText( fs)
            && (text_value = XmTextFieldGetString( FS_FilterText( fs)))    )
        {
            searchData.mask =
	      XmStringGenerate(text_value, XmFONTLIST_DEFAULT_TAG,
			       XmCHARSET_TEXT, NULL) ;
            searchData.mask_length = XmStringLength( searchData.mask) ;
            XtFree( text_value) ;
            }
        if(    FS_DirText( fs)
            && (text_value = XmTextFieldGetString( FS_DirText( fs)))    )
        {
            searchData.dir =
	      XmStringGenerate(text_value, XmFONTLIST_DEFAULT_TAG,
			       XmCHARSET_TEXT, NULL) ;
            searchData.dir_length = XmStringLength( searchData.dir) ;
            XtFree( text_value) ;
            }
        searchData.reason = XmCR_NONE ;

        FileSelectionBoxUpdate( fs, &searchData) ;

        XmStringFree( searchData.mask) ;
        searchData.mask = NULL ;
        searchData.mask_length = 0 ;
        XmStringFree( searchData.dir) ;
        searchData.dir = NULL ;
        searchData.dir_length = 0 ;
        }

    /* Use the XmNqualifySearchDataProc routine to fill in all fields of the
    *   callback data record.
    */
    (*FS_QualifySearchDataProc( fs))( (Widget) fs, (XtPointer) &searchData,
                                            (XtPointer) &qualifiedSearchData) ;
    switch(    (long) which_button    )
    {
        case XmDIALOG_OK_BUTTON:
        {
            if(    SB_MustMatch( fs)    )
            {
                match = XmListItemExists( SB_List( fs),
                                                   qualifiedSearchData.value) ;
                }
            if(    !match    )
            {
                qualifiedSearchData.reason = XmCR_NO_MATCH ;
                XtCallCallbackList( ((Widget) fs),
                   fs->selection_box.no_match_callback, &qualifiedSearchData) ;
                }
            else
            {   qualifiedSearchData.reason = XmCR_OK ;
                XtCallCallbackList( ((Widget) fs),
                         fs->selection_box.ok_callback, &qualifiedSearchData) ;
                }
            allowUnmanage = TRUE ;
            break ;
            }
        case XmDIALOG_APPLY_BUTTON:
        {
            qualifiedSearchData.reason = XmCR_APPLY ;
            XtCallCallbackList( ((Widget) fs),
                      fs->selection_box.apply_callback, &qualifiedSearchData) ;
            break ;
            }
        case XmDIALOG_CANCEL_BUTTON:
        {
            qualifiedSearchData.reason = XmCR_CANCEL ;
            XtCallCallbackList( ((Widget) fs),
                     fs->selection_box.cancel_callback, &qualifiedSearchData) ;
            allowUnmanage = TRUE ;
            break ;
            }
        case XmDIALOG_HELP_BUTTON:
        {
            if(    fs->manager.help_callback    )
            {
                qualifiedSearchData.reason = XmCR_HELP ;
                XtCallCallbackList( ((Widget) fs),
                             fs->manager.help_callback, &qualifiedSearchData) ;
                }
            else
            {   _XmManagerHelp((Widget) fs, callback->event, NULL, NULL) ;
                }
            break ;
            }
        }
    XmStringFree( qualifiedSearchData.pattern) ;
    XmStringFree( qualifiedSearchData.dir) ;
    XmStringFree( qualifiedSearchData.mask) ;
    XmStringFree( qualifiedSearchData.value) ;

    if(    allowUnmanage
        && fs->bulletin_board.shell
        && fs->bulletin_board.auto_unmanage   )
    {
        XtUnmanageChild( (Widget) fs) ;
        }
    return ;
    }

/****************************************************************
 * This function returns the widget id of the
 *   specified SelectionBox child widget.
 ****************/
Widget XmFileSelectionBoxGetChild(Widget fs, unsigned char which)
{
     Widget child ;

    _XmWidgetToAppContext(fs);
    _XmAppLock(app);

    switch(    which    )
    {
        case XmDIALOG_DIR_LIST:
        {   child = FS_DirList( fs) ;
            break ;
            }
        case XmDIALOG_DIR_LIST_LABEL:
        {   child = FS_DirListLabel( fs) ;
            break ;
            }
        case XmDIALOG_FILTER_LABEL:
        {   child = FS_FilterLabel( fs) ;
            break ;
            }
        case XmDIALOG_FILTER_TEXT:
        {   child = FS_FilterText( fs) ;
            break ;
            }
        default:
            child = _XmSelectionBoxGetChild(fs, which) ;
            break ;
        }
    _XmAppUnlock(app);
    return child;
    }

/****************************************************************/
void
XmFileSelectionDoSearch(
        Widget fs,
        XmString dirmask )
{
            XmFileSelectionBoxCallbackStruct searchData ;
            String          textString ;
/****************/

    _XmWidgetToAppContext(fs);
    _XmAppLock(app);

    searchData.reason = XmCR_NONE ;
    searchData.event = 0 ;
    searchData.value = NULL ;
    searchData.length = 0 ;
    searchData.dir = NULL ;
    searchData.dir_length = 0 ;
    searchData.pattern = NULL ;
    searchData.pattern_length = 0 ;

    if(    dirmask    )
    {
        searchData.mask = XmStringCopy( dirmask) ;
        searchData.mask_length = XmStringLength( searchData.mask) ;
        }
    else
    {   if(    FS_FilterText( fs)    )
        {
            textString = XmTextFieldGetString( FS_FilterText( fs)) ;
            }
        else
        {   textString = NULL ;
            }
        if(    textString    )
        {   searchData.mask =
	      XmStringGenerate(textString, XmFONTLIST_DEFAULT_TAG,
			       XmCHARSET_TEXT, NULL) ;
            searchData.mask_length = XmStringLength( searchData.mask) ;
            XtFree( textString) ;
            }
        else
        {   searchData.mask = NULL ;
            searchData.mask_length = 0 ;
            }
        if(    FS_DirText( fs)
            && (textString = XmTextFieldGetString( FS_DirText( fs)))    )
        {
            searchData.dir =
	      XmStringGenerate(textString, XmFONTLIST_DEFAULT_TAG,
			       XmCHARSET_TEXT, NULL);
            searchData.dir_length = XmStringLength( searchData.dir) ;
            XtFree( textString) ;
            }
        }
    FileSelectionBoxUpdate( (XmFileSelectionBoxWidget) fs, &searchData) ;

    XmStringFree( searchData.mask) ;
    XmStringFree( searchData.dir) ;
    _XmAppUnlock(app);
    return ;
    }

/****************************************************************/
Widget
XmCreateFileSelectionBox(
        Widget p,
        String name,
        ArgList args,
        Cardinal n )
{
/****************/

    return( XtCreateWidget( name, xmFileSelectionBoxWidgetClass, p, args, n));
}

Widget
XmVaCreateFileSelectionBox(
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
                         xmFileSelectionBoxWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedFileSelectionBox(
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
                         xmFileSelectionBoxWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}

/****************************************************************
 * This convenience function creates a DialogShell
 *   and a FileSelectionBox child of the shell;
 *   returns the FileSelectionBox widget.
 ****************/
Widget
XmCreateFileSelectionDialog(
        Widget parent,
        char *name,
        ArgList arglist,
        Cardinal argcount )
{
   return XmeCreateClassDialog (xmFileSelectionBoxWidgetClass,
				parent, name, arglist, argcount) ;
}

static void
FSBConvert(Widget wid, XtPointer client_data, XtPointer cb_struct)
{
  enum { XmATARGETS, XmAFILE, XmAFILE_NAME, XmA_MOTIF_EXPORT_TARGETS,
	 XmA_MOTIF_CLIPBOARD_TARGETS, XmATEXT, NUM_ATOMS };
  static char *atom_names[] = {
    XmSTARGETS, XmSFILE, XmSFILE_NAME, XmS_MOTIF_EXPORT_TARGETS,
    XmS_MOTIF_CLIPBOARD_TARGETS, XmSTEXT };
  XmConvertCallbackStruct *cs = (XmConvertCallbackStruct *) cb_struct;
  Widget fsb = (Widget) client_data;
  Atom atoms[XtNumber(atom_names)];

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(XtDisplay(wid), atom_names, XtNumber(atom_names), False, atoms);

  if (FS_DirSearchProc(fsb) == DirSearchProc &&
      FS_FileSearchProc(fsb) == FileSearchProc) {
    if ((cs -> target == atoms[XmATARGETS] ||
	 cs -> target == atoms[XmA_MOTIF_EXPORT_TARGETS] ||
	 cs -> target == atoms[XmA_MOTIF_CLIPBOARD_TARGETS])) {
      Atom *targs;
      targs = (Atom *) XtMalloc(sizeof(Atom) * 2);
      targs[0] = atoms[XmAFILE];
      targs[1] = atoms[XmAFILE_NAME];
      cs -> value = (XtPointer) targs;
      cs -> length = 2;
      cs -> type = XA_ATOM;
      cs -> format = 32;
      cs -> status = XmCONVERT_MERGE;
    } else if (cs -> target == atoms[XmAFILE] ||
	       cs -> target == atoms[XmAFILE_NAME]) {
      cs -> target = atoms[XmATEXT];
      cs -> status = XmCONVERT_DEFAULT;
    }
  } else {
    cs -> status = XmCONVERT_REFUSE;
  }
}
