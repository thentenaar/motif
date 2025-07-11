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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: icccm.c /main/7 1995/07/13 17:57:34 drk $"
#endif
#endif
#include <stdio.h>
#include <X11/Xatom.h>
#include <Mrm/MrmAppl.h>

#include <testlib.h>

static void targets_proc();
static void formats_proc();
static void multiple_proc();
static void timestamp_proc();
static void owner_proc();
static void quit_proc();

static MrmHierarchy mrm_id;
static char *mrm_vec[]={"icccm.uid"};
static MrmCode mrm_class;

static MrmCount regnum = 6;
static MRMRegisterArg regvec[] = {
	{"targets_proc", (caddr_t)targets_proc},
	{"formats_proc", (caddr_t)formats_proc},
	{"multiple_proc", (caddr_t)multiple_proc},
	{"timestamp_proc", (caddr_t)timestamp_proc},
	{"owner_proc", (caddr_t)owner_proc},
	{"quit_proc", (caddr_t)quit_proc}
	};

static void selection_handler();
static void print_sel_event();
static void print_atom();
static void handle_notify();
static void handle_request();
static void handle_clear();
static char *get_window_property();
static void init_multiple_prop();
static void print_targets_notify();
static void print_timestamp_notify();
static void print_multiple_notify();
static void print_generic_notify();
static void print_string_notify();
static int protocol_error_handler();
static Atom get_request_atom();
static void free_request_atom();

static Widget app_main;

static Window window;

static Atom xa_CLIPBOARD;
static Atom xa_MULTIPLE;
static Atom xa_TIMESTAMP;
static Atom xa_TARGETS;

static Atom owner_targets[2];
static int owner_ntargets;

static char owner_string[] = "ICCCM Clipboard Text";
static long owner_integer = 911;

static int ntargets = 0;
static Atom *target_atoms = NULL;

main(argc, argv)
    int argc;
    char **argv;
{
    MrmInitialize ();

   /*  initialize toolkit  */
    CommonTestInit(argc, argv);
    
    if (MrmOpenHierarchy (1, mrm_vec, NULL, &mrm_id) != MrmSUCCESS) exit(0);
    if (MrmRegisterNames (regvec, regnum) != MrmSUCCESS) exit(0);
    if (MrmFetchWidget (mrm_id, "app_main", Shell1, &app_main, &mrm_class)
						!= MrmSUCCESS) exit(0);
    XtManageChild(app_main);
    XtRealizeWidget(Shell1);
    window = XtWindow(Shell1);
    xa_CLIPBOARD = XInternAtom(display,"CLIPBOARD", False);
    xa_MULTIPLE = XInternAtom(display,"MULTIPLE", False);
    xa_TIMESTAMP= XInternAtom(display,"TIMESTAMP", False);
    xa_TARGETS = XInternAtom(display,"TARGETS", False);
    owner_targets[0] = XA_STRING;
    owner_targets[1] = XA_INTEGER;
    owner_ntargets = 2;

    /* Set an error handler for debugging purposes */
    XSetErrorHandler((XErrorHandler) protocol_error_handler);

    XtAddEventHandler(Shell1, 0, True, selection_handler, NULL);
    XtAppMainLoop(app_context);
}

/*********************************************************************
 *
 * Selection handler and utilities
 *
 ********************************************************************/

static void selection_handler (w, client_data, event, cont)
    Widget w;
    caddr_t client_data;
    XEvent *event;
    Boolean *cont;
{
    switch(event->xany.type) {
	case(SelectionNotify):
		print_sel_event(event);
		handle_notify(event);
		break;
	case(SelectionRequest):
		print_sel_event(event);
		handle_request(event);
		break;
	case(SelectionClear):
		print_sel_event(event);
		handle_clear(event);
		break;
    }
    return;
}

static void print_sel_event(ev)
    XEvent *ev;
{
    Display *dpy = ev->xany.display;

    switch (ev->type) {
	case (SelectionClear) :
	    printf ("SelectionClear(");
	    print_atom(dpy, ev->xselectionclear.selection); printf (")\n");
	    break;
	case (SelectionNotify) :
	    printf ("SelectionNotify(");
	    print_atom(dpy, ev->xselection.selection); printf (",");
	    print_atom(dpy, ev->xselection.target); printf (",");
	    print_atom(dpy, ev->xselection.property); printf (")\n");
	    break;
	case (SelectionRequest) :
	    printf ("SelectionRequest(");
	    print_atom(dpy, ev->xselectionrequest.selection); printf (",");
	    print_atom(dpy, ev->xselectionrequest.target); printf (",");
	    print_atom(dpy, ev->xselectionrequest.property); printf (")\n");
	    break;
	default :
	    printf ("print_sel_event : Not a selection event.\n");
	    break;
    }
}

static void print_atom(dpy, atom)
    Display *dpy;
    Atom atom;
{
    char *str;

    if (atom != None) {
	char *str;
	printf ("%s", str=XGetAtomName(dpy,atom));
	XFree(str);
    }
    else {
	printf ("None");
    }
}

/*****************************************
 *
 * SelectionNotify handler and utilities
 *
 *****************************************/

static void print_targets_notify(target, property)
    Atom target;
    Atom property;
{
    Atom *data;
    int nitems;
    int i;

    data = (Atom *)get_window_property(property, &nitems);
    if (nitems > 0) {
	printf ("  TARGETS:\n");
	for (i=0; i<nitems; i++) {
	    printf("    ");print_atom(display, data[i]); printf ("\n");
		
	}
	ntargets = nitems;
	if (target_atoms != NULL) XFree(target_atoms);
	target_atoms = data;
    }
    else {
	printf ("  No TARGETS data\n");
    }
}

static void print_timestamp_notify(target, property)
    Atom target;
    Atom property;
{
    unsigned long *data;
    int nitems;
    int i;

    data = (unsigned long *)get_window_property(property, &nitems);
    if (nitems > 0) {
	printf ("  TIMESTAMP: %u\n",*data);
	XFree(data);
    }
    else {
	printf ("  No TIMESTAMP data\n");
    }
}

static void print_multiple_notify(target, property)
    Atom target;
    Atom property;
{
    Atom *data;
    int nitems;
    int i;

    data = (Atom *)get_window_property(property, &nitems);
    if (nitems > 0) {
	printf ("  MULTIPLE:\n");
	for (i=0; i<nitems; i+=2) {
	    printf("    ");print_atom(display, data[i]);
	    printf (" = ");print_atom(display, data[i+1]);
	    printf ("\n");
	    if (data[i+1] != None) print_generic_notify(data[i],data[i+1]);		
	}
	XFree(data);
    }
    else {
	printf ("  No MULTIPLE data\n");
    }
}
static void print_string_notify(target, property)
    Atom target;
    Atom property;
{
    char *data;
    int nitems;

    data = get_window_property(property, &nitems);
    if (nitems > 0) {
	printf ("  STRING: \"%s\"\n",data);
	XFree(data);
    }
    else {
	printf ("  No STRING data\n");
    }
}
static void print_generic_notify(target, property)
    Atom target;
    Atom property;
{
    char *data;
    int nitems;

    data = get_window_property(property, &nitems);
    if (nitems > 0) {
	printf ("  "); print_atom(display, target);
	printf (" : %d items\n", nitems);
	XFree(data);
    }
    else {
	printf ("  No "); print_atom(display, target);
	printf (" data\n");
    }
}

static void convert_selection(target,time)
    Atom target;
    Time time;
{
  Atom prop = get_request_atom();

  if (target==xa_MULTIPLE) init_multiple_prop(prop);
  XConvertSelection(display, xa_CLIPBOARD, target, prop, window, time);
}

static void init_multiple_prop(property)
    Atom property;
{
    int i;
    Atom *data;
    char *retprop;
    char *prop;

    if (ntargets==0) return;
    data = (Atom *)XtMalloc(ntargets*2*sizeof(Atom));
    for (i=0; i<ntargets; i++) {
	data[2*i] = target_atoms[i];
	prop = XGetAtomName(display, target_atoms[i]);
	retprop = XtMalloc (strlen(prop)+8+1);
	strcpy(retprop, prop);
	strcat(retprop,"_RETPROP");
	data[2*i+1] = XInternAtom(display,retprop,False);
	XtFree(retprop);
	XFree(prop);
    }
    XChangeProperty(display, window, property, XA_ATOM, 32,
	PropModeReplace, (unsigned char *)data, ntargets*2);
    XtFree((char *) data); 
}

static char *get_window_property(property, nitems)
    Atom property;
    int *nitems;
{
    int status;
    Atom actual_type;
    int actual_format;
    unsigned long items;
    unsigned long bytes_after;
    unsigned char *data;

    free_request_atom(property);

    if (property == None) {
	*nitems = 0;
	return (NULL);
    }
    status = XGetWindowProperty(display, window, property, 0, 9999,
			True, AnyPropertyType, &actual_type, &actual_format,
			&items, &bytes_after, &data);
    if (status != Success) {
	printf("XGetWindowProperty failed\n");
	*nitems = 0;
	return(NULL);
    }
    *nitems = items;
    return((char *) data);			
}


static void handle_notify(event)
    XSelectionEvent *event;
{
    if (event->selection != xa_CLIPBOARD) return;

    if (event->target == xa_TARGETS)
	print_targets_notify(event->target,event->property);
    else if (event->target == XA_STRING)
	print_string_notify(event->target,event->property);
    else if (event->target == xa_MULTIPLE)
	print_multiple_notify(event->target,event->property);
    else if (event->target == xa_TIMESTAMP)
	print_timestamp_notify(event->target,event->property);
    else
	print_generic_notify(event->target,event->property);
}

/*****************************************
 *
 * SelectionRequest handler and utilities
 *
 *****************************************/

static void change_window_property(property, req, data, type, format, nitems)
    Atom property;
    Window req;
    unsigned char *data;
    Atom type;
    int format;
    int nitems;
{
    XChangeProperty(display, req, property, type, format, PropModeReplace,
		    data, nitems);
}

static void handle_request(event)
    XSelectionRequestEvent *event;
{
    XSelectionEvent se;

    if (event->selection != xa_CLIPBOARD) return;
    if (event->time == CurrentTime)
	printf ("icccm: recieved SelectionRequest(CurrentTime)\n");
    se.type = SelectionNotify;
    se.display = event->display;
    se.requestor = event->requestor;
    se.selection = event->selection;
    se.target = event->target;
    if (event->property != None)
	se.property = event->property;
    else
	se.property = event->target;
    se.time = event->time;

    if (event->target == xa_TARGETS) {
	change_window_property(se.property, event->requestor,
		(char *)owner_targets, XA_ATOM, 32, owner_ntargets);
    }
    else if (event->target == XA_STRING) {
	change_window_property(se.property, event->requestor,
		owner_string, XA_STRING, 8, strlen(owner_string));
    }
    else if (event->target == XA_INTEGER) {
	change_window_property(se.property, event->requestor,
		(char*)&owner_integer, XA_INTEGER, 32, 1);
    }
    else {
	se.property=None;
    }
    XSendEvent(event->display, event->requestor, False, 
	       (EventMask)0, (XEvent *) &se);
}

/*****************************************
 *
 * SelectionClear handler and utilities
 *
 *****************************************/

static void handle_clear(event)
    XSelectionClearEvent *event;
{
    if (event->selection != xa_CLIPBOARD) return;
}

/*********************************************************************
 *
 * Pushbutton action routines
 *
 ********************************************************************/

static void targets_proc(widget, tag, cb)
	Widget widget;
	caddr_t *tag;
	XmAnyCallbackStruct *cb;
{
    XButtonEvent *bev = (XButtonEvent *)(cb->event);

    convert_selection(xa_TARGETS, bev->time);
}

static void formats_proc(widget, tag, cb)
	Widget widget;
	caddr_t *tag;
	XmAnyCallbackStruct *cb;
{
    XButtonEvent *bev = (XButtonEvent *)(cb->event);
    int i;

    for (i=0; i<ntargets; i++) {
	convert_selection(target_atoms[i], bev->time);
    }
}

static void multiple_proc(widget, tag, cb)
	Widget widget;
	caddr_t *tag;
	XmAnyCallbackStruct *cb;
{
    XButtonEvent *bev = (XButtonEvent *)(cb->event);

    convert_selection(xa_MULTIPLE, bev->time);
}

static void timestamp_proc(widget, tag, cb)
	Widget widget;
	caddr_t *tag;
	XmAnyCallbackStruct *cb;
{
    XButtonEvent *bev = (XButtonEvent *)(cb->event);

    convert_selection(xa_TIMESTAMP, bev->time);
}

#define RETRY_LIMIT 5
static void owner_proc(widget, tag, cb)
	Widget widget;
	caddr_t *tag;
	XmAnyCallbackStruct *cb;
{
    XButtonEvent *bev = (XButtonEvent *)(cb->event);
    int i;

    for (i=0; i < RETRY_LIMIT; i++) {
	XSetSelectionOwner (display, xa_CLIPBOARD, window, bev->time);
	if (XGetSelectionOwner(display, xa_CLIPBOARD)==window) {
	    printf ("XSetSelectionOwner(CLIPBOARD)\n");
	    break;
	}
    }
}
#undef RETRY_LIMIT

static void quit_proc(widget, tag, cb)
	Widget widget;
	caddr_t *tag;
	XmAnyCallbackStruct *cb;
{
    exit(0);
}


/* This error handler prints the error information and then simply 
   continues */
static int protocol_error_handler(display, event)
     Display *display;
     XErrorEvent *event;
{
  char errortext[100];

  XGetErrorText(display, event -> error_code, errortext, 100);

  printf("X Protocol error: %s\nXID %d serial %d major %d minor %d\ncontinuing...",
	 errortext,
	 event -> resourceid,
	 event -> request_code,
	 event -> minor_code);
  return(0);
}

/* Routines to allocate and free request atoms.  Allow for 100 of
   the buggers.  We'll name them _MOTIF_ATOM_%d,  which will
   reuse some of the toolkit atoms */

Boolean atoms_inited = False;
Boolean in_use[100];
Atom atoms[100];

static Atom get_request_atom() 
{
  int i;

  if (atoms_inited == False) {
    for(i = 0; i < 100; i++) {
      in_use[i] = False;
      atoms[i] = None;
    }
    atoms_inited = True;
  }

  /* Find the first not in use and return it */
  i = 0;
  while(i < 100 && in_use[i]) i++;
  
  if (i >= 100) return(None);

  if (atoms[i] == 0) {
    char buffer[32];
    sprintf(buffer, "_MOTIF_ATOM_%d",i);
    atoms[i] = XInternAtom(display, buffer, False);
  }

  in_use[i] = True;

  return(atoms[i]);
}

static void free_request_atom(a)
     Atom a;
{
  int i;

  i = 0;

  /* Find the atom and reset the bit */
  while(i < 100 && atoms[i] != a) i++;

  if (i < 100)
    in_use[i] = False;
}
