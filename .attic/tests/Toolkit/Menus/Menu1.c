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
static char rcsid[] = "$XConsortium: Menu1.c /main/12 1996/07/26 16:55:18 jakku $"
#endif
#endif


#include <testlib.h>
#include <X11/Xatom.h>
#include <X11/StringDefs.h>

/*  Global Variables  */
Widget    MenuBar1, PullDown1, PullDown2, PullDown3, MenuBtn1;
Widget    MenuBtn2, MenuBtn3, Label1, Separator1, PushBtn1;
Widget    Separator3, Separator4, MenuBtn4, PullDown4;
Widget    Menu4, PullDown4, Menu5, PullDown5;
Widget    ToggleBtn1, PullDown1A, CascadeBtn1, Label2;
Widget    Separator2, ToggleBtn2, PullDown2A, CascadeBtn2;
Widget    PushBtn2, Label3, ToggleBtn3, PullDown3A, CascadeBtn3;
Widget    PushBtn3, Label1A, Separator1A, PushBtn1A, PushBtn1B;
Widget    PushBtn1C, Label2A, Separator2A, ToggleBtn2A;
Widget    ToggleBtn2B, ToggleBtn2C, Label3A, Separator3A;
Widget    PullDown3AA, PullDown3BA, PullDown3CA, CascadeBtn3A;
Widget    CascadeBtn3B, CascadeBtn3C, Label3AA, Separator3AA;
Widget    PushBtn3AA, Label3BA, Separator3BA, ToggleBtn3BA;
Widget    Label3CA, Separator3CA, PullDown3CAA, CascadeBtn3CA;
Widget    Label3CAA, tearOffWidget;
Widget	  MenuBtn6, PullDown6 = 0, Separator6, PushBtn6, Label6;
Widget	  MenuBtn7, PullDown7 = 0, Separator7, PushBtn7, Label7;


typedef struct cbdatastruct
{
    char* title;
    Widget label;
} CBDataStruct;

void MapCB(Widget w, XtPointer closure, XtPointer call_data)
{
    Arg args[1];
    XmString str;
    static unsigned long ctr = 0;
    char s[80];

    ctr++;

    printf("Widget %s mapped: %d\n", (char*)closure, ctr);

    sprintf(s, "%s: %d", (char*)closure, ctr);

    /* First set the tear-off title to the string passed in. */
    str = XmStringCreateSimple(s);

    XtSetArg(args[0], XmNtearOffTitle, str);
    XtSetValues(w, args, 1);
    XmStringFree(str);
}

void UnmapCB(Widget w, XtPointer closure, XtPointer call_data)
{
  printf("Widget Menu%d unmapped\n", (int) closure);
}

#ifndef MOTIF1_1

void tearOffActivate(Widget w, XtPointer closure, XtPointer call_data)
{
    Arg args[1];
    Widget label;
    XmString str;
    String s;

    label = (Widget)closure;

    /* Read the label from the border - was set via tearOffTitle resource. */
    XtSetArg(args[0], XmNtitle, &s);
    /* The Parent of the tear-off is a WmShell. */
    XtGetValues(XtParent(w), args, 1);

    printf("%s activated\n", s);

    /* The first child of the input widget is a label.  Set the
       label's labelString to the text read from the border. */
    str = XmStringCreateSimple(s);
    XtSetArg(args[0], XmNlabelString, str);
    XtSetValues(label, args, 1);
    XmStringFree(str);
}

void tearOffDeactivate(Widget w, XtPointer closure, XtPointer call_data)
{
  printf("Tear off deactivated\n");
}

static XtCallbackRec toDCallback[] = 
{
  {tearOffDeactivate, (XtPointer) NULL},
  {NULL, (XtPointer) NULL}
};

#endif /* MOTIF1_1 */

static Widget CreateLabel(char *label, Widget parent)
{
  Widget widget;
  register int n;
  Arg args[MAX_ARGS];
  XmString tcs;

  n = 0;
  tcs = XmStringLtoRCreate(label, XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs);  n++;
  widget = XmCreateLabel(parent, label, args, n);
  XtManageChild(widget);
  XmStringFree(tcs);

  return(widget);
}

static Widget CreatePushButton(char *label, KeySym mnemonic, Widget parent)
{
  Widget widget;
  register int n;
  Arg args[MAX_ARGS];
  XmString tcs;

  n = 0;
  tcs = XmStringLtoRCreate(label, XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs);  n++;
  XtSetArg(args[n], XmNmnemonic, mnemonic);  n++;
  widget = XmCreatePushButton(parent, label, args, n);
  XtManageChild(widget);
  XmStringFree(tcs);

  return(widget);
}

static Widget CreateToggle(char *label, KeySym mnemonic, Widget parent)
{
  Widget widget;
  register int n;
  Arg args[MAX_ARGS];
  XmString tcs;

  n = 0;
  tcs = XmStringLtoRCreate(label, XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs);  n++;
  XtSetArg(args[n], XmNmnemonic, mnemonic);  n++;
  widget = XmCreateToggleButton(parent, label, args, n);
  XtManageChild(widget);
  XmStringFree(tcs);

  return(widget);
}

static Widget CreateSeparator(Widget parent, char *name)
{
  Widget widget;
  widget = XmCreateSeparator(parent, name, NULL, 0);
  XtManageChild(widget);
  return(widget);
}

static Widget CreatePullDown(Widget parent, char *name)
{
  Widget widget;
  widget = XmCreatePulldownMenu (parent, name, NULL, 0);
  return(widget);
}

static Widget CreateCascade(char *label, KeySym mnemonic, Widget submenu,
			    Widget parent)
{
  Widget widget;
  register int  n;
  Arg args[MAX_ARGS];
  XmString tcs;

  n = 0;
  tcs = XmStringLtoRCreate(label, XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[n], XmNlabelString, tcs);  n++;
  XtSetArg(args[n], XmNmnemonic,  mnemonic);  n++;
  if ((int)submenu != 0)
    {
      XtSetArg(args[n], XmNsubMenuId, submenu); 
      n++;
    }
  widget = XmCreateCascadeButton(parent, label, args,n);
  XtManageChild(widget);
  XmStringFree(tcs);
  return(widget);
}

void CascadeCB(Widget w, XtPointer closure, XtPointer call_data)
{
  register int n;
  Arg args[MAX_ARGS];

  if (PullDown6 == NULL)
    {
      printf("Cascading Callback, add a menu\n");
      n = 0;
      PullDown6 = XmCreatePulldownMenu (MenuBar1, "pulldown6", args, n);
      Label6 = CreateLabel("Menu6", PullDown6);
      Separator6    = CreateSeparator(PullDown6, "Separator6");
      PushBtn6      = CreatePushButton("pushbutton6", 'p', PullDown6);

      n = 0;
      XtSetArg(args[n], XmNsubMenuId, PullDown6); n++;
      XtSetValues(w, args, n);
      XtManageChild(PullDown6);
    }
}

void ActCB(Widget w, XtPointer closure, XtPointer call_data)
{
  register int n;
  Arg args[MAX_ARGS];

  if (PullDown7 == NULL)
    {
      printf("Activate Callback, add a menu\n");
      n = 0;
      PullDown7 = XmCreatePulldownMenu (MenuBar1, "pulldown7", args, n);
      Label7 = CreateLabel("Menu7", PullDown7);
      Separator7    = CreateSeparator(PullDown7, "Separator7");
      PushBtn7      = CreatePushButton("pushbutton7", 'p', PullDown7);

      n = 0;
      XtSetArg(args[n], XmNsubMenuId, PullDown7); n++;
      XtSetValues(w, args, n);
      XtManageChild(PullDown7);
    }
}

int main(argc, argv)
int argc;
char **argv;
{
  register int  n;
  Arg args[MAX_ARGS];
  static CBDataStruct PD1Data, PD2Data, PD3Data, PD1AData;

  CommonTestInit(argc, argv);

  /*
   * MenuBar1
   */

  MenuBar1 = XmCreateMenuBar(Shell1, "MenuBar1", NULL, 0);
  XtManageChild(MenuBar1);

  /*
   *  Create a pulldown menu that can be torn off
   */

  n = 0;
#ifndef MOTIF1_1
  XtSetArg(args[n], XmNtearOffModel, XmTEAR_OFF_ENABLED); n++;
  XtSetArg(args[n], XmNtearOffMenuDeactivateCallback, toDCallback); n++;
#endif
  PullDown1 = XmCreatePulldownMenu (MenuBar1, "pulldown1", args, n);
/*  XtManageChild(PullDown1);*/
 
  XtAddCallback(PullDown1, XmNmapCallback, MapCB, (XtPointer) "TearOff #1");
  XtAddCallback(PullDown1, XmNunmapCallback, UnmapCB, (XtPointer) 1);

  MenuBtn1 = CreateCascade("MenuBtn1", '1', PullDown1, MenuBar1);

  n = 0;
#ifndef MOTIF1_1
  XtSetArg(args[n], XmNtearOffModel, XmTEAR_OFF_ENABLED); n++;
  XtSetArg(args[n], XmNtearOffMenuDeactivateCallback, toDCallback); n++;
#endif
  PullDown2 = XmCreatePulldownMenu (MenuBar1, "pulldown2", args, n);

  XtAddCallback(PullDown2, XmNmapCallback, MapCB, (XtPointer)"TearOff #2");
  XtAddCallback(PullDown2, XmNunmapCallback, UnmapCB, (XtPointer) 2);

  MenuBtn2 = CreateCascade("MenuBtn2", '2', PullDown2, MenuBar1);

  n = 0;
#ifndef MOTIF1_1
  XtSetArg(args[n], XmNtearOffModel, XmTEAR_OFF_ENABLED); n++;
  XtSetArg(args[n], XmNtearOffMenuDeactivateCallback, toDCallback); n++;
#endif
  PullDown3 = XmCreatePulldownMenu (MenuBar1, "pulldown3", args, n);

  XtAddCallback(PullDown3, XmNmapCallback, MapCB, (XtPointer)"TearOff #3");
  XtAddCallback(PullDown3, XmNunmapCallback, UnmapCB, (XtPointer) 3);

  MenuBtn3 = CreateCascade("MenuBtn3", '3', PullDown3, MenuBar1);

  XtRealizeWidget(Shell1);
  
#ifndef MOTIF1_1
  
  /*
   * Check to see if XmGetTearOffControl can get the widget id of 
   *  the tear off control
   */

  tearOffWidget = XmGetTearOffControl(PullDown1); 

#endif

  /*
   * Menu1
   */

  Label1 = CreateLabel("Menu1Label", PullDown1);
#ifndef MOTIF1_1
  XtAddCallback(PullDown1, XmNtearOffMenuActivateCallback, tearOffActivate,
		(XtPointer)Label1);
#endif
  Separator1    = CreateSeparator(PullDown1, "Separator1");
  PushBtn1      = CreatePushButton("pushbutton1", 'p', PullDown1);
  ToggleBtn1    = CreateToggle("toggle1", 't', PullDown1);

  n = 0;
#ifndef MOTIF1_1
  XtSetArg(args[n], XmNtearOffModel, XmTEAR_OFF_ENABLED); n++;
  XtSetArg(args[n], XmNtearOffMenuDeactivateCallback, toDCallback); n++;
#endif
  PullDown1A    = XmCreatePulldownMenu(PullDown1, "PullDown1A", NULL, 0);

  XtAddCallback(PullDown1A, XmNmapCallback, MapCB, (XtPointer)"TearOff #1A");
  XtAddCallback(PullDown1A, XmNunmapCallback, UnmapCB, (XtPointer) 4);

  CascadeBtn1   = CreateCascade("cascade1", 'c', PullDown1A, PullDown1);

  /*
   * Menu2
   */

  Label2 = CreateLabel("Menu2Label", PullDown2);
#ifndef MOTIF1_1
  XtAddCallback(PullDown2, XmNtearOffMenuActivateCallback, tearOffActivate,
		(XtPointer)Label2);
#endif
  Separator2    = CreateSeparator(PullDown2, "Separator2");
  ToggleBtn2    = CreateToggle("toggle2", 't', PullDown2);
  Separator3    = CreateSeparator(PullDown2,"Separator3");
  PullDown2A    = CreatePullDown(PullDown2, "PullDown2A");
  CascadeBtn2   = CreateCascade("cascade2", 'c', PullDown2A, PullDown2);
  Separator4    = CreateSeparator(PullDown2, "Separator4");
  PushBtn2      = CreatePushButton("pushbutton2", 'p', PullDown2);

  /*
   * Menu3
   */

  Label3        = CreateLabel("Menu3Label", PullDown3);
#ifndef MOTIF1_1
  XtAddCallback(PullDown3, XmNtearOffMenuActivateCallback, tearOffActivate,
		(XtPointer)Label3);
#endif
  PullDown3A    = CreatePullDown(PullDown3, "PullDown3A");
  CascadeBtn3   = CreateCascade("cascade3", 'c', PullDown3A, PullDown3);
  PushBtn3      = CreatePushButton("pushbutton3", 'p', PullDown3);
  ToggleBtn3    = CreateToggle("toggle3", 't', PullDown3);

  /*
   * Menu1A
   */

  Label1A       = CreateLabel("Menu1ALabel", PullDown1A);
#ifndef MOTIF1_1
  XtAddCallback(PullDown1A, XmNtearOffMenuActivateCallback, tearOffActivate,
		(XtPointer)Label1A);
#endif
  Separator1A   = CreateSeparator(PullDown1A, "Separator1A");
  PushBtn1A     = CreatePushButton("pushbutton1A", 'A', PullDown1A);
  PushBtn1B     = CreatePushButton("pushbutton1B", 'B', PullDown1A);
  PushBtn1C     = CreatePushButton("pushbutton1C", 'C',PullDown1A);

  /*
   * Menu2A
   */

  Label2A       = CreateLabel("Menu2ALabel", PullDown2A);
  Separator2A   = CreateSeparator(PullDown2A, "Separator2A");
  ToggleBtn2A   = CreateToggle("toggle2A", 'A', PullDown2A);
  ToggleBtn2B   = CreateToggle("toggle2B", 'B', PullDown2A);
  ToggleBtn2C   = CreateToggle("toggle2C", 'C', PullDown2A);

  /*
   * Menu3A
   */

  Label3A       = CreateLabel("Menu3ALabel", PullDown3A);
  Separator3A   = CreateSeparator(PullDown3A, "Separator3A");
  PullDown3AA   = CreatePullDown(PullDown3A, "PullDown3AA");
  CascadeBtn3A  = CreateCascade("cascade3A", 'A', PullDown3AA, PullDown3A);
  PullDown3BA   = CreatePullDown(PullDown3A, "PullDown3BA");
  CascadeBtn3B  = CreateCascade("cascade3B", 'B', PullDown3BA, PullDown3A);
  PullDown3CA   = CreatePullDown(PullDown3A, "PullDown3CA");
  CascadeBtn3C  = CreateCascade("cascade3C", 'C', PullDown3CA, PullDown3A);

  /*
   * Menu3AA
   */

  Label3AA      = CreateLabel("Menu3AALabel", PullDown3AA);
  Separator3AA  = CreateSeparator(PullDown3AA, "Separator3AA");
  PushBtn3AA    = CreatePushButton("pushbutton3AA", 'p', PullDown3AA);

  /*
   * Menu3BA
   */

  Label3BA      = CreateLabel("Menu3BALabel", PullDown3BA);
  Separator3BA  = CreateSeparator(PullDown3BA, "Separator3BA");
  ToggleBtn3BA  = CreateToggle("toggle3BA", 't', PullDown3BA);

  /*
   * Menu3CA
   */

  Label3CA      = CreateLabel("Menu3CALabel", PullDown3CA);
  Separator3CA  = CreateSeparator(PullDown3CA, "Separator3CA");
  PullDown3CAA  = CreatePullDown(PullDown3CA, "PullDown3CAA");

  CascadeBtn3CA = CreateCascade("cascade3CA", 'c', PullDown3CAA,
				PullDown3CA);

  /*
   * Menu3CAA
   */

  Label3CAA     = CreatePushButton("Menu3CAALabel", 'C', PullDown3CAA);

  CommonPause();
  CommonPause();

/* Add extra pauses for Tear off tests */

#ifndef MOTIF1_1
  CommonPause();
  CommonPause();  
  CommonPause();
  CommonPause();
  CommonPause();
#endif

  CommonPause();
  CommonPause();
  CommonPause();

  n = 0;
  MenuBtn6 = CreateCascade("MenuBtn6", '6', 0, MenuBar1);
  XtAddCallback(MenuBtn6, XmNcascadingCallback, CascadeCB, (XtPointer) 0);

  CommonPause();
  n = 0;
  MenuBtn7 = CreateCascade("MenuBtn7", '7', 0, MenuBar1);
  XtAddCallback(MenuBtn7, XmNactivateCallback, ActCB, (XtPointer) 0);

  CommonPause();

  /* 
   * Test for 8 bit mnemonic
   */

  PullDown4 = CreatePullDown(MenuBar1, "pulldown4");
  Menu4 = CreateCascade("Menu4", (unsigned char) '\247', PullDown4, MenuBar1);

  CommonPause();

  /*  
   *  Add code for PIR 2965 
   *  Illegal mnemonics cause core dump
   */

  PullDown5 = CreatePullDown(MenuBar1, "pulldown5");
  Menu5 = CreateCascade("Menu5", (unsigned char) '\t',PullDown5, MenuBar1);
  
  CommonPause();
  CommonPause();

/*
 * Add extra CommonPause to test CUA menu enhancements
 */

  CommonPause();

  XtDestroyWidget(MenuBar1);
 
  XtAppMainLoop(app_context);
}












