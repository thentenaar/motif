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

/*   $XConsortium: ToggleBtn5.c /main/5 1995/07/13 17:33:10 drk $  */

#include <testlib.h>
#include <X11/Xresource.h>
#include <X11/StringDefs.h>

void toggleCB1();
void toggleCB2();
void toggleCB3();
void toggleCB4();
void do_change();

static XtCallbackRec 
  disarmCR1[] = {{ toggleCB1, NULL}, {NULL, NULL},};

static XtCallbackRec
  disarmCR2[] = {{ toggleCB2, NULL}, {NULL, NULL},};

static XtCallbackRec
  disarmCR3[] = {{ toggleCB3, NULL}, {NULL, NULL},};

static XtCallbackRec
  disarmCR4[] = {{ toggleCB4, NULL}, {NULL, NULL},};


Arg      args[MAX_ARGS];
Cardinal n;

Widget	 bb;

Widget   ToggleButton1,
         ToggleButton2,
         ToggleButton3,
         ToggleButton4;

Widget   PushButton1,
         Label1,
         PushButtonMove1,
         PushButtonMove2,
         PushButtonMove3;
         

static void SetSpacing(spacing)
Dimension spacing;
{
  printf("change spacing to %d.\n", spacing);

  n = 0;
  XtSetArg(args[n], XmNspacing, spacing); n++;
  XtSetValues(ToggleButton1, args, n);
  XtSetValues(ToggleButton2, args, n);
  XtSetValues(ToggleButton3, args, n);
  XtSetValues(ToggleButton4, args, n);
}

static void GetSpacing()
{
  Dimension spacing;

  n = 0;
  XtSetArg(args[n], XmNspacing, &spacing); n++;
  XtGetValues(ToggleButton1, args, n);
  printf("ToggleButton1 spacing : %d.\n", spacing);
  XtGetValues(ToggleButton2, args, n);
  printf("ToggleButton2 spacing : %d.\n", spacing);
  XtGetValues(ToggleButton3, args, n);
  printf("ToggleButton3 spacing : %d.\n", spacing);
  XtGetValues(ToggleButton4, args, n);
  printf("ToggleButton4 spacing : %d.\n\n", spacing);
}

void toggleCB1(w, client_data, call_data)
Widget w;
caddr_t  client_data;
caddr_t  call_data;
{
  switch(((XmToggleButtonCallbackStruct*)call_data)->reason)
    {
    case XmCR_DISARM: 
      SetSpacing(50);
      GetSpacing();
      break;
    default:
      printf("Oops - unknown callback reason!\n");
      exit(0);
    }
}

void toggleCB2(w, client_data, call_data)
Widget w;
caddr_t  client_data;
caddr_t  call_data;
{
  switch(((XmToggleButtonCallbackStruct*)call_data)->reason)
    {
    case XmCR_DISARM:
      SetSpacing(5);
      GetSpacing();
      break;
    defaults:
      printf("Oops - unknown callback reason!\n");
      exit(0);
    }
}

void toggleCB3(w, client_data, call_data)
Widget w;
caddr_t  client_data;
caddr_t  call_data;
{
  switch(((XmToggleButtonCallbackStruct*)call_data)->reason)
    {
    case XmCR_DISARM:
      SetSpacing(100);
      GetSpacing();
      break;
    defaults:
      printf("Oops - unknown callback reason!\n");
      exit(0);
    }
}

void toggleCB4(w, client_data, call_data)
Widget w;
caddr_t  client_data;
caddr_t  call_data;
{
  switch(((XmToggleButtonCallbackStruct*)call_data)->reason)
    {
    case XmCR_DISARM:
      SetSpacing(75);
      GetSpacing();
      break;
    defaults:
      printf("Oops - unknown callback reason!\n");
      exit(0);
    }
}

void do_change(w, client_data, call_data)
Widget w;
Widget client_data;
caddr_t call_data;
{
  Arg args[2];
  int n = 0;
	
  /* to test y */
  XtSetArg(args[n], XmNy, 240); n++;
  XtSetValues(client_data, args, n);
}


void
main(unsigned int argc, char **argv)
{
  XmString str;

  CommonTestInit(argc, argv);

  n = 0;
  bb = XmCreateBulletinBoard(Shell1, "bb", args, n);
  
  /* Begin test for CR 5303 */

  n = 0;
  XtSetArg(args[n], XmNindicatorOn, True);  n++;
  XtSetArg(args[n], XmNy, 5); n++;
  XtSetArg(args[n], XmNdisarmCallback, disarmCR1);  n++;
  ToggleButton1 = XmCreateToggleButton(bb, "ToggleButton1", args, n);

  n = 0;
  XtSetArg(args[n], XmNindicatorOn, True);  n++;
  XtSetArg(args[n], XmNy, 40); n++;
  XtSetArg(args[n], XmNdisarmCallback, disarmCR2);  n++;
  ToggleButton2 = XmCreateToggleButton(bb, "ToggleButton2", args, n);

  n = 0;
  XtSetArg(args[n], XmNindicatorOn, True);  n++;
  XtSetArg(args[n], XmNy, 75); n++;
  XtSetArg(args[n], XmNdisarmCallback, disarmCR3);  n++;
  ToggleButton3 = XmCreateToggleButton(bb, "ToggleButton3", args, n);

  n = 0;
  XtSetArg(args[n], XmNindicatorOn, True);  n++;
  XtSetArg(args[n], XmNy, 110); n++;
  XtSetArg(args[n], XmNdisarmCallback, disarmCR4);  n++;
  ToggleButton4 = XmCreateToggleButton(bb, "ToggleButton4", args, n);

  XtManageChild(bb);
  XtManageChild(ToggleButton1);
  XtManageChild(ToggleButton2);
  XtManageChild(ToggleButton3);
  XtManageChild(ToggleButton4);

  XtRealizeWidget(Shell1);

  /* End test for CR 5303 */

  CommonPause();

  XtDestroyWidget(ToggleButton1);
  XtDestroyWidget(ToggleButton2);
  XtDestroyWidget(ToggleButton3);
  XtDestroyWidget(ToggleButton4);

  /* Begin test for CR 5163 */

  n = 0;
  XtSetArg(args[n], XmNwidth, 400); n++;
  XtSetArg(args[n], XmNheight, 400); n++;
  XtSetArg(args[n], XmNresizePolicy, XmRESIZE_NONE); n++;
  XtSetValues(bb, args, n);

  n = 0;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 10); n++;
  XtSetArg(args[n], XmNwidth, 100); n++;
  XtSetArg(args[n], XmNborderWidth, 1); n++;
  XtSetArg(args[n], XmNheight, 100); n++;
  ToggleButton1 = XmCreateToggleButton(bb, "ToggleButton1", args, n);

  n = 0;
  XtSetArg(args[n], XmNx, 150); n++;
  XtSetArg(args[n], XmNy, 10); n++;
  XtSetArg(args[n], XmNwidth, 100); n++;
  XtSetArg(args[n], XmNheight, 100); n++;
  XtSetArg(args[n], XmNborderWidth, 1); n++;
  PushButton1 = XmCreatePushButton(bb, "PushButton1", args, n);

  n = 0;
  XtSetArg(args[n], XmNx, 280); n++;
  XtSetArg(args[n], XmNy, 10); n++;
  XtSetArg(args[n], XmNwidth, 100); n++;
  XtSetArg(args[n], XmNheight, 100); n++;
  XtSetArg(args[n], XmNborderWidth, 1); n++;
  Label1 = XmCreateLabel(bb, "Label1", args, n);

  str = XmStringCreateLtoR("Move TglBtn1", XmSTRING_DEFAULT_CHARSET); 
  n = 0;
  XtSetArg(args[n], XmNx, 10); n++;
  XtSetArg(args[n], XmNy, 150); n++;
  XtSetArg(args[n], XmNwidth, 100); n++;
  XtSetArg(args[n], XmNheight, 50); n++;
  XtSetArg(args[n], XmNlabelString, str); n++;
  PushButtonMove1 = XmCreatePushButton(bb, "PushButtonMove1", args, n);
  XtAddCallback(PushButtonMove1, XmNactivateCallback, do_change, ToggleButton1);

  XmStringFree(str);
  str = XmStringCreateLtoR("Move PushBtn1", XmSTRING_DEFAULT_CHARSET); 
  n = 0;
  XtSetArg(args[n], XmNx, 150); n++;
  XtSetArg(args[n], XmNy, 150); n++;
  XtSetArg(args[n], XmNwidth, 100); n++;
  XtSetArg(args[n], XmNheight, 50); n++;
  XtSetArg(args[n], XmNlabelString, str); n++;
  PushButtonMove2 = XmCreatePushButton(bb, "PushButtonMove2", args, n);
  XtAddCallback(PushButtonMove2, XmNactivateCallback, do_change, PushButton1);

  XmStringFree(str);
  str = XmStringCreateLtoR("Move Lbl1", XmSTRING_DEFAULT_CHARSET); 
  n = 0;
  XtSetArg(args[n], XmNx, 280); n++;
  XtSetArg(args[n], XmNy, 150); n++;
  XtSetArg(args[n], XmNwidth, 100); n++;
  XtSetArg(args[n], XmNheight, 50); n++;
  XtSetArg(args[n], XmNlabelString, str); n++;
  PushButtonMove3 = XmCreatePushButton(bb, "PushButtonMove3", args, n);
  XtAddCallback(PushButtonMove3, XmNactivateCallback, do_change, Label1);
  XmStringFree(str);

  XtManageChild(ToggleButton1);
  XtManageChild(PushButton1);
  XtManageChild(Label1);
  XtManageChild(PushButtonMove1);
  XtManageChild(PushButtonMove2);
  XtManageChild(PushButtonMove3);

  /* End test for CR 5163 */

  CommonPause();
  
  XtDestroyWidget(bb);

  CommonPause();

  XtAppMainLoop(app_context);

}





