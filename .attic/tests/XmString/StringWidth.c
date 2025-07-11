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
static char rcsid[] = "$XConsortium: StringWidth.c /main/7 1995/07/13 19:56:55 drk $"
#endif
#endif
#include        <stdio.h>
#include        <testlib.h>
 
char *test_strings[] = {"", "", "a", "a", "aaaaaa" };
Dimension expected[] = {0, 0, 9, 54, 54, 0, 0 };

#define XmSTRING_DIRECTION_REVERT 2
#define NUM_TESTS 7 

void
main( int argc, char **argv )
{
    XmString	cs, cs1, cs2, cs3, temp;
    XmFontList	fl;
    XFontStruct *font;
    register	i;
    Dimension received[NUM_TESTS];
    int test_num = 0;
 
    CommonTestInit (argc, argv);

    fl = CommonGetFontList
	      ("-adobe-courier-medium-r-normal--14-140-75-75-m-90-iso8859-1");

/*  Test 1 */ 
    cs = XmStringCreate ( test_strings[test_num], XmSTRING_DEFAULT_CHARSET );
    received[test_num++] = XmStringWidth(fl , cs );

/*  Test 2 */ 
    cs1 = XmStringCreate ( test_strings[test_num], XmSTRING_DEFAULT_CHARSET );
    for (i = 0; i < 5; i++) {
        temp = cs1;
	cs1 = XmStringConcat( cs, temp);
        XmStringFree( temp );
    }
    received[test_num++] = XmStringWidth(fl , cs1 );
    XmStringFree( cs );
    XmStringFree( cs1 );

/*  Test 3 */
    cs = XmStringSegmentCreate ( test_strings[test_num], 
		XmSTRING_DEFAULT_CHARSET,
                XmSTRING_DIRECTION_L_TO_R,
                FALSE);
    received[test_num++] = XmStringWidth(fl , cs );

/*  Test 4 */ 
    cs1 = XmStringSegmentCreate ( test_strings[test_num], 
                XmSTRING_DEFAULT_CHARSET,
                XmSTRING_DIRECTION_L_TO_R,
                FALSE);
    for (i = 0; i < 5; i++) {
        temp = cs1;
	cs1 = XmStringConcat( cs, temp);
        XmStringFree( temp );
    }
    received[test_num++] = XmStringWidth(fl , cs1 );
    XmStringFree( cs );
    XmStringFree( cs1 );

/*  Test 5 */
    cs = XmStringCreate ( test_strings[test_num], XmSTRING_DEFAULT_CHARSET );
    received[test_num++] = XmStringWidth(fl , cs );
    XmStringFree( cs );

/*  Test 6 */
    cs1 = XmStringDirectionCreate( XmSTRING_DIRECTION_L_TO_R );
    received[test_num++] = XmStringWidth(fl , cs1 );
    XmStringFree( cs1 );

/*  Test 7 */ 
    cs2 = XmStringDirectionCreate( XmSTRING_DIRECTION_REVERT );
    received[test_num++] = XmStringWidth(fl , cs2 );
    XmStringFree( cs2 );

    printf ("Summary of test results\n");
    printf ("Test #\tExpected\tReceived\tStatus\n\n");
    for (i = 0; i < test_num; i++) {
      printf ("%6d\t%8d\t%8d\t%s\n",     i + 1,
	                              (int) expected[i],
	                              (int) received[i],
	                              ((expected[i] == received[i]) 
				                   ? "PASSED\n" : "FAILED\n"));
    }

    fflush (stdout);

    CommonPause();
    XtAppMainLoop( app_context );
}

