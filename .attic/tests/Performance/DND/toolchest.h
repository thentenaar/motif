/* $XConsortium: toolchest.h /main/5 1995/07/15 21:01:53 drk $ */
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


#include "ChRedef.h"
/* #include "sgi_visual.h" */
#include <X11/Intrinsic.h>

/* Chests menu description file pathnames: */

#define HOME_CHESTRC "/.chestrc"
#define SYS_CHESTRC  "/usr/lib/X11/system.chestrc"
#define CHEST_SUFFIX ".chest"
#define CHEST_SUFFIX_LENGTH 6

/* name of the top level menu */
#define TOP_MENU_NAME "ToolChest"

/* menu entry types */
enum menuEntryType {ME_NONE, ME_EXEC, ME_CHECKEXEC, ME_CHECKEXPR, ME_SEPARATOR,
		  ME_LABEL, ME_TITLE, ME_MENU};

struct menuEntry
{
    char *label;
    enum menuEntryType type;
    char *menuName;	/* these two fields could be unionized if this gets */
    char *execString;	/* more complicated				    */
    char *exprString;
    Boolean sensitive:1;
    Boolean removed:1;	/* explicitly removed */
    Boolean empty:1;	/* empty menu */
    struct menuEntry*next, *prev;
#ifdef TCMENU
    struct menu *containingMenu;
    Widget entryWidget;
#endif /*TCMENU */
};

struct menu
{
    char *name;
    struct menuEntry *firstMenuEntry;
    struct menuEntry *lastMenuEntry;
    Boolean referenced:1;
    struct menu *next;
#ifdef TCMENU
    struct menuEntry *activeEntry;
    Widget menuWidget;
#endif    
};
    

extern int lineno;
extern char *progName;

/* Visual information */
extern Boolean menuVisualStatus;	/* overall visual status */
extern char * menuVisualType;		/* visual type*/
extern int menuVisualDepth;		/* depth of visual */
extern Colormap menuVisualColormap;	/* the associated colormap */
extern Visual *menuVisual;		/* the visual itself */
extern Window wm_windows[2];	/* windows for XSetWMColormapWindows */


/* Decal information */
extern Boolean showDecal;		/* True if the decal should be shown */
extern Pixel decalForeground;		/* foreground color of Decal */

/* functions */
extern void tcFreeAll();


extern void * tcMalloc(size_t);
extern void FileError();
extern void AppendFile(char *name);
extern void IncludeFile(char *name, Boolean warnIfMissing);
extern char NextChar();
extern void PrevChar();
extern char *SaveChar();
extern void NullChar();
extern char *UnquoteString(char *string);

/* menu functions */
extern struct menuEntry *NewMenuEntry(char *label, enum menuEntryType type);
extern struct menu *FindMenu(char *name, Boolean mallocIt);
extern void RemoveMenuEntry(char *name, char *menuName);
extern void BuildTopMenu(Widget parent, Boolean isMenuBar);

/* execution functions */
extern void ExecCallback (Widget widget, struct menuEntry *client_data,
			  caddr_t call_data);
extern Boolean CheckExec(char *command);
extern Boolean CheckExpr(char *expr);



/* FROM globals.c */
int lineno;	/* current line number */
char *progName;

/* Visual information */
Boolean menuVisualStatus;	/* overall visual status */
char * menuVisualType;		/* visual type*/
int menuVisualDepth;		/* depth of visual */
Colormap menuVisualColormap;	/* the associated colormap */
Visual *menuVisual;		/* the visual itself */
Window wm_windows[2];	/* windows for XSetWMColormapWindows */

/* Decal information */
Boolean showDecal;		/* True if the decal should be shown */
Pixel decalForeground;		/* foreground color of Decal */
