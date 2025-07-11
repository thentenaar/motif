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
static char rcsid[] = "$XConsortium: Command4.c /main/7 1995/07/13 19:17:16 drk $"
#endif
#endif

#include <testlib.h>

#define MAXHISTORY 128
#define MAX_ITEMS 50

void HeightChange();
void WidthChange();
void ProcessListItems();
void PopulateList();
void CommandChanged();
static void    Quit();
void 	       PrintHeaders();
void	       CreateCommandBox();
void CallRedo();

char 	 *Istrings[MAXHISTORY];
XmString itemTable[MAX_ITEMS];

#define NUM_TOGGLES 7

Widget    Command1, Text1;
Widget    toggles[NUM_TOGGLES];
Widget    Frame0, SetB, GetB, Append, Error;
Widget    Scale0, Scale1;
Widget    Layout;


void  main(argc, argv)
    int     argc;
    char  **argv;
{
    register int  n, i;
    Arg           args[MAX_ARGS];
    Widget        Separator1, Frame1;
    Widget        Form2;
    Widget        Scale2, ControlBox;
    Widget        Frame2, RadioBox1;
    Widget        Go, PopB, QuitB, Redo;
    Widget	  Frame3, RowCol;
    static char   *toggle_names[ ] = { 
				      "listVisibleItemCount",
				      "historyVisibleItemCount",
				      "listItemCount",
				      "historyItemCount",
				      "historyMaxItems",
				      "promptString",
				      "command"

				      };
    /* NOTE: If the CONTENTS of this list are changed, you may have to
       change routine PopulateList() !!!! */

    char 	buf[32];
    Dimension 	height;
    Dimension 	width;
    int 	offset;
    XmString    tcs;

#ifdef DEBUG
    printf ("Number of names in name array %d\n", 
	             ((sizeof (toggle_names))/(sizeof(char *))) );
#endif

    CommonTestInit(argc, argv);

    XtRealizeWidget(Shell1);
    

    /* Overall Plan:
       Create Command box whose list/listCount resources can be set
       and gotten by user-driven interface
     */

    n = 0;
    Layout = XmCreateForm (Shell1, "Layout", args, n);
    XtRealizeWidget(Layout);
    XtManageChild(Layout);

    CreateCommandBox(Layout, True, NULL);

    n = 0;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); 	n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); 	n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); 	n++;
    XtSetArg (args[n], XmNtopWidget, Frame0);	 		n++;
    XtSetArg (args[n], XmNtopOffset, 5);			n++;
    XtSetArg (args[n], XmNrightOffset, 5);			n++;
    XtSetArg (args[n], XmNleftOffset, 5);			n++;
    Frame1 = XmCreateFrame (Layout, "Frame1", args, n);
    XtRealizeWidget(Frame1);
    XtManageChild(Frame1);

#ifdef DEBUG
    n = 0;
    XtSetArg (args[n], XmNtopOffset, &offset); 			n++;
    XtGetValues (Frame1, args, n);
    printf ("Dependent's starting top offset is %d\n", offset);
#endif

    n = 0;
    Form2 = XmCreateForm (Frame1, "Form2", args, n);
    XtRealizeWidget(Form2);
    XtManageChild(Form2);

    /* This is to get around Command not REALLY knowing how big it wants
       to be until after Realize. If this is a bug, it's a bug in the
       way XtRealize works */

    n = 0;
    tcs = XmStringCreateSimple ("Width in Pixels");
    XtSetArg (args[n], XmNmappedWhenManaged, False); 		n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); 	n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); 	n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); 	n++;
    XtSetArg (args[n], XmNshowValue, True); 			n++;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL); 		n++;
    XtSetArg (args[n], XmNtitleString, tcs );			n++;
    XtSetArg (args[n], XmNmaximum, 500); 			n++;
    XtSetArg (args[n], XmNminimum, 0); 				n++;
    Scale0 = XmCreateScale (Form2, "Scale0", args, n);
    XtRealizeWidget(Scale0);
    XmStringFree(tcs);
    XtManageChild(Scale0);

    XtAddCallback (Scale0, XmNvalueChangedCallback, WidthChange, NULL); 

    n = 0;
    tcs = XmStringCreateSimple ("Height in Pixels");
    XtSetArg (args[n], XmNmappedWhenManaged, False); 		n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); 	n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); 	n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); 	n++;
    XtSetArg (args[n], XmNtopWidget, Scale0);			n++;
    XtSetArg (args[n], XmNshowValue, True); 			n++;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL); 		n++;
    XtSetArg (args[n], XmNtitleString, tcs );                   n++;
    XtSetArg (args[n], XmNmaximum, 500); 			n++;
    XtSetArg (args[n], XmNminimum, 0); 				n++;
    Scale1 = XmCreateScale (Form2, "Scale1", args, n);
    XtRealizeWidget(Scale1);
    XmStringFree(tcs);
    XtManageChild(Scale1);

    XtAddCallback (Scale1, XmNvalueChangedCallback, HeightChange, NULL); 

    n = 0;
    tcs = XmStringCreateSimple ("Item Count/Visible Item Count");
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); 	n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); 	n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); 	n++;
    XtSetArg (args[n], XmNtopWidget, Scale1); 			n++;
    XtSetArg (args[n], XmNshowValue, True); 			n++;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL); 		n++;
    XtSetArg (args[n], XmNtitleString, tcs );                   n++;
    XtSetArg (args[n], XmNmaximum, MAX_ITEMS); 			n++;
    XtSetArg (args[n], XmNminimum, 0); 				n++;
    Scale2 = XmCreateScale (Form2, "Scale2", args, n);
    XtRealizeWidget(Scale2);
    XmStringFree( tcs );
    XtManageChild(Scale2);

    n = 0;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); 	n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); 	n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); 	n++;
    XtSetArg (args[n], XmNtopWidget, Scale2); 			n++;
    XtSetArg(args[n], XmNeditMode, XmSINGLE_LINE_EDIT);		n++;
    Text1 = XmCreateText(Form2, "Text1", args, n);
    XtRealizeWidget(Text1);
    XtManageChild(Text1);

    n = 0;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); 	n++;
    XtSetArg (args[n], XmNtopWidget, Text1); 			n++;
    ControlBox = XmCreateRowColumn (Form2, "ControlBox", args, n);
    XtRealizeWidget(ControlBox);
    XtManageChild(ControlBox);
    
    for (i = 0; i <= NUM_TOGGLES - 1; i++)
      {    
		   n = 0;
           tcs = XmStringCreateSimple(toggle_names[i]);
           XtSetArg (args[n], XmNlabelString, tcs ); n++;
  	   XtSetArg (args[n], XmNuserData, (XtPointer)toggle_names[i]);	n++;
           sprintf(buf, "toggleButton%d", i);
	   toggles[i] = XmCreateToggleButtonGadget(ControlBox, buf, args, n);
	   XtRealizeWidget (toggles[i]);
	   XtManageChild (toggles[i]);
           XmStringFree( tcs );
       }

    n = 0;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); 	n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); 	n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); 	n++;
    XtSetArg (args[n], XmNtopWidget, Frame1); 			n++;
    XtSetArg (args[n], XmNleftOffset, 5); 			n++;
    XtSetArg (args[n], XmNrightOffset, 5); 			n++;
    Frame2 = XmCreateFrame (Layout, "Frame2", args, n);
    XtRealizeWidget(Frame2);
    XtManageChild(Frame2);

    n = 0;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL); 		n++;
    XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); 		n++;
    RadioBox1 = XmCreateRadioBox (Frame2, "RadioBox1", args, n);
    XtRealizeWidget(RadioBox1);
    XtManageChild(RadioBox1);

    n = 0;
    tcs = XmStringCreateSimple ("Get");
    XtSetArg (args[n], XmNlabelString, tcs );			n++;
    GetB = XmCreateToggleButtonGadget(RadioBox1, "GetB", args, n);
    XtRealizeWidget(GetB);
    XmStringFree(tcs);
    XtManageChild(GetB);

    n = 0;
    tcs = XmStringCreateSimple ("Set");
    XtSetArg (args[n], XmNlabelString, tcs );			n++;
    SetB = XmCreateToggleButtonGadget(RadioBox1, "SetB", args, n);
    XtRealizeWidget(SetB);
    XmStringFree(tcs);
    XtManageChild(SetB);

    n = 0;
    tcs = XmStringCreateSimple ("Append");
    XtSetArg (args[n], XmNlabelString, tcs );			n++; 
    Append = XmCreateToggleButtonGadget(RadioBox1, "Append", args, n);
    XtRealizeWidget(Append);
    XmStringFree(tcs);
    XtManageChild(Append);

    n = 0;
    tcs = XmStringCreateSimple ("Error");
    XtSetArg (args[n], XmNlabelString, tcs );			n++;
    Error = XmCreateToggleButtonGadget(RadioBox1, "Error", args, n);
    XtRealizeWidget(Error);
    XmStringFree(tcs);
    XtManageChild(Error);

    n = 0;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); 	n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); 	n++;
    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); 	n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); 	n++;
    XtSetArg (args[n], XmNtopWidget, Frame2); 			n++;
    XtSetArg (args[n], XmNleftOffset, 5); 			n++;
    XtSetArg (args[n], XmNrightOffset, 5); 			n++;
    XtSetArg (args[n], XmNbottomOffset, 5); 			n++;
    Frame3 = XmCreateFrame (Layout, "Frame3", args, n);
    XtRealizeWidget(Frame3);
    XtManageChild(Frame3);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL);		n++;
    RowCol = XmCreateRowColumn(Frame3, "RowCol", args, n);
    XtRealizeWidget(RowCol);
    XtManageChild(RowCol);

    n = 0;
    tcs = XmStringCreateSimple ("Perform Action");
    XtSetArg (args[n], XmNlabelString, tcs );			n++;
    Go = XmCreatePushButtonGadget(RowCol, "Go", args, n);
    XtRealizeWidget(Go);
    XmStringFree(tcs);
    XtManageChild(Go);

    XtAddCallback (Go, XmNactivateCallback, ProcessListItems, Scale2);

    n = 0;
    tcs = XmStringCreateSimple ("Populate List");
    XtSetArg (args[n], XmNlabelString, tcs );			n++;
    PopB = XmCreatePushButton (RowCol, "PopB", args, n);
    XtRealizeWidget(PopB);
    XmStringFree(tcs);
    XtManageChild(PopB);

    XtAddCallback (PopB, XmNactivateCallback, PopulateList, Scale2);

    n = 0;
    tcs = XmStringCreateSimple("Recreate Command");
    XtSetArg (args[n], XmNlabelString, tcs );		n++;
    Redo = XmCreatePushButtonGadget(RowCol, "Redo", args, n);
    XtRealizeWidget(Redo);
    XmStringFree(tcs);
    XtManageChild(Redo);

    XtAddCallback (Redo, XmNactivateCallback, CallRedo, Frame1);

    n = 0;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple ("Quit")); 			n++;
    QuitB = XmCreatePushButtonGadget (RowCol, "QuitB", args, n);
    XtRealizeWidget(QuitB);
    XtManageChild(QuitB);

    XtAddCallback (QuitB, XmNactivateCallback, Quit, NULL);

    
    /* manage widgets */
    XtManageChild(Redo);
    XtManageChild(QuitB);
    XtManageChild(Go);
    XtManageChild(PopB);
    XtManageChild(SetB);
    XtManageChild(GetB);
    XtManageChild(Append);
    XtManageChild(Error);
    XtManageChild(RadioBox1);
    XtManageChild(Frame2);
    XtManageChild(Frame3);
    XtManageChild(ControlBox);
    XtManageChild(Scale0);    
    XtManageChild(Scale1);    
    XtManageChild(Scale2);
    XtManageChild(Text1);
    XtManageChild(Form2);
    XtManageChild(Frame1);
    XtManageChild(Frame3);
    XtManageChild(RowCol);
    XtManageChild(Layout);

    XtRealizeWidget(Shell1);

    /* quick, update the scale to the correct command box width and height */
    n = 0;
    XtSetArg (args[n], XmNheight, &height); 			n++;
    XtSetArg (args[n], XmNheight, &width); 			n++;
    XtGetValues (Command1, args, n);

#ifdef DEBUG
    printf ("Height of Command received is %d\n", (int) height);
    printf ("Width of Command received is %d\n", (int) width);
#endif

    n = 0;
    XtSetArg (args[n], XmNmappedWhenManaged, True); 		n++;
    XtSetArg (args[n], XmNvalue, (int) height); 		n++;
    XtSetValues (Scale1, args, n);

    n = 0;
    XtSetArg (args[n], XmNmappedWhenManaged, True); 		n++;
    XtSetArg (args[n], XmNvalue, (int) width); 			n++;
    XtSetValues (Scale0, args, n);

    XtMapWidget (Scale0);
    XtMapWidget (Scale1);

    CommonPause();

    XtAppMainLoop(app_context);
}


void CallRedo(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;

{
    Dimension width, height;
    Arg args[2];
    int n;

    CreateCommandBox (Layout, False, (Widget) client_data);

    /* quick, update the scale to the correct command box width and height */
    n = 0;
    XtSetArg (args[n], XmNheight, &height); 			n++;
    XtSetArg (args[n], XmNheight, &width); 			n++;
    XtGetValues (Command1, args, n);

#ifdef DEBUG
    printf ("Height of Command received is %d\n", (int) height);
    printf ("Width of Command received is %d\n", (int) width);
#endif

    n = 0;
    XtSetArg (args[n], XmNmappedWhenManaged, True); 		n++;
    XtSetArg (args[n], XmNvalue, (int) height); 		n++;
    XtSetValues (Scale1, args, n);

    n = 0;
    XtSetArg (args[n], XmNmappedWhenManaged, True); 		n++;
    XtSetArg (args[n], XmNvalue, (int) width); 			n++;
    XtSetValues (Scale0, args, n);

}


void CreateCommandBox(parent, firstTime, dependent)
    Widget parent;
    Boolean firstTime;
    Widget dependent;
{
    Arg args[10];
    int n;
#ifdef DEBUG
    XmFormWidget DEBUGW;
    int offset;

    DEBUGW = (XmFormWidget) parent;
#endif

    if (!firstTime) {
   	n = 0;
     	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); 	n++;
     	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); 	n++;
	XtSetArg (args[n], XmNtopOffset, 5);			n++;
	XtSetArg (args[n], XmNleftOffset, 5);			n++;
     	XtSetValues (dependent, args, n);
	XtDestroyWidget (Frame0);
  	XtDestroyWidget (Command1);

    }

    n = 0;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); 	n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); 	n++;
    XtSetArg (args[n], XmNtopOffset, 5);			n++;
    XtSetArg (args[n], XmNleftOffset, 5);			n++;
    Frame0 = XmCreateFrame(parent, "Frame0", args, n);
    XtRealizeWidget(Frame0);
    XtManageChild(Frame0);

    n = 0;
    Command1 = XmCreateCommand(Frame0, "Command1", args, n);
    XtRealizeWidget(Command1);
    XtManageChild(Command1);

    XtAddCallback (Command1, XmNcommandChangedCallback, CommandChanged, 
		   (XtPointer) 1);
    XtAddCallback (Command1, XmNcommandEnteredCallback, CommandChanged, 
		   (XtPointer) 2);

    XtManageChild (Command1);

    if (!firstTime) {
    	n = 0;
    	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); 	n++;
    	XtSetArg (args[n], XmNtopWidget, Frame0); n++;
    	XtSetValues (dependent, args, n);

#ifdef DEBUG
    	n = 0;
    	XtSetArg (args[n], XmNtopOffset, &offset); 		n++;
    	XtGetValues (dependent, args, n);
    	printf ("Dependent's top offset is %d\n", offset);
#endif
    }

}


void CommandChanged(w, client_data, call_data)
    Widget w;
    XtPointer client_data;
    XtPointer call_data;
    
{
    static int i = 0;
    
    switch ((int) client_data) {
    	case 1:  printf ("Received CommandChangedCallback number %d\n", ++i);
       	         break;
    	case 2:  printf ("Received CommandEnteredCallback *********\n\n");
             	 break;
    	default: printf ("Received some bogus callback, HELP !!!!!!\n");
		 break;
    }
    
}


void WidthChange (w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
    
{
    Arg args[1];
    
    XtSetArg (args[0], XmNwidth, 
    	  ((Dimension)(((XmScaleCallbackStruct *)call_data)->value)));
    XtSetValues (Command1, args, 1);
    
}


void HeightChange (w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
    
{
    Arg args[1];
    
    XtSetArg (args[0], XmNheight, 
    	  ((Dimension)(((XmScaleCallbackStruct *)call_data)->value)));
    XtSetValues (Command1, args, 1);
    
}


void ProcessListItems (w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;

/* figure out what setting of each toggle is */
/* and do appropriate thing */
/* client data is widget ID of Scale2 */

{
    int n, i;
    Arg args[1];
    char *resource_name;
    char *reg_string;
    int  countValue;
    XmString textString, tcs;

printf ("Processing List items....\n");

if (XmToggleButtonGetState (GetB))     /* get is true */
  { PrintHeaders();

    for (i = 0; i <= NUM_TOGGLES - 1; i++)
	{  /* if a reztoggle is on, get value of that */
	   /* Command resource */
           /* the userData of the togglebutton is the index into toggle_names*/
	   /* that contains the string to be set */
           
	  if (XmToggleButtonGadgetGetState(toggles[i]))
	    { 
	      XtSetArg (args[0], XmNuserData, &resource_name);
	      XtGetValues (toggles[i], args, 1);
	     
	      if ((strcmp(resource_name, "promptString") != 0) &&
		  (strcmp(resource_name, "command") !=0) ) {
	      	XtSetArg (args[0], resource_name, &countValue);
	      	XtGetValues (Command1, args, 1);

	      	printf ("%25s\t%d\n", resource_name, countValue);
	      }
	      else {
		XtSetArg (args[0], resource_name, &textString);
		XtGetValues (Command1, args, 1);

		printf("%25s\t%s\n", resource_name, CommonCsToRs(textString));
	      }
	      
	    }
	}
    }
else   
  if (XmToggleButtonGetState (SetB)) {   /* set */
       XtSetArg (args[0], XmNvalue, &countValue);
       XtGetValues ((Widget) client_data, args, 1);
   
       reg_string = XmTextGetString(Text1);
       tcs = XmStringCreateSimple(reg_string);

       for (i = 0; i <= NUM_TOGGLES - 1; i++)
	  if (XmToggleButtonGadgetGetState(toggles[i]))
	    {
	      XtSetArg (args[0], XmNuserData, &resource_name);
	      XtGetValues (toggles[i], args, 1);
	     
	      if ((strcmp(resource_name, "promptString") != 0) &&
		  (strcmp(resource_name, "command") !=0) ) {
	      		XtSetArg (args[0], resource_name, countValue);
	      		XtSetValues (Command1, args, 1);	      
	      }
	      else {
			if (strcmp(resource_name, "command") ==0)
				XmCommandSetValue(Command1, tcs);
			else {
				XtSetArg (args[0], resource_name, tcs);
				XtSetValues (Command1, args, 1);

			}
			XmTextSetString(Text1, NULL);
	      }
	    }
         XtFree(reg_string);
         XmStringFree(tcs);
   }
   else 
	if (XmToggleButtonGetState (Append) ||
	    XmToggleButtonGetState (Error)) {
	    reg_string = XmTextGetString(Text1);
	    tcs = XmStringCreateSimple(reg_string);

	    for (i = 0; i <= NUM_TOGGLES - 1; i++)
		if (XmToggleButtonGadgetGetState(toggles[i])) {

	    		XtSetArg (args[0], XmNuserData, &resource_name);
	    		XtGetValues (toggles[i], args, 1);

			if (strcmp(resource_name, "command") != 0)
				printf("Inappropriate append for resource %s\n",
					resource_name);
			else {
				if (XmToggleButtonGetState (Append))
	    				XmCommandAppendValue(Command1, tcs);
				else
					XmCommandError(Command1, tcs);
				XmTextSetString(Text1, NULL);
			}
		}
           XtFree(reg_string);
           XmStringFree(tcs);
	}
			
fflush (stdout);
}


void PopulateList (w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data; /* client data Widget ID of Scale2 */
{
    char buf[32];
    int value;
    register int i;
    Arg args[2];
    
    /* change Command's history list to contain # of items set by Scale */
    /* we'll set either the historyItems or listItems depending on what's */
    /* on: listItemCount or historyItemCount */
    /* historyItem will be the default */
    /* first, create a list of items, number of items scale's value */
    
    XmScaleGetValue ((Widget)client_data, &value);
    
    printf ("Creating %d strings\n", value);
    
    for (i = 0; i <= (value - 1); i++) {
     	 sprintf (buf, "ListItem%d", i);
       	 itemTable[i] = XmStringCreateSimple (buf);
    }
    
    /* we SHOULD look through all items and figure out which are 
       listItemCount and histItemCount but for now, we'll just KNOW
       they are in toggles[2,3] */
    
    if  ( (XmToggleButtonGadgetGetState (toggles[2])) &&
          (!(XmToggleButtonGadgetGetState(toggles[3]))) ) {
         	XtSetArg (args[0], XmNlistItems, itemTable);
          	XtSetArg (args[1], XmNlistItemCount, value);
          	XtSetValues(Command1, args, 2);
    }
    else {
         	XtSetArg (args[0], XmNhistoryItems, itemTable);
          	XtSetArg (args[1], XmNhistoryItemCount, value);
          	XtSetValues(Command1, args, 2);
    }
}


void PrintHeaders()

{
    printf ("\n%25s\tValue\n", "Resource Name");
    printf ("%25s\t=====\n\n", "=============");
    fflush (stdout);
}


static void  Quit()
{
     printf("Begin exiting ... please standby ... \n");
     fflush(stdout);
     exit(0);
}
