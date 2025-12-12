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
#ifndef _XmMenuShellP_h
#define _XmMenuShellP_h

#include <Xm/MenuShell.h>
#include <Xm/XmP.h>
#include <X11/ShellP.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The MenuShell instance record */

typedef struct 
{
    unsigned char	    focus_policy;
    XmFocusData		    focus_data;
    Boolean                 private_shell;
    XmFontList      	    default_font_list;
    XmFontList              button_font_list;
    XmFontList              label_font_list;
    XmDirection             layout_direction;
    Boolean                 animate;
} XmMenuShellPart;


/* Full instance record declaration */

typedef  struct _XmMenuShellRec 
{
    CorePart		    core;
    CompositePart	    composite;
    ShellPart		    shell;
    OverrideShellPart	    override;
    XmMenuShellPart	    menu_shell;
} XmMenuShellRec;

typedef  struct _XmMenuShellWidgetRec /* OBSOLETE (for compatibility only).*/
{
    CorePart		    core;
    CompositePart	    composite;
    ShellPart		    shell;
    OverrideShellPart	    override;
    XmMenuShellPart	    menu_shell;
} XmMenuShellWidgetRec;



/* MenuShell class structure */

typedef struct 
{
    XtActionProc    popdownOne;	     /* unpost portion or all of menu */
    XtActionProc    popdownEveryone; /* unpost portion of menu */
    XtActionProc    popdownDone;     /* unpost all menus */
    XmMenuPopupProc popupSharedMenupane; /* post shared menus */
				         /* (should be popupSharedMenuShell) */
    XtPointer	    extension;	     /* Pointer to extension record */
} XmMenuShellClassPart;


/* Full class record declaration */

typedef struct _XmMenuShellClassRec 
{
    CoreClassPart	    core_class;
    CompositeClassPart	    composite_class;
    ShellClassPart	    shell_class;
    OverrideShellClassPart  override_shell_class;
    XmMenuShellClassPart    menu_shell_class;
} XmMenuShellClassRec;


externalref XmMenuShellClassRec  xmMenuShellClassRec;

#define MS_FocusPolicy(m) \
	(((XmMenuShellWidget)m)->menu_shell.focus_policy)

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmMenuShellP_h */
/* DON'T ADD STUFF AFTER THIS #endif */
