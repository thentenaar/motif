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
static char rcsid[] = "$XConsortium: BeginSession.c /main/7 1995/07/14 11:31:26 drk $"
#endif
#endif
/***********************************************************************
  	@(#)BeginSession.c	1.8	Date:03/04/91
  	Author: TAT
	History:
            05/24/90 SJS add to sccs
            08/30/90 SJS cosmetic changes, exit on error
            10/03/90 SJS make keycode initialization changes
	Calls:

	Summary:
            Initializes the input synthesis libraries local copies of display 
        and application context pointers.  It also sets up local-globals for
        the screen and the root_window, and establishes pointers to functions 
        which need to be defined external to this library.  This remote function
        referencing is necessary since this library has been designed to know
        nothing internally about the widget set which it interacts with.

	    One further addition is to do all the key code lookup *1* time.
	This is a change from looking the key codes up every time a keyclick
	needs to be done.

        INPUTS:
            display_in     -  the display that is connected to
            app_context_in -  the Xt application context
            num_objects    -  number of unique objects defined
            generate_objects_func -a function which updates the object hierarchy
            get_object_type_name_func - a function which returns the object
                                        type name
            get_widget_class_name_func - a function which returns widget class
                                         name
            trace_msg_func - a function which prints out a trace message
            warning_msg_func - a function which prints out a warning message
            error_msg_func - a function which prints out an error message
        
        OUTPUTS:
            none

        RETURNS:
            nothing
************************************************************************/
#define DECLARE_XISLIB_GLOBALS_HERE

#include <AutoMessages.h>
#include "xislib.h"
#include <Xm/XmP.h>		/* for XmKeyBinding */

#define XK_MISCELLANY

#include <X11/keysymdef.h>


void AutoGetAdditionalModifier();


void  xisBeginSession(display_in,app_context_in,num_object_types,
                      generate_objects_func,
                      get_object_type_name_func, get_widget_class_name_func,
                      window_to_widget_func,
                      trace_msg_func, warning_msg_func, error_msg_func)
Display *display_in;
XtAppContext app_context_in;
int num_object_types;
void (*generate_objects_func) ( /* XisObjectRecord *root_obj, Root object */ );

char *(*get_object_type_name_func) ( /* int object_type
                                        Object type code (user defined) */ );

char *(*get_widget_class_name_func) ( /* Widget widget
                                         Widget to find lowest class name*/ );

Widget (*window_to_widget_func) ( /* Window window
                                     Window to find widget for */ );

void (*trace_msg_func)();      /* These three funcs have params like printf */
void (*warning_msg_func)();
void (*error_msg_func)();
{
    static char routine_name[] = "xisBeginSession():";
    int i;
    char msg_string[125];

    /* Validate incoming parameters */

    xisTraceMsg = trace_msg_func;
    xisWarningMsg = warning_msg_func;
    xisErrorMsg = error_msg_func;

    if (error_msg_func == 0) {
        fprintf(stderr, "%s called with NULL error_msg_func\n", routine_name);
        exit(-1);
    }
    if (display_in == 0) {
        sprintf (msg_string, _AutoMessages[WARNMSG115], routine_name);
        AutoError(msg_string);
        
    }
    if (app_context_in == 0) {
        sprintf (msg_string, _AutoMessages[WARNMSG104], routine_name);
        AutoError(msg_string);
        
    }
    if (generate_objects_func == 0) {
        sprintf (msg_string, _AutoMessages[WARNMSG108], routine_name);
        AutoError(msg_string);
        
    }
    if (get_object_type_name_func == 0) {
        sprintf (msg_string, _AutoMessages[WARNMSG106], routine_name);
        AutoError(msg_string);

    }
    if (get_widget_class_name_func == 0) {
        sprintf (msg_string, _AutoMessages[WARNMSG110], routine_name);
        AutoError(msg_string);

    }
    if (window_to_widget_func == 0) {
        sprintf (msg_string, _AutoMessages[WARNMSG111], routine_name);
        AutoError(msg_string);
    }
    if (trace_msg_func == 0) {
        sprintf (msg_string, _AutoMessages[WARNMSG112], routine_name);
        AutoError(msg_string);
    }
    if (warning_msg_func == 0) {
        sprintf (msg_string, _AutoMessages[WARNMSG113], routine_name);
        AutoError(msg_string);
    }
    xisAppContext = app_context_in;
    xisDisplay = display_in;

    xisScreen = XDefaultScreen(xisDisplay);
    xisRootWindow = XDefaultRootWindow(xisDisplay);

    xisNumObjectTypes = num_object_types;

    xisGenerateObjects = generate_objects_func;
    xisGetObjectTypeName = get_object_type_name_func;
    xisGetWidgetClassName = get_widget_class_name_func;
    xisWindowToWidget = window_to_widget_func;

    /* Initialize the Root Object */
    xisObjects[0].in_use = 1;
    xisObjects[0].id.window = xisRootWindow;
    xisObjects[0].id.widget = NULL;
    xisObjects[0].id.object_type = 0; /* corresponds to oUserDefined */
    xisObjects[0].id.instance = 0;
    xisObjects[0].x = 0;
    xisObjects[0].y = 0;
    xisObjects[0].width = DisplayWidth(xisDisplay,xisScreen);
    xisObjects[0].height = DisplayHeight(xisDisplay,xisScreen);
    xisObjects[0].parent = NULL;
    xisObjects[0].first_child = NULL;
    xisObjects[0].next_sibling = NULL;
    xisObjects[0].proc_InformExpectedActions = NULL;

    xisMaxObjectNum++;           /* "Alloc" the object */


} /* end xisBeginSession */




static void 
CvtVirtualToActualKeysym( dpy, virtKeysym, actualKeysymRtn, modifiersRtn )
     Display *dpy ;
     KeySym virtKeysym ;
     KeySym *actualKeysymRtn ;
     Modifiers *modifiersRtn ;
{
  int num_keys;
  XmKeyBinding keys;

  /* Initialize the return parameters. */
  *actualKeysymRtn = NoSymbol;
  *modifiersRtn = 0;

  /* Arbitrarily return the first keysym in the list. */
  num_keys = XmeVirtualToActualKeysyms(dpy, virtKeysym, &keys);
  if (num_keys > 0)
    {
      *actualKeysymRtn = keys[0].keysym;
      *modifiersRtn = keys[0].modifiers;
    }
  XtFree((char*) keys);
}



void xisInitKeyCodes(sh_widget)
Widget sh_widget ;
{

    KeySym   tmp_key, tmp_key2 ;
    int i,j ;
    int x,y ;
    Boolean collision ;
    Modifiers	new_modifier;
    char msg_string[125];

    /* Initialize all keycodes declared */

    for(i=0; i< _MaxKey; i++) {
        tmp_key = XStringToKeysym(xisKeySymName[i].key_name) ;

        /* if it is an osf key then translate it again */
        if ( strncmp(xisKeySymName[i].key_name,"osf",3) == 0 )
            CvtVirtualToActualKeysym(xisDisplay,tmp_key,&tmp_key2,
				     &xisKeySymName[i].modifier) ;
        else /* just copy it over */
           tmp_key2 = tmp_key ;

        xisKeySymName[i].code = XKeysymToKeycode(xisDisplay,tmp_key2) ;
	/*
	 * The following two lines added for automation.
	 */
	AutoGetAdditionalModifier(xisDisplay, tmp_key2, &new_modifier);
	xisKeySymName[i].modifier |= new_modifier;

        (*xisTraceMsg)(" keycode number %2d: %s -> %d -> %d/%d --> %d \n",
               i,xisKeySymName[i].key_name,tmp_key,tmp_key2,
                      xisKeySymName[i].modifier,xisKeySymName[i].code) ;
  
       
        /* a key is undefined */
        if (( i != 0 ) && ( i != (_MaxKey-1) ))  {
           if (( tmp_key == NoSymbol ) || ( tmp_key2 == NoSymbol )) {
	     sprintf (msg_string,_AutoMessages[WARNMSG114],
		      xisKeySymName[i].key_name) ;
             AutoMessage (msg_string);

	   }   
         }
      }
    
         /* look for duplicate key bindings */

        collision = False ;
        for(i=1;i<(_MaxKey-1);i++) {
           for(j=i;j<(_MaxKey-1);j++) {
              if ( i != j ) {
                 x = (int)xisKeySymName[i].code ;
                 y = (int)xisKeySymName[j].code ;
                 if ( x == y ) {
                    collision = True ;
		  }
	       }
	    }
	 }

} /* end xisInitKeyCodes */


/*
 *	The following routine has been added to get the group modifier when
 *	needed. If the key modmap is arranged in more than two columns
 *	(i.e. when the no. of keysyms_per_keycode is greater than two), to
 *	send the key event corresponding to any keysym in the third or
 *	fourth column of the modmap, it is necessary to add the group modifier
 *	to the current modifier switch in order for the server to interpret
 *	the event correctly. This group modifier is available through the
 *	variable 'mode_switch' in the display, which gets initialized when
 *	the internal modmap for X is getting initialized. (InitModMap() in
 *	XKeyBind.c).
 */
void AutoGetAdditionalModifier(dpy, ks, new_mod)
Display		*dpy;
KeySym		ks;
Modifiers	*new_mod;
{
    register int i, j, n;
    Boolean	done = False;
    KeySym	ksym, *ks_list;
    XModifierKeymap *mod_keymap;
    int		min_keycode_rtn, max_keycode_rtn ;
    int		keysyms_per_keycode_rtn ;
    int		keycode_count ;
    unsigned int  mode_switch;

    *new_mod = 0;

/*
 * Fix for CR 5663 -- Use function interface to retrieve
 * publicly accessible Display info.
 */
    XDisplayKeycodes( dpy, &min_keycode_rtn, &max_keycode_rtn ) ;
    keycode_count = max_keycode_rtn - min_keycode_rtn + 1 ;
    if (!( ks_list =
	   XGetKeyboardMapping( dpy, min_keycode_rtn, keycode_count,
				&keysyms_per_keycode_rtn ) ) )
		return; 

/*
 * Fix for CR 5663 -- Since X11R5 made the display mode_switch
 * variable opaque, we will have to regenerate it here, using
 * the same algorithm as in XKeyBind.c.
 */

    mod_keymap = XGetModifierMapping( dpy );
    /* Find any Mod<n> modifier acting as the Group modifier */
    mode_switch = 0;
    n = 8 * mod_keymap->max_keypermod;
    for (i = 3*mod_keymap->max_keypermod; i < n; i++) {
        for (j = 0; j < keysyms_per_keycode_rtn; j++) {
            ksym = XKeycodeToKeysym(dpy, mod_keymap->modifiermap[i], j);
            if (ksym == XK_Mode_switch)
                mode_switch |= 1 << (i / mod_keymap->max_keypermod);
        }
    }

    for (i = min_keycode_rtn; i <= max_keycode_rtn && !done; i++) {

	for (j = 0; j < keysyms_per_keycode_rtn; j++) {
	    	if (XKeycodeToKeysym(dpy, (KeyCode) i, j) == ks) {
			if (j > 1)
				*new_mod = mode_switch;
			done = True;
			break;
		}
	}

    }

    XFree( ks_list ) ;
    XFreeModifiermap ( mod_keymap );
    return;

}
