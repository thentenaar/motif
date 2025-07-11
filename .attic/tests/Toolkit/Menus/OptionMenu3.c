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
static char rcsid[] = "$XConsortium: OptionMenu3.c /main/8 1995/07/13 18:49:46 drk $"
#endif
#endif


#include <testlib.h>
#define NUM_RC_CHILDREN 9

Widget RC;
Widget RC_children[NUM_RC_CHILDREN];

static char *RC_children_names[] = { 
  "Himalyan",  "Siamese",
  "Domestic Shorthair",  "Maine Coon Cat",
  "Burmese",  "Exotic Shorthair",
  "Turkish Van",  "Persian",  "Russian Blue"
  };

Widget PushBtn1, PushBtn2, PushBtn3;
Widget SharedMenu;

void changeSize(Widget w, XtPointer cd, XtPointer cs)
{
  int cdi = (int) cd ;
  Dimension width, height;
  for (width = height = 5; cdi; cdi--)
    {
      width = width * 10;
      height = height * 5;
    }
  XtVaSetValues(w, XmNwidth, width, XmNheight, height, NULL);
}

void FromWhere(Widget w, XtPointer client_data, XtPointer call_data)
{
  Arg args[1];
  Widget whichWidget;
  XFontStruct 	*fnt;
  XmFontList 	font1;

  whichWidget = XmGetPostedFromWidget ((Widget) client_data);

  XtSetArg(args[0], XmNshadowThickness, 4); 
  XtSetValues ((XmOptionButtonGadget(whichWidget)), args, 1);

  fnt = XLoadQueryFont(XtDisplay(Shell1), "8x13bold");
  font1 = XmFontListCreate(fnt, XmSTRING_DEFAULT_CHARSET);

  XtSetArg (args[0], XmNfontList, font1);
  XtSetValues ((XmOptionLabelGadget(whichWidget)), args, 1);

}


/*********** Callback for the Pushbuttons ***********************/

void ButtonCB (w, client_data, call_data) 
Widget	w;		/*  widget id		*/
XtPointer	client_data;	/*  data from application   */
XtPointer	call_data;	/*  data from widget class  */
{
   /*  print message and terminate program  */
   printf ("Option %s selected.\n", client_data);
}

int main(argc, argv)
int argc;
char **argv;
{
    register int i,n;
    Arg args[MAX_ARGS];
    XmString tcs;
    Widget pointsList[3];
    char buf[32];
    Widget pulldown1, pulldown2, option_menus[2], options1[3], options2[3];
    Widget regularPane, child1, child2, child3;
    Widget SharedMenu2, temp_widget;
    Widget option, menu, pb[3], items[4];
    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg(args[n], XmNwidth,  200);  n++;
    XtSetArg(args[n], XmNheight, 40);   n++;
    XtSetArg(args[n], XtNgeometry, "+0+300");  n++;
    XtSetArg(args[n], XmNallowShellResize, True); n++;
    XtSetValues(Shell1, args, n);

    /*
    ** on panel 1, create a 3x3 RowColumn that contains
    ** 9 Option Menus.
    ** Just to complicate things, we'll use some shared menupanes
    */

    n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 3); n++; 
    XtSetArg(args[n], XmNadjustLast, False); n++;
    XtSetArg(args[n], XmNrowColumnType, XmWORK_AREA); n++;

    /*  Using a MENU_BAR Row Column produces somewhat fewer warnings */
    /*    XtSetArg(args[n], XmNrowColumnType, XmMENU_BAR); n++;      */

    RC = XmCreateRowColumn(Shell1, "RC", args, n);
    XtManageChild(RC);

    /* create a regular menupane for another button */
    n = 0;
    regularPane = XmCreatePulldownMenu(RC, "regularPane", args, n);
    
    n = 0;
    tcs = XmStringCreateSimple("Black");
    XtSetArg (args[n], XmNlabelString,  tcs); n++;
    child1 = XmCreatePushButton(regularPane, "child1", args, n);
    XmStringFree(tcs);

    n = 0;
    tcs =  XmStringCreateSimple("Grey Tabby");
    XtSetArg (args[n], XmNlabelString, tcs); n++;
    child2 = XmCreatePushButton(regularPane, "child2", args, n);
    XmStringFree(tcs);

    n = 0;
    tcs = XmStringCreateSimple("Calico");
    XtSetArg (args[0], XmNlabelString, tcs); n++;
    child3 = XmCreatePushButton(regularPane, "child3", args, n);
    XtManageChild (child1); XtManageChild (child2); XtManageChild (child3);
    XmStringFree(tcs);


    /*     XtSetValues (RC_children[2], args, 1); */


    /* put the nine cascadebutton children of RC into an array */

    for (i = 0; i <= (NUM_RC_CHILDREN - 1); i++)
      {  
         n = 0;
         sprintf (buf, "OptionMenu%d", i);
	 tcs = XmStringCreate(RC_children_names[i],
			      XmSTRING_DEFAULT_CHARSET);
	 XtSetArg(args[n], XmNnavigationType, XmNONE); n++;
         XtSetArg(args[n], XmNlabelString, tcs); n++;
	 /* for debug: why arrows not zits? */
	 if (i == 2)  XtSetArg (args[0], XmNsubMenuId, regularPane);
         RC_children[i] = XmCreateOptionMenu(RC, buf, args, n);

/* substituting Cascade Button children in MENU_BAR gets rid of all warnings*/
/*         RC_children[i] = XmCreateCascadeButton(parent, buf, args, n); */

         XtManageChild(RC_children[i]);
	 XmStringFree(tcs);
      }

    /* create the shared menu pane for children 0, 1 and 5 */
    /* first, put together the widget list for subMenuId */

    n = 0;
    XtSetArg(args[n], XmNrowColumnType, XmMENU_PULLDOWN); n++; 
    SharedMenu = XmCreatePulldownMenu(RC, "SharedMenu", args, n);

    n = 0;
    tcs = XmStringCreate("Seal Point", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    PushBtn1 = XmCreatePushButton(SharedMenu, "PushBtn1", args, n);
    XtAddCallback(PushBtn1, XmNactivateCallback, FromWhere, (XtPointer) SharedMenu);
    XmStringFree(tcs);

    n = 0;
    tcs = XmStringCreate ("Lynx Point", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    PushBtn2 = XmCreatePushButton(SharedMenu, "PushBtn2", args, n);
    XtAddCallback(PushBtn2, XmNactivateCallback, FromWhere, (XtPointer) SharedMenu);
    XmStringFree(tcs);

    n = 0;
    tcs = XmStringCreate("Chocolate Point", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    PushBtn3 = XmCreatePushButton(SharedMenu, "PushBtn3", args, n);
    XtAddCallback(PushBtn3, XmNactivateCallback, FromWhere, (XtPointer) SharedMenu);
    XtManageChild(PushBtn1); XtManageChild(PushBtn2); XtManageChild(PushBtn3);
    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, SharedMenu); n++;
    XtSetValues(RC_children[0], args, n);
    XtSetValues(RC_children[1], args, n);
    XtSetValues(RC_children[5], args, n);

    /*
     *  Begin test for PIRs 2505, 2997, 2691
     */

    n = 0;
    XtSetArg(args[n], XmNrowColumnType, XmMENU_PULLDOWN); n++; 
    SharedMenu2 = XmCreatePulldownMenu(RC, "SharedMenu", args, n);

    tcs = XmStringCreate("Chocolate Point", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[0], XmNlabelString, tcs); 
    XtManageChild(XmCreatePushButton(SharedMenu2, "PushBtn3", args, 1));
    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, SharedMenu2); n++;
    XtSetValues(RC_children[0], args, n);
    XtSetValues(RC_children[1], args, n);
    XtSetValues(RC_children[5], args, n);

    /*
     *  End test for PIRs 2505, 2997, 2691
     */

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, SharedMenu); n++;
    XtSetValues(RC_children[0], args, n);
    XtSetValues(RC_children[1], args, n);
    XtSetValues(RC_children[5], args, n);
    XtRealizeWidget(Shell1);
    CommonPause();

    /*  Begin Test For PIR 3049 */
    
    XtDestroyWidget(RC);

    RC = XmCreateRowColumn(Shell1, "rc", NULL, 0);
    XtManageChild(RC);

    menu = XmCreatePulldownMenu(RC, "menu", NULL, 0);
    items[0] = XmCreatePushButton(menu, "item0", NULL, 0);
    items[1] = XmCreatePushButton(menu, "item1", NULL, 0);
    items[2] = XmCreatePushButton(menu, "item2", NULL, 0);
    items[3] = XmCreatePushButton(menu, "item3", NULL, 0);
    XtManageChildren(items, XtNumber(items));

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, menu); n++;
    XtSetArg(args[n], XmNmenuHistory, items[0]); n++;
    tcs = XmStringCreate("OPTION", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtManageChild(option = XmCreateOptionMenu(RC, "option", args, n));
    XmStringFree(tcs);

    pb[0] = XmCreatePushButton(RC, "SMALL", NULL, 0);
    pb[1] = XmCreatePushButton(RC, "LARGE", NULL, 0);
    pb[2] = XmCreatePushButton(RC, "LARGER", NULL, 0);
    XtAddCallback(pb[0], XmNactivateCallback, changeSize, (XtPointer) 0);
    XtAddCallback(pb[1], XmNactivateCallback, changeSize, (XtPointer) 1);
    XtAddCallback(pb[2], XmNactivateCallback, changeSize, (XtPointer) 2);
    XtManageChildren(pb, XtNumber(pb));

    /*  End test For PIR 3049 */

    CommonPause();
 
    /*  Begin test for PIR 2732 */

    XtDestroyWidget(RC);

    n = 0;
    XtSetArg(args[n], XmNnumColumns, 2); n++;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    RC = XmCreateRowColumn(Shell1, "rc", args, n);
    XtManageChild(RC);

    /* Create two pulldown menus in rc */

    pulldown1 = (Widget)XmCreatePulldownMenu(RC, "pulldown1", NULL, 0);

    n = 0;
    tcs = XmStringCreate("A-option",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNmnemonic, 'A'); n++;
    options1[0] = XmCreatePushButtonGadget(pulldown1, "option1a", args, n);
    XtAddCallback(options1[0], XmNactivateCallback, ButtonCB, (XtPointer) "1A");
    XmStringFree(tcs);

    n = 0;
    tcs = XmStringCreate("B-option",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNmnemonic, 'B'); n++;
    options1[1] = XmCreatePushButtonGadget(pulldown1, "option1b", args, n);
    XtAddCallback(options1[1], XmNactivateCallback, ButtonCB, (XtPointer) "1B");
    XmStringFree(tcs);

    n = 0;
    tcs = XmStringCreate("C-option",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNmnemonic, 'C'); n++;
    options1[2] = XmCreatePushButtonGadget(pulldown1, "option1c", args, n);
    XtAddCallback(options1[2], XmNactivateCallback, ButtonCB, (XtPointer) "1C");
    XtManageChildren(options1, 3);
    XmStringFree(tcs);

    pulldown2 = (Widget)XmCreatePulldownMenu(RC, "pulldown2", NULL, 0);

    n = 0;
    tcs = XmStringCreate("A-option",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs); n++; 
    XtSetArg(args[n], XmNmnemonic, 'A'); n++;
    options2[0] = XmCreatePushButtonGadget(pulldown2, "option2a", args, n);
    XtAddCallback(options2[0], XmNactivateCallback, ButtonCB, (XtPointer) "2A");
    XmStringFree(tcs);

    n = 0;
    tcs = XmStringCreate("B-option",XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNmnemonic, 'B'); n++;
    options2[1] = XmCreatePushButtonGadget(pulldown2, "option2b", args, n);
    XtAddCallback(options2[1], XmNactivateCallback, ButtonCB, (XtPointer) "2B");
    XmStringFree(tcs);

    n = 0;
    tcs = XmStringCreate("C-option", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNmnemonic, 'C'); n++;
    options2[2] = XmCreatePushButtonGadget(pulldown2, "option2c", args, n);
    XtAddCallback(options2[2], XmNactivateCallback, ButtonCB, (XtPointer) "2C");
    XtManageChildren(options2, 3);
    XmStringFree(tcs);

    /* Create option menus and attach the two pulldown menus */

    n = 0;
    tcs = XmStringCreate("First Option Set", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNmnemonic, 'F'); n++;
    XtSetArg(args[n], XmNsubMenuId, pulldown1); n++;
    XtSetArg(args[n], XmNmenuHistory, options1[2]); n++;
    XtSetArg(args[n], XmNheight, 100); n++;
    option_menus[0] = XmCreateOptionMenu(RC, "option_menu1", args, n);
    XmStringFree(tcs);

    n = 0;
    tcs = XmStringCreate("Second Option Set", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNmnemonic, 'S'); n++;
    XtSetArg(args[n], XmNsubMenuId, pulldown2); n++;
    XtSetArg(args[n], XmNmenuHistory, options2[0]); n++;
    XtSetArg(args[n], XmNheight, 100); n++;
    option_menus[1] = XmCreateOptionMenu(RC, "option_menu2", args, n);
    XtManageChildren(option_menus, 2);
    XmStringFree(tcs);

    CommonPause();

    temp_widget = XmOptionButtonGadget(option_menus[0]);
    XtSetArg(args[0], XmNheight, 80);
    XtSetValues(temp_widget, args, 1);
    temp_widget = XmOptionLabelGadget(option_menus[0]);
    XtSetValues(temp_widget, args, 1);

    CommonPause();

    XtAppMainLoop(app_context);
}














