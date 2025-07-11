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
<<<<<<< 1.6.8.2
static char rcsid[] = "$XConsortium: OptionMenu4.c /main/10 1995/07/13 18:50:16 drk $"
======
static char rcsid[] = "$XConsortium: OptionMenu4.c /main/10 1995/07/13 18:50:16 drk $"
>>>>>>> 1.6.7.2
#endif
#endif


#include <testlib.h>

#define   NUM_RC_CHILDREN 9

Widget    RC;
Widget    RC_children[NUM_RC_CHILDREN];
static char *RC_children_names[] = { 
  "Himalyan",
  "Siamese",
  "D. Shorthair",
  "Maine Cat",
  "Burmese",
  "E. Shorthair",
  "Turkish Van",
  "Persian",
  "Russian Blue"
  };

Widget RC2, RC2pd, RC2b1, RC2b2, RC2b3, OM1, OM2, OM3;
Widget RC3, RC3pd, RC3pb1, RC3pb2, RC3pb3,RC3om;
Widget PushBtn1, PushBtn2, PushBtn3;
Widget SharedMenu, Form;
Widget mb1, om1, popupshell, itm1, itm2, itm3;
XmString  cmpLabel;

Widget AddMenuItem(Widget menu, char *itemName)
{
  Arg arg[10];
  int n;
  Widget menuItem;
  XmString tempString;

  n = 0;
  tempString = XmStringCreateLtoR(itemName, XmSTRING_DEFAULT_CHARSET);
  XtSetArg(arg[n], XmNlabelString, (XtArgVal) tempString);   n++;
  XtSetArg(arg[n], XmNsensitive, (XtArgVal) TRUE); n++;
  menuItem = XmCreatePushButtonGadget(menu, itemName, arg, n);
  XmStringFree(tempString);
  return(menuItem);
} /* AddMenuItem */

int main(int argc, char **argv)
{
  register int i,n;
  Arg args[MAX_ARGS];
  XmString tcs;
  char buf[32];

  CommonTestInit(argc, argv);

#ifdef MOTIF1_1
  n = 0;
  XtSetArg(args[n], XmNwidth, 10);	n++;
  XtSetArg(args[n], XmNheight, 10);	n++;
  XtSetArg(args[n], XmNallowShellResize, True); n++;
  XtSetValues(Shell1, args, n);
#endif /* MOTIF1_1 */
    
  n = 0;
  XtSetArg(args[n], XtNgeometry, "+0+350");  n++;
  XtSetValues(Shell1, args, n);
  
  /* 
   *  Test: nine Option Menu children of a Form. 
   */

  n = 0;
  RC = XmCreateForm (Shell1, "RC", args, n);
  XtManageChild (RC);

  /* 
   *  Put the nine cascadebutton children of RC into an array 
   */

  for (i = 0; i <= (NUM_RC_CHILDREN - 1); i++)
    {  
      n = 0;
      tcs = XmStringCreate(RC_children_names[i], XmSTRING_DEFAULT_CHARSET);
      sprintf (buf, "OptionMenu%d", i);
      /* this resource off for bug Pxxxx */
      XtSetArg(args[n], XmNnavigationType, XmNONE); n++;
      XtSetArg(args[n], XmNlabelString, tcs); n++;
      RC_children[i] = XmCreateOptionMenu(RC, buf, args, n);
      XmStringFree(tcs);

      /*
       *  Substituting Cascade Button children in MENU_BAR gets rid
       *  of all warnings
       */
      /*         RC_children[i] = XmCreateCascadeButton(RC, buf, args, n); */

      XtManageChild(RC_children[i]);
    }

  /*
   * Create the shared menu pane for children 0, 1 and 5.
   * First, put together the widget list for subMenuId.
   */

  n = 0;
  XtSetArg(args[n], XmNrowColumnType, XmMENU_PULLDOWN); n++; 
  SharedMenu = XmCreatePulldownMenu(RC, "SharedMenu", args, n);

  n = 0;
  tcs = XmStringCreate("Seal Point" , XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  PushBtn1 = XmCreatePushButton(SharedMenu, "PushBtn1", args, 1);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringCreate("Lynx Point", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  PushBtn2 = XmCreatePushButton(SharedMenu, "PushBtn2", args, n);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringCreate("Chocolate Point", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  PushBtn3 = XmCreatePushButton(SharedMenu, "PushBtn3", args, n);
  XmStringFree(tcs);

  /*
   *  Manage the push buttons
   */

  XtManageChild(PushBtn1); 
  XtManageChild(PushBtn2); 
  XtManageChild(PushBtn3);

  n = 0;
  XtSetArg(args[n], XmNsubMenuId, SharedMenu); n++;
  XtSetValues(RC_children[0], args, n);
  XtSetValues(RC_children[1], args, n);
  XtSetValues(RC_children[5], args, n);

  /*  
   *  Set Form constraints.
   *  For Children 0,3,6 set top attachment ATTACH_FORM
   *  For all others, set top attachment to widget [i-1]
   */

  for (i = 0; i<=8; i++)
    {  
      n = 0;
      if (i%3)
	{ 
	  XtSetArg(args[n], XmNtopWidget, RC_children[i - 1]); n++;
	  XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	  XtSetValues(RC_children[i], args, n);
	}
      else
	{ 
	  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	  XtSetValues (RC_children[i], args, n);
	}
    }
    
  /* 
   *  Set children 0,1 and 2 left-attach Form
   */

  for (i = 0; i<=2; i++)
    { 
      n = 0;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
      XtSetValues(RC_children[i], args, n);
    }

  /* 
   *  Set all except above to left-attach, widget i - 3 
   */

  for (i=3; i <= 8; i++)
    { 
      n = 0;
      XtSetArg(args[n], XmNleftWidget, RC_children[i - 3]); n++;
      XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
      XtSetValues(RC_children[i], args, n);
    }

  XtManageChild(RC);
  XtRealizeWidget(Shell1);
  CommonPause();

  /*
   *  Test 2: Verify different OptionMenu XmNmenuHistory settings
   *  for 3 OptionMenus sharing the same pulldown...
   */
  
  XtUnmanageChild(RC);

  /*
   *  Create a rowColumn
   */

  RC2 = XmCreateRowColumn(Shell1, "RC2", NULL, 0);

  RC2pd = XmCreatePulldownMenu(RC2, "RC2pd", NULL, 0);

  XtManageChild(RC2b1 = XmCreatePushButton(RC2pd, "Button1", NULL, 0));
  XtManageChild(RC2b2 = XmCreatePushButton(RC2pd, "Button2", NULL, 0));
  XtManageChild(RC2b3 = XmCreatePushButton(RC2pd, "Button3", NULL, 0));

  XtSetArg(args[0], XmNsubMenuId, RC2pd);
  XtSetArg(args[1], XmNmenuHistory, RC2b1);
  XtManageChild(XmCreateOptionMenu(RC2, "option1", args, 2));

  XtSetArg(args[1], XmNmenuHistory, RC2b3);
  XtManageChild(XmCreateOptionMenu(RC2, "option2", args, 2));

  XtSetArg(args[1], XmNmenuHistory, RC2b2); 
  XtManageChild(XmCreateOptionMenu(RC2, "option3", args, 2));

  XtManageChild(RC2);

  CommonPause();

  /*
   *  Test 3: Verify that when an OptionMenu is the child of
   *   a BulletinBoard, it can initialize its XmNmenuHistory
   *   correctly.
   */

  XtUnmanageChild(RC2);

  n=0;
  XtSetArg(args[n], XmNwidth, 600); n++;
  XtSetArg(args[n], XmNheight, 400); n++;
  RC3 = XmCreateRowColumn(Shell1, "RC3", args, n);

  RC3pd = XmCreatePulldownMenu(RC3, "RC3pd", args, 0);

  n=0;
  tcs = XmStringLtoRCreate("Item1", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n], XmNmarginLeft, 4); n++;
  XtSetArg(args[n],  XmNmarginRight, 4); n++;
  RC3pb1 = XmCreatePushButton(RC3pd, "PLBPB1", args, n);
  XmStringFree(tcs);

  /*
   * Create pushbutton 2
   */

  n = 0;
  tcs = XmStringLtoRCreate("A long item", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs); n++;
  XtSetArg(args[n],  XmNmarginLeft, 4); n++;
  XtSetArg(args[n],  XmNmarginRight,  4); n++;
  RC3pb2 = XmCreatePushButton(RC3pd, "PLBPB2", args, n);	
  XmStringFree(tcs);

  /*
   * Create pushbutton 3
   */

  n = 0;
  tcs = XmStringLtoRCreate("Item3", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString,  tcs); n++;
  XtSetArg(args[n], XmNmarginLeft, 4); n++;
  XtSetArg(args[n], XmNmarginRight, 4); n++;

  RC3pb3 = XmCreatePushButton(RC3pd, "PLBPB3", args, n);    	
  XmStringFree(tcs);

  /*
   * Create the option menu
   */

  n=0;
  XtSetArg(args[n], XmNsubMenuId,  RC3pd); n++;
  XtSetArg(args[n], XmNx, 50); n++;
  XtSetArg(args[n], XmNy, 10); n++;
  XtSetArg(args[n], XmNborderWidth, 0); n++;
  XtSetArg(args[n], XmNmenuHistory, RC3pb2); n++;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  XtSetArg(args[n], XmNrowColumnType, XmMENU_OPTION); n++;

  RC3om = XmCreateOptionMenu(RC3, "RC3OM", args, n);  

  XtManageChild(RC3pb1);
  XtManageChild(RC3pb2);
  XtManageChild(RC3pb3);
  XtManageChild(RC3om);
  XtManageChild(RC3);

  CommonPause();

  XtUnmanageChild(RC3);

  /*
   *  Test 4.  Create an option menu.  Then change the contents of
   *   the default selected item.  It should be updated correctly.
   */

  n = 0;

  /*
   * Motif 1.1 has a bug where if the shell gets realized with no
   * children, an error occurs because the shell thinks it has no
   * width and height.  In Motif 1.2 this is supposed to get fixed.
   * BTW - this only surfaces during automation where widgets are 
   * managed as they are created.
   */

#ifdef MOTIF1_1
  XtSetArg(args[n], XmNwidth, 10); n++;
  XtSetArg(args[n], XmNheight, 10); n++;
#endif

  XtSetArg(args[n], XmNallowShellResize, True); n++;
  popupshell = XtCreatePopupShell("pshell",
				  applicationShellWidgetClass,
				  Shell1,
				  args,
				  n);

  om1 = XmCreatePulldownMenu(popupshell, "pulld", NULL, 0);
  n = 0;
  cmpLabel = XmStringCreate("ButtonLabel:", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, cmpLabel); n++;
  XtSetArg(args[n], XmNsubMenuId, om1); n++;
  mb1 = XmCreateOptionMenu(popupshell, "mb1", args, n);
  XmStringFree(cmpLabel);
  XtManageChild(mb1);

  itm1 = AddMenuItem(om1, "Item1");
  XtManageChild(itm1);

  itm2 = AddMenuItem(om1, "Item2");
  XtManageChild(itm2);

  itm3 = AddMenuItem(om1, "Item3");
  XtManageChild(itm3);

  XtPopup(popupshell, XtGrabNone);

  cmpLabel = XmStringCreate("new long Text", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[0], XmNlabelString, cmpLabel);
  XtSetValues(itm1, args, 1);
  XmStringFree(cmpLabel);
  
/* 
 * The below commented code resets menuHistory to another item, 
 *  and when setting it back to the original item, it then shows
 *  "new long Text" as the current item.
 */

/*   XtSetArg(args[0], XmNmenuHistory, NULL);
   XtSetValues(mb1, args, 1);
*/

  XtSetArg(args[0], XmNmenuHistory, itm1);
  XtSetValues(mb1, args, 1);

  CommonPause();

  XtAppMainLoop(app_context);

} /*  End main */

