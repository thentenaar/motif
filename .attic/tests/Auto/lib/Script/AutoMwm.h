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
/*   $XConsortium: AutoMwm.h /main/11 1996/10/30 11:34:34 drk $ */
/*
 *	File: AutoMwm.h
 *
 *	Contains Automated regression info specific to mwm.
 */

#ifndef _AutoMwm_h_
#define _AutoMwm_h_




#define	MAX_NAME_LEN	95
#define	MAX_MENU_ITEMS	20

#define	MAX_TITLE_REGIONS	4
#define	MAX_RESIZE_HANDLES	8


typedef struct _Auto_Rectangle
{
    INT16   x B16;
    INT16   y B16;
    CARD16  width B16;
    CARD16  height B16;
} AutoRectangle;

typedef struct _Auto_Gadget_Rectangle {

    INT32   id B32;
    CARD32  pad1 B32;
    AutoRectangle  rect;

} AutoGadgetRectangle;

typedef struct _Icon_Info {

    INT32   clientState B32;
    INT32   useIconBox B32;             /* Made int for alignment */
    INT32   X B32;
    INT32   Y B32;
    INT32   Width B32;
    INT32   Height B32;
    Window  iconFrameWin B32;
    CARD32  pad0 B32;

} IconRectInfo;

typedef struct _Icon_Box_Info {

    INT32       iconboxX B32;
    INT32       iconboxY B32;
    INT32       iconboxWidth B32;
    INT32       iconboxHeight B32;
    INT32       pointerX B32;           /* x, y to locate pointer */
    INT32       pointerY B32;
    INT32       top_arrowX B32;
    INT32       top_arrowY B32;
    INT32       bottom_arrowX B32;
    INT32       bottom_arrowY B32;
    INT32       left_arrowX B32;
    INT32       left_arrowY B32;
    INT32       right_arrowX B32;
    INT32       right_arrowY B32;
    INT32       vert_inc_needed B32;    /* -ve if decrement */
    INT32       horiz_inc_needed B32;   /* -ve if decrement */
    Window      iconFrameWin B32;       /* IconBox Frame window */
    Window      iconShellWin B32;       /* IconBox toplevel Shell window */
    Window      iconScrollWin B32;      /* IconBox Scrolled Window */
    Window      hScrollWin B32;         /* Horiz. ScrollBar Window */
    Window      vScrollWin B32;         /* Vert. ScrollBar Window */
    CARD32      pad0 B32;

} IconBoxRectInfo;


typedef struct _Menu_Info {

    INT32   sensitive B32;              /* 1 or 0 only */
    CARD32  item_y B32;
    char    item_name[MAX_NAME_LEN + 1];

} MenuInfo;


/*
 * Contents of the _MWM_FRAME_INFO property.
 */

typedef struct _Prop_MotifWm_Frame_Icon_Info
{
	BYTE				byte_order;             /* Byte order on the system */
    BYTE				pad0;
    INT32               windowX B32;            /* Window's position */
    INT32               windowY B32;
    INT32               upperBorderWidth B32;   /* Upper Border Width */
    INT32               lowerBorderWidth B32;   /* Lower Border Width */
    Window              frameWin B32;           /* Frame Window */
    Window              titleWin B32;           /* Title Window */
    Window              menuWin B32;            /* System Menu Window */
    CARD32              pad1 B32;
    AutoRectangle       titleRect;              /* Frame Info */
    AutoGadgetRectangle titleGadgets[MAX_TITLE_REGIONS];/* Title Box regions */
    INT32               titleGadgetCount B32;
    CARD32              pad2 B32;
    AutoGadgetRectangle resizeGadgets[MAX_RESIZE_HANDLES]; /* Resize Handles */
    MenuInfo            windowMenu[MAX_MENU_ITEMS]; /* Window Menu Info */
    INT32               menuItemCount B32;          /* Total no. of items */
    INT32               sensitiveItemCount B32;    /* No. of sensitive items */
    IconRectInfo        iconInfo;                   /* Icon Info */
    IconBoxRectInfo     iconBoxInfo;                /* Icon Info in IconBox */

} PropMotifWmFrameIconInfo;







typedef struct _Prop_MotifWm_Frame_Icon_Info	PropMwmFrameIconInfo;


/* number of bytes in _MWM_FRAME_ICON_INFO */
#define PROP_MOTIF_WM_FRAME_ICON_INFO_ELEMENTS	\
						sizeof(PropMwmFrameIconInfo)
#define PROP_MWM_FRAME_ICON_INFO_ELEMENTS	\
						PROP_MOTIF_WM_FRAME_ICON_INFO_ELEMENTS

/* atom name for _MWM_FRAME_INFO property */
#define _XA_MOTIF_WM_FRAME_ICON_INFO 	"_MOTIF_WM_FRAME_ICON_INFO"
#define _XA_MWM_FRAME_ICON_INFO		_XA_MOTIF_WM_FRAME_ICON_INFO

#define	MWM_FRAME_TITLE		0
#define	MWM_FRAME_SYSTEM	1
#define	MWM_FRAME_MINIMIZE	2
#define	MWM_FRAME_MAXIMIZE	3

#define	MWM_NORMAL_STATE	1
#define	MWM_ICONIC_STATE	2
#define	MWM_MAXIMIZED_STATE	3



/* Group Names for item checking */
#define SYSTEM_MENU			0
#define	ICON_MENU			1
#define	WINDOW_DECORATION	2




extern unsigned char	AutoByteOrderChar;






typedef struct {
    int move_x;
    int move_y;
} MoveData;


typedef struct {
    int width;
    int height;
    int gravity;

} ResizeData;


typedef struct {
    int item_num;
    char item_name[MAX_NAME_LEN + 1];
    Boolean keyboard;
} ItemSelectData;


typedef struct {
    Boolean keyboard;
} MenuPostData;

typedef struct {
    int group;
    char item_name[MAX_NAME_LEN + 1];
} ItemCheckData;


typedef struct {

  CARD32 h_min;
  CARD32 h_max;
  CARD32 h_slider_area_width;
  CARD32 h_slider_x;
  CARD32 h_slider_area_x;
  CARD32 v_min;
  CARD32 v_max;
  CARD32 v_slider_area_height;
  CARD32 v_slider_y;
  CARD32 v_slider_area_y;
  CARD32 right_arrow_x;
  CARD32 right_arrow_y;
  CARD32 left_arrow_x;
  CARD32 left_arrow_y;
  CARD32 top_arrow_x; 
  CARD32 top_arrow_y; 
  CARD32 bottom_arrow_x;
  CARD32 bottom_arrow_y;
  CARD32 icon_box_x; 
  CARD32 icon_box_y;
  CARD32 icon_box_width;
  CARD32 icon_box_height;
  CARD32 icon_shell_win;
  CARD32 frame_win;
  CARD32 scroll_win;
  CARD32 h_scroll_win;
  CARD32 v_scroll_win;
  CARD32 last_col;
  CARD32 last_row;
  CARD32 i_place_w;
  CARD32 i_place_h;
} IconBoxInfo;



/* This is used to pass the data from the request
   setup routine to the callback routine where the
   data is actually transferred. */

#ifndef AUTOMATION

typedef struct _UTMPackageRec {
  int fmt;
  Atom target;
  unsigned long len;
  XtPointer param;
  XtCallbackProc doneProc;
  XtPointer closure;
} UTMPackageRec;

/*
 * The following keeps track of multiple utm requests and
 * pulls the right data off
 */

typedef struct _DataQueueRec {
  XtPointer data;
  struct _DataQueueRec *next, *prev;
} DataQueueRec;



#define WINDOW_MINIMIZE                    0
#define WINDOW_MAXIMIZE                    1
#define WINDOW_MOVE                        2
#define WINDOW_RAISE                       3
#define WINDOW_RESIZE                      4
#define WINDOW_MENU_ITEM_SELECT            5
#define WINDOW_DEICONIFY                   6
#define WINDOW_NORMALIZE                   7
#define WINDOW_MENU_POST                   8       
#define WINDOW_FOCUS                       9
#define WINDOW_MENU_UNPOST                 10
#define WINDOW_ITEM_CHECK                  11
#define ICON_MOVE                          12
#define ICON_MENU_POST                     13     
#define ICON_MENU_UNPOST                   14    
#define ICON_MENU_ITEM_SELECT              15     



#endif




#ifndef AUTOMATION


extern void AutoWmRaise( 
                        Window window);
extern void AutoWmIconify( 
		    Window window);
extern void AutoWmDeIconify( 
                        Window window);
extern void AutoWmMaximize( 
                        Window window);
extern void AutoWmMenuPost( 
                        Window window, 
			Boolean keyboard);
extern void AutoWmMenuUnpost( 
                        Window window);
extern void AutoWmMenuSelect( 
                        Window window,
                        int item_num,
                        char *item_name, 
			Boolean keyboard);
extern void AutoWmMove( 
                        Window window,
                        int x,
                        int y);
extern void AutoWmResize( 
                        Window window,
                        int gravity,
                        int width,
                        int height);
extern void AutoWmFocus( 
                        Window window);
extern void AutoWmIconMenuPost( 
                        Window window, 
			Boolean keyboard);
extern void AutoWmIconMenuUnpost( 
                        Window window);
extern void AutoWmIconMenuSelect( 
                        Window window,
                        int item_num,
                        char *item_name, 
			Boolean keyboard);
extern void AutoWmIconMove( 
                        Window window,
                        int x,
                        int y);
extern void AutoWmItemCheck( 
                        Window window,
                        int group,
                        char *item_name);
extern void AutoWmNormalize (
                        Window window);

extern void LocatePtr( 
                        Window window,
                        int dest_x,
                        int dest_y);
extern void AutoInitByteOrderChar(void );
extern void AutoSwapBytes(PropMwmFrameIconInfo *window_prop);


#else



extern Boolean AutoWmRaise( 
                        Window window);
extern Boolean AutoWmIconify( 
		    Window window);
extern Boolean AutoWmDeIconify( 
                        Window window);
extern Boolean AutoWmMaximize( 
                        Window window);
extern Boolean AutoWmMenuPost( 
                        Window window, 
						Boolean keyboard);
extern Boolean AutoWmMenuUnpost( 
                        Window window);
extern Boolean AutoWmMenuSelect( 
                        Window window,
                        int item_num,
                        char *item_name, 
						Boolean keyboard);
extern Boolean AutoWmMove( 
                        Window window,
                        int x,
                        int y);
extern Boolean AutoWmResize( 
                        Window window,
                        int gravity,
                        int width,
                        int height);
extern Boolean AutoWmFocus( 
                        Window window);
extern Boolean AutoWmIconMenuPost( 
                        Window window, 
						Boolean keyboard);
extern Boolean AutoWmIconMenuUnpost( 
                        Window window);
extern Boolean AutoWmIconMenuSelect( 
                        Window window,
                        int item_num,
                        char *item_name, 
						Boolean keyboard);
extern Boolean AutoWmIconMove( 
                        Window window,
                        int x,
                        int y);
extern Boolean AutoWmItemCheck( 
                        Window window,
                        int group,
                        char *item_name);
extern void LocatePtr( 
                        Window window,
                        int dest_x,
                        int dest_y);
extern void AutoInitByteOrderChar(void );
extern void AutoSwapBytes(PropMwmFrameIconInfo *window_prop);


#endif /* AUTOMATION */





#endif /* _AutoMwm_h_ */


