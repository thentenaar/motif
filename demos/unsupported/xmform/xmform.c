/* $XConsortium: xmform.c /main/4 1995/07/15 20:47:50 drk $ */
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
/**---------------------------------------------------------------------
***	
***	file:		xmform.c
***
***	project:	Motif Widgets example programs
***
***	creation:	Spring 91 - Cambridge
***                       Daniel Dardailler.
***
***	description:	This program demonstrates the possibilities of
***                     the Motif form widget.
***                     It can display a string of letters made from buttons 
***	                and arrows (resizable using Form attachments).
***
***	defaults:	xmform looks nicer with these defaults:
***
xmform*highlightThickness:       0
xmform.XmForm.shadowThickness:   3
xmform*XmPushButton.background:  cadetblue
xmform*XmArrowButton.foreground: cadetblue
xmform*XmArrowButton.background: lightgrey
xmform*XmForm.background:        lightgrey
xmform*topShadowColor:           white
xmform*bottomShadowColor:        black
***-------------------------------------------------------------------*/

#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/ArrowB.h>

static void FillItPlease();
static void NearlyEvenSpread();
static Widget AddStem() ;

int main(argc, argv) 
int argc; char **argv;
{
    XtAppContext app_context;
    Widget      toplevel, topform, *forms ;
    Cardinal len_word, p ;
    String word = "MOTIF" ;
    char s[2] ;

    toplevel = XtAppInitialize(&app_context, "XMdemos", NULL, 0,
			       &argc, argv, NULL, NULL, 0);
    topform = XmCreateForm(toplevel, "topform", NULL, 0);
    XtManageChild(topform);

    /* pick up the string to be displayed if any */
    if (argc == 2) word = argv[1] ;

    /* create a form for each letter and fill it with the 'stem widgets' */
    len_word = strlen(word);
    forms = (Widget*)calloc(len_word,sizeof(Widget));
    s[1] = 0 ;  /* string used for the subform widget name */
    for (p = 0 ; p < len_word; p++) { 
	s[0] = word[p] ;  /* subform  names are the letters themselves */
	forms[p] = XmCreateForm(topform, s, NULL, 0); 
	FillItPlease(forms[p], word[p]);
    }

    /* evenly spread out the forms with some spacing in between */
    NearlyEvenSpread(forms, p, 
		     2,          /* horizontal spacing */
		     9) ;        /* vertical margin */

    XtManageChildren(forms, p);
    XtRealizeWidget(toplevel);
    XtAppMainLoop(app_context);

    return 0;    /* make compiler happy */
}


static void NearlyEvenSpread(child_list, num_child, 
			     hor_spacing, ver_spacing)
Widget * child_list;
Cardinal num_child;
Dimension hor_spacing;
Dimension ver_spacing;
{
    /* this algorithm works only for the default fractionBase == 100 */
    /* hor_spacing and ver_margin are equally applied on each sides */

    Dimension hor_child_room ;
    Cardinal n, i ;
    Arg args[10] ;

    hor_child_room = 100 / num_child ;

    for (i = 0 ; i < num_child ; i++) {
	n = 0 ;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++ ;
	XtSetArg(args[n], XmNleftPosition, 
		 hor_child_room*i + hor_spacing); n++ ;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++ ;
	XtSetArg(args[n], XmNrightPosition, 
		 hor_child_room*(i+1) - hor_spacing); n++ ;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++ ;
	XtSetArg(args[n], XmNtopPosition, ver_spacing); n++ ;
	XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++ ;
	XtSetArg(args[n], XmNbottomPosition, 100 - ver_spacing); n++ ;
	XtSetValues(child_list[i], args, n);
    }
}

static void FillItPlease(form, letter)
Widget form ;
char letter ;
{
    /* This routine only handle the "MOTIF" letters, it's up 
       to you to implement the rest, good luck for K,N,R,V,X,Y,Z... */

    Cardinal n = 0, p = 0 ;
    Arg args[10] ;
    Widget child[10] ;

    switch(letter) {
    case 'M' :
	/* 2 buttons and the arrow in between on top */
	child[p] = AddStem(form, 0, 25, 100, 0, XmPUSHBUTTON); p++;
	child[p] = AddStem(form, 0, 100, 100, 75, XmPUSHBUTTON); p++;
	child[p] = AddStem(form, 0, 75, 35, 25, NULL); p++;
/*	XtSetArg(args[n], XmNshadowThickness, 0); n++;*/
	XtSetArg(args[n], XmNarrowDirection, XmARROW_DOWN); n++;
	XtSetValues(child[2], args, n);
	XtManageChildren(child, p);
	break ;
    case 'O' :
	/* 2 left and right vertical buttons and 2 horizontals */
	child[p] = AddStem(form, 25, 25, 75, 0, XmPUSHBUTTON); p++;
	child[p] = AddStem(form, 0, 100, 25, 0, XmPUSHBUTTON); p++;
	child[p] = AddStem(form, 25, 100, 75, 75, XmPUSHBUTTON); p++;
	child[p] = AddStem(form, 75, 100, 100, 0, XmPUSHBUTTON); p++;
	XtManageChildren(child, p);
	break ;
    case 'T' :
	/* 1 vertical button and 1 horizontal on top */
	child[p] = AddStem(form, 25, 63, 100, 38, XmPUSHBUTTON); p++;
	child[p] = AddStem(form, 0, 100, 25, 0, XmPUSHBUTTON); p++;
	XtManageChildren(child, p);
	break ;
    case 'I' :
	/* 1 vertical button + one arrow on top */
	child[p] = AddStem(form, 0, 63, 25, 38, NULL); p++;
	child[p] = AddStem(form, 30, 63, 100, 38, XmPUSHBUTTON); p++;
/*	XtSetArg(args[n], XmNshadowThickness, 0); n++;*/
	XtSetValues(child[0], args, n);
	XtManageChildren(child, p);
	break ;
    case 'F' :
	/* 1 vertical button and 2 horizontal on the right */
	child[p] = AddStem(form, 0, 25, 100, 0, XmPUSHBUTTON); p++;
	child[p] = AddStem(form, 0, 100, 25, 25, XmPUSHBUTTON); p++;
	child[p] = AddStem(form, 36, 75, 61, 25, XmPUSHBUTTON); p++;
	XtManageChildren(child, p);
	break ;
    default:
	child[p] = AddStem(form, 0, 100, 100, 0, XmPUSHBUTTON); p++;
	XtManageChildren(child, p);
	break ;
    }
}


static Widget AddStem(form, top, right, bottom, left, type)
Widget form ;
Dimension top, right, bottom, left;
int type ;
{
    /* create an object, either a button or an arrow, using
       positionnal attachment everywhere, instead of possibly 
       form attachment or widget attachment */

    Cardinal n = 0 ;
    Arg args[10] ;

    XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNtopPosition, top); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNrightPosition, right); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNbottomPosition, bottom); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_POSITION); n++;
    XtSetArg(args[n], XmNleftPosition, left); n++;
    if (type == XmPUSHBUTTON)
	return XmCreatePushButton(form, "", args, n); 
    else 
	return XmCreateArrowButton(form, "", args, n); 
}

