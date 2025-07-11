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
/*   $XConsortium: xislib.h /main/10 1996/10/30 11:34:43 drk $ */
/***********************************************************************
  	@(#)xislib.h	1.46     Date:03/18/91                        
  	Author: TAT
	History:
            01/05/89 TAT Initial implementation
            04/09/90 TAT Major Update with X11R4; remove syncs
            04/23/90 TAT Reorganize to input synth bare essentials and move
                         all else to mvs library
            05/30/90 SJS add to sccs
            06/01/90 PSN add defines for key, button & modifier combination
            06/05/90 PSN add prototype for new pi xisGetDragDist()
            06/07/90 PSN add prototype for new pi xisIsMouseButtonDown()
            07/02/90 SJS more key codes
            07/03/90 PSN Fixed key code defines to make it easy to insert new
			 key codes any where in the list
            08/09/90 SJS Use key code that are vendor independent.
                         Move prototypes to xis_protos.h
            12/06/90 PSN Added global to keep track of natural pointer mode
			 (OwnerGrabMode) so that, IS does not grab pointer
			 incorrectly.
	Calls:

	Summary:
            Follows below
************************************************************************/
/*************************************************************     
    MOTIF INPUT SYNTHESIS UTILITIES DEFINITIONS (INCLUDE FILE)

*************************************************************/
/************************************************************
    
  DESCRIPTION:

    This module contains structure definitions preprocessor
    definitions and globals which are defined for use with
    the xislib.c module.

    In xislib.c the preprocessor constant 
    DECLARE_XISLIB_GLOBALS_HERE is defined which causes the
    generation of storage defining code for globals here
    where in all other places that this file is included 
    which does not define this constant only references to
    these globals are made.

    Also, it is assumed that your C compiler handles ANSI-C
    prototype function declarations (where additional type
    checking can be done on your parameters). Where this 
    is not the case, precede inclusion of this file with
    NOT_ANSI_C defined or just put its definition in your
    cc command line.

**********************************************************/

#ifndef XISLIB
#define XISLIB

#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include <X11/Xlib.h>
#include <X11/Xproto.h>

#include <X11/Intrinsic.h>

#include "mvs_defines.h" 

/************* define xisGLOBAL, xisINIT, PARAMS *****************************/

#ifdef DECLARE_XISLIB_GLOBALS_HERE
#define xisGLOBAL  
#define xisINIT(x) = x
#else
#define xisGLOBAL extern
#define xisINIT(x)
#endif


/******************************************************************************
                GLOBAL DISPLAY AND APPLICATION CONTEXT INFO
******************************************************************************/

  xisGLOBAL XtAppContext xisAppContext xisINIT(NULL);
                                            /* Application's Xt context       */

  xisGLOBAL Display *xisDisplay xisINIT(NULL);
                                            /* Display test is connected to   */

  xisGLOBAL int xisScreen;                  /* Default screen number          */
  xisGLOBAL Window xisRootWindow;           /* Root window on default screen  */

  /* NOTE: This version of the input synthesis utils runs only on 1 screen    */

  xisGLOBAL Window xisSyncWindow xisINIT(0L);
                                            /* Used in xisProcessEvents       */
                                            /* to sync WM,Server,and Client   */

  xisGLOBAL Window xisDummyFocusWindow xisINIT(0L);
                                            /* Used to set focus away from app*/

  xisGLOBAL Time xisLastEventTime xisINIT(0);          
                                            /* X-server most recent event time*/

  xisGLOBAL Time xisLastButtonPressTime xisINIT(0);    
                                            /* X-server most recent b.prs time*/

  xisGLOBAL Time xisMultiClickTime xisINIT(0);         
                                            /* Max time between button clicks */

  xisGLOBAL short xisUseSyntheticTime xisINIT(0);          
                                            /* True = synthesize event times  */

  xisGLOBAL Time xisSyntheticTime xisINIT(0);
                                            /* Fake X-server time in millisecs*/

  xisGLOBAL void (*xisTraceMsg)(  )  xisINIT(NULL); 
                                            /* Trace message func like printf */

  xisGLOBAL void (*xisWarningMsg)() xisINIT(NULL);
                                            /* Warning message func like print*/

  xisGLOBAL void (*xisErrorMsg)() xisINIT(NULL);
                                            /* Error message func like printf */

  xisGLOBAL int xisTerminateTestFlag xisINIT(0);
                                            /* Set true when user does ctrl-C */

/******************************************************************************
                OBJECT DECLARATION
******************************************************************************/


  typedef struct _XisObjectIdRecord {
      Widget            widget;
      Window            window;
      int               object_type;
      int               instance;
      Window            window_under_parent; /* If Shell, set to window    */
                                             /* under root, else set to    */
                                             /* same window as in id.window*/

      /* From here to end of record is for XQueryTree info caching */ 

      Window            root_window;       /* For multi-screen tests */
      Window            parent_window;
      unsigned int      num_children;
      Window            *children_windows; /* In stack order from bot to top */
  } XisObjectIdRecord;

  typedef void (*XisInformExpectedActionsProc)();

  typedef struct _XisObjectRecord {
    XisObjectIdRecord            id;
    int                          id_code;  /* small unique portable int id */
    int                          in_use;
    int                          x,y,width,height;
    int                          visibility_state;
    unsigned long                your_event_mask;
    unsigned long                do_not_propagate_mask;
    struct _XisObjectRecord      *parent;
    struct _XisObjectRecord      *first_child;
    struct _XisObjectRecord      *next_sibling;
    XisInformExpectedActionsProc proc_InformExpectedActions;
  } XisObjectRecord;

# define MAX_NUM_OBJECTS 5000
  xisGLOBAL XisObjectRecord xisObjects[MAX_NUM_OBJECTS] xisINIT({ 0 });
  xisGLOBAL int xisMaxObjectNum xisINIT(0);

# define NULL_Object (XisObjectRecord *)NULL

  xisGLOBAL int xisNumObjectTypes xisINIT(0);

  xisGLOBAL void (*xisGenerateObjects)(/*root_obj*/) xisINIT(NULL);
  xisGLOBAL char *(*xisGetObjectTypeName)(/* int object_type */) xisINIT(NULL);
  xisGLOBAL char *(*xisGetWidgetClassName)(/* Widget widget */) xisINIT(NULL);
  xisGLOBAL Widget (*xisWindowToWidget)(/* Window window */) xisINIT(NULL);


/******************************************************************************
                DEFINE InformExpectedActions FUNCTION POINTER
******************************************************************************/

/* XIS Inform Record contains information which is valid only during a call  */
/* of an InformExpectedAction function.  It is made global so that all of the*/
/* xis state querying commands can access this information without explicitly*/
/* passing it as a parameter (each routine needs only a portion of all these */
/* fields).                                                                  */
 
typedef struct _XisInformRecord {
    /* These fields are always valid */
    int             is_valid;           /* True when info here is valid      */
    int             event_code;         /* The event_code passed to InfExpAct*/
    XisObjectRecord *current_obj;       /* The current obj w/InformExpectActi*/
    XisObjectRecord *action_obj;        /* The object where the action ocured*/
    XisObjectRecord *pointer_obj;       /* The object the pointer is over    */

    /* These fields are usable only by Enter/Leave events                    */
    XisObjectRecord *leave_obj;         /* The object being left             */
    XisObjectRecord *enter_obj;         /* The object to be entered          */

    /* These fields depend on which event occurred (see below) */
    unsigned int    modifier_key_status;/* The modifier keys held down       */
    int             button_num;         /* The button which did something    */
    int             key_code;           /* The xis keycode which did something*/
    int             edge_code;          /* The top,bot,left,rght edge crossed*/
    int             num_clicks;         /* The number of key/button clicks   */
    int             del_x,del_y;        /* Change in pntr position for motion*/
    int             new_width,new_height;/* Resized size                     */
} XisInformRecord;

xisGLOBAL XisInformRecord xisInform;


/* POSSIBLE EVENT CODE VALUES */      /* Relavent xisInform record fields     */

# define EventMouseButtonDown       0 /* modifiers,button_num                 */
# define EventMouseButtonUp         1 /* modifiers,button_num                 */
# define EventMouseButtonMultiClick 2 /* modifiers,button_num,num_clicks      */
# define EventKeyClick              3 /* modifiers,key_code                   */
# define EventPointerEnter          4 /* edge_code                            */
# define EventPointerLeave          5 /* edge_code                            */
# define EventFocusIn               6 /* none                                 */
# define EventFocusOut              7 /* none                                 */
# define EventExpose                8 /* none                                 */
# define EventDestroy               9 /* none                                 */
# define EventMap                  10 /* none                                 */
# define EventUnmap                11 /* none                                 */
# define EventKeyPress             12 /* modifiers,key_code                   */
# define EventKeyRelease           13 /* modifiers,key_code                   */
# define EventPointerMotion        14 /* del_x,del_y                          */
# define EventResize               15 /* new_width, new_height                */
# define MAXEVENT                  16


/* POSSIBLE MODIFIER KEY CODE MASKS */

  /*
     Use those already define in X.h:

     ShiftMask, LockMask, ControlMask, Mod1Mask, Mod2Mask, Mod3Mask, Mod4Mask
  */


/* POSSIBLE BUTTON NUM CODES */

  /*
     Use those already defined in X.h: 
     
     Button1, Button2, Button3, Button4, Button5
  */ 
 

/* POSSIBLE EDGE CODE VALUES */

#define UndefinedEdge  0
#define TopEdge        1
#define BottomEdge     2
#define LeftEdge       3
#define RightEdge      4


/******************************************************************************
                MOUSE POINTER AND BUTTON MANAGEMENT DECLARATIONS
******************************************************************************/

  xisGLOBAL int xisSavePointerX;         /* Position of pointer rel. to root  */
  xisGLOBAL int xisSavePointerY;         /* when xisSavePointer() was called. */

  xisGLOBAL int xisPointerSaved xisINIT(0); /* Flag indicating pntr was saved.*/

  xisGLOBAL int xisPointerGrabbed xisINIT(0);/* Flag indicating pntr is grabed*/
  xisGLOBAL Window xisGrabPointerWindow; /* Window where pointer was grabbed. */
  xisGLOBAL Window xisGrabPointerSubwindow; /* Subwindow where button was pressed. */

  xisGLOBAL Window xisSaveFocusWindow;   /* Keyboard focus window.            */
  xisGLOBAL int xisSaveFocusRevert;      /* Keyboard focus revert flag.       */

#ifndef DECLARE_XISLIB_GLOBALS_HERE
  xisGLOBAL unsigned int xisMouseButtonMask[]; /* Maps button num to mask */
#else
  xisGLOBAL unsigned int xisMouseButtonMask[6] = 
    { 0, Button1Mask, Button2Mask, Button3Mask, Button4Mask, Button5Mask };
#endif

#ifndef DECLARE_XISLIB_GLOBALS_HERE
  xisGLOBAL unsigned int xisMouseButtonDetail[]; /* Maps button num to detail */
#else 
  xisGLOBAL unsigned int xisMouseButtonDetail[6] = 
    { 0, Button1, Button2, Button3, Button4, Button5 };
#endif

/* Move pointer direction codes */

#define MoveAny        0
#define MoveLeft       1
#define MoveRight      2
#define MoveUp         3
#define MoveDown       4

/* Keep track of last selected widget (where button pressed) */

  xisGLOBAL XisObjectRecord *xisSelectedObject   xisINIT(NULL);


/******************************************************************************
                KEYBOARD SYNTHESIS DECLARATIONS
******************************************************************************/

#define MAX_KEY_NAME 32

/* Define keys which can be put in detail on KeyPress,KeyRelease events */

#define _KeyNone   		0
#define _KeyActivate 		(_KeyNone+1)
#define _KeyAddMode  		(_KeyActivate+1)
#define _KeyBackSpace    	(_KeyAddMode+1)
#define _KeyBeginLine   	(_KeyBackSpace+1)
#define _KeyCancel  		(_KeyBeginLine+1)
#define _KeyClear   		(_KeyCancel+1)
#define _KeyCopy   		(_KeyClear+1)
#define _KeyCut   		(_KeyCopy+1)
#define _KeyDelete 		(_KeyCut+1)
#define _KeyDown    		(_KeyDelete+1)
#define _KeyEndLine             (_KeyDown+1)
#define _KeyEnter		(_KeyEndLine+1)
#define _KeyEscape      	(_KeyEnter+1)
#define _KeyHelp     		(_KeyEscape+1)
#define _KeyInsert     		(_KeyHelp+1)
#define _KeyLeft     		(_KeyInsert+1)
#define _KeyMenu     		(_KeyLeft+1)
#define _KeyMenuBar     	(_KeyMenu+1)
#define _KeyPageDown     	(_KeyMenuBar+1)
#define _KeyPageUp     		(_KeyPageDown+1)
#define _KeyPaste     		(_KeyPageUp+1)
#define _KeyPrimaryPaste     	(_KeyPaste+1)
#define _KeyQuickPaste     	(_KeyPrimaryPaste+1)
#define _KeyReturn  		(_KeyQuickPaste+1)
#define _KeyRight     		(_KeyReturn+1)
#define _KeySelect     		(_KeyRight+1)
#define _KeySpace     		(_KeySelect+1)
#define _KeyTab     		(_KeySpace+1)
#define _KeyUndo     		(_KeyTab+1)
#define _KeyUp     		(_KeyUndo+1)
#define _Keya     		(_KeyUp+1)
#define _Keyb     		(_Keya+1)
#define _Keyc     		(_Keyb+1)
#define _Keyd     		(_Keyc+1)
#define _Keye     		(_Keyd+1)
#define _Keyf     		(_Keye+1)
#define _Keyg     		(_Keyf+1)
#define _Keyh     		(_Keyg+1)
#define _Keyi     		(_Keyh+1)
#define _Keyj        		(_Keyi+1)
#define _Keyk        		(_Keyj+1)
#define _Keyl        		(_Keyk+1)
#define _Keym        		(_Keyl+1)
#define _Keyn        		(_Keym+1)
#define _Keyo        		(_Keyn+1)
#define _Keyp        		(_Keyo+1)
#define _Keyq        		(_Keyp+1)
#define _Keyr        		(_Keyq+1)
#define _Keys        		(_Keyr+1)
#define _Keyt        		(_Keys+1)
#define _Keyu        		(_Keyt+1)
#define _Keyv        		(_Keyu+1)
#define _Keyw        		(_Keyv+1)
#define _Keyx        		(_Keyw+1)
#define _Keyy        		(_Keyx+1)
#define _Keyz        		(_Keyy+1)
#define _KeyBackSlash  		(_Keyz+1)
#define _KeySlash  		(_KeyBackSlash+1)
#define _KeyBraceRight		(_KeySlash+1)
#define _KeyBraceLeft		(_KeyBraceRight+1)
#define _KeyBracketRight	(_KeyBraceLeft+1)
#define _KeyBracketLeft		(_KeyBracketRight+1)

#define _Key1			(_KeyBracketLeft+1)
#define _Key2			(_Key1+1)
#define _Key3			(_Key2+1)
#define _Key4			(_Key3+1)
#define _Key5			(_Key4+1)
#define _Key6			(_Key5+1)
#define _Key7			(_Key6+1)
#define _Key8			(_Key7+1)
#define _Key9			(_Key8+1)
#define _Key0			(_Key9+1)

#define _MaxKey   	        (_Key0+1) 


#define NoModifierKeys 0
 
#define KeyNone		NoModifierKeys,		_KeyNone
#define KeyActivate	NoModifierKeys,		_KeyActivate
#define KeyAddMode	NoModifierKeys,		_KeyAddMode
#define KeyBackSpace	NoModifierKeys,		_KeyBackSpace
#define KeyBackTab	ShiftMask,		_KeyTab
#define KeyBeginData	ControlMask,		_KeyBeginLine
#define KeyBeginLine	NoModifierKeys,		_KeyBeginLine
#define KeyCancel	NoModifierKeys,		_KeyCancel
#define KeyClear	NoModifierKeys,		_KeyClear
#define KeyCopy		NoModifierKeys,		_KeyCopy
#define KeyCut		NoModifierKeys,		_KeyCut
#define KeyDelete	NoModifierKeys,		_KeyDelete
#define KeyDeselectAll	ControlMask,		_KeyBackSlash
#define KeyDown		NoModifierKeys,		_KeyDown
#define KeyEndData	ControlMask,		_KeyEndLine
#define KeyEndLine	NoModifierKeys,		_KeyEndLine
#define KeyEnter	NoModifierKeys,		_KeyEnter
#define KeyEscape	NoModifierKeys,		_KeyEscape
#define KeyExtend	ShiftMask,              _KeySelect
#define KeyHelp		NoModifierKeys,		_KeyHelp
#define KeyInsert	NoModifierKeys,		_KeyInsert
#define KeyLeft		NoModifierKeys,		_KeyLeft
#define KeyMenu		NoModifierKeys,		_KeyMenu
#define KeyMenuBar	NoModifierKeys,		_KeyMenuBar
#define KeyNextField	NoModifierKeys,		_KeyTab
#define KeyNextMenu	ControlMask,		_KeyDown
#define KeyPageDown	NoModifierKeys,		_KeyPageDown
#define KeyPageLeft	ControlMask,		_KeyPageUp
#define KeyPageRight	ControlMask,		_KeyPageDown
#define KeyPageUp	NoModifierKeys,		_KeyPageUp
#define KeyPaste	NoModifierKeys,		_KeyPaste
#define KeyPrevField	ShiftMask,		_KeyTab
#define KeyPrevMenu	ControlMask,		_KeyUp
#define KeyPrimaryCopy	ControlMask,	        _KeyPrimaryPaste
#define KeyPrimaryCut	Mod1Mask,               _KeyPrimaryPaste
#define KeyPrimaryPaste	NoModifierKeys,		_KeyPrimaryPaste
#define KeyQuickCopy	ControlMask,		_KeyQuickPaste
#define KeyQuickCut	Mod1Mask,		_KeyQuickPaste
#define KeyQuickExtend	ShiftMask,	        _KeyQuickPaste
#define KeyQuickPaste	NoModifierKeys,		_KeyQuickPaste
#define KeyReselect	ControlMask|ShiftMask,	_KeySelect
#define KeyRestore	ControlMask|ShiftMask,	_KeyInsert
#define KeyRight	NoModifierKeys,		_KeyRight
#define KeySelect	NoModifierKeys,		_KeySelect
#define KeySelectAll	ControlMask,		_KeySlash
#define KeySpace	NoModifierKeys,		_KeySpace
#define KeyTab		NoModifierKeys,		_KeyTab
#define KeyUndo		NoModifierKeys,		_KeyUndo
#define KeyUp		NoModifierKeys,		_KeyUp
#define KeyBackSlash	NoModifierKeys,		_KeyBackSlash
#define KeySlash	NoModifierKeys,		_KeySlash
#define KeyReturn	NoModifierKeys,		_KeyReturn
#define KeyBraceRight	NoModifierKeys,		_KeyBraceRight
#define KeyBraceLeft	NoModifierKeys,		_KeyBraceLeft
#define KeyBracketRight	NoModifierKeys,		_KeyBracketRight
#define KeyBracketLeft	NoModifierKeys,		_KeyBracketLeft
#define Key1		NoModifierKeys,		_Key1
#define Key2		NoModifierKeys,		_Key2
#define Key3		NoModifierKeys,		_Key3
#define Key4		NoModifierKeys,		_Key4
#define Key5		NoModifierKeys,		_Key5
#define Key6		NoModifierKeys,		_Key6
#define Key7		NoModifierKeys,		_Key7
#define Key8		NoModifierKeys,		_Key8
#define Key9		NoModifierKeys,		_Key9
#define Key0		NoModifierKeys,		_Key0

#define KeyExclamation	ShiftMask,		_Key1
#define KeyAt		ShiftMask,		_Key2
#define KeyNumberSign	ShiftMask,		_Key3
#define KeyDollar	ShiftMask,		_Key4
#define KeyPercent	ShiftMask,		_Key5
#define KeyCaret	ShiftMask,		_Key6
#define KeyAmpersand	ShiftMask,		_Key7
#define KeyAsterisk	ShiftMask,		_Key8
#define KeyParenLeft	ShiftMask,		_Key9
#define KeyParenRight	ShiftMask,		_Key0

#define Keya 	NoModifierKeys, 	_Keya
#define Keyb 	NoModifierKeys, 	_Keyb
#define Keyc 	NoModifierKeys, 	_Keyc
#define Keyd 	NoModifierKeys, 	_Keyd
#define Keye 	NoModifierKeys, 	_Keye
#define Keyf 	NoModifierKeys, 	_Keyf
#define Keyg 	NoModifierKeys, 	_Keyg
#define Keyh 	NoModifierKeys, 	_Keyh
#define Keyi 	NoModifierKeys, 	_Keyi
#define Keyj 	NoModifierKeys, 	_Keyj
#define Keyk 	NoModifierKeys, 	_Keyk
#define Keyl 	NoModifierKeys, 	_Keyl
#define Keym 	NoModifierKeys, 	_Keym
#define Keyn 	NoModifierKeys, 	_Keyn
#define Keyo 	NoModifierKeys, 	_Keyo
#define Keyp 	NoModifierKeys, 	_Keyp
#define Keyq 	NoModifierKeys, 	_Keyq
#define Keyr 	NoModifierKeys, 	_Keyr
#define Keys 	NoModifierKeys, 	_Keys
#define Keyt 	NoModifierKeys, 	_Keyt
#define Keyu 	NoModifierKeys, 	_Keyu
#define Keyv 	NoModifierKeys, 	_Keyv
#define Keyw 	NoModifierKeys, 	_Keyw
#define Keyx 	NoModifierKeys, 	_Keyx
#define Keyy 	NoModifierKeys, 	_Keyy
#define Keyz 	NoModifierKeys, 	_Keyz

#define Btn1   		NoModifierKeys, 	Button1
#define Btn2   		NoModifierKeys, 	Button2
#define Btn3   		NoModifierKeys, 	Button3
#define Btn4   		NoModifierKeys, 	Button4
#define Btn5   		NoModifierKeys, 	Button5

#define CtrlBtn1   	ControlMask, 		Button1
#define CtrlBtn2   	ControlMask, 		Button2
#define CtrlBtn3   	ControlMask, 		Button3
#define CtrlBtn4   	ControlMask, 		Button4
#define CtrlBtn5   	ControlMask, 		Button5

#define ShiftBtn1   	ShiftMask, 		Button1
#define ShiftBtn2   	ShiftMask, 		Button2
#define ShiftBtn3   	ShiftMask, 		Button3
#define ShiftBtn4   	ShiftMask, 		Button4
#define ShiftBtn5   	ShiftMask, 		Button5


/* Here is where we define the special combinations of shifted and 
   control keys. The cases here are enough the cover all of the 
   text and list widgets. 
*/


#define KeyShiftEndData      ShiftMask|ControlMask,         _KeyEndLine
#define KeyShiftBeginData    ShiftMask|ControlMask,         _KeyBeginLine
#define KeyShiftUp           ShiftMask,                     _KeyUp
#define KeyShiftDown         ShiftMask,                     _KeyDown
#define KeyShiftBeginLine    ShiftMask,                     _KeyBeginLine
#define KeyShiftEndLine      ShiftMask,                     _KeyEndLine
#define KeyShiftActivate     ShiftMask,                     _KeyActivate
#define KeyCtrlRight        ControlMask,                   _KeyRight
#define KeyCtrlLeft         ControlMask,                   _KeyLeft
#define KeyCtrlBeginLine    ControlMask,                   _KeyBeginLine
#define KeyCtrlEndLine      ControlMask,                   _KeyEndLine
#define KeyCtrlSlash        ControlMask,                   _KeySlash
#define KeyCtrlBackSlash    ControlMask,                   _KeyBackSlash
#define KeyCtrlDown         ControlMask,                   _KeyDown
#define KeyCtrlUp           ControlMask,                   _KeyUp
#define KeyCtrlSelect       ControlMask,                   _KeySelect
#define KeyShiftLeft         ShiftMask,                     _KeyLeft
#define KeyShiftPageDown     ShiftMask,                     _KeyPageDown
#define KeyShiftPageUp       ShiftMask,                     _KeyPageUp
#define KeyShiftRight        ShiftMask,                     _KeyRight
#define KeyShiftCtrlDown    ShiftMask|ControlMask,         _KeyDown
#define KeyShiftCtrlLeft    ShiftMask|ControlMask,         _KeyLeft
#define KeyShiftCtrlRight   ShiftMask|ControlMask,         _KeyRight
#define KeyShiftCtrlUp      ShiftMask|ControlMask,         _KeyUp








/* Key Symbol Names                                                      */
/* KeySyms are X's way of naming keys. Valid KeySym names are listed in  */
/* <X11/keysymdef.h> by removing the XK_ prefix from each name.          */

typedef struct {
    char      key_name[MAX_KEY_NAME];
    int       def_num;
    KeyCode   code;
    Modifiers modifier ;
}  xisKeySymStruct;

#ifndef DECLARE_XISLIB_GLOBALS_HERE
xisGLOBAL xisKeySymStruct xisKeySymName[];
#else
xisGLOBAL xisKeySymStruct xisKeySymName[_MaxKey+1] = {
 /* this next key must be this !!!  */
{/*KeyNone   	   */ "",		 _KeyNone,	0, 0	},
{/*KeyActivate     */ "osfActivate",     _KeyActivate,	0, 0	},
{/*KeyAddMode      */ "osfAddMode",      _KeyAddMode,	0, 0	},
{/*KeyBackSpace    */ "osfBackSpace",    _KeyBackSpace, 0, 0	},
{/*KeyBeginLine    */ "osfBeginLine",    _KeyBeginLine, 0, 0	},
{/*KeyCancel       */ "osfCancel",	 _KeyCancel,	0, 0	},
{/*KeyClear        */ "osfClear",	 _KeyClear,	0, 0	},
{/*KeyCopy         */ "osfCopy",	 _KeyCopy,	0, 0	},
{/*KeyCut          */ "osfCut", 	 _KeyCut,	0, 0	},
{/*KeyDelete       */ "osfDelete",	 _KeyDelete,	0, 0	},
{/*KeyDown         */ "osfDown",	 _KeyDown,	0, 0	},
{/*KeyEndLine      */ "osfEndLine",	 _KeyEndLine,	0, 0	},
{/*KeyEnter        */ "KP_Enter", 	 _KeyEnter,	0, 0	},
{/*KeyEscape       */ "Escape", 	 _KeyEscape,	0, 0	},
{/*KeyHelp         */ "osfHelp",	 _KeyHelp,	0, 0	},
{/*KeyInsert       */ "osfInsert",       _KeyInsert,	0, 0	},
{/*KeyLeft         */ "osfLeft", 	 _KeyLeft,	0, 0	},
{/*KeyMenu         */ "osfMenu",	 _KeyMenu,	0, 0	},
{/*KeyMenuBar      */ "osfMenuBar",      _KeyMenuBar,	0, 0	},
{/*KeyPageDown     */ "osfPageDown",	 _KeyPageDown,	0, 0	},
{/*KeyPageUp       */ "osfPageUp",	 _KeyPageUp,	0, 0	},
{/*KeyPaste        */ "osfPaste", 	 _KeyPaste,	0, 0	},
{/*KeyPrimaryPaste */ "osfPrimaryPaste", _KeyPrimaryPaste,0, 0	},
{/*KeyQuickPaste   */ "osfQuickPaste",   _KeyQuickPaste,0, 0	},
{/*KeyReturn       */ "Return", 	 _KeyReturn,	0, 0	},
{/*KeyRight        */ "osfRight", 	 _KeyRight,	0, 0	},
{/*KeySelect       */ "osfSelect", 	 _KeySelect,	0, 0	},
{/*KeySpace        */ "space", 	         _KeySpace,	0, 0	},
{/*KeyTab          */ "Tab", 	         _KeyTab,	0, 0	},
{/*KeyUndo         */ "osfUndo",	 _KeyUndo,	0, 0	},
{/*KeyUp           */ "osfUp", 	         _KeyUp	,	0, 0	},
{/*a          	   */ "a",      	 _Keya,	0, 0	},
{/*b        	   */ "b",      	 _Keyb,	0, 0	},
{/*c        	   */ "c",      	 _Keyc,	0, 0	},
{/*d        	   */ "d",      	 _Keyd,	0, 0	},
{/*e        	   */ "e",      	 _Keye,	0, 0	},
{/*f        	   */ "f",      	 _Keyf,	0, 0	},
{/*g        	   */ "g",      	 _Keyg,	0, 0	},
{/*h        	   */ "h",      	 _Keyh,	0, 0	},
{/*i        	   */ "i",      	 _Keyi,	0, 0	},
{/*j        	   */ "j",      	 _Keyj,	0, 0	},
{/*k        	   */ "k",      	 _Keyk,	0, 0	},
{/*l        	   */ "l",      	 _Keyl,	0, 0	},
{/*m        	   */ "m",      	 _Keym,	0, 0	},
{/*n        	   */ "n",      	 _Keyn,	0, 0	},
{/*o        	   */ "o",      	 _Keyo,	0, 0	},
{/*p        	   */ "p",      	 _Keyp,	0, 0	},
{/*q        	   */ "q",      	 _Keyq,	0, 0	},
{/*r        	   */ "r",      	 _Keyr,	0, 0	},
{/*s        	   */ "s",      	 _Keys,	0, 0	},
{/*t        	   */ "t",      	 _Keyt,	0, 0	},
{/*u        	   */ "u",      	 _Keyu,	0, 0	},
{/*v        	   */ "v",      	 _Keyv,	0, 0	},
{/*w        	   */ "w",      	 _Keyw,	0, 0	},
{/*x        	   */ "x",      	 _Keyx,	0, 0	},
{/*y        	   */ "y",      	 _Keyy,	0, 0	},
{/*z        	   */ "z",      	 _Keyz,	0, 0	},
{/*KeyBackSlash    */ "backslash",       _KeyBackSlash,	0, 0	},
{/*KeySlash        */ "slash", 	         _KeySlash,	0, 0	},
{/*KeyBraceRight   */ "braceright",	 _KeyBraceRight, 0, 0   },
{/*KeyBraceLeft    */ "braceleft",	 _KeyBraceLeft,  0, 0   },
{/*KeyBracketRight */ "bracketright",	 _KeyBracketRight, 0, 0   },
{/*KeyBracketLeft  */ "bracketleft",	 _KeyBracketLeft,  0, 0   },
{/* 1		   */ "1",		 _Key1, 0, 0 },
{/* 2		   */ "2",		 _Key2, 0, 0 },
{/* 3		   */ "3",		 _Key3, 0, 0 },
{/* 4		   */ "4",		 _Key4, 0, 0 },
{/* 5		   */ "5",		 _Key5, 0, 0 },
{/* 6		   */ "6",		 _Key6, 0, 0 },
{/* 7		   */ "7",		 _Key7, 0, 0 },
{/* 8		   */ "8",		 _Key8, 0, 0 },
{/* 9		   */ "9",		 _Key9, 0, 0 },
{/* 0		   */ "0",		 _Key0, 0, 0 },

 /* this next key must be this !!!  */
{/*_MaxKey   	   */ "UndefinedKey",    _MaxKey,	0, 0	},
};


#endif
#define KEYBOARD_FOCUS_CONTROLLED 0
#define KEYBOARD_FOCUS_NATURAL    1

xisGLOBAL short xisKeyboardFocusMode xisINIT(KEYBOARD_FOCUS_NATURAL);

#define POINTER_GRAB_CONTROLLED 0
#define POINTER_GRAB_NATURAL    1

xisGLOBAL short xisPointerGrabMode xisINIT(POINTER_GRAB_CONTROLLED);


/******************************************************************************
                WIDGET IMPLICIT STATE INFO MANAGEMENT DECLARATIONS
******************************************************************************/

/* XIS State Record contains  Current Relavent Physical X-Server States */

/* NOTE: None of these fields point to objects since the lifetime of any one */
/*       object is very short. They are regenerated after each call of       */
/*       xisProcessEvents.  So any particular object might be located at     */
/*       a different location after each call.                               */

typedef struct _XisStateRecord {
    Window       focus_window;          /* Only really needed for DummyFocus */
    Widget       focus_widget;          /* The latest known focus widget     */
    int          focus_object_type;     /* The latest focus object           */
    int          focus_instance;        /* The latest focus instance         */
    int          focus_revert_to;       /* The latest focus revert to field  */
    Widget       selected_widget;       /* Which widget,object_type,instance */
    int          selected_object_type;  /* ...that the pointer was over when */
    int          selected_instance;     /* ...the button was last pressed.   */
    unsigned int mod_button_state;      /* Which modkeys & buttons are down  */
} XisStateRecord;

xisGLOBAL XisStateRecord xisState;

/* Patch for List and Text tests on r3 server */

xisGLOBAL int xisUseCurrentTime xisINIT(0);

#include "xis_protos.h"

#endif /* xislib.h */
