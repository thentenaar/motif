/* $XConsortium: Navigation14.c /main/5 1995/07/15 21:11:44 drk $ */
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

#include <testlib.h>

/* forward declarations */

/* main program */
main (int argc, char ** argv)
{
   Arg args[10];
   int n;
   Widget parent, nestedtg1;
   Widget *pkids;
   Widget nkids[3];
   int use_scale;
   int i;
   XmString list_items[4];

   /* various static storage */

   static char arrow_transl[] = "<Key>osfUp:ManagerGadgetTraverseUp()\n\
<Key>osfDown:ManagerGadgetTraverseDown()\n\
<Key>osfLeft:ManagerGadgetTraverseLeft()\n\
<Key>osfRight:ManagerGadgetTraverseRight()";

   static String list_strings[] = {"Chuq", "Mikie", "Snickers", "Europa"};


   /* initialize Toolkit */
   CommonTestInit(argc, argv);

   /* malloc space for pkids */

   pkids = (Widget *) XtMalloc(sizeof(Widget) * 4);

   if ((UserData != NULL)  && (strcmp (UserData, "scale_child") == 0)) 
	use_scale = 1;
   else use_scale = 0;

   n = 0;
   XtSetArg (args[n], XmNheight, 300); n++;
   XtSetArg (args[n], XmNwidth, 300); n++;
   parent = XmCreateBulletinBoard (Shell1, "parent", args, n);
  
   n = 0;
   pkids[0] = XmCreatePushButton (parent, "DefaultButton", args, n); 
   XtAddCallback (pkids[0], XmNactivateCallback, CommonGenericCB, NULL);
  
   n = 0;
   XtSetArg (args[n], XmNy, 75); n++;
   XtSetArg (args[n], XmNx, 0); n++;
   XtSetArg (args[n], XmNorientation, XmVERTICAL); n++;
   XtSetArg (args[n], XmNnavigationType, XmNONE); n++; /* not STICKY */
   XtSetArg (args[n], XmNtraversalOn, True); n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg (args[n], XmNhighlightThickness, 2); n++;
   if (use_scale) 
   	pkids[1] = XmCreateScale (parent, "ScrollBar", args, n); 
   else
        pkids[1] = XmCreateScrollBar (parent, "ScrollBar", args, n); 

   XtOverrideTranslations (pkids[1], XtParseTranslationTable (arrow_transl));

   n = 0;
   XtSetArg (args[n], XmNx, 150); n++;
   XtSetArg (args[n], XmNy, 0); n++;
   pkids[2] = XmCreateRowColumn (parent, "RowCol", args, n);

   n = 0;
   XtSetArg (args[n], XmNx, 150); n++;
   XtSetArg (args[n], XmNy, 250); n++;
   XtSetArg (args[n], XmNnavigationType, XmNONE); n++; /* not TAB_GROUP */
   pkids[3] = XmCreateText (parent, "Text", args, n);

   /* create three children for the RowColumn */
   nkids[0] = XmCreateToggleButton (pkids[2], "Toggle1", args, 0);
   nkids[1] = XmCreateToggleButton (pkids[2], "Toggle2", args, 0);
   nkids[2] = XmCreateToggleButton (pkids[2], "Toggle3", args, 0);

   XtManageChildren (nkids, 3);
   XtManageChildren (pkids, 4);
   XtManageChild (parent);
   XtRealizeWidget (Shell1);

   CommonPause ();

   /* destroy the Scroll Bar or Scale and recreate a new, tab group one,
      which is not traversable  */

   XtDestroyWidget (pkids[1]);

   n = 0;
   XtSetArg (args[n], XmNy, 75); n++;
   XtSetArg (args[n], XmNx, 0); n++;
   XtSetArg (args[n], XmNorientation, XmVERTICAL); n++;
   XtSetArg (args[n], XmNtraversalOn, False); n++;
   XtSetArg (args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg (args[n], XmNhighlightThickness, 2); n++;
   if (use_scale) {
   	pkids[1] = XmCreateScale (parent, "ScrollBar", args, n); 
	XtRealizeWidget (pkids[1]);
   }
   else
        pkids[1] = XmCreateScrollBar (parent, "ScrollBar", args, n); 

   XtManageChild (pkids[1]);

   CommonPause();

   /* begin test for PIR 4705 */

   XmProcessTraversal(pkids[1], XmTRAVERSE_CURRENT);

   /* end test for PIR 4705 */

   CommonPause();

   /* make every pkid a tab group and traversable */

   n = 0;
   XtSetArg (args[n], XmNnavigationType, XmTAB_GROUP); n++;
   XtSetArg (args[n], XmNtraversalOn, True); n++;
   
   for (i = 0; i <= 3; i++)
     XtSetValues (pkids[i], args, n);

   CommonPause();

   /* make all pkids STICKY_TAB_GROUPS */
   /* the reason for the funky order is so that we end up with a traversal
      order reminiscent of counter-clockwise once the EXCLUSIVE_TAB_GROUP
      list is added in the next panel */
   
   n = 0;
   XtSetArg (args[n], XmNnavigationType, XmSTICKY_TAB_GROUP); n++;
   XtSetValues (pkids[0], args, n);
   XtSetValues (pkids[1], args, n);
   XtSetValues (pkids[3], args, n);
   XtSetValues (pkids[2], args, n);

   CommonPause();

   /* add an EXCLUSIVE_TAB_GROUP as a pkid sibling. */

   pkids = (Widget *) XtRealloc ((char *)pkids, (sizeof (Widget) * 5));

   /* set up list contents */

   for (i = 0; i <= 3; i++)
     list_items[i] = XmStringCreateLocalized (list_strings[i]);

   n = 0;
   XtSetArg (args[n], XmNitems, list_items); n++;
   XtSetArg (args[n], XmNitemCount, 4); n++;
   XtSetArg (args[n], XmNvisibleItemCount, 2); n++;
   XtSetArg (args[n], XmNx, 100); n++;
   XtSetArg (args[n], XmNy, 150); n++;
   XtSetArg (args[n], XmNnavigationType, XmEXCLUSIVE_TAB_GROUP); n++;
   XtSetArg (args[n], XmNselectionPolicy, XmMULTIPLE_SELECT); n++;
   pkids[4] = XmCreateScrolledList (parent, "List", args, n);
   XtManageChild (pkids[4]);

   CommonPause();

   /* restore the Text and Button pkids to XmNONE and change the 
      BB parent to an EXCLUSIVE_TAB_GROUP. Notably, do this
      setting out of child order. */
   /* also, set the ScrollBar/Scale and RowColumn back to default TAB_GROUPs*/

   n = 0;
   XtSetArg (args[n], XmNnavigationType, XmNONE); n++;
   XtSetValues (pkids[3], args, n); 
   XtSetValues (pkids[0], args, n);

   n = 0;
   XtSetArg (args[n], XmNnavigationType, XmTAB_GROUP); n++;
   XtSetValues (pkids[2], args, n);
   XtSetValues (pkids[1], args, n);

   n = 0;
   XtSetArg (args[n], XmNnavigationType, XmEXCLUSIVE_TAB_GROUP); n++;
   XtSetValues (parent, args, n);

   CommonPause();
   CommonPause(); /* exit */

   XtAppMainLoop (app_context);
}



