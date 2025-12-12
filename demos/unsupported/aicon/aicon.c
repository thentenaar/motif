/* $XConsortium: aicon.c /main/5 1995/07/15 20:47:16 drk $ */
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
#include <stdlib.h>
#include <Xm/XmAll.h>
#include <Xmd/AdjView.h>   
#include <Xmd/Help.h>   
#ifdef SYS_DIR
#include <sys/dir.h>
#else
#ifdef NDIR
#include <ndir.h>
#else
#include <dirent.h>
#endif
#endif

#define APP_CLASS "XmdAIcon"


 
typedef struct {
  String icon_dir;
  String valid_extensions;
} ApplicationData, *ApplicationDataPtr;
static ApplicationData AppData;

/* no need for a prefix, this is not exported */
#define NiconDir "iconDir"
#define CIconDir "IconDir"
#define NvalidExtensions "validExtensions"
#define CValidExtensions "ValidExtensions"

static XtResource resources[] = {
  { NiconDir, CIconDir, XmRString, sizeof(String),
	XtOffset(ApplicationDataPtr, icon_dir), XmRString, "." },
  { NvalidExtensions, CValidExtensions, XmRString, sizeof(String),
	XtOffset(ApplicationDataPtr, valid_extensions), XmRString, 
	"xbm,xpm,bm,pm" },
};

/*
 * Fallbacks: keep it minimum
 */
static String fallbacks[] = {
"*help_manager.helpFile: aicon",
"aicon.geometry:700x500",
NULL
};


static void 
HelpCB (Widget w, XtPointer client_data, XtPointer call_data) 
{
    static Widget help_widget = NULL ;

    if (!help_widget)
	help_widget = XmdCreateHelpDialog(w, "help_manager", NULL, 0);

    XtManageChild(help_widget);   
}

static void 
DefCB(
        Widget	widget,
	XtPointer tag,
	XtPointer data)
{
    XmContainerSelectCallbackStruct *cbs = 
	(XmContainerSelectCallbackStruct*)data;
    Pixmap pix ;
    Window root;
    int x,y;
    unsigned int loc_width, loc_height, border_width, loc_depth;

    XtVaGetValues(cbs->selected_items[0], XmNlargeIconPixmap, &pix, NULL);
    if (pix != XmUNSPECIFIED_PIXMAP) {
	XGetGeometry(XtDisplay(widget), pix, &root, &x, &y,
		     &loc_width, &loc_height, &border_width, &loc_depth);
	if (loc_depth == DefaultDepthOfScreen(XtScreen(widget))) {
	    XSetWindowBackgroundPixmap(XtDisplay(widget), 
				       DefaultRootWindow(XtDisplay(widget)), 
				       pix);
	    XClearWindow(XtDisplay(widget), 
			 DefaultRootWindow(XtDisplay(widget)));
	}
    }

}
static void
AppModalDialogWarning (w, str)
Widget w;
String str;
{	
    static Widget warning_dialog = NULL ;
    XmString message_string ;

    if (!warning_dialog) {
	warning_dialog = XmCreateWarningDialog (w, "Huh ho", NULL, 0);
	XtAddCallback (warning_dialog, XmNhelpCallback, HelpCB, NULL);
    }

    message_string = XmStringGenerate((XtPointer) str,
				      NULL, XmCHARSET_TEXT, NULL);
    XtVaSetValues(warning_dialog, XmNmessageString, message_string, NULL);
    XmStringFree(message_string);

    XtManageChild(warning_dialog);
}




static void 
PopulateContainer(cont, dir)
Widget cont ;
String dir ;
{
    char icon_fullname[256] ;
#if defined (SYS_DIR) || defined (NDIR)
    struct direct 	*item;		/*  entry in directory	*/
#else
    struct dirent 	*item;		/*  entry in directory	*/
#endif
    static Widget * icong = NULL ;
    static int n_icong = 0;
    Cardinal num_children, i;
    Widget * children ;
    int dir_len ;
    DIR	*dirp;		

    dir_len = strlen(dir) ;
    /* let's remove the FSB added * pattern */
    if (dir[dir_len-1] == '*') dir[--dir_len] = '\0';
    
    /*  try to open the icons directory  */
    if (!(dirp = opendir (dir))) {
	char s[256] ;
	sprintf(s, "Cannot open iconDir: %s\n", dir);
	AppModalDialogWarning(cont, s);
	return;
    }

    /* first destroy current set of icons (if any) */
    XtVaGetValues(cont, XmNnumChildren, &num_children, 
		  XmNchildren, &children, NULL);
    XtUnmanageChildren(children, num_children);
    for (i=0; i < num_children; i++) XtDestroyWidget(children[i]);

    strcpy(icon_fullname, dir);
    icon_fullname[dir_len] = '/' ;

    /* turn Container (back) to CELLS. Also ask for natural cell sizes */
    XtVaSetValues (cont, XmNspatialStyle, XmCELLS, 
		   XmNlargeCellWidth, 0, XmNlargeCellHeight, 0, NULL);  

    /*  read one entry each time through the loop  */
    for (item = readdir (dirp), i = 0;  item != NULL;  
	 item = readdir (dirp)) {

	/* reset the fullname every time */
	icon_fullname[dir_len+1] = '\0' ;
	strcat(icon_fullname, item->d_name);

	if ((!strcmp (item->d_name, ".")) ||
	    (!strcmp (item->d_name, "..")))
	    continue ;

	/* check the extension only if ALL is not part of it */
	if (!strstr(AppData.valid_extensions, "ALL")) {
	    /* the extension is defined by anything after the last . */
	    String extension = strrchr(item->d_name, '.');
	    /* if no . or . is last char of the file name, invalid */
	    if (!extension || extension[1] == '\0') continue ;
	    /* if the extension is in not the list, invalid */
	    if (!strstr(AppData.valid_extensions, extension+1)) continue;
	}

	if (i >= n_icong) {
	    n_icong += 500 ;
	    icong = (Widget*)XtRealloc((char*)icong, 
				       n_icong * sizeof(Widget*));
	}
	icong[i] = XtVaCreateWidget(item->d_name, xmIconGadgetClass, cont,
			  XtVaTypedArg, XmNlargeIconPixmap, XmRString,
			  icon_fullname, strlen(icon_fullname)+1, NULL);
	i++;
    }

    XtManageChildren(icong, i);

    /* turn Container back to NONE */
    XtVaSetValues (cont, XmNspatialStyle, XmNONE, NULL);  
}


static void QuitCB (w, client_data, call_data) 
Widget		w;		/*  widget id		*/
XtPointer		client_data;	/*  data from application   */
XtPointer		call_data;	/*  data from widget class  */
{
    exit(0);
}
static void UnMapCB (w, client_data, call_data) 
Widget		w;		/*  widget id		*/
XtPointer		client_data;	/*  data from application   */
XtPointer		call_data;	/*  data from widget class  */
{
    XtUnmanageChild(w);
}

static void OKCB (w, client_data, call_data) 
Widget		w;		/*  widget id		*/
XtPointer		client_data;	/*  data from application   */
XtPointer		call_data;	/*  data from widget class  */
{
    Widget cont = *(Widget*)client_data ;
    String dir ;
    unsigned char path_mode ;

    /* in CDE mode, the FilterText is useless, we have to look at the
       Directory text */
    XtVaGetValues(w, XmNpathMode, &path_mode, NULL);
    if (path_mode != XmPATH_MODE_RELATIVE)
	dir = XmTextGetString(XtNameToWidget(w, "FilterText"));
    else 
	dir = XmTextGetString(XtNameToWidget(w, "DirText"));

    PopulateContainer(cont, dir);
}

static void OpenCB (w, client_data, call_data) 
Widget		w;		/*  widget id		*/
XtPointer		client_data;	/*  data from application   */
XtPointer		call_data;	/*  data from widget class  */
{
    static Widget fsb_box = NULL ;

    if (!fsb_box) {
	Arg		args[2];	
	Cardinal	n;	
	XmString dir ;

	dir = XmStringGenerate((XtPointer) AppData.icon_dir,
			       NULL, XmCHARSET_TEXT, NULL);
	n = 0;
	XtSetArg (args[n], XmNdirectory, dir);  n++;
	fsb_box = XmCreateFileSelectionDialog (w, "Open Icon Dir", 
					       args, n);
	XtAddCallback (fsb_box, XmNhelpCallback, HelpCB, NULL);
	XmStringFree(dir);
	XtAddCallback (fsb_box, XmNokCallback, OKCB, client_data);
	XtAddCallback (fsb_box, XmNcancelCallback, UnMapCB, client_data);
	XtUnmanageChild(XtNameToWidget(fsb_box, "Selection"));
	XtUnmanageChild(XtNameToWidget(fsb_box, "Text"));
    }    

    XtManageChild (fsb_box);
}

static Widget CreateMainWindowWithMenuBar(toplevel, client_data)
Widget toplevel ;
XtPointer client_data ;
{
    Widget	main_window, menu_bar, menu_pane, cascade, button ;
    
    Arg		args[20];	
    Cardinal	n;		
    

    /*	Create MainWindow.
     */
    n = 0;
    main_window = XmCreateMainWindow (toplevel, "main_window", args, n);
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
    button = XmCreatePushButton (menu_pane, "Open...", args, n);
    XtManageChild (button);
    XtAddCallback (button, XmNactivateCallback, OpenCB, client_data);
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



 
int
main(int argc, char **argv)
{
    
    XtAppContext app_context;
    Widget		toplevel, sw, cont, mainw ;

    XtSetLanguageProc(NULL, NULL, NULL);
    toplevel = XtVaAppInitialize(&app_context, APP_CLASS, NULL, 0,
				 &argc, argv, fallbacks, NULL);

    XtGetApplicationResources(toplevel,
			      (XtPointer)&AppData,
			      resources,
			      XtNumber(resources),
			      NULL,
			      0);

    mainw = CreateMainWindowWithMenuBar(toplevel, &cont);
    XtManageChild(mainw);
 
    /* we want a scrolledwindow because the container window
       can potentially be huge */
    sw = XtVaCreateManagedWidget("sw", xmScrolledWindowWidgetClass, mainw,
			   XmNscrollingPolicy, XmAUTOMATIC, NULL);
    
    cont = XtVaCreateManagedWidget("cont", xmContainerWidgetClass, sw, 
			       XmNspatialResizeModel, XmGROW_BALANCED, NULL);
    XtAddCallback(cont, XmNdefaultActionCallback, DefCB, NULL);

    /* setup our clipwindow/workwindow constraint stuff */
    XmdSWAdjustView (sw) ;

    XtRealizeWidget(toplevel);

    PopulateContainer(cont, AppData.icon_dir);
    
    XtAppMainLoop(app_context);
    
   return 0;    /* make compiler happy */
}
