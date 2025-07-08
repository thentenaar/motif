/* $XConsortium: getsubres.c /main/6 1995/07/15 20:45:28 drk $ */
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
#include <Xmd/Help.h>   

/* This is no your ordinary demo, it displays widget class info and
   needs a couple of P.h file usually reserved for widget writers */
#include <Xm/VaSimpleP.h>

#include <Xm/SashP.h>
#include <Xm/TearOffBP.h>
#include <Xm/GrabShellP.h>
#include <Xm/IconHP.h>
#include <Xm/ClipWindowP.h>

static void QuitCB ();			
static void HelpCB ();			

typedef struct _ClassArrayRec {
    String name ;
    WidgetClass class ;
} ClassArrayRec ;


/* Cannot initialize everything in C */
static ClassArrayRec class_array[] = {
  { "Object",  (WidgetClass) NULL },
  { "RectObj",  (WidgetClass) NULL },
  { "Widget",  (WidgetClass) NULL },
  { "Core",  (WidgetClass) NULL },
  { "Composite",  (WidgetClass) NULL },
  { "Constraint",  (WidgetClass) NULL },  

  { "Primitive",  (WidgetClass) NULL },
  { "ScrollBar",  (WidgetClass) NULL },
  { "ArrowB",  (WidgetClass) NULL },
  { "List",  (WidgetClass) NULL },
  { "Separator",  (WidgetClass) NULL },
  { "TextF",  (WidgetClass) NULL },
  { "Label",  (WidgetClass) NULL },
  { "DrawnB",  (WidgetClass) NULL },
  { "PushB",  (WidgetClass) NULL },
  { "ToggleB",  (WidgetClass) NULL },
  { "CascadeB",  (WidgetClass) NULL },  

  { "Text",  (WidgetClass) NULL },       

  { "Gadget",  (WidgetClass) NULL },
  { "ArrowBG",  (WidgetClass) NULL },
  { "SeparatorG",  (WidgetClass) NULL },
  { "LabelG",  (WidgetClass) NULL },
  { "PushBG",  (WidgetClass) NULL },
  { "ToggleBG", (WidgetClass) NULL },    
  { "CascadeBG",  (WidgetClass) NULL }, 

  { "Manager", (WidgetClass) NULL },
  { "BulletinB", (WidgetClass) NULL },
  { "DrawingA", (WidgetClass) NULL },
  { "Frame", (WidgetClass) NULL  },
  { "MainW", (WidgetClass) NULL },
  { "Form", (WidgetClass) NULL },
  { "RowCol", (WidgetClass) NULL },
  { "ScrolledW", (WidgetClass) NULL },
  { "PanedW", (WidgetClass) NULL },
  { "SelectionB", (WidgetClass) NULL },
  { "FileSB", (WidgetClass) NULL },
  { "MessageB", (WidgetClass) NULL },
  { "Scale",  (WidgetClass) NULL },
  { "Command",  (WidgetClass) NULL },    

  { "Shell",  (WidgetClass) NULL },
  { "OverrideShell",  (WidgetClass) NULL },
  { "WMShell",  (WidgetClass) NULL },   

  { "MenuShell",  (WidgetClass) NULL },  

  { "VendorS",  (WidgetClass) NULL },
  { "ToplevelShell",  (WidgetClass) NULL },
  { "ApplicationShell",  (WidgetClass) NULL },
  { "TransientShell",  (WidgetClass) NULL },  

  { "DialogShell",  (WidgetClass) NULL },    

  { "Sash",  (WidgetClass) NULL },
  { "TearOffB",  (WidgetClass) NULL },

  { "Display",  (WidgetClass) NULL },
  { "Screen",  (WidgetClass) NULL },
  { "DragContext",  (WidgetClass) NULL },
  { "DragIcon",  (WidgetClass) NULL },
  { "DropSiteMgr",  (WidgetClass) NULL },
  { "DropTransfer",  (WidgetClass) NULL }, 

  { "DragOverS",  (WidgetClass) NULL },    

  { "GrabShell",  (WidgetClass) NULL },   
  { "IconH",  (WidgetClass) NULL },   
  { "ClipWindow",  (WidgetClass) NULL },   

  { "Container",  (WidgetClass) NULL },   
  { "IconG",  (WidgetClass) NULL },   
  { "Notebook",  (WidgetClass) NULL },   
  { "SpinBox",  (WidgetClass) NULL },   
  { "SimpleSpinBox",  (WidgetClass) NULL },   
  { "ComboBox",  (WidgetClass) NULL },   
};  

/* stupid c compiler */
static void InitClassArray () {
  int n;

  n = 0;
  class_array[n].class = objectClass; n++;
  class_array[n].class = rectObjClass; n++;
  class_array[n].class = widgetClass; n++;  /* unNamed */
  class_array[n].class = coreWidgetClass; n++;
  class_array[n].class = compositeWidgetClass; n++;
  class_array[n].class = constraintWidgetClass; n++;

  class_array[n].class = xmPrimitiveWidgetClass; n++;
  class_array[n].class = xmScrollBarWidgetClass; n++;
  class_array[n].class = xmArrowButtonWidgetClass; n++;
  class_array[n].class = xmListWidgetClass; n++;
  class_array[n].class = xmSeparatorWidgetClass; n++;
  class_array[n].class = xmTextFieldWidgetClass; n++;
  class_array[n].class = xmLabelWidgetClass; n++;
  class_array[n].class = xmDrawnButtonWidgetClass; n++;
  class_array[n].class = xmPushButtonWidgetClass; n++;
  class_array[n].class = xmToggleButtonWidgetClass; n++;
  class_array[n].class = xmCascadeButtonWidgetClass; n++;
  class_array[n].class = xmTextWidgetClass; n++;

  class_array[n].class = xmGadgetClass; n++;
  class_array[n].class = xmArrowButtonGadgetClass; n++;
  class_array[n].class = xmSeparatorGadgetClass; n++;
  class_array[n].class = xmLabelGadgetClass; n++;
  class_array[n].class = xmPushButtonGadgetClass; n++;
  class_array[n].class = xmToggleButtonGadgetClass; n++;
  class_array[n].class = xmCascadeButtonGadgetClass; n++;

  class_array[n].class = xmManagerWidgetClass; n++;
  class_array[n].class = xmBulletinBoardWidgetClass; n++;
  class_array[n].class = xmDrawingAreaWidgetClass; n++;
  class_array[n].class = xmFrameWidgetClass; n++;
  class_array[n].class = xmMainWindowWidgetClass; n++;
  class_array[n].class = xmFormWidgetClass; n++;
  class_array[n].class = xmRowColumnWidgetClass; n++;
  class_array[n].class = xmScrolledWindowWidgetClass; n++;
  class_array[n].class = xmPanedWindowWidgetClass; n++;
  class_array[n].class = xmSelectionBoxWidgetClass; n++;
  class_array[n].class = xmFileSelectionBoxWidgetClass; n++;
  class_array[n].class = xmMessageBoxWidgetClass; n++;
  class_array[n].class = xmScaleWidgetClass; n++;
  class_array[n].class = xmCommandWidgetClass; n++;

  class_array[n].class = shellWidgetClass; n++;
  class_array[n].class = overrideShellWidgetClass; n++;
  class_array[n].class = wmShellWidgetClass; n++;

  class_array[n].class = xmMenuShellWidgetClass; n++;

  class_array[n].class = vendorShellWidgetClass; n++;
  class_array[n].class = topLevelShellWidgetClass; n++;
  class_array[n].class = applicationShellWidgetClass; n++;
  class_array[n].class = transientShellWidgetClass; n++;
  class_array[n].class = xmDialogShellWidgetClass; n++;

  class_array[n].class = xmSashWidgetClass; n++;          /* primitive */
  class_array[n].class = xmTearOffButtonWidgetClass; n++; /* pushbutton */

  class_array[n].class = xmDisplayClass; n++;              /* appshell */
  class_array[n].class = xmScreenClass; n++;               /* core */
  class_array[n].class = xmDragContextClass; n++;          /* core */
  class_array[n].class = xmDragIconObjectClass; n++;       /* object */
  class_array[n].class = xmDropSiteManagerObjectClass; n++;/* object */
  class_array[n].class = xmDropTransferObjectClass; n++;   /* object */
  class_array[n].class = xmDragOverShellWidgetClass; n++;  /* vendors */

  class_array[n].class = xmGrabShellWidgetClass; n++;  
  class_array[n].class = xmIconHeaderClass; n++;  
  class_array[n].class = xmClipWindowWidgetClass; n++;  
  class_array[n].class = xmContainerWidgetClass; n++;  
  class_array[n].class = xmIconGadgetClass; n++;  
  class_array[n].class = xmNotebookWidgetClass; n++;   
  class_array[n].class = xmSpinBoxWidgetClass; n++;  
  class_array[n].class = xmSimpleSpinBoxWidgetClass; n++;  
  class_array[n].class = xmComboBoxWidgetClass; n++;  
}


static WidgetClass GetClassPointer(class)
String class ;
{
    Cardinal i ;
    static Boolean first_time = True ;

    if (first_time) InitClassArray () ;
    first_time = False ;

    for (i = 0 ; i < XtNumber(class_array); i++) {
	if (strcmp(class, class_array[i].name) == 0)
	    return class_array[i].class ;
    }
    return NULL ;
}


static String buffer = NULL ;
static short rows = 0, columns = 0 ;
    
static void 
AddToBuffer (String fmt, ...)
{
    va_list args;
    static Cardinal curpos = 0 ;
    char tmp[256] ;
    Cardinal i, tmplen ;

    Va_start(args, fmt);

    (void)vsnprintf(tmp, sizeof tmp, fmt, args);
    tmplen = strlen(tmp) ;
    columns = (columns > tmplen)? columns : tmplen ;
    buffer = XtRealloc(buffer, curpos + tmplen + 1);
    for (i=0 ; tmp[i]; i++) {
	buffer[curpos++] = tmp[i] ;
	if (tmp[i] == '\n') rows ++ ;
    }
    buffer[curpos] = '\0' ;
    

    va_end(args);
}


static void GetPrintRes (name) 
String name ;
{
    Cardinal i, j, k, num_resources, num_sec;
    XtResourceList resource_list ;
    XmSecondaryResourceData * res_sec_list ;
    WidgetClass class ;
    char buff_line[128];

    if ((class = GetClassPointer(name)) == NULL) {
	XtWarning("Cannot find specified class name");
	class = widgetClass ;
	name = "Widget" ;
    }

    XtInitializeWidgetClass (class) ;

    AddToBuffer("Fetching resources for widget %s:\n", name);
    AddToBuffer("=======================================\n");

    /* fetch Xt regular */
    XtGetResourceList(class, &resource_list, &num_resources);
    AddToBuffer("\nRegular Xt resources: %d\n",num_resources);
    AddToBuffer(  "------------------------\n");
    buff_line[0] = ' ';
    buff_line[1] = '\0';

    for (i = 0 ; i < num_resources; i++) {
	snprintf(buff_line + strlen(buff_line) - 1,
	         sizeof buff_line - strlen(buff_line),
	         "%-38s", resource_list[i].resource_name);

	if (i & 1) {
	    AddToBuffer(" %s\n", buff_line);
	    buff_line[0] = ' ';
	    buff_line[1] = '\0';
	}
    }

    if (!(i & 1))
	AddToBuffer(" %s\n", buff_line);
    XtFree((XtPointer)resource_list) ;

    /* fetch Xt constraint  */
    XtGetConstraintResourceList(class, &resource_list, &num_resources);
    if (num_resources) {
	AddToBuffer("\nConstraint Xt resources: %d\n",num_resources);
	AddToBuffer(  "------------------------\n");
	buff_line[0] = ' ';
	buff_line[1] = '\0';

	for (i = 0 ; i < num_resources; i++) {
	    snprintf(buff_line + strlen(buff_line) - 1,
	             sizeof buff_line - strlen(buff_line),
	             "%-38s", resource_list[i].resource_name);

	    if (i & 1) {
		AddToBuffer(" %s\n", buff_line);
		buff_line[0] = ' ';
		buff_line[1] = '\0';
	    }
	}

	if (!(i & 1))
	    AddToBuffer(" %s\n", buff_line);
	XtFree((XtPointer)resource_list) ;
    }
    
    /* fetch Motif second */
    if ((num_sec = XmGetSecondaryResourceData (class, &res_sec_list))) {
	AddToBuffer("\n\nMotif secondary blocks: %d\n", num_sec);    
	AddToBuffer(    "--------------------------\n");
    
	for (i = 0; i < num_sec; i++) {
	    buff_line[0] = ' ';
	    buff_line[1] = '\0';
	    AddToBuffer("\nSecondary[%d] : %d resources\n\n", i,
	                res_sec_list[i]->num_resources);
	    for (j = 0 ; j < res_sec_list[i]->num_resources; j++) {
		snprintf(buff_line + strlen(buff_line) - 1,
		         sizeof buff_line - strlen(buff_line),
		         "%-38s", res_sec_list[i]->resources[j].resource_name);

		if (j & 1) {
		    AddToBuffer(" %s\n", buff_line);
		    buff_line[0] = ' ';
		    buff_line[1] = '\0';
	        }
	    }

	    if (!(j & 1))
		AddToBuffer(" %s\n", buff_line);
	    XtFree((XtPointer)res_sec_list[i]->resources);
	    XtFree((XtPointer)res_sec_list[i]);
	}

	XtFree((XtPointer)res_sec_list);
    }
}

static String fallbacks[] = {
    "*help_manager.helpFile: getsubres",
    NULL
};

#define APP_CLASS "XmdGetsubres"

int main(argc, argv)
int argc ; char **argv;
{
    Widget toplevel, main_window, menu_bar, menu_pane, cascade, 
           button, viewer ;
    XtAppContext app_context;
    String name ;
    Cardinal i ;
    Arg args[10] ;
    Cardinal n;


    /* Usage: getres class | getres All*/
    toplevel = XtAppInitialize(&app_context, APP_CLASS, NULL, 0,
			       &argc, argv, fallbacks, NULL, 0);

    /* get a default in no name provided */
    if (argc == 1) name = "Widget";
    else name = argv[1] ;
    
    /* Fill a string buffer with lines of text (this demo was originally
       designed with printf...) */
    if (strcmp(name, "All") == 0) {
	for (i = 0; i < XtNumber(class_array); i++) {
	    GetPrintRes (class_array[i].name);
	    AddToBuffer(
            "\n*******************************************************\n\n");
	}
    } else {
	GetPrintRes(name) ;
    }

    n = 0;
    main_window = XmCreateMainWindow (toplevel, "main_window", args, n);
    XtManageChild (main_window);

    /*	Create MenuBar in MainWindow.
     */
    n = 0;
    menu_bar = XmCreateMenuBar (main_window, "menu_bar", args, n); 
    XtManageChild (menu_bar);


    /*	Create "File" PulldownMenu with a Quit button
     */
    n = 0;
    menu_pane = XmCreatePulldownMenu (menu_bar, "menu_pane", args, n);

    n = 0;
    button = XmCreatePushButton (menu_pane, "Quit", args, n);
    XtManageChild (button);
    XtAddCallback (button, XmNactivateCallback, QuitCB, NULL);

    n = 0;
    XtSetArg (args[n], XmNsubMenuId, menu_pane);  n++;
    cascade = XmCreateCascadeButton (menu_bar, "File", args, n);
    XtManageChild (cascade);


    /*	Create "Help" PulldownMenu with Help button.
     */
    n = 0;
    menu_pane = XmCreatePulldownMenu (menu_bar, "menu_pane", args, n);

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


    /* Create the viewer widget: here a text */
    n = 0;
    XtSetArg (args[n], XmNvalue, buffer);  n++;
    XtSetArg (args[n], XmNrows, rows+1);  n++;
    XtSetArg (args[n], XmNcolumns, columns-1);  n++;
    XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT);  n++;
    if (strcmp(name, "All") != 0) {
	viewer = XmCreateText(main_window, "viewer", args, n);
    } else  {
	viewer = XmCreateScrolledText(main_window, "viewer", args, n);
    } 
    XtManageChild(viewer);

    XtFree(buffer) ; /* Text has its own copy */

    XtRealizeWidget(toplevel);
    XtAppMainLoop(app_context);

    return 0;    /* make compiler happy */
}

/*-------------------------------------------------------------
**	QuitCB			- callback for quit button
*/
static void QuitCB (w, client_data, call_data) 
Widget		w;		/*  widget id		*/
caddr_t		client_data;	/*  data from applicaiton   */
caddr_t		call_data;	/*  data from widget class  */
{
    exit (0);
}

/*-------------------------------------------------------------
**	HelpCB			- callback for help button
*/
static void HelpCB (w, client_data, call_data) 
Widget		w;		/*  widget id		*/
caddr_t		client_data;	/*  data from application   */
caddr_t		call_data;	/*  data from widget class  */
{
    static Widget help_widget = NULL ;

    if (!help_widget)
	help_widget = XmdCreateHelpDialog(w, "help_manager", NULL, 0);

    XtManageChild(help_widget);   
}


 
