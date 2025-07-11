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
static char rcsid[] = "$XConsortium: xmAuto.c /main/4 1995/07/13 20:57:42 drk $Date: 92/03/13
18:36:34 $"
#endif
#endif

#include <testlib.h>
#include <xmAuto.h>

void main(argc, argv)
int  	argc;
char 	**argv;
{
     int 	n, i;
     Arg 	args[MAX_ARGS];

     XtToolkitInitialize();
     app_context = XtCreateApplicationContext();
     display = XtOpenDisplay(app_context, NULL, argv[0], "XMauto",
     				NULL, 0, &argc, argv);
     if (display == NULL) {
      	  fprintf(stderr, "%s:  Can't open display\n", argv[0]);
       	  exit(1);
     }

     XtSetArg(args[n], XmNallowShellResize, True);  	n++;
     Shell1 = XtAppCreateShell(argv[0], "XMauto", 
		               applicationShellWidgetClass, display, args, n);

     CreateLayout();

     CreateDialogs();

     XtRealizeWidget(Shell1);

     XtAppMainLoop(app_context);

}


static void CreateLayout()
{
    Arg args[MAX_ARGS];
    int n;

    Widget Form, RadioBox1, RadioBox2;
    Widget Frame0, Frame1, Frame2, Frame3;
    Widget Frame4, Frame5;
    Widget Frame1Title, Frame2Title, Frame3Title;
    Widget Frame4Title, Frame5Title; 
    Widget RowCol1, RowCol2, RowCol3;
    Widget PushVisualFile, PushDataFile, PushScriptFile;
    Widget PushInstructions, PushFailColor, PushExtra;
    Widget PushRun, PushQuit, PushAddTest, PushClear;
    Widget PushRead, PushWrite, PushSave;

    XmString tcs;

    n = 0; 
    Frame0 = XmCreateFrame(Shell1, "Frame0", args, n);
    XtManageChild(Frame0);

    tcs = XmStringCreateSimple("Automation Test Execution Environment");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD);	n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment, 
	     XmALIGNMENT_CENTER);				n++;
    Frame1Title = XmCreateLabel(Frame0, "Frame1Title", args, n);
    XtManageChild(Frame1Title);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNmarginWidth, 10);			n++;
    XtSetArg(args[n], XmNmarginHeight, 10);			n++;
    Form = XmCreateForm(Frame0, "Form", args, n);
    XtManageChild(Form);

    n = 0; 
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
    Frame1 = XmCreateFrame(Form, "Frame1", args, n);
    XtManageChild(Frame1);

    tcs = XmStringCreateSimple("Tests to be run");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD);	n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment, 
	     XmALIGNMENT_CENTER);				n++;
    Frame1Title = XmCreateLabel(Frame1, "Frame1Title", args, n);
    XtManageChild(Frame1Title);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNvisibleItemCount, 10);			n++;
    XtSetArg(args[n], XmNwidth, 200);				n++;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT);		n++;
    List1 = XmCreateScrolledList(Frame1, "List1", args, n);
    XtManageChild(List1);

    XtAddCallback(List1, XmNbrowseSelectionCallback, BrowseCB, NULL);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNleftWidget, Frame1);			n++;
    XtSetArg(args[n], XmNleftOffset, 10);			n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
    Frame2 = XmCreateFrame(Form, "Frame2", args, n);
    XtManageChild(Frame2);

    tcs = XmStringCreateSimple("Execution Mode");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD);	n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment, 
	     XmALIGNMENT_BEGINNING);				n++;
    Frame2Title = XmCreateLabel(Frame2, "Frame2Title", args, n);
    XtManageChild(Frame2Title);

    XmStringFree(tcs);

    n = 0;
    RadioBox1 = XmCreateRadioBox(Frame2, "RadioBox1", args, n);
    XtManageChild(RadioBox1);

    tcs = XmStringCreateSimple("Interactive Manual Mode");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    ToggleManual = XmCreateToggleButton(RadioBox1, "ToggleManual", args, n);
    XtManageChild(ToggleManual);

    XtAddCallback(ToggleManual, XmNarmCallback, SimpleCB, MANUAL);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("Record Mode");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    ToggleRecord = XmCreateToggleButton(RadioBox1, "ToggleRecord", args, n);
    XtManageChild(ToggleRecord);

    XtAddCallback(ToggleRecord, XmNarmCallback, SimpleCB, RECORD);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("Compare Mode");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    XtSetArg(args[n], XmNset, True);				n++;
    ToggleCompare = XmCreateToggleButton(RadioBox1, "ToggleCompare", args, n);
    XtManageChild(ToggleCompare);

    XtAddCallback(ToggleCompare, XmNarmCallback, SimpleCB, COMPARE);

    XmStringFree(tcs); 

    tcs = XmStringCreateSimple("Ignore Comparsions");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    ToggleIgnore = XmCreateToggleButton(RadioBox1, "ToggleIgnore", args, n);
    XtManageChild(ToggleIgnore);

    XtAddCallback(ToggleIgnore, XmNarmCallback, SimpleCB, IGNORE);

    XmStringFree(tcs);
    
    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNleftWidget, Frame2);			n++;
    XtSetArg(args[n], XmNleftOffset, 10);			n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
    Frame3 = XmCreateFrame(Form, "Frame3", args, n);
    XtManageChild(Frame3);

    tcs = XmStringCreateSimple("Extra Options");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD);	n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment, 
	     XmALIGNMENT_BEGINNING);				n++;
    Frame3Title = XmCreateLabel(Frame3, "Frame3Title", args, n);
    XtManageChild(Frame3Title);

    XmStringFree(tcs);

    n = 0;
    RowCol1 = XmCreateRowColumn(Frame3, "RowCol1", args, n);
    XtManageChild(RowCol1);

    tcs = XmStringCreateSimple("Full Error Reporting");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    ToggleFull = XmCreateToggleButton(RowCol1, "ToggleFull", args, n);
    XtManageChild(ToggleFull);

    XtAddCallback(ToggleFull, XmNarmCallback, SimpleCB, FULL);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("Produce xwd Window dumps on failures");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    ToggleWinDump = XmCreateToggleButton(RowCol1, "ToggleWinDump", args, n);
    XtManageChild(ToggleWinDump);

    XtAddCallback(ToggleWinDump, XmNarmCallback, SimpleCB, DUMP);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("Trace mode");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    ToggleTrace = XmCreateToggleButton(RowCol1, "ToggleTrace", args, n);
    XtManageChild(ToggleTrace);

    XtAddCallback(ToggleTrace, XmNarmCallback, SimpleCB, TRACE);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNleftWidget, Frame1);			n++;
    XtSetArg(args[n], XmNleftOffset, 10);			n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNtopWidget, Frame2);			n++;
    XtSetArg(args[n], XmNtopOffset, 20);			n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
    Frame4 = XmCreateFrame(Form, "Frame4", args, n);
    XtManageChild(Frame4);

    tcs = XmStringCreateSimple("Options");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD);	n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment, 
	     XmALIGNMENT_BEGINNING);				n++;
    Frame4Title = XmCreateLabel(Frame4, "Frame4Title", args, n);
    XtManageChild(Frame4Title);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER);	n++;
    RowCol2 = XmCreateRowColumn(Frame4, "RowCol2", args, n);
    XtManageChild(RowCol2);

    tcs = XmStringCreateSimple("Delay Time");

    n = 0;
    XtSetArg(args[n], XmNtitleString, tcs);			n++;
    XtSetArg(args[n], XmNmaximum, 50);				n++;
    XtSetArg(args[n], XmNminimum, 0);				n++;
    XtSetArg(args[n], XmNvalue, 0);				n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL);		n++;
    XtSetArg(args[n], XmNshowValue, True);			n++;
    ScaleDelay = XmCreateScale(RowCol2, "ScaleDelay", args, n);
    XtManageChild(ScaleDelay);

    XtAddCallback(ScaleDelay, XmNvalueChangedCallback, SimpleCB, DELAY);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("Change Failure Color");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    PushFailColor = XmCreatePushButton(RowCol2, "PushFailColor", args, n);
    XtManageChild(PushFailColor);

    XtAddCallback(PushFailColor, XmNactivateCallback, PromptCB, COLOR);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("Change Input Instructions File");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    PushInstructions = XmCreatePushButton(RowCol2, "PushInstructions", args, n);
    XtManageChild(PushInstructions);

    XtAddCallback(PushInstructions, XmNactivateCallback, 
		  FileSelectCB, INSTRUCT);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("Change Input Script File");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    PushScriptFile = XmCreatePushButton(RowCol2, "PushScriptFile", args, n);
    XtManageChild(PushScriptFile);

    XtAddCallback(PushScriptFile, XmNactivateCallback, FileSelectCB, SCRIPT);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("Change Output Visual File");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    PushVisualFile = XmCreatePushButton(RowCol2, "PushVisualFile", args, n);
    XtManageChild(PushVisualFile);

    XtAddCallback(PushVisualFile, XmNactivateCallback, FileSelectCB, VISUAL);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("Change Output Data File");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    PushDataFile = XmCreatePushButton(RowCol2, "PushDataFile", args, n);
    XtManageChild(PushDataFile);

    XtAddCallback(PushDataFile, XmNactivateCallback, FileSelectCB, OUTPUT);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("Add Extra Arguments");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    PushExtra = XmCreatePushButton(RowCol2, "PushExtra", args, n);
    XtManageChild(PushExtra);

    XtAddCallback(PushExtra, XmNactivateCallback, PromptCB, EXTRA);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNtopWidget, Frame1);			n++;
    XtSetArg(args[n], XmNtopOffset, 10);			n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNrightWidget, Frame4);			n++;
    XtSetArg(args[n], XmNrightOffset, 10);			n++;
    Frame5 = XmCreateFrame(Form, "Frame5", args, n);
    XtManageChild(Frame5);

    tcs = XmStringCreateSimple("Control");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    XtSetArg(args[n], XmNchildType, XmFRAME_TITLE_CHILD);	n++;
    XtSetArg(args[n], XmNchildHorizontalAlignment, 
	     XmALIGNMENT_BEGINNING);				n++;
    Frame5Title = XmCreateLabel(Frame5, "Frame5Title", args, n);
    XtManageChild(Frame5Title);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNentryAlignment, XmALIGNMENT_CENTER);	n++;
    RowCol3 = XmCreateRowColumn(Frame5, "RowCol3", args, n);
    XtManageChild(RowCol3);

    tcs = XmStringCreateSimple("Add Test");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    PushAddTest = XmCreatePushButton(RowCol3, "PushAddTest", args, n);
    XtManageChild(PushAddTest);

    XtAddCallback(PushAddTest, XmNactivateCallback, AddTestCB, NULL);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("Run Tests");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    PushRun = XmCreatePushButton(RowCol3, "PushRun", args, n);
    XtManageChild(PushRun);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("Clear Args");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    PushClear = XmCreatePushButton(RowCol3, "PushClear", args, n);
    XtManageChild(PushClear);

    XtAddCallback(PushClear, XmNactivateCallback, ClearCB, NULL);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("Write Tests to file");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    PushWrite = XmCreatePushButton(RowCol3, "PushWrite", args, n);
    XtManageChild(PushWrite);

    XtAddCallback(PushWrite, XmNactivateCallback, FileSelectCB, WRITE);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("Read Tests from file");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    PushRead = XmCreatePushButton(RowCol3, "PushRead", args, n);
    XtManageChild(PushRead);

    XtAddCallback(PushRead, XmNactivateCallback, FileSelectCB, READ);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("Save Test to List");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    PushSave = XmCreatePushButton(RowCol3, "PushSave", args, n);
    XtManageChild(PushSave);

    XtAddCallback(PushSave, XmNactivateCallback, SaveToListCB, NULL);

    XmStringFree(tcs);

    tcs = XmStringCreateSimple("Quit");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    PushQuit = XmCreatePushButton(RowCol3, "PushQuit", args, n);
    XtManageChild(PushQuit);

    XtAddCallback(PushQuit, XmNactivateCallback, QuitCB, NULL);

    tcs = XmStringCreateSimple("Current Test String:: None");

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
    XtSetArg(args[n], XmNtopWidget, Frame5);			n++;
    XtSetArg(args[n], XmNtopOffset, 20);			n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    Label1 = XmCreateLabel(Form, "Label1", args, n);
    XtManageChild(Label1);

}

static void CreateDialogs()
{
    Arg args[2];
    int n;

    n = 0;
    XtSetArg(args[n], XmNdirectory, NULL);			n++;
    XtSetArg(args[n], XmNautoUnmanage, True);			n++;
    FileSelectBox = 
		XmCreateFileSelectionDialog(Shell1, "FileSelectBox", args, n);

    n = 0;
    XtSetArg(args[n], XmNautoUnmanage, True);			n++;
    PromptDialog = XmCreatePromptDialog(Shell1, "PromptDialog", args, n);

}

static void ResetFSB()
{
    XmString tcs;
    Arg args[2];
    int n;

    tcs = XmStringCreateSimple("*");

    n = 0;
    XtSetArg(args[n], XmNdirectory, NULL);			n++;
    XtSetArg(args[n], XmNpattern, tcs);				n++;
    XtSetValues(FileSelectBox, args, n);

    XmStringFree(tcs);

    XtRemoveAllCallbacks(FileSelectBox, XmNokCallback);

}

static void AddTestCB(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
    Arg args[1];
    XmString tcs;
    int n;

    ClearArgs();

    ResetFSB();

    tcs = XmStringCreateSimple("*.c");

    n = 0;
    XtSetArg(args[n], XmNpattern, tcs);			n++;
    XtSetValues(FileSelectBox, args, n);

    XmStringFree(tcs);

    XtAddCallback(FileSelectBox, XmNokCallback, GetTestNameCB, NULL);

    XtManageChild(FileSelectBox);

}

static void GetTestNameCB(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
    XmFileSelectionBoxCallbackStruct *FSBStruct = 
			((XmFileSelectionBoxCallbackStruct *)call_data);
    int length, i;
    String value;

    for (i = 0 ; i < strlen(CurrentTestString) ; i++) 
	CurrentTestString[i] = ' ';
    XmStringGetLtoR(FSBStruct->value, XmSTRING_DEFAULT_CHARSET, &value);
    length = strlen(value);
    strncpy(CurrentTestString, value, length - 2);

    XtFree(value);

    BuildTestString(True);

}

static void BuildTestString(set)
Boolean set;
{
    XmString tcs;
    Arg args[MAX_ARGS];
    int n;

    strcpy(LabelString, "Current Test String:: ");
    strcat(LabelString, CurrentTestString);
    strcat(LabelString, " ");
    strcat(LabelString, arguments);

    tcs = XmStringCreateSimple(LabelString);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    XtSetValues(Label1, args, n);

    XmStringFree(tcs);

    if (set) {
    	tcs = XmStringCreateSimple(CurrentTestString);

    	XmListAddItemUnselected(List1, tcs, 0);
    	XmListSetHorizPos(List1, 200);

        n = 0;
        XtSetArg(args[n], XmNitemCount, &list_pos);		n++;
        XtGetValues(List1, args, n);

    	XmStringFree(tcs);
    }

}

static void SimpleCB(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
    int i, type, n;
    Arg args[5];
    char tmp_str[5];
    String value = NULL;
    XmString tcs = NULL;

    int scale_value;

    type = (int) client_data;

    switch(type) {
	case MANUAL:	if (!manual)
				strcat(arguments, " -m ");
			break;
	case RECORD:	if (!record)
				strcat(arguments, " -r ");
			break;
	case IGNORE:	if (!ignore)
				strcat(arguments, " -c ");
			break;
	case FULL:	if (!full)
				strcat(arguments, " -a ");
			break;
	case DUMP:	if (!dumps)
				strcat(arguments, " -W ");
			break;
	case TRACE:	if (!trace)
				strcat(arguments, " -T ");
			break;
	case DELAY:	XmScaleGetValue(ScaleDelay, &scale_value);
			if (scale_value != 0) {
				strcat(arguments, " -D ");
				sprintf(tmp_str, "%d ", scale_value);
				strcat(arguments, tmp_str);
			}
			break;
	case COLOR:	n = 0;
			XtSetArg(args[n], XmNtextString, &tcs);		n++;
			XtGetValues(PromptDialog, args, n);
			XmStringGetLtoR(tcs, XmSTRING_DEFAULT_CHARSET, &value);
			if (strcmp(value, "") != 0) {
				strcat(arguments, " -C ");
				strcat(arguments, value);
				strcat(arguments, " ");
			}
			XmStringFree(tcs);
			break;
	case EXTRA:	n = 0;
			XtSetArg(args[n], XmNtextString, &tcs);		n++;
			XtGetValues(PromptDialog, args, n);
			XmStringGetLtoR(tcs, XmSTRING_DEFAULT_CHARSET, &value);
			if (strcmp(value, "") != 0) {
				strcat(arguments, " ");
				strcat(arguments, value);
				strcat(arguments, " ");
			}
			XmStringFree(tcs);
			break;
	case COMPARE:	break;
	default:	printf("xmAuto (warning): Unkown Option in SimpleCB\n");
			break;
    }

    BuildTestString(False);

}

static void ClearCB(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{

    ClearArgs();

}

static void ClearArgs()
{
    int i, n;

    manual = False; record = False;
    compare = True; ignore = False;
    full = False; dumps = False;
    trace = False; delay = 0;

    XmToggleButtonSetState(ToggleManual, manual, False);
    XmToggleButtonSetState(ToggleRecord, record, False);
    XmToggleButtonSetState(ToggleCompare, compare, False);
    XmToggleButtonSetState(ToggleIgnore, ignore, False);
    XmToggleButtonSetState(ToggleFull, full, False);
    XmToggleButtonSetState(ToggleWinDump, dumps, False);
    XmToggleButtonSetState(ToggleTrace, trace, False);

    XmScaleSetValue(ScaleDelay, delay);

    for (i = 0; i < 256; i++)
     	arguments[0] = '\0';

    BuildTestString(False);

}

static void PromptCB(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
    int n, type;
    Arg args[1];
    XmString tcs;

    XtRemoveAllCallbacks(PromptDialog, XmNokCallback);

    type = (int) client_data;

    switch (type) {
	case COLOR: 	tcs = XmStringCreateSimple
				("Enter New Failure Color Please:");
    			XtAddCallback(PromptDialog, XmNokCallback,
				      SimpleCB, COLOR);
			break;
	case EXTRA:	tcs = XmStringCreateSimple
				("Enter any Extra Arguments Please:");
    			XtAddCallback(PromptDialog, XmNokCallback,
				      SimpleCB, EXTRA);
			break;
	default:	printf("xmAuto (warning): Unkown Option in PromptCB\n");
    }

    n = 0;
    XtSetArg(args[n], XmNselectionLabelString, tcs);		n++;
    XtSetValues(PromptDialog, args, n);

    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNvalue, "");	n++;
    XtSetValues(XmSelectionBoxGetChild(PromptDialog, XmDIALOG_TEXT),
		args, n);

    XtManageChild(PromptDialog);

}

static void FileSelectCB(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{

    ResetFSB();

    if ((int) client_data == READ)
    	XtAddCallback(FileSelectBox, XmNokCallback, ReadInCB, NULL);
    else
	if ((int) client_data == WRITE)
		XtAddCallback(FileSelectBox, XmNokCallback, WriteOutCB, NULL);
	else
    		XtAddCallback(FileSelectBox, XmNokCallback, ChangeFileCB, 
			      client_data);

    XtManageChild(FileSelectBox);

}

static void ChangeFileCB(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
    XmFileSelectionBoxCallbackStruct *FSBStruct = 
			((XmFileSelectionBoxCallbackStruct *)call_data);
    String value;
    int type;

    XmStringGetLtoR(FSBStruct->value, XmSTRING_DEFAULT_CHARSET, &value);

    type = (int) client_data;

    switch (type) {
	case INSTRUCT:	strcat(arguments, " -I ");
			break;
	case VISUAL:	strcat(arguments, " -V ");
			break;
	case SCRIPT:	strcat(arguments, " -S ");
			break;
	case OUTPUT:	strcat(arguments, " -O ");
			break;
	default: 	printf
			  ("xmAuto (warning): Unkown Option in ChangeFileCB\n");
			break;
    }

    strcat(arguments, value);
    strcat(arguments, " ");

    BuildTestString(False);
    
}

static void WriteOutCB(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
    XmFileSelectionBoxCallbackStruct *FSBStruct = 
			((XmFileSelectionBoxCallbackStruct *)call_data);
    String value;
    Arg args[2];
    int itemCount, n;
    XmStringTable items;
    XmString tcs;
    FILE *fp;

    XmStringGetLtoR(FSBStruct->value, XmSTRING_DEFAULT_CHARSET, &value);
    if (! (fp = fopen(value, "w"))) 
	printf("xmAuto (warning): cannot open %s for writing\n", value);
    else {
	n = 0;
	XtSetArg(args[n], XmNitems, &items);			n++;
	XtSetArg(args[n], XmNitemCount, &itemCount);		n++;
	XtGetValues(List1, args, n);

	for (n = 0; n < itemCount; n++) {
		XmStringGetLtoR(items[n], XmSTRING_DEFAULT_CHARSET, &value);
		fprintf(fp, "%s\n", value);
	}

	fflush(fp);
	fclose(fp);
    }
}

static void ReadInCB(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
    XmFileSelectionBoxCallbackStruct *FSBStruct = 
			((XmFileSelectionBoxCallbackStruct *)call_data);
    String value;
    String str_value;
    Arg args[2];
    int itemCount, n;
    XmString items[512];
    XmString tcs;
    FILE *fp;

    itemCount = 0;
    XmStringGetLtoR(FSBStruct->value, XmSTRING_DEFAULT_CHARSET, &value);
    if (! (fp = fopen(value, "r"))) 
	printf("xmAuto (warning): cannot open %s for reading\n", value);
    else {
	str_value = (char *)malloc(MAXLINE * sizeof(char));
	while (fgets(str_value, MAXLINE, fp) != NULL) 
		items[itemCount++] = XmStringCreateSimple(str_value);
	XtFree(str_value);

	n = 0;
	XtSetArg(args[n], XmNitems, items);			n++;
	XtSetArg(args[n], XmNitemCount, itemCount);		n++;
	XtSetValues(List1, args, n);

    	XmListSetHorizPos(List1, 200);

	fclose(fp);
    }
}

static void BrowseCB(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
    XmListCallbackStruct *ListStruct = 
			((XmListCallbackStruct *)call_data);
    String value;
    Boolean into_arg = False;
    int n, i;

    list_pos = ListStruct->item_position;

    XmStringGetLtoR(ListStruct->item, XmSTRING_DEFAULT_CHARSET, &value);
    sscanf(value, "%s", CurrentTestString);
    for (i= 0, n = 0 ; n < (strlen(value) - strlen(CurrentTestString)); n++) {
	if (! isspace(value[strlen(CurrentTestString) + n]) && !into_arg) {
		into_arg = True;
		arguments[i++] = value[strlen(CurrentTestString) + n];
	}
	else
		arguments[i++] = value[strlen(CurrentTestString) + n];
    }
    arguments[i] = '\0';

    BuildTestString(False);

}

static void SaveToListCB(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
    char save_string[1024];
    Arg args[1];
    XmString tcs[1];
    int n;

    strcpy(save_string, CurrentTestString);
    strcat(save_string, " ");
    strcat(save_string, arguments);

    tcs[0] = XmStringCreateSimple(save_string);

    XmListReplaceItemsPos(List1, tcs, 1, list_pos);

    XmStringFree(tcs[0]);

    tcs[0] = XmStringCreateSimple("Current Test String:: None");

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs[0]);			n++;
    XtSetValues(Label1, args, n);

    XmStringFree(tcs[0]);

    CurrentTestString[0] = '\0';
    arguments[0] =  '\0';
	
    XmListSetHorizPos(List1, 200);

}

static void QuitCB(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{

    printf("xmAuto: Exiting...\n");
    exit(0);

}
