/* $TOG: AutoWmCmd.c /main/13 1999/02/24 17:39:34 mgreess $ */
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

/*
 *    File:    AutoWmCmd.c
 *
 *    Contains routines related with window manipulation.  
 *
 *
 */

#include <AutoMessages.h>
#include <math.h>
#include <testlib.h>
#include <X11/keysym.h>
#include <X11/IntrinsicP.h>
#include <sys/types.h>
#include <sys/time.h>
#include "AutoMwm.h"





#ifndef MOTIF1_1
#include <Xm/XmosP.h>
#endif /* MOTIF1_1 */

#ifdef MOTIF1_1
#define memmove(s1, s2, n)	bcopy(s2, s1, n)
#endif /* MOTIF1_1 */


typedef struct {
                 int operation;
                 Window window; } ClientData;


extern void AutoMessage();
extern void AutoError();

#define PTR_MOVE_INCREMENT 10

#define DO_NOTHING_TIME   90000 /* useconds = about .09 seconds */


#ifndef AUTOMATION

#define PackBoolean(d, num)     PackCARD8((d), (CARD8) (num))
#define PackListNum(d, num)     PackCARD16((d), (CARD16) (num))
#define UnpackBoolean(data)     ((Boolean) UnpackCARD8(data))
#define UnpackListNum(data)     ((int) UnpackCARD16(data))

#endif





static Boolean CheckWindowMenuItem( 
                        Window window,
                        char *item_name);

static Boolean CheckIconMenuItem( 
                        Window window,
                        char *item_name);

static Boolean CheckWindowDecoration( 
                        Window window,
                        char *item_name,
                        Boolean *illegal);

static void GetIconPosition( 
                        Window window,
                        IconRectInfo *icon_rect,
                        int *window_x,
                        int *window_y,
                        int *root_x,
                        int *root_y);

static Boolean GetMwmFrameIconProp( 
                        Window window);

static void SetMwmFrameIconProp( 
                        Window window);

static void DragPtr( 
                        Window window,
                        int button,
                        int dest_x,
                        int dest_y);

static void PressMouseButton( 
                        Window window,
                        int button,
                        unsigned int button_state,
                        int wx,
                        int wy,
                        int root_x,
                        int root_y,
                        Time time);

static void ReleaseMouseButton( 
                        Window window,
                        int button,
                        unsigned int button_state,
                        int wx,
                        int wy,
                        int root_x,
                        int root_y,
                        Time time);

static void ClickMouseButton( 
                        Window window,
                        int button,
                        unsigned int click_count,
                        int wx,
                       int wy,
                        int root_x,
                        int root_y);

static void GetNewButtonState( 
                        int button,
                        int *button_state);

static void PressKey( 
                        Window window,
                        int keycode,
                        unsigned int key_state,
                        int wx,
                        int wy,
                        int root_x,
                        int root_y,
                        Time time,
			Boolean postSystemMenu);

static void ReleaseKey( 
                        Window window,
                        int keycode,
                        unsigned int key_state,
                        int wx,
                        int wy,
                        int root_x,
                        int root_y,
                        Time time,
			Boolean postSystemMenu);

static void ClickKey( 
                        Window window,
                        KeySym keysym,
                        int click_count,
			unsigned int key_state,
                        int wx,
                        int wy,
                        int root_x,
                        int root_y,
			Boolean postSystemMenu);

static void GetKeyButtonState( 
                        Window window,
                        int *key_state,
                        int *button_state);

static void GetPointerLocation( 
                        Window window,
                        int *window_x,
                        int *window_y,
                        int *root_x,
                        int *root_y);

static Boolean GetWindowGeometry( 
                        Window window,
                        int *window_x,
                        int *window_y,
                        unsigned int *window_width,
                        unsigned int *window_height);

static void GetCurrentTime( 
                        Window window,
                        Time *time);

static Boolean IsMenuItemSensitive( 
                        Window window,
                        int item_num,
                        char *item_name);

static void GetMenuItemLocation( 
                        Window window,
                        int item_num,
                        int total_items,
                        int *item_x,
                        int *item_y,
			MenuInfo *menu_info);

static Boolean GetMenuItemNum( 
                        Window window,
                        char *item_name,
                        int *item_num);

static Boolean GetNumClicks( 
                        Window window,
                        int item_num,
                        int *num_clicks);

static void do_nothing( void );

static void RefreshWindow( void );
static void WindowMenuUnpost(
			     Window frameWin, 
			     Window menuWin);
static void IconMenuUnpost(
			     Window iconFrameWin, 
			     Window menuWin, 
			     int window_x, 
			     int window_y, 
			     int root_x, 
			     int root_y);

#ifndef AUTOMATION
static Boolean DoIconify(
			 Window window,
			 XtPointer data);

static Boolean DoMaximize(
			 Window window,
			 XtPointer data);
static Boolean DoRaise(
			 Window window,
			 XtPointer data);

static Boolean DoMove(
			 Window window,
			 XtPointer data
		     );

static Boolean DoResize(
			 Window window,
			 XtPointer data);
		       


static Boolean DoMenuItemSelect(
			 Window window,
			 XtPointer data);

static Boolean DoIconMenuItemSelect(
			 Window window,
			 XtPointer data);
		         
static Boolean DoMenuPost(
			 Window window,
			 XtPointer data);
static Boolean DoFocus(
			 Window window,
			 XtPointer data);
static Boolean DoMenuUnpost(
			 Window window,
			 XtPointer data);
static Boolean DoDeconify(
			  Window window,
			  XtPointer data);
			  
static Boolean DoIconMove(
			  Window window,
			  XtPointer data);
			  
static Boolean DoIconMenuPost(
			  Window window,
			  XtPointer data);
			  
static Boolean DoIconMenuUnpost(
			  Window window,
			  XtPointer data);
			  
static void SendMessage(
			ClientData *client_data,
			XtPointer data,
			int function,
			Time time);

static void SendUTMMessage (              
		   Widget requestor,
		   Atom selection,
		   Atom target,
		   XtPointer param,
		   unsigned long paramLen,
		   int paramFmt,
		   XtCallbackProc doneProc,
		   XtPointer closure,
		   Time time
		   );

void UTMDestinationProc (              
				Widget w,
				XtPointer call_data,
				XtPointer client_data);

static void SelectionValueReceived (
			   Widget w, 
			   XtPointer clientData,
			   XtPointer callData);

static void EnqueueUtmData (
			    XtPointer data);

static XtPointer DequeueUtmData ();


static XtPointer PackCARD32(
			    XtPointer data,
			    CARD32 val);
static XtPointer PackCARD16(
			    XtPointer data,
			    CARD16 val);	
static XtPointer PackCARD8(
			   XtPointer data,
			   CARD8 val);
static XtPointer PackString(
			   XtPointer data,
			   String val);

static CARD32 UnpackCARD32(
			    XtPointer *data);
			   
static CARD16 UnpackCARD16(
			    XtPointer *data);

static CARD8 UnpackCARD8(
			   XtPointer *data);

static String UnpackString(
			   XtPointer *data);

static void FillIconBoxInfo(
		     PropMotifWmFrameIconInfo *frame_icon_prop,
		     IconBoxInfo *ib,
		     CARD32 icon_x,
		     CARD32 icon_y);

Time GetTimestamp(Display *display);

#endif /* AUTOMATION */







#ifndef AUTOMATION
void UTMDestinationProc(); /* invoked by UTM */
#endif


/* Global variables */


static PropMotifWmFrameIconInfo    frame_icon_prop;

#ifndef AUTOMATION
static DataQueueRec *dataQueue = NULL , *dataQueueTail  = NULL;
ClientData cdata;
/* x, y for move */

MoveData window_move_data,icon_move_data;

/* gravity, width, height for resize */

ResizeData resize_data;


/* item num, item name, keyboard selecting menu item*/

ItemSelectData item_select_data,icon_item_select_data;

/* for posting the window/icon menu */

MenuPostData window_menu_post_data,icon_menu_post_data;


/* item name, group, for seeing if an item exists in window menu */

ItemCheckData item_check_data;

#endif /* AUTOMATION */


/*****************************************************************************
 *              Public routines called from AutoParseCmd
 ****************************************************************************/


#ifdef AUTOMATION
Boolean
#else
void
#endif /* AUTOMATION */
AutoWmRaise(
        Window window )
{

#ifdef AUTOMATION

    if (GetMwmFrameIconProp(window) == False)
        return(False);

    if (frame_icon_prop.iconInfo.clientState == MWM_ICONIC_STATE)
        return(False);

    XRaiseWindow(display, window);
    XSync(display, False);

    return(True);

#else

    XtPointer msg,save;
    int size = 0;
    Time time;


    time = GetTimestamp(display);

    size += sizeof(CARD32);          /* this is window id */
    size += sizeof(CARD32);          /* this is the mask we are passing in */

    save = msg = (XtPointer) XtMalloc(sizeof(CARD8) * size);
    msg = PackCARD32(msg,(CARD32)window);
    msg = PackCARD32(msg,(CARD32)WINDOW_RAISE_INFO);


    cdata.operation = WINDOW_RAISE;
    cdata.window = window;

    SendMessage (&cdata, save, size, time);


#endif /* AUTOMATION */



    

} /* AutoWmRaise */



#ifdef AUTOMATION
Boolean
#else
void
#endif /* AUTOMATION */
AutoWmDeIconify(
        Window window )
{

#ifdef AUTOMATION



    IconRectInfo    *icon_rect;
    int             window_x, window_y, root_x, root_y;
    Window          event_window;
    Time            time;
        int                             i;

    if (GetMwmFrameIconProp(window) == False)
        return(False);

    if (frame_icon_prop.iconInfo.clientState != MWM_ICONIC_STATE) {

                for (i = 0; i < 10; i++)
                do_nothing();
        XSync(display, False);

        if (GetMwmFrameIconProp(window) == False)
                return(False);
        if (frame_icon_prop.iconInfo.clientState != MWM_ICONIC_STATE)
                return(False);

        }

  icon_rect = &frame_icon_prop.iconInfo;

    

  GetIconPosition(window, icon_rect, &window_x, &window_y, &root_x, &root_y);

  event_window = icon_rect->iconFrameWin;

    LocatePtr(event_window, root_x, root_y);
    ClickMouseButton(event_window, Button1, 2,
                     window_x, window_y, root_x, root_y);

    GetCurrentTime(frame_icon_prop.frameWin, &time);

        /* Wait till the internal window state changes from ICONIC */

    GetMwmFrameIconProp(window);
    while (frame_icon_prop.iconInfo.clientState == MWM_ICONIC_STATE) {
        do_nothing();
        XSync(display, False);
        GetMwmFrameIconProp(window);
    }
      RefreshWindow();

    return(True);

#else

    XtPointer msg,save;
    int size = 0;
    Time time;


    time = GetTimestamp(display);

    size += sizeof(CARD32);          /* this is window id */
    size += sizeof(CARD32);          /* this is the mask we are passing in */

    save = msg = (XtPointer) XtMalloc(size);
    msg = PackCARD32(msg,(CARD32)window);
    msg = PackCARD32(msg,(CARD32)WINDOW_DEICONIFY_INFO);


    cdata.operation = WINDOW_DEICONIFY;
    cdata.window = window;

    SendMessage (&cdata, save, size, time);



#endif  /* AUTOMATION */

} /* AutoWmDeIconify */



#ifdef AUTOMATION
Boolean
#else
void
#endif /* AUTOMATION */
AutoWmIconify(
        Window window)
{

#ifdef AUTOMATION

 AutoRectangle *rect;
    int        window_x, window_y;
    int        root_x, root_y;
    int        frame_x, frame_y;
    unsigned int window_width, window_height;
    Window     event_window;
        int                i;

    if (GetMwmFrameIconProp(window) == False)
        return(False);

    if (frame_icon_prop.iconInfo.clientState == MWM_ICONIC_STATE) {

                for (i = 0; i < 10; i++)
                do_nothing();
        XSync(display, False);

        if (GetMwmFrameIconProp(window) == False)
                return(False);
        if (frame_icon_prop.iconInfo.clientState == MWM_ICONIC_STATE)
                return(False);
   }

    if (frame_icon_prop.titleGadgetCount <= 0 ||
        frame_icon_prop.titleGadgets[MWM_FRAME_MINIMIZE].id == -1)
        return(False);

    event_window = frame_icon_prop.frameWin;

    if (GetWindowGeometry(event_window, &frame_x, &frame_y, 
                          &window_width, &window_height) == False)
        return(False);

    rect = &frame_icon_prop.titleGadgets[MWM_FRAME_MINIMIZE].rect;
    window_x = rect->x + (rect->width / 2);
    window_y = rect->y + (rect->height / 2);
    root_x = frame_x + window_x;
    root_y = frame_y + window_y;
    LocatePtr(frame_icon_prop.frameWin, root_x, root_y);
    ClickMouseButton(frame_icon_prop.frameWin, Button1, 1, window_x, window_y,
                     root_x, root_y);

    XSync(display, False);
    /* Wait till the internal window state changes to ICONIC */

    GetMwmFrameIconProp(window);
    while (frame_icon_prop.iconInfo.clientState != MWM_ICONIC_STATE) {

        do_nothing();
        XSync(display, False);
        GetMwmFrameIconProp(window);

    }

    return(True);


#else


    /* SELECTION */

    XtPointer msg,save;
    int size = 0;
    Time time;


    time = GetTimestamp(display);

    size += sizeof(CARD32);          /* this is window id */
    size += sizeof(CARD32);          /* this is the mask we are passing in */

    save = msg = (XtPointer) XtMalloc(size);
    msg = PackCARD32(msg,(CARD32)window);
    msg = PackCARD32(msg,(CARD32)WINDOW_MINIMIZE_INFO);


    cdata.operation = WINDOW_MINIMIZE;
    cdata.window = window;

    SendMessage (&cdata, save, size, time);




#endif  /* AUTOMATION */

} /* AutoWmIconify */


   






#ifdef AUTOMATION
Boolean
#else
void 
#endif /* AUTOMATION */
AutoWmMaximize(
        Window window )
{


#ifdef AUTOMATION

    AutoRectangle *rect;
    int        window_x, window_y;
    int        root_x, root_y;
    int        frame_x, frame_y;
    unsigned int window_width, window_height;
    Window     event_window;
    Time       time;
    int        old_state, new_state;
	int		   i;

    if (GetMwmFrameIconProp(window) == False)
        return(False);

    if (frame_icon_prop.iconInfo.clientState == MWM_ICONIC_STATE) {

		for (i = 0; i < 10; i++)
        	do_nothing();
        XSync(display, False);

    	if (GetMwmFrameIconProp(window) == False)
        	return(False);
    	if (frame_icon_prop.iconInfo.clientState == MWM_ICONIC_STATE)
        	return(False);

	}

    if (frame_icon_prop.titleGadgetCount <= 0 ||
        frame_icon_prop.titleGadgets[MWM_FRAME_MAXIMIZE].id == -1)
        return(False);

    event_window = frame_icon_prop.frameWin;
    old_state = frame_icon_prop.iconInfo.clientState;

    if (GetWindowGeometry(event_window, &frame_x, &frame_y, 
                          &window_width, &window_height) == False)
        return(False);

    rect = &frame_icon_prop.titleGadgets[MWM_FRAME_MAXIMIZE].rect;
    window_x = rect->x + (rect->width / 2);
    window_y = rect->y + (rect->height / 2);
    root_x = frame_x + window_x;
    root_y = frame_y + window_y;
    LocatePtr(event_window, root_x, root_y);
    ClickMouseButton(frame_icon_prop.frameWin, Button1, 1, window_x, window_y,
                     root_x, root_y);

    GetCurrentTime(frame_icon_prop.frameWin, &time);

    new_state = (old_state == MWM_MAXIMIZED_STATE) ? MWM_NORMAL_STATE :
                MWM_MAXIMIZED_STATE;
    GetMwmFrameIconProp(window);
    while (frame_icon_prop.iconInfo.clientState != new_state) {
        do_nothing();
        XSync(display, False);
        GetMwmFrameIconProp(window);
    }


	RefreshWindow();

    return(True);

#else

    XtPointer msg,save;
    int size = 0;
    Time time;
    

    time = GetTimestamp(display);

    size += sizeof(CARD32);          /* this is window id */
    size += sizeof(CARD32);          /* this is the mask we are passing in */

    save = msg = (XtPointer) XtMalloc(size);
    msg = PackCARD32(msg,(CARD32)window);
    msg = PackCARD32(msg,(CARD32)WINDOW_MAXIMIZE_INFO);

    cdata.operation = WINDOW_MAXIMIZE;
    cdata.window = window;

    SendMessage (&cdata, save, size, time);



#endif /* AUTOMATION */



} /* AutoWmMaximize */


static void
WindowMenuUnpost(
        Window frameWin, Window menuWin )
{

    int       root_x, root_y;
    int       window_x, window_y;
    Time      time;
    int        frame_x, frame_y;
    unsigned int    window_width, window_height;


    GetPointerLocation(frameWin, &window_x, &window_y, &root_x, &root_y);

    ClickKey(menuWin, XK_Escape, 1, 0, window_x, window_y, root_x, root_y, 
			 False);

    GetCurrentTime(frameWin, &time);

} /* WindowMenuUnpost */


#ifdef AUTOMATION
Boolean
#else
void
#endif /* AUTOMATION */
AutoWmMenuPost(
        Window window, Boolean keyboard )
{


#ifdef AUTOMATION

    AutoRectangle *rect;
    int        window_x, window_y;
    int        frame_x, frame_y;
    int        root_x, root_y;
    Window     event_window;
    Time       time;
    unsigned int    window_width, window_height;
	int		   i;


    if (GetMwmFrameIconProp(window) == False)
        return(False);

    if (frame_icon_prop.iconInfo.clientState == MWM_ICONIC_STATE) {

		for (i = 0; i < 10; i++)
        	do_nothing();
        XSync(display, False);

    	if (GetMwmFrameIconProp(window) == False)
        	return(False);
    	if (frame_icon_prop.iconInfo.clientState == MWM_ICONIC_STATE)
        	return(False);

	}

	if (keyboard == True) {

    	if (GetWindowGeometry(frame_icon_prop.frameWin, &frame_x, &frame_y, 
                          	  &window_width, &window_height) == False)
        	return(False);

		/*
		 * If title region present, locate pointer to middle of title
		 * region; else, locate pointer to middle of the window.
		 */
		if (frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].id != -1) {

    		rect = &frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect;
    		window_x = rect->x + (rect->width / 2);
    		window_y = rect->y + (rect->height / 2);
    		root_x = frame_x + window_x;
    		root_y = frame_y + window_y;

		}

		else {
			
			window_x = window_width / 2;
			window_y = window_height / 2;
			root_x = frame_x + window_x;
			root_y = frame_y + window_y;

		}

    	LocatePtr(frame_icon_prop.frameWin, root_x, root_y);
		GetCurrentTime(frame_icon_prop.frameWin, &time);
    	ClickKey(frame_icon_prop.frameWin, XK_space, 1, 
				 Mod1Mask, window_x, window_y, root_x, root_y, True);

		GetCurrentTime(frame_icon_prop.frameWin, &time);
		return(True);

	}

    if (frame_icon_prop.titleGadgetCount <= 0 ||
       	frame_icon_prop.titleGadgets[MWM_FRAME_SYSTEM].id == -1)
       	return(False);

    event_window = frame_icon_prop.frameWin;

    if (GetWindowGeometry(event_window, &frame_x, &frame_y, 
                          &window_width, &window_height) == False)
        return(False);

    rect = &frame_icon_prop.titleGadgets[MWM_FRAME_SYSTEM].rect;
    window_x = rect->x + (rect->width / 2);
    window_y = rect->y + (rect->height / 2);
    root_x = frame_x + window_x;
    root_y = frame_y + window_y;
    LocatePtr(event_window, root_x, root_y);
    GetCurrentTime(frame_icon_prop.frameWin, &time);
    PressMouseButton(frame_icon_prop.frameWin, Button1, 0, 
                      window_x, window_y, root_x, root_y, time);

    GetCurrentTime(frame_icon_prop.frameWin, &time);
    return(True);


#else

    /* SELECTION */

    XtPointer msg,save;
    int size = 0;
    Time time;


    time = GetTimestamp(display);

    size += sizeof(CARD32);          /* this is window id */
    size += sizeof(CARD32);          /* this is the mask we are passing in */

    save = msg = (XtPointer) XtMalloc(size);
    msg = PackCARD32(msg,(CARD32)window);
    msg = PackCARD32(msg,(CARD32)WINDOW_MENU_POST_INFO);


    cdata.operation = WINDOW_MENU_POST;
    cdata.window = window;

    window_menu_post_data.keyboard = keyboard;
    

    SendMessage (&cdata, save, size, time);


#endif  /* AUTOMATION */

} /* AutoWmMenuPost */


#ifdef AUTOMATION
Boolean
#else
void
#endif /* AUTOMATION */
AutoWmMenuUnpost(
        Window window )
{


#ifdef AUTOMATION


    int       root_x, root_y;
    int       window_x, window_y;
    Time      time;
    AutoRectangle *rect;
    int        frame_x, frame_y;
    unsigned int    window_width, window_height;


    if (GetMwmFrameIconProp(window) == False)
        return(False);

    if (frame_icon_prop.iconInfo.clientState == MWM_ICONIC_STATE)
        return(False);


	WindowMenuUnpost(frame_icon_prop.frameWin, frame_icon_prop.menuWin);

    return(True);

#else
    /* SELECTION */

    XtPointer msg,save;
    int size = 0;
    Time time;


    time = GetTimestamp(display);

    size += sizeof(CARD32);          /* this is window id */
    size += sizeof(CARD32);          /* this is the mask we are passing in */

    save = msg = (XtPointer) XtMalloc(size);
    msg = PackCARD32(msg,(CARD32)window);
    msg = PackCARD32(msg,(CARD32)WINDOW_MENU_UNPOST_INFO);


    cdata.operation = WINDOW_MENU_UNPOST;
    cdata.window = window;

    SendMessage (&cdata, save, size, time);






#endif /* AUTOMATION */

} /* AutoWmMenuUnpost */


/*
 *
 * Select the menu item specified. The item can be specified by item_num;
 * or if item_num is specified as -1, then it may be specified by
 * item_name, in which case, a call to AutoWmGetMenuIndex is made to get
 * the corresponding item_num.
 *
 * The routine returns False if:
 *
 *    item_num is greater than the no. of items in the menu,
 *    item_num is -1, and item_name is NULL,
 *    item_num is -1, and item_name is not present in the menu,
 *    item_num is -1, and item_name is insensitive.
 *
 * Otherwise, 
 *
 *	  it calculates the position of the item in the menu window
 *	  it moves the pointer to that position
 *	  it generates a mouse click event to select the item,
 *	  and returns True.
 *
 *	NOTE:
 *    For the first phase of implementation, we should make the WindowMenu
 *    items for f.resize and f.move to be either insensitive, or absent.
 *
 */

#ifdef AUTOMATION
Boolean
#else
void
#endif /* AUTOMATION */
AutoWmMenuSelect(
        Window window,
        int item_num,
        char *item_name, 
		Boolean keyboard )
{

#ifdef AUTOMATION

 char    current_item[MAX_NAME_LEN + 1];
    char    warn_buf[2 * MAX_NAME_LEN + 1];
    int     current_num;
    int     root_x, root_y;
    int     window_x, window_y;
    Boolean retval;
    Time    time;
    int     item_x, item_y;
    int     i;

  retval = True;

        /*
         * The following 4 lines of code are here to overcome a
         * peculiar delay problem in mwm about menu Window.
         */
        XSync(display, False);
        for (i = 0; i < 100; i++)
                do_nothing();
        XSync(display, False);

    if (GetMwmFrameIconProp(window) == False)
                retval = False;

    if (item_num > frame_icon_prop.menuItemCount || 
        item_num > MAX_MENU_ITEMS) {
 if (item_num > frame_icon_prop.menuItemCount)
                        sprintf(warn_buf, 
                        _AutoMessages[SCRMSG13],
                        item_num, frame_icon_prop.menuItemCount);
    else
                        sprintf(warn_buf, 
                        _AutoMessages[SCRMSG14],
                        item_num, MAX_MENU_ITEMS);
                AutoMessage(warn_buf);
                retval = False;

        }

    if (item_num != -1)
        current_num = item_num;
    else {

        if (item_name == NULL)
                        retval = False;
        if (GetMenuItemNum(window, item_name, &current_num) == False) {

                        sprintf(warn_buf, 
                                _AutoMessages[SCRMSG15],
                                        item_name);
                        AutoMessage(warn_buf);
                        retval = False;

                }

    }

        XSync(display, False);

    if (IsMenuItemSensitive(window, current_num, current_item) == False) {

        sprintf(warn_buf, _AutoMessages[SCRMSG16],
                current_num, current_item);
        AutoMessage(warn_buf);

    }
/**DEBUG** Check for "Size" and "Move" and disallow it till the key event
 problem is not solved **/
        if ((strcmp(current_item, "Size") == 0) ||
                (strcmp(current_item, "Move") == 0)) {
                AutoMessage(_AutoMessages[SCRMSG17]);
                retval = False;

        }

        if (retval == False) {
                WindowMenuUnpost(frame_icon_prop.frameWin, frame_icon_prop.menuWin);
                return(retval);
        }

    GetMenuItemLocation(frame_icon_prop.menuWin, current_num, 
                        frame_icon_prop.menuItemCount, &item_x, &item_y,
                                                &frame_icon_prop.windowMenu[current_num - 1]);

    LocatePtr(frame_icon_prop.frameWin, item_x, item_y);
    GetPointerLocation(frame_icon_prop.menuWin, &window_x, &window_y, 
                       &root_x, &root_y);
    ClickMouseButton(frame_icon_prop.menuWin, Button1, 1, window_x, window_y,
                     root_x, root_y);
   GetCurrentTime(frame_icon_prop.frameWin, &time);

        XSync(display, False);

        for (i = 0; i < 25; i++)
                do_nothing();

        RefreshWindow();

       return (retval);

#else


    XtPointer msg,save;
    int size = 0;
    Time time;



    time = GetTimestamp(display);

    size += sizeof(CARD32);          /* this is window id */
    size += sizeof(CARD32);          /* this is the mask we are passing in */



    save = msg = (XtPointer) XtMalloc(size);
    msg = PackCARD32(msg,(CARD32)window);
    msg = PackCARD32(msg,(CARD32)WINDOW_MENU_ITEM_SELECT_INFO);

    if (item_num == -1)
      strcpy (item_select_data.item_name,item_name);
    item_select_data.item_num = item_num;
    item_select_data.keyboard = keyboard;


    cdata.operation = WINDOW_MENU_ITEM_SELECT;
    cdata.window = window;
    

    
    SendMessage (&cdata,save, size, time);


#endif /* AUTOMATION */

} /* AutoWmMenuSelect */


/*
 * Moves the window to x, y relative to rootWindow
 */

#ifdef AUTOMATION
Boolean
#else
void
#endif /* AUTOMATION */
AutoWmMove(
        Window window,
        int x,
        int y )


{

#ifdef AUTOMATION


    Window  event_window;
    AutoRectangle *title_rect, *menu_rect;
    int        window_x, window_y;
    int        root_x, root_y;
    int        dest_x, dest_y;
    int        x_incr, y_incr;
    int        frame_x, frame_y;
    unsigned int window_width, window_height;
    int        i;

    if (GetMwmFrameIconProp(window) == False)
        return(False);

    if (frame_icon_prop.iconInfo.clientState == MWM_ICONIC_STATE) {

		for (i = 0; i < 10; i++)
        	do_nothing();
        XSync(display, False);

    	if (GetMwmFrameIconProp(window) == False)
        	return(False);
    	if (frame_icon_prop.iconInfo.clientState == MWM_ICONIC_STATE)
        	return(False);

	}

    if (frame_icon_prop.titleGadgetCount <= 0 ||
        frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].id == -1)
        return(False);

    event_window = frame_icon_prop.frameWin;
    if (GetWindowGeometry(event_window, &frame_x, &frame_y, 
                          &window_width, &window_height) == False)
        return(False);

    title_rect = &frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect;
    window_x = title_rect->x + (title_rect->width / 2);
    window_y = title_rect->y + (title_rect->height / 2);
    root_x = frame_x + window_x;
    root_y = frame_y + window_y;

    menu_rect = (frame_icon_prop.titleGadgets[MWM_FRAME_SYSTEM].id == -1) ?
                NULL :
                &frame_icon_prop.titleGadgets[MWM_FRAME_SYSTEM].rect;

    window_x = frame_icon_prop.windowX + title_rect->x + 
               (title_rect->width / 2);
    window_y = frame_icon_prop.windowY + title_rect->y + 
               (title_rect->height / 2);

    /*
     * x, y provided are the window corners. From that, and from the
     * width, height of menu rectangle, and title rectangle, compute
     * the actual destination x, y to which the pointer should be moved.
     */
    x_incr = (menu_rect == NULL) ? 0 : menu_rect->width;
    y_incr = frame_icon_prop.upperBorderWidth;
    x_incr += (title_rect->width / 2);
    x_incr += (frame_icon_prop.lowerBorderWidth - 1);
    y_incr += (title_rect->height / 2);
    dest_x = x + x_incr;
    dest_y = y + y_incr;

    LocatePtr(event_window, root_x, root_y);

    /*
	 * The following 4 lines of code are here to overcome a
	 * peculiar delay problem in mwm under either heavy load
	 * or on very fast machines.
	 */
	XSync(display, False);
	for (i = 0; i < 50; i++)
		do_nothing();
	XSync(display, False);

    DragPtr(frame_icon_prop.frameWin, Button1, dest_x, dest_y);

	RefreshWindow();
        
    return(True);

#else


    XtPointer msg,save;
    int size = 0;
    Time time;



    time = GetTimestamp(display);

    size += sizeof(CARD32);          /* this is window id */
    size += sizeof(CARD32);          /* this is the mask we are passing in */

    save = msg = (XtPointer) XtMalloc(size);
    msg = PackCARD32(msg,(CARD32)window);
    msg = PackCARD32(msg,(CARD32)WINDOW_MOVE_INFO);

    window_move_data.move_x = x;
    window_move_data.move_y = y;

    cdata.operation = WINDOW_MOVE;
    cdata.window = window;

    
    SendMessage (&cdata,save, size, time);

#endif  /* AUTOMATION */



} /* AutoWmMove */


/*
 * Resize the window by locating the pointer at one of the resize handles,
 * and dragging the pointer to make the size of the window (width x height).
 * A valid  width argument is mandatory, whereas the height argument may be 
 * -1 if it is not to be considered. If the gravity specified is NORTH or
 * SOUTH, the width argument will be treated as height. If the gravity
 * specified is NORTHEAST, NORTHWEST, SOUTHEAST, or SOUTHWEST, and if
 * height is specified as -1, the resizing of window will take place only
 * along the width. If the gravity is NORTH, SOUTH, EAST, or WEST, and a
 * valid height is specified, it will be ignored.
 *
 */

#ifdef AUTOMATION
Boolean
#else
void
#endif /* AUTOMATION */
AutoWmResize(
        Window window,
        int gravity,
        int width,
        int height )

{

#ifdef AUTOMATION


  AutoRectangle *rect;
    AutoRectangle *title_rect;
    int        window_x, window_y;
    int        root_x, root_y;
    int        dest_x, dest_y;
    int        frame_x, frame_y;
    unsigned int window_width, window_height;
    int        x_incr, y_incr;
    int        title_height;
    Window     event_window;
    int        act_width, act_height;
    Time       time;
    int        i;
  if (width < 0) {
        AutoMessage(_AutoMessages[SCRMSG4]);
        return(False);
    }

    if (GetMwmFrameIconProp(window) == False)
        return(False);

    if (frame_icon_prop.iconInfo.clientState == MWM_ICONIC_STATE) {

                for (i = 0; i < 10; i++)
                do_nothing();
        XSync(display, False);

        if (GetMwmFrameIconProp(window) == False)
                return(False);
        if (frame_icon_prop.iconInfo.clientState == MWM_ICONIC_STATE)
                return(False);

        }

    if (frame_icon_prop.resizeGadgets[gravity].id == -1)
        return(False);
  event_window = frame_icon_prop.frameWin;
    if (GetWindowGeometry(event_window, &frame_x, &frame_y, 
                          &window_width, &window_height) == False)
        return(False);

    title_rect = &frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect;
    title_height = (frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].id != -1) ? 
                    title_rect->height : 0;

    /*
     * Compute the actual new width and new height to which the
     * window is to be resized. If they are equal to the current
     * width and height of the window, return.
     */
    x_incr = (frame_icon_prop.resizeGadgets[WM_EAST].id != -1) ?
             frame_icon_prop.resizeGadgets[WM_EAST].rect.width :
             frame_icon_prop.lowerBorderWidth;
    y_incr = title_height + frame_icon_prop.lowerBorderWidth + 
             frame_icon_prop.upperBorderWidth;
    act_width = width + (2 * x_incr);
    act_height = height + y_incr;
    if (act_width == window_width && act_height == window_height)
        return(True);
   /* Compute the x and y increments/decrements to get new size */
    x_incr = y_incr = 0;
    switch (gravity) {

        case WM_NORTH:
        case WM_SOUTH:

            y_incr = width - window_height; /* New width treated as height */
            break;

        case WM_EAST:
        case WM_WEST:

            x_incr = width - window_width;
            break;

        case WM_NORTHEAST:
        case WM_NORTHWEST:
        case WM_SOUTHEAST:
        case WM_SOUTHWEST:

            x_incr = width - window_width;
            y_incr = (height > 0) ? (height - window_height) : 0;
        break;

    }

    rect = &frame_icon_prop.resizeGadgets[gravity].rect;

    /*
     * Compute the starting point to place the pointer to start resizing
     * depending on gravity.
     */
    switch (gravity) {

        case WM_NORTH:

            window_x = rect->x + (rect->width / 2);
            window_y = rect->y;
            break;

        case WM_SOUTH:

            window_x = rect->x + (rect->width / 2);
            window_y = rect->y + rect->height - 1;
            break;
        case WM_EAST:

            window_x = rect->x + rect->width - 1;
            window_y = rect->y + (rect->height / 2);
            break;

        case WM_WEST:

            window_x = rect->x;
            window_y = rect->y + (rect->height / 2);
            break;

        case WM_NORTHEAST:

            window_x = rect->x + rect->width - 1;
            window_y = rect->y;
            break;

        case WM_NORTHWEST:

            window_x = rect->x;
            window_y = rect->y;
            break;
      case WM_SOUTHEAST:

            window_x = rect->x + rect->width - 1;
            window_y = rect->y + rect->height - 1;
            break;

        case WM_SOUTHWEST:

            window_x = rect->x;
            window_y = rect->y + rect->height - 1;
            break;

    }

    root_x = frame_x + window_x;
    root_y = frame_y + window_y;

    /* 
     * Compute the destination x, y depending on gravity,
     * and the computed x_incr, y_incr values (+ve or -ve).
     */
    switch (gravity) {
     case WM_NORTH:

            dest_x = root_x;
                        dest_y = root_y;
            if (y_incr > 0)    /* Increase height, meaning decrease y */
                dest_y -= y_incr;
            if (y_incr < 0)    /* Decrease height, meaning increase y */
                dest_y -= y_incr;
            dest_y -= (title_height + frame_icon_prop.upperBorderWidth +
                       frame_icon_prop.lowerBorderWidth);
            break;

        case WM_SOUTH:

            dest_x = root_x;
            dest_y = root_y + y_incr; /* If y_incr < 0, height is decreased */
            dest_y += (title_height + frame_icon_prop.upperBorderWidth +
                       frame_icon_prop.lowerBorderWidth);
            break;

        case WM_EAST:
         dest_y = root_y;
            dest_x = root_x + x_incr; /* If x_incr < 0, width is decreased */
            dest_x += (2 * rect->width);
            break;

        case WM_WEST:

            dest_y = root_y;
            dest_x = root_x;
            if (x_incr > 0)    /* Increase width, meaning decrease x */
                dest_x -= x_incr;
            if (x_incr < 0)    /* Decrease width, meaning increase x */
                dest_x -= x_incr;
            dest_x -= (2 * rect->width);
            break;

        case WM_NORTHEAST:

            dest_x = root_x + x_incr; /* If x_incr < 0, width is decreased */
                        dest_y = root_y;
            if (y_incr > 0)    /* Increase height, meaning decrease y */
                dest_y -= y_incr;
            if (y_incr < 0)    /* Decrease height, meaning increase y */
    rect = &frame_icon_prop.resizeGadgets[WM_EAST].rect;
            dest_x += (2 * rect->width);
                        if (height > 0)
                dest_y -= (title_height + frame_icon_prop.upperBorderWidth +
                          frame_icon_prop.lowerBorderWidth);
            break;

        case WM_NORTHWEST:

                        dest_x = root_x;
                        dest_y = root_y;
            if (x_incr > 0)    /* Increase width, meaning decrease x */
                dest_x -= x_incr;
            if (x_incr < 0)    /* Decrease width, meaning increase x */
                dest_x -= x_incr;
            if (y_incr > 0)    /* Increase height, meaning decrease y */
                dest_y -= y_incr;
            if (y_incr < 0)    /* Decrease height, meaning increase y */
                dest_y -= y_incr;
            rect = &frame_icon_prop.resizeGadgets[WM_WEST].rect;
            dest_x -= (2 * rect->width);
                        if (height > 0)
                dest_y -= (title_height + frame_icon_prop.upperBorderWidth +
                    frame_icon_prop.lowerBorderWidth);
            break;

        case WM_SOUTHEAST:

            dest_x = root_x + x_incr; /* If x_incr < 0, width is decreased */
            dest_y = root_y + y_incr; /* If y_incr < 0, height is decreased */
            rect = &frame_icon_prop.resizeGadgets[WM_EAST].rect;
            dest_x += (2 * rect->width);
                        if (height > 0)
                dest_y += (title_height + frame_icon_prop.upperBorderWidth +
                          frame_icon_prop.lowerBorderWidth);
            break;

        case WM_SOUTHWEST:

            if (x_incr > 0)    /* Increase width, meaning decrease x */
                dest_x = root_x - x_incr;
            if (x_incr < 0)    /* Decrease width, meaning increase x */
                dest_x = root_x - x_incr;
            dest_y = root_y + y_incr; /* If y_incr < 0, height is decreased */
            rect = &frame_icon_prop.resizeGadgets[WM_WEST].rect;
    dest_x -= (2 * rect->width);
                        if (height > 0)
                dest_y += (title_height + frame_icon_prop.upperBorderWidth +
                          frame_icon_prop.lowerBorderWidth);
            break;

        default:

            AutoError(_AutoMessages[WARNMSG91]);
            return(False);

    }

    LocatePtr(event_window, root_x, root_y);

    /*
         * The following 4 lines of code are here to overcome a
         * peculiar delay problem in mwm under either heavy load
         * or on very fast machines.
         */
        XSync(display, False);
        for (i = 0; i < 50; i++)
     XSync(display, False);

    DragPtr(frame_icon_prop.frameWin, Button1, dest_x, dest_y);
        
    GetCurrentTime(frame_icon_prop.frameWin, &time);

        RefreshWindow();

    return(True);

#else



    XtPointer msg,save;
    int size = 0;
    Time time;

  
    time = GetTimestamp(display);

    size += sizeof(CARD32);          /* this is window id */
    size += sizeof(CARD32);          /* this is the mask we are passing in */

    save = msg = (XtPointer) XtMalloc(sizeof(CARD8) * size);
    msg = PackCARD32(msg,(CARD32)window);

    cdata.operation = WINDOW_RESIZE;


    switch (gravity)
      {
      case WM_NORTH:
	msg = PackCARD32(msg,(CARD32)WINDOW_RESIZE_NORTH_INFO);
	break;

      case WM_SOUTH:
	msg = PackCARD32(msg,(CARD32)WINDOW_RESIZE_SOUTH_INFO);
	break;

      case WM_EAST:
	msg = PackCARD32(msg,(CARD32)WINDOW_RESIZE_EAST_INFO);
	break;

      case WM_WEST:
	msg = PackCARD32(msg,(CARD32)WINDOW_RESIZE_WEST_INFO);
	break;

      case WM_NORTHEAST:
	msg = PackCARD32(msg,(CARD32)WINDOW_RESIZE_NORTHEAST_INFO);
	break;

      case WM_NORTHWEST:
	msg = PackCARD32(msg,(CARD32)WINDOW_RESIZE_NORTHWEST_INFO);
	break;

      case WM_SOUTHEAST:
	msg = PackCARD32(msg,(CARD32)WINDOW_RESIZE_SOUTHEAST_INFO);
	break;

      case WM_SOUTHWEST:
	msg = PackCARD32(msg,(CARD32)WINDOW_RESIZE_SOUTHWEST_INFO);
	break;
      }


    cdata.window = window;
    resize_data.width = width;
    resize_data.height = height;
    resize_data.gravity = gravity;


    SendMessage (&cdata, save, size, time);



#endif  /* AUTOMATION */

} /* AutoWmResize */


/*
 * Gives the window, input focus
 */

#ifdef AUTOMATION
Boolean
#else
void
#endif  /* AUTOMATION */
AutoWmFocus(
        Window window )
{


#ifdef AUTOMATION
    AutoRectangle *title_rect;
    int        window_x, window_y;
    int        root_x, root_y;
    int        frame_x, frame_y;
    unsigned int window_width, window_height;
    Window     event_window;
    int        i;

    if (GetMwmFrameIconProp(window) == False)
        return(False);

    if (frame_icon_prop.iconInfo.clientState == MWM_ICONIC_STATE) {

		for (i = 0; i < 10; i++)
        	do_nothing();
        XSync(display, False);

    	if (GetMwmFrameIconProp(window) == False)
        	return(False);
    	if (frame_icon_prop.iconInfo.clientState == MWM_ICONIC_STATE)
        	return(False);

	}

    if (frame_icon_prop.titleGadgetCount <= 0 ||
        frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].id == -1)
        return(False);

    event_window = frame_icon_prop.frameWin;
    if (GetWindowGeometry(event_window, &frame_x, &frame_y, 
                          &window_width, &window_height) == False)
        return(False);

    title_rect = &frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect;
    window_x = title_rect->x + (title_rect->width / 2);
    window_y = title_rect->y + (title_rect->height / 2);
    root_x = frame_x + window_x;
    root_y = frame_y + window_y;

    LocatePtr(event_window, root_x, root_y);
    ClickMouseButton(frame_icon_prop.frameWin, Button1, 1, window_x, window_y,
                     root_x, root_y);

    return(True); 

#else


    XtPointer msg,save;
    int size = 0;
    Time time;


    time = GetTimestamp(display);

    size += sizeof(CARD32);          /* this is window id */
    size += sizeof(CARD32);          /* this is the mask we are passing in */

    save = msg = (XtPointer) XtMalloc(size);
    msg = PackCARD32(msg,(CARD32)window);
    msg = PackCARD32(msg,(CARD32)WINDOW_FOCUS_INFO);


    cdata.operation = WINDOW_FOCUS;
    cdata.window = window;

    SendMessage (&cdata, save, size, time);

#endif /* AUTOMATION */

} /* AutoWmFocus */

#ifdef AUTOMATION
Boolean
#else
void
#endif /* AUTOMATION */
AutoWmIconMenuPost(
        Window window, Boolean keyboard )
{

#ifdef AUTOMATION

    IconRectInfo    	        *icon_rect;
    Window          	        event_window;
    int             	        i;
    int        			window_x, window_y;
    int        			root_x, root_y;
    Time       			time;
    XWindowAttributes	        wattrs;


    if (GetMwmFrameIconProp(window) == False)
        return(False);

	if (frame_icon_prop.iconInfo.clientState != MWM_ICONIC_STATE) {

		for (i = 0; i < 10; i++)
			do_nothing();
			XSync(display, False);

		if (GetMwmFrameIconProp(window) == False)
			return(False);
    	if (frame_icon_prop.iconInfo.clientState != MWM_ICONIC_STATE)
        	return(False);

	}

	/* 
	 * If icon is inside an IconBox, iconMenu can not be posted via
	 * keyboard.
	 */
	if (keyboard == True && frame_icon_prop.iconInfo.useIconBox == True)
		return(False);

	/*
	 * Raise icon window for posting iconMenu via keyboard
	 */
	if (keyboard == True)
		XRaiseWindow(display, window);

	icon_rect = &frame_icon_prop.iconInfo;
    event_window = icon_rect->iconFrameWin;
	XGetWindowAttributes(display, event_window, &wattrs);

    GetIconPosition(window, icon_rect, &window_x, &window_y, &root_x, &root_y);

	LocatePtr(event_window, root_x, root_y);

	if (keyboard == True) {

		GetCurrentTime(event_window, &time);
    	ClickKey(event_window, XK_space, 1, 
				 Mod1Mask, window_x, window_y, root_x, root_y, False);

		GetCurrentTime(event_window, &time);
		return(True);

	}

    ClickMouseButton(event_window, Button1, 1,
					 window_x, window_y, root_x, root_y);

	GetCurrentTime(event_window, &time);

	return(True);

#else

    XtPointer msg,save;
    int size = 0;
    Time time;


    time = GetTimestamp(display);

    size += sizeof(CARD32);          /* this is window id */
    size += sizeof(CARD32);          /* this is the mask we are passing in */

    save = msg = (XtPointer) XtMalloc(size);
    msg = PackCARD32(msg,(CARD32)window);
    msg = PackCARD32(msg,(CARD32)ICON_MENU_POST_INFO);


    cdata.operation = ICON_MENU_POST;
    cdata.window = window;

    icon_menu_post_data.keyboard = keyboard;
    

    SendMessage (&cdata, save, size, time);

#endif  /* AUTOMATION */
} /* AutoWmIconMenuPost */



static void
IconMenuUnpost(Window iconFrameWin, Window menuWin, 
			   int window_x, int window_y, int root_x, int root_y)
{


	Time	time;


    ClickKey(menuWin, XK_Escape, 1, 0, window_x, window_y, root_x, root_y, 
			 False);

	GetCurrentTime(iconFrameWin, &time);



} /* IconMenuUnpost */


#ifdef AUTOMATION
Boolean
#else
void
#endif /* AUTOMATION */
AutoWmIconMenuUnpost(
        Window window )
{


#ifdef AUTOMATION


	IconRectInfo    	*icon_rect;
	Window          	event_window;
    int        			window_x, window_y;
    int        			root_x, root_y;
	int		  			new_x, new_y;
    Time       			time;
	XWindowAttributes	wattrs;

    if (GetMwmFrameIconProp(window) == False)
        return(False);

    if (frame_icon_prop.iconInfo.clientState != MWM_ICONIC_STATE)
        return(False);

	icon_rect = &frame_icon_prop.iconInfo;

	event_window = icon_rect->iconFrameWin;

	XGetWindowAttributes(display, event_window, &wattrs);

   	GetIconPosition(window, icon_rect, &window_x, &window_y, &root_x, &root_y);

	IconMenuUnpost(icon_rect->iconFrameWin, frame_icon_prop.menuWin, 
				   window_x, window_y, root_x, root_y);

	return(True);

#else

    XtPointer msg,save;
    int size = 0;
    Time time;


    time = GetTimestamp(display);

    size += sizeof(CARD32);          /* this is window id */
    size += sizeof(CARD32);          /* this is the mask we are passing in */

    save = msg = (XtPointer) XtMalloc(size);
    msg = PackCARD32(msg,(CARD32)window);
    msg = PackCARD32(msg,(CARD32)ICON_MENU_UNPOST_INFO);


    cdata.operation = ICON_MENU_UNPOST;
    cdata.window = window;


    SendMessage (&cdata, save, size, time);




#endif  /* AUTOMATION */

} /* AutoWmIconMenuUnpost */




/*
 *
 * Select the menu item specified. The item can be specified by item_num;
 * or if item_num is specified as -1, then it may be specified by
 * item_name, in which case, a call to AutoWmGetMenuIndex is made to get
 * the corresponding item_num.
 *
 * The routine returns False if:
 *
 *    item_num is greater than the no. of items in the menu,
 *    item_num is -1, and item_name is NULL,
 *    item_num is -1, and item_name is not present in the menu,
 *    item_num is -1, and item_name is insensitive.
 *
 * Otherwise, 
 *
 *    it generates the necessary number of DownArrow click events
 *     to select the specified item, and returns True.
 *
 *    For the first phase of implementation, we should make the WindowMenu
 *    items for f.resize and f.move to be either insensitive, or absent.
 *
 */


#ifdef AUTOMATION
Boolean
#else
void
#endif /* AUTOMATION */
AutoWmIconMenuSelect(
        Window window,
        int item_num,
        char *item_name, 
		Boolean keyboard )
{
#ifdef AUTOMATION

    char    			current_item[MAX_NAME_LEN + 1];
    char    			warn_buf[MAX_NAME_LEN + 1];
    int     			current_num;
    int     			root_x, root_y;
    int     			window_x, window_y;
    Boolean 			retval;
    Time    			time;
    int     			item_x, item_y;
    int     			i;
    XWindowAttributes	        wattrs;
    IconRectInfo    	        *icon_rect;

    
    retval = True;

    /*
	 * The following 4 lines of code are here to overcome a
	 * peculiar delay problem in mwm about menu Window.
	 */
	XSync(display, False);
	for (i = 0; i < 100; i++)
		do_nothing();
	XSync(display, False);

    if (GetMwmFrameIconProp(window) == False)
        return(retval);

    if (frame_icon_prop.iconInfo.clientState != MWM_ICONIC_STATE)
        return(False);

	if (XGetWindowAttributes(display, frame_icon_prop.menuWin, &wattrs) == 0)
		return(False);
	if (wattrs.map_state == IsUnmapped || wattrs.map_state == IsUnviewable)
		return(False);

    if (item_num > frame_icon_prop.menuItemCount ||
        item_num > MAX_MENU_ITEMS) {

        if (item_num > frame_icon_prop.menuItemCount)
            sprintf(warn_buf,
            "Item no. specified (%d) is > than total no. of menu items (%d)\n",
            item_num, frame_icon_prop.menuItemCount);
        else
            sprintf(warn_buf,
            _AutoMessages[SCRMSG14],
            item_num, MAX_MENU_ITEMS);
        AutoMessage(warn_buf);
        retval = False;

    }

    if (item_num != -1)
        current_num = item_num;
    else {

        if (item_name == NULL)
            retval = False;
        if (GetMenuItemNum(window, item_name, &current_num) == False) {

            sprintf(warn_buf, _AutoMessages[SCRMSG15],
                    item_name);
            AutoMessage(warn_buf);
            retval = False;

		}

    }

	if (retval == False) {

		icon_rect = &frame_icon_prop.iconInfo;
		GetIconPosition(window, icon_rect, &window_x, &window_y, 
						&root_x, &root_y);
		IconMenuUnpost(icon_rect->iconFrameWin, frame_icon_prop.menuWin,
					   window_x, window_y, root_x, root_y);

		return(retval);

	}

    if (IsMenuItemSensitive(window, current_num, current_item) == False) {

    	sprintf(warn_buf, _AutoMessages[SCRMSG16],
                current_num, current_item);
        AutoMessage(warn_buf);

    }

	retval = True;
	GetMenuItemLocation(frame_icon_prop.menuWin, current_num,
						frame_icon_prop.menuItemCount, &item_x, &item_y,
						&frame_icon_prop.windowMenu[current_num - 1]);

	LocatePtr(frame_icon_prop.frameWin, item_x, item_y);
	GetPointerLocation(frame_icon_prop.menuWin, &window_x, &window_y,
					   &root_x, &root_y);
	ClickMouseButton(frame_icon_prop.menuWin, Button1, 1, window_x, window_y,
					 root_x, root_y);
	GetCurrentTime(frame_icon_prop.frameWin, &time);

	XSync(display, False);

	for (i = 0; i < 25; i++)
		do_nothing();

	RefreshWindow();

	return(True);

#else

    XtPointer msg,save;
    int size = 0;
    Time time;



    time = GetTimestamp(display);

    size += sizeof(CARD32);          /* this is window id */
    size += sizeof(CARD32);          /* this is the mask we are passing in */



    save = msg = (XtPointer) XtMalloc(size);
    msg = PackCARD32(msg,(CARD32)window);
    msg = PackCARD32(msg,(CARD32)ICON_MENU_ITEM_SELECT_INFO);

    if (item_num == -1)
      strcpy (icon_item_select_data.item_name,item_name);
    icon_item_select_data.item_num = item_num;
    icon_item_select_data.keyboard = keyboard;


    cdata.operation = ICON_MENU_ITEM_SELECT;
    cdata.window = window;
    

    
    SendMessage (&cdata,save, size, time);



#endif  /* AUTOMATION */

} /* AutoWmIconMenuSelect */


/*
 * Moves the icon window to x, y relative to rootWindow
 */

#ifdef AUTOMATION
Boolean
#else
void
#endif /* AUTOMATION */
AutoWmIconMove(
        Window window,
        int x,
        int y )
{


#ifdef AUTOMATION

	IconRectInfo    *icon_rect;
	Window          event_window;
    int             i;
    int        		window_x, window_y;
    int        		root_x, root_y;
	int				x_incr, y_incr;
    int        		dest_x, dest_y;
	Atom			frame_icon_atom;


    if (GetMwmFrameIconProp(window) == False)
        return(False);

	if (frame_icon_prop.iconInfo.clientState != MWM_ICONIC_STATE) {

		for (i = 0; i < 10; i++)
			do_nothing();
			XSync(display, False);

		if (GetMwmFrameIconProp(window) == False)
			return(False);
    	if (frame_icon_prop.iconInfo.clientState != MWM_ICONIC_STATE)
        	return(False);

	}

	icon_rect = &frame_icon_prop.iconInfo;

	if (icon_rect->useIconBox == True) {
		AutoMessage(_AutoMessages[SCRMSG5]);
		return(False);
	}

    GetIconPosition(window, icon_rect, &window_x, &window_y, &root_x, &root_y);

	x_incr = icon_rect->Width / 2;
	y_incr = icon_rect->Height / 2;
	dest_x = x + x_incr;
	dest_y = y + y_incr;

        event_window = icon_rect->iconFrameWin;
	LocatePtr(event_window, root_x, root_y);
	DragPtr(event_window, Button1, dest_x, dest_y);

	frame_icon_atom = XmInternAtom(display, 
				"_MOTIF_WM_FRAME_ICON_INFO", False);

	frame_icon_prop.iconInfo.X = x;
	frame_icon_prop.iconInfo.Y = y;
	SetMwmFrameIconProp(window);

	return(True);

#else
    /* SELECTION */

    XtPointer msg,save;
    int size = 0;
    Time time;


    time = GetTimestamp(display);

    

    size += sizeof(CARD32);          /* this is window id */
    size += sizeof(CARD32);          /* this is the mask we are passing in */

    save = msg = (XtPointer) XtMalloc(size);
    msg = PackCARD32(msg,(CARD32)window);
    msg = PackCARD32(msg,(CARD32)ICON_MOVE_INFO);


    icon_move_data.move_x = x;
    icon_move_data.move_y = y;

    cdata.operation = ICON_MOVE;
    cdata.window = window;

    SendMessage (&cdata, save, size, time);
#endif

} /* AutoWmIconMove */


#ifdef AUTOMATION
Boolean
#else
void
#endif /* AUTOMATION */
AutoWmItemCheck(
        Window window,
        int group,
        char *item_name )
{


#ifdef AUTOMATION

	Boolean	retval;
	Boolean	illegal;


	switch(group) 
	    {
	      case SYSTEM_MENU:
	      if (CheckWindowMenuItem(window, item_name) == False) 
		  {
		      fprintf(stdout, "%s NOT present as part of WindowSystemMenu\n", item_name);

		  }
	      else 
		  {
		      fprintf(stdout, "%s IS present as part of WindowSystemMenu\n", item_name);

		  }
	      retval = True;

	      break;

	      case ICON_MENU:
	      if (CheckIconMenuItem(window, item_name) == False) 
		  {
		      fprintf(stdout, "%s NOT present as part of IconMenu\n", item_name);

		  }
	      else 
		  {

		      fprintf(stdout, "%s IS present as part of IconMenu\n", item_name);

		  }
	      retval = True;
	      break;

	      case WINDOW_DECORATION:
	      if (CheckWindowDecoration(window, item_name, &illegal) == False)
		  {
		      if (illegal == True)
			  retval = False;
		      else 
			  {
			      fprintf(stdout, "%s NOT present as part of decorations\n", item_name);
							
			      retval = True;

			   }

		  }
	       else 
		   {

		      fprintf(stdout, "%s IS present as part of decorations\n",item_name);
		       retval = True;

		   }
		break;

		default:

		AutoMessage(_AutoMessages[SCRMSG6]);
		retval = False;
		break;

	  }

	return(retval);

#else


    XtPointer msg,save;
    int size = 0;
    Time time;


    time = GetTimestamp(display);

    size += sizeof(CARD32);          /* this is window id */
    size += sizeof(CARD32);          /* this is the mask we are passing in */

    save = msg = (XtPointer) XtMalloc(size);
    msg = PackCARD32(msg,(CARD32)window);
    msg = PackCARD32(msg,(CARD32)WINDOW_MENU_ITEM_CHECK_INFO);


    cdata.operation = WINDOW_ITEM_CHECK;
    cdata.window = window;

    item_check_data.group = group;
    strcpy (item_check_data.item_name,item_name);

    SendMessage (&cdata, save, size, time);

#endif  /* AUTOMATION */

} /* AutoWmWindowItemCheck */



/****************************************************************************
 *                           Utility Routines
 ****************************************************************************/

static Boolean
CheckWindowMenuItem(
        Window window,
        char *item_name )
{

	int		current_num;


	XSync(display, False);
#ifdef AUTOMATION
    if (GetMwmFrameIconProp(window) == False)
        return(False);
#endif
    if (GetMenuItemNum(window, item_name, &current_num) == False)
		return(False);

	return(True);

} /* CheckWindowMenuItem */


static Boolean
CheckIconMenuItem(
        Window window,
        char *item_name )
{

	int					current_num;
	XWindowAttributes	wattrs;


	XSync(display, False);

#ifdef AUTOMATION

    if (GetMwmFrameIconProp(window) == False)
        return(False);

#endif

    if (frame_icon_prop.iconInfo.clientState != MWM_ICONIC_STATE) {

		AutoMessage(_AutoMessages[SCRMSG7]);
        return(False);
	}

	if (XGetWindowAttributes(display, frame_icon_prop.menuWin, &wattrs) == 0)
		return(False);
	if (wattrs.map_state == IsUnmapped || wattrs.map_state == IsUnviewable) {

		AutoMessage(_AutoMessages[SCRMSG8]);
		return(False);

	}

    if (GetMenuItemNum(window, item_name, &current_num) == False)
		return(False);

	return(True);

} /* CheckIconMenuItem */


static Boolean
CheckWindowDecoration(
        Window window,
        char *item_name,
        Boolean *illegal )
{

	int		i;
	Boolean	retval;

    Atom wm_state = XmInternAtom (display, "WM_STATE", False);
    struct {
            int state;
            Window icon_win;
	    } *state_info;
    unsigned long new_items, new_bytes_after;
    unsigned char *new_data;
    Atom new_type;
    int new_format;



	retval = False;
	*illegal = True;

#ifdef AUTOMATION
    if (GetMwmFrameIconProp(window) == False)
        return(retval);
    if (frame_icon_prop.iconInfo.clientState == MWM_ICONIC_STATE) {

		for (i = 0; i < 10; i++)
        	do_nothing();
        XSync(display, False);

    	if (GetMwmFrameIconProp(window) == False)
        	return(retval);
    	if (frame_icon_prop.iconInfo.clientState == MWM_ICONIC_STATE) {

	       	AutoMessage(_AutoMessages[SCRMSG9]);
        	return(retval);

		}
	      }
#else



    XGetWindowProperty (display, window, wm_state, 0, 100, False,
			AnyPropertyType, &new_type, &new_format,
			&new_items, &new_bytes_after, 
			(unsigned char **)(&state_info));


    if (state_info->state == IconicState)
      {
	for (i = 0; i < 10; i++)
	  do_nothing();
	XSync (display, False);
        XGetWindowProperty (display, window, wm_state, 0, 100, False,
			    AnyPropertyType, &new_type, &new_format,
			    &new_items, &new_bytes_after, 
			    (unsigned char **)(&state_info));
	if (state_info->state == IconicState)
	    {
		AutoMessage(_AutoMessages[SCRMSG9]);
        	return(retval);
	    }

      }

#endif  /* AUTOMATION */

	*illegal = False;
    if (frame_icon_prop.titleGadgetCount <= 0)
		return(retval);

	if (strcmp(item_name, "RTitle") == 0) {
		if (frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].id != -1)
			retval = True;
	}
	else if (strcmp(item_name, "RSystemMenu") == 0) {
		if (frame_icon_prop.titleGadgets[MWM_FRAME_SYSTEM].id != -1)
			retval = True;
	}
	else if (strcmp(item_name, "RMinimize") == 0) {
		if (frame_icon_prop.titleGadgets[MWM_FRAME_MINIMIZE].id != -1)
			retval = True;
	}
	else if (strcmp(item_name, "RMaximize") == 0) {
		if (frame_icon_prop.titleGadgets[MWM_FRAME_MAXIMIZE].id != -1)
			retval = True;
	}
	else if (strcmp(item_name, "RResizeHandles") == 0) {
		/*
		 * Since the presence of Resize Handles on a window is an
		 * ALL or NONE case, it is sufficient  to check with one
		 * gravity.
		 */
		if (frame_icon_prop.resizeGadgets[WM_NORTHWEST].id != -1)
			retval = True;
	}
	else if (strcmp(item_name, "RBorder") == 0) {
		if (frame_icon_prop.upperBorderWidth > 0 ||
		    frame_icon_prop.lowerBorderWidth > 0)
			retval = True;
	}
	else {

		AutoMessage(_AutoMessages[SCRMSG10]);
		retval = False;
		*illegal = True;

	}

	return(retval);

} /* CheckWindowDecoration */


static void
GetIconPosition(
        Window window,
        IconRectInfo *icon_rect,
        int *window_x,
        int *window_y,
        int *root_x,
        int *root_y )
{

    IconBoxRectInfo *icon_box_rect;
    Window          event_window;
    int             vert_inc_needed, horiz_inc_needed;
    int		    wx, wy, rx, ry;
    int             new_x, new_y;
    Window          dummy_child;
    Atom	    frame_icon_atom;

    if (icon_rect->useIconBox == False) {

        *window_x = icon_rect->Width / 2;
        *window_y = icon_rect->Height / 2;
        *root_x = icon_rect->X + *window_x;
        *root_y = icon_rect->Y + *window_y;

    }
    else {

        icon_box_rect = &frame_icon_prop.iconBoxInfo;
        event_window = icon_box_rect->iconScrollWin;

        /* Make the icon visible, if it is not already */
        vert_inc_needed = icon_box_rect->vert_inc_needed;
        horiz_inc_needed = icon_box_rect->horiz_inc_needed;
        if (vert_inc_needed > 0) {

            event_window = icon_box_rect->vScrollWin;
            wx = icon_box_rect->bottom_arrowX;
            wy = icon_box_rect->bottom_arrowY;
            rx = icon_box_rect->iconboxX + wx;
            ry = icon_box_rect->iconboxY + wy;
            XTranslateCoordinates(display, icon_box_rect->iconShellWin,
                                  event_window, wx, wy, 
                                  &new_x, &new_y, &dummy_child);
            wx = new_x;
            wy = new_y;
            LocatePtr(event_window, rx, ry);
            ClickMouseButton(event_window, Button1, vert_inc_needed, 
                             wx, wy, rx, ry);

        }
        else if (vert_inc_needed < 0) {

            event_window = icon_box_rect->vScrollWin;
            vert_inc_needed = -vert_inc_needed;
            wx = icon_box_rect->top_arrowX;
            wy = icon_box_rect->top_arrowY;
            rx = icon_box_rect->iconboxX + wx;
            ry = icon_box_rect->iconboxY + wy;
            XTranslateCoordinates(display, icon_box_rect->iconShellWin,
                                  event_window, wx, wy, 
                                  &new_x, &new_y, &dummy_child);
            wx = new_x;
            wy = new_y;
            LocatePtr(event_window, rx, ry);
            ClickMouseButton(event_window, Button1, vert_inc_needed, 
                             wx, wy, rx, ry);

        }
        if (horiz_inc_needed > 0) {

            event_window = icon_box_rect->hScrollWin;
            wx = icon_box_rect->right_arrowX;
            wy = icon_box_rect->right_arrowY;
            rx = icon_box_rect->iconboxX + wx;
            ry = icon_box_rect->iconboxY + wy;
            XTranslateCoordinates(display, icon_box_rect->iconShellWin,
                                  event_window, wx, wy, 
                                  &new_x, &new_y, &dummy_child);
            wx = new_x;
            wy = new_y;
            LocatePtr(event_window, rx, ry);
            ClickMouseButton(event_window, Button1, horiz_inc_needed, 
                             wx, wy, rx, ry);

        }
        else if (horiz_inc_needed < 0) {

            event_window = icon_box_rect->hScrollWin;
            horiz_inc_needed = -horiz_inc_needed;
            wx = icon_box_rect->left_arrowX;
            wy = icon_box_rect->left_arrowY;
            rx = icon_box_rect->iconboxX + wx;
            ry = icon_box_rect->iconboxY + wy;
            XTranslateCoordinates(display, icon_box_rect->iconShellWin,
                                  event_window, wx, wy, 
                                  &new_x, &new_y, &dummy_child);
            wx = new_x;
            wy = new_y;
            LocatePtr(event_window, rx, ry);
            ClickMouseButton(event_window, Button1, horiz_inc_needed, 
                             wx, wy, rx, ry);

        }

        *window_x = icon_rect->Width / 2;
        *window_y = icon_rect->Height / 2;
        *root_x = icon_box_rect->iconboxX + icon_box_rect->pointerX;
        *root_y = icon_box_rect->iconboxY + icon_box_rect->pointerY;


#ifdef AUTOMATION

		if (vert_inc_needed != 0 || horiz_inc_needed != 0) {

			frame_icon_atom = XmInternAtom(display, 
									   	   "_MOTIF_WM_FRAME_ICON_INFO", False);

			frame_icon_prop.iconBoxInfo.horiz_inc_needed =
			frame_icon_prop.iconBoxInfo.vert_inc_needed = 0;
			SetMwmFrameIconProp(window);
	
		}
#endif /* AUTOMATION */

      }


	return;

} /* GetIconPosition */



void
LocatePtr(
        Window window,
        int dest_x,
        int dest_y )
{

    int    screen_width, screen_height;
    int    window_x, window_y;
    int    root_x, root_y;
    int    x = -100;
    int    y = -100;
    int    a, b;
    double dist, tmp;

    screen_width = WidthOfScreen(screen);
    screen_height = HeightOfScreen(screen);;

    if (dest_x < 0)
        dest_x = 0;
    if (dest_x > screen_width)
        dest_x = screen_width;
    if (dest_y < 0)
        dest_y = 0;
    if (dest_y > screen_height)
        dest_y = screen_height;

    /* Move in increments */
    /* loop until position of mouse hotspot is over the destination */

    while ((x != dest_x) || (y != dest_y)) {

        GetPointerLocation(window, &window_x, &window_y, &root_x, &root_y);

        a = dest_y - root_y;         /* Given start and end points,       */
        b = -(dest_x - root_x);      /* calculate coefficients of line    */
                                     /* Ax + By + C = 0.                  */

        dist = sqrt((double)(a*a + b*b));/* Calc current distance betw points */

        if (dist < PTR_MOVE_INCREMENT) { /* If within incr range set to end pt*/
            x = dest_x;            
            y = dest_y;
        }
        else {                           /* Otherwise calc new x,y along line */
            tmp = PTR_MOVE_INCREMENT / dist;
            x = (int)(-b * tmp + root_x + 0.5);
            y = (int)(a * tmp + root_y + 0.5);
        }

        /* Move pointer to next x,y location */

        XWarpPointer(display, None, rootWindow, 0, 0, 0, 0, x, y);
		XSync(display, False);

        /* Generate motion notify events if needed */

        GetPointerLocation(window, &window_x, &window_y, &root_x, &root_y);

    }

    return;

} /* LocatePtr */


static void
DragPtr(
        Window window,
        int button,
        int dest_x,
        int dest_y )
{

    int    window_x, window_y;
    int    root_x, root_y;
    int    x = -100;
    int    y = -100;
    int    a, b;
    int    i;
    double dist, tmp;
    int    button_state = 0;
    Time   time;
    XEvent event;
    long   event_mask;
    int    screen_width, screen_height;

    screen_width = WidthOfScreen(screen);
    screen_height = HeightOfScreen(screen);;

    GetPointerLocation(window, &window_x, &window_y, &root_x, &root_y);
    GetCurrentTime(window, &time);
    PressMouseButton(window, button, button_state, 
                     window_x, window_y, root_x, root_y, time);

    /*
	 * The following 4 lines of code are here to overcome a
	 * peculiar delay problem in mwm under either heavy load
	 * or on very fast machines.
	 */
	XSync(display, False);
	for (i = 0; i < 50; i++)
		do_nothing();
	XSync(display, False);

    GetNewButtonState(button, &button_state);
    event_mask = (button == 1) ? Button1MotionMask : 
                 ((button == 2) ? Button2MotionMask : Button3MotionMask);
    /* Move in increments */
    /* loop until position of mouse hotspot is over the destination */

    while ((x != dest_x) || (y != dest_y)) {

        GetPointerLocation(window, &window_x, &window_y, &root_x, &root_y);
        if ((root_x <= 0 || root_x >= screen_width) ||
            (root_y <= 0 || root_y >= screen_height))
            break;

        a = dest_y - root_y;         /* Given start and end points,       */
        b = -(dest_x - root_x);      /* calculate coefficients of line    */
                                     /* Ax + By + C = 0.                  */

        dist = sqrt((double)(a*a + b*b));/* Calc current distance betw points */

        if (dist < PTR_MOVE_INCREMENT) { /* If within incr range set to end pt*/
            x = dest_x;            
            y = dest_y;
        }
        else {                           /* Otherwise calc new x,y along line */
            tmp = PTR_MOVE_INCREMENT / dist;
            x = (int)(-b * tmp + root_x + 0.5);
            y = (int)(a * tmp + root_y + 0.5);
        }

        /* Move pointer to next x,y location */

        XWarpPointer(display, None, rootWindow, 0, 0, 0, 0, x, y);

        GetPointerLocation(window, &window_x, &window_y, &root_x, &root_y);

        /* Send appropriate motion notify events */

        GetCurrentTime(window, &time);
        event.xmotion.type = MotionNotify;
        event.xmotion.root = rootWindow;
        event.xmotion.display = display;
        event.xmotion.window = window;
        event.xmotion.time = time;
        event.xmotion.x = window_x;
        event.xmotion.y = window_y;
        event.xmotion.x_root = root_x;
        event.xmotion.y_root = root_y;
        event.xmotion.state = button_state;
        event.xmotion.is_hint = NotifyHint;
        event.xmotion.same_screen = True;

        XSendEvent(display, window, False, event_mask, &event);
        XSync(display, False);

    }

    GetPointerLocation(window, &window_x, &window_y, &root_x, &root_y);
    GetCurrentTime(window, &time);
    ReleaseMouseButton(window, button, button_state, 
                       window_x, window_y, root_x, root_y, time);
    XSync(display, False);

    return;

} /* DragPtr */


static void
PressMouseButton(
        Window window,
        int button,
        unsigned int button_state,
        int wx,
        int wy,
        int root_x,
        int root_y,
        Time time )
{

    XEvent  event;
    long    event_mask;

    event.xbutton.type = ButtonPress;
    event.xbutton.root = rootWindow;
    event.xbutton.display = display;
    event.xbutton.window = window;
    event.xbutton.subwindow = 0;
    event.xbutton.time = time;
    event.xbutton.x = wx;
    event.xbutton.y = wy;
    event.xbutton.x_root = root_x;
    event.xbutton.y_root = root_y;
    event.xbutton.state = button_state;
    event.xbutton.button = button;
    event.xbutton.same_screen = True;

    event_mask = ButtonPressMask;

    XSendEvent(display, window, False, event_mask, &event);

    return;

} /* PressMouseButton */


static void
ReleaseMouseButton(
        Window window,
        int button,
        unsigned int button_state,
        int wx,
        int wy,
        int root_x,
        int root_y,
        Time time )
{

    XEvent  event;
    long    event_mask;

    event.xbutton.type = ButtonRelease;
    event.xbutton.root = rootWindow;
    event.xbutton.display = display;
    event.xbutton.window = window;
    event.xbutton.subwindow = 0;
    event.xbutton.time = time;
    event.xbutton.x = wx;
    event.xbutton.y = wy;
    event.xbutton.x_root = root_x;
    event.xbutton.y_root = root_y;
    event.xbutton.state = button_state;
    event.xbutton.button = button;
    event.xbutton.same_screen = True;

    event_mask = ButtonReleaseMask;

    XSendEvent(display, window, False, event_mask, &event);

} /* ReleaseMouseButton */


static void
ClickMouseButton(
        Window window,
        int button,
        unsigned int click_count,
        int wx,
        int wy,
        int root_x,
        int root_y )
{

    int    button_state;
    int    i;
    Time   time;

    GetCurrentTime(window, &time);

    for (i =0; i < click_count; i++) {

        button_state = 0;
        PressMouseButton(window, button, button_state, 
                         wx, wy, root_x, root_y, time);
        time += 1;
        GetNewButtonState(button, &button_state);
        ReleaseMouseButton(window, button, button_state, 
                           wx, wy, root_x, root_y, time);
        XSync(display, False);

    }

    return;

} /* ClickMouseButton */


static void
GetNewButtonState(
        int button,
        int *button_state )
{

    switch (button) {

        case Button1:

            *button_state |= Button1Mask;
            break;

        case Button2:

            *button_state |= Button2Mask;
            break;

        case Button3:

            *button_state |= Button3Mask;
            break;

        case Button4:

            *button_state |= Button4Mask;
            break;

        case Button5:

            *button_state |= Button5Mask;
            break;

    }

    return;

} /* GetNewButtonState */


static void
PressKey(
        Window window,
        int keycode,
        unsigned int key_state,
        int wx,
        int wy,
        int root_x,
        int root_y,
        Time time,
		Boolean postSystemMenu)
{

    XEvent  event;
    long    event_mask;

    event.xkey.type = KeyPress;
    event.xkey.display = display;
	if (postSystemMenu == False) {
    	event.xkey.window = window;
    	event.xkey.root = rootWindow;
    	event.xkey.subwindow = 0;
	}
	else {
    	event.xkey.window = rootWindow;
    	event.xkey.root = rootWindow;
    	event.xkey.subwindow = window;
	}
    event.xkey.time = time;
    event.xkey.x = wx;
    event.xkey.y = wy;
    event.xkey.x_root = root_x;
    event.xkey.y_root = root_y;
    event.xkey.state = key_state;
    event.xkey.keycode = keycode;
    event.xkey.same_screen = True;

    event_mask = KeyPressMask;

    XSendEvent(display, window, False, event_mask, &event);

    return;

} /* PressKey */


static void
ReleaseKey(
        Window window,
        int keycode,
        unsigned int key_state,
        int wx,
        int wy,
        int root_x,
        int root_y,
        Time time,
		Boolean postSystemMenu)
{

    XEvent  event;
    long    event_mask;

    event.xkey.type = KeyRelease;
    event.xkey.display = display;
	if (postSystemMenu == False) {
    	event.xkey.window = window;
    	event.xkey.root = rootWindow;
    	event.xkey.subwindow = 0;
	}
	else {
    	event.xkey.window = rootWindow;
    	event.xkey.root = rootWindow;
    	event.xkey.subwindow = window;
	}
    event.xkey.time = time;
    event.xkey.x = wx;
    event.xkey.y = wy;
    event.xkey.x_root = root_x;
    event.xkey.y_root = root_y;
    event.xkey.state = key_state;
    event.xkey.keycode = keycode;
    event.xkey.same_screen = True;

    event_mask = KeyReleaseMask;

    XSendEvent(display, window, False, event_mask, &event);

} /* ReleaseKey */


static void
ClickKey(
        Window window,
        KeySym keysym,
        int click_count,
		unsigned int key_state,
        int wx,
        int wy,
        int root_x,
        int root_y,
		Boolean postSystemMenu)
{

    int     i;
    Time    time;
	KeyCode	actual_code;

	actual_code = XKeysymToKeycode(display, keysym);

    GetCurrentTime(window, &time);

    for (i =0; i < click_count; i++) {

        PressKey(window, actual_code, key_state, wx, wy, root_x, root_y, time,
				 postSystemMenu);
        time += 1;
        ReleaseKey(window, actual_code, key_state, wx, wy, root_x, root_y, time,
				   postSystemMenu);

		xisSynchronize();

        XSync(display, False);

    }

    return;

} /* ClickKey */


static void
GetKeyButtonState(
        Window window,
        int *key_state,
        int *button_state )
{

    Window  dummy_root, dummy_child;
    int     dummy_rootx, dummy_rooty;
    int     window_x, window_y;
    unsigned int    key_buttons;

    XQueryPointer(display, window, &dummy_root, &dummy_child,
                  &dummy_rootx, &dummy_rooty, &window_x, &window_y,
                  &key_buttons);

    *button_state = key_buttons & (Button1Mask | Button2Mask | 
                     Button3Mask | Button4Mask | Button5Mask);
    *key_state = key_buttons & (ShiftMask | LockMask | ControlMask |
                 Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask);

    return;

} /* GetKeyButtonState */


static void
GetPointerLocation(
        Window window,
        int *window_x,
        int *window_y,
        int *root_x,
        int *root_y )
{

    Window  root, child;
    int     rx, ry, wx, wy;
    unsigned int    key_buttons;

    XQueryPointer(display, window, &root, &child, &rx, &ry, 
                  &wx, &wy, &key_buttons);
    *root_x = rx;
    *root_y = ry;
    *window_x = wx;
    *window_y = wy;

    return;

} /* GetPointerLocation */


static Boolean
GetWindowGeometry(
        Window window,
        int *window_x,
        int *window_y,
        unsigned int *window_width,
        unsigned int *window_height )
{

    Window              root, child;
    int                 win_x, win_y;
    unsigned int        win_width, win_height, border_width, depth;

    if (XGetGeometry(display, window, &root, &win_x, &win_y, 
                     &win_width, &win_height, 
                     &border_width, &depth) == 0) {

        fprintf(stderr, "Error: Unable to get window geometry\n");
        return(False);

    }

    XTranslateCoordinates(display, window, root, 0, 0, &win_x, &win_y, &child);
    *window_x = win_x;
    *window_y = win_y;
    *window_width = win_width;
    *window_height = win_height;

    return(True);

} /* GetWindowGeometry */


static void
GetCurrentTime(
        Window window,
        Time *time )
{

    static Boolean  first = True;
    static Window   syncWindow;
    static Atom     my_property;
    static Atom     property_type;
    XEvent          event;
    static unsigned char property_value = 0;

    if (first == True) {

        syncWindow = XCreateSimpleWindow(display, rootWindow, 0, 0, 10, 10,
                                         0, 0, 0);
        XSelectInput(display, syncWindow, PropertyChangeMask);
        first = False;

    }

    my_property = XInternAtom(display,"my property",False);
    property_type  = XInternAtom(display,"INTEGER",True);

    XChangeProperty(display, syncWindow, my_property, property_type, 
                    8, PropModeReplace, &property_value, 1);

    while(XCheckTypedWindowEvent(display, syncWindow, PropertyNotify, 
                                 &event) == False)
       ;

    *time = event.xproperty.time;

    return;

} /* GetCurrentTime */


static Boolean
IsMenuItemSensitive(
        Window window,
        int item_num,
        char *item_name )
{

    Boolean     retval;
    MenuInfo    *menu_info;

    retval = False;

    menu_info = &frame_icon_prop.windowMenu[0];

    strcpy(item_name, menu_info[item_num - 1].item_name);
    retval = menu_info[item_num - 1].sensitive;

    return(retval);

} /* IsMenuItemSensitive */


static void
GetMenuItemLocation(
        Window window,
        int item_num,
        int total_items,
        int *item_x,
        int *item_y,
	MenuInfo *menu_info )
{

	int		i;
	XWindowAttributes	w_attrs;
	int	item_height, new_x, new_y;
	Window	child;

	XSync(display, False);
	for (i = 0; i < 10; i++)
		do_nothing();

	XGetWindowAttributes(display, window, &w_attrs);
	XTranslateCoordinates(display, window, rootWindow, w_attrs.x, w_attrs.y, 
					  	  &new_x, &new_y, &child);

#ifdef DEBUG
	fprintf(stderr, "Inside GetMenuItemLocation: \n");
	fprintf(stderr, "x = %d y = %d width = %d height = %d border_width = %d\n",
			w_attrs.x, w_attrs.y, w_attrs.width, w_attrs.height, 
			w_attrs.border_width);
	fprintf(stderr, "new_x = %d new_y = %d\n", new_x, new_y);
	fflush(stderr);
#endif

    *item_x = new_x + (w_attrs.width / 2);
    item_height = w_attrs.height / total_items;
	*item_y = new_y + menu_info->item_y;

    return;

} /* GetMenuItemLocation */


static Boolean
GetMenuItemNum(
        Window window,
        char *item_name,
        int *item_num )
{

    int         i;
    Boolean     retval;
    MenuInfo    *menu_info;

    retval = False;
    *item_num = -1;

    menu_info = &frame_icon_prop.windowMenu[0];

    for (i = 0; i < frame_icon_prop.menuItemCount && i < MAX_MENU_ITEMS; i++) {

        if (strcmp(menu_info->item_name, item_name) == 0) {

            *item_num = i + 1;
            retval = True;
            break;

        }
        menu_info++;
    }

    return(retval);

} /* GetMenuItemNum */


static Boolean
GetNumClicks(
        Window window,
        int item_num,
        int *num_clicks )
{

    int         i;
    int         sensitive_count;
    Boolean     retval;
    MenuInfo    *menu_info;

    retval = False;
    *num_clicks = 0;

    menu_info = &frame_icon_prop.windowMenu[0];

    sensitive_count = 0;
    for (i = 0; i < frame_icon_prop.menuItemCount && i < MAX_MENU_ITEMS; i++) {

        if (i == (item_num - 1)) {

            *num_clicks = sensitive_count;
            retval = True;
            break;

        }
        /* If item not sensitive, skip it */
        if (menu_info->sensitive == True)
            sensitive_count++;
        menu_info++;

    }

    return(retval);

} /* GetNumClicks */



static void
do_nothing( void )
{
    fd_set bogus;
    struct timeval timeoutval;

    timeoutval.tv_sec = 0;
    timeoutval.tv_usec = DO_NOTHING_TIME;
    select(0, NULL, NULL, NULL, &timeoutval);
    return;
} /* do_nothing */


/*
 * This routine makes sure that all expose events are sent to server.
 * This has been made a separate routine with the view that down
 * the road there might be more things that need to be done, than
 * just calling xisSynchronize.
 */

static void
RefreshWindow( void )
{

	xisSynchronize();

	return;

} /* RefreshWindow */






/***************************************************************************
 * Routines needed when using UTM for mwm automation
 ***************************************************************************/

#ifndef AUTOMATION

static Boolean
DoResize(

        Window window,
        XtPointer data)
{


    AutoRectangle *rect;
    AutoRectangle *title_rect;
    int        window_x, window_y;
    int        root_x, root_y;
    int        dest_x, dest_y;
    int        frame_x, frame_y;
    unsigned int window_width, window_height;
    int        x_incr, y_incr;
    int        act_width, act_height;
    Time       time;
    int        i;
    Window     event_window;
    int width, height, gravity;


    struct {
            int state;
            Window icon_win;
	    } *state_info;
    unsigned long new_items, new_bytes_after;
    unsigned char *new_data;
    Atom new_type;
    int new_format;
    int title_height;

    Atom wm_state = XmInternAtom (display, "WM_STATE", False);




    /* 
     * data needed to do the resize
     */

    width = resize_data.width;
    height = resize_data.height;
    gravity = resize_data.gravity;

    if (width < 0) 
      {
        AutoMessage(_AutoMessages[SCRMSG4]);
        return False;
      }


    XGetWindowProperty (display, window, wm_state, 0, 100, False,
			AnyPropertyType, &new_type, &new_format,
			&new_items, &new_bytes_after, 
			(unsigned char **)(&state_info));


    if (state_info->state == IconicState) 
      {

   	for (i = 0; i < 10; i++)
        	do_nothing();
        XSync(display, False);
        XGetWindowProperty (display, window, wm_state, 0, 100, False,
			AnyPropertyType, &new_type, &new_format,
			&new_items, &new_bytes_after, 
			(unsigned char **)(&state_info));
        if (state_info->state == IconicState) 
	  return False;
      }



    /* Unpack the data here */

	/* resizeGadgets[WM_EAST].id */
	frame_icon_prop.resizeGadgets[WM_EAST].id = UnpackCARD32(&data);    
  
	/* resizeGadgets[WM_EAST].rect.width */  
       frame_icon_prop.resizeGadgets[WM_EAST].rect.width = UnpackCARD32(&data); 

	/* resizeGadgets[WM_WEST].id */
       frame_icon_prop.resizeGadgets[WM_WEST].id = UnpackCARD32(&data);    
	
  
	/* resizeGadgets[WM_WEST].rect.width */  
      frame_icon_prop.resizeGadgets[WM_WEST].rect.width = UnpackCARD32(&data); 

      frame_icon_prop.resizeGadgets[gravity].id = UnpackCARD32(&data);    
      frame_icon_prop.resizeGadgets[gravity].rect.x = UnpackCARD32(&data); 
      frame_icon_prop.resizeGadgets[gravity].rect.y = UnpackCARD32(&data); 
      frame_icon_prop.resizeGadgets[gravity].rect.width = UnpackCARD32(&data); 
      frame_icon_prop.resizeGadgets[gravity].rect.height = UnpackCARD32(&data); 
  frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].id = UnpackCARD32 (&data);
  frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect.x = UnpackCARD32 (&data);
  frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect.y = UnpackCARD32 (&data);    
  frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect.width = UnpackCARD32 (&data);
  frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect.height = UnpackCARD32 (&data);
  
    frame_icon_prop.upperBorderWidth = UnpackCARD32(&data);         
    frame_icon_prop.lowerBorderWidth = UnpackCARD32(&data);         
    frame_icon_prop.frameWin = UnpackCARD32(&data);         




    if (frame_icon_prop.resizeGadgets[gravity].id == INVALID)
      return False;



    event_window = frame_icon_prop.frameWin;

    if (GetWindowGeometry(event_window, &frame_x, &frame_y, 
                          &window_width, &window_height) == False)
        return(False);

    title_rect = &frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect;
    

    if (frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].id == INVALID)
      title_height = 0;
    else
      title_height = title_rect->height;


    /*
     * Compute the actual new width and new height to which the
     * window is to be resized. If they are equal to the current
     * width and height of the window, return.
     */


    
    if (frame_icon_prop.resizeGadgets[WM_EAST].id == INVALID)
      x_incr = frame_icon_prop.lowerBorderWidth;
    else
      x_incr = frame_icon_prop.resizeGadgets[WM_EAST].rect.width;
	
    y_incr = title_height + frame_icon_prop.lowerBorderWidth +
             frame_icon_prop.upperBorderWidth;

    act_width = width + (2 * x_incr);
    act_height = height + y_incr;
    if (act_width == window_width && act_height == window_height)
        return(True);

    /* Compute the x and y increments/decrements to get new size */
    x_incr = y_incr = 0;
    switch (gravity) {

        case WM_NORTH:
        case WM_SOUTH:

            y_incr = width - window_height; /* New width treated as height */
            break;

        case WM_EAST:
        case WM_WEST:

            x_incr = width - window_width;
            break;

        case WM_NORTHEAST:
        case WM_NORTHWEST:
        case WM_SOUTHEAST:
        case WM_SOUTHWEST:

            x_incr = width - window_width;
            y_incr = (height > 0) ? (height - window_height) : 0;
            break;

    }

    rect = &frame_icon_prop.resizeGadgets[gravity].rect;

    /*
     * Compute the starting point to place the pointer to start resizing
     * depending on gravity.
     */
    switch (gravity) {

        case WM_NORTH:

            window_x = rect->x + (rect->width / 2);
            window_y = rect->y;
            break;

        case WM_SOUTH:

            window_x = rect->x + (rect->width / 2);
            window_y = rect->y + rect->height - 1;
            break;

        case WM_EAST:

            window_x = rect->x + rect->width - 1;
            window_y = rect->y + (rect->height / 2);
            break;

        case WM_WEST:

            window_x = rect->x;
            window_y = rect->y + (rect->height / 2);
            break;

        case WM_NORTHEAST:

            window_x =  rect->x + rect->width - 1;
            window_y = rect->y;
            break;

        case WM_NORTHWEST:

            window_x = rect->x;
            window_y = rect->y;
            break;

        case WM_SOUTHEAST:

            window_x = rect->x + rect->width - 1;
            window_y = rect->y + rect->height - 1;
            break;

        case WM_SOUTHWEST:

            window_x = rect->x;
            window_y = rect->y + rect->height - 1;
            break;

    }

    root_x = frame_x + window_x;
    root_y = frame_y + window_y;

    /* 
     * Compute the destination x, y depending on gravity,
     * and the computed x_incr, y_incr values (+ve or -ve).
     */
    switch (gravity) {

        case WM_NORTH:

            dest_x = root_x;
			dest_y = root_y;
            if (y_incr > 0)    /* Increase height, meaning decrease y */
                dest_y -= y_incr;
            if (y_incr < 0)    /* Decrease height, meaning increase y */
                dest_y -= y_incr;
            dest_y -= (title_height + frame_icon_prop.upperBorderWidth +
		       frame_icon_prop.lowerBorderWidth);

            break;

        case WM_SOUTH:

            dest_x = root_x;
            dest_y = root_y + y_incr; /* If y_incr < 0, height is decreased */
            dest_y += (title_height + frame_icon_prop.upperBorderWidth +
		       frame_icon_prop.lowerBorderWidth);
            break;

        case WM_EAST:

            dest_y = root_y;
            dest_x = root_x + x_incr; /* If x_incr < 0, width is decreased */
            dest_x += (2 * rect->width);
            break;

        case WM_WEST:

            dest_y = root_y;
            dest_x = root_x;
            if (x_incr > 0)    /* Increase width, meaning decrease x */
                dest_x -= x_incr;
            if (x_incr < 0)    /* Decrease width, meaning increase x */
                dest_x -= x_incr;
            dest_x -= (2 * rect->width);
            break;

        case WM_NORTHEAST:

            dest_x = root_x + x_incr; /* If x_incr < 0, width is decreased */
			dest_y = root_y;
            if (y_incr > 0)    /* Increase height, meaning decrease y */
                dest_y -= y_incr;
            if (y_incr < 0)    /* Decrease height, meaning increase y */
                dest_y -= y_incr;
	    rect = &frame_icon_prop.resizeGadgets[WM_EAST].rect;
            dest_x += (2 * rect->width);
			if (height > 0)
            dest_y -= (title_height + frame_icon_prop.upperBorderWidth +
		       frame_icon_prop.lowerBorderWidth);
            break;

        case WM_NORTHWEST:

			dest_x = root_x;
			dest_y = root_y;
            if (x_incr > 0)    /* Increase width, meaning decrease x */
                dest_x -= x_incr;
            if (x_incr < 0)    /* Decrease width, meaning increase x */
                dest_x -= x_incr;
            if (y_incr > 0)    /* Increase height, meaning decrease y */
                dest_y -= y_incr;
            if (y_incr < 0)    /* Decrease height, meaning increase y */
                dest_y -= y_incr;
	    rect = &frame_icon_prop.resizeGadgets[WM_WEST].rect;
            dest_x -= (2 * rect->width);
			if (height > 0)
            dest_y -= (title_height + frame_icon_prop.upperBorderWidth +
		       frame_icon_prop.lowerBorderWidth);

            break;

        case WM_SOUTHEAST:

            dest_x = root_x + x_incr; /* If x_incr < 0, width is decreased */
            dest_y = root_y + y_incr; /* If y_incr < 0, height is decreased */
	    rect = &frame_icon_prop.resizeGadgets[WM_EAST].rect;
            dest_x += (2 * rect->width);
			if (height > 0)
            dest_y += (title_height + frame_icon_prop.upperBorderWidth +
		       frame_icon_prop.lowerBorderWidth);

            break;

        case WM_SOUTHWEST:

            if (x_incr > 0)    /* Increase width, meaning decrease x */
                dest_x = root_x - x_incr;
            if (x_incr < 0)    /* Decrease width, meaning increase x */
                dest_x = root_x - x_incr;
            dest_y = root_y + y_incr; /* If y_incr < 0, height is decreased */
	    rect = &frame_icon_prop.resizeGadgets[WM_WEST].rect;
            dest_x -= (2 * rect->width);
			if (height > 0)
            dest_y += (title_height + frame_icon_prop.upperBorderWidth +
		       frame_icon_prop.lowerBorderWidth);
            break;

        default:

            AutoError(_AutoMessages[WARNMSG91]);
            return(False);

    }

    LocatePtr(event_window, root_x, root_y);

    /*
	 * The following 4 lines of code are here to overcome a
	 * peculiar delay problem in mwm under either heavy load
	 * or on very fast machines.
	 */
	XSync(display, False);
	for (i = 0; i < 50; i++)
		do_nothing();
	XSync(display, False);


    DragPtr(event_window, Button1, dest_x, dest_y);
        
    GetCurrentTime(event_window, &time);

	RefreshWindow();

    return(True);
  }

#endif /* AUTOMATION */



static Boolean
DoDeiconify(Window window, XtPointer data)
{


    IconRectInfo    *icon_rect;
    int             window_x, window_y, root_x, root_y;
    Window          event_window;
    Time            time;
    int             i;

    int last_col,last_row,i_place_h,i_place_w;
    int icon_x,icon_y;
    IconBoxInfo icon_box_info;


    struct {
            int state;
            Window icon_win;
	    } *state_info;
    unsigned long new_items, new_bytes_after;
    unsigned char *new_data;
    Atom new_type;
    int new_format;

    Atom wm_state = XmInternAtom (display, "WM_STATE", False);

    XGetWindowProperty (display, window, wm_state, 0, 100, False,
			AnyPropertyType, &new_type, &new_format,
			&new_items, &new_bytes_after, 
			(unsigned char **)(&state_info));


    if (state_info->state != IconicState)
      {
	for (i = 0; i < 10; i++)
	  do_nothing();
	XSync (display, False);
        XGetWindowProperty (display, window, wm_state, 0, 100, False,
			    AnyPropertyType, &new_type, &new_format,
			    &new_items, &new_bytes_after, 
			    (unsigned char **)(&state_info));
	if (state_info->state != IconicState)
	    return False;

      }


    frame_icon_prop.iconInfo.useIconBox = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.X = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.Y = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.Width = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.Height = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.iconFrameWin = UnpackCARD32 (&data);

    if (frame_icon_prop.iconInfo.useIconBox == True)
      {
	icon_box_info.h_min = UnpackCARD32 (&data);
	icon_box_info.h_max = UnpackCARD32 (&data);
	icon_box_info.h_slider_area_width = UnpackCARD32 (&data);
	icon_box_info.h_slider_x = UnpackCARD32 (&data);
	icon_box_info.h_slider_area_x = UnpackCARD32 (&data);
	icon_box_info.v_min = UnpackCARD32 (&data);
	icon_box_info.v_max = UnpackCARD32 (&data);
	icon_box_info.v_slider_area_height = UnpackCARD32 (&data);
	icon_box_info.v_slider_y = UnpackCARD32 (&data);
	icon_box_info.v_slider_area_y = UnpackCARD32 (&data);
	icon_box_info.right_arrow_x = UnpackCARD32 (&data);	
	icon_box_info.right_arrow_y = UnpackCARD32 (&data);
	icon_box_info.left_arrow_x = UnpackCARD32 (&data);	
	icon_box_info.left_arrow_y = UnpackCARD32 (&data);
	icon_box_info.top_arrow_x = UnpackCARD32 (&data);	
	icon_box_info.top_arrow_y = UnpackCARD32 (&data);
	icon_box_info.bottom_arrow_x = UnpackCARD32 (&data);	
	icon_box_info.bottom_arrow_y = UnpackCARD32 (&data);
	icon_box_info.icon_box_x = UnpackCARD32 (&data);	
	icon_box_info.icon_box_y = UnpackCARD32 (&data);
	icon_box_info.icon_box_width = UnpackCARD32 (&data);	
	icon_box_info.icon_box_height = UnpackCARD32 (&data);
	icon_box_info.icon_shell_win = UnpackCARD32 (&data);
	icon_box_info.frame_win = UnpackCARD32 (&data);
	icon_box_info.scroll_win = UnpackCARD32 (&data);
	icon_box_info.h_scroll_win = UnpackCARD32 (&data);
	icon_box_info.v_scroll_win = UnpackCARD32 (&data);
	icon_box_info.last_col = UnpackCARD32 (&data);
	icon_box_info.last_row = UnpackCARD32 (&data);
	icon_box_info.i_place_h = UnpackCARD32 (&data);
	icon_box_info.i_place_w = UnpackCARD32 (&data);

      }

    icon_x = frame_icon_prop.iconInfo.X;
    icon_y = frame_icon_prop.iconInfo.X;

    if (frame_icon_prop.iconInfo.useIconBox)
        
    FillIconBoxInfo (&frame_icon_prop, &icon_box_info,icon_x,icon_y);

    icon_rect = &frame_icon_prop.iconInfo;

    GetIconPosition(window, icon_rect, &window_x, &window_y, &root_x, &root_y);

    event_window = icon_rect->iconFrameWin;

    LocatePtr(event_window, root_x, root_y);
    ClickMouseButton(event_window, Button1, 2,
                     window_x, window_y, root_x, root_y);

    GetCurrentTime(frame_icon_prop.frameWin, &time);

        /* Wait till the internal window state changes from ICONIC */

    XGetWindowProperty (display, window, wm_state, 0, 100, False,
			    AnyPropertyType, &new_type, &new_format,
			    &new_items, &new_bytes_after, 
			    (unsigned char **)(&state_info));

    while (state_info->state == IconicState)
      {
	do_nothing();
	XSync (display,False);
        XGetWindowProperty (display, window, wm_state, 0, 100, False,
			    AnyPropertyType, &new_type, &new_format,
			    &new_items, &new_bytes_after, 
			    (unsigned char **)(&state_info));
      }

      RefreshWindow();

    return(True);

}


#ifndef AUTOMATION

static Boolean
DoMenuPost(Window window, XtPointer data)
{

    AutoRectangle *rect;
    int        window_x, window_y;
    int        frame_x, frame_y;
    int        root_x, root_y;
    Window     event_window;
    Time       time;
    unsigned int    window_width, window_height;
	int		   i;
    Boolean keyboard;

    struct {
            int state;
            Window icon_win;
	    } *state_info;
    unsigned long new_items, new_bytes_after;
    unsigned char *new_data;
    Atom new_type;
    int new_format;

    Atom wm_state = XmInternAtom (display, "WM_STATE", False);

    XGetWindowProperty (display, window, wm_state, 0, 100, False,
			AnyPropertyType, &new_type, &new_format,
			&new_items, &new_bytes_after, 
			(unsigned char **)(&state_info));


    if (state_info->state == IconicState)
      {
	for (i = 0; i < 10; i++)
	  do_nothing();
	XSync (display, False);
        XGetWindowProperty (display, window, wm_state, 0, 100, False,
			    AnyPropertyType, &new_type, &new_format,
			    &new_items, &new_bytes_after, 
			    (unsigned char **)(&state_info));
	if (state_info->state == IconicState)
	    return False;

      }

    /*
     * Unpack data from UTM 
     */
  frame_icon_prop.titleGadgetCount = UnpackCARD32 (&data);
  frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].id = UnpackCARD32 (&data);
  frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect.x = UnpackCARD32 (&data);
  frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect.y = UnpackCARD32 (&data);    
  frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect.width = UnpackCARD32 (&data);
  frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect.height = UnpackCARD32 (&data);
  frame_icon_prop.titleGadgets[MWM_FRAME_SYSTEM].id = UnpackCARD32 (&data);
  frame_icon_prop.titleGadgets[MWM_FRAME_SYSTEM].rect.x = UnpackCARD32 (&data);
  frame_icon_prop.titleGadgets[MWM_FRAME_SYSTEM].rect.y = UnpackCARD32 (&data);    
  frame_icon_prop.titleGadgets[MWM_FRAME_SYSTEM].rect.width = UnpackCARD32 (&data);
  frame_icon_prop.titleGadgets[MWM_FRAME_SYSTEM].rect.height = UnpackCARD32 (&data);
  frame_icon_prop.frameWin = UnpackCARD32 (&data);


    keyboard = window_menu_post_data.keyboard;
	if (keyboard == True) {

    	if (GetWindowGeometry(frame_icon_prop.frameWin, &frame_x, &frame_y, 
                          	  &window_width, &window_height) == False)
        	return(False);

		/*
		 * If title region present, locate pointer to middle of title
		 * region; else, locate pointer to middle of the window.
		 */
		if (frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].id != INVALID) {

    		rect = &frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect;
    		window_x = rect->x + (rect->width / 2);
    		window_y = rect->y + (rect->height / 2);
    		root_x = frame_x + window_x;
    		root_y = frame_y + window_y;

		}

		else {
			
			window_x = window_width / 2;
			window_y = window_height / 2;
			root_x = frame_x + window_x;
			root_y = frame_y + window_y;

		}

    	LocatePtr(frame_icon_prop.frameWin, root_x, root_y);
	GetCurrentTime(frame_icon_prop.frameWin, &time);
    	ClickKey(frame_icon_prop.frameWin, XK_space, 1, 
		 Mod1Mask, window_x, window_y, root_x, root_y, True);

		GetCurrentTime(frame_icon_prop.frameWin, &time);
		return(True);

	}

    if (frame_icon_prop.titleGadgetCount <= 0 ||
       	frame_icon_prop.titleGadgets[MWM_FRAME_SYSTEM].id == INVALID)
       	return(False);

    event_window = frame_icon_prop.frameWin;

    if (GetWindowGeometry(event_window, &frame_x, &frame_y, 
                          &window_width, &window_height) == False)
        return(False);

    rect = &frame_icon_prop.titleGadgets[MWM_FRAME_SYSTEM].rect;
    window_x = rect->x + (rect->width / 2);
    window_y = rect->y + (rect->height / 2);
    root_x = frame_x + window_x;
    root_y = frame_y + window_y;
    LocatePtr(event_window, root_x, root_y);
    GetCurrentTime(frame_icon_prop.frameWin, &time);
    PressMouseButton(frame_icon_prop.frameWin, Button1, 0, 
                      window_x, window_y, root_x, root_y, time);

    GetCurrentTime(frame_icon_prop.frameWin, &time);
    return(True);

}

#endif /* AUTOMATION */


static Boolean
DoMenuUnpost(Window window, XtPointer data)
{

    int       root_x, root_y;
    int       window_x, window_y;
    Time      time;
    AutoRectangle *rect;
    int        frame_x, frame_y;
    unsigned int    window_width, window_height;

    /* Unpack data from UTM */

    frame_icon_prop.iconInfo.clientState = UnpackCARD32 (&data);
    frame_icon_prop.menuWin = UnpackCARD32 (&data);
    frame_icon_prop.frameWin = UnpackCARD32 (&data);


    if (frame_icon_prop.iconInfo.clientState == MWM_ICONIC_STATE)
        return(False);


    WindowMenuUnpost(frame_icon_prop.frameWin, frame_icon_prop.menuWin);

    return(True);

}

static Boolean
DoFocus(Window window, XtPointer data)
{

    AutoRectangle *title_rect;
    int        window_x, window_y;
    int        root_x, root_y;
    int        frame_x, frame_y;
    unsigned int window_width, window_height;
    Window     event_window;
    int        i;

 struct {
            int state;
            Window icon_win;
	    } *state_info;
    unsigned long new_items, new_bytes_after;
    unsigned char *new_data;
    Atom new_type;
    int new_format;

    Atom wm_state = XmInternAtom (display, "WM_STATE", False);

    XGetWindowProperty (display, window, wm_state, 0, 100, False,
			AnyPropertyType, &new_type, &new_format,
			&new_items, &new_bytes_after, 
			(unsigned char **)(&state_info));


    if (state_info->state == IconicState)
      {
	for (i = 0; i < 10; i++)
	  do_nothing();
	XSync (display, False);
        XGetWindowProperty (display, window, wm_state, 0, 100, False,
			    AnyPropertyType, &new_type, &new_format,
			    &new_items, &new_bytes_after, 
			    (unsigned char **)(&state_info));
	if (state_info->state == IconicState)
	    return False;

      }


    /* unpack data from UTM */

  frame_icon_prop.titleGadgetCount = UnpackCARD32 (&data);
  frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].id = UnpackCARD32 (&data);
  frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect.x = UnpackCARD32 (&data);
  frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect.y = UnpackCARD32 (&data);    
  frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect.width = UnpackCARD32 (&data);
  frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect.height = UnpackCARD32 (&data);    
frame_icon_prop.frameWin = UnpackCARD32 (&data);    


    if (frame_icon_prop.titleGadgetCount <= 0 ||
        frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].id == INVALID)
        return(False);

    event_window = frame_icon_prop.frameWin;
    if (GetWindowGeometry(event_window, &frame_x, &frame_y, 
                          &window_width, &window_height) == False)
        return(False);

    title_rect = &frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect;
    window_x = title_rect->x + (title_rect->width / 2);
    window_y = title_rect->y + (title_rect->height / 2);
    root_x = frame_x + window_x;
    root_y = frame_y + window_y;

    LocatePtr(event_window, root_x, root_y);
    ClickMouseButton(frame_icon_prop.frameWin, Button1, 1, window_x, window_y,
                     root_x, root_y);

    return(True); 

  }

#ifndef AUTOMATION

static Boolean
DoWindowItemCheck(Window window, XtPointer data)
{

    Boolean retval;
    Boolean illegal;
    String temp_string;
    int group;
    char item_name[MAX_NAME_LEN + 1];
    int i;

    group = item_check_data.group;
    strcpy (item_name,item_check_data.item_name);
    /*
     * Unpack data from UTM 
     */

     
     frame_icon_prop.iconInfo.clientState = UnpackCARD32 (&data);
     frame_icon_prop.menuWin = UnpackCARD32 (&data);
     frame_icon_prop.menuItemCount = UnpackCARD32 (&data);
     for (i=0; i < frame_icon_prop.menuItemCount && i < MAX_MENU_ITEMS; i++)
	 {
	     temp_string = (char *)UnpackString(&data);
	     strcpy (frame_icon_prop.windowMenu[i].item_name, temp_string);
	 }
     
     frame_icon_prop.titleGadgetCount = UnpackCARD32 (&data);
     frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].id = UnpackCARD32 (&data);
     frame_icon_prop.titleGadgets[MWM_FRAME_SYSTEM].id = UnpackCARD32 (&data);
     frame_icon_prop.titleGadgets[MWM_FRAME_MINIMIZE].id = UnpackCARD32 (&data);
     frame_icon_prop.titleGadgets[MWM_FRAME_MAXIMIZE].id = UnpackCARD32 (&data);
     frame_icon_prop.resizeGadgets[WM_NORTHWEST].id = UnpackCARD32 (&data);
     frame_icon_prop.upperBorderWidth = UnpackCARD32 (&data);
     frame_icon_prop.lowerBorderWidth = UnpackCARD32 (&data);
		                            




    switch(group) 
	{
	  case SYSTEM_MENU:


	    if (CheckWindowMenuItem(window, item_name) == False) 
		{
		    fprintf(stdout, "%s NOT present as part of WindowSystemMenu\n", item_name);

		 }
	    else 
		 {
		    fprintf(stdout, "%s IS present as part of WindowSystemMenu\n", item_name);

		  }
	    retval = True;

	    break;

	
	  case ICON_MENU:

	    if (CheckIconMenuItem(window, item_name) == False) 
		{
		    fprintf(stdout, "%s NOT present as part of IconMenu\n",item_name);

		}
	    else 
		{

	            fprintf(stdout, "%s IS present as part of IconMenu\n", item_name);

	        }
	      retval = True;
	      break;

	    
	  case WINDOW_DECORATION:

	    if (CheckWindowDecoration(window, item_name, &illegal) == False)
		{
		    if (illegal == True)
			retval = False;
		     else 
			  {
			      fprintf(stdout, "%s NOT present as part of decorations\n", item_name);
							
			      retval = True;

			   }

		  }
	       else 
		   {

		      fprintf(stdout, "%s IS present as part of decorations\n", item_name);
		       retval = True;

		   }
		break;

		default:

		AutoMessage(_AutoMessages[SCRMSG6]);
		retval = False;
		break;

	  }

	return(retval);
}



static Boolean
DoIconMenuPost(Window window, XtPointer data)

{


    IconRectInfo    	        *icon_rect;
    Window          	        event_window;
    int             	        i;
    int        			window_x, window_y;
    int        			root_x, root_y;
    Time       			time;
    XWindowAttributes	        wattrs;
    IconBoxInfo                 icon_box_info;
    int                         icon_x,icon_y;

   struct {
            int state;
            Window icon_win;
	    } *state_info;
    unsigned long new_items, new_bytes_after;
    unsigned char *new_data;
    Atom new_type;
    int new_format;
    Boolean keyboard;

    Atom wm_state = XmInternAtom (display, "WM_STATE", False);

    XGetWindowProperty (display, window, wm_state, 0, 100, False,
			AnyPropertyType, &new_type, &new_format,
			&new_items, &new_bytes_after, 
			(unsigned char **)(&state_info));




    if (state_info->state != IconicState)
      {
	for (i = 0; i < 10; i++)
	  do_nothing();
	XSync (display, False);
        XGetWindowProperty (display, window, wm_state, 0, 100, False,
			    AnyPropertyType, &new_type, &new_format,
			    &new_items, &new_bytes_after, 
			    (unsigned char **)(&state_info));
	if (state_info->state != IconicState)
	    return False;

      }

    keyboard = icon_menu_post_data.keyboard;

    frame_icon_prop.iconInfo.useIconBox = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.X = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.Y = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.Width = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.Height = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.iconFrameWin = UnpackCARD32 (&data);


    if (frame_icon_prop.iconInfo.useIconBox == True)
      {
	icon_box_info.h_min = UnpackCARD32 (&data);
	icon_box_info.h_max = UnpackCARD32 (&data);
	icon_box_info.h_slider_area_width = UnpackCARD32 (&data);
	icon_box_info.h_slider_x = UnpackCARD32 (&data);
	icon_box_info.h_slider_area_x = UnpackCARD32 (&data);
	icon_box_info.v_min = UnpackCARD32 (&data);
	icon_box_info.v_max = UnpackCARD32 (&data);
	icon_box_info.v_slider_area_height = UnpackCARD32 (&data);
	icon_box_info.v_slider_y = UnpackCARD32 (&data);
	icon_box_info.v_slider_area_y = UnpackCARD32 (&data);
	icon_box_info.right_arrow_x = UnpackCARD32 (&data);	
	icon_box_info.right_arrow_y = UnpackCARD32 (&data);
	icon_box_info.left_arrow_x = UnpackCARD32 (&data);	
	icon_box_info.left_arrow_y = UnpackCARD32 (&data);
	icon_box_info.top_arrow_x = UnpackCARD32 (&data);	
	icon_box_info.top_arrow_y = UnpackCARD32 (&data);
	icon_box_info.bottom_arrow_x = UnpackCARD32 (&data);	
	icon_box_info.bottom_arrow_y = UnpackCARD32 (&data);
	icon_box_info.icon_box_x = UnpackCARD32 (&data);	
	icon_box_info.icon_box_y = UnpackCARD32 (&data);
	icon_box_info.icon_box_width = UnpackCARD32 (&data);	
	icon_box_info.icon_box_height = UnpackCARD32 (&data);
	icon_box_info.icon_shell_win = UnpackCARD32 (&data);
	icon_box_info.frame_win = UnpackCARD32 (&data);
	icon_box_info.scroll_win = UnpackCARD32 (&data);
	icon_box_info.h_scroll_win = UnpackCARD32 (&data);
	icon_box_info.v_scroll_win = UnpackCARD32 (&data);
	icon_box_info.last_col = UnpackCARD32 (&data);
	icon_box_info.last_row = UnpackCARD32 (&data);
	icon_box_info.i_place_w = UnpackCARD32 (&data);
	icon_box_info.i_place_h = UnpackCARD32 (&data);

      }

    icon_x = frame_icon_prop.iconInfo.X;
    icon_y = frame_icon_prop.iconInfo.X;

    if (frame_icon_prop.iconInfo.useIconBox)
        
	FillIconBoxInfo (&frame_icon_prop, &icon_box_info,icon_x,icon_y);

 	/* 
	 * If icon is inside an IconBox, iconMenu can not be posted via
	 * keyboard.
	 */
	if (keyboard == True && frame_icon_prop.iconInfo.useIconBox == True)
		return(False);

	/*
	 * Raise icon window for posting iconMenu via keyboard
	 */
	if (keyboard == True)
		XRaiseWindow(display, window);

	icon_rect = &frame_icon_prop.iconInfo;
    event_window = icon_rect->iconFrameWin;
	XGetWindowAttributes(display, event_window, &wattrs);

    GetIconPosition(window, icon_rect, &window_x, &window_y, &root_x, &root_y);

	LocatePtr(event_window, root_x, root_y);

	if (keyboard == True) {

		GetCurrentTime(event_window, &time);
    	ClickKey(event_window, XK_space, 1, 
				 Mod1Mask, window_x, window_y, root_x, root_y, False);

		GetCurrentTime(event_window, &time);
		return(True);

	}

    ClickMouseButton(event_window, Button1, 1,
					 window_x, window_y, root_x, root_y);

	GetCurrentTime(event_window, &time);

	return(True);
}

#endif /* AUTOMATION */








static Boolean
DoIconMenuUnpost(
        Window window,
	XtPointer data)
{


	IconRectInfo    	*icon_rect;
	Window          	event_window;
	int        			window_x, window_y;
	int        			root_x, root_y;
	int		  			new_x, new_y;
	Time       			time;
	XWindowAttributes	wattrs;



    IconBoxInfo                 icon_box_info;
    int                         icon_x,icon_y;

   struct {
            int state;
            Window icon_win;
	    } *state_info;
    unsigned long new_items, new_bytes_after;
    unsigned char *new_data;
    Atom new_type;
    int new_format;
    Boolean keyboard;

    Atom wm_state = XmInternAtom (display, "WM_STATE", False);

    XGetWindowProperty (display, window, wm_state, 0, 100, False,
			AnyPropertyType, &new_type, &new_format,
			&new_items, &new_bytes_after, 
			(unsigned char **)(&state_info));




    if (state_info->state != IconicState)
	    return False;



    frame_icon_prop.iconInfo.useIconBox = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.X = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.Y = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.Width = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.Height = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.iconFrameWin = UnpackCARD32 (&data);


    if (frame_icon_prop.iconInfo.useIconBox == True)
      {
	icon_box_info.h_min = UnpackCARD32 (&data);
	icon_box_info.h_max = UnpackCARD32 (&data);
	icon_box_info.h_slider_area_width = UnpackCARD32 (&data);
	icon_box_info.h_slider_x = UnpackCARD32 (&data);
	icon_box_info.h_slider_area_x = UnpackCARD32 (&data);
	icon_box_info.v_min = UnpackCARD32 (&data);
	icon_box_info.v_max = UnpackCARD32 (&data);
	icon_box_info.v_slider_area_height = UnpackCARD32 (&data);
	icon_box_info.v_slider_y = UnpackCARD32 (&data);
	icon_box_info.v_slider_area_y = UnpackCARD32 (&data);
	icon_box_info.right_arrow_x = UnpackCARD32 (&data);	
	icon_box_info.right_arrow_y = UnpackCARD32 (&data);
	icon_box_info.left_arrow_x = UnpackCARD32 (&data);	
	icon_box_info.left_arrow_y = UnpackCARD32 (&data);
	icon_box_info.top_arrow_x = UnpackCARD32 (&data);	
	icon_box_info.top_arrow_y = UnpackCARD32 (&data);
	icon_box_info.bottom_arrow_x = UnpackCARD32 (&data);	
	icon_box_info.bottom_arrow_y = UnpackCARD32 (&data);
	icon_box_info.icon_box_x = UnpackCARD32 (&data);	
	icon_box_info.icon_box_y = UnpackCARD32 (&data);
	icon_box_info.icon_box_width = UnpackCARD32 (&data);	
	icon_box_info.icon_box_height = UnpackCARD32 (&data);
	icon_box_info.icon_shell_win = UnpackCARD32 (&data);
	icon_box_info.frame_win = UnpackCARD32 (&data);
	icon_box_info.scroll_win = UnpackCARD32 (&data);
	icon_box_info.h_scroll_win = UnpackCARD32 (&data);
	icon_box_info.v_scroll_win = UnpackCARD32 (&data);
	icon_box_info.last_col = UnpackCARD32 (&data);
	icon_box_info.last_row = UnpackCARD32 (&data);
	icon_box_info.i_place_w = UnpackCARD32 (&data);
	icon_box_info.i_place_h = UnpackCARD32 (&data);

      }

    icon_x = frame_icon_prop.iconInfo.X;
    icon_y = frame_icon_prop.iconInfo.X;

    if (frame_icon_prop.iconInfo.useIconBox)
        
	FillIconBoxInfo (&frame_icon_prop, &icon_box_info,icon_x,icon_y);


	icon_rect = &frame_icon_prop.iconInfo;

	event_window = icon_rect->iconFrameWin;

	XGetWindowAttributes(display, event_window, &wattrs);

   	GetIconPosition(window, icon_rect, &window_x, &window_y, &root_x, &root_y);

	IconMenuUnpost(icon_rect->iconFrameWin, frame_icon_prop.menuWin, 
				   window_x, window_y, root_x, root_y);

	return(True);



} /* DoIconMenuUnpost */


#ifndef AUTOMATION

static Boolean
DoIconMove(Window window, XtPointer data)

{
    IconRectInfo    *icon_rect;
    Window          event_window;
    int             i;
    int        	    window_x, window_y;
    int        	    root_x, root_y;
    int		    x_incr, y_incr;
    int        	    dest_x, dest_y;
    int             x,y;
    int             icon_x,icon_y;

    struct {
            int state;
            Window icon_win;
	    } *state_info;
    unsigned long new_items, new_bytes_after;
    unsigned char *new_data;
    Atom new_type;
    int new_format;

    Atom wm_state = XmInternAtom (display, "WM_STATE", False);

    XGetWindowProperty (display, window, wm_state, 0, 100, False,
			AnyPropertyType, &new_type, &new_format,
			&new_items, &new_bytes_after, 
			(unsigned char **)(&state_info));


    if (state_info->state != IconicState)
      {
	for (i = 0; i < 10; i++)
	  do_nothing();
	XSync (display, False);
        XGetWindowProperty (display, window, wm_state, 0, 100, False,
			    AnyPropertyType, &new_type, &new_format,
			    &new_items, &new_bytes_after, 
			    (unsigned char **)(&state_info));
	if (state_info->state != IconicState)
	    return False;

      }


    x = icon_move_data.move_x;
    y = icon_move_data.move_y;


    frame_icon_prop.iconInfo.useIconBox = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.X = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.Y = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.Width = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.Height = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.iconFrameWin = UnpackCARD32 (&data);



	icon_rect = &frame_icon_prop.iconInfo;

	if (icon_rect->useIconBox == True) {
		AutoMessage(_AutoMessages[SCRMSG5]);
		return(False);
	}

    GetIconPosition(window, icon_rect, &window_x, &window_y, &root_x, &root_y);

	x_incr = icon_rect->Width / 2;
	y_incr = icon_rect->Height / 2;
	dest_x = x + x_incr;
	dest_y = y + y_incr;

        event_window = icon_rect->iconFrameWin;
	LocatePtr(event_window, root_x, root_y);


        /*
	 * The following 4 lines of code are here to overcome a
	 * peculiar delay problem in mwm under either heavy load
	 * or on very fast machines.
	 */
	XSync(display, False);
        for (i = 0; i < 50; i++)
	      do_nothing();
	XSync(display, False);

        
	DragPtr(event_window, Button1, dest_x, dest_y);

	RefreshWindow();

return(True);

}

#endif /* AUTOMATION */





void
AutoWmNormalize(
        Window window)
{

    XtPointer msg,save;
    int size = 0;
    Time time;


#ifndef AUTOMATION

    time = GetTimestamp(display);

    size += sizeof(CARD32);          /* this is window id */
    size += sizeof(CARD32);          /* this is the mask we are passing in */

    save = msg = (XtPointer) XtMalloc(size);
    msg = PackCARD32(msg,(CARD32)window);
    msg = PackCARD32(msg,(CARD32)WINDOW_MAXIMIZE_INFO); /* same info as for maximize */

    cdata.operation = WINDOW_NORMALIZE;
    cdata.window = window;

    SendMessage (&cdata, save, size, time);

#endif /* AUTOMATION */
  }






static Boolean
DoIconify(Window window, XtPointer data)
{
    AutoRectangle *rect;
    int          window_x, window_y;
    int          root_x, root_y;
    int          frame_x, frame_y;
    unsigned int window_width, window_height;
    int		 i;
    Window     event_window;


    struct {
            int state;
            Window icon_win;
	    } *state_info;
    unsigned long new_items, new_bytes_after;
    unsigned char *new_data;
    Atom new_type;
    int new_format;

    Atom wm_state = XmInternAtom (display, "WM_STATE", False);

    XGetWindowProperty (display, window, wm_state, 0, 100, False,
			AnyPropertyType, &new_type, &new_format,
			&new_items, &new_bytes_after, 
			(unsigned char **)(&state_info));


    if (state_info->state == IconicState)
      {
	for (i = 0; i < 10; i++)
	  do_nothing();
	XSync (display, False);
        XGetWindowProperty (display, window, wm_state, 0, 100, False,
			    AnyPropertyType, &new_type, &new_format,
			    &new_items, &new_bytes_after, 
			    (unsigned char **)(&state_info));
	if (state_info->state == IconicState)
	    return False;

      }
	



    /* This assumes that the data has been packed in the following order:
       1. id 2. count of gadgets 3. x position 4. y position 5. width
       6. height 7. event window */


    frame_icon_prop.titleGadgetCount = UnpackCARD32 (&data); /* cound of mwm gadgets */

    frame_icon_prop.titleGadgets[MWM_FRAME_MINIMIZE].id = UnpackCARD32 (&data);                 /* id, e.g FRAME_MINIMIZE */
    
    frame_icon_prop.titleGadgets[MWM_FRAME_MINIMIZE].rect.x = UnpackCARD32 (&data);             /* x location */

    frame_icon_prop.titleGadgets[MWM_FRAME_MINIMIZE].rect.y = UnpackCARD32 (&data);             /* y location */
    frame_icon_prop.titleGadgets[MWM_FRAME_MINIMIZE].rect.width = UnpackCARD32 (&data);         /* width of button */
    frame_icon_prop.titleGadgets[MWM_FRAME_MINIMIZE].rect.height = UnpackCARD32 (&data);        /* height of button */
    frame_icon_prop.frameWin = UnpackCARD32 (&data);      /* frame window */


    rect = &frame_icon_prop.titleGadgets[MWM_FRAME_MINIMIZE].rect;

    if (frame_icon_prop.titleGadgetCount <= 0 ||
	frame_icon_prop.titleGadgets[MWM_FRAME_MINIMIZE].id == INVALID)
       return False;

    event_window = frame_icon_prop.frameWin;



    if (GetWindowGeometry(event_window, &frame_x, &frame_y, 
                          &window_width, &window_height) == False)
      return False;


     window_x = rect->x + (rect->width / 2);
     window_y = rect->y + (rect->height / 2);
     root_x = frame_x + window_x;
     root_y = frame_y + window_y;
     LocatePtr(event_window, root_x, root_y);
     ClickMouseButton(event_window, Button1, 1, window_x, window_y,
                     root_x, root_y);


     

    XSync(display, False);

    /*
     * Use XGetWindowProperty to determine if the state is iconic
     */

    XGetWindowProperty (display, window, wm_state, 0, 100, False,
			AnyPropertyType, &new_type, &new_format,
			&new_items, &new_bytes_after, 
		        (unsigned char **)(&state_info));    
    while (state_info->state != IconicState)
      {
	do_nothing();
	XSync (display, False);
        XGetWindowProperty (display, window, wm_state, 0, 100, False,
			AnyPropertyType, &new_type, &new_format,
			&new_items, &new_bytes_after, 
		        (unsigned char **)(&state_info));    
      }

    return True;

  }


#ifndef AUTOMATION

static Boolean
DoIconMenuItemSelect(Window window, XtPointer data)
{

    char    			current_item[MAX_NAME_LEN + 1];
    char    			warn_buf[MAX_NAME_LEN + 1];
    int     			current_num;
    int     			root_x, root_y;
    int     			window_x, window_y;
    Boolean 			retval;
    Time    			time;
    int     			item_x, item_y;
    int     			i;
    XWindowAttributes	        wattrs;
    IconRectInfo    	        *icon_rect;
    String temp_string;

    int last_col,last_row,i_place_h,i_place_w;
    int icon_x,icon_y;
    IconBoxInfo icon_box_info;


    int item_num;
    Boolean keyboard;
    char item_name[MAX_NAME_LEN + 1];


    item_num = icon_item_select_data.item_num;
    keyboard = icon_item_select_data.keyboard;
   
    if (item_num == -1)
          strcpy (item_name,icon_item_select_data.item_name);
    
    






    retval = True;

    /*
	 * The following 4 lines of code are here to overcome a
	 * peculiar delay problem in mwm about menu Window.
	 */
	XSync(display, False);
	for (i = 0; i < 100; i++)
		do_nothing();
	XSync(display, False);


    /*
     * Unpack data from UTM 
     */


    frame_icon_prop.iconInfo.clientState = UnpackCARD32 (&data);
    frame_icon_prop.menuItemCount = UnpackCARD32 (&data);
    frame_icon_prop.sensitiveItemCount = UnpackCARD32 (&data);
    frame_icon_prop.menuWin = UnpackCARD32 (&data);
    frame_icon_prop.frameWin = UnpackCARD32 (&data);
    

    for (i=0; i < frame_icon_prop.menuItemCount && i < MAX_MENU_ITEMS; i++)
      {
	frame_icon_prop.windowMenu[i].sensitive = UnpackCARD32 (&data);
	frame_icon_prop.windowMenu[i].item_y = UnpackCARD32 (&data);
	temp_string = (char *)UnpackString(&data);
	strcpy (frame_icon_prop.windowMenu[i].item_name, temp_string);
      }

    frame_icon_prop.iconInfo.useIconBox = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.X = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.Y = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.Width = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.Height = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.iconFrameWin = UnpackCARD32 (&data);

    if (frame_icon_prop.iconInfo.useIconBox == True)
      {
	icon_box_info.h_min = UnpackCARD32 (&data);
	icon_box_info.h_max = UnpackCARD32 (&data);
	icon_box_info.h_slider_area_width = UnpackCARD32 (&data);
	icon_box_info.h_slider_x = UnpackCARD32 (&data);
	icon_box_info.h_slider_area_x = UnpackCARD32 (&data);
	icon_box_info.v_min = UnpackCARD32 (&data);
	icon_box_info.v_max = UnpackCARD32 (&data);
	icon_box_info.v_slider_area_height = UnpackCARD32 (&data);
	icon_box_info.v_slider_y = UnpackCARD32 (&data);
	icon_box_info.v_slider_area_y = UnpackCARD32 (&data);
	icon_box_info.right_arrow_x = UnpackCARD32 (&data);	
	icon_box_info.right_arrow_y = UnpackCARD32 (&data);
	icon_box_info.left_arrow_x = UnpackCARD32 (&data);	
	icon_box_info.left_arrow_y = UnpackCARD32 (&data);
	icon_box_info.top_arrow_x = UnpackCARD32 (&data);	
	icon_box_info.top_arrow_y = UnpackCARD32 (&data);
	icon_box_info.bottom_arrow_x = UnpackCARD32 (&data);	
	icon_box_info.bottom_arrow_y = UnpackCARD32 (&data);
	icon_box_info.icon_box_x = UnpackCARD32 (&data);	
	icon_box_info.icon_box_y = UnpackCARD32 (&data);
	icon_box_info.icon_box_width = UnpackCARD32 (&data);	
	icon_box_info.icon_box_height = UnpackCARD32 (&data);
	icon_box_info.icon_shell_win = UnpackCARD32 (&data);
	icon_box_info.frame_win = UnpackCARD32 (&data);
	icon_box_info.scroll_win = UnpackCARD32 (&data);
	icon_box_info.h_scroll_win = UnpackCARD32 (&data);
	icon_box_info.v_scroll_win = UnpackCARD32 (&data);
	icon_box_info.last_col = UnpackCARD32 (&data);
	icon_box_info.last_row = UnpackCARD32 (&data);
	icon_box_info.i_place_h = UnpackCARD32 (&data);
	icon_box_info.i_place_w = UnpackCARD32 (&data);

      }
    if (frame_icon_prop.iconInfo.useIconBox)
        
	FillIconBoxInfo (&frame_icon_prop, &icon_box_info,icon_x,icon_y);


    if (frame_icon_prop.iconInfo.clientState != MWM_ICONIC_STATE)
        return(False);

	if (XGetWindowAttributes(display, frame_icon_prop.menuWin, &wattrs) == 0)
		return(False);
	if (wattrs.map_state == IsUnmapped || wattrs.map_state == IsUnviewable)
		return(False);

    if (item_num > frame_icon_prop.menuItemCount ||
        item_num > MAX_MENU_ITEMS) {

        if (item_num > frame_icon_prop.menuItemCount)
            sprintf(warn_buf,
            "Item no. specified (%d) is > than total no. of menu items (%d)\n",
            item_num, frame_icon_prop.menuItemCount);
        else
            sprintf(warn_buf,
            _AutoMessages[SCRMSG14],
            item_num, MAX_MENU_ITEMS);
        AutoMessage(warn_buf);
        retval = False;

    }

    if (item_num != -1)
        current_num = item_num;
    else {
        if (item_name == NULL)
            retval = False;
        if (GetMenuItemNum(window, item_name, &current_num) == False) {

            sprintf(warn_buf, _AutoMessages[SCRMSG15],
                    item_name);
            AutoMessage(warn_buf);
            retval = False;

		}

    }

	if (retval == False) {

		icon_rect = &frame_icon_prop.iconInfo;
		GetIconPosition(window, icon_rect, &window_x, &window_y, 
						&root_x, &root_y);
		IconMenuUnpost(icon_rect->iconFrameWin, frame_icon_prop.menuWin,
					   window_x, window_y, root_x, root_y);

		return(retval);

	}

    if (IsMenuItemSensitive(window, current_num, current_item) == False) {

    	sprintf(warn_buf, _AutoMessages[SCRMSG16],
                current_num, current_item);
        AutoMessage(warn_buf);

    }

	retval = True;
	GetMenuItemLocation(frame_icon_prop.menuWin, current_num,
						frame_icon_prop.menuItemCount, &item_x, &item_y,
						&frame_icon_prop.windowMenu[current_num - 1]);

	LocatePtr(frame_icon_prop.frameWin, item_x, item_y);
	GetPointerLocation(frame_icon_prop.menuWin, &window_x, &window_y,
					   &root_x, &root_y);
	ClickMouseButton(frame_icon_prop.menuWin, Button1, 1, window_x, window_y,
					 root_x, root_y);
	GetCurrentTime(frame_icon_prop.frameWin, &time);

	XSync(display, False);

	for (i = 0; i < 25; i++)
		do_nothing();

	RefreshWindow();

	return(True);
  }

#endif /* AUTOMATION */



#ifndef AUTOMATION

static Boolean
DoMenuItemSelect(Window window, XtPointer data)

{

  


    char    current_item[MAX_NAME_LEN + 1];
    char    warn_buf[2 * MAX_NAME_LEN + 1];
    int     current_num;
    int     root_x, root_y;
    int     window_x, window_y;
    Boolean retval;
    Time    time;
    int     item_x;
    int     i;
    int     item_y;
    String temp_string;

    int item_num;
    Boolean keyboard;
    char item_name[MAX_NAME_LEN + 1];
    IconBoxInfo icon_box_info;

    item_num = item_select_data.item_num;
    keyboard = item_select_data.keyboard;
   
    if (item_num == -1)
          strcpy (item_name,item_select_data.item_name);
    
    

        retval = True;

	/*
	 * The following 4 lines of code are here to overcome a
	 * peculiar delay problem in mwm about menu Window.
	 */
	XSync(display, False);
	for (i = 0; i < 100; i++)
		do_nothing();
	XSync(display, False);


    /*
     * Unpack data from UTM 
     */

    frame_icon_prop.menuItemCount = UnpackCARD32 (&data);
    frame_icon_prop.sensitiveItemCount = UnpackCARD32 (&data);
    frame_icon_prop.menuWin = UnpackCARD32 (&data);
    frame_icon_prop.frameWin = UnpackCARD32 (&data);
    

    for (i=0; i < frame_icon_prop.menuItemCount && i < MAX_MENU_ITEMS; i++)
      {
	frame_icon_prop.windowMenu[i].sensitive = UnpackCARD32 (&data);
	frame_icon_prop.windowMenu[i].item_y = UnpackCARD32 (&data);
	temp_string = (char *)UnpackString(&data);
	strcpy (frame_icon_prop.windowMenu[i].item_name, temp_string);
      }


    frame_icon_prop.iconInfo.useIconBox = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.X = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.Y = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.Width = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.Height = UnpackCARD32 (&data);
    frame_icon_prop.iconInfo.iconFrameWin = UnpackCARD32 (&data);

    if (frame_icon_prop.iconInfo.useIconBox == True)
      {
	icon_box_info.h_min = UnpackCARD32 (&data);
	icon_box_info.h_max = UnpackCARD32 (&data);
	icon_box_info.h_slider_area_width = UnpackCARD32 (&data);
	icon_box_info.h_slider_x = UnpackCARD32 (&data);
	icon_box_info.h_slider_area_x = UnpackCARD32 (&data);
	icon_box_info.v_min = UnpackCARD32 (&data);
	icon_box_info.v_max = UnpackCARD32 (&data);
	icon_box_info.v_slider_area_height = UnpackCARD32 (&data);
	icon_box_info.v_slider_y = UnpackCARD32 (&data);
	icon_box_info.v_slider_area_y = UnpackCARD32 (&data);
	icon_box_info.right_arrow_x = UnpackCARD32 (&data);	
	icon_box_info.right_arrow_y = UnpackCARD32 (&data);
	icon_box_info.left_arrow_x = UnpackCARD32 (&data);	
	icon_box_info.left_arrow_y = UnpackCARD32 (&data);
	icon_box_info.top_arrow_x = UnpackCARD32 (&data);	
	icon_box_info.top_arrow_y = UnpackCARD32 (&data);
	icon_box_info.bottom_arrow_x = UnpackCARD32 (&data);	
	icon_box_info.bottom_arrow_y = UnpackCARD32 (&data);
	icon_box_info.icon_box_x = UnpackCARD32 (&data);	
	icon_box_info.icon_box_y = UnpackCARD32 (&data);
	icon_box_info.icon_box_width = UnpackCARD32 (&data);	
	icon_box_info.icon_box_height = UnpackCARD32 (&data);
	icon_box_info.icon_shell_win = UnpackCARD32 (&data);
	icon_box_info.frame_win = UnpackCARD32 (&data);
	icon_box_info.scroll_win = UnpackCARD32 (&data);
	icon_box_info.h_scroll_win = UnpackCARD32 (&data);
	icon_box_info.v_scroll_win = UnpackCARD32 (&data);
	icon_box_info.last_col = UnpackCARD32 (&data);
	icon_box_info.last_row = UnpackCARD32 (&data);
	icon_box_info.i_place_w = UnpackCARD32 (&data);
	icon_box_info.i_place_h = UnpackCARD32 (&data);

      }


    

    if (item_num > frame_icon_prop.menuItemCount || 
        item_num > MAX_MENU_ITEMS) {

		if (item_num > frame_icon_prop.menuItemCount)
			sprintf(warn_buf, 
	    "Item no. specified (%d) is > than total no. of menu items (%d)\n",
	     item_num, frame_icon_prop.menuItemCount);
		else
	      sprintf(warn_buf, 
			_AutoMessages[SCRMSG14],
			item_num, MAX_MENU_ITEMS);
		AutoMessage(warn_buf);
		retval = False;

	}

    if (item_num != -1)
        current_num = item_num;
    else {

        if (item_name == NULL)
			retval = False;
        if (GetMenuItemNum(window, item_name, &current_num) == False) {

			sprintf(warn_buf, 
				_AutoMessages[SCRMSG15],
					item_name);
			AutoMessage(warn_buf);
			retval = False;

		}

    }

	XSync(display, False);

    if (IsMenuItemSensitive(window, current_num, current_item) == False) {

    	sprintf(warn_buf, _AutoMessages[SCRMSG16],
                current_num, current_item);
        AutoMessage(warn_buf);

    }
/**DEBUG** Check for "Size" and "Move" and disallow it till the key event
 problem is not solved **/
	if ((strcmp(current_item, "Size") == 0) ||
		(strcmp(current_item, "Move") == 0)) {

		
		AutoMessage(_AutoMessages[SCRMSG17]);
		retval = False;

	}

	if (retval == False) {
	  WindowMenuUnpost(frame_icon_prop.frameWin, frame_icon_prop.menuWin);
		return(retval);
	}

    GetMenuItemLocation(frame_icon_prop.menuWin, current_num, 
                        frame_icon_prop.menuItemCount, &item_x, &item_y,
			&frame_icon_prop.windowMenu[current_num - 1]);

    LocatePtr(frame_icon_prop.frameWin, item_x, item_y);
    GetPointerLocation(frame_icon_prop.menuWin, &window_x, &window_y, 
                       &root_x, &root_y);
    ClickMouseButton(frame_icon_prop.menuWin, Button1, 1, window_x, window_y,
                     root_x, root_y);
    GetCurrentTime(frame_icon_prop.frameWin, &time);

	XSync(display, False);

	for (i = 0; i < 25; i++)
		do_nothing();

	RefreshWindow();

    return(retval);

  }

#endif /* AUTOMATION */



static Boolean
DoRaise(Window window, XtPointer data)
{

  CARD32 state;

  state = UnpackCARD32 (&data);

    if (state == MWM_ICONIC_STATE)
      return False;

    XRaiseWindow(display, window);
    XSync(display, False);

}



static Boolean
DoMaximize(Window window, XtPointer data)
{

    AutoRectangle *rect;
    Window     event_window;
    int        window_x, window_y;
    int        root_x, root_y;
    int        frame_x, frame_y;
    unsigned int window_width, window_height;
    Time       time;
    int		   i;


    struct {
            int state;
            Window icon_win;
	    } *state_info;
    unsigned long new_items, new_bytes_after;
    unsigned char *new_data;
    Atom new_type;
    int new_format;

    XWindowAttributes w_attributes;
    int old_height,old_width,new_height,new_width;

    Atom wm_state = XmInternAtom (display, "WM_STATE", False);

    XGetWindowProperty (display, window, wm_state, 0, 100, False,
			AnyPropertyType, &new_type, &new_format,
			&new_items, &new_bytes_after, 
			(unsigned char **)(&state_info));


    if (state_info->state == IconicState) 
      {

   	for (i = 0; i < 10; i++)
        	do_nothing();
        XSync(display, False);
        XGetWindowProperty (display, window, wm_state, 0, 100, False,
			AnyPropertyType, &new_type, &new_format,
			&new_items, &new_bytes_after, 
			(unsigned char **)(&state_info));
        if (state_info->state == IconicState) 
	    return False;

      }


    XGetWindowAttributes (display,window,&w_attributes);
      old_width = w_attributes.width;
      old_height = w_attributes.height;

    /*
     * Assumed order of data is id, gadget count, x, y, width, height, event 
     * window
     */
    
    frame_icon_prop.titleGadgetCount = UnpackCARD32 (&data);          

    frame_icon_prop.titleGadgets[MWM_FRAME_MAXIMIZE].id = 
      UnpackCARD32 (&data);            
    frame_icon_prop.titleGadgets[MWM_FRAME_MAXIMIZE].rect.x = 
                           UnpackCARD32 (&data);         /* x location */
    frame_icon_prop.titleGadgets[MWM_FRAME_MAXIMIZE].rect.y 
                          = UnpackCARD32 (&data);         /* x location */
    frame_icon_prop.titleGadgets[MWM_FRAME_MAXIMIZE].rect.width 
                          = UnpackCARD32 (&data);         /* x location */
    frame_icon_prop.titleGadgets[MWM_FRAME_MAXIMIZE].rect.height
                          = UnpackCARD32 (&data);         /* x location */
    frame_icon_prop.frameWin = UnpackCARD32 (&data);          


    rect = &frame_icon_prop.titleGadgets[MWM_FRAME_MAXIMIZE].rect;

    event_window =  frame_icon_prop.frameWin;


	
     if (GetWindowGeometry(event_window, &frame_x, &frame_y, 
                          &window_width, &window_height) == False)
       return False;


     window_x = rect->x + (rect->width / 2);
     window_y = rect->y + (rect->height / 2);
     root_x = frame_x + window_x;
     root_y = frame_y + window_y;
     LocatePtr(event_window, root_x, root_y);
     ClickMouseButton(event_window, Button1, 1, window_x, 
			     window_y,root_x, root_y);

     GetCurrentTime(event_window, &time);

     /*
      * Get attributes to know that window has been maximized
      */


    XGetWindowAttributes (display,window,&w_attributes);
      new_width = w_attributes.width;
      new_height = w_attributes.height;

    while ((new_height == old_height) && (new_width == old_width))
      {
	do_nothing();
	XSync (display,False);
        XGetWindowAttributes (display,window,&w_attributes);
        new_width = w_attributes.width;
        new_height = w_attributes.height;
      }
    
    RefreshWindow();
	
    return True;

  }




#ifndef AUTOMATION

static Boolean
DoMove(

        Window window,
        XtPointer data)
        
{

    AutoRectangle *title_rect, *menu_rect;
    Window  event_window;
    int        window_x, window_y;
    int        root_x, root_y;
    int        dest_x, dest_y;
    int        x_incr, y_incr;
    int        frame_x, frame_y;
    unsigned int window_width, window_height;
    int        i;
    int        x,y;



    struct {
            int state;
            Window icon_win;
	    } *state_info;
    unsigned long new_items, new_bytes_after;
    unsigned char *new_data;
    Atom new_type;
    int new_format;

    Atom wm_state = XmInternAtom (display, "WM_STATE", False);


    XGetWindowProperty (display, window, wm_state, 0, 100, False,
			AnyPropertyType, &new_type, &new_format,
			&new_items, &new_bytes_after, 
			(unsigned char **)(&state_info));


    if (state_info->state == IconicState) 
      {

   	for (i = 0; i < 10; i++)
        	do_nothing();
        XSync(display, False);
        XGetWindowProperty (display, window, wm_state, 0, 100, False,
			AnyPropertyType, &new_type, &new_format,
			&new_items, &new_bytes_after, 
			(unsigned char **)(&state_info));
        if (state_info->state == IconicState) 
	    return False;

      }



    






    /* The "x_and_y" argument is a structure holding x and y. */

    x = window_move_data.move_x;
    y = window_move_data.move_y;


    frame_icon_prop.titleGadgetCount = UnpackCARD32 (&data);

    frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].id  
                       = UnpackCARD32 (&data);        
    frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect.x =
      UnpackCARD32 (&data);           
    frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect.y =
      UnpackCARD32 (&data);           
    frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect.width =
      UnpackCARD32 (&data);           
    frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect.height =
      UnpackCARD32 (&data);           
    frame_icon_prop.titleGadgets[MWM_FRAME_SYSTEM].id     
                       = UnpackCARD32 (&data);             

    frame_icon_prop.titleGadgets[MWM_FRAME_SYSTEM].rect.x =
      UnpackCARD32 (&data);         
    frame_icon_prop.titleGadgets[MWM_FRAME_SYSTEM].rect.y =
      UnpackCARD32 (&data);           
    frame_icon_prop.titleGadgets[MWM_FRAME_SYSTEM].rect.width =
      UnpackCARD32 (&data);           
    frame_icon_prop.titleGadgets[MWM_FRAME_SYSTEM].rect.height =
      UnpackCARD32 (&data);           
    frame_icon_prop.upperBorderWidth = UnpackCARD32 (&data);
    frame_icon_prop.lowerBorderWidth = UnpackCARD32 (&data);
    frame_icon_prop.windowX = UnpackCARD32 (&data);         
    frame_icon_prop.windowY = UnpackCARD32 (&data);         
    frame_icon_prop.frameWin = UnpackCARD32 (&data);           

    title_rect = &frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].rect;
    menu_rect = &frame_icon_prop.titleGadgets[MWM_FRAME_SYSTEM].rect;




    if (frame_icon_prop.titleGadgetCount <= 0 ||
	frame_icon_prop.titleGadgets[MWM_FRAME_TITLE].id == INVALID)
      return False;

    event_window = frame_icon_prop.frameWin;
	
    if (GetWindowGeometry(event_window, &frame_x, &frame_y, 
                          &window_width, &window_height) == False)
      return False;
	



	    
     window_x = title_rect->x + (title_rect->width / 2);
     window_y = title_rect->y + (title_rect->height / 2);
     root_x = frame_x + window_x;
     root_y = frame_y + window_y;

     if (frame_icon_prop.titleGadgets[MWM_FRAME_SYSTEM].id == INVALID)   
       menu_rect = NULL;
    else menu_rect = &frame_icon_prop.titleGadgets[MWM_FRAME_SYSTEM].rect;
	   

     window_x = frame_icon_prop.windowX + title_rect->x + 
	       (title_rect->width / 2);
     window_y = frame_icon_prop.windowX + title_rect->y + 
               (title_rect->height / 2);

    /*
     * x, y provided are the window corners. From that, and from the
     * width, height of menu rectangle, and title rectangle, compute
     * the actual destination x, y to which the pointer should be moved.
     */
    if (menu_rect == NULL) 
       x_incr = 0;
    else
       x_incr = menu_rect->width;
       y_incr = frame_icon_prop.upperBorderWidth;
       x_incr += (title_rect->width / 2);
       x_incr += (frame_icon_prop.lowerBorderWidth - 1);
       y_incr += (title_rect->height / 2);
       dest_x = x + x_incr;
       dest_y = y + y_incr;

       LocatePtr(event_window, root_x, root_y);
  
        /*
	 * The following 4 lines of code are here to overcome a
	 * peculiar delay problem in mwm under either heavy load
	 * or on very fast machines.
	 */
	XSync(display, False);
        for (i = 0; i < 50; i++)
	      do_nothing();
	XSync(display, False);

        
	DragPtr(event_window, Button1, dest_x, dest_y);

	RefreshWindow();

    return True;

  }

static void
SendUTMMessage (
     Widget w,
     Atom selection,
     Atom target,
     XtPointer param,
     unsigned long paramLen,
     int paramFmt,
     XtCallbackProc doneProc,
     XtPointer closure,
     Time time)
{

  UTMPackageRec *pUtmData;
  int scr = XScreenNumberOfScreen (XtScreen(w));

  /* allocated and build utm package to send */

  pUtmData = (UTMPackageRec *) XtMalloc(sizeof(UTMPackageRec));
  pUtmData->fmt = paramFmt;
  pUtmData->len = paramLen;
  pUtmData->target = target;
  pUtmData->param = param;
  pUtmData->doneProc = doneProc;
  pUtmData->closure = closure;

  /* queue up the data which is pulled off in the UTMDestinationProc */

   EnqueueUtmData (pUtmData);  


  /* This causes the DestinationCB to be invoked. This is where the
     param data is transferred. */


  if (!XmeNamedSink (w,         
		     selection,
		     XmCOPY,
		     (XtPointer)NULL,
		     time))
    fprintf (stderr, "UTM Error: UTMSendMessage failed.\n");
  
}



void 
UTMDestinationProc (Widget w, XtPointer client_data, XtPointer call_data)
{

  XmDestinationCallbackStruct *dcs = (XmDestinationCallbackStruct *)call_data;


   /* pull off the real client data */
  UTMPackageRec *pUtmData = (UTMPackageRec *)DequeueUtmData(); 


  

  /* if no UTMData, then transfer can't be done.*/
  
  if (pUtmData == NULL)
    return;

  /* set up parameters to pass */

  XmTransferSetParameters (dcs->transfer_id,
			   pUtmData->param, /* prt to parameter data */
			   pUtmData->fmt, /* param format */
			   pUtmData->len,   /* size of param */
			   dcs->selection); /* not used */
 /* make the transfer. This invokes the selection owner's ConvertCB.
    When done, UTMReplyReceived callback proc is called. */
   
  XmTransferValue (dcs->transfer_id,
		   pUtmData->target, /* target to convert against */
		   pUtmData->doneProc, /* proc to call when done */
		   pUtmData->closure, /* client data to done proc */
		   dcs->time);

  XtFree ((char *)pUtmData);

} /* UTMDestinationProc */



/*
 * Puts data on the queue between sending messages and having the
 * destination callbacks invoked.
 */

static void
EnqueueUtmData(XtPointer data)
{
  DataQueueRec *dataRec = (DataQueueRec *)XtMalloc(sizeof(DataQueueRec));

  dataRec->data = data;
  dataRec->next = dataQueue;
  dataRec->prev = NULL;

  if (dataQueue == NULL)
    dataQueueTail = dataRec;
  else
    dataQueue->prev = dataRec;
  dataQueue = dataRec;

  return;
}




static XtPointer
DequeueUtmData()
{
  DataQueueRec *ptr = dataQueueTail;
  XtPointer dataPtr = NULL;

  if (ptr)
    {
      dataQueueTail = ptr->prev;

      if (ptr->prev != NULL)
        ptr->prev->next = NULL;
      else
        dataQueue = NULL;
 
      dataPtr = ptr->data;
      XtFree((char*)ptr);
    }
  else
     fprintf(stderr, "UTM ERROR: Data not found in queue.\n");

  return (dataPtr);
}



static void 
SendMessage(ClientData *client_data, XtPointer msg, int len, Time time)
{


   SendUTMMessage  (UTMDrawingArea, WM_S0,
                         _MOTIF_WM_AUTOMATION,
			 (XtPointer)msg, (unsigned long)len, 8,
                         SelectionValueReceived, (XtPointer)client_data, time);
 }


/*
 * These functions are called when the data becomes available
 */ 


/*
 * This is the function that is called for all WM operations. 
 */


static void 
SelectionValueReceived (Widget w, XtPointer clientData, XtPointer callData)
{

  /* PUT IN SOME CHECKS */


  XmSelectionCallbackStruct *Selection;
  XtPointer data,c_data;
  int op;
  Window win;


  Selection = (XmSelectionCallbackStruct *)callData;




  

  /* Get the value of the selection by accessing the appropriate field in
     the callData structure */


  data = Selection->value;

  if (data == NULL)
    {
      printf ("AUTOWARNING** : selection value is NULL!\n");
      return;
    }

  win = ((ClientData *)clientData)->window;
  op =  ((ClientData *)clientData)->operation;
  


  switch (op)
    {

    case WINDOW_MINIMIZE:
      if (! (DoIconify(win,data)))
	 AutoMessage(_AutoMessages[SCRMSG24]);
      break;

    case WINDOW_MAXIMIZE:
      if (! (DoMaximize(win,data)))
	AutoMessage(_AutoMessages[SCRMSG22]);
      break;

    case WINDOW_NORMALIZE:
      if (! (DoMaximize(win,data)))
	AutoMessage(_AutoMessages[SCRMSG23]);
        return;
      break;

    case WINDOW_MOVE:
      if (! (DoMove(win,data)))
	 AutoMessage(_AutoMessages[SCRMSG35]);

      break;

    case WINDOW_RAISE:
      if (!(DoRaise(win,data)))
	 AutoMessage(_AutoMessages[SCRMSG41]);
      break;


    case WINDOW_RESIZE:
      if (!(DoResize(win,data)))
	AutoMessage(_AutoMessages[SCRMSG34]);
      break;


    case WINDOW_MENU_ITEM_SELECT:
      if (!(DoMenuItemSelect(win,data)))
	 AutoMessage(_AutoMessages[SCRMSG32]);
      break;

    case WINDOW_DEICONIFY:
      if (!(DoDeiconify(win,data)))
	 AutoMessage(_AutoMessages[SCRMSG25]);
      break;

    case WINDOW_MENU_POST:
      if (!(DoMenuPost(win,data)))
	 AutoMessage(_AutoMessages[SCRMSG28]);
      break;

    case WINDOW_MENU_UNPOST:
      if (!(DoMenuUnpost(win,data)))
	 AutoMessage(_AutoMessages[SCRMSG29]);
      break;

    case WINDOW_FOCUS:
      if (!(DoFocus(win,data)))
	 AutoMessage(_AutoMessages[SCRMSG26]);
      break;

    case WINDOW_ITEM_CHECK:
      if (!(DoWindowItemCheck(win,data)))
	 AutoMessage(_AutoMessages[SCRMSG36]);
      break;

    case ICON_MOVE:
      if (!(DoIconMove(win,data)))
	 AutoMessage(_AutoMessages[SCRMSG37]);
      break;

    case ICON_MENU_POST:
      if (!(DoIconMenuPost(win,data)))
	 AutoMessage(_AutoMessages[SCRMSG30]);
      break;

    case ICON_MENU_UNPOST:
      if (!(DoIconMenuUnpost(win,data)))
	 AutoMessage(_AutoMessages[SCRMSG31]);
      break;

    case ICON_MENU_ITEM_SELECT:
      if (!(DoIconMenuItemSelect(win,data)))
	 AutoMessage(_AutoMessages[SCRMSG33]);
      break;

    default:
      break;




    }
  
}


Time
GetTimestamp (Display *dpy)
{
  XEvent   event;
  XWindowAttributes attr;
  Window   rwin = RootWindowOfScreen(XtScreen(UTMDrawingArea));
  Atom     time_atom = XInternAtom(display, "_MOTIF_CURRENT_TIME", False);

  XGetWindowAttributes(display, rwin, &attr);
  if (! (attr.your_event_mask & PropertyChangeMask))
    XSelectInput(display, rwin, attr.your_event_mask | PropertyChangeMask);

  XChangeProperty(display, rwin, time_atom, time_atom, 8, PropModeAppend, NULL, 0);
  XWindowEvent(display, rwin, PropertyChangeMask, &event);

  if (! (attr.your_event_mask & PropertyChangeMask))
   XSelectInput(display, rwin, attr.your_event_mask);

  return(event.xproperty.time);
}





static XtPointer
PackCARD32(XtPointer data, CARD32 val)
{
    CARD16 bottom = val & (0xFFFF);
    CARD16 top = val >> 16;

    data = PackCARD16(data, top);
    data = PackCARD16(data, bottom);
    return(data);
}





/*	Function Name: PackCARD16
 *	Description: Packs an 16 bit value into the data stream.
 *	Arguments: data - The data stream to pack into.
 *                 val - The 16 bit value to pack.
 *	Returns: data - A pointer into the next empty location in the
 *                      data stream.
 */



    
static XtPointer
PackCARD16(XtPointer data, CARD16 val)
{
    CARD8 bottom = val & (0xFF);
    CARD8 top = val >> 8;

    data = PackCARD8(data, top);
    data = PackCARD8(data, bottom);
    return(data);
}

/*	Function Name: PackCARD8
 *	Description: Packs an 8 bit value into the data stream.
 *	Arguments: data - The data stream to pack into.
 *                 val - The 8 bit value to pack.
 *	Returns: data - A pointer into the next empty location in the
 *                      data stream.
 */
    
static XtPointer
PackCARD8(XtPointer data, CARD8 val)
{
    CARD8 *ptr = (CARD8 *) data;

    *ptr = (CARD8) val;
    data = ((char*)data) + 1;

    return(data);
}





static XtPointer
PackString(XtPointer data, String str)
{
    register int i, len = strlen(str);

    data = PackListNum(data, len);
    for (i = 0; i < len; i++, str++) 
        data = PackCARD8(data, *str);

    return(data);
}

static String
UnpackString(XtPointer *data_ptr)
{
    register int i;
    int len = UnpackListNum(data_ptr);
    char *str, *top = XtMalloc((len + 1) * sizeof(char));
    
    for (str = top, i = 0; i < len; i++, str++) {
        *str = (char) UnpackCARD8(data_ptr);
    }
    *str = '\0';

    return((String) top);
}


static CARD32
UnpackCARD32(XtPointer *data_ptr)
{
    CARD32 val = UnpackCARD16(data_ptr) << 16;
    val |= UnpackCARD16(data_ptr);
    return(val);
}


static CARD16
UnpackCARD16(XtPointer *data_ptr)
{
    CARD16 val = UnpackCARD8(data_ptr) << 8;
    val |= UnpackCARD8(data_ptr);
    return(val);
}


static CARD8
UnpackCARD8(XtPointer *data_ptr)
{
    CARD8 ret_val = *((char *) *data_ptr);

    *data_ptr = ((char *) *data_ptr) + 1;

    return(ret_val);
}





static void FillIconBoxInfo(PropMotifWmFrameIconInfo *frame_icon_prop,
			    IconBoxInfo *ib, CARD32 icon_x, CARD32 icon_y)
{

    int                 hslider_area_width, vslider_area_height;
    int                 num_visible_cols, num_visible_rows;
    int                 visible_first_col, visible_last_col, visible_first_row,
                        visible_last_row, current_icon_col, current_icon_row;
    int                 pointerX, pointerY;
    int                 vert_slider_inc, horiz_slider_inc;
    int                 vert_inc_needed, horiz_inc_needed;
    int                 iPlaceW, iPlaceH, maxval, minval;
    int                 iconX, iconY;
        int                 sliderX, sliderY, slider_area_x, slider_area_y;
    int                 lastRow, lastCol;

    
    
    lastCol = ib->last_col;
    lastRow = ib->last_row;
    iPlaceW = ib->i_place_w;
    iPlaceH = ib->i_place_h;
    iconX = icon_x;
    iconY = icon_y;

    /* Compute pointerX and horiz_inc_needed */
    minval = ib->h_min;
    maxval = ib->h_max;
    hslider_area_width = ib->h_slider_area_width;
    horiz_slider_inc = (iPlaceW * hslider_area_width) / (maxval - minval);
    sliderX = ib->h_slider_x;
    slider_area_x = ib->h_slider_area_x;
    visible_first_col = round_quotient((sliderX - slider_area_x),
                                       horiz_slider_inc);
    num_visible_cols = round_quotient((hslider_area_width * (lastCol + 1)),
                                      (maxval - minval));
    visible_last_col = visible_first_col + num_visible_cols - 1;
    current_icon_col = iconX / iPlaceW;
  if ((visible_first_col == 0 && visible_last_col == lastCol) ||
        (current_icon_col >= visible_first_col && 
         current_icon_col <= visible_last_col)) {
        
        horiz_inc_needed = 0;
        if (num_visible_cols > 1)
            pointerX = slider_area_x + (current_icon_col * iPlaceW) + 
                       (iPlaceW / 2);
        else
            pointerX = slider_area_x + (iPlaceW / 2);

    }
    else if (current_icon_col < visible_first_col) {

        horiz_inc_needed = current_icon_col - visible_first_col;
        pointerX = slider_area_x + (iPlaceW / 2);

    }
    else if (current_icon_col > visible_last_col) {

        horiz_inc_needed = current_icon_col - visible_last_col;
        pointerX = slider_area_x + 
    ((visible_last_col - visible_first_col) * iPlaceW) +
                   (iPlaceW / 2);

    }

    /* Now compute pointerY and vert_inc_needed */
    minval = ib->v_min;
    maxval = ib->v_max;
    vslider_area_height = ib->v_slider_area_height;
    vert_slider_inc = (iPlaceH * vslider_area_height) / (maxval - minval);
    sliderY = ib->v_slider_y;
    slider_area_y = ib->v_slider_area_y;
    visible_first_row = round_quotient((sliderY - slider_area_y), 
                                       vert_slider_inc);
    num_visible_rows = round_quotient((vslider_area_height * (lastRow + 1)),
                                      (maxval - minval));
    visible_last_row = visible_first_row + num_visible_rows - 1;
    current_icon_row = iconY / iPlaceH;

    if ((visible_first_row == 0 && visible_last_row == lastRow) ||
        (current_icon_row >= visible_first_row && 
         current_icon_row <= visible_last_row)) {
     vert_inc_needed = 0;
        if (num_visible_rows > 1)
            pointerY = slider_area_y + (current_icon_row * iPlaceH) + 
                       (iPlaceH / 2);
        else
            pointerY = slider_area_y + (iPlaceH / 2);

    }
    else if (current_icon_row < visible_first_row) {

        vert_inc_needed = current_icon_row - visible_first_row;
        pointerY = slider_area_y + (iPlaceH / 2);

    }
    else if (current_icon_row > visible_last_row) {

        vert_inc_needed = current_icon_row - visible_last_row;
        pointerY = slider_area_y + 
                   ((visible_last_row - visible_first_row) * iPlaceH) +
                   (iPlaceH / 2);

    }
   frame_icon_prop->iconBoxInfo.iconboxY = ib->icon_box_y;
    frame_icon_prop->iconBoxInfo.iconboxWidth = ib->icon_box_width;
    frame_icon_prop->iconBoxInfo.iconboxHeight = ib->icon_box_height;
    frame_icon_prop->iconBoxInfo.pointerX = pointerX;
    frame_icon_prop->iconBoxInfo.pointerY = pointerY;
    frame_icon_prop->iconBoxInfo.horiz_inc_needed = horiz_inc_needed;
    frame_icon_prop->iconBoxInfo.vert_inc_needed = vert_inc_needed;
    frame_icon_prop->iconBoxInfo.iconShellWin = ib->icon_shell_win;
    frame_icon_prop->iconBoxInfo.left_arrowX = ib->left_arrow_x;
    frame_icon_prop->iconBoxInfo.left_arrowY = ib->left_arrow_y;
    frame_icon_prop->iconBoxInfo.right_arrowX = ib->right_arrow_x; 
    frame_icon_prop->iconBoxInfo.right_arrowY = ib->right_arrow_y; 
    frame_icon_prop->iconBoxInfo.top_arrowX = ib->top_arrow_x;
    frame_icon_prop->iconBoxInfo.top_arrowY = ib->top_arrow_y;
    frame_icon_prop->iconBoxInfo.bottom_arrowX = ib->bottom_arrow_x;
    frame_icon_prop->iconBoxInfo.bottom_arrowY = ib->bottom_arrow_y;
    frame_icon_prop->iconBoxInfo.iconFrameWin = ib->frame_win;
     frame_icon_prop->iconBoxInfo.iconScrollWin = ib->scroll_win;  
    frame_icon_prop->iconBoxInfo.hScrollWin = ib->h_scroll_win;  
    frame_icon_prop->iconBoxInfo.vScrollWin = ib->v_scroll_win;  

}

#endif /* AUTOMATION */







round_quotient(num, denom)
int    num;
int    denom;
{
    double    act_quotient;
    int        quotient;

    if (denom <= 0) {
        exit(0);
    }
    act_quotient = (double)num / (double)denom;
    quotient = act_quotient;

    quotient = ((act_quotient - quotient) > 0.5) ? (quotient + 1) :
               quotient;

    return(quotient);
}



/****************************************************************************
 * Routines needed when using the older version of mwm automation
 ****************************************************************************/



#ifdef AUTOMATION

static Boolean
GetMwmFrameIconProp(
        Window window )
{

    char    *ret_data;
    Atom    frame_icon_atom, new_type;
    int     new_format;
    unsigned long    new_nitems, new_bytes_after;

    frame_icon_atom = XmInternAtom(display, "_MOTIF_WM_FRAME_ICON_INFO", False);
    XGetWindowProperty(display, window, frame_icon_atom, 0L, 
                       PROP_MWM_FRAME_ICON_INFO_ELEMENTS, False, 
                       AnyPropertyType, &new_type, &new_format,
                       &new_nitems, &new_bytes_after,
                       (unsigned char **)&ret_data);

    if (ret_data == NULL)
        return(False);

#ifdef DEBUG
	fprintf(stdout, "new_nitems from GetMwmFrameIconProp = %d\n", new_nitems);
#endif
	/*
	 * If property not read completely, we should get property again
	 * with the current "new_nitems" as the offset, and
	 * (PROP_MWM_FRAME_ICON_INFO_ELEMENTS - new_nitems) as the number
	 * expected size of the property. Need to add code to do this.
	 */
	if (new_bytes_after > 0)
		fprintf(stderr, "%d bytes not read from _MOTIF_WM_FRAME_ICON_INFO",
				new_bytes_after);
    memmove((char *)&frame_icon_prop, (char *)ret_data, new_nitems);

	if (ret_data != NULL)
		XFree((char *)ret_data);

	/* Swap bytes if necessary */
	AutoSwapBytes(&frame_icon_prop);

    return(True);

} /* GetMwmFrameIconProp */


static void
SetMwmFrameIconProp(
        Window window )
{

	Atom	frame_icon_atom;

    frame_icon_atom = XmInternAtom(display, "_MOTIF_WM_FRAME_ICON_INFO", 
								   False);
	frame_icon_prop.byte_order = AutoByteOrderChar;
    XChangeProperty(display, window, frame_icon_atom, frame_icon_atom, 
					8, PropModeReplace, (unsigned char *)&frame_icon_prop,
                    PROP_MWM_FRAME_ICON_INFO_ELEMENTS);
    XSync(display, False);

} /* SetMwmFrameIconProp */

#endif /* AUTOMATION */



