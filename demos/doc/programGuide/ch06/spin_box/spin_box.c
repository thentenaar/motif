/* $XConsortium: spin_box.c /main/5 1995/07/15 20:39:47 drk $ */
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
#include <stdio.h>
#include <stdlib.h>
/* #include <X11/Xos.h>  */
#include <Xm/XmAll.h>

#define NUM_MONTHS	12
#define NUM_LETTERS	10
#define Y_OFFSET	50

#define SPIN1_MIN	 0
#define SPIN1_MAX	 6

XtAppContext app_context;

/*
 * Externaly declared functions
 */
void CreateMenus(Widget);
void setMonths(void);
void setLetters(void);
void printReason(int reason);
void setMaxDay(Widget monthWidget, int nextMM);
void changedSpin0(Widget w, XtPointer client, XtPointer call);
void changedSpin1(Widget w, XtPointer client, XtPointer call);
void changedSpin6(Widget w, XtPointer client, XtPointer call);
void modifySpin1(Widget w, XtPointer client, XtPointer call);
void modifySpin4(Widget w, XtPointer client, XtPointer call);
void demoExit(Widget w, XtPointer client, XtPointer call);
int  main(int argc, char **argv);
void CreateSpinBoxes(Widget parent_of_spin_box);

int thisMM = 3;
int thisDD = 27;
int thisYY = 94;

static char *months[NUM_MONTHS] =
  {
  "January", "February", "March", "April", "May", "June",
  "July", "August", "September", "October", "November", "December"
  };

static char *letters[NUM_LETTERS] =
  {
  "a", "b", "c", "d", "e", "f", "g", "f", "h", "i"
  };

XmString monthValues[NUM_MONTHS];
XmString letterValues[NUM_LETTERS];

/*****		SpinBox Widgets		*****/

Widget spin0, spin0_text, spin1, spin1_text, spin2, spin2_text;
Widget spin3, spin3_text, spin4, spin4_text, spin5, spin5_text;

Widget spin6, spin6_text1, spin6_deco1, spin6_text2, spin6_deco2;
Widget spin6_text3;
Widget top_level;


#define APP_CLASS "XmdSpinBox"

static String fallbacks[] = {
"spinbox.allowShellResize: True",
"spinbox*fontList: fixed",
NULL
};


/* main() */
int
main (int argc, char **argv)
{
   Widget main_window;

   XtSetLanguageProc(NULL, NULL, NULL);
   top_level = XtVaOpenApplication(&app_context, APP_CLASS,
                                NULL, 0, &argc, argv,
                                fallbacks, sessionShellWidgetClass,
                                NULL);
   main_window = XtVaCreateManagedWidget("main_window",
                                xmMainWindowWidgetClass, top_level,
                                NULL);

   CreateMenus(main_window);
   CreateSpinBoxes(main_window);

   XtRealizeWidget(top_level);
   XtAppMainLoop(app_context);
   exit(0);
}



void
CreateSpinBoxes(Widget parent_of_spin_box)
{
Cardinal     n;
Position     nextY;
Arg	     argList[25];
Widget       parent;
XmString     decoString;

    /*****  Set Y position for next SpinBox  *****/
    nextY = 10;

    /*****  Create BulletinBoard parent for SpinBox widgets  *****/
    n = 0;
    XtSetArg(argList[n], XmNwidth, 250); n++;
    XtSetArg(argList[n], XmNheight, 400); n++;

    parent = XmCreateBulletinBoard(parent_of_spin_box, "Parent",
                                   argList, n);
    XtManageChild(parent);

    /****************************************************************/

    /*****
     *****  Create SpinBox spin0
     *****
     *****  Choices:  months of the year
     *****
     *****  Callbacks:
     *****    - changedSpin0 Prints string desription of callback
     *****        reason, and prints 'new' year when boundary
     *****        is crossed (January to December, decrement year;
     *****        December to January, increment year);
     *****/

    /*****  Create SpinBox parent  *****/
    n = 0;
    XtSetArg(argList[n], XmNy, nextY); n++;

    spin0 = XmCreateSpinBox(parent, "spin0", argList, n);

    /*****  Increment Y position for next SpinBox  *****/
    nextY += Y_OFFSET;

    /*****  Create XmString array of month names  *****/
    setMonths();

    /*****  Create TextField child  *****/
    n = 0;

    XtSetArg(argList[n], XmNvalues, monthValues); n++;
    XtSetArg(argList[n], XmNnumValues, 12); n++;
    XtSetArg(argList[n], XmNspinBoxChildType, XmSTRING); n++;
    XtSetArg(argList[n], XmNselectionPolicy, XmSINGLE_SELECT); n++;
    XtSetArg(argList[n], XmNeditable, False); n++;

    spin0_text = XmCreateTextField(spin0, "spin0_text", argList, n);

    /*****  Manage SpinBox  *****/
    XtManageChild(spin0);

    /*****  Call changedSpin0 AFTER displayed value has changed  *****/
    XtAddCallback(spin0, XmNvalueChangedCallback, changedSpin0, (XtPointer) 0);

    /*****  Manage SpinBox child  *****/
    XtManageChild(spin0_text);

    /*****  End of SpinBox spin0  *****/



    /*****
     *****  Create SpinBox spin1
     *****
     *****  Choices:  letters of the alphabet
     *****
     *****  Callbacks:
     *****    - modifySpin1 Callback prevents spinning beyond
     *****        positions 0 (letter a) and 6 (letter g)
     *****
     *****    - changedSpin1 Sets arrow sensitivity to inhibit wrapping
     *****
     *****/

    /*****  Create SpinBox parent  *****/
    n = 0;
    XtSetArg(argList[n], XmNy, nextY); n++;
    XtSetArg(argList[n], XmNdefaultArrowSensitivity,
                         XmARROWS_DECREMENT_SENSITIVE); n++;
    spin1 = XmCreateSpinBox(parent, "spin1", argList, n);


    /*****  Increment Y position for next SpinBox  *****/
    nextY += Y_OFFSET;

    /*****  Create XmString array of single letters  *****/
    setLetters();

    /*****  Create SpinBox child  *****/
    n = 0;
    XtSetArg(argList[n], XmNvalues, letterValues); n++;
    XtSetArg(argList[n], XmNnumValues, NUM_LETTERS); n++;
    XtSetArg(argList[n], XmNspinBoxChildType, XmSTRING); n++;
    XtSetArg(argList[n], XmNarrowSensitivity,
                         XmARROWS_INCREMENT_SENSITIVE); n++;
    XtSetArg(argList[n], XmNeditable, False); n++;

    spin1_text = XmCreateTextField(spin1, "spin0_text", argList, n);


    /*****  Manage SpinBox  *****/
    XtManageChild(spin1);

    /*****  Call modifySpin1 BEFORE displayed value is changed  *****/
    XtAddCallback(spin1, XmNmodifyVerifyCallback, modifySpin1, (XtPointer) 0);

    /*****  Call changedSpin1 AFTER displayed value has changed  *****/
    XtAddCallback(spin1, XmNvalueChangedCallback, changedSpin1, (XtPointer) 0);

    /*****  Manage SpinBox child  *****/
    XtManageChild(spin1_text);

    /*****  End of SpinBox spin1  *****/



    /*****
     *****  Create SpinBox spin2
     *****
     *****  Choices:  0.0000 to 0.0010 by .0002
     *****
     *****  Initial Position:  4 (displays 0.0008)
     *****
     *****  Callbacks:  none
     *****
     *****/

    /*****  Create SpinBox parent  *****/
    n = 0;
    XtSetArg(argList[n], XmNy, nextY); n++;

    spin2 = XmCreateSpinBox( parent, "spin2", argList, n );

    /*****  Increment Y position for next SpinBox  *****/
    nextY +=  Y_OFFSET;

    n = 0;
    XtSetArg(argList[n], XmNposition, 4); n++;
    XtSetArg(argList[n], XmNdecimalPoints, 4); n++;
    XtSetArg(argList[n], XmNincrementValue, 2); n++;
    XtSetArg(argList[n], XmNspinBoxChildType, XmNUMERIC); n++;
    XtSetArg(argList[n], XmNeditable, False); n++;

    spin2_text = XmCreateTextField( spin2,
				    "spin2_text",
				    argList,
				    n );


    /*****  Manage SpinBox  *****/
    XtManageChild(spin2);

    /*****  Manage SpinBox child  *****/
    XtManageChild(spin2_text);

    /*****  End of SpinBox spin2  *****/



    /*****
     *****  Create SpinBox spin3
     *****
     *****  Choices:  0.000 to 121.500 by 0.002
     *****
     *****  Callbacks:  none
     *****
     *****/

    /*****  Create SpinBox parent  *****/
    n = 0;
    XtSetArg(argList[n], XmNy, nextY); n++;
    XtSetArg(argList[n], XmNrepeatDelay, 250); n++;
    XtSetArg(argList[n], XmNarrowLayout, XmARROWS_BEGINNING); n++;

    spin3 = XmCreateSpinBox( parent,
			     "spin3",
			     argList,
			     n );


    /*****  Increment Y position for next SpinBox  *****/
    nextY +=  Y_OFFSET;

    n = 0;
    XtSetArg(argList[n], XmNminimumValue, 0); n++;
    XtSetArg(argList[n], XmNdecimalPoints, 3); n++;
    XtSetArg(argList[n], XmNincrementValue, 2); n++;
    XtSetArg(argList[n], XmNmaximumValue, 121500); n++;
    XtSetArg(argList[n], XmNspinBoxChildType, XmNUMERIC); n++;

    spin3_text = XmCreateTextField( spin3,
				    "spin3_text",
				    argList,
				    n );


    /*****  Manage SpinBox  *****/
    XtManageChild(spin3);

    /*****  Manage SpinBox child  *****/
    XtManageChild(spin3_text);

    /*****  End of SpinBox spin3  *****/



    /*****
     *****  Create SpinBox spin4
     *****
     *****  Choices:  0 to 10 by 1
     *****
     *****  Callbacks:
     *****    - modifySpin4 Callback inhibits spinning below 0
     *****      at all times.  Wrapping increments the maximum value.
     *****
     *****/

    /*****  Create SpinBox parent  *****/
    n = 0;
    XtSetArg(argList[n], XmNy, nextY); n++;
    XtSetArg(argList[n], XmNarrowSize, 35); n++;
    XtSetArg(argList[n], XmNrepeatDelay, 250); n++;
    XtSetArg(argList[n], XmNinitialDelay, 500); n++;
    XtSetArg(argList[n], XmNarrowLayout, XmARROWS_SPLIT); n++;

    spin4 = XmCreateSpinBox( parent,
			     "spin4",
			     argList,
			     n );

    /*****  Increment Y position for next SpinBox  *****/
    nextY +=  Y_OFFSET;

    n = 0;
    XtSetArg(argList[n], XmNmaximumValue, 4); n++;
    XtSetArg(argList[n], XmNspinBoxChildType, XmNUMERIC); n++;

    spin4_text = XmCreateTextField( spin4,
				    "spin4_text",
				    argList,
				    n );


    /*****  Manage SpinBox  *****/
    XtManageChild(spin4);

    /*****  Call modifySpin4 BEFORE displayed value is changed  *****/
    XtAddCallback(spin4, XmNmodifyVerifyCallback, modifySpin4, (XtPointer) 0);

    /*****  Manage SpinBox child  *****/
    XtManageChild(spin4_text);

    /*****  End of SpinBox spin4  *****/



    /*****
     *****  Create SpinBox spin5
     *****
     *****  Choices:  0 to 10 by 1
     *****
     *****  Delay:  Repeat delay is 0 - displayed value only increments
     *****          or decrements ONCE each time arrow is armed
     *****
     *****  Callbacks:  none
     *****
     *****/

    /*****  Create SpinBox parent  *****/
    n = 0;
    XtSetArg(argList[n], XmNy, nextY); n++;
    XtSetArg(argList[n], XmNarrowSize, 20); n++;
    XtSetArg(argList[n], XmNrepeatDelay, 0); n++;
    XtSetArg(argList[n], XmNarrowLayout, XmARROWS_BEGINNING); n++;

    spin5 = XmCreateSpinBox( parent,
			     "spin5",
			     argList,
			     n );


    /*****  Increment Y position for next SpinBox  *****/
    nextY +=  Y_OFFSET;

    n = 0;
    XtSetArg(argList[n], XmNminimumValue, 1); n++;
    XtSetArg(argList[n], XmNposition, 1); n++;
    XtSetArg(argList[n], XmNspinBoxChildType, XmNUMERIC); n++;
    XtSetArg(argList[n], XmNeditable, False); n++;

    spin5_text = XmCreateTextField( spin5,
				    "spin5_text",
				    argList,
				    n );


    /*****  Manage SpinBox  *****/
    XtManageChild(spin5);

    /*****  Manage SpinBox child  *****/
    XtManageChild(spin5_text);

    /*****  End of SpinBox spin5  *****/



    /*****
     *****  Create SpinBox spin6
     *****
     *****  This example has multiple children, including two
     *****  decoration children.  In addition, this SpinBox
     *****  includes 'chaining', the process where a change
     *****  in one child causes values to change in the child
     *****  and on or more other children.  Chaining is performed
     *****  by the valueChanged callback.
     *****
     *****  Choices:  spin6_text1 1 to 12 (months)
     *****            spin6_deco1 '/' decoration
     *****            spin6_text2 1 to 28-31 (max days varies by month)
     *****            spin6_deco2 '/' decoration
     *****            spin6_text3 0 to 99 (years)
     *****
     *****  Callbacks:
     *****    - changedSpin6 sets maximum days in month.  The month field
     *****          is chained to the day field, and the year field is
     *****          chained to the month field.  (When the day child
     *****          wraps, the month child is also changed.  When the
     *****          month child wraps, the year child is also changed.)
     *****
     *****/

    /*****  Create SpinBox parent  *****/
    n = 0;
    XtSetArg(argList[n], XmNy, nextY); n++;
    XtSetArg(argList[n], XmNinitialDelay, 0); n++;
    XtSetArg(argList[n], XmNrepeatDelay, 150); n++;

    spin6 = XmCreateSpinBox( parent,
			     "spin6",
			     argList,
			     n );


    /*****  Increment Y position for next SpinBox  *****/
    nextY +=  Y_OFFSET;

    /*****  Create SpinBox child  *****/
    n = 0;
    XtSetArg(argList[n], XmNwidth, 30); n++;
    XtSetArg(argList[n], XmNposition, thisMM - 1); n++;
    XtSetArg(argList[n], XmNminimumValue, 1); n++;
    XtSetArg(argList[n], XmNmaximumValue, 12); n++;
    XtSetArg(argList[n], XmNspinBoxChildType, XmNUMERIC); n++;

    spin6_text1 = XmCreateTextField( spin6,
				     "spin6_text1",
				     argList,
				     n );


    /*****  Create SpinBox decoration child  *****/
    n = 0;
    decoString = XmStringLtoRCreate("/", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(argList[n], XmNlabelString, decoString); n++;

    spin6_deco1 = XmCreateLabel( spin6,
			         "spin6_deco1",
			         argList,
			         n );

    /*****  Create SpinBox child  *****/
    n = 0;
    XtSetArg(argList[n], XmNwidth, 30); n++;
    XtSetArg(argList[n], XmNposition, thisDD - 1); n++;
    XtSetArg(argList[n], XmNminimumValue, 1); n++;
    XtSetArg(argList[n], XmNmaximumValue, 31); n++;
    XtSetArg(argList[n], XmNspinBoxChildType, XmNUMERIC); n++;

    spin6_text2 = XmCreateTextField( spin6,
				     "spin6_text2",
				     argList,
				     n );

    /*****  Create SpinBox decoration child  *****/
    n = 0;
    decoString = XmStringLtoRCreate("/", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(argList[n], XmNlabelString, decoString); n++;
    spin6_deco2 = XmCreateLabel( spin6,
			         "spin6_deco2",
			         argList,
			         n );


    XmStringFree(decoString);

    /*****  Create SpinBox child  *****/
    n = 0;
    XtSetArg(argList[n], XmNwidth, 30); n++;
    XtSetArg(argList[n], XmNposition, thisYY); n++;
    XtSetArg(argList[n], XmNmaximumValue, 99); n++;
    XtSetArg(argList[n], XmNspinBoxChildType, XmNUMERIC); n++;


    spin6_text3 = XmCreateTextField( spin6,
				     "spin6_text3",
				     argList,
				     n );

    /*****  Manage SpinBox  *****/
    XtManageChild(spin6);

    /*****  Call changedSpin6 AFTER displayed value has changed  *****/
    XtAddCallback(spin6, XmNvalueChangedCallback, changedSpin6, (XtPointer) 0);

    /*****  Manage SpinBox children  *****/
    XtManageChild(spin6_text1);
    XtManageChild(spin6_deco1);
    XtManageChild(spin6_text2);
    XtManageChild(spin6_deco2);
    XtManageChild(spin6_text3);

    /*****  End of SpinBox spin6  *****/
}



/*****							*****/
/*****			Utility Functions		*****/
/*****							*****/

/*****
 *****	setMonths - create XmString array of month names
 *****/
void
setMonths()
{
XmString tempString;
int	 monthLoop;

    for (monthLoop = 0; monthLoop < NUM_MONTHS; monthLoop++)
	{
	tempString = XmStringCreate(months[monthLoop], XmFONTLIST_DEFAULT_TAG);
	monthValues[monthLoop] = tempString;
	}
}

/*****
 *****	setLetters - create XmString array of single letters
 *****/
void
setLetters()
{
XmString tempString;
int	 letterLoop;

    for (letterLoop = 0; letterLoop < NUM_LETTERS; letterLoop++)
	{
	tempString = XmStringCreate(letters[letterLoop], XmFONTLIST_DEFAULT_TAG);
	letterValues[letterLoop] = tempString;
	}
}

/*****
 *****	printReason - print string description of callback reason
 *****/
void
printReason(int reason)
{

    printf("reason = ");

    switch(reason)
	{
	case XmCR_SPIN_NEXT:
	    printf("XmCR_SPIN_NEXT");
	    break;

	case XmCR_SPIN_PRIOR:
	    printf("XmCR_SPIN_PRIOR");
	    break;

	case XmCR_SPIN_FIRST:
	    printf("XmCR_SPIN_FIRST");
	    break;

	case XmCR_SPIN_LAST:
	    printf("XmCR_SPIN_LAST");
	    break;

	case XmCR_OK:
	    printf("XmCR_OK");
	    break;

	default:
	    printf("** unknown ** (%d)", reason);
	    break;
	}

    printf("\n");
}

/*****
 *****	setMaxDay - set maximumValue for day widget
 *****/
void
setMaxDay(Widget monthWidget, int nextMM)
{
Cardinal n;
Arg      argList[5];
int	 lastDay;

    if (nextMM == 2)
	{
	if (thisYY %4 == 0 && thisYY %400 != 0)
	    lastDay = 29;
	else
	    lastDay = 28;
	}
    else if (nextMM ==  4
	 ||  nextMM ==  6
	 ||  nextMM ==  9
	 ||  nextMM == 11)
	lastDay = 30;
    else
	lastDay = 31;

    n = 0;

    XtSetArg(argList[n], XmNmaximumValue, lastDay);
    n++;

    XtSetValues(monthWidget, argList, n);
}

/*****							*****/
/*****			Callback Functions		*****/
/*****							*****/

void
changedSpin0(Widget w, XtPointer client, XtPointer call)
{
XmSpinBoxCallbackStruct *user;
static int		thisYear = 1994;

    user = (XmSpinBoxCallbackStruct *)call;

    /*****  Print string description of reason  *****/
    printReason(user->reason);

    if (user->crossed_boundary)
	{
	if (user->reason == XmCR_SPIN_NEXT)
	  thisYear++;
	else
	  thisYear--;

	printf("Year Now: %d\n", thisYear);
	}
}

void
changedSpin1(Widget w, XtPointer client, XtPointer call)
{
XmSpinBoxCallbackStruct *user;
Cardinal		n;
Arg		   	argList[5];

    user = (XmSpinBoxCallbackStruct *)call;

    n = 0;

    if (user->position == SPIN1_MAX)
	XtSetArg(
		argList[n],
		XmNarrowSensitivity,
		XmARROWS_DECREMENT_SENSITIVE
		);
    else if (user->position == SPIN1_MIN)
	XtSetArg(
		argList[n],
		XmNarrowSensitivity,
		XmARROWS_INCREMENT_SENSITIVE
		);
    else
	XtSetArg(argList[n], XmNarrowSensitivity, XmARROWS_SENSITIVE);

    n++;

    XtSetValues(user->widget, argList, n);
}

void
changedSpin6(Widget w, XtPointer client, XtPointer call)
{
XmSpinBoxCallbackStruct *user;
Cardinal		n;
Arg		   	argList[5];
int			saveMM;
int			saveDD;
int			saveYY;

    user = (XmSpinBoxCallbackStruct *)call;

    if (user->widget == spin6_text1)
	{
	thisMM = user->position + 1;

        if (thisDD <=3 )
	    setMaxDay(spin6_text2, thisMM -1);
	else if (thisDD > 27)
	    setMaxDay(spin6_text2, thisMM);

	saveYY = thisYY;

	if (user->crossed_boundary)
        {
	    if (user->reason == XmCR_SPIN_NEXT)
		thisYY++;
	    else
		thisYY--;
        }

	if (thisYY != saveYY)
	    {
	    if (thisYY < 0)
		thisYY = 99;
	    else if (thisYY > 99)
		thisYY %= 100;

	    n = 0;
	    XtSetArg(argList[n], XmNposition, thisYY);
	    n++;

	    XtSetValues(spin6_text3, argList, n);
	    }
	}
    else if (user->widget == spin6_text2)
	{
        thisDD = user->position + 1;

        if (thisDD <=3 && user->reason == XmCR_SPIN_PRIOR)
            setMaxDay(spin6_text2, thisMM -1);
        else if (thisDD > 27 && user->reason == XmCR_SPIN_NEXT)
            setMaxDay(spin6_text2, thisMM);

        saveMM = thisMM;
        saveYY = thisYY;

        if (user->crossed_boundary)
        {
            if (user->reason == XmCR_SPIN_NEXT)
                thisMM++;
            else
                thisMM--;
        }

        if (thisMM != saveMM)
            {
            if (thisMM < 1)
		{
		thisMM = 12;
		thisYY--;
		}
            else if (thisMM > 12)
		{
		thisMM = 1;
		thisYY++;
		}

            n = 0;
            XtSetArg(argList[n], XmNposition, thisMM - 1);
            n++;

            XtSetValues(spin6_text1, argList, n);

	    if (thisYY != saveYY)
		{
		n = 0;
		XtSetArg(argList[n], XmNposition, thisYY);
		n++;

		XtSetValues(spin6_text3, argList, n);
		}
            }
        }
    else if (user->widget == spin6_text3)
	{
	thisYY = user->position;

        if (user->reason == XmCR_OK)
	    {
	    if (thisDD <= 3)
		setMaxDay(spin6_text2, thisMM - 1);
	    else if (thisDD > 27)
		setMaxDay(spin6_text2, thisMM);
	    }
	}
}

void
modifySpin1(Widget w, XtPointer client, XtPointer call)
{
XmSpinBoxCallbackStruct *user;

    user = (XmSpinBoxCallbackStruct *)call;

    if (user->position > SPIN1_MAX)
	user->doit = False;
    else
        /*****  Print string description of reason  *****/
	printReason(user->reason);
}

void
modifySpin4(Widget w, XtPointer client, XtPointer call)
{
XmSpinBoxCallbackStruct *user;
int			newHigh;
Cardinal		n;
Arg		   	argList[5];

    user = (XmSpinBoxCallbackStruct *)call;

    if (user->crossed_boundary)
	{
        if (user->reason == XmCR_SPIN_NEXT)
	    {
	    n = 0;
	    XtSetArg(argList[n], XmNmaximumValue, &newHigh); n++;
	    XtGetValues(user->widget, argList, n);

	    newHigh++;

	    n = 0;
	    XtSetArg(argList[n], XmNmaximumValue, newHigh); n++;
	    XtSetValues(user->widget, argList, n);
            user->position = 0;
	    }
	else if (user->reason == XmCR_SPIN_PRIOR)
		user->doit = False;
	}
}

