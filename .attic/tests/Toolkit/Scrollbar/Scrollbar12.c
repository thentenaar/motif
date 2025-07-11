/* $XConsortium: Scrollbar12.c /main/5 1995/07/15 21:09:25 drk $ */
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

#define SCROLLBAR_WIDTH  50
#define CLIENT_WIDTH     SCROLLBAR_WIDTH * 8
#define CLIENT_HEIGHT    CLIENT_WIDTH
#define SEPARATOR_WIDTH  5
#define MARGINS          20

void main ( int argc, char **argv )
{
  register int n;
  Arg      args[MAX_ARGS];
  Widget   BulletinBoard1, Scrollbar1, Separator1, Separator2;
  Position sb_xcoord, sb_ycoord;

  CommonTestInit(argc, argv);

  n = 0;
  XtSetArg(args[n], XmNwidth,  CLIENT_WIDTH);  n++;
  XtSetArg(args[n], XmNheight, CLIENT_HEIGHT);  n++;
  BulletinBoard1 = XmCreateBulletinBoard(Shell1, "BulletinBoard1", args, n);
  XtManageChild(BulletinBoard1);

 /*
  *  XmNsnapBackMultiple is the default.
  *  Scrollbar should continue sliding regardless of where on the screen
  *  the pointer is being moved.
  */
  sb_xcoord = 3.5 * SCROLLBAR_WIDTH;
  sb_ycoord = 0;

  n = 0;
  XtSetArg(args[n], XmNx, sb_xcoord ); n++;
  XtSetArg(args[n], XmNy,  sb_ycoord );  n++;
  XtSetArg(args[n], XmNwidth,  SCROLLBAR_WIDTH);  n++;
  XtSetArg(args[n], XmNheight, CLIENT_HEIGHT - 2 * MARGINS);  n++;
  XtSetArg(args[n], XmNorientation, XmVERTICAL);  n++;
  Scrollbar1 = XmCreateScrollBar(BulletinBoard1, "Scrollbar1", args, n);
  XtManageChild(Scrollbar1);

  XtRealizeWidget( Shell1 );

  CommonPause();

 /*
  *  XmNsnapBackMultiple = 0.
  *  Scrollbar slider should "snap back" as soon as pointer moves
  *  outside the bounds of the scrollbar.
  */
  n = 0;
  XtSetArg(args[n], XmNsnapBackMultiple, 0);  n++;
  XtSetValues( Scrollbar1, args, n );

  CommonPause();

 /*
  *  XmNsnapBackMultiple = 1.
  *  Scrollbar slider should "snap back" as soon as pointer moves
  *  beyond the distance of one SCROLLBAR_WIDTH from the scrollbar boundary.
  */
  n = 0;
  XtSetArg(args[n], XmNsnapBackMultiple, 1 );  n++;
  XtSetValues( Scrollbar1, args, n );

 /*
  *  Place vertical separators at a distance of one SCROLLBAR_WIDTH
  *  from the bounds of the Scrollbar.
  */
  n = 0;
  XtSetArg(args[n], XmNx, sb_xcoord - SCROLLBAR_WIDTH ); n++;
  XtSetArg(args[n], XmNy,  0 );  n++;
  XtSetArg(args[n], XmNwidth, SEPARATOR_WIDTH );  n++;
  XtSetArg(args[n], XmNheight, CLIENT_HEIGHT - 2 * MARGINS );  n++;
  XtSetArg(args[n], XmNorientation, XmVERTICAL);  n++;
  XtSetArg(args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT);  n++;
  Separator1 = XmCreateSeparator( BulletinBoard1, "Separator1", args, n ); 
  XtManageChild( Separator1 );
  
  n = 0;
  XtSetArg(args[n], XmNx, sb_xcoord + 2 * SCROLLBAR_WIDTH ); n++;
  XtSetArg(args[n], XmNy,  0 );  n++;
  XtSetArg(args[n], XmNwidth, SEPARATOR_WIDTH );  n++;
  XtSetArg(args[n], XmNheight, CLIENT_HEIGHT - 2 * MARGINS );  n++;
  XtSetArg(args[n], XmNorientation, XmVERTICAL);  n++;
  XtSetArg(args[n], XmNseparatorType, XmSHADOW_ETCHED_OUT);  n++;
  Separator2 = XmCreateSeparator( BulletinBoard1, "Separator2", args, n ); 
  XtManageChild( Separator2 );

 /*
  *  Reset the dimensions of the bulletin board widget.
  */
  n = 0;
  XtSetArg(args[n], XmNwidth,  CLIENT_WIDTH);  n++;
  XtSetArg(args[n], XmNheight, CLIENT_HEIGHT);  n++;
  XtSetValues( BulletinBoard1, args, n );
  
  CommonPause();

 /*
  *  XmNsnapBackMultiple =  2.
  *  Scrollbar slider should "snap back" as soon as pointer moves
  *  beyond the distance of 2 x SCROLLBAR_WIDTH from the scrollbar boundary.
  */
  n = 0;
  XtSetArg(args[n], XmNsnapBackMultiple, 2 );  n++;
  XtSetValues( Scrollbar1, args, n );


 /*
  *  Update the position of the separators to reflect the new
  *  "snap back" boundary.
  */
  n = 0;
  XtSetArg(args[n], XmNx, sb_xcoord - 2 * SCROLLBAR_WIDTH );  n++;
  XtSetValues( Separator1, args, n );
  
  n = 0;
  XtSetArg(args[n], XmNx, sb_xcoord + 3 * SCROLLBAR_WIDTH );  n++;
  XtSetValues( Separator2, args, n );

 /*
  *  Reset the dimensions of the bulletin board widget.
  */
  n = 0;
  XtSetArg(args[n], XmNwidth,  CLIENT_WIDTH);  n++;
  XtSetArg(args[n], XmNheight, CLIENT_HEIGHT);  n++;
  XtSetValues( BulletinBoard1, args, n );

  CommonPause();

 /*
  *  Change the orientation of the scrollbar to be horizontal.
  *  Place the scrollbar in the horizontal center of its parent.
  *  Set XmNsnapBackMultiple to 1000.
  */
  sb_xcoord = 0;
  sb_ycoord = 3.5 * SCROLLBAR_WIDTH;

  n = 0;
  XtSetArg(args[n], XmNx,  sb_xcoord);  n++;
  XtSetArg(args[n], XmNy,  sb_ycoord);  n++;
  XtSetArg(args[n], XmNwidth,  CLIENT_WIDTH - 2 * MARGINS);  n++;
  XtSetArg(args[n], XmNheight, SCROLLBAR_WIDTH);  n++;
  XtSetArg(args[n], XmNorientation,  XmHORIZONTAL);  n++;
  XtSetArg(args[n], XmNsnapBackMultiple,  1000);  n++;
  XtSetValues( Scrollbar1, args, n );

 /*
  *  Unmanage the separators.
  */ 
  XtUnmanageChild( Separator1 );
  XtUnmanageChild( Separator2 );

 /*
  *  Reset the dimensions of the bulletin board widget.
  */
  n = 0;
  XtSetArg(args[n], XmNwidth,  CLIENT_WIDTH);  n++;
  XtSetArg(args[n], XmNheight, CLIENT_HEIGHT);  n++;
  XtSetValues( BulletinBoard1, args, n );

  CommonPause();

 /*
  *  XmNsnapBackMultiple = 0 
  */
  n = 0;
  XtSetArg(args[n], XmNsnapBackMultiple,  0);  n++;
  XtSetValues( Scrollbar1, args, n );
  
  CommonPause();

 /*
  *  XmNsnapBackMultiple = 1 
  *  Place separators above and below the scrollbar at the distance
  *  of one scrollbar width.
  */
  n = 0;
  XtSetArg(args[n], XmNsnapBackMultiple,  1);  n++;
  XtSetValues( Scrollbar1, args, n );
  
  n = 0;
  XtSetArg(args[n], XmNx, sb_xcoord );  n++;
  XtSetArg(args[n], XmNy, sb_ycoord - SCROLLBAR_WIDTH );  n++;
  XtSetArg(args[n], XmNwidth, CLIENT_WIDTH - 2 * MARGINS );  n++;
  XtSetArg(args[n], XmNheight, SEPARATOR_WIDTH );  n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL );  n++;
  XtSetValues( Separator1, args, n );
  XtManageChild( Separator1 );

  n = 0;
  XtSetArg(args[n], XmNx, sb_xcoord );  n++;
  XtSetArg(args[n], XmNy, sb_ycoord +  2 * SCROLLBAR_WIDTH );  n++;
  XtSetArg(args[n], XmNwidth, CLIENT_WIDTH - 2 * MARGINS );  n++;
  XtSetArg(args[n], XmNheight, SEPARATOR_WIDTH );  n++;
  XtSetArg(args[n], XmNorientation, XmHORIZONTAL );  n++;
  XtSetValues( Separator2, args, n );
  XtManageChild( Separator2 );

 /*
  *  Reset the dimensions of the bulletin board widget.
  */
  n = 0;
  XtSetArg(args[n], XmNwidth,  CLIENT_WIDTH);  n++;
  XtSetArg(args[n], XmNheight, CLIENT_HEIGHT);  n++;
  XtSetValues( BulletinBoard1, args, n );

  CommonPause();

 /*
  *  XmNsnapBackMultiple = 2
  *  Place separators above and below the scrollbar at the distance
  *  of one scrollbar width.
  */
  n = 0;
  XtSetArg(args[n], XmNsnapBackMultiple,  2);  n++;
  XtSetValues( Scrollbar1, args, n );

  n = 0;
  XtSetArg(args[n], XmNx, sb_xcoord );  n++;
  XtSetArg(args[n], XmNy, sb_ycoord - 2 * SCROLLBAR_WIDTH );  n++;
  XtSetValues( Separator1, args, n );

  n = 0;
  XtSetArg(args[n], XmNx, sb_xcoord );  n++;
  XtSetArg(args[n], XmNy, sb_ycoord + 3 * SCROLLBAR_WIDTH );  n++;
  XtSetValues( Separator2, args, n );

 /*
  *  Reset the dimensions of the bulletin board widget.
  */
  n = 0;
  XtSetArg(args[n], XmNwidth,  CLIENT_WIDTH);  n++;
  XtSetArg(args[n], XmNheight, CLIENT_HEIGHT);  n++;
  XtSetValues( BulletinBoard1, args, n );

  CommonPause();
  CommonPause();

  XtAppMainLoop( app_context );
}
