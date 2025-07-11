/* $XConsortium: InputMethod2.c /main/5 1995/07/15 21:00:40 drk $ */
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
 * 
 */
/*
 * HISTORY
 */

#include <testlib.h>

#include <X11/bitmaps/star>
#include <X11/bitmaps/xlogo16>

/*
 *  Declare local functions.
 */

static Boolean GetSupportedStyle(XIMStyle supported_style, unsigned short i);
static void PrintSupportedStyle(XIMStyle supported_style);
static void CreatePanel(void );
static Boolean FindIfSupported(int style);
static Widget CreatePushButton(Widget parent, char *label);
static Pixmap MakePixmap(Widget toplevel, char bits[], int width, int height);
static void BackgroundCB(Widget w, XtPointer client_data, XtPointer call_data);
static void BackPixmapCB(Widget w, XtPointer client_data, XtPointer call_data);
static void ForegroundCB(Widget w, XtPointer client_data, XtPointer call_data);
static void ChoiceCB(Widget w, XtPointer client_data, XtPointer call_data);

/*
 *  Declare global variables.
 */
#define MY_OFFSET 30
#define OVERTHESPOT 1
#define OFFTHESPOT 2
#define ROOT 3

Widget Text1, Text2, TextField1;
Widget current_text;
unsigned short IM_style_count;
XIMStyle IM_style[10];
int InputStyle[10];
int test_style;
XmFontList small_fontlist, medium_fontlist, big_fontlist;
Pixmap star_pixmap, xlogo_pixmap;

typedef struct {
    String small_font, medium_font, big_font;
} ApplicationData, *ApplicationDataPtr;

static XtResource resources[] = {
   {"small_font", "Small_font", XtRString, sizeof (String),
    XtOffset(ApplicationDataPtr, small_font), XtRString, "*medium*-14-*"},
   {"medium_font", "Medium_font", XtRString, sizeof (String),
    XtOffset(ApplicationDataPtr, medium_font), XtRString, "*medium*-16-*,*18*"},
   {"big_font", "Big_font", XtRString, sizeof (String),
    XtOffset(ApplicationDataPtr, big_font), XtRString, "*medium*-24-*"},
};

static char *BackgroundColor[4] = {
    "black", 
    "yellow", 
    "green", 
    "blue"
};

static char *ForegroundColor[3] = {
    "white", 
    "red", 
    "grey"
};



/*
 *  Main
 */
void  main (argc, argv)
    int     argc;
    char  **argv;
{
    Widget           Form1;
    Arg              args[20];
    int              n;
    char             TestName[20], test_char;
    XIM              XIM_object;
    XIMStyles        *XIM_styles;
    XIC              XIC_Text1, XIC_Text2, XIC_TextField1;
    ApplicationData  my_data;

    TestName[0] = '\0';
    test_style = OVERTHESPOT;
    test_char = 'a';

    CommonTestI18NInit(argc, argv, NULL, NULL);

    if (UserData != NULL) {

	n = 0;

	if (strcmp(UserData, "a") == 0) {
               test_style = OVERTHESPOT;
	       XtSetArg(args[n], XmNpreeditType, "OverTheSpot"); n++;
	} else if (strcmp(UserData, "b") == 0) {
               test_style = OFFTHESPOT;
	       XtSetArg(args[n], XmNpreeditType, "OffTheSpot"); n++;
        } else if (strcmp(UserData, "c") == 0) {
               test_style = ROOT;
	       XtSetArg(args[n], XmNpreeditType, "Root"); n++;
        }
	
	XtSetArg(args[n], XmNinputPolicy, XmPER_WIDGET); n++;
	XtSetValues(Shell1, args, n);
       
	test_char = *UserData;

	free(UserData);
    }
    
    sprintf(TestName, "InputMethod2%c", test_char);

    CommonPause();

    /* Testing XmImGetXIM */

    XIM_object = (XIM) 0;
    XIM_object = XmImGetXIM(Shell1);

    if (XIM_object == (XIM) 0) {
	printf("\nCannot open input method.\n");
	exit(0);
    }
    else
	printf("\nInput method opened successfully for locale %s.\n\n",
	       XLocaleOfIM(XIM_object));
    
    CommonPause();

    /* Query the input method to see what input styles it supports. */

    XIM_styles = (XIMStyles *) 0;
    IM_style_count = (unsigned short) 0;

    if (XGetIMValues(XIM_object, XNQueryInputStyle, &XIM_styles, NULL)!=NULL) {
	printf("Query on input styles failed.\n\n");
	printf("No reason to continue this test.\n");
	exit(0);
    }

    else {
	unsigned short i;
	Boolean proceed = FALSE;

	printf("Number of supported input styles: %d\n\n", 
	       XIM_styles->count_styles);

	IM_style_count = (unsigned short) 0;
	for (i = 0; i < XIM_styles->count_styles; i++) 
	    proceed = proceed || 
		      GetSupportedStyle(XIM_styles->supported_styles[i], i);
	
	XFree(XIM_styles);
	if (!proceed) {
	    printf("No known styles supported by this input method.\n");
	    printf("No reason to continue this test.\n");
	    exit(0);
        }

	if (FindIfSupported(test_style))
	    printf("Input Method and Application support the same input style.\n");
	else {
	    printf("Input Method and Application don't support the same input style.\n");
	    printf("No reason to continue this test.\n");
	    exit(0);	
        }    
    }

    CommonPause();

    /* get resources and do conversions */

    XtGetApplicationResources(Shell1, &my_data, resources,
			      XtNumber(resources), NULL, 0);

    small_fontlist = XmFontListAppendEntry(NULL, 
			    XmFontListEntryLoad(XtDisplay(Shell1), 
						my_data.small_font,
						XmFONT_IS_FONTSET, 
						XmFONTLIST_DEFAULT_TAG));
/*
    medium_fontlist = XmFontListAppendEntry(NULL, 
			    XmFontListEntryLoad(XtDisplay(Shell1),
						my_data.medium_font,
						XmFONT_IS_FONTSET, 
						XmFONTLIST_DEFAULT_TAG));

    big_fontlist = XmFontListAppendEntry(NULL, 
			    XmFontListEntryLoad(XtDisplay(Shell1),
						my_data.big_font,
						XmFONT_IS_FONTSET, 
						XmFONTLIST_DEFAULT_TAG));
*/
    n = 0;
    XtSetArg(args[n], XmNmarginHeight, 15); n++;
    XtSetArg(args[n], XmNmarginWidth, 15); n++;
    Form1 = XmCreateForm(Shell1, TestName, args, n);
    XtManageChild (Form1);

    /*
     *  Creates Text1 and Text2.
     */

    n = 0;

    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM );        n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM );       n++;
    XtSetArg(args[n], XmNcolumns, 15);                          n++;
    XtSetArg(args[n], XmNrows, 5);			        n++;
    XtSetArg(args[n], XmNscrollVertical, True);                 n++;
    XtSetArg(args[n], XmNscrollHorizontal, True);               n++;
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); 		n++;
    XtSetArg(args[n], XmNwordWrap, True); 			n++;
    XtSetArg(args[n], XmNvalue, "Testing");                     n++;
    XtSetArg(args[n], XmNfontList, small_fontlist);             n++; 
    Text1 = XmCreateScrolledText(Form1, "Text1", args, n);

    XtManageChild(Text1);

    n = 0;

    XtSetArg(args[n], XmNcolumns, 20);                          n++;
    XtSetArg(args[n], XmNrows, 5);			        n++;
    XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); 		n++;
    XtSetArg(args[n], XmNwordWrap, True); 			n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM );        n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET );     n++;
    XtSetArg(args[n], XmNleftWidget, Text1);                    n++;
    XtSetArg(args[n], XmNleftOffset, MY_OFFSET );               n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM );      n++;
    XtSetArg(args[n], XmNvalue, "Input Method with ");          n++;
/*    XtSetArg(args[n], XmNfontList, medium_fontlist);            n++;  */
    XtSetArg(args[n], XmNfontList, small_fontlist);             n++; 
    Text2 = XmCreateText(Form1, "Text2", args, n);

    XtManageChild(Text2);

    /*
     *  Creates TextField1.
     */

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET );      n++;
    XtSetArg(args[n], XmNtopWidget, Text1);                     n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM );       n++;
    XtSetArg(args[n], XmNcolumns, 40);                          n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM );      n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM );     n++;
    XtSetArg(args[n], XmNvalue, "per widget Input Context.");   n++;
/*    XtSetArg(args[n], XmNfontList, big_fontlist);               n++;  */
    XtSetArg(args[n], XmNfontList, small_fontlist);             n++; 
    TextField1 = XmCreateTextField(Form1, "TextField1", args, n);

    XtManageChild(TextField1);

    XtRealizeWidget(Shell1);

    /* Test Input Context with different attributes */

    CreatePanel();
    
    CommonPause();
    CommonPause();

    /* Test that we really have different Input Context */

    XIC_Text1 = XmImGetXIC(Text1, XmINHERIT_POLICY, NULL, (Cardinal) 0);
    XIC_Text2 = XmImGetXIC(Text2, XmINHERIT_POLICY, NULL, (Cardinal) 0);
    XIC_TextField1 = XmImGetXIC(TextField1, XmINHERIT_POLICY, NULL, 
				(Cardinal) 0);

    if ((XIC_Text1 != XIC_Text2) && (XIC_Text1 != XIC_TextField1)
	&& (XIC_Text2 != XIC_TextField1))
	printf("\nEach Text widget has its own Input Context\n");
    else
        printf("\nERROR: Text widgets are sharing the Input Context\n");

    CommonPause();

    /* Text XmImUnregister */

    XmImUnregister(Text1);
    CommonPause();

    CommonPause();
 
    XtAppMainLoop(app_context);
}

static Boolean
GetSupportedStyle(XIMStyle supported_style, unsigned short i)
{
    Boolean return_status = FALSE;

    printf("Style %d: ", i + 1);

    if ((supported_style & XIMPreeditPosition) &&
	(supported_style & (XIMStatusArea|XIMStatusNothing|XIMStatusNone))) {
	printf("Over-the-spot\n");
	InputStyle[IM_style_count] = OVERTHESPOT;
	IM_style[IM_style_count] = supported_style;
	return_status = TRUE;
	IM_style_count++;
    }

    else if ((supported_style & XIMPreeditArea) &&
	(supported_style & (XIMStatusArea|XIMStatusNothing|XIMStatusNone))) {
	printf("Off-the-spot\n");
	InputStyle[IM_style_count] = OFFTHESPOT;
	IM_style[IM_style_count] = supported_style;
	return_status = TRUE;
	IM_style_count++;
    }

    else if ((supported_style & XIMPreeditNothing) &&
	(supported_style & (XIMStatusNothing|XIMStatusNone))) {
	printf("Root-window\n");
	InputStyle[IM_style_count] = ROOT;
	IM_style[IM_style_count] = supported_style;
	return_status = TRUE;
	IM_style_count++;
    }

    else if ((supported_style & XIMPreeditNone) &&
	(supported_style & XIMStatusNone)) {
	printf("None\n"); 
	return_status = FALSE;    
    }
    else {
	printf("Unknown composed of: ");
	PrintSupportedStyle(supported_style);
	return_status = FALSE;
    }

    return (return_status);
}


static void
PrintSupportedStyle(XIMStyle supported_style)
{
    if (supported_style & XIMPreeditArea) 
	printf("XIMPreeditArea ");
    if (supported_style & XIMPreeditCallbacks) 
	printf("XIMPreeditCallbacks ");
    if (supported_style & XIMPreeditPosition) 
	printf("XIMPreeditPosition ");
    if (supported_style & XIMPreeditNothing) 
	printf("XIMPreeditNothing ");
    if (supported_style & XIMPreeditNone) 
	printf("XIMPreeditNone ");

    if (supported_style & XIMStatusArea) 
	printf("XIMStatusArea ");
    if (supported_style & XIMStatusCallbacks) 
	printf("XIMStatusCallbacks ");
    if (supported_style & XIMStatusNothing) 
	printf("XIMStatusNothing ");
    if (supported_style & XIMStatusNone) 
	printf("XIMStatusNone ");

    return;
}

static void 
CreatePanel(void )
{
    Arg		args[15];
    Cardinal	n;
    XmString	tcs;
    Widget	FormDialog;
    Widget      InputStyleLabel;
    Widget      Background_Pulldown, Background_Option;
    Widget      Black, Yellow, Green, Blue;
    Widget      BackPixmap_Pulldown, BackPixmap_Option;
    Widget      Pix_star, Pix_xlogo16;
    Widget      Foreground_Pulldown, Foreground_Option;
    Widget      White, Red, Grey;
    Widget      Separator;
    Widget      ChoiceLabel, ChoiceRadioBox, STextTB, TextTB, TextFTB;

    n = 0;
    XtSetArg(args[n], XmNhorizontalSpacing, 5); n++;
    XtSetArg(args[n], XmNverticalSpacing, 5); n++;    
    FormDialog = XmCreateFormDialog(Shell1, "FormDialog", args, n);
    XtManageChild(FormDialog);

    if (test_style == OVERTHESPOT)
	tcs = XmStringCreateLtoR("Input Style: Over The Spot", 
				 XmFONTLIST_DEFAULT_TAG);
    else if (test_style == OFFTHESPOT)
	tcs = XmStringCreateLtoR("Input Style: Off The Spot", 
				 XmFONTLIST_DEFAULT_TAG);
    else if (test_style == ROOT)
	tcs = XmStringCreateLtoR("Input Style: Root", 
				 XmFONTLIST_DEFAULT_TAG);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    InputStyleLabel = XmCreateLabelGadget(FormDialog, "InputStyleLabel", 
					  args, n);
    XtManageChild(InputStyleLabel);
    XmStringFree(tcs);

    n = 0;
    Background_Pulldown = XmCreatePulldownMenu(FormDialog, 
					       "Background_Pulldown",
					       args, n);
    Black = CreatePushButton(Background_Pulldown, "Black");
    Yellow = CreatePushButton(Background_Pulldown, "Yellow");
    Green = CreatePushButton(Background_Pulldown, "Green");
    Blue = CreatePushButton(Background_Pulldown, "Blue");

    XtAddCallback(Black, XmNactivateCallback, BackgroundCB, 
		  (XtPointer) BackgroundColor[0]);
    XtAddCallback(Yellow, XmNactivateCallback, BackgroundCB, 
		  (XtPointer) BackgroundColor[1]);
    XtAddCallback(Green, XmNactivateCallback, BackgroundCB, 
		  (XtPointer) BackgroundColor[2]);
    XtAddCallback(Blue, XmNactivateCallback, BackgroundCB, 
		  (XtPointer) BackgroundColor[3]);

    tcs = XmStringCreateLtoR("Background ", XmFONTLIST_DEFAULT_TAG);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, InputStyleLabel); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNmenuHistory, Black); n++;    
    XtSetArg(args[n], XmNsubMenuId, Background_Pulldown); n++;
    Background_Option = XmCreateOptionMenu(FormDialog, "Background_Option",
					   args, n);    
    XtManageChild(Background_Option);
    XmStringFree(tcs);

    n = 0;
    BackPixmap_Pulldown = XmCreatePulldownMenu(FormDialog, 
					       "BackPixmap_Pulldown",
					       args, n);
    star_pixmap = MakePixmap(Shell1, (char *) star_bits,
			     star_width, star_height);
    xlogo_pixmap = MakePixmap(Shell1, (char *) xlogo16_bits, 
			      xlogo16_width, xlogo16_height);

    n = 0;
    XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
    XtSetArg(args[n], XmNlabelPixmap, star_pixmap); n++;
    Pix_star = XmCreatePushButton(BackPixmap_Pulldown, "Pix_star", args, n);
    XtManageChild(Pix_star);

    XtAddCallback(Pix_star, XmNactivateCallback, BackPixmapCB, 
		  (XtPointer) &star_pixmap);

    n = 0;
    XtSetArg(args[n], XmNlabelType, XmPIXMAP); n++;
    XtSetArg(args[n], XmNlabelPixmap, xlogo_pixmap); n++;
    Pix_xlogo16 = XmCreatePushButton(BackPixmap_Pulldown, "Pix_xlogo16", 
				     args, n);
    XtManageChild(Pix_xlogo16);

    XtAddCallback(Pix_xlogo16, XmNactivateCallback, BackPixmapCB, 
		  (XtPointer) &xlogo_pixmap);

    tcs = XmStringCreateLtoR("Background Pixmap ", XmFONTLIST_DEFAULT_TAG);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, Background_Option); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNsubMenuId, BackPixmap_Pulldown); n++;
    BackPixmap_Option = XmCreateOptionMenu(FormDialog, "BackPixmap_Option",
					args, n);    
    XtManageChild(BackPixmap_Option);
    XmStringFree(tcs);

    n = 0;
    Foreground_Pulldown = XmCreatePulldownMenu(FormDialog, 
					       "Foreground_Pulldown",
					       args, n);
    White = CreatePushButton(Foreground_Pulldown, "White");
    Red = CreatePushButton(Foreground_Pulldown, "Red");
    Grey = CreatePushButton(Foreground_Pulldown, "Grey");

    XtAddCallback(White, XmNactivateCallback, ForegroundCB, 
		  (XtPointer) ForegroundColor[0]);
    XtAddCallback(Red, XmNactivateCallback, ForegroundCB, 
		  (XtPointer) ForegroundColor[1]);
    XtAddCallback(Grey, XmNactivateCallback, ForegroundCB, 
		  (XtPointer) ForegroundColor[2]);

    tcs = XmStringCreateLtoR("Foreground ", XmFONTLIST_DEFAULT_TAG);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, BackPixmap_Option); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNmenuHistory, White); n++;    
    XtSetArg(args[n], XmNsubMenuId, Foreground_Pulldown); n++;
    Foreground_Option = XmCreateOptionMenu(FormDialog, "Foreground_Option",
					   args, n);    
    XtManageChild(Foreground_Option);
    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, Foreground_Option);   n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    Separator = XmCreateSeparatorGadget(FormDialog, "Separator", args, n);
    XtManageChild(Separator);

    tcs = XmStringCreateLtoR("Apply to ", XmFONTLIST_DEFAULT_TAG);

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, Separator);           n++;
    XtSetArg(args[n], XmNtopOffset, 20);                  n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
    XtSetArg(args[n], XmNlabelString, tcs);               n++;
    ChoiceLabel = XmCreateLabelGadget(FormDialog, "ChoiceLabel", args, n);

    XtManageChild(ChoiceLabel);
    XmStringFree(tcs);
    
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg(args[n], XmNtopWidget, Separator);            n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNleftWidget,  ChoiceLabel);        n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
    XtSetArg(args[n], XmNpacking, XmPACK_TIGHT);           n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL);       n++;
    XtSetArg(args[n], XmNborderWidth, 2);                  n++;
    ChoiceRadioBox = XmCreateRadioBox(FormDialog, "ChoiceRadioBox", args, n);
    XtManageChild(ChoiceRadioBox);
 
    tcs = XmStringCreateLtoR("Scrolled Text", XmFONTLIST_DEFAULT_TAG);

    n = 0;
    XtSetArg(args[n], XmNset, XmSET); n++;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    current_text = Text1;
    STextTB = XmCreateToggleButtonGadget(ChoiceRadioBox, "STextTB", args, n);

    XtManageChild(STextTB);
    XmStringFree(tcs);
    XtAddCallback(STextTB, XmNvalueChangedCallback, ChoiceCB, 
		  (XtPointer) &Text1);

    tcs = XmStringCreateLtoR("Text", XmFONTLIST_DEFAULT_TAG);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    TextTB = XmCreateToggleButtonGadget(ChoiceRadioBox, "TextTB", args, n);

    XtManageChild(TextTB);
    XmStringFree(tcs);
    XtAddCallback(TextTB, XmNvalueChangedCallback, ChoiceCB, 
		  (XtPointer) &Text2);


    tcs = XmStringCreateLtoR("Text Field", XmFONTLIST_DEFAULT_TAG);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    TextFTB = XmCreateToggleButtonGadget(ChoiceRadioBox, "TextFTB", args, n);

    XtManageChild(TextFTB);
    XmStringFree(tcs);
    XtAddCallback(TextFTB, XmNvalueChangedCallback, ChoiceCB, 
		  (XtPointer) &TextField1);

    return;
}


static Boolean
FindIfSupported(int style)
{
    unsigned short i;

    for (i = 0; i < IM_style_count; i++)
	if (InputStyle[i] == style)
	    return(TRUE);

    return(FALSE);
}

static Widget
CreatePushButton(Widget parent, char *label)
{

    Widget          widget;
    register int    n;
    Arg             args[MAX_ARGS];
    XmString        tcs;

    tcs = XmStringCreateLtoR(label, XmFONTLIST_DEFAULT_TAG);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    widget = XmCreatePushButtonGadget(parent, label, args, n);
    XtManageChild(widget);

    XmStringFree(tcs);

    return(widget);

}
static Pixmap
MakePixmap(Widget toplevel, char bits[], int width, int height)
{
  return (Pixmap)XCreatePixmapFromBitmapData(XtDisplay(toplevel),
			   DefaultRootWindow(XtDisplay(toplevel)),
                           bits,
                           width,
                           height,
                           BlackPixelOfScreen(XtScreen(toplevel)),
                           WhitePixelOfScreen(XtScreen(toplevel)),
                           DefaultDepthOfScreen(XtScreen(toplevel)));
}

static void 
BackgroundCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Arg  args[1];

    XtSetArg(args[0], XmNbackground, CommonGetColor((char *) client_data));
    XtSetValues(current_text, args, 1);

    return;
}

static void 
BackPixmapCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Arg  args[1];

    XtSetArg(args[0], XmNbackgroundPixmap, *((Pixmap *) client_data));
    XtSetValues(current_text, args, 1);

    return;
}

static void 
ForegroundCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Arg  args[1];

    XtSetArg(args[0], XmNforeground, CommonGetColor((char *) client_data));
    XtSetValues(current_text, args, 1);

    return;
}

static void 
ChoiceCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Arg  args[1];
    unsigned char value;
    
    XtSetArg(args[0], XmNset, &value);
    XtGetValues(w, args, 1);

    if (value == XmSET) 
	current_text = * (Widget *) client_data;

    return;
}
