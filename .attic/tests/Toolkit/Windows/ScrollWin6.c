/* $XConsortium: ScrollWin6.c /main/4 1995/07/15 21:11:58 drk $ */
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

static char* textData = "...a place where children did not even know what a bush, a forest, or a pond was;\n\
 where the air was so bad with carbon dioxide and sulphur that pollution killed at least one member in every family;\n\
 a place where men and beasts baked in a furnace for the eight months of summer until\n\
 the monsoon transformed their alleyways and shacks into swamps of mud and excrement;\n\
 a place where leprosy, tuberculosis, dysentery and all the malnutrition diseases ...\n\
 reduced the average life expectancy to one of the lowest in the world; a place where\n\
 eighty-five hundred cows and buffalo, tied up to dung heaps, provided milk infected with germs.\n\
 Above all, however, the City of Joy was a place where the most extreme economic poverty ran rife.\n\
 Nine out of ten of its inhabitants did not have a single rupee per day with which\n\
 to buy half a pound of rice.  Furthermore, like all ... slums, the\n\
 City of Joy was generally ignored by other citizens of Calcutta,\n\
 except in case of crime or strike.  Considered a dangerous\n\
 neighborhood with a terrible reputation, the haunt of untouchables,\n\
 pariahs, social rejects, it was a world apart, living apart from the\n\
 world.\n\
\n\
 The City of Joy, Dominique Lapierre,\n\
 Warner Books, Inc., New York, NY, 1985.";

void  main(argc, argv)
    int     argc;
    char  **argv;
{
    register int  n;
    Arg           args[MAX_ARGS];
    Widget        scroll1, label1, hscrollbar, vscrollbar, frame1;
    XmString      s;

    CommonTestInit(argc, argv);

    n = 0;
    XtSetArg(args[n], XmNwidth, 100); n++;
    XtSetArg(args[n], XmNheight, 100); n++;
    XtSetValues(Shell1, args, n);

    n = 0;
    XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC); n++;
    XtSetArg(args[n], XmNwidth, 100); n++;
    XtSetArg(args[n], XmNheight, 100); n++;
    scroll1 = XmCreateScrolledWindow(Shell1, "scrollwin1", args, n);

    n = 0;
    XtSetArg(args[n], XmNscrolledWindowChildType, XmNO_SCROLL); n++;
    frame1 = XmCreateFrame(scroll1, "frame1", args, n);

    n = 0;
    s = XmStringCreateLtoR(textData, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, s); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    label1 = XmCreateLabel(frame1, "label1", args, n);
    XmStringFree(s);

    XtManageChild(label1);
    XtManageChild(frame1);
    XtManageChild(scroll1);
    XtRealizeWidget(Shell1);

    CommonPause();

    XtDestroyWidget(label1);
    XtDestroyWidget(frame1);
    XtDestroyWidget(scroll1);

    n = 0;
    XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC); n++;
    XtSetArg(args[n], XmNwidth, 100); n++;
    XtSetArg(args[n], XmNheight, 100); n++;
    scroll1 = XmCreateScrolledWindow(Shell1, "scrollwin1", args, n);

    n = 0;
    XtSetArg(args[n], XmNscrolledWindowChildType, XmSCROLL_VERT); n++;
    frame1 = XmCreateFrame(scroll1, "frame1", args, n);

    n = 0;
    s = XmStringCreateLtoR(textData, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, s); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    label1 = XmCreateLabel(frame1, "label1", args, n);
    XmStringFree(s);

    XtManageChild(label1);
    XtManageChild(frame1);
    XtManageChild(scroll1);

    CommonPause();

    XtDestroyWidget(label1);
    XtDestroyWidget(frame1);
    XtDestroyWidget(scroll1);

    n = 0;
    XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC); n++;
    XtSetArg(args[n], XmNwidth, 100); n++;
    XtSetArg(args[n], XmNheight, 100); n++;
    scroll1 = XmCreateScrolledWindow(Shell1, "scrollwin1", args, n);

    n = 0;
    XtSetArg(args[n], XmNscrolledWindowChildType, XmSCROLL_HOR); n++;
    frame1 = XmCreateFrame(scroll1, "frame1", args, n);

    n = 0;
    s = XmStringCreateLtoR(textData, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, s); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    label1 = XmCreateLabel(frame1, "label1", args, n);
    XmStringFree(s);

    XtManageChild(label1);
    XtManageChild(frame1);
    XtManageChild(scroll1);

    CommonPause();

    XtDestroyWidget(label1);
    XtDestroyWidget(frame1);
    XtDestroyWidget(scroll1);

    n = 0;
    XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC); n++;
    XtSetArg(args[n], XmNwidth, 100); n++;
    XtSetArg(args[n], XmNheight, 100); n++;
    scroll1 = XmCreateScrolledWindow(Shell1, "scrollwin1", args, n);

    n = 0;
    XtSetArg(args[n], XmNscrolledWindowChildType, XmWORK_AREA); n++;
    frame1 = XmCreateFrame(scroll1, "frame1", args, n);

    n = 0;
    s = XmStringCreateLtoR(textData, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, s); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    label1 = XmCreateLabel(frame1, "label1", args, n);
    XmStringFree(s);

    XtManageChild(label1);
    XtManageChild(frame1);
    XtManageChild(scroll1);

    CommonPause();

    XtDestroyWidget(label1);
    XtDestroyWidget(frame1);
    XtDestroyWidget(scroll1);

    n = 0;
    XtSetArg(args[n], XmNscrollingPolicy, XmAPPLICATION_DEFINED); n++;
    XtSetArg(args[n], XmNwidth, 100); n++;
    XtSetArg(args[n], XmNheight, 100); n++;
    scroll1 = XmCreateScrolledWindow(Shell1, "scrollwin1", args, n);

    n = 0;
    XtSetArg(args[n], XmNscrolledWindowChildType, XmHOR_SCROLLBAR); n++;
    XtSetArg(args[n], XmNshadowThickness, 10); n++;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    hscrollbar = XmCreateScrollBar(scroll1, "hscrollbar", args, n);

    n = 0;
    XtSetArg(args[n], XmNscrolledWindowChildType, XmVERT_SCROLLBAR); n++;
    XtSetArg(args[n], XmNshadowThickness, 10); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    vscrollbar = XmCreateScrollBar(scroll1, "vscrollbar", args, n);

    n = 0;
    XtSetArg(args[n], XmNscrolledWindowChildType, XmWORK_AREA); n++;
    frame1 = XmCreateFrame(scroll1, "frame1", args, n);

    n = 0;
    s = XmStringCreateLtoR(textData, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, s); n++;
    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
    label1 = XmCreateLabel(frame1, "label1", args, n);
    XmStringFree(s);

    XtManageChild(label1);
    XtManageChild(frame1);
    XtManageChild(hscrollbar);
    XtManageChild(vscrollbar);
    XtManageChild(scroll1);

    CommonPause();

    XtAppMainLoop(app_context);
}
