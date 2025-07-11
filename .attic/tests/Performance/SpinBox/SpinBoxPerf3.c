/* $XConsortium: SpinBoxPerf3.c /main/5 1995/07/15 21:02:17 drk $ */
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


/* Test program for SpinBox*/
#define SYSV	1
#define MARGIN  10

#include <stdio.h>
#include <testlib.h>

#define NUM_MONTHS	12
#define NUM_LETTERS	10

#define SPIN1_MIN	 0
#define SPIN1_MAX	 6

static
setMonths();

static void
setMaxDay(Widget monthWidget, int nextMM);

void
changedSpinB(Widget w, XtPointer client, XtPointer call);

void
main(int argc, char **argv);

int thisMM = 1;
int thisDD = 1;
int thisYY = 89; 

static char *months[NUM_MONTHS] =
  {
  "January",
  "February",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December"
  };


XmString monthValues[NUM_MONTHS];
XmString letterValues[NUM_LETTERS];

/*****		SpinBox Widgets		*****/

Widget SpinBox;
Widget TextField1, TextField2, TextField3;
Widget Decoration1, Decoration2;

/*****							*****/
/*****			Utility Functions		*****/
/*****							*****/

/*****
 *****	setMonths - create XmString array of month names
 *****/
static
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
 *****	setMaxDay - set maximumValue for day widget
 *****/
static void
setMaxDay(Widget monthWidget, int nextMM)
{
Cardinal n;
Arg      argList[5];
int	 lastDay;

    lastDay = 10;

    n = 0;
    XtSetArg(argList[n], XmNmaximumValue, lastDay);
    n++;

    XtSetValues(monthWidget, argList, n);
}

/*****							*****/
/*****			Callback Functions		*****/
/*****							*****/


void
changedSpinB(Widget w, XtPointer client, XtPointer call)
{
XmSpinBoxCallbackStruct *user;
Cardinal		n;
Arg		   	argList[5];
int			saveMM;
int			saveDD;
int			saveYY;

    user = (XmSpinBoxCallbackStruct *)call;

    if (user->widget == TextField1)
	{
	thisMM = user->position + 1;

        if (thisDD <=3 )
	    setMaxDay(TextField2, thisMM -1);
	else if (thisDD > 27)
	    setMaxDay(TextField2, thisMM);

	saveYY = thisYY;

	if (user->crossed_boundary)
	    if (user->reason == XmCR_SPIN_NEXT)
		thisYY++;
	    else
		thisYY--;

	if (thisYY != saveYY)
	    {
	    if (thisYY < 89)
		thisYY = 99;
	    else if (thisYY > 99)
		thisYY %= 100;

	    n = 0;
	    XtSetArg(argList[n], XmNposition, thisYY);
	    n++;

	    XtSetValues(TextField3, argList, n);
	    }
	}
    else if (user->widget == TextField2)
	{
        thisDD = user->position + 1;

        if (thisDD <=3 && user->reason == XmCR_SPIN_PRIOR)
            setMaxDay(TextField2, thisMM -1);
        else if (thisDD > 27 && user->reason == XmCR_SPIN_NEXT)
            setMaxDay(TextField2, thisMM);

        saveMM = thisMM;
        saveYY = thisYY;

        if (user->crossed_boundary)
            if (user->reason == XmCR_SPIN_NEXT)
                thisMM++;
            else
                thisMM--;

        if (thisMM != saveMM)
            {
            if (thisMM < 1)
		{
		thisMM = 10;
		thisYY--;
		}
            else if (thisMM > 10)
		{
		thisMM = 1;
		thisYY++;
		}

            n = 0;
            XtSetArg(argList[n], XmNposition, thisMM - 1);
            n++;

            XtSetValues(TextField1, argList, n);

	    if (thisYY != saveYY)
		{
		n = 0;
		XtSetArg(argList[n], XmNposition, thisYY);
		n++;

		XtSetValues(TextField3, argList, n);
		}
            }
        }
    else if (user->widget == TextField3)
	{
	thisYY = user->position;

        if (user->reason == XmCR_OK)
	    {
	    if (thisDD <= 3)
		setMaxDay(TextField2, thisMM - 1);
	    else if (thisDD > 27)
		setMaxDay(TextField2, thisMM);
	    }
	}
}


/*****							*****/
/*****			Main Function			*****/
/*****							*****/

void
main (int argc, char **argv)
{
XFontStruct  *font;
XmFontList   fontlist;
Cardinal     n;
XmString     decoString;
Widget	     parent;
Arg	     argList[25];

    /*****  Application initialization  *****/
    CommonTestInit( argc, argv );
    XtRealizeWidget(Shell1);

    /*****  Create Font List  *****/
    font = XLoadQueryFont(display, "-*-courier-bold-r-*--18-*");
    fontlist = XmFontListCreate(font, "charset1");

    /*****  Create BulletinBoard parent for SpinBox widgets  *****/
    n = 0;
    XtSetArg(argList[n], XmNwidth, 250); n++;
    XtSetArg(argList[n], XmNheight, 100); n++;

    parent = XmCreateBulletinBoard(Shell1, "Parent", argList, n);
    XtManageChild(parent);

    /****************************************************************/

    /****************************************************************/

    /*****
     *****  Create SpinBox
     *****
     *****  This example has multiple children, including two
     *****  decoration children.  In addition, this SpinBox
     *****  includes 'chaining', the process where a change
     *****  in one child causes values to change in the child
     *****  of one more other children.  Chaining is performed
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
    XtSetArg(argList[n], XmNx, MARGIN); n++;
    XtSetArg(argList[n], XmNy, MARGIN); n++;
    SpinBox = XmCreateSpinBox( parent,
			     "SpinBox",
			     argList,
			     n );
			   

    /*****  Create SpinBox child  *****/
    n = 0;
    XtSetArg(argList[n], XmNwidth, 30); n++;
    XtSetArg(argList[n], XmNposition, thisMM - 1); n++;
    XtSetArg(argList[n], XmNminimumValue, 1); n++;
    XtSetArg(argList[n], XmNmaximumValue, 10); n++;
    XtSetArg(argList[n], XmNspinBoxChildType, XmNUMERIC); n++;

    TextField1 = XmCreateTextField( SpinBox,
				     "TextField1",
				     argList,
				     n ); 
				   

    /*****  Create SpinBox decoration child  *****/
    n = 0;
    decoString = XmStringCreateLtoR("/", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(argList[n], XmNlabelString, decoString); n++;

    Decoration1 = XmCreateLabel( SpinBox,
			         "Decoration1",
			         argList,
			         n );

    /*****  Create SpinBox child  *****/
    n = 0;
    XtSetArg(argList[n], XmNwidth, 30); n++;
    XtSetArg(argList[n], XmNposition, thisDD - 1); n++;
    XtSetArg(argList[n], XmNminimumValue, 1); n++;
    XtSetArg(argList[n], XmNmaximumValue, 10); n++;
    XtSetArg(argList[n], XmNspinBoxChildType, XmNUMERIC); n++;

    TextField2 = XmCreateTextField( SpinBox,
				     "TextField2",
				     argList,
				     n ); 

    /*****  Create SpinBox decoration child  *****/
    n = 0;
    decoString = XmStringCreateLtoR("/", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(argList[n], XmNlabelString, decoString); n++;
    Decoration2 = XmCreateLabel( SpinBox,
			         "Decoration2",
			         argList,
			         n );
			       

    XmStringFree(decoString);

    /*****  Create SpinBox child  *****/
    n = 0;
    XtSetArg(argList[n], XmNwidth, 30); n++;
    XtSetArg(argList[n], XmNposition, thisYY); n++;
    XtSetArg(argList[n], XmNmaximumValue, 89); n++;
    XtSetArg(argList[n], XmNmaximumValue, 99); n++;
    XtSetArg(argList[n], XmNspinBoxChildType, XmNUMERIC); n++;
    

    TextField3 = XmCreateTextField( SpinBox,
				     "TextField3",
				     argList,
				     n ); 

    /*****  Manage SpinBox  *****/
    XtManageChild(SpinBox);

    /*****  Call changedSpinB AFTER displayed value has changed  *****/
    XtAddCallback(SpinBox, XmNvalueChangedCallback, changedSpinB, (XtPointer) 0);

    /*****  Manage SpinBox children  *****/
    XtManageChild(TextField1);
    XtManageChild(Decoration1);
    XtManageChild(TextField2);
    XtManageChild(Decoration2);
    XtManageChild(TextField3);

    CommonPause();
    CommonPause();


    /****  Main Loop  *****/
    XtAppMainLoop(app_context);
}
