/* $XConsortium: tcedit.c /main/5 1995/07/15 21:01:48 drk $ */
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


#include <stdio.h>
#include <Xm/MainW.h>
#include <Xm/ScrolledW.h>
#include <Xm/BulletinB.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>
#include <Xm/TextF.h>
#include <Xm/Frame.h>
#include <Xm/CascadeBG.h>
#include <Xm/PushBG.h>
#include <Xm/DrawingA.h>
#include "toolchest.h"

static Widget menuParent;

static Pixmap decalPixmap;
static Pixmap arrowPixmap;
static Pixmap nullPixmap;

static Pixel menuBackground, menuForeground;
static XmFontList menuFontList;
static Dimension menuShadowThickness;

#define decal_width 18
#define decal_height 16
static char decal_bits[] = {
   0xf0, 0x7f, 0x00, 0x10, 0x40, 0x00, 0x10, 0xc0, 0x00, 0xf0, 0xff, 0x00,
   0xc0, 0xff, 0x00, 0xf0, 0x7f, 0x00, 0x10, 0x40, 0x00, 0x10, 0xc0, 0x00,
   0x10, 0xc0, 0x00, 0x10, 0xc0, 0x00, 0x10, 0xc0, 0x00, 0x10, 0xc0, 0x00,
   0x10, 0xc0, 0x00, 0x10, 0xc0, 0x00, 0xf0, 0xff, 0x00, 0xc0, 0xff, 0x00};

#define arrow_width 16
#define arrow_height 16
static char arrow_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x38, 0x00, 0xc8, 0x00, 0x08, 0x03,
   0x08, 0x0c, 0x08, 0x30, 0x08, 0x70, 0x08, 0x3c, 0x08, 0x0f, 0xc8, 0x03,
   0xf8, 0x00, 0x38, 0x00, 0x08, 0x00, 0x00, 0x00};

#define null_width 16
#define null_height 16
static char null_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


static Widget createMenuWidget(char *menuName, Boolean isTop);
static void manageMenuWidget(Widget w, Position x, Position y);
static void makeDecalPixmap(Widget w);

extern void CreatePanel(Widget parent);
extern void UpdatePanel(struct menu *menu);

static void
ParseCmdLine(int argc, char **argv)
{
    register int i;
    
    if (argc == 1)
	AppendFile(NULL);
    else
    {
	for (i=1; i<argc; i++)
	{
	    if (argv[i][0] == '+' && argv[i][1] == '\0')
		AppendFile(NULL);
	    else
		AppendFile(argv[1]);
	}
    }
}

static XtCallbackProc
labelChangedCB(Widget w, caddr_t client_data, caddr_t call_data)
{
    /* a workaround for text widget not behaving with variable fonts */
    XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0, TRUE);
}

static XtCallbackProc
labelSetCB(Widget w, caddr_t client_data, caddr_t call_data)
{
}

static XtCallbackProc
decalPressedCB(Widget w, Widget client_data, caddr_t call_data)
{
    XmProcessTraversal(client_data, XmTRAVERSE_CURRENT);
    XmTextFieldSetInsertionPosition(client_data, 9999);
}

static void
deactivateEntry(register struct menuEntry *entry)
{
    if (entry->type == ME_MENU)
    {
	struct menu *menu = FindMenu(entry->menuName, FALSE);
	if (menu && menu->menuWidget)
	{
	    if (menu->activeEntry)
	    {
		deactivateEntry(menu->activeEntry);
		menu->activeEntry = NULL;
	    }
	    XtUnmanageChild(menu->menuWidget);
	}
    }
}

static void
activateEntry(register struct menuEntry *entry)
{
    register struct menu *menu = entry->containingMenu;
    Arg args[5];
    int n;
    Position menuX, menuY;
    Position entryX, entryY;
    Dimension entryWidth;

    if (menu->activeEntry)
    {
	if (menu->activeEntry == entry)
	    return;
	deactivateEntry(menu->activeEntry);
    }
    menu->activeEntry = entry;
    if (entry->type == ME_MENU)
    {
	Widget menuWidget = createMenuWidget(entry->menuName, FALSE);

	/* get positions of menu and entry */
	n = 0;
	XtSetArg (args[n], XmNx, &menuX); n++;
	XtSetArg (args[n], XmNy, &menuY); n++;
	XtGetValues (menu->menuWidget, args, n);
	n = 0;
	XtSetArg (args[n], XmNx, &entryX); n++;
	XtSetArg (args[n], XmNy, &entryY); n++;
	XtSetArg (args[n], XmNwidth, &entryWidth); n++;
	XtGetValues (entry->entryWidget, args, n);

	manageMenuWidget(menuWidget, menuX+entryX+entryWidth, menuY+entryY);
    }
}

static XtCallbackProc
activateEntryCB(Widget w, struct menuEntry *client_data, caddr_t call_data)
{
    activateEntry(client_data);
}

static XtEventHandler
activateEntryEH(Widget w, struct menuEntry *client_data, XEvent *event)
{
    if (event->xbutton.button == Button1)
	activateEntry(client_data);
}

static Widget createMenuWidget(char *menuName, Boolean isTop)
{
    Widget menuWidget;
    Widget w;
    Widget form,decal;
    register struct menu *menu;
    register struct menuEntry *entry;
    enum menuEntryType lastType = ME_NONE;
    Boolean thisEntryRemoved = FALSE;
    Boolean lastEntryRemoved;
    Arg args[20];
    int n;
    Widget prev;
    XtCallbackProc labelChangedCB(), labelSetCB(), activateEntryCB(), decalPressedCB();
    XtEventHandler activateEntryEH();

    menu = FindMenu(menuName, TRUE);

    if (menu->menuWidget)
	return (menu->menuWidget);
    
    n = 0;
    XtSetArg (args[n], XmNshadowType, XmSHADOW_OUT);n++;
    XtSetArg (args[n], XmNshadowThickness, menuShadowThickness);n++;
    menuWidget = XmCreateForm(menuParent, "menuBar", args, n);
    /* don't manage at this point */

    prev = NULL;
    for (entry = menu->firstMenuEntry; entry; entry = entry->next)
    {
	n = 0;
	if (prev)
	{
	    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);n++;
	    XtSetArg (args[n], XmNtopWidget, prev);n++;
	}
	else
	{
	    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);n++;
	    XtSetArg (args[n], XmNtopOffset, menuShadowThickness);n++;
	}
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);n++;
	XtSetArg (args[n], XmNleftOffset, menuShadowThickness);n++;
	XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);n++;
	XtSetArg (args[n], XmNrightOffset, menuShadowThickness);n++;
	
	lastEntryRemoved = thisEntryRemoved;
	if (entry->removed)
	{
	    thisEntryRemoved = TRUE;
	}
	else
	{
	    thisEntryRemoved = FALSE;
	    switch (entry->type)
	    {
	    case ME_EXEC:
	    case ME_CHECKEXEC:
	    case ME_CHECKEXPR:
	    case ME_MENU:
	    case ME_LABEL:
	    case ME_TITLE:
		XtSetArg (args[n], XmNbackground, menuBackground); n++;
		XtSetArg (args[n], XmNforeground, menuForeground); n++;
		XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
		form = XmCreateForm(menuWidget, entry->label, args, n);
		XtManageChild(form);
		prev = form;
		entry->entryWidget = form;
		n = 0;
		XtSetArg (args[n], XmNtraversalOn, FALSE);n++;
		XtSetArg (args[n], XmNshadowThickness, 0);n++;
		XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);n++;
		XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);n++;
		XtSetArg (args[n], XmNlabelType, XmPIXMAP);n++;
		if (entry->type == ME_MENU)
		{
		    XtSetArg (args[n], XmNlabelPixmap,
			      isTop?decalPixmap:arrowPixmap);n++;
		}
		else
		{
		    XtSetArg (args[n], XmNlabelPixmap, nullPixmap);n++;
		}
		decal = XmCreatePushButtonGadget(form, "decal", args, n);
		XtManageChild(decal);
		n = 0;
		XtSetArg (args[n], XmNvalue, entry->label);n++;
		XtSetArg (args[n], XmNshadowThickness, 0);n++;
		XtSetArg (args[n], XmNbackground, menuBackground); n++;
		XtSetArg (args[n], XmNforeground, menuForeground); n++;
		XtSetArg (args[n], XmNfontList, menuFontList); n++;
		XtSetArg (args[n], XmNresizeWidth, TRUE); n++;
		XtSetArg (args[n], XmNcolumns, 1); n++; /*actually grows */
		XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);n++;
		XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);n++;
		XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);n++;
		XtSetArg (args[n], XmNrightWidget, decal);n++;
		w = XmCreateTextField(form, entry->label, args, n);
		XtAddCallback (w, XmNvalueChangedCallback, (XtCallbackProc) labelChangedCB,
			       NULL);
		XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) labelSetCB,
			       NULL);
		XtAddCallback (w, XmNactivateCallback, (XtCallbackProc) activateEntryCB,
			       entry);
		XtAddEventHandler (w, ButtonPressMask, FALSE, (XtEventHandler) activateEntryEH,
			       entry);
		XtManageChild(w);

		XtAddCallback (decal, XmNactivateCallback, (XtCallbackProc) decalPressedCB, w);
		XtAddCallback (decal, XmNactivateCallback, (XtCallbackProc) activateEntryCB,
			       entry);
		if (entry->type == ME_TITLE)
		{
		    n = 0;
		    XtSetArg (args[n], XmNseparatorType, XmDOUBLE_LINE); n++;
		    w = XmCreateSeparatorGadget (menuWidget, "separator",
						 args, n); n++;
		    XtManageChild(w);
		    prev = w;
		}
		break;
	    case ME_SEPARATOR:
		/* if we removed the last entry and the last non
		 * removed entry was a separator, a title, or the start,
		 * remove this entry.  However, don't set thisEntryRemoved
		 * so that if another separator immediatly follows, it is
		 * drawn.
		 */
		if (!(lastEntryRemoved &&
		    (lastType == ME_SEPARATOR ||
		     lastType == ME_TITLE ||
		     lastType == ME_NONE)))
		{
		    w = XmCreateSeparatorGadget(menuWidget, entry->label,
						args, n);
		    XtManageChild(w);
		    prev = w;
		    entry->entryWidget = w;
		}
		break;
	    default:
		XtError ("Unknown menu item type");
	    }
	}
    }
    if (prev)
    {
	n = 0;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);n++;
	XtSetArg (args[n], XmNbottomOffset, menuShadowThickness);n++;
	XtSetValues(prev, args, n);
    }
    menu->menuWidget = menuWidget;
    return (menuWidget);
}


static void
manageMenuWidget(Widget w, Position x, Position y)
{
    XtMoveWidget(w, x, y);
    XtManageChild(w);
}

static XtCallbackProc
quitCB(Widget w, caddr_t client_data, caddr_t call_data)
{
    exit (0);
}

static Widget
createMenuBar(Widget parent)
{
    Widget menuBar;
    Widget cascade;
    Widget button;
    Widget pane;
    Arg args[5];
    int n;
    XtCallbackProc quitCB();

    n = 0;
    menuBar = XmCreateMenuBar(parent, "menuBar", args, n);
    XtManageChild (menuBar);

    n = 0;
    pane = XmCreatePulldownMenu (menuBar, "filePane", args, n);

    n = 0;
    button = XmCreatePushButtonGadget (pane, "menuQuit", args, n);
    XtManageChild (button);
    XtAddCallback (button, XmNactivateCallback, (XtCallbackProc) quitCB, NULL);

    n = 0;
    XtSetArg (args[n], XmNsubMenuId, pane); n++;
    cascade = XmCreateCascadeButtonGadget (menuBar, "menuFile", args, n);
    XtManageChild (cascade);

    /* sqirrel away menu button font, background, foreground & shadow
     * thickness.  Get the fontlist from a button in the menu
     */
    n = 0;
    XtSetArg (args[n], XmNfontList, &menuFontList); n++;
    XtGetValues(cascade, args, n);

    /* the background and foreground must come from the menubar, because
     * the button is a gadget.  Also, only the menu itself has a shadow
     */
    n = 0;
    XtSetArg (args[n], XmNbackground, &menuBackground); n++;
    XtSetArg (args[n], XmNforeground, &menuForeground); n++;
    XtSetArg (args[n], XmNshadowThickness, &menuShadowThickness); n++;
    XtGetValues(menuBar, args, n);

    makeDecalPixmap(menuBar);

    return (menuBar);
}

static void
createTcEdit(Widget toplevel)
{
    Arg     args[20];
    int     n;
    Widget  form;
    Widget  mainw;
    Widget  menuBar;
    Widget  scrolledw;
    Widget  topMenu;

    n = 0;
    mainw = XmCreateMainWindow(toplevel, "mainWindow", args, n);
    XtManageChild(mainw);

    menuBar = createMenuBar(mainw);

    n = 0;
    form = XmCreateForm(mainw, "topForm", args, n);
    XtManageChild(form);

    n = 0;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);n++;
    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);n++;
    XtSetArg (args[n], XmNscrollingPolicy, XmAUTOMATIC);n++;
    scrolledw = XmCreateScrolledWindow(form, "menuScroll", args, n);
    XtManageChild(scrolledw);

    /* because of the way schemes work, drawing areas are the same color
     * as a scrolled window, so put menus in a drawing area
     */
    n = 0;
    menuParent = XmCreateDrawingArea(scrolledw, "menuParent", args, n);
    XtManageChild(menuParent);

    XmMainWindowSetAreas(mainw, menuBar, NULL, NULL, NULL, form);

    topMenu =  createMenuWidget(TOP_MENU_NAME, TRUE);
    manageMenuWidget(topMenu, 10, 10);
}

void
main (int argc, char **argv)
{
    Arg     wargs[10];
    Widget  toplevel;
    int     n;

    progName = argv[0];

    toplevel = XtInitialize(argv[0], "Tcedit", NULL, 0, &argc, argv);

    ParseCmdLine(argc, argv);

    yyparse();

    createTcEdit(toplevel);

    XtRealizeWidget(toplevel);
    XtMainLoop();
}

static void
makeDecalPixmap(Widget w)
{
    int n;
    Arg wargs[3];
    GC  gc;
    XGCValues    values;
    static Boolean initd = FALSE;
    Pixmap bitmap;

    if (initd)
	return;
    initd = TRUE;

    n = 0;
    XtSetArg(wargs[n], XmNforeground, &values.foreground);n++;
    XtSetArg(wargs[n], XmNbackground, &values.background);n++;
    XtGetValues(w, wargs, n);

    gc = XtGetGC(w, GCForeground | GCBackground, &values);

    bitmap = XCreateBitmapFromData(XtDisplay(w), 
					  RootWindowOfScreen(XtScreen(w)),
					  decal_bits, decal_width, decal_height);
    decalPixmap = XCreatePixmap(XtDisplay(w), 
			       RootWindowOfScreen(XtScreen(w)),
			       decal_width, decal_height,
			       DefaultDepthOfScreen(XtScreen(w)));

    XCopyPlane(XtDisplay(w), bitmap, decalPixmap, gc, 0, 0, 
	       decal_width, decal_height, 0, 0, 1); 

    XFreePixmap(XtDisplay(w), bitmap);

    bitmap = XCreateBitmapFromData(XtDisplay(w), 
					  RootWindowOfScreen(XtScreen(w)),
					  arrow_bits, arrow_width, arrow_height);
    arrowPixmap = XCreatePixmap(XtDisplay(w), 
			       RootWindowOfScreen(XtScreen(w)),
			       arrow_width, arrow_height,
			       DefaultDepthOfScreen(XtScreen(w)));

    XCopyPlane(XtDisplay(w), bitmap, arrowPixmap, gc, 0, 0, 
	       arrow_width, arrow_height, 0, 0, 1); 

    XFreePixmap(XtDisplay(w), bitmap);

    bitmap = XCreateBitmapFromData(XtDisplay(w), 
					  RootWindowOfScreen(XtScreen(w)),
					  null_bits, null_width, null_height);
    nullPixmap = XCreatePixmap(XtDisplay(w), 
			       RootWindowOfScreen(XtScreen(w)),
			       null_width, null_height,
			       DefaultDepthOfScreen(XtScreen(w)));

    XCopyPlane(XtDisplay(w), bitmap, nullPixmap, gc, 0, 0, 
	       null_width, null_height, 0, 0, 1); 

    XFreePixmap(XtDisplay(w), bitmap);

    XtReleaseGC(w, gc);
}

