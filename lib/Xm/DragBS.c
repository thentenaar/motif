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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: DragBS.c /main/29 1998/03/18 15:10:28 csn $"
#endif
#endif
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


/*****************************************************************************
 *
 *  The purpose of the routines in this module is to cache frequently needed
 *  data on window properties to reduce roundtrip server requests.
 *
 *  The data is stored on window properties of motifWindow, a persistent,
 *  override-redirect, InputOnly child window of the display's default root
 *  window.  A client looks for the motifWindow id on the "_MOTIF_DRAG_WINDOW"
 *  property of the display's default root window.  If it finds the id, the
 *  client saves it in its displayToMotifWindowContext.  Otherwise, the
 *  client creates the motifWindow and stores its id on that root property
 *  and saves the id in its displayToMotifWindowContext.  MotifWindow is
 *  mapped but is not visible on the screen.
 *
 *  Two sets of data are stored on motifWindow properties:
 *
 *    1. an atom table, and
 *    2. a targets list table.
 *
 *  The "_MOTIF_DRAG_ATOMS" property on motifWindow contains an atoms table,
 *  consisting of pairs of atoms and timestamps.  The atoms are interned
 *  once and are available for clients to use without repeated roundtrip
 *  server requests to intern them.  A timestamp of 0 indicates that the
 *  atom is available.  A nonzero timestamp indicates when the atom was last
 *  allocated to a client.  The atoms table initially contains only atom
 *  "_MOTIF_ATOM_0" with timestamp 0.  A client requests an atom by calling
 *  _XmAllocMotifAtom() with a timestamp.  _XmAllocMotifAtom() tries to find
 *  an available atom in the table.  If it succeeds it sets the atom's
 *  timestamp to the value specified and returns the atom.  If no atom is
 *  available, _XmAllocMotifAtom() adds an atom to the table with the
 *  specified timestamp, updates the "_MOTIF_DRAG_ATOMS" property on
 *  motifWindow, and returns the new atom.  These new atoms are named
 *  "_MOTIF_ATOM_n" where n is 1, 2, 3, ... .  The routine _XmGetMotifAtom()
 *  returns the atom from the atoms table with nonzero timestamp less than
 *  but closest to a specified value.  It does not change the atoms table.
 *  A client frees an atom by calling _XmFreeMotifAtom(), which sets the
 *  atom's timestamp to 0 and updates the "_MOTIF_DRAG_ATOMS" property on
 *  motifWindow.  To minimize property access, the client saves the address
 *  of its current atoms table on the displayToAtomsContext context.
 *
 *  The "_MOTIF_DRAG_TARGETS" property on motifWindow contains a targets
 *  table, consisting of a sequence of target lists to be shared among
 *  clients.  These target lists are sorted into ascending order to avoid
 *  permutations.  By sharing the targets table, clients may pass target
 *  lists between themselves by passing instead the corresponding target
 *  list indexes.  The routine _XmInitTargetsTable() initializes the atoms
 *  table on the "_MOTIF_DRAG_ATOMS" property, then initializes the targets
 *  table on the "_MOTIF_DRAG_TARGETS" property to contain only two lists:
 *
 *		{ 0,		}, and
 *		{ XA_STRING,	}
 *
 *  A client adds a target list to the targets table by passing the target
 *  list to _XmTargetsToIndex().  _XmTargetsToIndex() first sorts the target
 *  list into ascending order, then searches the targets table for a match.
 *  If it finds a match, it returns the index of the matching targets table
 *  entry.  Otherwise, it adds the sorted target list to the table, updates
 *  the "_MOTIF_DRAG_TARGETS" property on motifWindow, and returns the index
 *  of the new targets table entry.  A client uses _XmIndexToTargets() to
 *  map a targets table index to a target list.  To minimize property access,
 *  the client saves the address of its current targets table on the
 *  displayToTargetsContext context.
 *
 *  The "_MOTIF_DRAG_PROXY_WINDOW" property on motifWindow contains the
 *  window id of the DragNDrop proxy window.  The routine
 *  _XmGetDragProxyWindow() returns the window id stored there.
 ***************************************************************************/

#include <Xm/XmP.h>
#include "XmI.h"
#include "DragICCI.h"
#include "DragBSI.h"
#include "MessagesI.h"
#include <Xm/MwmUtil.h>

#include <X11/Xatom.h>
#include <X11/Xresource.h>

#include <stdio.h>
#include <stdlib.h>

#undef _XmIndexToTargets
#undef _XmTargetsToIndex

#define MAXSTACK	1200
#define MAXPROPLEN	100000L

/* structures to improve portability of WriteTargets */
typedef struct {
    CARD32 value B32;
} CARD32Item;

typedef struct {
    CARD16 value B16;
    CARD16 pad B16;
} CARD16Item;

#define MESSAGE1	_XmMMsgDragBS_0000
#define MESSAGE2	_XmMMsgDragBS_0001
#define MESSAGE3	_XmMMsgDragBS_0002
#define MESSAGE4	_XmMMsgDragBS_0003
#define MESSAGE5	_XmMMsgDragBS_0004
#define MESSAGE6	_XmMMsgDragBS_0005
#define MESSAGE7	_XmMMsgDragBS_0006


/********    Static Function Declarations    ********/

static int LocalErrorHandler(
                        Display *display,
                        XErrorEvent *error) ;
static void StartProtectedSection(
                        Display *display,
                        Window window) ;
static void EndProtectedSection(
                        Display *display) ;
static Window GetMotifWindow(
                        Display *display) ;
static void SetMotifWindow(
                        Display *display,
                        Window motifWindow) ;
static xmTargetsTable GetTargetsTable(
                        Display *display) ;
static void SetTargetsTable(
                        Display *display,
                        xmTargetsTable targetsTable) ;
static xmAtomsTable GetAtomsTable(
                        Display *display) ;
static void SetAtomsTable(
                        Display *display,
                        xmAtomsTable atomsTable) ;
static Window ReadMotifWindow(
                        Display *display) ;
static Window CreateMotifWindow(
                        Display *display) ;
static void WriteMotifWindow(
                        Display *display,
                        Window *motifWindow) ;
static void WriteAtomsTable(
                        Display *display,
                        xmAtomsTable atomsTable) ;
static Boolean ReadAtomsTable(
                        Display *display,
                        xmAtomsTable atomsTable) ;
static void WriteTargetsTable(
                        Display *display,
                        xmTargetsTable targetsTable) ;
static Boolean ReadTargetsTable(
                        Display *display,
                        xmTargetsTable targetsTable) ;
static xmTargetsTable CreateDefaultTargetsTable(
                        Display *display) ;
static xmAtomsTable CreateDefaultAtomsTable(
                        Display *display) ;
static int AtomCompare(
                        XmConst void *atom1,
                        XmConst void *atom2) ;

/********    End Static Function Declarations    ********/

static Boolean		bad_window;
static XErrorHandler	oldErrorHandler = NULL;
static unsigned long	firstProtectRequest;
static Window		errorWindow;

static XContext 	displayToMotifWindowContext = 0;
static XContext 	displayToTargetsContext = 0;
static XContext		displayToAtomsContext = 0;


/*****************************************************************************
 *
 *  LocalErrorHandler ()
 *
 ***************************************************************************/

static int
LocalErrorHandler(
        Display *display,
        XErrorEvent *error )
{
    int ret_val;

    _XmProcessLock();
    if (error->error_code == BadWindow &&
	error->resourceid == errorWindow &&
	error->serial >= firstProtectRequest) {
        bad_window = True;
	_XmProcessUnlock();
	return 0;
    }

    if (oldErrorHandler == NULL) {
	_XmProcessUnlock();
        return 0;  /* should never happen */
    }

    ret_val = (*oldErrorHandler)(display, error);
    _XmProcessUnlock();
    return ret_val;
}

/*****************************************************************************
 *
 *  StartProtectedSection ()
 *
 *  To protect against reading or writing to a property on a window that has
 *  been destroyed.
 ***************************************************************************/

static void
StartProtectedSection(
        Display *display,
        Window window )
{
    bad_window = False;
    oldErrorHandler = XSetErrorHandler (LocalErrorHandler);
    firstProtectRequest = NextRequest (display);
    errorWindow = window;
}

/*****************************************************************************
 *
 *  EndProtectedSection ()
 *
 *  Flushes any generated errors on and restores the original error handler.
 ***************************************************************************/

static void
EndProtectedSection(
        Display *display )
{
    XSync (display, False);
    XSetErrorHandler (oldErrorHandler);
    oldErrorHandler = NULL;
}

/*****************************************************************************
 *
 *  GetMotifWindow ()
 *
 *  Get the motifWindow id from the displayToMotifWindowContext.
 ***************************************************************************/

static Window
GetMotifWindow(
        Display *display )
{
    Window	motifWindow;
    XContext	loc_context;

    _XmProcessLock();
    if (!displayToMotifWindowContext)
        displayToMotifWindowContext = XUniqueContext();

    loc_context = displayToMotifWindowContext;
    _XmProcessUnlock();

    if (XFindContext(display,
                     DefaultRootWindow (display),
		     loc_context,
		     (char **)&motifWindow)) {
        motifWindow = None;
    }
    return (motifWindow);
}

/*****************************************************************************
 *
 *  SetMotifWindow ()
 *
 *  Set the motifWindow id into the displayToMotifWindowContext.
 ***************************************************************************/

static void
SetMotifWindow(
        Display *display,
        Window motifWindow )
{
    Window oldMotifWindow;
    XContext loc_context;

    _XmProcessLock();
    if (!displayToMotifWindowContext)
        displayToMotifWindowContext = XUniqueContext();

    loc_context = displayToMotifWindowContext;
    _XmProcessUnlock();

    /*
     * Save window data.
     * Delete old context if one exists.
     */
    if (XFindContext (display, DefaultRootWindow (display),
			loc_context,
			(char **) &oldMotifWindow)) {
	XSaveContext(display,
                        DefaultRootWindow (display),
			loc_context,
			(char *) motifWindow);
    }
    else if (oldMotifWindow != motifWindow) {
	XDeleteContext (display, DefaultRootWindow (display),
			loc_context);
	XSaveContext(display,
                        DefaultRootWindow (display),
			loc_context,
			(char *) motifWindow);
    }
}

/*****************************************************************************
 *
 *  GetTargetsTable ()
 *
 *  Get the targets table address from the displayToTargetsContext.
 ***************************************************************************/

static xmTargetsTable
GetTargetsTable(
        Display *display )
{
    xmTargetsTable	targetsTable;
    XContext		loc_context;

    _XmProcessLock();
    if (!displayToTargetsContext)
        displayToTargetsContext = XUniqueContext();

    loc_context = displayToTargetsContext;
    _XmProcessUnlock();

    if (XFindContext(display,
                     DefaultRootWindow (display),
		     loc_context,
		     (char **)&targetsTable)) {
        targetsTable = NULL;
    }
    return (targetsTable);
}

/*****************************************************************************
 *
 *  SetTargetsTable ()
 *
 *  Set the targets table address into the displayToTargetsContext.
 ***************************************************************************/

static void
SetTargetsTable(
        Display *display,
        xmTargetsTable targetsTable )
{
    xmTargetsTable oldTargetsTable;
    XContext	loc_context;

    _XmProcessLock();
    if (!displayToTargetsContext)
        displayToTargetsContext = XUniqueContext();

    loc_context = displayToTargetsContext;
    _XmProcessUnlock();

    /*
     * Save targets data.
     * Delete old context if one exists.
     */
    if (XFindContext (display, DefaultRootWindow (display),
			loc_context,
			(char **) &oldTargetsTable)) {
	if (targetsTable)
	XSaveContext(display,
                        DefaultRootWindow (display),
			loc_context,
			(char *) targetsTable);
    }
    else if (oldTargetsTable != targetsTable) {
	XDeleteContext (display, DefaultRootWindow (display),
			loc_context);
        {
          unsigned i = 0 ;
          while(    i < oldTargetsTable->numEntries    )
            {
              XtFree( (char *) oldTargetsTable->entries[i++].targets) ;
            }
          XtFree( (char *) oldTargetsTable->entries) ;
          XtFree( (char *) oldTargetsTable) ;
        }

	if (targetsTable)
	XSaveContext(display,
                        DefaultRootWindow (display),
			loc_context,
			(char *) targetsTable);
    }
}

/*****************************************************************************
 *
 *  GetAtomsTable ()
 *
 *  Get the atomsTable address from the displayToAtomsContext.
 ***************************************************************************/

static xmAtomsTable
GetAtomsTable(
        Display *display )
{
    xmAtomsTable	atomsTable;
    XContext		loc_context;

    _XmProcessLock();
    if (!displayToAtomsContext)
	displayToAtomsContext = XUniqueContext();

    loc_context = displayToAtomsContext;
    _XmProcessUnlock();

    if (XFindContext (display,
                      DefaultRootWindow (display),
		      loc_context,
		      (XPointer *)&atomsTable)) {
        atomsTable = NULL;
    }
    return (atomsTable);
}

/*****************************************************************************
 *
 *  SetAtomsTable ()
 *
 *  Set the atoms table address into the displayToAtomsContext.
 ***************************************************************************/

static void
SetAtomsTable(
        Display *display,
        xmAtomsTable atomsTable )
{
    xmAtomsTable oldAtomsTable;
    XContext loc_context;

    _XmProcessLock();
    if (!displayToAtomsContext)
        displayToAtomsContext = XUniqueContext();

    loc_context = displayToAtomsContext;
    _XmProcessUnlock();

    /*
     * Save atom data.
     * Delete old context if one exists.
     */
    if (XFindContext (display, DefaultRootWindow (display),
			loc_context,
			(char **) &oldAtomsTable)) {
	if (atomsTable)
	XSaveContext(display,
                        DefaultRootWindow (display),
	                loc_context,
			(char *) atomsTable);
    }
    else if (oldAtomsTable != atomsTable) {
	XDeleteContext (display, DefaultRootWindow (display),
			loc_context);
        XtFree( (char *) (oldAtomsTable->entries)) ;
        XtFree( (char *) oldAtomsTable) ;
	if (atomsTable)
	XSaveContext(display,
                        DefaultRootWindow (display),
	                loc_context,
			(char *) atomsTable);
    }
}

/*****************************************************************************
 *
 *  ReadMotifWindow ()
 *
 ***************************************************************************/

static Boolean RMW_ErrorFlag;

static int
RMW_ErrorHandler(Display *display, /* unused */
		 XErrorEvent* event) /* unused */
{
    _XmProcessLock();
    RMW_ErrorFlag = True;
    _XmProcessUnlock();
    return 0 ; /* unused */
}

static Window
ReadMotifWindow(
        Display *display )
{
    Atom            motifWindowAtom;
    Atom            type;
    int             format;
    unsigned long   lengthRtn;
    unsigned long   bytesafter;
    Window         *property = NULL;
    Window	    motifWindow = None;
    XErrorHandler old_Handler;

    /* Setup error proc and reset error flag */
    old_Handler = XSetErrorHandler((XErrorHandler) RMW_ErrorHandler);
    _XmProcessLock();
    RMW_ErrorFlag = False;
    _XmProcessUnlock();

    motifWindowAtom = XInternAtom (display, XmI_MOTIF_DRAG_WINDOW, False);

    if ((XGetWindowProperty (display,
                             RootWindow (display, 0),
                             motifWindowAtom,
                             0L, MAXPROPLEN,
			     False,
                             AnyPropertyType,
                             &type,
			     &format,
			     &lengthRtn,
			     &bytesafter,
			     (unsigned char **) &property) == Success) &&
         (type == XA_WINDOW) &&
	 (format == 32) &&
	 (lengthRtn == 1)) {
	motifWindow = *property;
    }
    if (property) {
	XFree ((char *)property);
    }

    XSetErrorHandler(old_Handler);

    _XmProcessLock();
    if (RMW_ErrorFlag) motifWindow = None;
    _XmProcessUnlock();

    return (motifWindow);
}

/*****************************************************************************
 *
 *  CreateMotifWindow ()
 *
 *  Creates a persistent window to hold the target list and atom pair
 *  properties.  This window is not visible on the screen.
 *
 ***************************************************************************/

static Window
CreateMotifWindow(
        Display *display )
{
    Display	         *ndisplay;
    XSetWindowAttributes sAttributes;
    Window	         motifWindow;

    if ((ndisplay = XOpenDisplay(XDisplayString(display))) == NULL) {
	XmeWarning( (Widget) XmGetXmDisplay (display), MESSAGE3);
	return None;
    }

    XGrabServer(ndisplay);

    XSetCloseDownMode (ndisplay, RetainPermanent);

    sAttributes.override_redirect = True;
    sAttributes.event_mask = PropertyChangeMask;
    motifWindow = XCreateWindow (ndisplay,
                                 DefaultRootWindow (ndisplay),
			         -100, -100, 10, 10, 0, 0,
			         InputOnly,
			         CopyFromParent,
			         (CWOverrideRedirect |CWEventMask),
			         &sAttributes);
    XMapWindow (ndisplay, motifWindow);

    WriteMotifWindow (ndisplay, &motifWindow);

    XCloseDisplay(ndisplay);

    return (motifWindow);
}

/*****************************************************************************
 *
 *  WriteMotifWindow ()
 *
 ***************************************************************************/

static void
WriteMotifWindow(
        Display *display,
        Window *motifWindow )
{
    Atom	motifWindowAtom;

    motifWindowAtom = XInternAtom (display, XmI_MOTIF_DRAG_WINDOW, False);

    XChangeProperty (display,
                     RootWindow (display, 0),
                     motifWindowAtom,
		     XA_WINDOW,
		     32,
		     PropModeReplace,
		     (unsigned char *) motifWindow,
		     1);
}

/*****************************************************************************
 *
 *  WriteAtomsTable ()
 *
 ***************************************************************************/

static void
WriteAtomsTable(
        Display *display,
        xmAtomsTable atomsTable )
{
    BYTE			stackData[MAXSTACK];
    struct _propertyRec {
	xmMotifAtomsPropertyRec	info;
	xmMotifAtomsTableRec	entry[1];
    } *propertyRecPtr;

    Atom                	atomsTableAtom;
    int	       			i;
    Window			motifWindow;
    size_t			dataSize;

    if (!atomsTable) {
	XmeWarning( (Widget) XmGetXmDisplay (display), MESSAGE4);
	return;
    }

    /* If the data is bigger than the default stack allocation, then
     * allocate heap storage, else use automatic storage.
     */
    dataSize = sizeof(xmMotifAtomsPropertyRec) +
      atomsTable->numEntries * sizeof(xmMotifAtomsTableRec) ;

    if ( dataSize > MAXSTACK ) {
	propertyRecPtr = (struct _propertyRec *)XtMalloc( dataSize );
    } else {
	propertyRecPtr = (struct _propertyRec *)stackData;
    }

    propertyRecPtr->info.byte_order = (BYTE) _XmByteOrderChar;
    propertyRecPtr->info.protocol_version = (BYTE) _MOTIF_DRAG_PROTOCOL_VERSION;
    propertyRecPtr->info.num_atoms = atomsTable->numEntries;
    propertyRecPtr->info.heap_offset = dataSize;

    /* write each entry's atom and time */

    for (i = 0; i < atomsTable->numEntries; i++) {
        propertyRecPtr->entry[i].atom = atomsTable->entries[i].atom;
        propertyRecPtr->entry[i].time = atomsTable->entries[i].time;
    }

    /*
     *  Write the buffer to the property within a protected section.
     */

    atomsTableAtom = XInternAtom (display, XmI_MOTIF_DRAG_ATOMS, False);
    motifWindow = GetMotifWindow (display);
    _XmProcessLock();
    StartProtectedSection (display, motifWindow);
    XChangeProperty (display,
                     motifWindow,
		     atomsTableAtom,
		     atomsTableAtom,
		     8,
		     PropModeReplace,
		     (unsigned char *)propertyRecPtr,
		     dataSize );

    /* If we had to use a heap buffer, free it. */
    if (propertyRecPtr != (struct _propertyRec *)stackData) {
        XtFree((char *)propertyRecPtr);
    }
    EndProtectedSection (display);
    if (bad_window) {
	XmeWarning( (Widget) XmGetXmDisplay (display), MESSAGE1);
    }
    _XmProcessUnlock();
}

/*****************************************************************************
 *
 *  ReadAtomsTable ()
 *
 ***************************************************************************/

static Boolean
ReadAtomsTable(
        Display *display,
        xmAtomsTable atomsTable )
{
    struct {
	xmMotifAtomsPropertyRec info;
	xmMotifAtomsTableRec	entry[1];
    } *propertyRecPtr = NULL;
    Atom                        atomsTableAtom;
    int				format;
    unsigned long 		bytesafter, lengthRtn;
    Atom			type;
    int				i;
    Boolean			ret;
    Window			motifWindow;

    atomsTableAtom = XInternAtom (display, XmI_MOTIF_DRAG_ATOMS, False);
    motifWindow = GetMotifWindow (display);
    _XmProcessLock();
    StartProtectedSection (display, motifWindow);
    ret = ((XGetWindowProperty (display, 	/* display* */
    				motifWindow,	/* window */
			        atomsTableAtom,	/* property atom */
			        0L, MAXPROPLEN,	/* long_offset, long_length */
			        False,		/* delete flag */
			        atomsTableAtom,	/* property type */
			        &type,		/* returned actual type */
			        &format,	/* returned actual format */
			        &lengthRtn,	/* returned item count */
			        &bytesafter,	/* returned bytes remaining */
			        (unsigned char **) &propertyRecPtr)
						/* returned data */
	    == Success) &&
           (lengthRtn >= sizeof(xmMotifAtomsPropertyRec)));
    EndProtectedSection (display);

    if (bad_window) {
	static Boolean first_time = True;

	/*
	 * Try to recreate the motifWindow. We could have gotten an invalid
	 * window id from the _MOTIF_DRAG_WINDOW property.
	 */
	if (first_time) {
	    motifWindow = CreateMotifWindow (display);
	    SetMotifWindow (display, motifWindow);
	    first_time = False;
	} else
	    XmeWarning( (Widget) XmGetXmDisplay (display), MESSAGE1);

	ret = False;
    }
    _XmProcessUnlock();

    if (ret) {
	if (propertyRecPtr->info.protocol_version !=
	    _MOTIF_DRAG_PROTOCOL_VERSION) {
	    XmeWarning( (Widget) XmGetXmDisplay (display), MESSAGE2);
	}

	if (propertyRecPtr->info.byte_order != _XmByteOrderChar) {
	    swap2bytes(propertyRecPtr->info.num_atoms);
	    swap4bytes(propertyRecPtr->info.heap_offset);
	}

        if (atomsTable == NULL)
        {
            atomsTable = (xmAtomsTable) XtMalloc(sizeof(xmAtomsTableRec));
            atomsTable->numEntries = 0;
            atomsTable->entries = NULL;

            SetAtomsTable (display, atomsTable);
        }

	if (propertyRecPtr->info.num_atoms > atomsTable->numEntries) {

            /*
	     *  expand the atoms table
	     */

            atomsTable->entries = (xmAtomsTableEntry) XtRealloc(
	        (char *)atomsTable->entries,	/* NULL ok */
		sizeof(xmAtomsTableEntryRec) * propertyRecPtr->info.num_atoms);
	}

	/*
	 *  Read the atom table entries.
	 */

	for (i = 0; i < (int)propertyRecPtr->info.num_atoms; i++) {
	    if (propertyRecPtr->info.byte_order != _XmByteOrderChar) {
		swap4bytes(propertyRecPtr->entry[i].atom);
		swap4bytes(propertyRecPtr->entry[i].time);
	    }

            atomsTable->entries[i].atom = (Atom) propertyRecPtr->entry[i].atom;
            atomsTable->entries[i].time = (Time) propertyRecPtr->entry[i].time;
	}
        atomsTable->numEntries = propertyRecPtr->info.num_atoms;
    }

    /*
     *  Free any memory that Xlib passed us.
     */

    if (propertyRecPtr) {
        XFree((char *)propertyRecPtr);
    }
    return (ret);
}

/*****************************************************************************
 *
 *  WriteTargetsTable ()
 *
 ***************************************************************************/

static void
WriteTargetsTable(
        Display *display,
        xmTargetsTable targetsTable )
{
    BYTE		stackData[MAXSTACK], *fill;
    struct _propertyRec {
	xmMotifTargetsPropertyRec	info;
    } *propertyRecPtr;

    Atom                targetsTableAtom;
    int			i, j;
    Window		motifWindow;
    size_t		dataSize;
    CARD16Item		shortItem;
    CARD32Item		longItem;

    if (!targetsTable) {
	XmeWarning( (Widget) XmGetXmDisplay (display), MESSAGE5);
	return;
    }

    /* Calculate the total size of the property. */
    dataSize = sizeof(xmMotifTargetsPropertyRec);

    for (i = 0; i < targetsTable->numEntries; i++) {
	dataSize += targetsTable->entries[i].numTargets * 4 + 2;
    }

    /* If size needed is bigger than the pre-allocated space, allocate a
     * bigger buffer.
     */
    if ( dataSize > MAXSTACK ){
	propertyRecPtr = (struct _propertyRec *)XtMalloc( dataSize );
    } else {
	propertyRecPtr = (struct _propertyRec *)stackData ;
    }

    propertyRecPtr->info.byte_order = (BYTE) _XmByteOrderChar;
    propertyRecPtr->info.protocol_version = (BYTE) _MOTIF_DRAG_PROTOCOL_VERSION;
    propertyRecPtr->info.num_target_lists = targetsTable->numEntries;
    propertyRecPtr->info.heap_offset = dataSize;

    /* write each target list's count and atoms */

    fill = (BYTE *)propertyRecPtr + sizeof(xmMotifTargetsPropertyRec);

    for (i = 0; i < targetsTable->numEntries; i++) {
        shortItem.value = targetsTable->entries[i].numTargets;
	memcpy( fill, &shortItem, 2 );
	fill += 2;

	/*
	 *  Write each Atom out one at a time as a CARD32.
	 */
	for (j = 0; j < targetsTable->entries[i].numTargets; j++) {
	    longItem.value = targetsTable->entries[i].targets[j];
	    memcpy( fill, &longItem, 4 );
	    fill += 4;
	}
    }

    /*
     *  Write the buffer to the property within a protected section.
     */

    targetsTableAtom = XInternAtom (display, XmI_MOTIF_DRAG_TARGETS, False);
    motifWindow = GetMotifWindow (display);
    _XmProcessLock();
    StartProtectedSection (display, motifWindow);
    XChangeProperty (display,
                     motifWindow,
		     targetsTableAtom,
		     targetsTableAtom,
		     8,
		     PropModeReplace,
		     (unsigned char *)propertyRecPtr,
		     dataSize);

    /* If a buffer was allocated, free it. */
    if (propertyRecPtr != (struct _propertyRec *)stackData) {
        XtFree((char *)propertyRecPtr);
    }
    EndProtectedSection (display);
    if (bad_window) {
	XmeWarning( (Widget) XmGetXmDisplay (display), MESSAGE1);
    }
    _XmProcessUnlock();
}

/*****************************************************************************
 *
 *  ReadTargetsTable ()
 *
 ***************************************************************************/

static Boolean
ReadTargetsTable(
        Display *display,
        xmTargetsTable targetsTable )
{
    struct _propertyRec {
	xmMotifTargetsPropertyRec	info;
    } *propertyRecPtr = NULL;

    char			*bufptr;
    short			num_targets;
    Atom                        targetsTableAtom;
    int				format;
    unsigned long 		bytesafter, lengthRtn;
    Atom			type;
    int				i, j;
    Atom		        *targets;
    Boolean			ret;
    Window			motifWindow;
    CARD16Item			shortItem;
    CARD32Item			longItem;

    targetsTableAtom = XInternAtom (display, XmI_MOTIF_DRAG_TARGETS, False);
    motifWindow = GetMotifWindow (display);
    _XmProcessLock();
    StartProtectedSection (display, motifWindow);
    ret = ((XGetWindowProperty (display,
    				motifWindow,
			        targetsTableAtom,
			        0L, MAXPROPLEN,
			        False,
			        targetsTableAtom,
			        &type,
			        &format,
			        &lengthRtn,
			        &bytesafter,
			        (unsigned char **) &propertyRecPtr) == Success) &&
           (lengthRtn >= sizeof(xmMotifTargetsPropertyRec)));
    EndProtectedSection (display);
    if (bad_window) {
	XmeWarning( (Widget) XmGetXmDisplay (display), MESSAGE1);
	ret = False;
    }
    _XmProcessUnlock();

    if (ret) {
	if (propertyRecPtr->info.protocol_version !=
	    _MOTIF_DRAG_PROTOCOL_VERSION) {
	    XmeWarning( (Widget) XmGetXmDisplay (display), MESSAGE2);
	}

	if (propertyRecPtr->info.byte_order != _XmByteOrderChar) {
	    swap2bytes(propertyRecPtr->info.num_target_lists);
	    swap4bytes(propertyRecPtr->info.heap_offset);
	}

        if (targetsTable == NULL)
        {
            targetsTable = (xmTargetsTable)XtMalloc(sizeof(xmTargetsTableRec));
            targetsTable->numEntries = 0;
            targetsTable->entries = NULL;

            SetTargetsTable (display, targetsTable);
        }

	if (propertyRecPtr->info.num_target_lists > targetsTable->numEntries) {

	    /*
	     *  expand the target table
	     */

            targetsTable->entries = (xmTargetsTableEntry)
	      XtRealloc(
			(char *)targetsTable->entries,	/* NULL ok */
			sizeof(xmTargetsTableEntryRec) *
			propertyRecPtr->info.num_target_lists);

	    /*
	     *  read the new entries
	     */

	    bufptr = (char *)propertyRecPtr + sizeof(xmMotifTargetsPropertyRec);
	    for (i = 0; i < targetsTable->numEntries; i++) {
		memcpy( &shortItem, bufptr, 2 );
	        if (propertyRecPtr->info.byte_order != _XmByteOrderChar) {
		    swap2bytes(shortItem.value);
		}
		num_targets = shortItem.value;

		bufptr += 2 + 4 * num_targets;

		if (num_targets != targetsTable->entries[i].numTargets) {
		    XmeWarning( (Widget) XmGetXmDisplay (display), MESSAGE6);
		}
	    }
	    for (; i < (int)propertyRecPtr->info.num_target_lists; i++) {
		memcpy( &shortItem, bufptr, 2 );
		bufptr += 2;
	        if (propertyRecPtr->info.byte_order != _XmByteOrderChar) {
	            swap2bytes(shortItem.value);
		}
		num_targets = shortItem.value;

                if(!num_targets)
		  targets = NULL;
                else
	          targets = (Atom *) XtMalloc(sizeof(Atom) * num_targets);
		/*
	 	 *  Read each Atom in one at a time.
	 	 */
		for (j = 0; j < num_targets; j++) {
		    memcpy( &longItem, bufptr, 4 );
		    bufptr += 4;
	            if (propertyRecPtr->info.byte_order != _XmByteOrderChar) {
			swap4bytes(longItem.value);
		    }
		    targets[j] = (Atom) longItem.value ;
		}

                targetsTable->numEntries++;
                targetsTable->entries[i].numTargets = num_targets;
                targetsTable->entries[i].targets = targets;
	    }
	}
    }

    /*
     *  Free any memory that Xlib passed us.
     */

    if (propertyRecPtr) {
        XFree((char *)propertyRecPtr);
    }
    return (ret);
}

/*****************************************************************************
 *
 *  CreateDefaultTargetsTable ()
 *
 *  Create the default targets table.
 ***************************************************************************/

static Atom nullTargets[] = 	{ 0,		};
static Atom stringTargets[] = 	{ XA_STRING,	};

static xmTargetsTable
CreateDefaultTargetsTable(
        Display *display )
{
    xmTargetsTable	targetsTable;
    Cardinal		size;

    targetsTable = (xmTargetsTable) XtMalloc(sizeof(xmTargetsTableRec));

    targetsTable->numEntries = 2;
    targetsTable->entries =
	(xmTargetsTableEntry) XtMalloc(sizeof(xmTargetsTableEntryRec) * 2);

/* According to specs and man pages, default values of XmNimportTargets
 * and XmNnumImportTargets should be NULL and zero respectively. The
 * corresponding record in default targets table is entries[0]. The original
 * setting of entries[0] (commented out below) was resulting in to default
 * values of non NULL pointer pointing to target list with NULL entry and
 * 1 for XmNimportTargets and XmNnumImportTargets respectively.
 *
 * The changed code set the XmNnumImportTargets to 0 and XmNimportTargets to
 * NULL. This change may cause problem in code where importTarget
 * is dereferanced twice without checking for possible NULL value. It that
 * happens, and there is no easy way to fix it, change entries[0].targets to
 * nullTargets, but leave entries[0].numTargets to 0.
 */

#if 0
    targetsTable->entries[0].numTargets = XtNumber(nullTargets);
    size = sizeof(Atom) * targetsTable->entries[0].numTargets;
    targetsTable->entries[0].targets = (Atom*) XtMalloc(size);
    memcpy(targetsTable->entries[0].targets, nullTargets, size);
#endif
    targetsTable->entries[0].numTargets = _XmDefaultNumImportTargets;
    targetsTable->entries[0].targets = (Atom*) _XmDefaultImportTargets;

    targetsTable->entries[1].numTargets = XtNumber(stringTargets);
    size = sizeof(Atom) * targetsTable->entries[1].numTargets;
    targetsTable->entries[1].targets = (Atom*) XtMalloc(size);
    memcpy(targetsTable->entries[1].targets, stringTargets, size);

    SetTargetsTable (display, targetsTable);
    return (targetsTable);
}

/*****************************************************************************
 *
 *  CreateDefaultAtomsTable ()
 *
 *  Create the default atoms table.
 ***************************************************************************/

static xmAtomsTable
CreateDefaultAtomsTable(
        Display *display )
{
    xmAtomsTable	atomsTable;

    atomsTable = (xmAtomsTable) XtMalloc(sizeof(xmAtomsTableRec));

    atomsTable->numEntries = 1;
    atomsTable->entries =
	(xmAtomsTableEntry) XtMalloc(sizeof(xmAtomsTableEntryRec));

    atomsTable->entries[0].atom =
	XInternAtom (display, XmS_MOTIF_ATOM_0, False);
    atomsTable->entries[0].time = 0;

    SetAtomsTable (display, atomsTable);
    return (atomsTable);
}

/*****************************************************************************
 *
 *  _XmInitTargetsTable ()
 *
 ***************************************************************************/

void
_XmInitTargetsTable(
        Display *display )
{
    Window	motifWindow;
    Boolean	grabbed = False;

    /*
     *  Read the motifWindow property on the root.  If the property is not
     *    there, create a persistant motifWindow and put it on the property.
     *  Reading the atom pair, atoms table, and targets table properties
     *    on motifWindow is delayed so they can be saved in contexts indexed
     *    by the original display.
     */


    if ((motifWindow = ReadMotifWindow (display)) == None) {
	motifWindow = CreateMotifWindow (display);
    }

    SetMotifWindow (display, motifWindow);

    /*
     * At this time, we are not sure the motifWindow id is valid,
     * but we will find out in the ReadAtomsTable. We will try to
     * recreate it there.
     */

    if (!ReadAtomsTable (display, GetAtomsTable (display))) {
        XGrabServer(display);
        grabbed = True;
        if (!ReadAtomsTable (display, GetAtomsTable (display))) {
            WriteAtomsTable (display, CreateDefaultAtomsTable (display));
        }
    }

    if (!ReadTargetsTable (display, GetTargetsTable (display))) {
        if (!grabbed) {
	    XGrabServer(display);
            grabbed = True;
	    if (!ReadTargetsTable (display, GetTargetsTable (display))) {
                WriteTargetsTable (display,
				   CreateDefaultTargetsTable (display));
	    }
	}
	else {
            WriteTargetsTable (display, CreateDefaultTargetsTable (display));
	}
    }

    if (grabbed) {
	XUngrabServer (display);
        XFlush (display);
    }
}

void _XmClearDisplayTables (Display *display)
{
       SetAtomsTable(display,NULL);
       SetTargetsTable(display,NULL);
}


/*****************************************************************************
 *
 *  _XmIndexToTargets ()
 *
 ***************************************************************************/

Cardinal
_XmIndexToTargets(
        Widget shell,
        Cardinal t_index,
        Atom **targetsRtn )
{
    Display		*display = XtDisplay (shell);
    xmTargetsTable	targetsTable;

    if (!(targetsTable = GetTargetsTable (display))) {
        _XmInitTargetsTable (display);
        targetsTable = GetTargetsTable (display);
    }

    if (t_index >= targetsTable->numEntries) {
        /*
	 *  Retrieve the targets table from motifWindow.
	 *  If this fails, then either the motifWindow or the targets table
	 *  property on motifWindow has been destroyed, so reinitialize.
	 */
        if (!ReadTargetsTable (display, targetsTable)) {
            _XmInitTargetsTable (display);
            targetsTable = GetTargetsTable (display);
	}
    }

    if (t_index >= targetsTable->numEntries) {
	XmeWarning ((Widget) XmGetXmDisplay (display), MESSAGE7);
        *targetsRtn = NULL;
        return 0;
    }

    *targetsRtn = targetsTable->entries[t_index].targets;
    return targetsTable->entries[t_index].numTargets;
}

/*****************************************************************************
 *
 *  _XmAtomCompare ()
 *
 *  The routine must return an integer less than, equal to, or greater than
 *  0 according as the first argument is to be considered less
 *  than, equal to, or greater than the second.
 ***************************************************************************/

static int
AtomCompare(
        XmConst void *atom1,
        XmConst void *atom2 )
{
    return (*((Atom *) atom1) - *((Atom *) atom2));
}

/*****************************************************************************
 *
 *  _XmTargetsToIndex ()
 *
 ***************************************************************************/

Cardinal
_XmTargetsToIndex(
        Widget shell,
        Atom *targets,
        Cardinal numTargets )
{
    Display		*display = XtDisplay (shell);
    Cardinal		i, j;
    Cardinal		size;
    Cardinal		oldNumEntries;
    Atom		*newTargets;
    xmTargetsTable	targetsTable;

    if(!numTargets) return 0;
    _XmProcessLock();

    if (!(targetsTable = GetTargetsTable (display))) {
        _XmInitTargetsTable (display);
        targetsTable = GetTargetsTable (display);
    }

    /*
     *  Create a new targets list, sorted in ascending order.
     */

    size =  sizeof(Atom) * numTargets;
    newTargets = (Atom *) XtMalloc(size);
    memcpy (newTargets, targets, size);
    qsort ((void *)newTargets, (size_t)numTargets, (size_t)sizeof(Atom),
           AtomCompare);
    /*
     *  Try to find the targets list in the targets table.
     */

    for (i = 0; i < targetsTable->numEntries; i++) {
	if (numTargets == targetsTable->entries[i].numTargets) {
            for (j = 0; j < numTargets; j++) {
	        if (newTargets[j] != targetsTable->entries[i].targets[j]) {
	            break;
		}
	    }
	    if (j == numTargets) {
	        XtFree ((char *)newTargets);
                _XmProcessUnlock();
	        return i;
	    }
	}
    }
    oldNumEntries = targetsTable->numEntries;

    /*
     *  Lock and retrieve the target table from motifWindow.
     *  If this fails, then either the motifWindow or the targets table
     *  property on motifWindow has been destroyed, so reinitialize.
     *  If the target list is still not in the table, add the target list
     *  to the table and write the table out to its property.
     */

    XGrabServer (display);
    if (!ReadTargetsTable (display, targetsTable)) {
	XUngrabServer (display);
        _XmInitTargetsTable (display);
	XGrabServer (display);
        targetsTable = GetTargetsTable (display);
    }

    for (i = oldNumEntries; i < targetsTable->numEntries; i++) {
	if (numTargets == targetsTable->entries[i].numTargets) {
	    for (j = 0; j < numTargets; j++) {
		if (newTargets[j] != targetsTable->entries[i].targets[j]) {
	            break;
		}
	    }
	    if (j == numTargets) {
	        XtFree ((char *)newTargets);
		break;
            }
	}
    }
    if (i == targetsTable->numEntries) {
        targetsTable->numEntries++;

        targetsTable->entries = (xmTargetsTableEntry) XtRealloc(
	    (char *)targetsTable->entries,	/* NULL ok */
	    sizeof(xmTargetsTableEntryRec) * (targetsTable->numEntries));

        targetsTable->entries[i].numTargets = numTargets;
        targetsTable->entries[i].targets = newTargets;
        WriteTargetsTable (display, targetsTable);
    }

    XUngrabServer (display);
    XFlush (display);
    _XmProcessUnlock();
    return i;
}

/*****************************************************************************
 *
 *  _XmAllocMotifAtom ()
 *
 *  Allocate an atom in the atoms table with the specified time stamp.
 ***************************************************************************/

Atom
_XmAllocMotifAtom(
        Widget shell,
        Time time )
{
    Display		*display = XtDisplay (shell);
    xmAtomsTable	atomsTable;
    xmAtomsTableEntry	p;
    Cardinal		i;
    char		atomname[80];
    Atom		atomReturn = None;

    if (!(atomsTable = GetAtomsTable (display))) {
        _XmInitTargetsTable (display);
        atomsTable = GetAtomsTable (display);
    }

    /*
     *  Lock and retrieve the atoms table from motifWindow.
     *  If this fails, then either the motifWindow or the atoms table
     *  property on motifWindow has been destroyed, so reinitialize.
     *  Try to find an available atom in the table (time == 0).
     *  If no atom is available, add an atom to the table.
     *  Write the atoms table out to its property.
     */

    XGrabServer (display);
    if (!ReadAtomsTable (display, atomsTable)) {
	XUngrabServer (display);
        _XmInitTargetsTable (display);
	XGrabServer (display);
        atomsTable = GetAtomsTable (display);
    }

    for (p = atomsTable->entries, i = atomsTable->numEntries; i; p++, i--) {
        if ((p->time) == 0) {
            p->time = time;
            atomReturn = p->atom;
	    break;
        }
    }

    if (atomReturn == None) {
	i = atomsTable->numEntries++;

        atomsTable->entries = (xmAtomsTableEntry) XtRealloc(
	    (char *)atomsTable->entries,	/* NULL ok */
  	    (atomsTable->numEntries * sizeof(xmAtomsTableEntryRec)));

        sprintf(atomname, "%s%d", "_MOTIF_ATOM_", i);
        atomsTable->entries[i].atom = XInternAtom (display, atomname, False);
        atomsTable->entries[i].time = time;
        atomReturn = atomsTable->entries[i].atom;
    }

    WriteAtomsTable (display, atomsTable);
    XUngrabServer (display);
    XFlush (display);
    return (atomReturn);
}

/*****************************************************************************
 *
 *  _XmGetMotifAtom ()
 *
 *  Get the atom from the atoms table with nonzero timestamp less than but
 *  closest to the specified value.
 ***************************************************************************/

Atom
_XmGetMotifAtom(
        Widget shell,
        Time time )
{
    Display		*display = XtDisplay (shell);
    xmAtomsTable	atomsTable;
    Cardinal		i;
    Atom		atomReturn = None;
    Time		c_time;

    /*
     *  Get the atoms table saved in the display's context.
     *  This table will be updated from the motifWindow property.
     */

    if (!(atomsTable = GetAtomsTable (display))) {
        _XmInitTargetsTable (display);
        atomsTable = GetAtomsTable (display);
    }

    /*
     *  Lock and retrieve the atoms table from motifWindow.
     *  If this fails, then either the motifWindow or the atoms table
     *  property on motifWindow has been destroyed, so reinitialize.
     *  Try to find the atom with nonzero timestamp less than but closest
     *  to the specified value.
     */

    XGrabServer (display);
    if (!ReadAtomsTable (display, atomsTable)) {
	XUngrabServer (display);
        _XmInitTargetsTable (display);
	XGrabServer (display);
        atomsTable = GetAtomsTable (display);
    }

    for (i = 0; i < atomsTable->numEntries; i++) {
        if ((atomsTable->entries[i].time) &&
            (atomsTable->entries[i].time <= time)) {
	    break;
	}
    }

    if (i < atomsTable->numEntries) {
        atomReturn = atomsTable->entries[i].atom;
        c_time = atomsTable->entries[i++].time;
        for (; i < atomsTable->numEntries; i++) {
            if ((atomsTable->entries[i].time > c_time) &&
                (atomsTable->entries[i].time < time)) {
                atomReturn = atomsTable->entries[i].atom;
                c_time = atomsTable->entries[i].time;
	    }
	}
    }

    XUngrabServer (display);
    XFlush (display);
    return (atomReturn);
}

/*****************************************************************************
 *
 *  _XmFreeMotifAtom ()
 *
 *  Free an atom in the atoms table by giving it a zero timestamp.
 ***************************************************************************/

void
_XmFreeMotifAtom(
        Widget shell,
        Atom atom )
{
    Display		*display = XtDisplay (shell);
    xmAtomsTable	atomsTable;
    xmAtomsTableEntry	p;
    Cardinal		i;

    if (atom == None) {
	return;
    }

    /*
     *  Get the atoms table saved in the display's context.
     *  This table will be updated from the motifWindow property.
     */

    if (!(atomsTable = GetAtomsTable (display))) {
        _XmInitTargetsTable (display);
        atomsTable = GetAtomsTable (display);
    }

    /*
     *  Lock and retrieve the atoms table from its property.
     *  If this fails, then either the motifWindow or the atoms table
     *  property on motifWindow has been destroyed, so reinitialize.
     *  Free the matched atom, if present, and write the atoms table out
     *  to its property.
     */

    XGrabServer (display);
    if (!ReadAtomsTable (display, atomsTable)) {
	XUngrabServer (display);
        _XmInitTargetsTable (display);
	XGrabServer (display);
        atomsTable = GetAtomsTable (display);
    }

    for (p = atomsTable->entries, i = atomsTable->numEntries; i; p++, i--) {
        if (p->atom == atom) {
            p->time = (Time) 0;
            WriteAtomsTable (display, atomsTable);
	    break;
        }
    }

    XUngrabServer (display);
    XFlush (display);
}

/*****************************************************************************
 *
 *  _XmDestroyMotifWindow ()
 *
 ***************************************************************************/

void
_XmDestroyMotifWindow(
        Display  *display )
{
    Window	motifWindow;
    Atom	motifWindowAtom;

    if ((motifWindow = ReadMotifWindow (display)) != None) {
        motifWindowAtom = XInternAtom (display, XmI_MOTIF_DRAG_WINDOW, False);
        XDeleteProperty (display,
                         DefaultRootWindow (display),
                         motifWindowAtom);
	XDestroyWindow (display, motifWindow);
    }
}

/*****************************************************************************
 *
 *  _XmGetDragProxyWindow ()
 *
 ***************************************************************************/

Window
_XmGetDragProxyWindow(
        Display  *display )
{
    Atom		motifProxyWindowAtom;
    Atom		type;
    int			format;
    unsigned long	lengthRtn;
    unsigned long	bytesafter;
    Window		*property = NULL;
    Window		motifWindow;
    Window		motifProxyWindow = None;

    if ((motifWindow = ReadMotifWindow (display)) != None) {

	motifProxyWindowAtom =
	    XInternAtom (display, XmI_MOTIF_DRAG_PROXY_WINDOW, False);

	_XmProcessLock();
	StartProtectedSection (display, motifWindow);

	if ((XGetWindowProperty (display,
                                 motifWindow,
                                 motifProxyWindowAtom,
                                 0L, MAXPROPLEN,
			         False,
                                 AnyPropertyType,
                                 &type,
			         &format,
			         &lengthRtn,
			         &bytesafter,
			         (unsigned char **) &property) == Success) &&
             (type == XA_WINDOW) &&
	     (format == 32) &&
	     (lengthRtn == 1)) {
	    motifProxyWindow = *property;
	}

	EndProtectedSection (display);
	_XmProcessUnlock();

	if (property) {
	    XFree ((char *)property);
	}
    }
    return (motifProxyWindow);
}
