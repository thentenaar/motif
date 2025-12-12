/* $XConsortium: MenuBP.h /main/4 1995/07/15 20:41:06 drk $ */
/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option)
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

#ifndef _ExmMenuBP_h
#define _ExmMenuBP_h

#include <Exm/MenuB.h>
#include <Exm/ExmStringP.h>

typedef struct _ExmMenuButtonClassPart
{
	XtPointer		extension;
} ExmMenuButtonClassPart;

typedef struct _ExmMenuButtonClassRec
{
	CoreClassPart        	core_class;
	XmPrimitiveClassPart	primitive_class;
	ExmSimpleClassPart	simple_class;
	ExmStringClassPart	string_class;
	ExmMenuButtonClassPart	menu_button_class;
} ExmMenuButtonClassRec;

extern ExmMenuButtonClassRec exmMenuButtonClassRec;

typedef struct _ExmMenuButtonPart
{
	XtCallbackList		activate_callback;

	Boolean			armed;
	Boolean			skip_callback;
	XtIntervalId		visual_timer;

	KeySym			mnemonic;
	XmStringCharSet		mnemonic_charset;
	char*			accelerator;
	XmString		accelerator_text;
} ExmMenuButtonPart;


typedef struct _ExmMenuButtonRec
{
	CorePart		core;
	XmPrimitivePart		primitive;
	ExmSimplePart		simple;
	ExmStringPart		string;
	ExmMenuButtonPart	menu_button;
} ExmMenuButtonRec;


/* Define the subclassing level index to be used with ResolvePartOffset */
#define ExmMenuButtonIndex (ExmCommandIndex + 1)

/********    Private Function Declarations    ********/
/********    End Private Function Declarations    ********/


#endif /* _ExmMenuBP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
