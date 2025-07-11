/* $XConsortium: TextFieldM1.c /main/4 1995/07/15 21:06:32 drk $ */
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

#define MANAGER_WIDTH  300
#define MANAGER_HEIGHT 300
#define MARGIN         10
#define TEXT_WIDTH     (MANAGER_WIDTH/2 - 2*MARGIN)
#define TEXT_HEIGHT    (MANAGER_HEIGHT/2 - 2*MARGIN)

static char string1[] = "\
abcdefghijklmnopqrstuvwxyz\n\
0123456789\n\
abcdefghijklmnopqrstuvwxyz\n\
0123456789\n\
Jan. Feb. Mar. Apr. May Jun. Jul. Aug. Sep. Oct. Nov. Dec."; 

void main (argc, argv) 
int argc; 
char **argv;
{
	int     n = 0;
	Arg     args[MAX_ARGS];
	Widget  BBoard1, Text1;

	CommonTestInit( argc, argv );

 	n = 0;	
	XtSetArg( args[n], XmNwidth, MANAGER_WIDTH ); n++;
	XtSetArg( args[n], XmNheight, MANAGER_HEIGHT ); n++;
	BBoard1 = XmCreateBulletinBoard( Shell1, "BBoard1", args, n );
	XtManageChild( BBoard1 );

      /*
       *  Create an XmTextField widget and destroy it prior to
       *  ever realizing it.
       */
	n = 0;
	XtSetArg( args[n], XmNx, MARGIN ); n++;
	XtSetArg( args[n], XmNy, MARGIN ); n++;
	XtSetArg( args[n], XmNwidth, MANAGER_WIDTH - 2 * MARGIN ); n++;
	XtSetArg( args[n], XmNheight, MANAGER_HEIGHT - 2 * MARGIN ); n++;
	XtSetArg( args[n], XmNvalue, string1 ); n++;
	XtSetArg( args[n], XmNeditable, False ); n++;
	Text1 = XmCreateTextField( BBoard1, "Text1", args, n );
	XtManageChild( Text1 );
	
	XtDestroyWidget( Text1 );

     	XtRealizeWidget(Shell1);

	CommonPause();
	CommonPause();
	XtAppMainLoop( app_context );
}
        
	
