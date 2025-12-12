/* $XConsortium: Notebook.h /main/5 1995/07/15 20:53:41 drk $ */
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

#ifndef _XmNotebook_h
#define _XmNotebook_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmNotebookWidgetClass;

typedef struct _XmNotebookClassRec *XmNotebookWidgetClass;
typedef struct _XmNotebookRec *XmNotebookWidget;


/************************************************************************
 *  Notebook Defines
 ************************************************************************/

/* XmNotebookPageStatus */
typedef enum
{
    XmPAGE_FOUND,		/* page widget found */
    XmPAGE_INVALID,		/* page number out of the range */
    XmPAGE_EMPTY,		/* no page widget found */
    XmPAGE_DUPLICATED		/* there are more than one page widgets */
} XmNotebookPageStatus;

/* Notebook page information structure */
typedef struct
{
    int         page_number;
    Widget      page_widget;
    Widget	status_area_widget;
    Widget      major_tab_widget;
    Widget	minor_tab_widget;
} XmNotebookPageInfo;


/************************************************************************
 *  Public Functions
 ************************************************************************/

#ifndef XmIsNotebook
#define XmIsNotebook(w) XtIsSubclass((w), xmNotebookWidgetClass)
#endif

extern Widget XmCreateNotebook(
			Widget		parent,
			String		name,
			ArgList		arglist,
			Cardinal	argcount);

extern XmNotebookPageStatus XmNotebookGetPageInfo(
			Widget notebook,
			int page_number,
			XmNotebookPageInfo *page_info) ;

extern Widget XmVaCreateNotebook(
                        Widget parent,
                        char *name,
                        ...);

extern Widget XmVaCreateManagedNotebook(
                        Widget parent,
                        char *name,
                        ...);


#ifdef __cplusplus
}
#endif

#endif /* _XmNotebook_h  */

