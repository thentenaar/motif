/* $XConsortium: StringToByte.c /main/5 1995/07/15 21:13:08 drk $ */
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

#include <testlib.h>

void
main( int argc, char **argv )
{
	XmString	string1, string2;
	unsigned char	*byte_stream;
	unsigned int	length1, length2, length3;
        Boolean         same;

     CommonTestInit( argc, argv );

    /*
     *  Create an XmString.
     *  Convert the XmString to a byte stream and back again.
     *  Verify that the converted XmString is the same as the
     *  original.
     */
     string1 = XmStringCreateLtoR("Original", XmFONTLIST_DEFAULT_TAG ); 

     length1 = XmCvtXmStringToByteStream( string1, &byte_stream );

     string2 = XmCvtByteStreamToXmString( byte_stream ); 

     same = XmStringCompare( string1, string2 );

     printf("XmCvtXmStringToByteStream with non-Null byte stream parameter: ");
     if ( same )
        printf("passed.");
     else
        printf("failed.");

     printf("\n\n");


    /*
     *  Verify that the correct length is returned when the byte
     *  stream ( second ) parameter is passed in as Null.
     *  The function should return the length in this instance.
     */
     length2 = XmCvtXmStringToByteStream( string1, (unsigned char **)0 );

     printf("XmCvtXmStringToByteStream with Null byte stream parameter: ");
     if ( length1 == length2 )
        printf("passed.");
     else
        printf("failed.");

     printf("\n\n");

     length3 = XmStringByteStreamLength( byte_stream );  

     printf("XmStringByteStreamLength: ");
     if ( length1 == length3 )
        printf("passed.");
     else
        printf("failed.");

     printf("\n\n");

     CommonPause();

     XmStringFree( string1 );
     XmStringFree( string2 );
     XtFree( byte_stream );
     XtAppMainLoop( app_context );
}
