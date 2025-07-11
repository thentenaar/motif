/* $TOG: UTMPrim1.c /main/7 1999/11/17 12:48:51 jff $ */
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

/*
 *  Defines
 */
#define NUM_FLAVORS 31 
#define NUM_SELECT_TYPES 4
#define FORM_WIDTH  500 
#define FORM_HEIGHT 375
#define LIST_WIDTH  150 
#define LABEL_WIDTH 100 
#define TEXT_WIDTH  150 
#define TEXT_HEIGHT 50 
#define OFFSET      15
#define BUTTON_CNT  4

#define TEXTF_COPYLINK   2
#define TEXTF_PASTELINK  3
#define TEXT_COPYLINK    4
#define TEXT_PASTELINK   5


/*
 *  Declare local character arrays.
 */
static char *flavors[] = {
     "Chocolate Fudge", "French Vanilla", "Wild Strawberry",
     "Maple Walnut", "Rocky Road", "Pistachio",
     "Peanut Butter Cup", "Heath Bar Crunch", "Jamoca",
     "Vanilla Fudge", "Mint Chocolate Chip", "Chocolate Chip",
     "Peach", "Butter Pecan", "Lemon Jubiliee", 
     "Daquari Ice", "Rasberry", "Cookie Crunch",
     "Pralenes 'n Creame", "Lemon 'n Lime", "Caramel Crunch",
     "Cherry Garcia", "White Chocolate", "Coconut",
     "Peppermint", "Marshmellow", "Tootie Fruitie",
     "Watermelon", "Bubble Gum", "English Breakfast",
     "Sky Blue Ice"  };

static char *select_types[] = {
     "Single", "Multiple",
     "Extended", "Browse" };

static char string1[] = 
     "I scream! You scream! ";

static char string2[] =
     "We all scream for ice cream !! ";

static char string3[] =
     "31 Amazing Flavors";

static char translations[] =
  "Ctrl<Key>l:  link-primary()";

/*
 *  Declare external functions.
 */
Widget XmCreatePopupMenu();

/*
 *  Declare local functions.
 */
static void CreateListPrim(), CreateTextPrim(), 
            CreateTextFieldPrim(),
            CreatePBPrim(), CreateToggleBPrim(), CreateLabelPrim(),
            CreateScalePrim();
static char *GetSafeAtom(Display *display, Atom a);
static char *GetStringFrom(XtEnum operation);
static int SIF_ErrorHandler(Display *display, XErrorEvent *event);
static void Convert_info(Widget w, XtPointer call_data);
static void Destination_info(Widget w, XtPointer call_data);

/*
 *  Declare global structures.
 */
typedef struct  _FlagWidgetPair
        {
        int     flag;
        Widget  w;
        }       FlagWidgetPair;

/*
 *  Declare global variables.
 */
int             n, ndx;
Arg             args[25];
XmFontList      fontlist;
XmString        ItemList[NUM_FLAVORS], Label;
Widget          Form1, ListLabel1, List1, 
                PullDown1, OptionPB[NUM_SELECT_TYPES], OptionMenu1, 
                Text1, TextLabel1, 
                TextField1, TextFieldLabel1,
                HorizSeparator1, VertSeparator1, 
                PBLabel1, PB1, ToggleBLabel1, ToggleB1,
                Label1, ScaleLabel1, Scale1,
                Popup1Btn[BUTTON_CNT], Popup1, Popup2Btn[BUTTON_CNT], Popup2,
                Popup3Btn[BUTTON_CNT], Popup3;
FlagWidgetPair  FWP1[BUTTON_CNT], FWP2[BUTTON_CNT], FWP3[BUTTON_CNT];

char            buf[10];


/* 
 *  Callback functions 
 */
static void QuitCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
   exit(0);
}

static void PopupCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
        FlagWidgetPair  *client_fwp;
        XButtonEvent *bev;

        client_fwp = (FlagWidgetPair *)client_data;
        bev = (XButtonEvent *)((XmPushButtonCallbackStruct *)call_data)->event;

        switch (client_fwp->flag)
        {
          case TEXTF_COPYLINK:
               if (!XmTextFieldCopyLink(client_fwp->w, bev->time))
                   printf("XmTextFieldCopyLink failed!\n");
               break;

          case TEXTF_PASTELINK:
               if (!XmTextFieldPasteLink(client_fwp->w))
                   printf("XmTextFieldPasteLink failed!\n");
               break;

          case TEXT_COPYLINK:
               if (!XmTextCopyLink(client_fwp->w, bev->time))
                   printf("XmTextCopyLink failed!\n");
               break;

          case TEXT_PASTELINK:
               if (!XmTextPasteLink(client_fwp->w))
                   printf("XmTextPasteLink failed!\n");
               break;
         }

}

static void SetSelectType(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
   Arg   args[5];
   int   n;


   if ( OptionPB[0] == (Widget)client_data )
   {
      n = 0;
      XtSetArg(args[n], XmNselectionPolicy, XmSINGLE_SELECT); n++;
      XtSetValues( List1, args, n );

      n = 0;
      XtSetArg(args[n], XmNmenuHistory, OptionPB[0]); n++;
      XtSetArg(args[n], XmNwhichButton, 1); n++;
      XtSetValues( OptionMenu1, args, n );

   }

   else if ( OptionPB[1] == (Widget)client_data )
   {
      n = 0;
      XtSetArg(args[n], XmNselectionPolicy, XmMULTIPLE_SELECT); n++;
      XtSetValues( List1, args, n );

      n = 0;
      XtSetArg(args[n], XmNmenuHistory, OptionPB[1]); n++;
      XtSetArg(args[n], XmNwhichButton, 2); n++;
      XtSetValues( OptionMenu1, args, n );
   }

   else if ( OptionPB[2] == (Widget)client_data )
   {
      n = 0;
      XtSetArg(args[n], XmNselectionPolicy, XmEXTENDED_SELECT); n++;
      XtSetValues( List1, args, n );

      n = 0;
      XtSetArg(args[n], XmNmenuHistory, OptionPB[2]); n++;
      XtSetArg(args[n], XmNwhichButton, 3); n++;
      XtSetValues( OptionMenu1, args, n );
   }

   else if ( OptionPB[3] == (Widget)client_data )
   {
      n = 0;
      XtSetArg(args[n], XmNselectionPolicy, XmBROWSE_SELECT); n++;
      XtSetValues( List1, args, n );

      n = 0;
      XtSetArg(args[n], XmNmenuHistory, OptionPB[3]); n++;
      XtSetArg(args[n], XmNwhichButton, 4); n++;
      XtSetValues( OptionMenu1, args, n );
   }

}

static void ListConvertCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
       printf("XmNconvertCallback called for List widget.\n");
       Convert_info(w, call_data);
}

static void ListDestinCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
       printf("XmNdestinationCallback called for List widget.\n");
       Destination_info(w, call_data);
}

static void TextConvertCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
  XmConvertCallbackStruct *cs;
  char *selection_atom_name, *target_atom_name;
  char *tf_string = "<Text Link>";

  cs = (XmConvertCallbackStruct *) call_data;

  selection_atom_name = GetSafeAtom(XtDisplay(w), cs->selection);
  if (selection_atom_name == NULL)
       selection_atom_name = "Illegal atom";

  target_atom_name = GetSafeAtom(XtDisplay(w), cs->target);
  if (target_atom_name == NULL)
       target_atom_name = "Illegal atom";

  printf("XmNconvertCallback called for Text widget.\n");
  printf("        selection         = %s\n", selection_atom_name);
  printf("        conversion target = %s\n", target_atom_name);

  /* support for Clipboard Link transfer */

  if ((cs->selection == XInternAtom(XtDisplay(w), XmSCLIPBOARD, False)) &&
      (cs->target == XInternAtom(XtDisplay(w), XmS_MOTIF_CLIPBOARD_TARGETS, 
				 False) ||
       cs->target == XInternAtom(XtDisplay(w), 
				 XmS_MOTIF_DEFERRED_CLIPBOARD_TARGETS, 
				 False))) {

      if (cs->parm != (XtPointer) 0)
	  printf("        parm              = %s\n", 
		 GetStringFrom((XtEnum) cs->parm));

      if ((cs->target == XInternAtom(XtDisplay(w), 
				     XmS_MOTIF_CLIPBOARD_TARGETS,
				     False)) &&
	  (cs->parm == (XtPointer) XmLINK)) {
      
	  Atom *targargs;

	  targargs = (Atom *) XtMalloc(sizeof(Atom));
	  *targargs = XInternAtom(XtDisplay(w), "MY_LINK_TARGET", False);

	  cs->type = XA_ATOM;
	  cs->format = 32;
	  cs->length = 1;
	  cs->value = (XtPointer) targargs;
	  cs->status = XmCONVERT_DONE;
      }
  } 
  else { 

      if (cs->target == XInternAtom(XtDisplay(w), "MY_LINK_TARGET", False)) {

	  cs->type = (Atom) XA_STRING;
	  cs->format = 8;
	  cs->length = (unsigned long) (strlen(tf_string) + 1);
	  cs->value = (XtPointer) XtNewString(tf_string);
	  cs->status = XmCONVERT_DONE;
      }
  }
  printf("\n"); 

  return;
}

static void 
TextReceiveData(Widget w, XtPointer ignore, 
		XmSelectionCallbackStruct *data)
{
    if (data->target == XInternAtom(XtDisplay(w), "MY_LINK_TARGET", False)) {
	
	if (data->length == 0) {
	    XmTransferDone(data -> transfer_id, XmTRANSFER_DONE_FAIL);
	    return;
        }

	XmTextInsert(w, XmTextGetInsertionPosition(w), data->value);

    }
}


static void TextDestinCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
  XmDestinationCallbackStruct *cs;
  char *selection_atom_name;

  cs = (XmDestinationCallbackStruct *) call_data;

  printf("XmNdestinationCallback called for Text widget.\n");

  selection_atom_name = GetSafeAtom(XtDisplay(w), cs->selection);
  if (selection_atom_name == NULL)
       selection_atom_name = "Illegal atom";

  printf("        selection = %s\n", selection_atom_name);
  printf("        operation = %s\n\n", GetStringFrom(cs->operation));

  if (cs->operation == XmLINK) 
      XmTransferValue(cs -> transfer_id, 
                  XInternAtom(XtDisplay(w), "MY_LINK_TARGET", False),
                  (XtCallbackProc) TextReceiveData, NULL, 0);

}

static void TextFieldConvertCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
  XmConvertCallbackStruct *cs;
  char *selection_atom_name, *target_atom_name;
  char *tf_string = "<TextField Link>";

  cs = (XmConvertCallbackStruct *) call_data;

  selection_atom_name = GetSafeAtom(XtDisplay(w), cs->selection);
  if (selection_atom_name == NULL)
       selection_atom_name = "Illegal atom";

  target_atom_name = GetSafeAtom(XtDisplay(w), cs->target);
  if (target_atom_name == NULL)
       target_atom_name = "Illegal atom";

  printf("XmNconvertCallback called for TextField widget.\n");
  printf("        selection         = %s\n", selection_atom_name);
  printf("        conversion target = %s\n", target_atom_name);

  /* support for Clipboard Link transfer */

  if ((cs->selection == XInternAtom(XtDisplay(w), XmSCLIPBOARD, False)) &&
      (cs->target == XInternAtom(XtDisplay(w), XmS_MOTIF_CLIPBOARD_TARGETS, 
				 False) ||
       cs->target == XInternAtom(XtDisplay(w), 
				 XmS_MOTIF_DEFERRED_CLIPBOARD_TARGETS, 
				 False))) {

      if (cs->parm != (XtPointer) 0)
	  printf("        parm              = %s\n", 
		 GetStringFrom((XtEnum) cs->parm));

      if ((cs->target == XInternAtom(XtDisplay(w), 
				     XmS_MOTIF_CLIPBOARD_TARGETS,
				     False)) &&
	  (cs->parm == (XtPointer) XmLINK)) {
      
	  Atom *targargs;

	  targargs = (Atom *) XtMalloc(sizeof(Atom));
	  *targargs = XInternAtom(XtDisplay(w), "MY_LINK_TARGET", False);

	  cs->type = XA_ATOM;
	  cs->format = 32;
	  cs->length = 1;
	  cs->value = (XtPointer) targargs;
	  cs->status = XmCONVERT_DONE;
      }
  } 
  else { 

      if (cs->target == XInternAtom(XtDisplay(w), "MY_LINK_TARGET", False)) {

	  cs->type = (Atom) XA_STRING;
	  cs->format = 8;
	  cs->length = (unsigned long) (strlen(tf_string) + 1);
	  cs->value = (XtPointer) XtNewString(tf_string);
	  cs->status = XmCONVERT_DONE;
      }
  }
  printf("\n"); 

  return;
}


static void 
TextFieldReceiveData(Widget w, XtPointer ignore, 
		     XmSelectionCallbackStruct *data)
{
    if (data->target == XInternAtom(XtDisplay(w), "MY_LINK_TARGET", False)) {
	
	if (data->length == 0) {
	    XmTransferDone(data -> transfer_id, XmTRANSFER_DONE_FAIL);
	    return;
        }

	XmTextFieldInsert(w, XmTextFieldGetInsertionPosition(w),
			  data->value);

    }
}

static void
TextFieldDestinCB(Widget w, XtPointer client_data, XtPointer call_data)
{

  XmDestinationCallbackStruct *cs;
  char *selection_atom_name;

  cs = (XmDestinationCallbackStruct *) call_data;

  printf("XmNdestinationCallback called for TextField widget.\n");

  selection_atom_name = GetSafeAtom(XtDisplay(w), cs->selection);
  if (selection_atom_name == NULL)
       selection_atom_name = "Illegal atom";

  printf("        selection = %s\n", selection_atom_name);
  printf("        operation = %s\n\n", GetStringFrom(cs->operation));

  if (cs->operation == XmLINK) 
      XmTransferValue(cs -> transfer_id, 
                  XInternAtom(XtDisplay(w), "MY_LINK_TARGET", False),
                  (XtCallbackProc) TextFieldReceiveData, NULL, 0);

}

static void LabelConvertCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
       printf("XmNconvertCallback called for Label widget. \n");
       Convert_info(w, call_data);
}

static void ScaleConvertCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
       printf("XmNconvertCallback called for Scale widget. \n");
       Convert_info(w, call_data);
}

static void PBConvertCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
       printf("XmNconvertCallback called for PushButton widget. \n");
       Convert_info(w, call_data);
}

static void ToggleConvertCB(Widget w, XtPointer client_data,
                                XtPointer call_data)
{
       printf("XmNconvertCallback called for ToggleButton widget. \n");
       Convert_info(w, call_data);
}

static void PostIt(Widget w, XtPointer client_data,
                                XEvent *call_data, Boolean *c)
{
        if (((XButtonEvent *)call_data)->button != Button3)
                return;
        XmMenuPosition((Widget)client_data, (XButtonPressedEvent *)call_data);
        XtManageChild((Widget)client_data);
}

/*
 *  Main
 */
void  main (argc, argv)
    int     argc;
    char  **argv;
{
    XtTranslations   trans_tab;
    int              test_num;
    Widget           QuitButton;

    test_num = 0;

    CommonTestInit(argc, argv);

    if (UserData != NULL) {

       if (strcmp(UserData, "-n") == 0)
	 test_num = 1;
    }

    free(UserData);

    trans_tab = XtParseTranslationTable (translations);

    n = 0;
    XtSetArg(args[n], XmNmarginHeight, 15); n++;
    XtSetArg(args[n], XmNmarginWidth, 15); n++;
    XtSetArg(args[n], XmNwidth, FORM_WIDTH); n++;
    XtSetArg(args[n], XmNheight, FORM_HEIGHT); n++;
    XtSetArg(args[n], XmNresizePolicy, XmRESIZE_NONE ); n++;
    Form1 = XmCreateForm(Shell1, "Form1", args, n);
    XtManageChild (Form1);

    fontlist = CommonGetFontList("8x13bold");

    /*
     *  Creates TextFieldLabel1 and TextField1.
     */
    CreateTextFieldPrim( Form1, &TextFieldLabel1, &TextField1 );

    /*  Set Attachments. */
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM );      n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM );     n++;
    XtSetValues(TextFieldLabel1, args, n);
    XtManageChild(TextFieldLabel1);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
    XtSetArg(args[n], XmNleftWidget, TextFieldLabel1 ); n++;
    XtSetValues(TextField1, args, n);
    XtManageChild(TextField1);

    /*  Create popup menu of functions for TextField widget. */
    Popup1 = XmCreatePopupMenu( TextField1, "Popup1", NULL, 0 );  
    XtAddEventHandler(TextField1, ButtonPressMask, False, PostIt, Popup1);

    ndx = 0;
    n = 0;
    Label = XmStringCreate("TextField Functions",
                                 XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label); n++;
    Popup1Btn[ndx++] = XmCreateLabelGadget(Popup1, "Title1",
                                                args, n);
    XmStringFree(Label);

    n = 0;
    Popup1Btn[ndx++] = XmCreateSeparatorGadget(Popup1, "separator1",
                                                args, n);

    n = 0;
    Label = XmStringCreateLtoR("XmTextFieldCopyLink", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label); n++;
    Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "CopyLink1",
                                            args, n);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("XmTextFieldPasteLink", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label); n++;
    Popup1Btn[ndx++] = XmCreatePushButtonGadget(Popup1, "PasteLink1",
                                            args, n);
    XmStringFree(Label);

    ndx = 2;
    FWP1[ndx-2].flag = TEXTF_COPYLINK; FWP1[ndx-2].w = TextField1;
    XtAddCallback(Popup1Btn[ndx], XmNactivateCallback,
                                    PopupCB, &FWP1[ndx-2]); ndx++;
    FWP1[ndx-2].flag = TEXTF_PASTELINK; FWP1[ndx-2].w = TextField1;
    XtAddCallback(Popup1Btn[ndx], XmNactivateCallback,
                                    PopupCB, &FWP1[ndx-2]); ndx++;

    XtManageChildren(Popup1Btn, ndx);

    /*
     *  Creates TextLabel1 and Text1.
     */
    CreateTextPrim( Form1, &TextLabel1, &Text1 );

    /*  Set Attachments. */
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET );      n++;
    XtSetArg(args[n], XmNtopWidget, TextFieldLabel1 );             n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM );     n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING );    n++;
    XtSetValues(TextLabel1, args, n);
    XtManageChild(TextLabel1);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET );      n++;
    XtSetArg(args[n], XmNtopWidget, TextField1 );                  n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET );     n++;
    XtSetArg(args[n], XmNleftWidget, TextLabel1 );              n++;
    XtSetValues(Text1, args, n);
    XtManageChild(Text1);

    /*  Create popup menu of functions for Text widget. */
    Popup2 = XmCreatePopupMenu( Text1, "Popup2", NULL, 0 );  
    XtAddEventHandler(Text1, ButtonPressMask, False, PostIt, Popup2);

    ndx = 0;
    n = 0;
    Label = XmStringCreate("Text Functions",
                            XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label); n++;
    Popup2Btn[ndx++] = XmCreateLabelGadget(Popup2, "Title2",
                                                args, n);
    XmStringFree(Label);

    n = 0;
    Popup2Btn[ndx++] = XmCreateSeparatorGadget(Popup2, "separator1",
                                                args, n);

    n = 0;
    Label = XmStringCreateLtoR("XmTextCopyLink", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label); n++;
    Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "CopyLink2",
                                            args, n);
    XmStringFree(Label);

    n = 0;
    Label = XmStringCreate("XmTextPasteLink", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, Label); n++;
    Popup2Btn[ndx++] = XmCreatePushButtonGadget(Popup2, "PasteLink2",
                                            args, n);
    XmStringFree(Label);

    ndx = 2;
    FWP2[ndx-2].flag = TEXT_COPYLINK; FWP2[ndx-2].w = Text1;
    XtAddCallback(Popup2Btn[ndx], XmNactivateCallback,
                                    PopupCB, &FWP2[ndx-2]); ndx++;
    FWP2[ndx-2].flag = TEXT_PASTELINK; FWP2[ndx-2].w = Text1;
    XtAddCallback(Popup2Btn[ndx], XmNactivateCallback,
                                    PopupCB, &FWP2[ndx-2]); ndx++;

    XtManageChildren(Popup2Btn, ndx);

    /*
     *  Create vertical separator. 
     */
    n = 0;
/*
    XtSetArg(args[n], XmNx, FORM_WIDTH * 2 / 3 );		       n++;
*/
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM );       n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET );    n++;
    XtSetArg(args[n], XmNleftWidget, Text1 );                  n++;
    XtSetArg(args[n], XmNleftOffset, OFFSET );                 n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM );    n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL );            n++;
    XtSetArg(args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT ); n++;
    XtSetArg(args[n], XmNshadowThickness, 5 );                 n++;
    VertSeparator1 = XmCreateSeparator( Form1, "VertSeparator1", args, n );

    XtManageChild(VertSeparator1);

    /*
     *  Creates ListLabel1, List1, and OptionMenu1 for setting the
     *  selection policy resource of the List1 widget.
     */
    CreateListPrim( Form1, &ListLabel1, &List1, &PullDown1, &OptionMenu1 );

    /*  Set Attachments. */
/*
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET );      n++;
    XtSetArg(args[n], XmNtopWidget, ListLabel1 );               n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM );       n++;
    XtSetArg(args[n], XmNwidth, LIST_WIDTH );			n++;
    XtSetValues(List1, args, n);
    XtManageChild(List1);
*/

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET );       n++;
    XtSetArg(args[n], XmNtopWidget, List1 );                     n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET );      n++;
    XtSetArg(args[n], XmNleftWidget, VertSeparator1 );           n++;
    XtSetValues(OptionMenu1, args, n);
    XtManageChild(OptionMenu1);

    /*
     *  Create horizontal separator. 
     */
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET );     n++;
    XtSetArg(args[n], XmNtopWidget, OptionMenu1 );             n++;
    XtSetArg(args[n], XmNtopOffset, OFFSET );                  n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM );      n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM );     n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL );          n++;
    XtSetArg(args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT ); n++;
    XtSetArg(args[n], XmNshadowThickness, 5 );                 n++;
    HorizSeparator1 = XmCreateSeparator( Form1, "HorizSeparator1", args, n );

    XtManageChild(HorizSeparator1);


    /*
     *  Create PBLabel1 and PB1.
     */
    CreatePBPrim( Form1, &PBLabel1, &PB1 );


    /*  Set Attachments. */
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET );      n++;
    XtSetArg(args[n], XmNtopWidget, HorizSeparator1 );          n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM );       n++;
    XtSetValues(PBLabel1, args, n);
    XtManageChild(PBLabel1);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET );      n++;
    XtSetArg(args[n], XmNtopWidget, PBLabel1 );                 n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM );       n++;
    XtSetValues(PB1, args, n);
    XtManageChild(PB1);


    /*
     *  Create ToggleBLabel1 and ToggleB1.
     */
    CreateToggleBPrim( Form1, &ToggleBLabel1, &ToggleB1 );

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET );      n++;
    XtSetArg(args[n], XmNtopWidget, PB1 );                      n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM );       n++;
    XtSetValues(ToggleBLabel1, args, n);
    XtManageChild(ToggleBLabel1);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET );      n++;
    XtSetArg(args[n], XmNtopWidget, ToggleBLabel1 );            n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM );       n++;
    XtSetValues(ToggleB1, args, n);
    XtManageChild(ToggleB1);


    /*
     *  Create Label1.
     */
    CreateLabelPrim( Form1, &Label1 );

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET );      n++;
    XtSetArg(args[n], XmNtopWidget, HorizSeparator1 );          n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM );      n++;
    XtSetValues(Label1, args, n);
    XtManageChild(Label1);

    /* 
     *  Create ScaleLabel1 and Scale1.
     */
    CreateScalePrim( Form1, &ScaleLabel1, &Scale1 );

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET );      n++;
    XtSetArg(args[n], XmNtopWidget, Label1 );                   n++;
    XtSetArg(args[n], XmNtopOffset, OFFSET );                   n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM );      n++;
    XtSetValues(ScaleLabel1, args, n);
    XtManageChild(ScaleLabel1);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET );      n++;
    XtSetArg(args[n], XmNtopWidget, ScaleLabel1 );              n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM );      n++;
    XtSetValues(Scale1, args, n);
    XtManageChild(Scale1);

    if (test_num == 1) {
       n = 0;
       XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET );      n++;
       XtSetArg(args[n], XmNtopWidget, Scale1 );                   n++;
       XtSetArg(args[n], XmNtopOffset, 20);                        n++;
       XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET );     n++;
       XtSetArg(args[n], XmNleftWidget, VertSeparator1 );          n++;
       XtSetArg(args[n], XmNleftOffset, 50);                       n++;
       QuitButton = XmCreatePushButtonGadget(Form1, "QuitButton", args, n);
       XtManageChild(QuitButton);
       XtAddCallback(QuitButton, XmNactivateCallback, QuitCB, NULL);
    }

    XtOverrideTranslations(TextField1, trans_tab);
    XtOverrideTranslations(Text1, trans_tab);

    XtRealizeWidget(Shell1);

    if (test_num == 0) {

       /*  Test convert and destination callbacks of TextField widget. */
       CommonPause();

       /*  Test convert and destination callbacks of Text widget. */
       CommonPause();

       /*  
	*  Test the CopyLink and PasteLink functions within each of the 
	*  Text widgets.
	*/

       CommonPause();

       /*  
	*  Test the CopyLink and PasteLink functions between each of the 
	*  Text widgets.
	*/
       CommonPause();

       /*  Test the primary-link() action in each of the text widgets. */
       CommonPause();

       /*  Test the secondary selection transfer in each of the text widgets.*/
       CommonPause();

       CommonPause();
       CommonPause();
       CommonPause();
       CommonPause();
    }

    XtAppMainLoop(app_context);
}


static void CreateListPrim( Parent, ListLabel, List, PullDown, OptionMenu )
Widget Parent, *ListLabel, *List, *PullDown, *OptionMenu;
{

    Label = XmStringCreateSimple("List"); 

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM );        n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET );       n++;
    XtSetArg(args[n], XmNleftWidget, VertSeparator1 );       n++;
    XtSetArg(args[n], XmNleftOffset, OFFSET );       n++;
    XtSetArg(args[n], XmNwidth, LIST_WIDTH );			n++;
    XtSetArg(args[n], XmNshadowThickness, 5 );			n++;
    XtSetArg(args[n], XmNlabelString, Label );			n++;
    XtSetArg(args[n], XmNhighlightOnEnter, True );		n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING );    n++;
    XtSetArg(args[n], XmNfontList, fontlist );                  n++;
    *ListLabel = XmCreateLabel( Parent, "ListLabel", args, n );
    XtManageChild(*ListLabel);

    XmStringFree(Label);


    /* convert array of character strings to XmStrings. */
    for (n = 0; n < NUM_FLAVORS; n++)
        ItemList[n] = XmStringLtoRCreate(flavors[n],
                          XmSTRING_DEFAULT_CHARSET);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET );      n++;
    XtSetArg(args[n], XmNtopWidget, *ListLabel );               n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET );     n++;
    XtSetArg(args[n], XmNleftWidget, VertSeparator1 );          n++;
    XtSetArg(args[n], XmNleftOffset, OFFSET );       n++;
    XtSetArg(args[n], XmNwidth, LIST_WIDTH );			n++;
    XtSetArg(args[n], XmNlistSpacing, 5);                       n++;
    XtSetArg(args[n], XmNlistMarginWidth, 2);                   n++;
    XtSetArg(args[n], XmNlistMarginHeight, 2);                  n++;
    XtSetArg(args[n], XmNlistSizePolicy, XmCONSTANT);           n++;
    XtSetArg(args[n], XmNitems, ItemList);                      n++;
    XtSetArg(args[n], XmNitemCount, NUM_FLAVORS);               n++;
    XtSetArg(args[n], XmNvisibleItemCount, 6);                  n++;
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED);  n++;
    XtSetArg(args[n], XmNbackground,
                WhitePixel(display,DefaultScreen(display)));    n++;
    XtSetArg(args[n], XmNforeground,
                BlackPixel(display,DefaultScreen(display)));    n++;
    XtSetArg(args[n], XmNselectionPolicy, XmSINGLE_SELECT );    n++;
    *List = XmCreateScrolledList(Parent, "List", args, n);
    XtManageChild(*List);

    XtAddCallback( *List, XmNconvertCallback, ListConvertCB, NULL );
    XtAddCallback( *List, XmNdestinationCallback, ListDestinCB, NULL );

    *PullDown = XmCreatePulldownMenu(Parent, "PullDown", NULL, 0);

    /* 
     *  Convert pushbutton labels from char * to XmString types. 
     *  Create pushbuttons within the pulldown menu. 
     */   
    for ( n = 0; n < NUM_SELECT_TYPES; n++ )
    {
       Label = XmStringCreateSimple(select_types[n]); 
       XtSetArg(args[0], XmNlabelString, Label ); 
       sprintf(buf, "PB%d", n );
       OptionPB[n] = XmCreatePushButton( PullDown1, buf, args, 1 );

       /*  Set callback functions for each pushbutton. */
       XtAddCallback( OptionPB[n], XmNactivateCallback, 
                      SetSelectType, OptionPB[n]); 
       XmStringFree( Label );
    }

    XtManageChildren(OptionPB, NUM_SELECT_TYPES);


    Label = XmStringCreateSimple("Selection Policy"); 

    n = 0;
    XtSetArg(args[n], XmNwidth, LIST_WIDTH );			n++;
    XtSetArg(args[n], XmNlabelString, Label);         n++;
    XtSetArg(args[n], XmNsubMenuId,   PullDown1);   n++;
/*
    XtSetArg(args[n], XmNmenuHistory, OptionPB[NUM_SELECT_TYPES - 1]); n++;
*/
    XtSetArg(args[n], XmNmenuHistory, OptionPB[0]); n++;
    XtSetArg(args[n], XmNwhichButton, NUM_SELECT_TYPES);    n++;
    XtSetArg(args[n], XmNshadowThickness, 5 );			n++;
    *OptionMenu = XmCreateOptionMenu(Parent, "OptionMenu", args, n);

    XmStringFree(Label);

}

static void CreateTextPrim( Parent, TextLabel, Text )
Widget Parent, *TextLabel, *Text;
{

    Label = XmStringCreateSimple("Text");

    n = 0;

    XtSetArg(args[n], XmNwidth, LABEL_WIDTH );			n++;
    XtSetArg(args[n], XmNheight, TEXT_HEIGHT );			n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING );    n++;
    XtSetArg(args[n], XmNfontList, fontlist );                  n++;
    XtSetArg(args[n], XmNlabelString, Label );			n++;
    *TextLabel = XmCreateLabel(Parent, "TextLabel", args, n );
   
    XmStringFree(Label);

    n = 0;

    XtSetArg(args[n], XmNwidth, TEXT_WIDTH );			n++;
    XtSetArg(args[n], XmNheight, TEXT_HEIGHT );			n++;
    XtSetArg(args[n], XmNrows, 2);			        n++;
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); 		n++;
    XtSetArg(args[n], XmNvalue, string2); 			n++;
    XtSetArg(args[n], XmNwordWrap, True); 			n++;
    *Text = XmCreateText(Parent, "Text", args, n);

    XtAddCallback( *Text, XmNconvertCallback, TextConvertCB, NULL );
    XtAddCallback( *Text, XmNdestinationCallback, TextDestinCB, NULL );

}


static void CreateTextFieldPrim( Parent, TextFieldLabel, TextField )
Widget Parent, *TextFieldLabel, *TextField;
{
    Label = XmStringCreateSimple("TextField");

    n = 0;
    XtSetArg(args[n], XmNwidth, LABEL_WIDTH );			n++;
    XtSetArg(args[n], XmNheight, TEXT_HEIGHT );			n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING );    n++;
    XtSetArg(args[n], XmNlabelString, Label); 			n++;
    XtSetArg(args[n], XmNfontList, fontlist );                  n++;
    *TextFieldLabel = XmCreateLabel(Parent, "TextField", args, n);


    XmStringFree(Label);

    n = 0;
    XtSetArg(args[n], XmNwidth, TEXT_WIDTH ); n++;
    XtSetArg(args[n], XmNheight, TEXT_HEIGHT );			n++;
    XtSetArg(args[n], XmNvalue, string3 ); n++;
    *TextField = XmCreateTextField( Parent, "TextField", args, n );


    XtAddCallback( *TextField, XmNconvertCallback, TextFieldConvertCB, NULL );
    XtAddCallback( *TextField, XmNdestinationCallback, 
                               TextFieldDestinCB, NULL );
}

static void CreatePBPrim( Parent, PBLabel, PB )
Widget Parent, *PBLabel, *PB;
{
    Label = XmStringCreateSimple("PushButton");

    n = 0;
    XtSetArg(args[n], XmNwidth, LIST_WIDTH );			n++;
    XtSetArg(args[n], XmNshadowThickness, 5 );			n++;
    XtSetArg(args[n], XmNlabelString, Label );			n++;
    XtSetArg(args[n], XmNhighlightOnEnter, True );		n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING );    n++;
    XtSetArg(args[n], XmNfontList, fontlist );                  n++;
    *PBLabel = XmCreateLabel( Parent, "PBLabel", args, n );

    XmStringFree(Label);

    Label = XmStringCreateSimple("Click Here !");
    n = 0;
    XtSetArg(args[n], XmNwidth, LIST_WIDTH );			n++;
    XtSetArg(args[n], XmNshadowThickness, 5 );			n++;
    XtSetArg(args[n], XmNlabelString, Label );			n++;
    XtSetArg(args[n], XmNhighlightOnEnter, True );		n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_CENTER );       n++;
    *PB = XmCreatePushButton( Parent, "PB", args, n );

    XmStringFree(Label);
 
    XtAddCallback(*PB, XmNconvertCallback, PBConvertCB, NULL );
}

static void CreateToggleBPrim( Parent, ToggleBLabel, ToggleB )
Widget Parent, *ToggleBLabel, *ToggleB;
{
    Label = XmStringCreateSimple("Toggle Button");

    n = 0;
    XtSetArg(args[n], XmNwidth, LIST_WIDTH );			n++;
    XtSetArg(args[n], XmNshadowThickness, 5 );			n++;
    XtSetArg(args[n], XmNlabelString, Label );			n++;
    XtSetArg(args[n], XmNhighlightOnEnter, True );		n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING );    n++;
    XtSetArg(args[n], XmNfontList, fontlist );                  n++;
    *ToggleBLabel = XmCreateLabel( Parent, "ToggleBLabel", args, n );

    XmStringFree(Label);

    Label = XmStringCreateSimple("<-- Select This !");

    n = 0;
    XtSetArg(args[n], XmNwidth, LIST_WIDTH );			n++;
    XtSetArg(args[n], XmNheight, 30 );   			n++;
    XtSetArg(args[n], XmNshadowThickness, 5 );			n++;
    XtSetArg(args[n], XmNlabelString, Label );			n++;
    XtSetArg(args[n], XmNselectColor,
                WhitePixel(display,DefaultScreen(display)));    n++;
    *ToggleB = XmCreateToggleButton(Parent, "ToggleB", args, n );

    XmStringFree(Label);
    XtAddCallback( *ToggleB, XmNconvertCallback, ToggleConvertCB, NULL ); 
}

static void CreateLabelPrim( Parent, Label1 )
Widget Parent, *Label1;
{
    Label = XmStringCreateLtoR("Label", XmFONTLIST_DEFAULT_TAG);

    n = 0;
    XtSetArg(args[n], XmNwidth, TEXT_WIDTH );                   n++;
    XtSetArg(args[n], XmNheight, 20 );                          n++;
    XtSetArg(args[n], XmNfontList, fontlist );                  n++;
    XtSetArg(args[n], XmNlabelString, Label );                  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING );    n++;
    *Label1 = XmCreateLabel(Parent, "Label1", args, n );

    XmStringFree(Label);

    XtAddCallback(*Label1, XmNconvertCallback, LabelConvertCB, NULL);
}

static void CreateScalePrim( Parent, ScaleLabel, Scale )
Widget Parent, *ScaleLabel, *Scale;
{
    Label = XmStringCreateSimple("Scale");

    n = 0;
    XtSetArg(args[n], XmNwidth, TEXT_WIDTH );                   n++;
    XtSetArg(args[n], XmNheight, 20 );                          n++;
    XtSetArg(args[n], XmNfontList, fontlist );                  n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING );    n++;
    XtSetArg(args[n], XmNlabelString, Label );                  n++;
    *ScaleLabel = XmCreateLabel(Parent, "ScaleLabel", args, n );

    XmStringFree(Label);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL );           n++;
    XtSetArg(args[n], XmNscaleWidth, TEXT_WIDTH );              n++;
    XtSetArg(args[n], XmNscaleHeight, 20 );              n++;
    XtSetArg(args[n], XmNshowValue, True );              n++;
    *Scale = XmCreateScale(Parent, "Scale", args, n );

    XtAddCallback(*Scale, XmNconvertCallback, ScaleConvertCB, NULL);
}

/* Error handler for XGetAtomName */

static int SIF_ErrorFlag;
 
static int 
SIF_ErrorHandler(Display *display, XErrorEvent *event)
{
  SIF_ErrorFlag = event -> type;

  return 0;
}

static char * 
GetSafeAtom(Display *display, Atom a)
{
  XErrorHandler old_Handler;
  char *returnvalue;

  /* Setup error proc and reset error flag */
  old_Handler = XSetErrorHandler((XErrorHandler) SIF_ErrorHandler);
  SIF_ErrorFlag = 0;

  returnvalue = XGetAtomName(display, a);

  XSetErrorHandler(old_Handler);

  if (SIF_ErrorFlag == 0)
    return(returnvalue);
  else
    return(NULL);
}


static char *
GetStringFrom(XtEnum operation)
{
  char *returnvalue;

  switch(operation) {
        case XmMOVE:
           returnvalue = "XmMOVE";
           break;
        case XmCOPY: 
           returnvalue = "XmCOPY";
           break;
        case XmLINK:
           returnvalue = "XmLINK";
           break;
        case XmOTHER:
           returnvalue = "XmOTHER";
           break;
        default:
           returnvalue = "Bad operation";
        }

  return(returnvalue);
}


static void
Convert_info(Widget w, XtPointer call_data)
{
  XmConvertCallbackStruct *cs;
  char *selection_atom_name, *target_atom_name;

  cs = (XmConvertCallbackStruct *) call_data;

  selection_atom_name = GetSafeAtom(XtDisplay(w), cs->selection);
  if (selection_atom_name == NULL)
       selection_atom_name = "Illegal atom";

  target_atom_name = GetSafeAtom(XtDisplay(w), cs->target);
  if (target_atom_name == NULL)
       target_atom_name = "Illegal atom";

  printf("        selection         = %s\n", selection_atom_name);
  printf("        conversion target = %s\n", target_atom_name);

  if (cs->selection == XInternAtom(XtDisplay(w), XmSCLIPBOARD, False)) 
      if (cs->parm != (XtPointer) 0)
	  printf("        parm              = %s\n", 
		 GetStringFrom((XtEnum) cs->parm));

  printf("\n"); 

}

static void
Destination_info(Widget w, XtPointer call_data)
{
  XmDestinationCallbackStruct *cs;
  char *selection_atom_name;

  cs = (XmDestinationCallbackStruct *) call_data;

  selection_atom_name = GetSafeAtom(XtDisplay(w), cs->selection);
  if (selection_atom_name == NULL)
       selection_atom_name = "Illegal atom";

  printf("        selection = %s\n", selection_atom_name);
  printf("        operation = %s\n\n", GetStringFrom(cs->operation));

}
