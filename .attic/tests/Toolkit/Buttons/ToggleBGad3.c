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
static char rcsid[] = "$XConsortium: ToggleBGad3.c /main/8 1995/07/13 17:31:30 drk $"
#endif
#endif


 

/****************************************
*   Automatic test program generation   *
*       Fri Mar 17 14:54:45 1989        *
****************************************/

#include <testlib.h>
#include <X11/StringDefs.h>
#include "smiley.bmp"
#include "circles1.bmp"

#define XBMs_BITS smiley_bits
#define XBMs_WIDTH smiley_width
#define XBMs_HEIGHT smiley_height
#define XBMu_BITS circles1_bits
#define XBMu_WIDTH circles1_width
#define XBMu_HEIGHT circles1_height

Pixmap px_select;
Pixmap px_unselect;

Widget Form = NULL, RowColumn1 = NULL, RowColumn2 = NULL;
Widget ToggleButtonG1 = NULL, ToggleButtonG2 = NULL;
Widget ToggleButtonG3 = NULL, ToggleButtonG4 = NULL;
Widget ToggleButtonG5 = NULL, ToggleButtonG6 = NULL;

Boolean mywait;

void toggleGCB();

static XtCallbackRec 
  valuechangedCR[] =  {{ toggleGCB, (caddr_t)NULL}, {NULL, NULL},},
  armCR[] = {{ toggleGCB, (caddr_t)NULL}, {NULL, NULL},},
  disarmCR[] = {{ toggleGCB, (caddr_t)NULL}, {NULL, NULL},};

void toggleGCB(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
  printf("ToggleG Callback Structure:\n    Reason:   ");
  switch( ((XmToggleButtonCallbackStruct*)call_data)->reason) {
  case XmCR_VALUE_CHANGED: 
    printf("XmCR_VALUE_CHANGED\n"); 
    break;
  case XmCR_ARM: 		
    printf("XmCR_ARM\n"); 
    break;
  case XmCR_DISARM: 	
    printf("XmCR_DISARM\n"); 
    break;
  default: 		
    printf("Oops - unknown callback reason!\n"); 
    break;
  }
}
void  main(argc, argv)
int argc;
char **argv;
{
  int n;
  Arg args[20];
  XWindowAttributes info;
  int delay = 0;
  XmString tcs;
  XFontStruct *newfont;
  XmFontList newfontlist;

  CommonTestInit(argc, argv);
    
  n = 0;
  XtSetArg(args[n], XmNwidth, 400);  n++;
  XtSetArg(args[n], XmNheight,300);  n++;
  XtSetValues(Shell1, args, n);
   
  XmRegisterConverters();

  XtRealizeWidget(Shell1);

  mywait = True;
  while (mywait) {
    XGetWindowAttributes(XtDisplay(Shell1), XtWindow(Shell1), &info);
    if (info.map_state == IsViewable) mywait = False;
    else sleep(1);
  }

  newfont = XLoadQueryFont(display, "variable");
  newfontlist = XmFontListCreate(newfont, "var_char_set");

  n = 0;
  Form = XmCreateForm(Shell1, "Form", args, n);
  XtManageChild(Form);

  n = 0;
  XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
  XtSetArg (args[n], XmNbackground, CommonGetColor ("lightGray")); n++;
  RowColumn1 = XmCreateRadioBox(Form, "RowColumn1", args, n);
  XtManageChild (RowColumn1);

  n = 0;
  tcs = XmStringLtoRCreate("ToggleG1", "var_char_set");
  XtSetArg(args[n], XmNfontList, newfontlist); n++;
  XtSetArg(args[n], XmNlabelString, tcs);  n++;
  XtSetArg(args[n], XmNmnemonic, 'g'); n++;
  XtSetArg(args[n], XmNmnemonicCharSet, "var_char_set"); n++;
  XtSetArg(args[n], XmNindicatorOn, True);  n++;
  XtSetArg(args[n], XmNvalueChangedCallback, valuechangedCR);  n++;
  XtSetArg(args[n], XmNarmCallback, armCR);  n++;
  XtSetArg(args[n], XmNdisarmCallback, disarmCR);  n++;
  ToggleButtonG1 = XmCreateToggleButtonGadget(RowColumn1, "ToggleButtonG1",
					      args, n);
  if(tcs) XmStringFree(tcs);
  XtManageChild(ToggleButtonG1);

  n = 0;
  XtSetArg(args[n], XmNindicatorType, XmONE_OF_MANY);  n++;
  XtSetArg(args[n], XmNfillOnSelect, False);  n++;
  ToggleButtonG2 = XmCreateToggleButtonGadget(RowColumn1, "ToggleButtonG2",
					      args, n);
  XtManageChild(ToggleButtonG2);

  n = 0;
  XtSetArg(args[n], XmNvisibleWhenOff, False);  n++;
  ToggleButtonG3 = XmCreateToggleButtonGadget(RowColumn1, "ToggleButtonG3",
					      args, n);
  XtManageChild(ToggleButtonG3);

  n = 0;
  XtSetArg(args[n], XmNset, True);  n++;
  XtSetArg(args[n], XmNfillOnSelect, True);  n++;
  ToggleButtonG4 = XmCreateToggleButtonGadget(RowColumn1, "ToggleButtonG4",
					      args, n);
  XtManageChild(ToggleButtonG4);

  px_unselect =
    XCreatePixmapFromBitmapData(display, 
				DefaultRootWindow(display),
				XBMu_BITS, XBMu_WIDTH,XBMu_HEIGHT,
				WhitePixel(display,DefaultScreen(display)),
				BlackPixel(display,DefaultScreen(display)),
				DefaultDepth(display,DefaultScreen(display)));
  px_select = 
    XCreatePixmapFromBitmapData(display, 
				DefaultRootWindow(display),
				XBMs_BITS, XBMs_WIDTH,XBMs_HEIGHT,
				WhitePixel(display,DefaultScreen(display)),
				BlackPixel(display,DefaultScreen(display)),
				DefaultDepth(display,DefaultScreen(display)));
  n = 0;
  XtSetArg(args[n], XmNlabelType, XmPIXMAP);  n++;
  XtSetArg(args[n], XmNselectPixmap, px_select);  n++;
  XtSetArg(args[n], XmNlabelPixmap, px_unselect);  n++;
  ToggleButtonG5 = XmCreateToggleButtonGadget(RowColumn1, "ToggleButtonG5",
					      args, n);
  XtManageChild(ToggleButtonG5);

  n = 0;
  XtSetArg(args[n], XmNindicatorOn, False);  n++;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  ToggleButtonG6 = XmCreateToggleButtonGadget(RowColumn1, "ToggleButtonG6",
					      args, n);
  XtManageChild(ToggleButtonG6);

  n = 0;
  tcs = XmStringLtoRCreate("ToggleG2", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs);  n++;
  XtSetValues(ToggleButtonG2, args, n);
  if(tcs) XmStringFree(tcs);

  printf("ToggleG1 state = %d\n", 
	 XmToggleButtonGadgetGetState(ToggleButtonG1));
  XmToggleButtonGadgetSetState(ToggleButtonG1, TRUE, FALSE);

  printf("ToggleG1 state = %d\n", 
	 XmToggleButtonGadgetGetState(ToggleButtonG1));
  XmToggleButtonGadgetSetState(ToggleButtonG1, FALSE, TRUE);

  printf("ToggleG1 state = %d\n", 
	 XmToggleButtonGadgetGetState(ToggleButtonG1));

  CommonPause();                       /* ONE_OF_MANY */

  XtSetArg(args[n], XmNradioBehavior, False); n++;
  XtSetArg(args[n], XmNradioAlwaysOne, False); n++;
  XtSetValues(RowColumn1, args, n);
  CommonPause();			   /* N_OF_MANY Behavior */

  n = 0;
  XtSetArg(args[n], XmNindicatorType, XmN_OF_MANY); n++;
  XtSetValues(ToggleButtonG1, args, n);
  XtSetValues(ToggleButtonG2, args, n);
  CommonPause();			   /* N_OF_MANY Appearance */

  /*
   *  Kill off the widgets
   */

  XtDestroyWidget(ToggleButtonG1);
  XtDestroyWidget(ToggleButtonG2);
  XtDestroyWidget(ToggleButtonG3);
  XtDestroyWidget(ToggleButtonG4);
  XtDestroyWidget(ToggleButtonG5);
  XtDestroyWidget(ToggleButtonG6);
  XtDestroyWidget(RowColumn1);
  
  /*
   *  Create a test for PIR 2430
   */

  n = 0;
  XtSetArg(args[n], XmNpacking, XmPACK_TIGHT); n++;
  XtSetArg(args[n], XmNbackground, CommonGetColor("black")); n++;
  XtSetArg(args[n], XmNforeground, CommonGetColor("orange")); n++;
  RowColumn1 = XmCreateRadioBox(Form, "RowColumn1", args, n);
  XtManageChild (RowColumn1);

  XtSetArg(args[n], XmNx, 125); n++;   /* add an offset */
  RowColumn2 = XmCreateRadioBox(Form, "RowColumn2", args, n);
  XtManageChild (RowColumn2);
  
  n = 0;
  XtSetArg(args[n], XmNshadowThickness, 2); n++;
  XtSetArg(args[n], XmNindicatorOn, True);  n++;
  ToggleButtonG1 = XmCreateToggleButtonGadget(RowColumn1, "ToggleButtonG1",
					      args, n);
  ToggleButtonG4 = XmCreateToggleButtonGadget(RowColumn2, "ToggleButtonG4",
					      args, n);
  ToggleButtonG2 = XmCreateToggleButtonGadget(RowColumn1, "ToggleButtonG2",
					      args, n);
  ToggleButtonG5 = XmCreateToggleButtonGadget(RowColumn2, "ToggleButtonG5",
					      args, n);
  ToggleButtonG3 = XmCreateToggleButtonGadget(RowColumn1, "ToggleButtonG3",
					      args, n);
  ToggleButtonG6 = XmCreateToggleButtonGadget(RowColumn2, "ToggleButtonG6",
					      args, n);
  XtManageChild(ToggleButtonG1);
  XtManageChild(ToggleButtonG4);
  XtManageChild(ToggleButtonG2);
  XtManageChild(ToggleButtonG5);
  XtManageChild(ToggleButtonG3);
  XtManageChild(ToggleButtonG6);
  
  /* Now change the colors of the second radiobox */

  n = 0;
  XtSetArg(args[n], XmNforeground, CommonGetColor("blue")); n++;
  XtSetArg(args[n], XmNbackground, CommonGetColor("magenta")); n++;
  XtSetValues(RowColumn2, args, n);

  /*  End test for PIR 2430 */

  CommonPause();
  XtDestroyWidget(Form);

  CommonPause();			/* exit */
  XtAppMainLoop(app_context);

}

