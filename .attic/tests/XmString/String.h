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
/*   $XConsortium: String.h /main/7 1995/07/13 19:53:28 drk $ */

#define	MAX_STRINGS	22
#define	MAX_TEXTS	8

XmString	string[MAX_STRINGS] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

char		*text[MAX_TEXTS] =
{
    "This is\nString1.",
    "String3 here",
    "String5\n",
    "String5",
    "Any old\ntext.",
    "This is the text",
	"This is to test PIR3933",
    "This tests\n\n\nPIR7403",
};

XmStringCharSet	charset[5] =
{
#ifndef MOTIF1_1
    (XmStringCharSet) XmFONTLIST_DEFAULT_TAG,
#else
    (XmStringCharSet) XmSTRING_DEFAULT_CHARSET,
#endif
    NULL,
    "ISOLatin1",
    "MyCharset",
    (XmStringCharSet) XmFALLBACK_CHARSET
};

char		*charsetname[5] =
{
#ifndef MOTIF1_1
	"XmFONTLIST_DEFAULT_TAG",
#else
    "XmSTRING_DEFAULT_CHARSET",
#endif
    "NULL",
    "ISOLatin1",
    "MyCharset",
    "XmFALLBACK_CHARSET"
};

char		error_string[120];

int		string_num,
		text_num,
		charset_num,
		test_num,
		errors;
