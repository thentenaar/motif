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
static char rcsid[] = "$XConsortium: DumpHier.c /main/9 1995/07/14 10:56:08 drk $"
#endif
#endif

#include "testlib.h"
#include <X11/IntrinsicP.h>

void DumpHierarchy(Widget w, FILE *file, int space);

/*************************************************************************
*									 *
*	Name:		CommonDumpHierarchy()				 *
*									 *
*	Function:	This function will take a widget, and a FILE   	 *
*			pointer to indicate where to write the output.	 *
*			The function will print out all widgets names    *
*			underneath that widget.			 	 *
*									 *
*	Returns:	Nothing						 *
*									 *
*									 *
*************************************************************************/

void CommonDumpHierarchy(Widget w, FILE *file)

{
	DumpHierarchy(w, file, 0);
}


void DumpHierarchy(Widget w, FILE *file, int space)

{
	int		i, j;
	CompositeWidget	cw;

	for (j = 0 ; j < space ; j++)
		fprintf(file, " ");

	if (XtIsComposite(w))
	{
		cw = (CompositeWidget)w;
		fprintf(file, "%s(parent)\n",XtName((Widget)cw));
		for(i = 0; i < cw->composite.num_children; i++)
		{
			DumpHierarchy(cw->composite.children[i], file, 
				      (space + 2));
		}
	}
	else
	{
		fprintf(file, "%s(leaf)\n",XtName(w));
	}

}
