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
 */ 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "XmI.h"
#include <Xm/PrintSP.h>
#include "ImageCachI.h"
#include "SyntheticI.h"
#include <stdlib.h>
#include <unistd.h>

/********    Static Function Declarations    ********/

static void ClassPartInitialize( 
                        WidgetClass wc) ;
static void ClassInitialize( void ) ;
static void Initialize( 
                        Widget requested_widget,
                        Widget new_widget,
                        ArgList args,
                        Cardinal *num_args) ;
static Boolean SetValues( 
                        Widget current,
                        Widget request,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void Destroy( 
                        Widget w) ;
static void PrintNotifyHandler(
		    Widget w,
		    XtPointer client_data,
		    XEvent *event,
		    Boolean *continue_to_dispatch);

static void ResourcesUpdate(
			    Widget w,
			    XPContext pcontext,
			    Dimension * pwidth, Dimension * pheight);

static void AttributesNotifyHandler(
		    Widget w,
		    XtPointer client_data,
		    XEvent *event,
		    Boolean *continue_to_dispatch);


static void SelectXpEvents (
		      Widget widget,
		      int *event_types,
		      XtPointer *select_data,
		      int count,
		      XtPointer client_data);

static void PrintToFileProc(Display  * dpy,
			    XPContext context,
			    unsigned char *data, 
			    unsigned int data_len,
			    XPointer client_data);

static void FinishProc(Display *display,
		       XPContext context,
		       XPGetDocStatus status,
		       XPointer client_data);

static void PDMPhase2Handler(Widget w,
			     XtPointer data,
			     XEvent *event,
			     Boolean *cont);

static void PDMSelectionProc (Widget w, 
			 XtPointer client_data,
			 Atom *selection,
			 Atom *type,
			 XtPointer value,
			 unsigned long *length,
			 int *format);

static Boolean PrintDispatchEvent (XEvent *event);

static void GetFromTable (Display* pdpy, XPContext context, Widget * widget);
static void DeleteFromTable (XPContext context, Widget widget);
static void AddToTable (XPContext context, Widget widget);


#define Offset(x) (XtOffsetOf(XmPrintShellRec, print.x))

static XtResource resources[] = {
   {
      XmNallowShellResize, XmCAllowShellResize, XmRBoolean, 
      sizeof(Boolean), XtOffsetOf(XmPrintShellRec, shell.allow_shell_resize), 
      XtRImmediate, (XtPointer)False,
   },
   {
       XmNminX, XmCMinX, XmRHorizontalDimension, sizeof (Dimension), 
       Offset(min_x), XmRImmediate, (XtPointer) 0,
   },
   {
       XmNminY, XmCMinY, XmRVerticalDimension, sizeof (Dimension), 
       Offset(min_y), XmRImmediate, (XtPointer) 0,
   },
   {
       XmNmaxX, XmCMaxX, XmRHorizontalDimension, sizeof (Dimension), 
       Offset(max_x), XmRImmediate, (XtPointer) 0,
   },
   {
       XmNmaxY, XmCMaxY, XmRVerticalDimension, sizeof (Dimension), 
       Offset(max_y), XmRImmediate, (XtPointer) 0,
   },    
   {
	XmNdefaultPixmapResolution, XmCDefaultPixmapResolution, XmRShort,
	sizeof(unsigned short), Offset(default_pixmap_resolution), 
	XmRImmediate, (XtPointer)100,
    },
    {
        XmNstartJobCallback, XmCCallback,
 	XmRCallback, sizeof(XtCallbackList),
 	Offset(start_job_callback),
        XmRImmediate, (XtPointer)NULL,
    },
    {
        XmNendJobCallback, XmCCallback,
 	XmRCallback, sizeof(XtCallbackList),
 	Offset(end_job_callback),
        XmRImmediate, (XtPointer)NULL,
    },
    {
        XmNpageSetupCallback, XmCCallback,
 	XmRCallback, sizeof(XtCallbackList),
 	Offset(page_setup_callback),
        XmRImmediate, (XtPointer)NULL,
    },
    {
        XmNpdmNotificationCallback, XmCCallback,
 	XmRCallback, sizeof(XtCallbackList),
 	Offset(pdm_notification_callback),
        XmRImmediate, (XtPointer)NULL,
    },

};

/** only getvalues is supported on these, not setvalues hook therefore */
static XmSyntheticResource syn_resources[] =
{
    {
        XmNminX, sizeof(Dimension), Offset(min_x),
        XmeFromHorizontalPixels,NULL,
    },
    {
        XmNminY, sizeof(Dimension), Offset(min_y),
        XmeFromVerticalPixels,NULL,
    },
    {
        XmNmaxX, sizeof(Dimension), Offset(max_x),
        XmeFromHorizontalPixels,NULL,
    },
    {
        XmNmaxY, sizeof(Dimension), Offset(max_y),
        XmeFromVerticalPixels,NULL,
    },
};

#undef Offset



externaldef(xmprintshellclassrec)
XmPrintShellClassRec xmPrintShellClassRec = {
    {	
	(WidgetClass) &applicationShellClassRec,  /* superclass		*/   
	"XmPrintShell",			/* class_name 		*/   
	sizeof(XmPrintShellRec),	/* size 		*/   
	ClassInitialize,		/* Class Initializer 	*/   
	ClassPartInitialize,	        /* class_part_init 	*/ 
	FALSE, 				/* Class init'ed ? 	*/   
	Initialize,		        /* initialize         	*/   
 	NULL,		                /* initialize_notify    */ 
	XtInheritRealize,		/* realize            	*/   
	NULL,	 			/* actions            	*/   
	0,				/* num_actions        	*/   
	resources,			/* resources          	*/   
	XtNumber(resources),		/* resource_count     	*/   
	NULLQUARK, 			/* xrm_class          	*/   
	FALSE, 				/* compress_motion    	*/   
	XtExposeCompressSeries, 	/* compress_exposure  	*/   
	FALSE, 				/* compress_enterleave	*/   
	FALSE, 				/* visible_interest   	*/   
	Destroy,			/* destroy            	*/   
	XtInheritResize, 		/* resize             	*/   
	(XtExposeProc)NULL, 		/* expose             	*/   
	SetValues, 		        /* set_values         	*/   
	(XtArgsFunc)NULL, 		/* set_values_hook      */ 
	XtInheritSetValuesAlmost,	/* set_values_almost    */ 
	_XmPrintShellGetValuesHook,	/* get_values_hook      */ 
	(XtAcceptFocusProc)NULL, 	/* accept_focus       	*/   
	XtVersion, 			/* intrinsics version 	*/   
	NULL, 				/* callback offsets   	*/   
	XtInheritTranslations,		/* tm_table           	*/   
	XtInheritQueryGeometry, 	/* query_geometry       */ 
	(XtStringProc)NULL, 		/* display_accelerator  */ 
	(XtPointer)NULL, 	        /* extension            */ 
    },	
    { 					/* composite class record */
	XtInheritGeometryManager,	/* geometry_manager 	*/
	XtInheritChangeManaged,		/* change_managed	*/
	XtInheritInsertChild,		/* insert_child		*/
	XtInheritDeleteChild, 		/* from the shell 	*/
	NULL, 				/* extension record     */
    },
    { 					/* shell class record 	*/
	NULL, 				/* extension record     */
    },
    { 					/* wm shell class record */
	NULL, 				/* extension record     */
    },
    { 					/* vendor shell class record */
	NULL,				/* extension record     */
    },
    { 					/* toplevelclass record */
	NULL, 				/* extension record     */
    },
    { 					/* appShell record 	*/
	NULL, 				/* extension record     */
    },
    {					/* PrintShell class	*/
	syn_resources,         		/* syn_resources */
	XtNumber(syn_resources),	/* num_syn_resources */
	NULL,				/* extension		*/
    },
};

externaldef(xmprintshellclass) WidgetClass 
      xmPrintShellWidgetClass = (WidgetClass) (&xmPrintShellClassRec);


static XContext PrintContextToWidget = 0;

/* global because used in ResInd.c to find out about resolution */
XContext _XmPrintScreenToShellContext = 0;
Cardinal _XmPrintShellCounter = 0 ;

/************************************************************************
 *
 *  ClassInitialize
 *
 ************************************************************************/
static void 
ClassInitialize( void )
{
    if (PrintContextToWidget == 0)
	PrintContextToWidget = XUniqueContext();
    if (_XmPrintScreenToShellContext == 0)
	_XmPrintScreenToShellContext = XUniqueContext();
}    

/************************************************************************
 *
 *  ClassPartInitialize
 *
 ************************************************************************/
static void 
ClassPartInitialize(
	WidgetClass wc )
{
	_XmFastSubclassInit(wc, XmPRINT_SHELL_BIT);
}


static void 
PrintNotifyHandler(
		    Widget w,
		    XtPointer client_data,
		    XEvent *event,
		    Boolean *continue_to_dispatch)
{
    XPPrintEvent * xpev = (XPPrintEvent *) event ;
    XmPrintShellWidget print_shell = (XmPrintShellWidget) w ;
    XmPrintShellCallbackStruct pr_cbs ;
    
    /* don't interfere with the app if it hasn't set any callback.
       (since we set the handler all the time)
       It is probably using it's own handler doing start page, etc */
    if (!print_shell->print.start_job_callback &&
	!print_shell->print.page_setup_callback &&
	!print_shell->print.end_job_callback) return ;

    pr_cbs.event = event;
    pr_cbs.context = xpev->context ;

    switch (xpev->detail) {
    case XPStartJobNotify :
	print_shell->print.last_page = False ;
	pr_cbs.reason = XmCR_START_JOB ;
	pr_cbs.last_page = False ;
	XtCallCallbackList (w, print_shell->print.start_job_callback, 
			    &pr_cbs);
	/* fall thru : call both callbacks if present */
    case XPEndPageNotify :
	/* if last end page, no need to call pagesetup again */
	if (!print_shell->print.last_page) {
	    pr_cbs.reason = XmCR_PAGE_SETUP;
	    pr_cbs.last_page = False ;
	    XtCallCallbackList (w, print_shell->print.page_setup_callback, 
				&pr_cbs);
	    /* on return from pageSetup, save whether or not that was the
	       last page set up by the app */
	    print_shell->print.last_page = pr_cbs.last_page;

	    /* only start a page if a callback is there */
	    if (print_shell->print.page_setup_callback) {
		XpStartPage(XtDisplay(w), XtWindow(w));	
	    }
	}
	break ;
    case XPStartPageNotify :
	if (print_shell->print.page_setup_callback) {
	    XpEndPage(XtDisplay(w));
	    if (print_shell->print.last_page) XpEndJob(XtDisplay(w));
	}
	break ;
    case XPEndJobNotify :
	pr_cbs.reason = XmCR_PAGE_SETUP;
	pr_cbs.last_page = True ;
	XtCallCallbackList (w, print_shell->print.page_setup_callback, 
			    &pr_cbs);
	pr_cbs.reason = XmCR_END_JOB ;
	XtCallCallbackList (w, print_shell->print.end_job_callback, 
			    &pr_cbs);
	break ;
    }
}

static void 
ResourcesUpdate(Widget w, XPContext pcontext,
		Dimension * pwidth, Dimension * pheight)
{
    XmPrintShellWidget print_shell = (XmPrintShellWidget) w ;
    String string_resolution ;
    XRectangle imageable_area ;

    /*** Get the resolution so that unit Type conversion works */
    string_resolution =  
	XpGetOneAttribute(XtDisplay(w), pcontext,
			  XPDocAttr, "default-printer-resolution") ;
    print_shell->print.print_resolution = atoi(string_resolution);
    XFree(string_resolution);

    if (!print_shell->print.print_resolution)
	/* compute from the X screen resolution */
	print_shell->print.print_resolution = 
	    (254 * WidthOfScreen(XtScreen(w)) +
	     5 * WidthMMOfScreen(XtScreen(w))) /
	    (10 * WidthMMOfScreen(XtScreen(w))) ;

    /* get our size from the print server now */
    XpGetPageDimensions(XtDisplay(w), pcontext,
			pwidth, pheight, 
			&imageable_area);
    print_shell->print.min_x = imageable_area.x ;    
    print_shell->print.min_y = imageable_area.y ;    
    print_shell->print.max_x = imageable_area.x + imageable_area.width ;    
    print_shell->print.max_y = imageable_area.y + imageable_area.height ;
}

static void 
AttributesNotifyHandler(
		    Widget w,
		    XtPointer client_data,
		    XEvent *event,
		    Boolean *continue_to_dispatch)
{
    XPAttributeEvent * xatrev = (XPAttributeEvent *) event ;
    XmPrintShellWidget print_shell = (XmPrintShellWidget) w ;
    Dimension width = w->core.width, height = w->core.height;

    /* For now, blindly update resolution and sizes */
    ResourcesUpdate(w, xatrev->context, &width, &height);

    XtResizeWidget(w, width, height, w->core.border_width);
}

static void 
SelectXpEvents (
		Widget widget,
		int *event_types,
		XtPointer *select_data,
		int count,
		XtPointer client_data)
{
    XpSelectInput(XtDisplay(widget),
		  XpGetContext(XtDisplay(widget)),
		  XPPrintMask|XPAttributeMask);
}


/*****************************
  Maintain a XPContext to widget binding for use in event dispatch to 
  print shell logic. 
  Limitation is only one shell per XPContext in this implementation.
  Could improve to dispatch to all widget in this context later, but
  not sure if supported by Xp itself.
******************************/

static void GetFromTable (Display* pdpy, XPContext context, Widget * widget) 
{
    XFindContext(pdpy, (XID)context, PrintContextToWidget, 
		 (XPointer *) widget);
}

static void DeleteFromTable (XPContext context, Widget widget) 
{
    XDeleteContext(XtDisplay(widget), (XID)context, PrintContextToWidget);
}

static void AddToTable (XPContext context, Widget widget) 
{
    XSaveContext(XtDisplay(widget), (XID)context, PrintContextToWidget, 
	 	 (XPointer) widget);
}


/*****************************/
static Boolean 
PrintDispatchEvent (XEvent *event)
{
    XPPrintEvent * xpev = (XPPrintEvent *) event ;
    Widget widget = NULL ;

    /* I only have a context, not a window, so I can't use
       XtWindowToWidget..  I have to maintain my own table
       widget/context.  Limitation: one context to one widget for now */

    GetFromTable(xpev->display, xpev->context, &widget);
    /* If ever supported, need to loop thru a list, if several
       print_shell have been created under the same XPContext, and
       dispatch the event to all of them */

    if (!widget) return False ;

    /* spec says to call filter first, even though in my case,
       I shouldn't have any IM around... */
    if (XFilterEvent(event, XtWindow(widget)))
	return True ;

    return XtDispatchEventToWidget(widget, event);
}


/************************************************************************
 *
 *  Initialize
 *
 ************************************************************************/
static void 
Initialize(
        Widget req_widget,
        Widget new_widget,
        ArgList args,
        Cardinal *num_args )
{
    int event_base_return, error_base_return ;
    XmPrintShellWidget print_shell = (XmPrintShellWidget) new_widget ;
    XPContext pcontext ;

    _XmProcessLock();
    /* mark the screen of this print shell in this context */
    XSaveContext(XtDisplay(new_widget), (XID)XtScreen(new_widget), 
		 _XmPrintScreenToShellContext, (XPointer) new_widget);
    /* also maintain a counter of all shell */
     _XmPrintShellCounter ++ ;
    _XmProcessUnlock();

    print_shell->print.xp_connected = False ;
    print_shell->print.print_resolution = 100 ;

    /*** first check if the Print extension is present at all */
    if (!XpQueryExtension (XtDisplay(new_widget),
			   &event_base_return, &error_base_return)) {
	return ;
    }

    /* need to check that the print context is set for
       the screen of this print shell (i.e. it's a print screen,
       not just a screen on a display that support printing
       and for which a context was establish on a different screen -
       which can be common in preview case */
    pcontext = XpGetContext(XtDisplay(new_widget));
    if (!pcontext || 
	XpGetScreenOfContext(XtDisplay(new_widget),pcontext) 
	!= XtScreen(new_widget)) {
	return ;
    }

    print_shell->print.xp_connected = True ;

    /*** add an element to a widget/XPContext table, used in 
         the event dispatch mechanism */
    AddToTable(pcontext, new_widget);

    /*** add extension event Handler for Start/End Job/Page Notify */
    XtInsertEventTypeHandler(new_widget, 
			     event_base_return + XPPrintNotify,
			     (XtPointer)XPPrintMask, 
			     PrintNotifyHandler, NULL, 
			     XtListTail);

    /*** also add extension event Handler for tracking attributes change */
    XtInsertEventTypeHandler(new_widget, 
			     event_base_return + XPAttributeNotify,
			     (XtPointer)XPAttributeMask, 
			     AttributesNotifyHandler, NULL, 
			     XtListTail);

    /* always register the extension selector, and the event dispatcher,
       it will override itself accordingly and works ok for different
       display connection */
    XtRegisterExtensionSelector(XtDisplay(new_widget), 
				event_base_return + XPPrintNotify, 
				event_base_return + XPAttributeNotify, 
				SelectXpEvents,
				NULL);
    (void) XtSetEventDispatcher(XtDisplay(new_widget), 
				event_base_return + XPPrintNotify, 
				PrintDispatchEvent);
    (void) XtSetEventDispatcher(XtDisplay(new_widget), 
				event_base_return + XPAttributeNotify, 
				PrintDispatchEvent);
			
    /* go and get resolution and initial sizes */
    ResourcesUpdate(new_widget, pcontext, 
		    &(new_widget->core.width), 
		    &(new_widget->core.height));
}

/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/
/*ARGSUSED*/
static Boolean 
SetValues(
        Widget current,
        Widget request,		/* unused */
        Widget new_w,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
    XmPrintShellWidget cur_print_shell = (XmPrintShellWidget) current ;
    XmPrintShellWidget new_print_shell = (XmPrintShellWidget) new_w ;

    if (!new_print_shell->print.xp_connected)
	return False ;

    /* need to check for changes in getvalues only resources .
       need to put out a warning */
    if (cur_print_shell->print.min_x != new_print_shell->print.min_x)
	new_print_shell->print.min_x = cur_print_shell->print.min_x ;
    if (cur_print_shell->print.min_y != new_print_shell->print.min_y)
	new_print_shell->print.min_y = cur_print_shell->print.min_y ;
    if (cur_print_shell->print.max_x != new_print_shell->print.max_x)
	new_print_shell->print.max_x = cur_print_shell->print.max_x ;
    if (cur_print_shell->print.max_y != new_print_shell->print.max_y)
	new_print_shell->print.max_y = cur_print_shell->print.max_y ;

    return False ;
}

/************************************************************************
 *
 *  Destroy
 *
 ************************************************************************/
/* ARGSUSED */
static void 
Destroy(
        Widget w )
{
    XmPrintShellWidget print_shell = (XmPrintShellWidget) w ;

    /* need to remove the pixmap from this shell: no sharing
       between diff shell and the same shell pointer id can
       come up next time */
    _XmCleanPixmapCache (XtScreen(w), w);

    if (!print_shell->print.xp_connected)
	return ;

    /*** remove entry in the widget/XPContext table, used in 
         the event dispatch mechanism */
    DeleteFromTable(XpGetContext(XtDisplay(w)), w);

    _XmProcessLock();
    /* unmark the screen of this print shell in this context */
    XDeleteContext(XtDisplay(w), (XID)XtScreen(w), 
		   _XmPrintScreenToShellContext);
    /* also maintain a counter of all shells alive */
    _XmPrintShellCounter -- ;
    _XmProcessUnlock();
}


/************************************************************************
 *
 *  Public API
 *
 ************************************************************************/

/************************************************************************
 *
 *  XmPrintSetup
 *
 *    add print display in app context and create the print shell 
 *
 ************************************************************************/
 

Widget
XmPrintSetup(
             Widget           video_widget,
             Screen           *print_screen,
             String            print_shell_name,
             ArgList           args,
             Cardinal          num_args)
{
    String video_name, video_class ;
    Widget pappshell ;
    Widget print_shell;
    Display  *print_display = DisplayOfScreen(print_screen);

    /* get to the video app root shell */
    while(!XtIsApplicationShell(video_widget))
	video_widget = XtParent(video_widget);

    if (!video_widget) return NULL;

    XtGetApplicationNameAndClass(XtDisplay(video_widget),
                                 &video_name, &video_class);

    /* we want to be able to specify things like:
       dtpad.print*textFontList: somefont
       dtpad.print*background:white
       so we create a first shell unrealized and them a popup
       named "print".
       */
    pappshell = XtVaAppCreateShell(video_name, video_class,
                                   applicationShellWidgetClass, 
                                   print_display, 
                                   XmNscreen, print_screen, 
                                   NULL);

    /* then create the XmPrintShell */
    print_shell = XtCreatePopupShell(print_shell_name, 
				     xmPrintShellWidgetClass, 
				     pappshell, args, num_args);

    /* Add callback to destroy application Shell parent. */
    XtAddCallback(print_shell, XmNdestroyCallback, 
                  _XmDestroyParentCallback, (XtPointer) NULL) ;

    /* we're mapping/unmapping at start/end page time */
    XtSetMappedWhenManaged(print_shell, False);

    /* realize the shell now, so that XmPrintPopupPDM works dy default */
    XtRealizeWidget(print_shell);

    return print_shell ;
}


/************************************************************************
 *
 *  XmPrintToFile
 *
 ************************************************************************/

/* need to be able to remove the filename or write/close the file */
typedef struct {
    String file_name;
    FILE * file;
    int pipe;
} FileDescRec ;


typedef struct { 
  Display *display;
  XPFinishProc finish_proc;
  XPointer client_data;
  int pipe;
  XtInputId input_id;
} FileCallbackRec;


static void PrintToFileProc(Display  * dpy,
			    XPContext context,
			    unsigned char *data, 
			    unsigned int data_len,
			    XPointer client_data)
{
    FileDescRec * file_desc = (FileDescRec *) client_data ;
    
    /* write to the file */
    fwrite(data, data_len, 1, file_desc->file);
}   


static void FinishProc(Display *display,
		       XPContext context,
		       XPGetDocStatus status,
		       XPointer client_data)
{
    FileDescRec * file_desc = (FileDescRec *) client_data ;

    /* remove the file if not successfull */
    if (status != XPGetDocFinished)
	 remove(file_desc->file_name);
    else fclose(file_desc->file);

    /* write the status to the parent */
    (void) write(file_desc->pipe, &status, sizeof(XPGetDocStatus));

    /* we don't do any free's or close's, as we are just
       going to exit, in fact, get out without calling any C++
       destructors, etc., as we don't want anything funny to happen
       to the parent */
    _exit(0);
}


static void FilePipeCB(XtPointer client_data, int *source, XtInputId *id)
{
  FileCallbackRec *cb = (FileCallbackRec *) client_data;
  XPGetDocStatus status;

  /* read the status from the child */ 
  (void) read(cb->pipe, &status, sizeof(XPGetDocStatus));

  XtRemoveInput(cb->input_id);

  close(cb->pipe);

  if (cb->finish_proc) {
    (*cb->finish_proc)(cb->display, XpGetContext(cb->display),
		       status, cb->client_data);
  }

  XtFree((char*)cb);
}


static void ChildPrintToFile(String display_name, 
		      XPContext pcontext,
		      FILE *file,
		      char *file_name,
		      int pipe,
		      String application_name, 
		      String application_class)
{
  FileDescRec *file_desc;
  XtAppContext app_context;
  int argc = 0;
  String argv[] = { NULL };
  Display *display;

  file_desc = (FileDescRec *) XtMalloc(sizeof(FileDescRec));
  file_desc->file_name = XtNewString(file_name);
  file_desc->file = file;
  file_desc->pipe = pipe;

  app_context = XtCreateApplicationContext();
  if ((display = XtOpenDisplay(app_context, display_name, 
			       application_name, application_class,
			       NULL, 0, &argc, argv)) == NULL) {
    _exit(1);
  }

  XpGetDocumentData(display, pcontext,
		    PrintToFileProc, FinishProc, 
		    (XPointer) file_desc);

  XtAppMainLoop(app_context);

  _exit(0);
}


XtEnum XmPrintToFile(Display *pdpy, 
		     char *file_name,
		     XPFinishProc finish_proc, 
		     XPointer client_data)
{
    FileCallbackRec *callback;
    int pid;
    XPContext pcontext ;
    String application_name, application_class, display_name;
    FILE *file;
    int filedes[2];
    
    /* make sure we can open the file for writing */
    if ((file = fopen(file_name, "w")) == NULL) {
      return False;
    }

    if ((pipe(filedes)) == -1) {
      fclose(file);
      return False;
    }

    /* its important to flush before we fork, to make sure that the
       XpStartJob gets through first in the parent */
    XFlush(pdpy);

    XtGetApplicationNameAndClass(pdpy,
				 &application_name, &application_class);

    display_name = XDisplayString(pdpy) ;
    pcontext = XpGetContext(pdpy) ;
    pid = fork();

    if (pid == 0) {
      ChildPrintToFile(display_name, pcontext, file, file_name, filedes[1],
		       application_name, application_class);
    }
    else if (pid < 0) {
      fclose(file);
      return False;
    }

    /* we are in the parent */

    fclose(file);
    close(filedes[1]);

    /* allocate the space for a callback */
    callback = (FileCallbackRec *) XtMalloc(sizeof(FileCallbackRec));
    callback->display = pdpy;
    callback->pipe = filedes[0];
    callback->finish_proc = finish_proc;
    callback->client_data = client_data;

    /* notification that the child has completed */
    callback->input_id = 
        XtAppAddInput(XtDisplayToApplicationContext(pdpy), 
		      callback->pipe, (XtPointer)XtInputReadMask,
		      FilePipeCB, callback);

    return True;
}


/*************************************************************
 **  XmPrintPopupPDM   code
 *************************************************************/

static void
PDMPhase2Handler(Widget w,
		 XtPointer data,
		 XEvent *event,
		 Boolean *cont)
{
    enum { XmAPDM_REPLY, XmAPDM_EXIT_OK, XmAPDM_EXIT_ERROR,
	   XmAPDM_EXIT_CANCEL, NUM_ATOMS };
    static char *atom_names[] =
      { XmIPDM_REPLY, XmIPDM_EXIT_OK, XmIPDM_EXIT_ERROR, XmIPDM_EXIT_CANCEL };

    XmPrintShellWidget print_shell = (XmPrintShellWidget) w ;
    XmPrintShellCallbackStruct pr_cbs ;
    XClientMessageEvent * xevent = (XClientMessageEvent *) event;
    Atom atoms[XtNumber(atom_names)];

    assert(XtNumber(atom_names) == NUM_ATOMS);
    XInternAtoms(XtDisplay(print_shell), atom_names, XtNumber(atom_names), 
		 False, atoms);

    if (xevent->type == ClientMessage &&
	xevent->message_type == atoms[XmAPDM_REPLY]) {

	/* check for the exit status of the PDM */
	if (xevent->data.l[0] == atoms[XmAPDM_EXIT_OK])
	    pr_cbs.reason = XmCR_PDM_OK ;
	else 
	if (xevent->data.l[0] == atoms[XmAPDM_EXIT_CANCEL])
	    pr_cbs.reason = XmCR_PDM_CANCEL ;
	else 
	if (xevent->data.l[0] == atoms[XmAPDM_EXIT_ERROR])
	    pr_cbs.reason = XmCR_PDM_EXIT_ERROR ;
	    /* some error message might have been logged */

	XtCallCallbackList (w, print_shell->print.pdm_notification_callback, 
			    &pr_cbs);
    }

    /* remove me */
    XtAddEventHandler(w, (EventMask) 0, True, PDMPhase2Handler, NULL);
}

typedef struct {
    Atom pdm_selection ;
    XmPrintShellWidget print_shell ;
    Widget transient_for_video_shell ;
    Window transient_for_input_only_window;
} PDMSelectData ;

static void PDMSelectionProc (Widget w, 
			 XtPointer client_data,
			 Atom *selection,
			 Atom *type,
			 XtPointer value,
			 unsigned long *length,
			 int *format)
{
    enum { XmAPDM_START_OK, XmAPDM_START_ERROR, XmAPDM_START_VXAUTH,
	   XmAPDM_START_PXAUTH, NUM_ATOMS };
    static char *atom_names[] = {
      XmIPDM_START_OK, XmIPDM_START_ERROR, XmIPDM_START_VXAUTH,
      XmIPDM_START_PXAUTH };

    XmPrintShellCallbackStruct pr_cbs ;
    PDMSelectData * pdm_select_data = (PDMSelectData *) client_data ;
    Atom atoms[XtNumber(atom_names)];

    assert(XtNumber(atom_names) == NUM_ATOMS);
    XInternAtoms(XtDisplay(pdm_select_data->print_shell), atom_names,
		 XtNumber(atom_names), False, atoms);

    /* set back the video shell widget usable */
    XDestroyWindow(XtDisplay(pdm_select_data->transient_for_video_shell), 
		  pdm_select_data->transient_for_input_only_window);

    /* look at value and decide if success or failure */

    if (!value) {
	pr_cbs.reason = XmCR_PDM_NONE ;
	pr_cbs.detail = (XtPointer) pdm_select_data->pdm_selection ;
    } else {
	/* look if value is PDM_START_OK, or PDM_START_VXAUTH, etc */
	if (*(Atom *)value == atoms[XmAPDM_START_OK])
	    pr_cbs.reason = XmCR_PDM_UP ;
	else
        if (*(Atom *)value == atoms[XmAPDM_START_ERROR])
	    pr_cbs.reason = XmCR_PDM_START_ERROR ;
	else
        if (*(Atom *)value == atoms[XmAPDM_START_VXAUTH])
	    pr_cbs.reason = XmCR_PDM_START_VXAUTH ;
	else
        if (*(Atom *)value == atoms[XmAPDM_START_PXAUTH])
	    pr_cbs.reason = XmCR_PDM_START_PXAUTH ;

	/* if PDM is up, install a handler for phase 2 */
	if (pr_cbs.reason == XmCR_PDM_UP) {
	    XtAddEventHandler((Widget)pdm_select_data->print_shell, 
			      (EventMask) 0, 
			      True, /* ClientMessage are non maskable */
			      PDMPhase2Handler, NULL);
	}
    }
	
    XtCallCallbackList ((Widget)pdm_select_data->print_shell, 
			pdm_select_data->print_shell
			   ->print.pdm_notification_callback, 
			&pr_cbs);
	
    XtFree((char*)pdm_select_data);
}


XtEnum
XmPrintPopupPDM(Widget print_shell,
		Widget transient_for_video_shell)
{

    Atom pdm_selection;
    Atom type;
    unsigned char * value;
    int length;
    int format;
    Atom PDM_START ;
    Display * display_used ;
    PDMSelectData * pdm_select_data ;
    Widget widget_for_selection ;
    XtAppContext app;
    unsigned long old_timeout;

    /* get parameter for PDM_START from libXp 
       ask conversion using XtSetSelectionParameters, 
       and then call XtGetSelectionValue, which registers a
       XtSelectionCallbackProc that will wait for failure (no pdm
       owner, or timeout) or success.

       Phase 2 of the pdm (clientmessage) will be handled by a
       event handler set up from the selection callback on success */

    if (!XpGetPdmStartParams (XtDisplay(print_shell), 
			 XtWindow(print_shell), 
			 XpGetContext(XtDisplay(print_shell)), 
			 XtDisplay(transient_for_video_shell),
			 XtWindow(transient_for_video_shell), &display_used,
			 &pdm_selection, &type, &format, &value, &length))
	return XmPDM_NOTIFY_FAIL;

    /* only support XPDMDISPLAY = "print" or "video" */
    if (display_used == XtDisplay(print_shell))
	widget_for_selection = print_shell ;
    else 
    if (display_used == XtDisplay(transient_for_video_shell))
	widget_for_selection = transient_for_video_shell ;
    else 
	return XmPDM_NOTIFY_FAIL ;

    XtSetSelectionParameters(widget_for_selection, pdm_selection,
			     type, (XtPointer)value, length, format);

    XFree(value);

    pdm_select_data = (PDMSelectData *) XtMalloc(sizeof(PDMSelectData));
    pdm_select_data->pdm_selection = pdm_selection ;
    pdm_select_data->transient_for_video_shell = transient_for_video_shell ;
    pdm_select_data->print_shell = (XmPrintShellWidget) print_shell ; 
                                  /* need this one in all cases */

    PDM_START = XInternAtom(XtDisplay(widget_for_selection), 
			    XmIPDM_START, False);

    app = XtWidgetToApplicationContext(widget_for_selection);

/* twenty minutes */
#define REALLY_LONG_TIMEOUT (2 * 60 * 1000) 

    _XmAppLock(app);

    old_timeout = XtAppGetSelectionTimeout(app);

    XtAppSetSelectionTimeout(app, REALLY_LONG_TIMEOUT);

    XtGetSelectionValue(widget_for_selection, 
			pdm_selection,
			PDM_START, 
			PDMSelectionProc, 
			(XtPointer)pdm_select_data, 
			XtLastTimestampProcessed(
				       XtDisplay(widget_for_selection)));

    XtAppSetSelectionTimeout(app, old_timeout);

    _XmAppUnlock(app);

    /* put up a InputOnly window on top of the dialog,
       so that the end-user cannot  muck around with the print setup 
       dialog hile the PDM is  trying to come up. 
       This is removed in PDMSelectionProc */

    pdm_select_data->transient_for_input_only_window =
	XCreateWindow(XtDisplay(transient_for_video_shell),
		      XtWindow(transient_for_video_shell),
		      0, 0, 
		      XtWidth(transient_for_video_shell), 
		      XtHeight(transient_for_video_shell), 
		      0, CopyFromParent, InputOnly, CopyFromParent, 
		      0, NULL);
    XMapRaised(XtDisplay(transient_for_video_shell),
	       pdm_select_data->transient_for_input_only_window);

    return XmPDM_NOTIFY_SUCCESS ;
}


/************************************************************************
 *
 *  XmRedisplayWidget
 *
 *    Call the expose method of the passed widget with a fake
 *    event corresponding to its entire area.
 *
 ************************************************************************/

void 
XmRedisplayWidget(Widget widget) 
{
    XExposeEvent xev ;
    Region region ;

    xev.type = Expose ;
                 /* is this better than 0 ? shouldn't make much difference
		  unless the expose method is very tricky... */
    xev.serial = LastKnownRequestProcessed(XtDisplay(widget)) ;  
    xev.send_event = False ;
    xev.display = XtDisplay(widget);
    xev.window = XtWindowOfObject(widget);  /* work with gadget too */
    xev.x = 0 ;
    xev.y = 0 ;
    xev.width = widget->core.width ;
    xev.height = widget->core.height ;
    xev.count = 0 ;

    region = XCreateRegion();
    XtAddExposureToRegion((XEvent*)&xev, region);    

    if (widget->core.widget_class->core_class.expose)
	(*(widget->core.widget_class->core_class.expose))
	    (widget, (XEvent*)&xev, region);

    XDestroyRegion(region);
}
