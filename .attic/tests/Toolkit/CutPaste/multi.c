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
static char rcsid[] = "$XConsortium: multi.c /main/7 1995/07/13 17:58:11 drk $"
#endif
#endif

#include <stdio.h>
#include <X11/Xatom.h>
#include <Mrm/MrmAppl.h>

#include <testlib.h>

#define MIN_VALUE 0
#define MAX_VALUE 100
#define MAX_LOG_SIZE 1000

#define K_popup_id 1
#define K_log_id 2
#define K_form_id 3
#define K_text_value_id 4
#define K_text_select_id 5
#define K_pixmap_value_id 6
#define K_pixmap_select_id 7
#define K_int_value_id 8
#define K_int_select_id 9
#define K_slider_id 10

static void create_proc();
static void slider_value_change();
static void cut_proc();
static void byname_proc();
static void paste_proc();
static void undo_proc();
static void inquire_proc();
static void lock_proc();
static void unlock_proc();
static void quit_proc();

static void init_pixmap();
static void init_values ();
static int get_toggle_state();
static void add_to_log();
static void clear_log();
static void set_slider();
static void set_text();
static void set_pixmap();
static void set_int();
static void init_byname();
static struct _byname *add_byname();
static void del_byname();
static struct _byname *get_byname();
static void byname_cb();
static void post_popup();
static void really_quit();

static MrmHierarchy mrm_id;
static char *mrm_vec[]={"multi.uid"};
static MrmCode mrm_class;

static MrmCount regnum = 10;
static MrmRegisterArg regvec[] = {
	{"create_proc", (caddr_t) create_proc},
	{"slider_value_change", (caddr_t) slider_value_change},
	{"cut_proc", (caddr_t) cut_proc},
	{"byname_proc", (caddr_t) byname_proc},
	{"paste_proc", (caddr_t) paste_proc},
	{"undo_proc", (caddr_t) undo_proc},
	{"inquire_proc", (caddr_t) inquire_proc},
	{"lock_proc", (caddr_t) lock_proc},
	{"unlock_proc", (caddr_t) unlock_proc},
	{"quit_proc", (caddr_t) quit_proc}
	};

static Widget app_main;
static Widget form;
static Widget log;
static Widget popup;
static Widget text_value;
static Widget text_select;
static Widget pixmap_value;
static Widget pixmap_select;
static Widget int_value;
static Widget int_select;
static Widget slider;

static Window win;
static char text_data[100];
static unsigned long int_data;
static Pixmap pixmap_id;
static Dimension pixmap_width;
static Dimension pixmap_height;
static GC pixmap_gc;
static XmString clip_cs;

struct _byname {
    int id;
    char *data;
    unsigned long length;
    struct _byname *prev;
    struct _byname *next;
};

static struct _byname *bdata = NULL;

int main(argc, argv)
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
    XFlush(display);
    win = XtWindow(log);

    init_pixmap();
    init_values(50);
    clear_log();
    XmClipboardRegisterFormat(display, "INTEGER", 32);
    clip_cs = XmStringCreateLtoR ("XmClipboard-Test", XmSTRING_DEFAULT_CHARSET);
    XtAppMainLoop(app_context);
}

static void post_popup (w, pw, event, c)
    Widget         w;
    XtPointer      pw;
    XEvent *  event;
    Boolean * c;
{
    if (((XButtonEvent *) event)->button != Button3) return;
    XmMenuPosition((Widget) pw, (XButtonEvent *)event);
    XtManageChild ((Widget) pw);
}

static void create_proc(w, tag, reason)
    Widget w;
    int *tag;
    unsigned long *reason;
{
    switch (*tag) {
        case K_form_id: form = w; XmAddTabGroup(w); break;
        case K_log_id: log = w; break;
        case K_text_value_id: text_value = w; break;
        case K_text_select_id: text_select = w; break;
        case K_pixmap_value_id: pixmap_value = w; break;
        case K_pixmap_select_id: pixmap_select = w; break;
        case K_int_value_id: int_value = w; break;
        case K_int_select_id: int_select = w; break;
        case K_slider_id: slider = w; XmAddTabGroup(w); break;
        case K_popup_id:
            popup = w;
            XtAddEventHandler (form, ButtonPressMask, False, post_popup, w);
            break;
    }
}

static void init_pixmap()
{
    Arg arglist[2];
    unsigned long vmask;
    XGCValues vinfo;

    XtSetArg (arglist[0], XmNwidth, &pixmap_width);
    XtSetArg (arglist[1], XmNheight, &pixmap_height);
    XtGetValues (pixmap_value, arglist, 2);
    pixmap_id = XCreatePixmap (display, win, pixmap_width, pixmap_height,
				DefaultDepth(display, DefaultScreen(display)));
    vinfo.function = GXcopy;
    vinfo.fill_style = FillSolid;
    vmask = GCFunction | GCFillStyle;
    pixmap_gc = XCreateGC (display, win, vmask, &vinfo);
}
/***********************************************************************
 *
 * Utility routines
 *
 ***********************************************************************/

static int get_toggle_state (text_ret, int_ret, pixmap_ret)
    int *text_ret;
    int *int_ret;
    int *pixmap_ret;
{
    int any_selected = False;

    *text_ret = False;
    *int_ret = False;
    *pixmap_ret = False;

    if (XmToggleButtonGadgetGetState(text_select)==True) {
	*text_ret = True;
	any_selected = True;
    }
    if (XmToggleButtonGadgetGetState(int_select)==True) {
	*int_ret = True;
	any_selected = True;
    }
    if (XmToggleButtonGadgetGetState(pixmap_select)==True) {
	*pixmap_ret = True;
	any_selected = True;
    }
    return(any_selected);
}

static log_size = 0;
static void add_to_log(str)
    char *str;
{
    int len = strlen(str);

    if (log_size+len < MAX_LOG_SIZE) {
	XmTextReplace (log, log_size, log_size, str);
	log_size += len;
    }
    else {
	XmTextSetString (log, str);
	log_size=len;
    }
}
static void clear_log()
{
    XmTextSetString (log, NULL);
    log_size=0;
}

/***********************************************************************
 *
 * Selection value routines
 *
 **********************************************************************/
static void init_values(val)
    int val;
{
    set_slider(val);
    set_text(val);
    set_pixmap(val);
    set_int(val);
}

static void slider_value_change(widget, tag, cb)
	Widget widget;
	XtPointer *tag;
	XmScaleCallbackStruct *cb;
{
    set_text (cb->value);
    set_pixmap (cb->value);
    set_int (cb->value);
}

static void set_slider(val)
    int val;
{
    if (val < MIN_VALUE || val > MAX_VALUE) return;
    XmScaleSetValue (slider, val);    
}

static void set_text(val)
    int val;
{
    XmString cs;
    Arg arglist[1];

    if (val < MIN_VALUE || val > MAX_VALUE) return;
    sprintf (text_data, "The value is %d percent", val);
    cs = XmStringCreateLtoR (text_data, XmSTRING_DEFAULT_CHARSET); 
    XtSetArg (arglist[0], XmNlabelString, cs);
    XtSetValues (text_value, arglist, 1);
    XmStringFree (cs);
}

static void set_pixmap(val)
    int val;
{
    Arg arglist[1];
    if (val < MIN_VALUE || val > MAX_VALUE) return;
    XSetForeground (display, pixmap_gc, WhitePixel(display, DefaultScreen(display)));  
    XFillRectangle (display, pixmap_id, pixmap_gc, 0, 0, pixmap_width,
							pixmap_height);
    if (val > 0) {
	XSetForeground (display, pixmap_gc, BlackPixel(display, DefaultScreen(display)));
 	XFillRectangle (display, pixmap_id, pixmap_gc, 1, pixmap_height/4,
			(val*pixmap_width)/100,	pixmap_height/2);
    }   
    /* Ensure local pixmap is displayed */
    XtSetArg (arglist[0], XmNlabelPixmap, pixmap_id);
    XtSetValues (pixmap_value, arglist, 1);
    /* Force the label to redraw */
    XClearArea (display, XtWindow(pixmap_value), 0, 0, 0, 0, True);
}

static void set_int(val)
    int val;
{
    char str[20];
    XmString cs;
    Arg arglist[1];

    if (val < MIN_VALUE || val > MAX_VALUE) return;
    int_data = val;
    sprintf (str, "%d", val);
    cs = XmStringCreateLtoR (str, XmSTRING_DEFAULT_CHARSET); 
    XtSetArg (arglist[0], XmNlabelString, cs);
    XtSetValues (int_value, arglist, 1);
    XmStringFree(cs);
}

/***********************************************************************
 *
 * Clipboard Actions, called from Popup
 *
 **********************************************************************/

static void quit_proc(widget, tag, cb)
	Widget widget;
	XtPointer *tag;
	XmAnyCallbackStruct *cb;
{
    XmClipboardPendingList items;
    unsigned long nitems;
    int i;
    struct _byname *b;
    Boolean any_pending = False;

#ifdef DEBUG
    printf ("\n    QUIT\n{\n");
#endif /* DEBUG */
    clear_log();
    if (XmClipboardInquirePendingItems (display, win, "STRING", &items, &nitems)
		== ClipboardSuccess) {
	if (nitems > 0)
	    for (i = 0; i < nitems; i++) {
		b = get_byname(items[i].DataId);
		if (b != NULL) {
		    add_to_log("Copied Pending STRING\n");
		    any_pending = True;
		    XmClipboardCopyByName(display, win, b->id, b->data,
					b->length, 0L);
		    del_byname(items[i].DataId);
		}
	    }
    }
    if (XmClipboardInquirePendingItems (display, win, "INTEGER", &items, &nitems)
		== ClipboardSuccess) {
	if (nitems > 0)
	    for (i = 0; i < nitems; i++) {
		b = get_byname(items[i].DataId);
		if (b != NULL) {
		    add_to_log("Copied Pending INTEGER\n");
		    any_pending = True;
		    XmClipboardCopyByName(display, win, b->id, b->data,
					b->length, 0L);
		    del_byname(items[i].DataId);
		}
	    }
    }
    if (XmClipboardInquirePendingItems (display, win, "PIXMAP", &items, &nitems)
		== ClipboardSuccess) {
	if (nitems > 0)
	    for (i = 0; i < nitems; i++) {
		b = get_byname(items[i].DataId);
		if (b != NULL) {
		    add_to_log("Copied Pending PIXMAP\n");
		    any_pending = True;
		    XmClipboardCopyByName(display, win, b->id, b->data,
					  b->length, 0L);
		    del_byname(items[i].DataId);
		}
	    }
    }
#ifdef DEBUG
    printf ("}\n");
#endif /* DEBUG */

    if (any_pending == True) {
	add_to_log ("Exit in 5 seconds...");
	XtAddTimeOut (5000, really_quit, NULL);
	XFlush(display);
    }
    else {
        XFlush(display);
	exit(0);
    }
}

static void really_quit(client_data, timer)
    XtPointer client_data;
    XtIntervalId timer;
{
    exit(0);
}

static void cut_proc(widget, tag, cb)
	Widget widget;
	XtPointer *tag;
	XmAnyCallbackStruct *cb;
{
    long item_id;
    XButtonEvent *bev = (XButtonEvent *)cb->event;
    int use_text;
    int use_int;
    int use_pixmap;

#ifdef DEBUG
    printf ("\n    CUT\n{\n");
#endif /* DEBUG */
    clear_log();
    if (get_toggle_state (&use_text, &use_int, &use_pixmap)==False)
	return;
    set_text(int_data);
    set_pixmap(int_data);
    set_int(int_data);
    if (XmClipboardStartCopy (display, win, clip_cs, bev->time, Shell1,
			NULL, &item_id) != ClipboardSuccess) {
	add_to_log ("StartCopy failed\n");
	XBell(display, 0);
	return;
    }
    if (use_text == True) {
	if (XmClipboardCopy (display, win, item_id, "STRING", text_data,
	    strlen(text_data), 0L, NULL) != ClipboardSuccess) {
	    XmClipboardCancelCopy(display, win, item_id);
	    add_to_log ("Copy STRING failed\n");
	    XBell(display, 0);
	}
	else {
	    add_to_log("Copied STRING\n");
	}
    }
    if (use_int == True) {
	if (XmClipboardCopy (display, win, item_id, "INTEGER", (char *)&int_data,
	    sizeof(long), 0L, NULL) != ClipboardSuccess) {
	    XmClipboardCancelCopy(display, win, item_id);
	    add_to_log ("Copy INTEGER failed\n");
	    XBell(display, 0);
	    return;
	}
	else {
	    add_to_log("Copied INTEGER\n");
	}
    }
    if (use_pixmap == True) {
	if (XmClipboardCopy (display, win, item_id, "PIXMAP", (char *)&pixmap_id,
	    sizeof(Pixmap), 0L, NULL) != ClipboardSuccess) {
	    XmClipboardCancelCopy(display, win, item_id);
	    add_to_log ("Copy PIXMAP failed\n");
	    XBell(display, 0);
	    return;
	}
	else {
	    add_to_log("Copied PIXMAP\n");
	}
    }
    if (XmClipboardEndCopy (display, win, item_id) != ClipboardSuccess) {
	XmClipboardCancelCopy(display, win, item_id);
	add_to_log ("EndCopy failed\n");
	XBell(display, 0);
	return;
    }
#ifdef DEBUG
    printf ("}\n");
#endif /* DEBUG */
    return;
}
static void byname_proc(widget, tag, cb)
	Widget widget;
	XtPointer *tag;
	XmAnyCallbackStruct *cb;
{
    long item_id;
    XButtonEvent *bev = (XButtonEvent *)cb->event;
    int use_text;
    int use_int;
    int use_pixmap;
    long data_id;
    struct _byname *b;

#ifdef DEBUG
    printf ("\n    CUT BY NAME\n{\n");
#endif /* DEBUG */
    clear_log();
    if (bdata == NULL) init_byname();
    if (get_toggle_state (&use_text, &use_int, &use_pixmap)==False)
	return;
    set_text(int_data);
    set_pixmap(int_data);
    set_int(int_data);

    if (XmClipboardStartCopy (display, win, clip_cs, bev->time, Shell1,
			byname_cb, &item_id) != ClipboardSuccess) {
	add_to_log ("StartCopy failed(by name)\n");
	XBell(display, 0);
	return;
    }
    if (use_text == True) {
	if (XmClipboardCopy (display, win, item_id, "STRING", NULL,
	    strlen(text_data), 0L, &data_id) != ClipboardSuccess) {
	    XmClipboardCancelCopy(display, win, item_id);
	    add_to_log ("Copy STRING failed(by name)\n");
	    XBell(display, 0);
	}
	else {
	    add_to_log("Copied STRING(by name)\n");
	    b=add_byname();
	    b->id = data_id;
	    b->length = strlen(text_data);
	    b->data = XtMalloc(b->length+1); /* for string + terminating NUL */
	    strcpy (b->data, text_data);
	}
    }
    if (use_int == True) {
	if (XmClipboardCopy (display, win, item_id, "INTEGER", NULL,
	    sizeof(long), 0L, &data_id) != ClipboardSuccess) {
	    XmClipboardCancelCopy(display, win, item_id);
	    add_to_log ("Copy INTEGER failed(by name)\n");
	    XBell(display, 0);
	    return;
	}
	else {
	    add_to_log("Copied INTEGER(by name)\n");
	    b=add_byname();
	    b->id = data_id;
	    b->length = sizeof(long);
	    b->data = XtMalloc(b->length);
	    *(long*)b->data = int_data;
	}
    }
    if (use_pixmap == True) {
	if (XmClipboardCopy (display, win, item_id, "PIXMAP", NULL,
	    sizeof(Pixmap), 0L, &data_id) != ClipboardSuccess) {
	    XmClipboardCancelCopy(display, win, item_id);
	    add_to_log ("Copy PIXMAP failed(by name)\n");
	    XBell(display, 0);
	    return;
	}
	else {
	    add_to_log("Copied PIXMAP(by name)\n");
	    b=add_byname();
	    b->id = data_id;
	    b->length = sizeof(Pixmap);
	    b->data = XtMalloc(b->length);
	    *(Pixmap*)b->data = pixmap_id;
	}
    }
    if (XmClipboardEndCopy (display, win, item_id) != ClipboardSuccess) {
	XmClipboardCancelCopy(display, win, item_id);
	add_to_log ("EndCopy failed(by name)\n");
	XBell(display, 0);
	return;
    }
#ifdef DEBUG
    printf ("}\n");
#endif /* DEBUG */
    return;
}

static void init_byname()
{
    bdata = (struct _byname *)XtMalloc(sizeof(struct _byname));
    bdata->prev = NULL;
    bdata->next = NULL;
    bdata->id = 0;
    bdata->length = 0;
    bdata->data = NULL;
}
static struct _byname *add_byname()
{
    struct _byname *b;

    b = (struct _byname *)XtMalloc(sizeof(struct _byname));
    b->prev = NULL;
    b->next = bdata;
    bdata->prev = b;
    bdata = b;
    return(b);
}
static void del_byname(id)
    int id;
{
    struct _byname *b;

    b = bdata;
    while (b->next != NULL) {
	if (b->id == id) {
	    (b->next)->prev = b->prev;
	    if (b->prev != NULL) (b->prev)->next = b->next;
	    if (b->data != NULL) XtFree (b->data);
	    if (b == bdata) bdata = b->next;
	    XtFree((char *)b);
	    break; /* assume id's are unique */
	}
	b = b->next;
    }
}
static struct _byname *get_byname(id)
    int id;
{
    struct _byname *b;

    b = bdata;
    while (b->next != NULL) {
	if (b->id == id) return (b);
	b = b->next;
    }
    return (NULL);
}


static void paste_proc(widget, tag, cb)
	Widget widget;
	XtPointer *tag;
	XmAnyCallbackStruct *cb;
{
    XButtonEvent *bev = (XButtonEvent *)cb->event;
    unsigned long length;
    char *buffer;
    unsigned long paste_length;
    int use_text;
    int use_int;
    int use_pixmap;
    XmString cs;
    Arg arglist[1];
    char str[20];

#ifdef DEBUG
    printf ("\n    PASTE\n{\n");
#endif /* DEBUG */
    clear_log();
    if (get_toggle_state (&use_text, &use_int, &use_pixmap)==False)
	return;
    if (XmClipboardStartRetrieve(display, win, bev->time) != ClipboardSuccess) {
	add_to_log ("StartRetrieve failed\n");
	XBell(display, 0);
	return;
    }
    if (use_text == True) {
	if(XmClipboardInquireLength(display, win, "STRING", &length)
					!=ClipboardSuccess) {
	    add_to_log("InquireLength on STRING failed\n");
	    XBell(display, 0);
	}
	else if (length > 0) {
	    buffer = (char *)XtMalloc((length+1) * sizeof(char));
	    if (XmClipboardRetrieve(display, win, "STRING", buffer, length,
			&paste_length, NULL) !=ClipboardSuccess) {
		add_to_log("Retrieve STRING failed\n");
		XtFree(buffer);
		XBell(display, 0);
	    }
	    else {
		add_to_log ("Retrieved STRING\n");
		buffer[paste_length] = '\0';
		cs = XmStringCreateLtoR(buffer,XmSTRING_DEFAULT_CHARSET); 
		XtSetArg (arglist[0], XmNlabelString, cs);
		XtSetValues (text_value, arglist, 1);
		XmStringFree(cs);
		XtFree(buffer);
	    }
	}
	else {
	    add_to_log ("Retrieve - No STRING data\n");
	    XBell(display, 0);
	}
    }
    if (use_int == True) {
	if(XmClipboardInquireLength(display, win, "INTEGER", &length)
					!=ClipboardSuccess) {
	    add_to_log("InquireLength on INTEGER failed\n");
	    XBell(display, 0);
	}
	else if (length > 0) {
	    buffer = (char *)XtMalloc(length);
	    if (XmClipboardRetrieve(display, win, "INTEGER", buffer, length,
			&paste_length, NULL) !=ClipboardSuccess) {
		add_to_log("Retrieve INTEGER failed\n");
		XtFree(buffer);
		XBell(display, 0);
	    }
	    else {
		add_to_log ("Retrieved INTEGER\n");
		sprintf (str, "%d", (int)*((long *)buffer));
		cs = XmStringCreateLtoR (str, XmSTRING_DEFAULT_CHARSET); 
		XtSetArg (arglist[0], XmNlabelString, cs);
		XtSetValues (int_value, arglist, 1);
		XmStringFree(cs);
		XtFree(buffer);
	    }
	}
	else {
	    add_to_log ("Retrieve - No INTEGER data\n");
	    XBell(display, 0);
	}
    }
    if (use_pixmap == True) {
	if(XmClipboardInquireLength(display, win, "PIXMAP", &length)
					!=ClipboardSuccess) {
	    add_to_log("InquireLength on PIXMAP failed\n");
	    XBell(display, 0);
	}
	else if (length > 0) {
	    buffer = (char *)XtMalloc(length);
	    if (XmClipboardRetrieve(display, win, "PIXMAP", buffer, length,
			&paste_length, NULL) !=ClipboardSuccess) {
		add_to_log("Retrieve PIXMAP failed\n");
		XtFree(buffer);
		XBell(display, 0);
	    }
	    else {
		add_to_log ("Retrieved PIXMAP\n");
		XtSetArg (arglist[0], XmNlabelPixmap, *((Pixmap *)buffer));
		XtSetValues (pixmap_value, arglist, 1);
		XClearArea (display, XtWindow(pixmap_value), 0, 0, 0, 0, True);
		XtFree(buffer);
	    }
	}
	else {
	    add_to_log ("Retrieve - No PIXMAP data\n");
	    XBell(display, 0);
	}
    }
    if (XmClipboardEndRetrieve(display, win) !=ClipboardSuccess) {
	add_to_log ("EndRetrieve failed\n");
	XBell(display, 0);
    }
#ifdef DEBUG
    printf ("}\n");
#endif /* DEBUG */
}

static void undo_proc(widget, tag, cb)
	Widget widget;
	XtPointer *tag;
	XmAnyCallbackStruct *cb;
{
#ifdef DEBUG
    printf ("\n    UNDO\n{\n");
#endif /* DEBUG */
    clear_log();
    if (XmClipboardUndoCopy (display, win) !=ClipboardSuccess) {
	add_to_log ("UNDO failed\n");
	XBell(display, 0);
	return;
    }
    add_to_log("UNDO complete\n");
#ifdef DEBUG
    printf ("}\n");
#endif /* DEBUG */
}
static void inquire_proc(widget, tag, cb)
	Widget widget;
	XtPointer *tag;
	XmAnyCallbackStruct *cb;
{
    int count;
    unsigned long maxlen;
    int i;
    char *buf;
    unsigned long retlen;

#ifdef DEBUG
    printf ("\n    INQUIRE\n{\n");
#endif /* DEBUG */
    clear_log();

    if (XmClipboardLock(display, win) != ClipboardSuccess) return;
    if (XmClipboardInquireCount(display,win,&count,&maxlen)
				==ClipboardSuccess) {
	buf = (char *)XtMalloc ((maxlen+1) * sizeof (char));
	if (count > 0) {
	    add_to_log ("Inquire - ");
	    for (i=1; i<=count; i++) {
		if (XmClipboardInquireFormat(display, win, i, buf,
			(unsigned long)(maxlen+1), &retlen)==ClipboardSuccess) {
		    buf[retlen] = '\0';
		    add_to_log (buf);
		    add_to_log (" ");
		}
	 	else {
		    add_to_log ("\nInquireFormat failed\n");
		    XBell(display, 0); 
		}
	    }
	    add_to_log ("\n");
	}
	else {
	    add_to_log("Inquire - No formats available\n");
	}
    }
    else {
	add_to_log ("\nInquireCount failed\n");
	XBell(display, 0);
    }
    XmClipboardUnlock(display, win, False);
#ifdef DEBUG
    printf ("}\n");
#endif /* DEBUG */
}

static void lock_proc(widget, tag, cb)
        Widget widget;
        XtPointer *tag;
        XmAnyCallbackStruct *cb;
{
    clear_log();
#ifdef DEBUG
    printf ("\n    LOCK\n{\n");
#endif /* DEBUG */
    if (XmClipboardLock(display, win) == ClipboardSuccess) {
        add_to_log("Clipboard Locked\n");
    }
    else {
        add_to_log("Lock failed\n");
    }
#ifdef DEBUG
    printf ("}\n");
#endif /* DEBUG */
}

static void unlock_proc(widget, tag, cb)
	Widget widget;
	XtPointer *tag;
	XmAnyCallbackStruct *cb;
{
    clear_log();
#ifdef DEBUG
    printf ("\n    UNLOCK\n{\n");
#endif /* DEBUG */
    if (XmClipboardUnlock(display, win, True) == ClipboardSuccess) {
        add_to_log("Clipboard Unlocked - all levels\n");
    }
    else {
        add_to_log("Unlock failed\n");
    }
#ifdef DEBUG
    printf ("}\n");
#endif /* DEBUG */
}

static void byname_cb (w, data_id, private, reason)
    Widget w;
    int *data_id;
    int *private;
    int *reason;
{
    struct _byname *b;

    if (bdata == NULL) return;
    switch (*reason) {
	case(XmCR_CLIPBOARD_DATA_DELETE) :
#ifdef DEBUG
    		printf ("<XmCR_CLIPBOARD_DATA_DELETE>\n");
#endif /* DEBUG */
		add_to_log ("XmCR_CLIPBOARD_DATA_DELETE\n");
		del_byname(*data_id);
		return;
		break;
	case(XmCR_CLIPBOARD_DATA_REQUEST) :
#ifdef DEBUG
    		printf ("<XmCR_CLIPBOARD_DATA_REQUEST>\n");
#endif /* DEBUG */
		add_to_log ("XmCR_CLIPBOARD_DATA_REQUEST\n");
		b = get_byname(*data_id);
		if (b==NULL) {
		    add_to_log ("No such byname record\n");
		    break;
		}
		if (XmClipboardCopyByName(display, win, *data_id, b->data,
		b->length, 0L) !=ClipboardSuccess) {
		    add_to_log ("byname copy failed\n");
		    XBell(display, 0);
		    return;
		}
		del_byname(*data_id);
		break;
	default :
		add_to_log ("Unknown reason in byname callback!\n");
		XBell(display, 0);
		return;
		break;
    }
}
