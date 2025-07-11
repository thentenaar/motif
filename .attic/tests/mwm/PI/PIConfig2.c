/* $XConsortium: PIConfig2.c /main/5 1995/07/15 21:13:57 drk $ */
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

#include <X11/Xlib.h>
#include <testlib.h>


/* test for CR 5217 */
void
main (int argc, char **argv)
{
   Display    *mydisplay;
   int        erster_aufruf=1; 
   Window     win_fix_pos, win_toggle;
   XEvent     event;
   XSizeHints hint;

   CommonTestInit (argc,argv);


   
   win_fix_pos = XCreateSimpleWindow (display, DefaultRootWindow(display),
                           50, 50, 200, 200, 0, 0, 1);

   win_toggle = XCreateSimpleWindow (display, DefaultRootWindow(display),
                           500, 500, 200, 200, 0, 0, 0);
 
   hint.flags = (USPosition | USSize | PMinSize | PMaxSize |
                 PResizeInc | PBaseSize | PAspect);

   hint.x = 50;
   hint.y = 50;
   hint.width  = 200; 
   hint.height = 200;
   hint.max_width  = 200;
   hint.max_height = 200;
   hint.min_width  = 200;
   hint.min_height = 200;
   hint.width_inc  = 1;
   hint.height_inc = 1;
   hint.base_width = hint.min_width;
   hint.base_height = hint.min_height;

   hint.min_aspect.x = hint.max_width;
   hint.min_aspect.y = hint.max_height;
   hint.max_aspect.x = hint.min_width;
   hint.max_aspect.y = hint.min_height;

   XSetStandardProperties (display, win_fix_pos, "win_fix_pos", "win_fix_pos",   None, None, None, &hint);

   hint.max_width  = 400;
   hint.max_height = 400;
   hint.x = 500;
   hint.y = 500;

   XSetStandardProperties( display, win_toggle, "win_toggle", "win_toggle",
                           None, None, None, &hint);

   XSelectInput (display, win_toggle, StructureNotifyMask);
   XMapWindow (display, win_toggle);
   XMapWindow (display, win_fix_pos);

   CommonPause();


   if (erster_aufruf == 1)
     {
       XMoveWindow (display, win_toggle, 50, 50);
       printf ("\nMove after Maximize was executed\n");
       erster_aufruf=0;
     }
    


CommonPause();

XtAppMainLoop(app_context);
}


