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
/*   $XConsortium: xis_protos.h /main/7 1995/07/14 11:43:40 drk $ */
/***********************************************************************
  	@(#)xis_protos.h	1.4.1.1	Date:1/22/91
  	Author: SJS
	History:
            08/08/90 SJS prototypes moved to here.
	Calls:

	Summary:
************************************************************************/

/*  This file need to be included by xislib.h because of type definitions */

/******************************************************************************
                EVENT FUNCTIONS
******************************************************************************/

extern Time xisGetEventTime /* Gets time stamp from event record. Returns 0
                               if none */

    PARAMS(( XEvent         *event               /*  Ptr to X-event struct */
          ));

extern Time xisGetServerTime /* Get approx current server time. Delay
                                specified amount*/

    PARAMS(( Time            inter_button_time   /* Min delay time since     */
                                                 /* last button press.       */
          ));

extern void xisMovePointer /* Move pointer to new x,y location with
                              linear motion    */

    PARAMS(( int             new_x,              /* Destination root x coord */
             int             new_y               /* Distination root y coord */
          ));

extern void xisPrintEvent /* Print relavent trace details of the specified
                             event */

    PARAMS(( XEvent         *event               /* Ptr to X-event struct    */
          ));

extern int xisSynchronize   /* Process events until system reaches next quiet
                               state.  Returns True if processed at least one
                               event */
    PARAMS(( void ));

extern void xisProcessEvents /* Process events until done_func = true or
                                time-out */

    PARAMS(( int           (*done_func)(),       /* Func that returns True   */
                                                 /* when done procesing evnts*/

             int             max_wait_time       /* Max time-out time in     */
                                                 /* milliseconds.            */
          ));

extern void xisProcessObjects /* Regenerate objects hierarchy */
    PARAMS(( void ));

extern void xisSendEvent /* Sends 1 synthesized event to the specified window */

    PARAMS(( Window          window,             /* Event window             */
             Window          subwindow,          /* Child windw toward action*/
             int             type,               /* XEvent type              */
             Time            time,               /* X-Server time stamp      */
             int             wx,                 /* X-coord of pntr in window*/
             int             wy,                 /* Y-coord of pntr in window*/
             int             rx,                 /* X-coord of pntr in root  */
             int             ry,                 /* Y-coord of pntr in root  */
             unsigned int    state,              /* XServer modifier keys    */
             unsigned int    detail              /* Key,Button changed state */
          )); 


/******************************************************************************
                INFO FUNCTIONS
******************************************************************************/

extern XisObjectRecord *xisAllocObject /* Allocates one object record and
                                          returns a pointer to it*/

    PARAMS(( XisObjectRecord *parent_obj,        /* Parent of new object     */
             Widget          widget,             /* Widget id of new object  */
             int             object_code,        /* Object code (user defined*/
             int             instance            /* Instance code(userdefined*/
          ));


extern void xisBeginSession /* Opens and initializes dialog with input synth
                               tools */

    PARAMS(( Display        *display_in,         /* Display connected to      */
             XtAppContext    app_context_in,     /* Xt Application Context    */
             int             num_objects_types,  /* Num unique objects        */
             void          (*generate_objects_func)(),/*Func updates objects  */
             char         *(*get_obj_type_name_func)(),/*Returns obj type name*/
             char         *(*get_class_name_func)(),/*Returns widg class name */
             Widget        (window_to_widget_func)(),/*Returns widg from windo*/
             void          (*trace_msg_func)(),    /* Trace message function  */
             void          (*warning_msg_func)(),  /* Warning message function*/
             void          (*error_msg_func)()     /* Error message function  */
          ));


extern void xisFreeObject /* Frees the specified object record storage */

    PARAMS(( XisObjectRecord *object             /* Object to free           */
          ));


extern XisInformRecord *xisGetInformInfo /* Gets pointer to xisInform info
                                            struct */
    PARAMS(( void ));


extern XisObjectRecord *xisGetObjectFromWidget /* Gets top level object
                                                  pointer from widget id   */

    PARAMS(( Widget          widget              /* Widget to map to object  */
          ));


extern XisStateRecord *xisGetStateInfo /* Gets pointer to xisState info
                                          struct */
    PARAMS(( void ));


extern void xisUseSessionInfo /* Checks if xisInitDisplayInfo called
                                 previously */

    PARAMS(( char           *routine_name        /* Routine name called from */
          ));



/******************************************************************************
                OTHER FUNCTIONS
******************************************************************************/

extern long xisConcat /* Concatenates two object types together */

    PARAMS(( long             cum_object_type,   /* Cumulative obj type      */
             long             new_object_type    /* New obj to concat on cum */
          ));

extern void xisCreateSimple /* Creates a simple window which may be used as  */
                            /* a "dummy" focus                               */

    PARAMS(( void ));

extern XisObjectRecord *xisFindObject /* Looks up the object with matching
                                         widget,obj,instance */

    PARAMS(( Widget          widget,             /* Object widget id         */
             int             object_type,        /* Object object_code       */
             int             instance            /* Object instance code     */
          ));


extern XisObjectRecord *xisFindObjectAtLocation /* Looks up the topmost object
                                                   at location x,y */

    PARAMS(( int             x,                  /* X-coord rel to root      */
             int             y                   /* Y-coord rel to root      */
          ));


extern XisObjectRecord *xisFindObjectFromWidget /* Looks up object with
                                                   matching widget */

    PARAMS(( Widget          widget              /* Widget to match          */
          ));


extern XisObjectRecord *xisFindObjectFromWindow /* Looks up object with
                                                   matching window id */

    PARAMS(( Window          window              /* Window id to match       */
          ));


extern XisObjectRecord *xisFindParentObjectFromWindow /* Looks up parent obj
                                                         with matching window*/

    PARAMS(( Window          window              /* Window id to match       */
          ));


extern int xisGetEdgeCode /* Returns TopEdge, BottomEdge, LeftEdge, or
                             RightEdge */

    PARAMS(( XisObjectRecord *object,            /* object in question       */
             int              old_x,             /* X coord moved ptr from   */
             int              old_y,             /* Y coord moved ptr from   */
             int              new_x,             /* X coord moved ptr to     */
             int              new_y              /* Y coord moved ptr to     */
          ));


extern XisObjectRecord *xisGetFocusObject /* Returns object/widget that has the
                                             keyboard input focus */
    PARAMS(( void ));

extern Widget xisGetFocusWidget /* Returns widget that has the MOTIF
                                             keyboard input focus */
    PARAMS(( void ));

extern char *xisGetKeyName /* Returns a ptr to a string describing an xis key */

    PARAMS(( int              key                /* xis key code             */
          ));


extern char *xisGetObjectName /* Returns pointer to string name of object */

    PARAMS(( XisObjectRecord *object             /* Object whose name you want*/
          ));


extern void xisGetPointerInfo /* Returns pointer location information */

    PARAMS(( Window          *ptr_window,        /* Innermost window ptr is in*/
             int             *root_x,            /* X-coord rel to root window*/
             int             *root_y,            /* Y-coord rel to root window*/
             int             *window_x,          /* X-coord rel to ptr window */
             int             *window_y,          /* Y-coord rel to ptr window */
             unsigned int    *button_mask        /* Currently pressed buttons */
          ));


extern void xisGetPointerLocation /* Returns current pointer location in root
                                     window */

    PARAMS(( int             *root_x,            /* X-coord rel to root window*/
             int             *root_y             /* Y-coord rel to root window*/
          ));


extern Widget xisGetShellWidget /* Returns the topmost shell of the
                                   specified widget */

    PARAMS(( Widget           widget             /* Widget in question       */
          ));


extern char *xisGetWindowName /* Returns the name of the specified window */

    PARAMS(( Window          window              /* Window to get name of    */
          ));


extern int xisIsAncestorObject /* Returns True if 1st object is ancestor of
                                  2nd */

    PARAMS(( XisObjectRecord *object1,           /* 1st object struct ptr    */
             XisObjectRecord *object2            /* 2nd object struct ptr    */
          ));


extern int xisIsAncestorWindow /* Returns True if 1st window is ancestor of
                                  2nd */

    PARAMS(( Window           window1,           /* 1st window               */
             Window           window2            /* 2nd window               */
          ));


extern int xisIsMapped /* Returns True if window is mapped */

    PARAMS(( Window           window             /* Window in question       */
          ));


extern int xisIsViewable /* Returns True if window is viewable */

    PARAMS(( Window           window             /* Window in question       */
          ));


extern int xisIsWidgetMapped /* Returns True if widget is mapped */

    PARAMS(( Widget           widget             /* Widget in question       */
          ));


extern void xisInitRandom /* Initializes the pseudo random num generator
                             with seed */

    PARAMS(( int              seed               /* Seed, any integer value  */
          ));


extern int xisArbitrary /* Returns arbitrary num between inclusive values
                           range */

    PARAMS(( int              lo,                /* Low inclusive number    */
             int              hi                 /* High inclusive number   */
          ));


extern void xisSetMultiClickTime /* Sets xis global min inter-click time
                                    interval */
  
    PARAMS(( Time             time               /* Time in milliseconds    */
          ));


extern int xisTerminateRequested /* Returns true if Ctrl-C pressed */
    PARAMS(( void ));


extern void xisUngrabMouseButton /* Ungrabs the mouse button */
    PARAMS(( void ));


extern int xisWindowExists /* Returns True if window exists */

    PARAMS(( Window           window             /* Window in question */
          ));
    


/******************************************************************************
                PRIMITIVE BEHAVIOR FUNCTIONS
******************************************************************************/

extern void xisCommonButtonPress /* Central button press synthesis routine */

    PARAMS(( unsigned int     modifier_keys,     /* ShiftMask,ControlMask,etc*/
             int              button_num,        /* Mouse button num 1 - 5   */
             int              clicks             /* Num button multi clicks  */
          ));


extern void xisCMovePointer /* Common Move pointer code for both routines
                               above */

    PARAMS(( XisObjectRecord *current_obj,       /* Current object           */
             int             curr_x,             /* Curr pointer root x coord*/
             int             curr_y,             /* Curr pointer root y coord */
             int             target_x,           /* Destination root x coord */
             int             target_y            /* Destination root y coord */
          ));


extern void xisClickMouseButton /* Press-hold-release mouse button */

    PARAMS(( unsigned int     modifier_keys,     /* ShiftMask,ControlMask,etc*/
             int              button_num         /* Mouse button num 1 - 5   */
          ));


extern void xisClickKey /* Press-hold-release mouse button */

    PARAMS(( unsigned int     modifier_keys,     /* ShiftMask,ControlMask,etc*/
             int              key                /* Xis key code             */
          ));


extern void xisCreateWidget /* Process InformExpectedActions on widget
                               creation */

    PARAMS(( Widget           widget             /* Widget id of new widget */
          ));


extern void xisDestroyWidget /* Process InformExpectedActions on widget
                                destruction */

    PARAMS(( Widget           widget             /* Widget id of new widget */
          ));


extern int xisIsActionRegion /* Returns true is specified region is where
                                event occur*/

    PARAMS(( int              object_type        /* Object type code        */
          ));


extern int xisIsActiveMouseButton /* Returns true is specified button_num is
                                     active */

    PARAMS(( unsigned int     modifier_keys,     /* ShiftMask,ControlMask,etc*/
    	     int              button_num         /* Button number 1- 5      */
          ));

extern int xisIsActiveKey        /* Returns true is specified key is active */

    PARAMS(( unsigned int     modifier_keys,     /* ShiftMask,ControlMask,etc*/
    	     int              key_code         /* Key is Space,Return...  */
          ));

extern int xisIsKeyboardFocusRegion /* Returns True if region is in keyboard
                                       focus */

    PARAMS(( int              object_type        /* Object type code        */
          ));


extern int xisIsPointerOverRegion /* Returns True if pointer over region */

    PARAMS(( int              object_type        /* Object type code        */
          ));


extern int xisIsPointerOverWidget /* Returns True if pointer is over curr
                                     widget */

    PARAMS(( int              bool               /* False = negative logic   */
          ));


extern int xisIsSelectedRegion /* Returns True if specified region is selected*/

    PARAMS(( int              object_type_in     /* Object type code         */
          ));


extern int xisIsWidgetInKeyboardFocus /* Returns True if widget is in keyboard
                                         focus */

    PARAMS(( int              bool               /* False = negative logic    */
          ));


extern int xisIsWidgetSelected /* Returns True if curr widget is selected */

    PARAMS(( int              bool               /* False = negative logic    */
          ));

extern int xisIsMouseButtonDown /* Returns True if given button | mod keys are
                                   down */

    PARAMS(( unsigned int     modifier_keys,     /* ShiftMask,ControlMask,etc*/
    	     int              button_num         /* Btn1,2,3.. */
          ));


extern void xisMultiClickMouseButton /* Synthesizes a multiclick mouse button
                                        event  */

    PARAMS(( unsigned int     modifier_keys,     /* ShiftMask,ControlMask,etc*/
             int              button_num,        /* Mouse button num 1-5     */
             int              num_clicks         /* Num clicks               */
          ));


extern void xisMovePointerRel /* Move pointer to new x,y location with linear
                                 motion */

    PARAMS(( int             new_x,              /* Destination root x coord */
             int             new_y               /* Distination root y coord */
          ));


extern void xisMovePointerTo /* Move pointer over specified widget, object,
                                instance */

    PARAMS(( Widget           widget,            /* Specified widget         */
             int              object_code,       /* Specified object         */
             int              instance,          /* Specified instance       */
             int              direction          /* MoveUp, MoveDown, etc    */
          ));


extern void xisPressMouseButton /* Synthesize a mouse button press event */

    PARAMS(( unsigned int     modifier_keys,     /* ShiftMask,ControlMask,etc*/
             int              button_num         /* Mouse button num 1 - 5   */
          ));


extern void xisProcessObjects   /* Regenerate the graphical objects data
                                   structure */
    PARAMS(( void ));


extern void xisReleaseMouseButton /* Synthesize a mouse button release event */

    PARAMS(( unsigned int     modifier_keys,     /* ShiftMask,ControlMask,etc*/
             int              button_num         /* Mouse button num 1 - 5   */
          ));


extern void xisRestorePointer /* Puts pointer back to location saved by
                                 xisSavePointer*/
    PARAMS(( void ));


extern void xisSavePointer /* Saves pointer location for restoring
                              w/xisRestorePointer*/
    PARAMS(( void ));


extern void xisSetKeyboardFocus /* Sets keyboard focus to specified widget */

    PARAMS(( Widget           widget,            /* Widget to set focus to   */
             int              object_type,       /* Object type              */
             int              instance           /* Instance of object       */
          ));


extern void xisSetKeyboardFocusObject /* Saves global keyboard focus setting */

    PARAMS(( Widget           widget,            /* Widget to set focus to   */
             int              object_type,       /* Object type              */
             int              instance           /* Instance of object       */
          ));


extern void xisSetFocusMode /* Sets keyboard focus mode to natural or
                               controlled */

    PARAMS(( int              mode               /* KEYBOARD_FOCUS_NATURAL,..*/
          ));

extern int xisGetDragDistance /* Get drag distance and widnow id */

    PARAMS(( Widget	      *pwin, 		 /* Returned widget id */
	     int	      *pdel_x, 		 /* Returned delta x */
	     int	      *pdel_y		 /* Returned delta y */
	  ));


/* End of xis_protos.h   Add new definitions ABOVE THIS LINE */
