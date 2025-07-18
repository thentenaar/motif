/* $XConsortium: sampler2_0.c /main/13 1995/07/15 20:46:01 drk $ */
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
/*
 * HISTORY
 */

#include <stdlib.h>
#include <Xm/XmAll.h>
#include <Xmd/AdjView.h>
#include <Xmd/Help.h>
/* need these semi public header during the construction of the
   ContainerOutline class widget tree, as data, not as real widgets */
#include <Xm/SashP.h>
#include <Xm/TearOffBP.h>
#include <Xm/GrabShell.h>
#include <Xm/IconH.h>
#ifdef SAMPLER_DEBUG_SSPINB
#include <Xm/SpinBP.h>
#include <Xm/SSpinBP.h>
#endif

/* The notebook maintains its own data thru XmNuserData, which
   points to a notebook data struct */
typedef struct
{
    String    page_name;
    WidgetClass page_class ;
    XtCallbackProc page_init_proc ;
} PageDataStruct;

typedef struct
{
    Widget  status;
    Cardinal num_pages ;
    PageDataStruct * pages ;
} NotebookDataStruct;


/**********************************************************************
  the following is used in the ContainerOutline page, which displays
   a tree of all the wigdet classes in Xt/Motif */

typedef struct _ClassInfoRec {
    Boolean documented ;  /* whether or not the class is documented */
    WidgetClass w_class ;
} ClassInfoRec;

typedef struct _IconGInfoRec {
    Cardinal parent_index ;  /* entryParent resource */
    ClassInfoRec class_info ;
} IconGInfoRec, * IconGInfo;


/*********** Following used to build the icong_array[] with the
             widget class tree information *****************/

/* this one is not exported by Xt, mimic it static here */
static RectObjClassRec unNamedObjClassRec = {
  {
    /* superclass	  */	(WidgetClass)&rectObjClassRec,
    /* class_name	  */	"UnNamedObj",
    /* widget_size	  */	0
  }
};
static WidgetClass unNamedObjClass = (WidgetClass) &unNamedObjClassRec;

#define ClassName(wclass) ((WidgetClass)wclass)->core_class.class_name


#define MAX_CLASSES 100
static IconGInfo icong_array[MAX_CLASSES] ;

/*** my own little stack manager: push and pop */
#define MAX_DEPTH 10
static Cardinal stack_index[MAX_DEPTH] ;
static Cardinal stack_heap = 0 ;
#define PushIndex(i) (stack_index[stack_heap++] = i)
#define PopIndex()   (stack_index[--stack_heap])


#define SSB_NUMERIC	0
#define SSB_STRING	1
#define NUM_SSB_WIDGETS	2
static Widget	ssb_widgets[NUM_SSB_WIDGETS];




/** each page has an Init routine which is called the first
    time a page is raised. It delays the real page creation somehow */
static PageDataStruct * NewPageSet (Cardinal *num_page_return);
static void NbCB (Widget w, XtPointer client_data, XtPointer call_data);
static void DestroyCB (Widget w, XtPointer client_data, XtPointer call_data);
static void QuitCB (Widget w, XtPointer client_data, XtPointer call_data);
static void HelpCB (Widget w, XtPointer client_data, XtPointer call_data);
static Widget CreateMainWindowWithMenuBar (Widget toplevel);
static void ContainerExpose (Widget wid,   XtPointer closure,
			     XEvent *event, Boolean *cont);
static void InitContainerSpatial(Widget w, XtPointer client_data,
				 XtPointer call_data);
static void TravCB (Widget w, XtPointer client_data, XtPointer call_data);
static void InitContainerOutline(Widget w, XtPointer client_data,
				 XtPointer call_data);
static void OKCB (Widget w, XtPointer client_data, XtPointer call_data);
static void InitPanedWindow(Widget w, XtPointer client_data,
			    XtPointer call_data);
static void InitFileSelectionBox(Widget w, XtPointer client_data,
				 XtPointer call_data);
static void InitTogglesWindow(Widget w, XtPointer client_data,
			      XtPointer call_data);
static void InitScaleCombo(Widget w, XtPointer client_data,
			   XtPointer call_data);
static void SetDyn(WidgetClass wclass, Cardinal index, Boolean documented);
static Cardinal InitClassArray (void);
static void sb_ModifyVerifyCB(Widget w, XtPointer client_data,
			      XtPointer call_data);
static void sb_ValueChangedCB(Widget w, XtPointer client_data,
			      XtPointer call_data);
static void tf_ActivateCB(Widget w, XtPointer client_data,
			    XtPointer call_data);
static void sb_ApplyCB(Widget w, XtPointer client_data,
			    XtPointer call_data);
static void sb_CancelCB(Widget w, XtPointer client_data,
			    XtPointer call_data);
static void sb_OkCB(Widget w, XtPointer client_data,
			    XtPointer call_data);
static Widget ssb_GetTextField(Widget w);


#define APP_CLASS "XmdSampler2_0"


static Widget toplevel ;  /* used in OKCB for set its size */

/*
 * Fallbacks: keep it minimum
 */
static String fallbacks[] = {
"*help_manager.helpFile: sampler2_0",
"*XmIconGadget.outlineState: XmEXPANDED",
"*SWContainerOutline.scrollingPolicy:XmAUTOMATIC",
"*ContainerOutline.layoutType:outline",
NULL
};

/***********************************************/
/***********************************************/
int main(int argc, char *argv[])
{
    XtAppContext app_context;
    int i ;
    Widget mainw, nb ;
    NotebookDataStruct nb_data ;
    XmNotebookCallbackStruct nb_call_data ;

    XtSetLanguageProc(NULL, NULL, NULL);
    toplevel = XtVaAppInitialize(&app_context, APP_CLASS, NULL, 0,
			       &argc, argv, fallbacks, NULL);

    /*** create utility widgets */
    mainw = CreateMainWindowWithMenuBar(toplevel);
    XtManageChild(mainw);

    /* allocate new pages struct */
    nb_data.pages = NewPageSet (&nb_data.num_pages) ;

    nb = XtVaCreateManagedWidget("nb", xmNotebookWidgetClass, mainw,
				 XmNuserData, &nb_data, NULL);
    XtAddCallback (nb, XmNpageChangedCallback, NbCB, NULL);
    XtAddCallback (nb, XmNdestroyCallback, DestroyCB, NULL);

    /*** create page and tab buttons, one per page */
    for (i=0; i< nb_data.num_pages; i++) {
	char tab_name[100] ;

	sprintf(tab_name, "TabTo%s", nb_data.pages[i].page_name);
	(void) XtVaCreateManagedWidget(nb_data.pages[i].page_name,
				       nb_data.pages[i].page_class,
				       nb,
				       NULL);
	(void) XtVaCreateManagedWidget(tab_name, xmPushButtonWidgetClass,
				      nb, NULL);
    }


    /*** create only one status area */
    nb_data.status = XtVaCreateManagedWidget("Status", xmLabelWidgetClass, nb,
					    XmNpageNumber, 1, NULL);

    XtRealizeWidget(toplevel);
    XtAppMainLoop(app_context);

    return (0);
}

/* I wish C could initialized that statically, but it can't */
static
PageDataStruct * NewPageSet (Cardinal *num_page_return)

{
  Cardinal n;
  PageDataStruct * pages;
#define NUM_PAGES 7

  pages = (PageDataStruct *) XtMalloc(NUM_PAGES * sizeof(PageDataStruct));

  n = 0;
  pages[n].page_name = "ContainerSpatial";
  pages[n].page_init_proc = InitContainerSpatial;
  pages[n].page_class = xmContainerWidgetClass; n++;
  pages[n].page_name = "SWContainerOutline";
  pages[n].page_init_proc = InitContainerOutline;
  pages[n].page_class = xmScrolledWindowWidgetClass; n++;
  pages[n].page_name = "PanedWindow";
  pages[n].page_init_proc = InitPanedWindow;
  pages[n].page_class = xmPanedWindowWidgetClass; n++;
  pages[n].page_name = "FileSelectionBox";
  pages[n].page_init_proc = InitFileSelectionBox;
  pages[n].page_class = xmFrameWidgetClass; n++;
  pages[n].page_name = "TogglesWindow";
  pages[n].page_init_proc = InitTogglesWindow;
  pages[n].page_class = xmScrolledWindowWidgetClass; n++;
  pages[n].page_name = "ScaleCombo";
  pages[n].page_init_proc = InitScaleCombo;
  pages[n].page_class = xmFormWidgetClass; n++;

  *num_page_return = n ;
  return pages ;
}

/**********************************************
 *   NbCB		- callback for Notebook
 *     Update the status label. and init the page if not yet done
 */
static void
NbCB (Widget w, XtPointer client_data, XtPointer call_data)
{
   XmNotebookCallbackStruct * nbcs =
	(XmNotebookCallbackStruct *) call_data ;
   NotebookDataStruct * nb_data ;

   /* get the data for this notebook from the notebook */
   XtVaGetValues(w, XmNuserData, &nb_data, NULL);

   /* set the status label to the widget I'm showing now, plus
      its page number */
   XtVaSetValues(nb_data->status,
		 XtVaTypedArg, XmNlabelString, XtRString,
		 nb_data->pages[nbcs->page_number - 1].page_name,
		 strlen(nb_data->pages[nbcs->page_number - 1].page_name)+1,
		 XmNpageNumber, nbcs->page_number, NULL);

   /* here I use page_class as a flag to check if the page
      has been initialized. page_class is the field that stores the
      class of the widget in the page, it is not NULL the first time
      thru and I set it NULL here cause I won't be using it again */
   if (nb_data->pages[nbcs->page_number - 1].page_class) {
       (* nb_data->pages[nbcs->page_number - 1].page_init_proc)
	   (nbcs->page_widget, NULL, NULL) ;
       nb_data->pages[nbcs->page_number - 1].page_class = NULL ;
   }

}

/**********************************************
 *   DestroyCB		- callback when Notebook dies
 *     Free the data associated with this Notebook.
 */
static void
DestroyCB (Widget w, XtPointer client_data, XtPointer call_data)
{
   NotebookDataStruct * nb_data ;

   /* get the data for this notebook from the notebook */
   XtVaGetValues(w, XmNuserData, &nb_data, NULL);

   XtFree((char*) nb_data->pages);

   /* nbdata itself comes from the stack in this program */
}

static void
QuitCB (Widget w, XtPointer client_data, XtPointer call_data)
{
    exit (0);
}

static void
HelpCB (Widget w, XtPointer client_data, XtPointer call_data)
{
    static Widget help_widget = NULL ;

    if (!help_widget)
	help_widget = XmdCreateHelpDialog(toplevel, "help_manager", NULL, 0);

    XtManageChild(help_widget);
}


static Widget
CreateMainWindowWithMenuBar(Widget top)
{
    Widget	main_window, menu_bar, menu_pane, cascade, button ;

    Arg		args[20];
    Cardinal	n;


    /*	Create MainWindow.
     */
    n = 0;
    main_window = XmCreateMainWindow (top, "main_window", args, n);
    XtManageChild (main_window);


    /*	Create MenuBar in MainWindow.
     */
    n = 0;
    menu_bar = XmCreateMenuBar (main_window, "menu_bar", args, n);
    XtManageChild (menu_bar);

    /*	Create "File" PulldownMenu.
     */
    n = 0;
    menu_pane = XmCreatePulldownMenu (menu_bar, "menu_pane", args, n);

    n = 0;
    button = XmCreatePushButton (menu_pane, "Exit", args, n);
    XtManageChild (button);
    XtAddCallback (button, XmNactivateCallback, QuitCB, NULL);

    n = 0;
    XtSetArg (args[n], XmNsubMenuId, menu_pane);  n++;
    cascade = XmCreateCascadeButton (menu_bar, "File", args, n);
    XtManageChild (cascade);


    /*	Create "Help" PulldownMenu.
     */
    n = 0;
    menu_pane = XmCreatePulldownMenu (menu_bar, "menu_pane3", args, n);

    n = 0;
    button = XmCreatePushButton (menu_pane, "Overview", args, n);
    XtManageChild (button);
    XtAddCallback (button, XmNactivateCallback, HelpCB, NULL);

    n = 0;
    XtSetArg (args[n], XmNsubMenuId, menu_pane);  n++;
    cascade = XmCreateCascadeButton (menu_bar, "Help", args, n);
    XtManageChild (cascade);
    n = 0;
    XtSetArg (args[n], XmNmenuHelpWidget, cascade);  n++;
    XtSetValues (menu_bar, args, n);

    return main_window ;
}

static void
ContainerExpose(
        Widget wid,
        XtPointer closure,	/* unused */
        XEvent *event,
        Boolean *cont )		/* unused */
{
    /* Turn the spatial container to NONE after it is realized */
    XtVaSetValues (wid, XmNspatialStyle, XmNONE, NULL);
}



static void
InitContainerSpatial (Widget w, XtPointer client_data, XtPointer call_data)
{
    Cardinal i ;
#define NUM_ICONS 10
    Widget icons[NUM_ICONS ] ;

    for (i=0; i<NUM_ICONS ; i++) {
	char icon_name[20] ;

	sprintf(icon_name, "IconGadget%d", i);
	icons[i] = XtVaCreateWidget(icon_name, xmIconGadgetClass,
				    w, NULL);
    }
    XtManageChildren(icons, i);

    /* add a handler to turn teh Container to NONE once it is all set */
    XtAddEventHandler (w, ExposureMask, False, ContainerExpose, NULL);
}


static void
TravCB (Widget w, XtPointer client_data, XtPointer call_data)
{
    XmTraverseObscuredCallbackStruct * tocs =
	(XmTraverseObscuredCallbackStruct *) call_data ;

    XmScrollVisible(w, tocs->traversal_destination, 20, 20) ;
}

static void
InitContainerOutline (Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget container, *icongs;
    Cardinal i ;

    /* get the number of classes in Xt/Motif */
    i = InitClassArray() ;

    /* allocate space for the array of icongadgets to be managed
       at once */
    icongs = (Widget*) XtMalloc(i * sizeof(Widget));

    XtAddCallback (w, XmNtraverseObscuredCallback, TravCB, NULL);

    /* create the container */
    container = XtVaCreateManagedWidget("ContainerOutline",
					xmContainerWidgetClass, w,
					NULL);

    /* setup our clipwindow/workwindow constraint stuff */
    XmdSWAdjustView (w) ;

    /* create the root */
    icongs[0] = XtVaCreateWidget(
		                 ClassName(icong_array[0]->class_info.w_class),
				 xmIconGadgetClass, container,
				 XmNentryParent, NULL,
				 XmNsensitive,
				 icong_array[0]->class_info.documented,
				 XmNuserData,
				 &(icong_array[0]->class_info),
				 NULL);
    i = 1 ;
    while (icong_array[i]) {
	/* parent_index has been set during InitClassArray */
	icongs[i] = XtVaCreateWidget(
				ClassName(icong_array[i]->class_info.w_class),
				   xmIconGadgetClass, container,
				   XmNentryParent,
				      icongs[icong_array[i]->parent_index],
				   XmNsensitive,
				      icong_array[i]->class_info.documented,
				   XmNuserData,
				       &(icong_array[i]->class_info),
				   NULL);
	i++ ;
    }

    XtManageChildren(icongs, i);

}

static void
OKCB (Widget w, XtPointer client_data, XtPointer call_data)
{
    String input_string ;
    int size_in_pixel ;
    String colon_location ;
    String resource_name ;
    XtEnum orientation ;

    /* get back something ala "width: 20cm" */
    input_string = XmTextFieldGetString((Widget)client_data);

    /* find the ':' */
    colon_location = strchr(input_string, ':');

    /* cut the input_string in two by ending it at color_location,
       the other piece being at colon_location+1 */
    if (colon_location) *colon_location = '\0';
    else {
	XtFree(input_string);
	return ;
    }

    /* now try to figure out the dimension being set */
    if (strstr(input_string, "width")) {
	orientation = XmHORIZONTAL ;
	resource_name = XmNwidth ;
    } else
    if (strstr(input_string, "height")) {
	orientation = XmVERTICAL ;
	resource_name = XmNheight ;
    } else {
	XtFree(input_string);
	return ;
    }

    size_in_pixel = XmConvertStringToUnits(XtScreen(w), colon_location+1,
					   orientation, XmPIXELS, NULL) ;

    if (size_in_pixel)
	XtVaSetValues(toplevel, resource_name, size_in_pixel, NULL);

    XtFree(input_string);
}



static void
InitPanedWindow (Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget child_w[2], sub_pane, temp, form, frame;

    child_w[0] = XtVaCreateWidget("Arrow1", xmArrowButtonWidgetClass,
				  w, NULL);
    sub_pane = child_w[1] = XtVaCreateWidget("SubPane",
					     xmPanedWindowWidgetClass,
					     w, NULL);
    XtManageChildren(child_w, 2);

    child_w[0] = XtVaCreateWidget("Arrow2", xmArrowButtonWidgetClass,
				  sub_pane, NULL);
    temp = child_w[1] = XtVaCreateWidget("Temp", xmMessageBoxWidgetClass,
					 sub_pane, NULL);
    XtRealizeWidget(temp);
    XtManageChildren(child_w, 2);

    form = XtVaCreateManagedWidget("Form", xmFormWidgetClass,
				   temp, NULL);

    frame = child_w[0] = XtVaCreateWidget("Frame", xmFrameWidgetClass,
					  form, NULL);
    child_w[1] = XtVaCreateWidget("TextF", xmTextFieldWidgetClass,
				  form, NULL);
    XtManageChildren(child_w, 2);
    XtAddCallback (temp, XmNokCallback, OKCB, (XtPointer)child_w[1]);

    child_w[0] = XtVaCreateWidget("Title", xmLabelGadgetClass,
				  frame, NULL);
    child_w[1] = XtVaCreateWidget("Label", xmLabelWidgetClass,
				  frame, NULL);
    XtManageChildren(child_w, 2);
}

static void
InitFileSelectionBox (Widget w, XtPointer client_data, XtPointer call_data)
{
    XtVaCreateManagedWidget("FileSelectionBox",
			    xmFileSelectionBoxWidgetClass,
			    w, NULL);
}

static void
InitTogglesWindow (Widget w, XtPointer client_data, XtPointer call_data)
{
    Cardinal i ;
#define NUM_TOG 50
    Widget work_area, toggles[NUM_TOG] ;

    work_area = XtVaCreateManagedWidget("RowColumn", xmRowColumnWidgetClass,
					w, NULL);
    for (i=0; i<NUM_TOG; i++) {
	char child_name[20] ;

	sprintf(child_name, "RCChild%d", i);
	toggles[i] = XtVaCreateWidget(child_name, xmToggleButtonWidgetClass,
				      work_area, NULL);
    }
    XtManageChildren(toggles, i);
}

static void
InitScaleCombo (Widget w, XtPointer client_data, XtPointer call_data)
{
    Arg			args[1];
    Widget		applyb, cbb, scale, selectb;
    Widget		spinb, sspinb_s, sspinb_n, textual;
    XtCallbackRec	sbModifyVerifyList[] = {
				{sb_ModifyVerifyCB, (caddr_t) NULL},
				{(XtCallbackProc) NULL, (caddr_t) NULL}
			    };
    XtCallbackRec	sbValueChangedList[] = {
				{sb_ValueChangedCB, (caddr_t) NULL},
				{(XtCallbackProc) NULL, (caddr_t) NULL}
			    };
    XtCallbackRec	tfActivateList[] = {
				{tf_ActivateCB, (caddr_t) NULL},
				{(XtCallbackProc) NULL, (caddr_t) NULL}
			    };
    XtCallbackRec	sbApplyList[] = {
				{sb_ApplyCB, (caddr_t) NULL},
				{(XtCallbackProc) NULL, (caddr_t) NULL}
			    };
    XtCallbackRec	sbCancelList[] = {
				{sb_CancelCB, (caddr_t) NULL},
				{(XtCallbackProc) NULL, (caddr_t) NULL}
			    };
    XtCallbackRec	sbOkList[] = {
				{sb_OkCB, (caddr_t) NULL},
				{(XtCallbackProc) NULL, (caddr_t) NULL}
			    };

    cbb = XtVaCreateWidget("ComboBox", xmComboBoxWidgetClass, w, NULL);
    XtRealizeWidget(cbb);
    XtManageChild(cbb);

    spinb = XtVaCreateWidget("SpinBox", xmSpinBoxWidgetClass, w,
			     XmNmodifyVerifyCallback, sbModifyVerifyList,
			     XmNvalueChangedCallback, sbValueChangedList,
			     NULL);
    XtRealizeWidget(spinb);
    XtManageChild(spinb);
    XtVaCreateManagedWidget("Date", xmTextFieldWidgetClass, spinb,
			    XmNactivateCallback, tfActivateList,
			    NULL);
    XtVaCreateManagedWidget("Month", xmTextFieldWidgetClass, spinb,
			    XmNactivateCallback, tfActivateList,
			    NULL);
    XtVaCreateManagedWidget("Year", xmTextFieldWidgetClass, spinb,
			    XmNactivateCallback, tfActivateList,
			    NULL);

    sspinb_n = XtVaCreateWidget(
		"SimpleSpinBoxNumeric", xmSimpleSpinBoxWidgetClass, w,
		XmNmodifyVerifyCallback, sbModifyVerifyList,
		XmNvalueChangedCallback, sbValueChangedList,
		NULL);
    textual = ssb_GetTextField(sspinb_n);
    XtSetArg(args[0], XmNactivateCallback, tfActivateList);
    XtSetValues(textual, args, 1);
    XtRealizeWidget(sspinb_n);
    XtManageChild(sspinb_n);

    sspinb_s = XtVaCreateWidget(
		"SimpleSpinBoxString", xmSimpleSpinBoxWidgetClass, w,
		XmNmodifyVerifyCallback, sbModifyVerifyList,
		XmNvalueChangedCallback, sbValueChangedList,
		NULL);
    textual = ssb_GetTextField(sspinb_s);
    XtSetArg(args[0], XmNactivateCallback, tfActivateList);
    XtSetValues(textual, args, 1);
    XtRealizeWidget(sspinb_s);
    XtManageChild(sspinb_s);

#ifdef SAMPLER_TEST_SSPINB
    XtVaSetValues(sspinb_n, XmNtextField, sspinb_s, NULL);
    XtVaSetValues(sspinb_n, XmNpositionType, XmPOSITION_INDEX, NULL);
    XtVaSetValues(spinb, XmNpositionType, XmPOSITION_INDEX, NULL);
#endif

#ifdef SAMPLER_TEST_SSPINB
    ssb_widgets[SSB_NUMERIC]  = sspinb_n;
    ssb_widgets[SSB_STRING]  = sspinb_s;

    sbApplyList[0].closure = (caddr_t) ssb_widgets;
    sbCancelList[0].closure = (caddr_t) ssb_widgets;
    sbOkList[0].closure = (caddr_t) ssb_widgets;

    selectb = XtVaCreateWidget(
		"SelectionBoxDebug", xmSelectionBoxWidgetClass, w,
		XmNapplyCallback, sbApplyList,
		XmNcancelCallback, sbCancelList,
		XmNokCallback, sbOkList,
		XmNdialogType, XmDIALOG_PROMPT,
		XmNmustMatch, False,
		NULL);
    XtRealizeWidget(selectb);
    XtManageChild(selectb);
    applyb = XtNameToWidget(selectb, "Apply");
    XtManageChild(applyb);
#endif

    scale = XtVaCreateWidget("Scale", xmScaleWidgetClass, w, NULL);
    XtRealizeWidget(scale);
    XtManageChild(scale);
    XmScaleSetTicks(scale,
		    10, 1, 4,
		    20, 10, 5);

}


/* Now comes the stuff that describe the Xt/Xm class hierarchy */

static void
SetDyn(WidgetClass wclass, Cardinal index, Boolean documented)
{
    static Cardinal i = 0 ;

    icong_array[i] = (IconGInfo) XtMalloc(sizeof(IconGInfoRec));

    icong_array[i]->parent_index = index ;
    icong_array[i]->class_info.w_class = wclass ;
    icong_array[i]->class_info.documented = documented ;

    i++ ;
}


static Cardinal
InitClassArray (void)
{
    /* This code builds the Xt/Motif widget class tree using
       a breadth first search algorithm */

  Cardinal n = 0;
  int index = -1;

  SetDyn(objectClass, index, True); PushIndex(n); n++;

  index = PopIndex () ;
  SetDyn(xmDragIconObjectClass, index, True); n++;
  SetDyn(xmDropTransferObjectClass, index, True); n++;
  SetDyn(xmDropSiteManagerObjectClass, index, False); n++;
  SetDyn(rectObjClass, index, True); PushIndex(n); n++;

  index = PopIndex () ;
  SetDyn(unNamedObjClass, index, False); PushIndex(n); n++;
  SetDyn(xmGadgetClass, index, True); PushIndex(n); n++;

  index = PopIndex () ;
  SetDyn(xmArrowButtonGadgetClass, index, True); n++;
  SetDyn(xmSeparatorGadgetClass, index, True); n++;
  SetDyn(xmLabelGadgetClass, index, True); PushIndex(n); n++;
  SetDyn(xmIconGadgetClass, index, True); PushIndex(n); n++;

  index = PopIndex () ;
  SetDyn(xmIconHeaderClass, index, False); n++;

  index = PopIndex () ;
  SetDyn(xmPushButtonGadgetClass, index, True); n++;
  SetDyn(xmToggleButtonGadgetClass, index, True); n++;
  SetDyn(xmCascadeButtonGadgetClass, index, True); n++;

  index = PopIndex () ;
  SetDyn(coreWidgetClass, index, True); PushIndex(n); n++;

  index = PopIndex () ;
  SetDyn(compositeWidgetClass, index, True); PushIndex(n); n++;
  SetDyn(xmScreenClass, index, True); n++;
  SetDyn(xmDragContextClass, index, True); n++;
  SetDyn(xmPrimitiveWidgetClass, index, True); PushIndex(n); n++;

  index = PopIndex () ;
  SetDyn(xmScrollBarWidgetClass, index, True); n++;
  SetDyn(xmArrowButtonWidgetClass, index, True); n++;
  SetDyn(xmListWidgetClass, index, True); n++;
  SetDyn(xmSeparatorWidgetClass, index, True); n++;
  SetDyn(xmTextFieldWidgetClass, index, True); n++;
  SetDyn(xmDrawnButtonWidgetClass, index, True); n++;
  SetDyn(xmTextWidgetClass, index, True); n++;
  SetDyn(xmSashWidgetClass, index, False); n++;
  SetDyn(xmLabelWidgetClass, index, True); PushIndex(n); n++;

  index = PopIndex () ;
  SetDyn(xmToggleButtonWidgetClass, index, True); n++;
  SetDyn(xmCascadeButtonWidgetClass, index, True); n++;
  SetDyn(xmPushButtonWidgetClass, index, True); PushIndex(n); n++;

  index = PopIndex () ;
  SetDyn(xmTearOffButtonWidgetClass, index, False); n++;

  index = PopIndex () ;
  SetDyn(constraintWidgetClass, index, True); PushIndex(n); n++;
  SetDyn(shellWidgetClass, index, True); PushIndex(n); n++;

  index = PopIndex () ;
  SetDyn(wmShellWidgetClass, index, True); PushIndex(n); n++;
  SetDyn(overrideShellWidgetClass, index, True); PushIndex(n); n++;

  index = PopIndex () ;
  SetDyn(xmMenuShellWidgetClass, index, True); n++;

  index = PopIndex () ;
  SetDyn(vendorShellWidgetClass, index, True); PushIndex(n); n++;

  index = PopIndex () ;
  SetDyn(xmGrabShellWidgetClass, index, False); n++;
  SetDyn(xmDragOverShellWidgetClass, index, False); n++;
  SetDyn(transientShellWidgetClass, index, True); PushIndex(n); n++;
  SetDyn(topLevelShellWidgetClass, index, True); PushIndex(n); n++;

  index = PopIndex () ;
  SetDyn(applicationShellWidgetClass, index, True); PushIndex(n); n++;

  index = PopIndex () ;
  SetDyn(xmDisplayClass, index, True); n++;

  index = PopIndex () ;
  SetDyn(xmDialogShellWidgetClass, index, True); n++;


  index = PopIndex () ;
  SetDyn(xmManagerWidgetClass, index, True); PushIndex(n); n++;

  index = PopIndex () ;
  SetDyn(xmDrawingAreaWidgetClass, index, True); n++;
  SetDyn(xmFrameWidgetClass, index, True); n++;
  SetDyn(xmRowColumnWidgetClass, index, True); n++;
  SetDyn(xmPanedWindowWidgetClass, index, True); n++;
  SetDyn(xmScaleWidgetClass, index, True); n++;
  SetDyn(xmComboBoxWidgetClass, index, True); n++;
  SetDyn(xmContainerWidgetClass, index, True); n++;
  SetDyn(xmNotebookWidgetClass, index, True); n++;
  SetDyn(xmSpinBoxWidgetClass, index, True); n++;
  SetDyn(xmScrolledWindowWidgetClass, index, True); PushIndex(n); n++;
  SetDyn(xmBulletinBoardWidgetClass, index, True); PushIndex(n); n++;

  index = PopIndex () ;
  SetDyn(xmFormWidgetClass, index, True); n++;
  SetDyn(xmMessageBoxWidgetClass, index, True); n++;
  SetDyn(xmSelectionBoxWidgetClass, index, True); PushIndex(n); n++;

  index = PopIndex () ;
  SetDyn(xmFileSelectionBoxWidgetClass, index, True); n++;
  SetDyn(xmCommandWidgetClass, index, True); n++;

  index = PopIndex () ;
  SetDyn(xmMainWindowWidgetClass, index, True); n++;

  return n ;
}


/* ARGSUSED */
static void
sb_ModifyVerifyCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    char			*buffer;
    XmSpinBoxCallbackStruct	*cbs = (XmSpinBoxCallbackStruct *) call_data;
    int				position = cbs->position;
    Widget			textual = (Widget) NULL;

    if (XtIsSubclass(w, xmSimpleSpinBoxWidgetClass))
      textual = ssb_GetTextField(w);
    else if (XtIsSubclass(w, xmSpinBoxWidgetClass))
      textual = cbs->widget;
    else
      textual = (Widget) NULL;

    if (! textual)
      {
	printf("Null textual widget for %s\n", XtName(w));
        return;
      }

#ifdef SAMPLER_TEST_SSPINB
    if (XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &buffer))
      printf("ModifyVerify value for %s.%s is %s\n",
	     XtName(w), XtName(textual), buffer);
#endif
}


/* ARGSUSED */
static void
sb_ValueChangedCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    char			*buffer;
    XmSpinBoxCallbackStruct	*cbs = (XmSpinBoxCallbackStruct *) call_data;
    int				position = cbs->position;
    Widget			textual = (Widget) NULL;

    if (XtIsSubclass(w, xmSimpleSpinBoxWidgetClass))
      textual = ssb_GetTextField(w);
    else if (XtIsSubclass(w, xmSpinBoxWidgetClass))
      textual = cbs->widget;
    else
      textual = (Widget) NULL;

    if (! textual) {
	printf("Null textual widget for %s\n", XtName(w));
        return;
    }

#ifdef SAMPLER_TEST_SSPINB
    if (XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &buffer))
      printf("ValueChanged value for %s.%s is %s\n",
	     XtName(w), XtName(textual), buffer);
#endif
}

/* ARGSUSED */
static void
tf_ActivateCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Arg		args[1];
    int		position, minimum;
    Widget	textual = (Widget) w;

    if (XmSpinBoxValidatePosition (textual, &position) == XmCURRENT_VALUE)
      XBell(XtDisplay(w), 0);

    /*
     * Force the widget to redraw itself.
     */
    XtSetArg(args[0], XmNposition, position);
    XtSetValues(textual, args, 1);
}


/* ARGSUSED */
/* ARGSUSED */
static void
sb_ApplyCB(Widget w, XtPointer client_data, XtPointer call_data)
{
/*
    Widget	*ssb_widgets = (Widget *) client_data;
*/
    XmString	xmstring;
    int		pos;
    Arg		args[1];

    /* Apply adds a new string to the sspinb_s widget */

    XtSetArg(args[0], XmNtextString, &xmstring);
    XtGetValues(w, args, 1);

    XtSetArg(args[0], XmNposition, &pos);
    XtGetValues(ssb_widgets[SSB_NUMERIC], args, 1);

    XmSimpleSpinBoxAddItem(ssb_widgets[SSB_STRING], xmstring, pos);
}


static void
sb_CancelCB(Widget w, XtPointer client_data, XtPointer call_data)
{
/*
    Widget	*ssb_widgets = (Widget *) client_data;
*/
    int		pos;
    Arg		args[1];

    /* Cancel deletes a string from the sspinb_s widget */

    XtSetArg(args[0], XmNposition, &pos);
    XtGetValues(ssb_widgets[SSB_NUMERIC], args, 1);

    XmSimpleSpinBoxDeletePos(ssb_widgets[SSB_STRING], pos);
}


/* ARGSUSED */
static void
sb_OkCB(Widget w, XtPointer client_data, XtPointer call_data)
{
/*
    Widget	*ssb_widgets = (Widget *) client_data;
*/
    XmString	xmstring;
    Arg		args[1];

    /* OK sets position of sspinb_s to the specified string. */

    XtSetArg(args[0], XmNtextString, &xmstring);
    XtGetValues(w, args, 1);

    XmSimpleSpinBoxSetItem(ssb_widgets[SSB_STRING], xmstring);
}


/* ARGSUSED */
static Widget
ssb_GetTextField(Widget w)
{
    Arg		args[1];
    Widget	textual = (Widget) NULL;

    XtSetArg(args[0], XmNtextField, &textual);
    XtGetValues(w, args, 1);
    return(textual);
}
