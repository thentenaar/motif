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

/************************************************************
*	INCLUDE FILES
*************************************************************/
#include <stdio.h>

#include "XmI.h"
#include <Xm/HierarchyP.h>
#include <Xm/PushB.h>
#include <Xm/IconButton.h>

/*
 * The bits for the default folder images.
 */

#define open_file_width 12
#define open_file_height 8
static unsigned char open_file_bits[] = {
   0x06, 0x00, 0xf9, 0x01, 0x01, 0x01, 0x01, 0x01, 0xf9, 0x0f, 0x05, 0x04,
   0x03, 0x02, 0xff, 0x01};

#define close_file_width 12
#define close_file_height 8
static unsigned char close_file_bits[] = {
   0x0e, 0x00, 0xf1, 0x01, 0xff, 0x02, 0x81, 0x02, 0x81, 0x02, 0x81, 0x02,
   0x81, 0x02, 0xff, 0x01};

/************************************************************
*	TYPEDEFS AND DEFINES
*************************************************************/

#define SUPERCLASS ((WidgetClass) &xmManagerClassRec)

#define ALLOC_INC 10

typedef enum {YES, NO, DONT_CARE} SuccessType;

/************************************************************
*	MACROS
*************************************************************/

/************************************************************
*	GLOBAL DECLARATIONS
*************************************************************/

/************************************************************
*	STATIC FUNCTION DECLARATIONS
*************************************************************/

static void ClassInit(), ClassPartInitialize(WidgetClass), InsertChild(Widget);
static void Destroy(Widget), ConstraintDestroy(Widget);
static void Realize(Widget, Mask *, XSetWindowAttributes *);
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void ConstraintInitialize(Widget, Widget, ArgList, Cardinal *);

static Boolean ConstraintSetValues(Widget, Widget, Widget, ArgList, Cardinal*);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal*);

static void ChangeNodeState(HierarchyConstraints);
static void MapNode(HierarchyConstraints), UnmapNode(HierarchyConstraints);
static void UnmapAllExtraNodes(Widget, HierarchyConstraints);
static void BuildNodeTable(Widget, HierarchyConstraints, Cardinal *);
static void ResetOpenCloseButton(Widget, HierarchyConstraints);
static void ToggleNodeState(Widget, XtPointer, XtPointer);

/******************
 * Action Routines
 ******************/

/******************
 * Type Converters
 ******************/

static Boolean CvtStringToNodeState(Display *, XrmValuePtr, Cardinal *,
				    XrmValuePtr, XrmValuePtr, XtPointer *);

/*********************
 * Internal Routines.
 *********************/

static void BumpChildren(HierarchyConstraints *, int, int);
static void RemoveNodeFromParent(HierarchyConstraints);
static void AddChild(HierarchyConstraints, HierarchyConstraints);
static void AddChildToList(HierarchyConstraints **, Cardinal *, Cardinal *,
			   HierarchyConstraints);
static void RemoveChildren(HierarchyConstraints *, Cardinal);
static void DestroyFolderImages(Widget);
static Boolean LoadFolderImages(Widget,Widget);

static HierarchyConstraints GetNodeInfo(Widget);

static Boolean AncestorClosed(HierarchyConstraints);

/************************************************************
*	STATIC DECLARATIONS
*************************************************************/

static char defaultTranslations[] =
	"<EnterWindow>:  ManagerEnter()    \n\
	 <LeaveWindow>:  ManagerLeave()    \n\
	 <FocusOut>:     ManagerFocusOut() \n\
	 <FocusIn>:      ManagerFocusIn()";

static XtResource resources[] =
{
  {
    XmNwidth, XmCWidth, XmRHorizontalDimension,
        sizeof(Dimension), XtOffsetOf(CoreRec, core.width),
        XmRImmediate, (XtPointer) 300
      },

      {
        XmNheight, XmCHeight, XmRVerticalDimension,
        sizeof(Dimension), XtOffsetOf(CoreRec, core.height),
        XmRImmediate, (XtPointer) 300
      },

      {
        XmNautoClose, XmCAutoClose, XmRBoolean,
        sizeof(Boolean), XtOffsetOf(XmHierarchyRec, hierarchy.auto_close),
        XmRImmediate, (XtPointer) True
      },

      {
        XmNrefigureMode, XmCBoolean, XmRBoolean,
        sizeof(Boolean), XtOffsetOf(XmHierarchyRec, hierarchy.refigure_mode),
        XmRImmediate, (XtPointer) True
      },

      {
        XmNnodeStateCallback, XmCNodeStateCallback, XmRCallback,
        sizeof(XtCallbackList), XtOffsetOf(XmHierarchyRec, hierarchy.node_state_callback),
        XmRPointer, (XtPointer) NULL
      },

      {
        XmNverticalMargin, XmCDimension, XmRVerticalDimension,
        sizeof(Dimension), XtOffsetOf(XmHierarchyRec, hierarchy.v_margin),
        XmRImmediate, (XtPointer) 2
      },

      {
        XmNhorizontalMargin, XmCDimension, XmRHorizontalDimension,
        sizeof(Dimension), XtOffsetOf(XmHierarchyRec, hierarchy.h_margin),
        XmRImmediate, (XtPointer) 2
      },

      {
        XmNopenFolderPixmap, XmCPixmap, XmRPrimForegroundPixmap,
        sizeof(Pixmap), XtOffsetOf(XmHierarchyRec, hierarchy.open_folder),
        XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
      },

      {
        XmNcloseFolderPixmap, XmCPixmap, XmRPrimForegroundPixmap,
        sizeof(Pixmap), XtOffsetOf(XmHierarchyRec, hierarchy.close_folder),
        XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
      },

      {
        XmNnodeStateChangedCallback, XmCNodeStateChangedCallback, XmRCallback,
        sizeof(XtCallbackList),  XtOffsetOf(XmHierarchyRec, hierarchy.node_state_changed_callback),
        XmRPointer, (XtPointer) NULL
      },

      {
        XmNnodeStateBegEndCallback, XmCCallback, XmRCallback,
        sizeof(XtCallbackList), XtOffsetOf(XmHierarchyRec, hierarchy.node_state_beg_end_callback),
        XmRPointer, (XtPointer) NULL
      },

      {
        XmNnodeStateBeginEndCallback, XmCCallback, XmRCallback,
        sizeof(XtCallbackList), XtOffsetOf(XmHierarchyRec, hierarchy.node_state_beg_end_callback),
        XmRPointer, (XtPointer) NULL
      }
    };

    static XmSyntheticResource get_resources[] =
    {
      {
        XmNhorizontalMargin, sizeof(Dimension),
        XtOffsetOf(XmHierarchyRec, hierarchy.h_margin),
        XmeFromHorizontalPixels, (XmImportProc) XmeToHorizontalPixels
      },
      {
        XmNverticalMargin, sizeof(Dimension),
        XtOffsetOf(XmHierarchyRec, hierarchy.v_margin),
        XmeFromVerticalPixels, (XmImportProc) XmeToVerticalPixels
      }
    };

    static XtResource constraints[] =
    {
      {
        XmNnodeState, XmCNodeState, XmRXmHierarchyNodeState,
        sizeof(XmHierarchyNodeState), XtOffsetOf(XmHierarchyConstraintRec, hierarchy.state),
        XmRImmediate, (XtPointer) XmOpen
      },

      {
        XmNparentNode, XmCParentNode, XmRWidget,
        sizeof(Widget), XtOffsetOf(XmHierarchyConstraintRec, hierarchy.parent),
        XmRImmediate, (XtPointer) NULL
      },

      {
        XmNinsertBefore, XmCInsertBefore, XmRWidget,
        sizeof(Widget), XtOffsetOf(XmHierarchyConstraintRec, hierarchy.insert_before),
        XmRImmediate, (XtPointer) NULL
      },

      {
        XmNnodeOpenFolderPixmap, XmCPixmap, XmRPrimForegroundPixmap,
        sizeof(Pixmap), XtOffsetOf(XmHierarchyConstraintRec, hierarchy.open_folder),
        XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
      },

      {
        XmNnodeCloseFolderPixmap, XmCPixmap, XmRPrimForegroundPixmap,
        sizeof(Pixmap), XtOffsetOf(XmHierarchyConstraintRec, hierarchy.close_folder),
        XmRImmediate, (XtPointer) XmUNSPECIFIED_PIXMAP
      }
    };

    XmHierarchyClassRec xmHierarchyClassRec = {
  { /* core fields */
    /* superclass		*/	SUPERCLASS,
    /* class_name		*/	"XmHierarchy",
    /* widget_size		*/	sizeof(XmHierarchyRec),
    /* class_initialize		*/	ClassInit,
    /* class_part_initialize	*/	ClassPartInitialize,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
    /* actions			*/	NULL,
    /* num_actions		*/	0,
    /* resources		*/	(XtResource*)resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	NULL,
    /* expose			*/	NULL,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	defaultTranslations,
    /* query_geometry		*/	NULL,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
   {		/* composite_class fields */
    /* geometry_manager   */      XtInheritGeometryManager,
    /* change_managed     */      XtInheritChangeManaged,
    /* insert_child       */      InsertChild,
    /* delete_child       */      XtInheritDeleteChild,
    /* extension          */      NULL,
   },
   {		/* constraint_class fields */
    /* resource list        */         (XtResource*)constraints,
    /* num resources        */         XtNumber(constraints),
    /* constraint size      */         sizeof(XmHierarchyConstraintRec),
    /* init proc            */         ConstraintInitialize,
    /* destroy proc         */         ConstraintDestroy,
    /* set values proc      */         ConstraintSetValues,
    /* extension            */         NULL,
   },
   {		/* manager_class fields */
    /* default translations   */      XtInheritTranslations,
    /* syn_resources          */      get_resources,
    /* num_syn_resources      */      XtNumber(get_resources),
    /* syn_cont_resources     */      NULL,
    /* num_syn_cont_resources */      0,
    /* parent_process         */      XmInheritParentProcess,
    /* extension	      */      NULL,
   },
  { /* Hierarchy fields */
      ChangeNodeState,		/* The function for changing the node state. */
      MapNode,			/* Maps a given node. */
      UnmapNode,		/* Unmaps a given node. */
      UnmapAllExtraNodes,	/* Unmaps all extra nodes. */
      BuildNodeTable,		/* Builds up the node table. */
      ResetOpenCloseButton,  /* Resets the information about the o/c button. */
      ToggleNodeState,		/* node_toggle function. */
      NULL,			/* extension          */
  }
};

WidgetClass xmHierarchyWidgetClass = (WidgetClass) &xmHierarchyClassRec;

/************************************************************
*	STATIC CODE
*************************************************************/

/*	Function Name: ClassInit
 *	Description:   Called to initialize information specific
 *                     to this widget class.
 *	Arguments:     none.
 *	Returns:       none.
 */

/*ARGSUSED*/
static void
ClassInit()
{
    XmHierarchyClassRec* wc = &xmHierarchyClassRec;

    XtSetTypeConverter(XmRString, XmRXmHierarchyNodeState,CvtStringToNodeState,
		       NULL, (Cardinal) 0, XtCacheAll, (XtDestructor) NULL);
}

/*	Function Name: ClassPartInitialize
 *	Description: handles inheritance of class functions.
 *	Arguments: class - the widget class of this widget.
 *	Returns: none.
 */

static void
ClassPartInitialize(WidgetClass class)
{
    XmHierarchyWidgetClass superC, wc = (XmHierarchyWidgetClass) class;

    _XmProcessLock();
    superC = (XmHierarchyWidgetClass)wc->core_class.superclass;

/*
 * We don't need to check for NULL super since we'll get to The functions
 * defined by the Hierarchy class eventually.
 */

    if (wc->hierarchy_class.change_node_state == XtInheritChangeNodeState)
    {
	wc->hierarchy_class.change_node_state =
	                            superC->hierarchy_class.change_node_state;
    }

    if (wc->hierarchy_class.map_node == XtInheritMapNode)
	wc->hierarchy_class.map_node = superC->hierarchy_class.map_node;

    if (wc->hierarchy_class.unmap_node == XtInheritUnmapNode)
	wc->hierarchy_class.unmap_node = superC->hierarchy_class.unmap_node;

    if (wc->hierarchy_class.unmap_all_extra_nodes==XtInheritUnmapAllExtraNodes)
    {
	wc->hierarchy_class.unmap_all_extra_nodes =
	                         superC->hierarchy_class.unmap_all_extra_nodes;
    }

    if (wc->hierarchy_class.build_node_table == XtInheritBuildNodeTable)
    {
	wc->hierarchy_class.build_node_table =
	                             superC->hierarchy_class.build_node_table;
    }

    if (wc->hierarchy_class.reset_open_close_button ==
	XtInheritResetOpenCloseButton)
    {
	wc->hierarchy_class.reset_open_close_button =
	                      superC->hierarchy_class.reset_open_close_button;
    }

    if (wc->hierarchy_class.toggle_node_state == XtInheritToggleNodeState) {

	wc->hierarchy_class.toggle_node_state =
	                            superC->hierarchy_class.toggle_node_state;
    }
    _XmProcessUnlock();
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

/*ARGSUSED*/
static void
Initialize(Widget req, Widget set, ArgList args, Cardinal * num_args)
{
    XmHierarchyWidget hw = (XmHierarchyWidget) set;
    HierarchyConstraints top_node;
    Window root = RootWindowOfScreen(XtScreen(set));

    top_node = ((HierarchyConstraints)
		XtMalloc(sizeof(HierarchyConstraintRec)));

    XmHierarchy_work_proc_id(hw) = (XtWorkProcId) NULL;
    XmHierarchy_node_table(hw) = NULL;
    XmHierarchy_num_nodes(hw) = XmHierarchy_alloc_nodes(hw) = 0;

    XmHierarchy_top_node(hw) = top_node;

    /* make default folder button images */

    XmHierarchy_def_open_folder(hw) =
      XCreateBitmapFromData(XtDisplay(set),root, (char *)open_file_bits,
			    open_file_width, open_file_height);

    XmHierarchy_def_close_folder(hw) =
      XCreateBitmapFromData(XtDisplay(set), root,(char *)close_file_bits,
			    close_file_width, close_file_height);

    XmHierarchyC_state(top_node) = XmHidden;
    XmHierarchyC_parent(top_node) = NULL;
    XmHierarchyC_widget(top_node) = NULL;
    XmHierarchyC_status(top_node) = IS_COMPRESSED;
    XmHierarchyC_num_children(top_node) = XmHierarchyC_alloc(top_node) = 0;
    XmHierarchyC_children(top_node) = NULL;

    (void)LoadFolderImages(req, set );
}

/*	Function Name: Realize
 *	Description:   Called to realize this widget.
 *	Arguments:     w - Widget to realize.
 *                     valueMask, attributes - attributes to use when creating
 *                     this widget's window.
 *	Returns:       none.
 *
 * This overrides the Manager's frobbing with various values.
 */

static void
Realize(Widget w, Mask *valueMask, XSetWindowAttributes *attributes)
{
    XtCreateWindow (w, InputOutput, CopyFromParent, *valueMask, attributes);
}

/*	Function Name: Destroy
 *	Description:   Cleans up after my widget.
 *	Arguments:     w - the widget.
 *	Returns:       none.
 */

static void
Destroy(Widget w)
{
    XmHierarchyWidget hw = (XmHierarchyWidget) w;

    if( XmHierarchy_work_proc_id(hw) != (XtWorkProcId) NULL )
    {
	XtRemoveWorkProc(XmHierarchy_work_proc_id(hw));
	XmHierarchy_work_proc_id(hw) = (XtWorkProcId) NULL;
    }

    DestroyFolderImages(w);

    XtFree((char *) XmHierarchy_node_table(hw));
    XtFree((char *) XmHierarchyC_children(XmHierarchy_top_node(hw)));
    XtFree((char *) XmHierarchy_top_node(hw));

    XtRemoveAllCallbacks(w, XmNnodeStateBegEndCallback);
}

/*	Function Name: InsertChild
 *	Description: called when a new child is to be added.
 *	Arguments: w - the new child.
 *	Returns: none.
 *
 * This routine simply makes sure that no gadgets are added.
 */

static void
InsertChild(Widget w)
{

XtWidgetProc insert_child;
   if (_XmGadgetWarning(w))
       return;

    _XmProcessLock();
    insert_child = ((CompositeWidgetClass) SUPERCLASS)->composite_class.insert_child;
    _XmProcessUnlock();

   (*insert_child)(w);
}

/************************************************************
 *
 * Functions for handling the Constraint resources.
 *
 ************************************************************/

/*	Function Name: ConstraintInitialize
 *	Description:   Called when a child's constriaint's need initializing.
 *	Arguments:     req - the widget being requested.
 *                     set - what this will become.
 *                     args, num_args - the argument list.
 *	Returns:       none.
 */

/*ARGSUSED*/
static void
ConstraintInitialize(Widget req, Widget set, ArgList args, Cardinal * num_args)
{
    XmHierarchyWidget hw = (XmHierarchyWidget) XtParent(set);
    XmHierarchyWidgetClass hc = (XmHierarchyWidgetClass) XtClass(hw);
    HierarchyConstraints node = GetNodeInfo(set);

    if (XmHierarchyC_parent(node) == set) {
	XmeWarning(set,	XmNnodeParentIsSelfMsg);
	XmHierarchyC_parent(node) = NULL;
    }

    XmHierarchyC_widget(node) = set;
    XmHierarchyC_status(node) = IS_COMPRESSED;
    XmHierarchyC_num_children(node) = XmHierarchyC_alloc(node) = 0;
    XmHierarchyC_children(node) = NULL;

    /*
     * I will be handling the mapped state of this widget
     * myself, not letting the Intrinsics get in the way.
     */

    XtSetMappedWhenManaged(set, False);

    XmHierarchyC_open_close_button(node) = NULL;

    if (XmHierarchyC_state(node) != XmNotInHierarchy) {
        XmHierarchyResetButtonProc reset_open_close_button;

        _XmProcessLock();
        reset_open_close_button = hc->hierarchy_class.reset_open_close_button;
        _XmProcessUnlock();

        (*reset_open_close_button)((Widget) hw, node);

	if (XmHierarchyC_parent(node) == NULL)
	    AddChild(XmHierarchy_top_node(hw), node);
	else
	    AddChild(GetNodeInfo(XmHierarchyC_parent(node)), node);
    }
}

/*	Function Name: ConstraintDestroy
 *	Description:   Destroys all data allocated by the constriaint
 *                     record.
 *	Arguments:     w - the widget.
 *	Returns:       none.
 */

/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * DANGER DANGER DANGER
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 * This code exercises a bug in the MIT R4 Xt Intrinsics
 * that will occur if a large number of widgets deleted from within
 * a ConstraintDestroy procedure or a Destroy Proceedure.
 *
 */

static void
ConstraintDestroy(Widget w)
{
    HierarchyConstraints node = GetNodeInfo(w);
    XmHierarchyWidget hw = (XmHierarchyWidget) XtParent(w);

    if (XmHierarchyC_state(node) == XmNotInHierarchy)
	return;

    /*
     * If the Hierarchy is being destroyed then we do not want to
     * destroy widgets that may already be gone.
     */

    if (!hw->core.being_destroyed) {
	XmHierarchyWidgetClass hc;
	Cardinal current;

	hc = (XmHierarchyWidgetClass) XtClass((Widget) hw);

	RemoveNodeFromParent(node);
	RemoveChildren(XmHierarchyC_children(node), XmHierarchyC_num_children(node));

	if (XmHierarchyC_open_close_button(node) != NULL)
	    XtDestroyWidget(XmHierarchyC_open_close_button(node));

	current = 0;
    {
        XmHierarchyBuildTableProc build_node_table;

        _XmProcessLock();
        build_node_table = hc->hierarchy_class.build_node_table;
        _XmProcessUnlock();

    	(*build_node_table) ((Widget)hw, XmHierarchy_top_node(hw), &current);
    }

	XmHierarchy_num_nodes(hw) = current;
    }

    XtFree((char *) XmHierarchyC_children(node));
}

/*	Function Name: RecursiveChildSetValues
 *	Description:   Gets the id of the old pixmap and then sets the
 *                     folder buttons
 *	Arguments:     HierarchyConstraints node - current node
 *                     Pixmap open_folder - new open folder image
 *		       Pixmap close_folder - new close folder image
 *	Returns:       none
 */
static void
SetChildValues( HierarchyConstraints node, Pixmap open_folder,
	       Pixmap close_folder )
{
  Arg args[1];
  Pixmap which = None;

  if (XmHierarchyC_state(node) == XmOpen)
  {
	if ( (XmUNSPECIFIED_PIXMAP != XmHierarchyC_open_folder(node)) && XmHierarchyC_open_folder(node))
		which = XmHierarchyC_open_folder(node);
	else
		which = open_folder;	/* can be None */
	XtSetArg(args[0], XmNpixmap, which );
	XtSetValues(XmHierarchyC_open_close_button(node), args, 1 );
  }
  else
  if (XmHierarchyC_state(node) == XmClosed)
  {
	if ( (XmUNSPECIFIED_PIXMAP != XmHierarchyC_close_folder(node)) && XmHierarchyC_close_folder(node))
		which = XmHierarchyC_close_folder(node);
	else
		which = close_folder;	/* can be None */
	XtSetArg(args[0], XmNpixmap, which );
	XtSetValues(XmHierarchyC_open_close_button(node), args, 1 );
  }
}


/*	Function Name: RecursiveChildSetValues
 *	Description:   Traverses the tree, and calls SetChildValues on each
 *                     node
 *	Arguments:     HierarchyConstraints curr_node
 *                     Pixmap open_folder
 *		       Pixmap close_folder
 *	Returns:       none
 */
static void
RecursiveSetChildValues( HierarchyConstraints curr_node, Pixmap open_folder,
			Pixmap close_folder )
{
  HierarchyConstraints *curr_child;
  Cardinal num=0, i;

  if (XmHierarchyC_widget(curr_node) != NULL)
    SetChildValues( curr_node, open_folder, close_folder );

  if (XmHierarchyC_num_children(curr_node) == 0) return;

  curr_child = XmHierarchyC_children(curr_node);
  num = XmHierarchyC_num_children(curr_node);

  for (i = 0; i < num; i++, curr_child++) {
    RecursiveSetChildValues( *curr_child, open_folder, close_folder );
  }
}



/*	Function Name: SetValues
 *	Description:   Called when XtSetValues is called
 *	Arguments:     current - the current (old) widget values.
 *                     request - before superclassed have changed things.
 *                     set - what will acutally be the new values.
 *                     args, num_args - the arguments in the list.
 *	Returns:       Boolean
 */
/*ARGSUSED*/
static Boolean
SetValues(Widget w, Widget request, Widget set, ArgList args,
	  Cardinal *num_args)
{
  XmHierarchyWidget setw = (XmHierarchyWidget)set;

  if (!LoadFolderImages( w, set )) return(False);

  RecursiveSetChildValues(XmHierarchy_top_node(setw),
			  XmHierarchy_open_folder(setw),
			  XmHierarchy_close_folder(setw));
  return(True);
}


/*	Function Name: ConstraintSetValues
 *	Description:   Called a constraint is changed on my children.
 *	Arguments:     current - the current (old) widget values.
 *                     request - before superclassed have changed things.
 *                     set - what will acutally be the new values.
 *                     args, num_args - the arguments in the list.
 *	Returns:       none
 */

/*ARGSUSED*/
static Boolean
ConstraintSetValues(Widget current, Widget request, Widget set,
		    ArgList args, Cardinal * num_args)
{
    XmHierarchyWidgetClass hc;
    XmHierarchyWidget hw = (XmHierarchyWidget) XtParent(set);
    HierarchyConstraints new_node = GetNodeInfo(set);
    HierarchyConstraints old_node = GetNodeInfo(current);
    Boolean insert_change = False;
    int i;

    for (i = 0; i < *num_args; i++)
	if (streq(args[i].name, XmNinsertBefore)) {
	    insert_change = True;
	    break;
	}

    hc = (XmHierarchyWidgetClass) XtClass(XtParent(set));

    if ((XmHierarchyC_parent(new_node) != XmHierarchyC_parent(old_node)) ||
	(insert_change))
    {
	Widget parent = XmHierarchyC_parent(new_node);

	if (XmHierarchyC_parent(new_node) == set) {
	    XmeWarning(set, XmNnodeParentIsSelfMsg);

	    XmHierarchyC_parent(new_node) = XmHierarchyC_parent(old_node);
	}
	else {
	    /*
	     * We can't just remove old_node because it is a copy of
	     * the widget and our compare to see if exists in the parent
	     * will fail.  So we store the new parent in a temp variable.
	     * reset parent to the old one, then remove it from the old parent,
	     * and finally add it to the new parent.
	     */
	    XmHierarchyC_parent(new_node) = XmHierarchyC_parent(old_node);
	    RemoveNodeFromParent(new_node);
	    XmHierarchyC_parent(new_node) = parent;
	    if (parent == NULL) {
		AddChild(XmHierarchy_top_node(hw), new_node);
	    }
	    else
		AddChild(GetNodeInfo(XmHierarchyC_parent(new_node)), new_node);
	}
    }

    if (XmHierarchyC_state(new_node) != XmHierarchyC_state(old_node)) {
	XtCallCallbackList((Widget) hw,
			   XmHierarchy_node_state_beg_end_callback(hw),
			   (XtPointer) True);

    {
        XmHierarchyNodeProc change_node_state;

        _XmProcessLock();
        change_node_state = hc->hierarchy_class.change_node_state;
        _XmProcessUnlock();

    	(*change_node_state)(new_node);
    }



	XtCallCallbackList((Widget) hw,
			   XmHierarchy_node_state_beg_end_callback(hw),
			   (XtPointer) False);
    }

    if ((XmHierarchyC_open_folder(new_node) != XmHierarchyC_open_folder(old_node))
	|| (XmHierarchyC_close_folder(new_node) != XmHierarchyC_close_folder(old_node)) )
    		SetChildValues(new_node, XmHierarchy_open_folder(hw), XmHierarchy_close_folder(hw));

    return(False);
}

/*	Function Name: Change node state
 *	Description:   Responds to changes in this node's state.
 *	Arguments:     node - the node that has changed.
 *	Returns:       none
 *
 * This function closes all ancestors
 */

/*ARGSUSED*/
static void
ChangeNodeState(HierarchyConstraints node)
{
    register int i, num;
    HierarchyConstraints * childp;
    XmHierarchyWidget hw = (XmHierarchyWidget)
	                   XtParent(XmHierarchyC_widget(node));
    XmHierarchyWidgetClass hc = (XmHierarchyWidgetClass) XtClass(hw);
    XmHierarchyNodeStateData node_data;

    XmHierarchyNodeProc change_node_state;
    XmHierarchyResetButtonProc reset_open_close_button;

    _XmProcessLock();
    change_node_state = hc->hierarchy_class.change_node_state;
    reset_open_close_button = hc->hierarchy_class.reset_open_close_button;
    _XmProcessUnlock();


    (*reset_open_close_button) (XtParent(XmHierarchyC_widget(node)), node);

    node_data.widget = XmHierarchyC_widget(node);
    node_data.state = XmHierarchyC_state(node);
    XtCallCallbackList((Widget) hw, XmHierarchy_node_state_changed_callback(hw),
		       &node_data);

    if (XmHierarchy_auto_close(hw)) {
	if ((XmHierarchyC_state(node) == XmClosed) ||
	    ((XmHierarchyC_state(node) == XmHidden) && AncestorClosed(node)))
	{
	    childp = XmHierarchyC_children(node);
	    num = XmHierarchyC_num_children(node);
	    for (i = 0; i < num; i++, childp++) {
		if (XmHierarchyC_state(*childp) == XmOpen) {
		    XmHierarchyC_state(*childp) = XmClosed;
		}
		(*change_node_state)(*childp);
	    }
	}
    }
}

/*	Function Name: AncestorClosed
 *	Description: Returns true if the first non-hidden ancestor of
 *                   this widget is closed.
 *	Arguments: node - node to check.
 *	Returns: A Boolean
 */

static Boolean
AncestorClosed(HierarchyConstraints node)
{
    while ((XmHierarchyC_parent(node) != NULL) &&
	   (XmHierarchyC_state(node) == XmHidden))
    {
	node = GetNodeInfo(XmHierarchyC_parent(node));
    }

    return(XmHierarchyC_state(node) == XmClosed);
}

/*	Function Name: UnmapAllExtraNodes
 *	Description:   Correctly unmaps each node in the hierarchy that is
 *                     currently compresed out.
 *	Arguments:     w - the ow.
 *                     node - node to work one.
 *	Returns:       none
 */

static void
UnmapAllExtraNodes(Widget w, HierarchyConstraints node)
{
    register int i;
    XmHierarchyWidgetClass hc = (XmHierarchyWidgetClass) (XtClass(w));
    register int num;
    register HierarchyConstraints * ptr;

    void (*unmap_extra)(Widget, HierarchyConstraints);
    XmHierarchyNodeProc unmap_node;

    _XmProcessLock();
    unmap_extra = hc->hierarchy_class.unmap_all_extra_nodes;
    unmap_node = hc->hierarchy_class.unmap_node;
    _XmProcessUnlock();

    if (XmHierarchyC_status(node) & IS_COMPRESSED)
	(*unmap_node)(node);

    ptr = XmHierarchyC_children(node);
    for (num = XmHierarchyC_num_children(node), i = 0; i < num; i++, ptr++)
	(*unmap_extra)(w, *ptr);
}

/*	Function Name: MapNode
 *	Description:   Maps all children of a given node.
 *	Arguments:     node - node to map.
 *	Returns:       none.
 */

static void
MapNode(HierarchyConstraints node)
{
    if (XmHierarchyC_status(node) & IS_MAPPED)
	return;

    if (XmHierarchyC_widget(node) != NULL) {
	XmHierarchyC_status(node) |= IS_MAPPED;
	XtSetMappedWhenManaged(XmHierarchyC_widget(node), True);
    }

    if (XmHierarchyC_open_close_button(node) != NULL)
	XtSetMappedWhenManaged(XmHierarchyC_open_close_button(node), True);
}

/*	Function Name: UnmapNode
 *	Description:   Unmaps all children of a given node.
 *	Arguments:     node - node to unmap.
 *	Returns:       none.
 */

static void
UnmapNode(HierarchyConstraints node)
{
    if (!(XmHierarchyC_status(node) & IS_MAPPED))
	return;

    if (XmHierarchyC_widget(node) != NULL) {
	XmHierarchyC_status(node) &= ~(IS_MAPPED); /* Unset the IS_MAPPED bit. */
	XtSetMappedWhenManaged(XmHierarchyC_widget(node), False);
    }

    if (XmHierarchyC_open_close_button(node) != NULL)
	XtSetMappedWhenManaged(XmHierarchyC_open_close_button(node), False);
}

/*	Function Name: _BuildNodeTable
 *	Description:   Recursive part of code that builds the table of
 *                     nodes to display.
 *	Arguments:     w - the hw widget.
 *                     node - node to add.
 *                     current_index - where to add this node.
 *	Returns:       none
 */

static void
_BuildNodeTable(Widget w, HierarchyConstraints node, Cardinal * current_index)
{
    XmHierarchyWidget hw = (XmHierarchyWidget) w;
    register int i;

    if ((node == NULL) || ((XmHierarchyC_widget(node) != NULL) &&
			   !XtIsManaged(XmHierarchyC_widget(node))))
    {
	return;
    }

    if (XmHierarchyC_state(node) != XmHidden) {
	XmHierarchyC_status(node) &= ~(IS_COMPRESSED); /* No longer compressed. */

	XmHierarchy_node_table(hw)[*current_index] = node;
	(*current_index)++;
    }

    if (XmHierarchyC_state(node) == XmClosed)
	return;

    for (i = 0; i < XmHierarchyC_num_children(node); i++)
	_BuildNodeTable(w, XmHierarchyC_children(node)[i], current_index);
}

/*	Function Name: BuildNodeTable
 *	Description:   builds the table of nodes to display.
 *	Arguments:     w - the hw widget.
 *                     node - node to add.
 *                     current_index - where to add this node.
 *	Returns:       none
 */

static void
BuildNodeTable(Widget w, HierarchyConstraints node, Cardinal * current_index)
{
    XmHierarchyWidget hw = (XmHierarchyWidget) w;

    if ((XmHierarchy_node_table(hw) == NULL) ||
	(XmHierarchy_alloc_nodes(hw) < hw->composite.num_children))
    {
	/*
	 * This will allocate more space than we need, but shouldn't be too
	 * terrible.
	 */

	XmHierarchy_node_table(hw) = (HierarchyConstraints *)
	    XtRealloc((XtPointer) XmHierarchy_node_table(hw),
		    sizeof(HierarchyConstraints) * hw->composite.num_children);

	XmHierarchy_alloc_nodes(hw) = hw->composite.num_children;
    }

    _BuildNodeTable(w, node, current_index);
}

/*	Function Name: ResetOpenCloseButton
 *	Description:   Creates or Destroys the Open/Close button
 *                     as appropriate.
 *	Arguments:     node - the node to check.
 *	Returns:       none.
 */

static void
ResetOpenCloseButton(Widget w, HierarchyConstraints node)
{
    Arg args[5];
    Pixmap image;
    Cardinal num_args;
    XmHierarchyWidget hw = (XmHierarchyWidget) w;
    XmHierarchyWidgetClass hc = (XmHierarchyWidgetClass) XtClass(w);

    if (XmHierarchyC_state(node) == XmAlwaysOpen) {
	if (XmHierarchyC_open_close_button(node) != NULL) {
	    XtDestroyWidget(XmHierarchyC_open_close_button(node));
	    XmHierarchyC_open_close_button(node) = NULL;
	}
	return;
    }

    switch(XmHierarchyC_state(node)) {
    case XmOpen:
    case XmHidden:
    default:
	if ( (XmUNSPECIFIED_PIXMAP != XmHierarchyC_open_folder(node)) && XmHierarchyC_open_folder(node))
		image = XmHierarchyC_open_folder(node);
	else
		image = XmHierarchy_open_folder(hw);
	break;
    case XmClosed:
	if ( (XmUNSPECIFIED_PIXMAP != XmHierarchyC_close_folder(node)) && XmHierarchyC_close_folder(node))
		image = XmHierarchyC_close_folder(node);
	else
		image = XmHierarchy_close_folder(hw);
	break;
    }

    num_args = 0;
    XtSetArg(args[num_args], XmNpixmap, image); num_args++;

    if (XmHierarchyC_open_close_button(node) == NULL) {
	XtSetArg(args[num_args], XmNmappedWhenManaged, False); num_args++;

	/*
	 * Special node state for items that should not be
	 * considered part of the hierarchy, but are children of this
	 * widget.
	 */

        XtSetArg(args[num_args], XmNnodeState, XmNotInHierarchy); num_args++;
	XtSetArg(args[num_args], XmNiconPlacement, XmIconOnly); num_args++;

	XmHierarchyC_open_close_button(node) =
	    XtCreateManagedWidget("openAndClose", xmIconButtonWidgetClass,
				  w, args, num_args);

	/*
	 * Make sure the mapped state of open/close button matches the
	 * node button that it corrosponds to.
	 */

	XtSetMappedWhenManaged(XmHierarchyC_open_close_button(node),
		((XmHierarchyC_status(node) & IS_MAPPED) ? True: False));

    _XmProcessLock();
	XtAddCallback(XmHierarchyC_open_close_button(node),
		      XmNactivateCallback,
		      hc->hierarchy_class.toggle_node_state, (XtPointer) node);
    _XmProcessUnlock();
    }
    else {
	XtSetValues(XmHierarchyC_open_close_button(node), args, num_args);
    }
}

/*	Function Name: ToggleNodeState
 *	Description:   Toggles the open/close state of a toggle button.
 *	Arguments:     w - the command button that activated this.
 *                     node_ptr - a pointer to the node info.
 *                     call_data - UNUSED.
 *	Returns:       none.
 */

static void
ToggleNodeState ( Widget w,
				  XtPointer node_ptr,
				  XtPointer call_data )
{
	Arg args[1];
	XmHierarchyWidget hw = ( XmHierarchyWidget ) XtParent ( w );
	HierarchyConstraints node = ( HierarchyConstraints ) node_ptr;

	XmHierarchyWidgetClass hc = (XmHierarchyWidgetClass) XtClass ((Widget) hw );
	XmHierarchyNodeStateData node_data;

    XmHierarchyNodeProc change_node_state;

    _XmProcessLock();
    change_node_state = hc->hierarchy_class.change_node_state;
    _XmProcessUnlock();
	/*
	 * Unset the icon button.
	 */

	XtSetArg ( args[0], XmNset, False );
	XtSetValues ( w, args, ( Cardinal ) 1 );

	if (XmHierarchyC_state(node) == XmOpen )
	{
		XtCallCallbackList( (Widget) hw,
		    XmHierarchy_node_state_beg_end_callback ( hw ),
			(XtPointer) True );

		XmHierarchyC_state ( node ) = XmClosed;

		(*change_node_state) ( (HierarchyConstraints) node );

	}
	else
    if ( XmHierarchyC_state ( node ) == XmClosed )
	{
		XtCallCallbackList ( ( Widget ) hw,
			XmHierarchy_node_state_beg_end_callback ( hw ),
			( XtPointer ) True );

		XmHierarchyC_state ( node ) = XmOpen;

		(*change_node_state) ( (HierarchyConstraints) node );

	}
	else
		return;

	node_data.widget = XmHierarchyC_widget ( node );
	node_data.state = XmHierarchyC_state ( node );

	XtCallCallbackList ( ( Widget ) hw,
        XmHierarchy_node_state_callback ( hw ),
		&node_data );

	XtCallCallbackList ( ( Widget ) hw,
        XmHierarchy_node_state_beg_end_callback ( hw ),
		( XtPointer ) False );
}



/************************************************************
 *
 * Type Converters.
 *
 ************************************************************/

/*	Function Name: CvtStringToNodeState
 *	Description:   Converts a string to a NodeState.
 *	Arguments:     dpy - the X Display.
 *                     args, num_args - *** NOT USED ***
 *                     fromVal - contains the string to convert.
 *                     toVal - contains the converted node state.
 *                     junk - *** UNUSED ***.
 *	Returns:
 */

/*ARGSUSED*/
static Boolean
CvtStringToNodeState(Display * dpy, XrmValuePtr args, Cardinal *num_args,
		     XrmValuePtr fromVal, XrmValuePtr toVal, XtPointer *junk)
{
    static XmHierarchyNodeState type;
    static XrmQuark XtQEAlwaysOpen, XtQEOpen, XtQEClosed, XtQEHidden;
    static XrmQuark XtQENotInHierarchy;
    static Boolean haveQuarks = FALSE;
    XrmQuark q;
    char lowerName[BUFSIZ];

    if (!haveQuarks) {
	XtQEAlwaysOpen = XrmStringToQuark("alwaysopen");
	XtQEOpen = XrmStringToQuark("open");
	XtQEClosed = XrmStringToQuark("closed");
	XtQEHidden = XrmStringToQuark("hidden");
	XtQENotInHierarchy = XrmStringToQuark("notinhierarchy");
	haveQuarks = TRUE;
    }

    XmCopyISOLatin1Lowered(lowerName, (char *) fromVal->addr);
    q = XrmStringToQuark(lowerName);

    if (q == XtQEAlwaysOpen)
	type = XmAlwaysOpen;
    else if (q == XtQEOpen)
	type = XmOpen;
    else if (q == XtQEClosed)
	type = XmClosed;
    else if (q == XtQEHidden)
	type = XmHidden;
    else if (q == XtQENotInHierarchy) 	/* note! shouldn't ever be needed */
	type = XmNotInHierarchy;
    else {
	XtDisplayStringConversionWarning(dpy, fromVal->addr,
					 XmRXmHierarchyNodeState);
	return(False);		/* Conversion failed. */
    }

    toVal->size = sizeof(XmHierarchyNodeState);

    if (toVal->addr == NULL) {
	toVal->addr = (XtPointer) &type;
	return(TRUE);
    }

    if (toVal->size >= sizeof(XmHierarchyNodeState)) {
	*((XmHierarchyNodeState *) toVal->addr) = type;
	return(TRUE);
    }

    return(FALSE);
}

/************************************************************
 *
 * Actions and Callbacks.
 *
 ************************************************************/

/************************************************************
 *
 * Internal routines.
 *
 ************************************************************/

/*	Function Name: LoadFolderImages
 *	Description: Loads the folder images.
 *	Arguments: w - the hierarchy widget.
 *	Returns: none.
 *
 *      If the new image resource value is XmUNSPECIFIED_PIXMAP,
 *      the image is given the default value. If the image has
 *      changed return True, otherwise return False
 *
 * It would be nice if these values were cached per screen or display.
 */

static Boolean
LoadFolderImages(Widget old, Widget set)
{
  XmHierarchyWidget setw = (XmHierarchyWidget) set;
  XmHierarchyWidget oldw = (XmHierarchyWidget) old;

  if (XmHierarchy_open_folder(setw) == XmUNSPECIFIED_PIXMAP){
    XmHierarchy_open_folder(setw) = XmHierarchy_def_open_folder(setw);
  }
  if (XmHierarchy_close_folder(setw) == XmUNSPECIFIED_PIXMAP){
    XmHierarchy_close_folder(setw) = XmHierarchy_def_close_folder(setw);
  }

  if (XmHierarchy_open_folder(oldw) != XmHierarchy_open_folder(setw))
    return(True);
  if (XmHierarchy_close_folder(oldw) != XmHierarchy_close_folder(setw))
    return(True);

  return(False);
}

/*	Function Name: DestroyFolderImages
 *	Description: Destroys the internal default folder images.
 *	Arguments: w - the hierarchy widget.
 *	Returns: none.
 */

static void
DestroyFolderImages(Widget w)
{
  XmHierarchyWidget hw = (XmHierarchyWidget) w;

  XFreePixmap(XtDisplay(w), XmHierarchy_def_open_folder(hw));
  XFreePixmap(XtDisplay(w), XmHierarchy_def_close_folder(hw));
}

/*	Function Name: RemoveChildren
 *	Description:   removes a list of children from this node.
 *	Arguments:     list - list of children to remove.
 *                     num - number of children in the list.
 *	Returns:       none.
 */

static void
RemoveChildren(HierarchyConstraints * list, Cardinal num)
{
    register int i;

    for (i = 0 ; i < num ; i++, list++ ) {
	/*
	 * Our parent is already gone.
	 */

	XmHierarchyC_status(*list) |= PARENT_GONE;
	XtDestroyWidget(XmHierarchyC_widget(*list));
    }
}

/*	Function Name: AddChild
 *	Description:   Adds the child specified to the parent.
 *	Arguments:     parent - parent to add this child to.
 *                     child  - child to add to this parent.
 *	Returns:       none.
 */

static void
AddChild(HierarchyConstraints parent, HierarchyConstraints child)
{
    if (parent == NULL)
	return;

    AddChildToList(&(XmHierarchyC_children(parent)),
		   &(XmHierarchyC_num_children(parent)),
		   &(XmHierarchyC_alloc(parent)), child);
}

/*	Function Name: AddChildToList
 *	Description: Adds a child the the specified list.
 *	Arguments:
 * IN/OUT         list - pointer to the list of children to add child to.
 * IN/OUT         num - pointer to the number of children.
 * IN/OUT         alloc - the amount of space allocated for children.
 *                child - child to insert.
 *	Returns: none.
 */

static void
AddChildToList(HierarchyConstraints ** list, Cardinal * num, Cardinal * alloc,
	       HierarchyConstraints child)
{
    register int i, j;
    HierarchyConstraints *l_child;
    SuccessType success = DONT_CARE;
    Widget insert_before = XmHierarchyC_insert_before(child);

    if (*alloc <= *num) {
	Cardinal size;

	(*alloc) += ALLOC_INC;
	size = sizeof(HierarchyConstraints) * (*alloc);
	*list = (HierarchyConstraints *) XtRealloc((XtPointer)*list, size);
    }

    if (insert_before != NULL) {
	success = NO;

	/*
	 * Hunt for the sibling that matches the insert_before widget.
	 */

	for (l_child = (*list) + (i = (*num - 1)); i >= 0; i--, l_child--)
	    if (XmHierarchyC_widget(*l_child) == insert_before) {
		/*
		 * Bump each child down and then insert the new child.
		 */
		for (j = (*num - 1); j >= i; j--)
		    (*list)[j + 1] = (*list)[j];
		(*list)[i] = child;

		success = YES;
		break;
	    }
    }

    if (success == NO) {
	String params[1];
	params[0] = XtName(XmHierarchyC_widget(child));
	_XmWarningMsg(XmHierarchyC_widget(child), XmNinsertBeforeNotSibling,
		XmNinsertBeforeNotSiblingMsg, params, 1);
    }

    if (success != YES)		/* Stick it on the end of the list. */
	(*list)[*num] = child;

    (*num)++; /* It always goes somewhere. */
}

/*	Function Name: GetNodeInfo
 *	Description:   Gets the node info associated with this widget.
 *	Arguments:     w - the widget.
 *	Returns:       the node info about this widget.
 */

static HierarchyConstraints
GetNodeInfo(Widget w)
{
    return((HierarchyConstraints) ((HierarchyConstraints)w->core.constraints));
}

/*	Function Name: RemoveNodeFromParent
 *	Description:   Removes the node from its parents children list.
 *	Arguments:     node  - the node to remove.
 *	Returns:       none
 */

static void
RemoveNodeFromParent(HierarchyConstraints node)
{
    register int i;
    HierarchyConstraints pnode;

    if (XmHierarchyC_status(node) & PARENT_GONE)
	return;

    if (XmHierarchyC_parent(node) == NULL) {
	XmHierarchyWidget hw;

	hw = (XmHierarchyWidget) XtParent(XmHierarchyC_widget(node));
	pnode = XmHierarchy_top_node(hw);
    }
    else
	pnode = GetNodeInfo(XmHierarchyC_parent(node));

    for (i = 0; i < XmHierarchyC_num_children(pnode); i++) {
	if (XmHierarchyC_children(pnode)[i] == node) {
	    BumpChildren(XmHierarchyC_children(pnode), i,
			 (int) XmHierarchyC_num_children(pnode));
	    XmHierarchyC_num_children(pnode)--;
	    return;
	}
    }
}

/*	Function Name: BumpChildren
 *	Description:   Bumps the children after this node up one.
 *	Arguments:     nlist - the node list
 *                     i - where to begin bumping.
 *                     num - number of children in list.
 *	Returns:       none
 */

static void
BumpChildren(HierarchyConstraints * nlist, int i, int num)
{
    for (i++ ; i < num ; i++ )
	nlist[i - 1] = nlist[i];
}

/************************************************************
 *
 * Public Functions
 *
 ************************************************************/

/*	Function Name: XmHierarchyOpenAllAncestors
 *	Description: This function opens all ancestors of the given node.
 *	Arguments: nw - the node (widget) that will be changed.
 *	Returns: none
 */

void
XmHierarchyOpenAllAncestors(Widget nw)
{
    Widget parent;
    HierarchyConstraints node;
    static Arg args[] = {
	{ XmNnodeState, (XtArgVal) XmOpen }
    };

    _XmWidgetToAppContext(nw);
    _XmAppLock(app);

    if (!XtParent(nw) || !XtIsSubclass(XtParent(nw), xmHierarchyWidgetClass))
      {
	_XmAppUnlock(app);
	return;
      }

    node = GetNodeInfo(nw);

    while ((parent = XmHierarchyC_parent(node)) != NULL) {
	node = GetNodeInfo(parent);

	if (XmHierarchyC_state(node) == XmClosed)
	    XtSetValues(XmHierarchyC_widget(node), args, XtNumber(args));
    }

    _XmAppUnlock(app);
}

/*	Function Name: XmHierarchyGetChildNodes
 *	Description: This function returns the node children of a node
 *	Arguments: nw - the node (widget) that will be examined
 *	Returns: a list of Widgets (must be XtFree'd) or NULL
 */
WidgetList XmHierarchyGetChildNodes(Widget nw)
{
    HierarchyConstraints node;
    WidgetList retval = (WidgetList)NULL;
    int i;

    _XmWidgetToAppContext(nw);
    _XmAppLock(app);

    if (!XtParent(nw) || !XtIsSubclass(XtParent(nw), xmHierarchyWidgetClass))
      {
	_XmAppUnlock(app);
	return retval;
      }

    node = GetNodeInfo(nw);

    if (0 == XmHierarchyC_num_children(node))
      {
	_XmAppUnlock(app);
	return retval;
      }

    retval = (WidgetList)XtMalloc((XmHierarchyC_num_children(node) + 1) * sizeof(Widget));

    for (i=0; i < XmHierarchyC_num_children(node); i++)
	retval[i] = XmHierarchyC_widget(XmHierarchyC_children(node)[i]);
    retval[i] = (Widget)NULL;

    _XmAppUnlock(app);
    return retval;
}

