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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: CutPaste.c /main/27 1999/05/26 17:42:48 samborn $"
#endif
#endif

#define CUTPASTE
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "XmI.h"		/* for MAX */
#include <Xm/CutPaste.h>
#include "MessagesI.h"
#include "CutPasteI.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define FIX_1529
#ifdef FIX_1529
#include <X11/Xmd.h>		/* for CARD32 */
#endif

#define XMERROR(key, message)                                            \
    XtErrorMsg (key, "xmClipboardError", "XmToolkitError", message, NULL, NULL)

#define XMRETRY 3
#define XM_APPEND 0
#define XM_REPLACE 1
#define XM_PREPEND 2

#define XM_HEADER_ID 0
#define XM_NEXT_ID   1
#define XM_LOCK_ID   2

#define XM_FIRST_FREE_ID 1000	/* First Item Id allocated */
#define XM_ITEM_ID_INC 1000	/* Increase in Item Id between each copy */
#define XM_ITEM_ID_MAX 5000	/* 'Safe' threshold for resetting Item Id */

#define XM_FORMAT_HEADER_TYPE 1
#define XM_DATA_ITEM_RECORD_TYPE 2
#define XM_HEADER_RECORD_TYPE 3

#define XM_UNDELETE 0
#define XM_DELETE 1

#define XM_DATA_REQUEST_MESSAGE 0
#define XM_DATA_DELETE_MESSAGE  1

#define XM_CLIPBOARD_MESSAGE1	_XmMMsgCutPaste_0000
#define XM_CLIPBOARD_MESSAGE2	_XmMMsgCutPaste_0001
#define XM_CLIPBOARD_MESSAGE3	_XmMMsgCutPaste_0002
#define CLIPBOARD_BAD_DATA_TYPE	_XmMMsgCutPaste_0003
#define BAD_DATA_TYPE		_XmMMsgCutPaste_0004
#define CLIPBOARD_CORRUPT	_XmMMsgCutPaste_0005
#define CORRUPT_DATA_STRUCTURE	_XmMMsgCutPaste_0006
/* #define CLIPBOARD_BAD_FORMAT	_XmMMsgCutPaste_0007	*/	/* unused */
#define BAD_FORMAT		_XmMMsgCutPaste_0008
#define BAD_FORMAT_NON_NULL	_XmMMsgCutPaste_0009


#define XM_STRING	 8
#define XM_COMPOUND_TEXT 8
#define XM_UTF8_STRING   8
#define XM_ATOM	32
#define XM_ATOM_PAIR	32
#define XM_BITMAP	32
#define XM_PIXMAP	32
#define XM_DRAWABLE	32
#define XM_SPAN	32
#define XM_INTEGER	32
#define XM_WINDOW	32
#define XM_PIXEL	32
#define XM_COLORMAP	32
#define XM_TEXT	 8

#define MAX_SELECTION_INCR(dpy) (((65536 < XMaxRequestSize(dpy)) ? \
      (65536 << 2)  : (XMaxRequestSize(dpy) << 2))-100)

#define BYTELENGTH( length, format ) \
  ((format == 8) ? length : \
   ((format == 16) ? length * sizeof(short) : \
    (length * sizeof(long))))

#define CONVERT_32_FACTOR (sizeof(unsigned long) / 4)

typedef long itemId;

typedef unsigned long timeStamp;

/*-----------------------------------------------------*/
/*   Define the clipboard selection information record */
/*-----------------------------------------------------*/

typedef struct _ClipboardSelectionInfo {
    long format;
    unsigned long count;
    char *data;
    Atom type;
    Boolean received;
    Boolean success;
} ClipboardSelectionInfoRec, *ClipboardSelectionInfo;

/*------------------------------------------------------------*/
/*   Define the clipboard property destroy information record */
/*------------------------------------------------------------*/

typedef struct _ClipboardDestroyInfo {

    Display *display;
    Window  window;
    Atom property;

} ClipboardDestroyInfoRec, *ClipboardDestroyInfo;

/*-------------------------------------------------------*/
/*   Define the clipboard cut by name information record */
/*-------------------------------------------------------*/

typedef struct _ClipboardCutByNameInfo {

    Window window;
    itemId formatitemid;

} ClipboardCutByNameInfoRec, *ClipboardCutByNameInfo;

/*---------------------------------------------------*/
/*   Define the clipboard format registration record */
/*---------------------------------------------------*/

typedef struct _ClipboardFormatRegRec {

    long formatLength;

} ClipboardFormatRegRec, *ClipboardFormatRegPtr;

/*-------------------------------------------*/
/*   Define the clipboard lock record        */
/*-------------------------------------------*/

typedef struct _ClipboardLockRec {

    Window windowId;
    long   lockLevel;

} ClipboardLockRec, *ClipboardLockPtr;

/*---------------------------------------------------*/
/*   Define the clipboard format item record         */
/*---------------------------------------------------*/

typedef struct _ClipboardFormatItemRec {

    long recordType;
    itemId parentItemId;/* this is the data item that owns the format */
    Display *displayId;	/* display id of application owning data */
    Window windowId;	/* window id for cut by name */
    Widget cutByNameWidget;    /* window id for cut by name */
    Window cutByNameWindow;    /* window id for cut by name */
    long cutByNameCBIndex;     /* address of callback routine for */
    			       /* cut by name data */
    long itemLength;	/* length of this format item data */
    itemId formatDataId;	/* id for format item data, */
    			        /* 0 if passed by name */
    Atom formatNameAtom;	/* format name atom */
    unsigned long formatNameLength;

    unsigned long cancelledFlag; /* format was cancelled by poster */
    unsigned long cutByNameFlag; /* data has not yet been provided */

    itemId thisFormatId;  /* id given application for identifying format item */
    itemId itemPrivateId; /* id provide by application for identifying item */

    unsigned long copiedLength;  /* amount already copied incrementally */

} ClipboardFormatItemRec, *ClipboardFormatItem;

/*-------------------------------------------------*/
/*   Define the clipboard data item record         */
/*-------------------------------------------------*/

typedef struct _ClipboardDataItemRec {

    long recordType;
    itemId adjunctData; /* for future compatibility */
    Display *displayId;	/* display id of application owning data */
    Window windowId;	/* window id for cut by name */

    itemId thisItemId;  /* item id of this data item */

    unsigned long dataItemLabelId; /* id of label (comp) string */
    unsigned long formatIdList;	 /* offset of beginning of format id list */
    long formatCount;		/* number of formats stored for this item */
    long cancelledFormatCount;	/* number of cut by name formats cancelled */

    unsigned long cutByNameFlag;  /* data has not yet been provided */
    unsigned long deletePendingFlag;	/* is item marked for deletion? */
    unsigned long permanentItemFlag;	/* is item permanent or temporary? */

    long cutByNameCBIndex;   /* address of callback routine for */
    					/* cut by name data */
    Widget cutByNameWidget;   /* widget receiving messages concerning */
    				      /* cut by name */
    Window cutByNameWindow;

} ClipboardDataItemRec, *ClipboardDataItem;

/*----------------------------------------------*/
/*   Define the clipboard header record         */
/*----------------------------------------------*/

typedef struct _ClipboardHeaderRec {

    long recordType;
    itemId adjunctHeader;	/* for future compatibility */
    unsigned long maxItems;	/* maximum number of clipboard items */
    				/* including those marked for delete */
    unsigned long dataItemList;	    /* offset of data item id list */

    itemId nextPasteItemId;    	/* data id of next item id to paste */
    itemId oldNextPasteItemId; 	/* data id of old next paste item id */
    itemId deletedByCopyId;	/* item marked deleted by last copy, if any */
    itemId lastCopyItemId;  /* item id of last item put on clipboard */
    itemId recopyId;        /* item id of item requested for recopy */

    unsigned long currItems;   /* current number of clipboard items */
    			       /* including those marked for delete */
    timeStamp   selectionTimestamp; /* for ICCCM clipboard selection compatability */
    timeStamp   copyFromTimestamp; /* time of event causing inquire or copy from */
    unsigned long foreignCopiedLength; /* amount copied so far in incr copy from */
			      /* selection */
    Window ownSelection;
    unsigned long incrementalCopyFrom;  /* requested in increments */

    unsigned long startCopyCalled;  /* to ensure that start copy is called
					before copy or endcopy */

} ClipboardHeaderRec, *ClipboardHeader;

/*---------------------------------------------*/

typedef union {
    ClipboardFormatItemRec format;
    ClipboardDataItemRec   item;
    ClipboardHeaderRec     header;
} ClipboardUnionRec, *ClipboardPointer;

/*---------------------------------------------*/

/********    Static Function Declarations    ********/

static Time ClipboardGetCurrentTime(
                        Display *dpy) ;
static void ClipboardSetNextItemId(
                        Display *display,
                        long itemid) ;
static Boolean WeOwnSelection( Display*, ClipboardHeader header );
static void AssertClipboardSelection(
                        Display *display,
                        Window window,
                        ClipboardHeader header,
                        Time time) ;
static Boolean ClipboardConvertProc(Widget, Atom *, Atom *, Atom *,
				 XtPointer *, unsigned long *, int *);
static Atom GetTypeFromTarget(Display*, Atom);
static Window InitializeSelection(
                        Display *display,
                        ClipboardHeader header,
                        Window window,
                        Time time) ;
static int RegIfMatch(
                        Display *display,
                        char *format_name,
                        char *match_name,
                        int format_length) ;
static int RegisterFormat(
                        Display *display,
                        char *format_name,
                        int format_length) ;
static void ClipboardError(
                        char *key,
                        char *message) ;
static void ClipboardEventHandler(
                        Widget widget,
                        XtPointer closure,
                        XEvent *event,
                        Boolean *cont) ;
static int ClipboardFindItem(
                        Display *display,
                        itemId itemid,
                        XtPointer *outpointer,
                        unsigned long *outlength,
			Atom *outtype,
                        int *format,
                        int rec_type) ;
static int GetWindowProperty(
                        Display *display,
                        Window window,
                        Atom property_atom,
                        XtPointer *outpointer,
                        unsigned long *outlength,
                        Atom *type,
                        int *format,
#if NeedWidePrototypes
                        int delete_flag) ;
#else
                        Boolean delete_flag) ;
#endif /* NeedWidePrototypes */
static int ClipboardRetrieveItem(
                        Display *display,
                        itemId itemid,
                        int add_length,
                        int def_length,
                        XtPointer *outpointer,
                        unsigned long *outlength,
  			Atom *type,
                        int *format,
                        int rec_type,
                        unsigned long discard) ;
static void ClipboardReplaceItem(
                        Display *display,
                        itemId itemid,
                        XtPointer pointer,
                        unsigned long length,
                        int mode,
                        int format,
#if NeedWidePrototypes
                        int free_flag,
#else
                        Boolean free_flag,
#endif /* NeedWidePrototypes */
  			Atom type);
static Atom ClipboardGetAtomFromId(
                        Display *display,
                        itemId itemid) ;
static Atom ClipboardGetAtomFromFormat(
                        Display *display,
                        char *format_name) ;
static int ClipboardGetLenFromFormat(
                        Display *display,
                        char *format_name,
                        int *format_length) ;
static ClipboardHeader ClipboardOpen(
                        Display *display,
                        int add_length) ;
static void ClipboardClose(
                        Display *display,
                        ClipboardHeader root_clipboard_header) ;
static void ClipboardDeleteId(
                        Display *display,
                        itemId itemid) ;
static ClipboardFormatItem ClipboardFindFormat(
                        Display *display,
                        ClipboardHeader header,
                        char *format,
                        itemId itemid,
                        int n,
                        unsigned long *maxnamelength,
                        int *count,
                        unsigned long *matchlength) ;
static void ClipboardDeleteFormat(
                        Display *display,
                        itemId formatitemid) ;
static void ClipboardDeleteFormats(
                        Display *display,
                        Window window,
                        itemId dataitemid) ;
static void ClipboardDeleteItemLabel(
                        Display *display,
                        Window window,
                        itemId dataitemid) ;
static unsigned long ClipboardIsMarkedForDelete(
                        Display *display,
                        ClipboardHeader header,
                        itemId itemid) ;
static void ClipboardDeleteItem(
                        Display *display,
                        Window window,
                        ClipboardHeader header,
                        itemId deleteid) ;
static void ClipboardDeleteMarked(
                        Display *display,
                        Window window,
                        ClipboardHeader header) ;
static void ClipboardMarkItem(
                        Display *display,
                        ClipboardHeader header,
                        itemId dataitemid,
                        unsigned long state) ;
static int ClipboardSendMessage(
                        Display *display,
                        Window window,
                        ClipboardFormatItem formatptr,
                        int messagetype) ;
static int ClipboardDataIsReady(
                        Display *display,
                        XEvent *event,
                        char *private_info) ;
#if 0
static int ClipboardRequestorIsReady(
                        Display *display,
                        XEvent *event,
                        char *private_info) ;
#endif
static int ClipboardGetSelection(
                        Display *display,
                        Window window,
                        Atom target,
                        XtPointer *value,
			Atom *type,
                        unsigned long *size,
			int *format) ;
static int ClipboardRequestDataAndWait(
                        Display *display,
                        Window window,
                        ClipboardFormatItem formatptr) ;
static itemId ClipboardGetNewItemId(
                        Display *display) ;
static void ClipboardSetAccess(
                        Display *display,
                        Window window) ;
static int ClipboardLock(
                        Display *display,
                        Window window) ;
static int ClipboardUnlock(
                        Display *display,
                        Window window,
#if NeedWidePrototypes
                        int all_levels) ;
#else
                        Boolean all_levels) ;
#endif /* NeedWidePrototypes */
static int ClipboardSearchForWindow(
                        Display *display,
                        Window parentwindow,
                        Window window) ;
static int ClipboardWindowExists(
                        Display *display,
                        Window window) ;
static void ClipboardReceiveData(Widget, XtPointer, Atom*, Atom*,
				 XtPointer, unsigned long *, int *) ;
static int ClipboardRetrieve(Display *display, Window window,
			     char *format, XtPointer buffer,
			     unsigned long length,
			     unsigned long *outlength,
			     long *private_id,
			     Atom *outtype);
static Boolean ClipboardGetByNameItem(Display* dpy,
				      Window win,
				      ClipboardHeader header,
				      char* format);
static void ClipboardTimeout(XtPointer, XtIntervalId*);
/********    End Static Function Declarations    ********/

/*---------------------------------------------*/
/* static data				       */
/*---------------------------------------------*/

static XmCutPasteProc	*cbProcTable = NULL;
static long		*cbIdTable = NULL;
static int		maxCbProcs = 0;


/*---------------------------------------------*/
/* internal routines			       */
/*---------------------------------------------*/

static Time
ClipboardGetCurrentTime(
        Display *dpy )
{
    XEvent event;
    Atom XmA_MOTIF_CLIP_TIME = XInternAtom(dpy, XmS_MOTIF_CLIP_TIME, False);

    XSelectInput(dpy, RootWindow(dpy, 0), PropertyChangeMask);
    XChangeProperty(dpy, RootWindow(dpy, 0),
                    XmA_MOTIF_CLIP_TIME, XmA_MOTIF_CLIP_TIME,
                    8,PropModeAppend,NULL,0);
    XWindowEvent(dpy, RootWindow(dpy, 0), PropertyChangeMask,&event);
    return(event.xproperty.time);
}
static void
ClipboardSetNextItemId(
        Display *display,
        long itemid )
{
    itemId base;
    itemId nextItem;
    XtPointer int_ptr;
    unsigned long length;
    ClipboardHeader header;
    Atom type;
    itemId current_item;
    itemId last_item;

    header = ClipboardOpen( display, 0 );
    current_item = header->nextPasteItemId;
    last_item = header->oldNextPasteItemId;
    ClipboardClose( display, header );

    nextItem = itemid;
    do {
	base = nextItem - (nextItem % XM_ITEM_ID_INC);
	if (base >= XM_ITEM_ID_MAX) {
	    nextItem = XM_FIRST_FREE_ID;
	} else {
	    nextItem = base + XM_ITEM_ID_INC;
	}
    } while (nextItem == current_item - 1 || nextItem == last_item - 1);

    ClipboardFindItem( display,
			XM_NEXT_ID,
			&int_ptr,
			&length,
			&type,
			0,
			0 );
    *(long *) int_ptr = nextItem;

    ClipboardReplaceItem( display, XM_NEXT_ID, int_ptr, length,
			PropModeReplace, 32, True, XA_INTEGER);
}


/***********************************************************************
 * WeOwnSelection
 *
 * Check to see if the Motif clipboard owns the clipboard selection.
 * This enables us to shortcut and not use ICCCM data transfer to get
 * the clipboard data.
 *
 * Note that this is necessary for clipboard locking to work correctly.
 *
 ***********************************************************************/
static Boolean
WeOwnSelection(
        Display *display,
	ClipboardHeader header )
{
    Window selectionwindow;
    Atom XmA_CLIPBOARD = XInternAtom( display, XmSCLIPBOARD, False );

    selectionwindow = XGetSelectionOwner( display, XmA_CLIPBOARD );

    return ( selectionwindow == header->ownSelection );
}

static void
AssertClipboardSelection(
        Display *display,
        Window window,
        ClipboardHeader header,
        Time time )
{
    Widget widget;
    Atom XmA_CLIPBOARD;

    header->ownSelection = None;
    header->selectionTimestamp = 0;

    widget = XtWindowToWidget (display, window);

    /* Need a valid Widget to add an event handler. */

    if (widget == NULL)
    {
	return;
    }

    /* Assert ownership of CLIPBOARD selection only if there is valid data. */

    if (header->nextPasteItemId == 0)
    {
	return;
    }

    header->ownSelection = window;
    header->selectionTimestamp = time;

    XmA_CLIPBOARD = XInternAtom( display, XmSCLIPBOARD, False );
    XtOwnSelection(widget, XmA_CLIPBOARD, time, ClipboardConvertProc,
		   NULL, NULL);
    return;
}

/* This function ensures that if the data item was copied by name,
   it will be retrieved for the convert proc.  It returns true if
   the data was not copied by name or if the retrieval is sucessful */

static Boolean
ClipboardGetByNameItem(Display* dpy, Window win,
		       ClipboardHeader header, char* format)
{
  short dataok;
  ClipboardFormatItem matchformat;
  unsigned long matchformatlength, maxname;
  int count;

  /* find the matching format for the next paste item */
  matchformat = ClipboardFindFormat(dpy, header, format,
				    (itemId) NULL, 0,
				    &maxname, &count,
				    &matchformatlength );
  if (matchformat != 0)
    {
      dataok = 1;

      if ( matchformat->cutByNameFlag == 1 )
	{
	  /* passed by name */
	  dataok = ClipboardRequestDataAndWait(dpy,
					       win,
					       matchformat);
	}
    } else {
      dataok = 0;
    }

  return(dataok != 0);
}


/*ARGSUSED*/
static Boolean
ClipboardConvertProc(Widget wid,
		     Atom *selection, /* unused */
		     Atom *target,
		     Atom *type,
		     XtPointer *value,
		     unsigned long *size,
		     int *format)
{
  enum { XmA_TARGETS, XmA_TIMESTAMP, NUM_ATOMS };
  static char *atom_names[] = { XmSTARGETS, XmSTIMESTAMP };
  Display *display = XtDisplay(wid);
  Window window = XtWindow(wid);
  ClipboardHeader header;
  Boolean rval = True;
  char *format_name = NULL;
  Atom atoms[XtNumber(atom_names)];

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms( display, atom_names, XtNumber(atom_names), False, atoms );

  *value = NULL;
  *type = XA_INTEGER;
  *size = 0;
  *format = 8;

  if ( ClipboardLock( display, window ) != ClipboardSuccess )
    {
      /* We can't lock the clipboard,  something's wrong */
      return(False);
    }

  /* get the clipboard header */
  header = ClipboardOpen( display, 0 );

  if ( !WeOwnSelection( display, header ) )
    {
      /* we don't own the selection, something's wrong */
      rval = False;
      goto done;
    }

  if ( *target == atoms[XmA_TARGETS] )
    {
      Atom *ptr, *save_ptr;
      ClipboardFormatItem nextitem;
      int n, count;
      unsigned long dummy;

      *size = 0;
      *format = 32;
      *type = XA_ATOM;

      /* find the first format for the next paste item, if any remain */
      nextitem = ClipboardFindFormat( display, header, 0,
				     (itemId) NULL, 1, &dummy,
				     &count, &dummy );

      /* allocate storage for list of target atoms,
	 plus the necessary */
      ptr = (Atom *)XtMalloc( sizeof(Atom) * (count + 2));
      save_ptr = ptr;

      /* Put required ICCCM targets which are supported */
      *ptr = atoms[XmA_TARGETS]; ptr++;
      *ptr = atoms[XmA_TIMESTAMP]; ptr++;
      n = 0; /* two targets right now */

      while ( nextitem != NULL && n < count )
	{
	  int ret_count;

	  /* add format to list */
	  *ptr = nextitem->formatNameAtom;
	  n = n + 1;
	  XtFree( (char *) nextitem );

	  /* find the nth format for the next paste item */
	  nextitem = ClipboardFindFormat( display, header, 0,
					 (itemId) NULL, n + 1, &dummy,
					 &ret_count, &dummy );
	  if (nextitem != NULL) ptr++;
	}
      *value = (char *) save_ptr;
      /* n is number of targets from the clipboard,
	 plus 2 builtin targets */
      *size = n + 2;
    } else if ( *target == atoms[XmA_TIMESTAMP] ) {
      timeStamp *timestamp;

      timestamp = (timeStamp *)XtMalloc(sizeof(timeStamp));
      *timestamp  = header->selectionTimestamp;
      *value = (char *) timestamp;
      *size = 1;
      *format = XM_INTEGER;
      *type = XA_INTEGER;
    } else {
      long private_id;
      unsigned long outlength;

      /* convert atom to format name */
      format_name = XGetAtomName( display, *target );
      ClipboardGetLenFromFormat( display, format_name, format) ;

      /* Make sure a byname item is first retrieved */
      ClipboardGetByNameItem(display, window, header, format_name);

      if (XmClipboardInquireLength( display, window, format_name, size)
	  != ClipboardSuccess) {
	rval = False;
	goto done;
      }

      if ( *size == 0 ) {
	rval = False;
	goto done;
      }

      *value = XtMalloc( (int) *size );

      if (ClipboardRetrieve(display, window, format_name,
			    (XtPointer) *value, *size, &outlength,
			    &private_id, type) != ClipboardSuccess ) {
	rval = False;
	goto done;
      }


      /* Fix size to be in format units */
      if (*format == 32)
	*size = *size / sizeof(long);
      else if (*format == 16)
	*size = *size / sizeof(short);
    }

 done:

  if (format_name != NULL) XFree(format_name);
  ClipboardClose( display, header );
  ClipboardUnlock( display, window, False );
  return(rval);
}


static Window
InitializeSelection(
        Display *display,
        ClipboardHeader header,
        Window window,
        Time time )
{
    Window selectionwindow;
    Atom XmA_CLIPBOARD = XInternAtom( display, XmSCLIPBOARD, False );

    /* If there is no CLIPBOARD owner, and we have clipboard
       data, then assert ownership, and use that data. */

    selectionwindow = XGetSelectionOwner (display, XmA_CLIPBOARD);


    /* if the header is corrupted, give up. */

    if (selectionwindow == window && header->ownSelection == None)
    {
	selectionwindow = None;
	XSetSelectionOwner (display, XmA_CLIPBOARD, None, time);
    }

    if (selectionwindow != None) /* someone owns CLIPBOARD already */
    {
	return selectionwindow;
    }

    /* assert ownership of the clipboard selection */

    AssertClipboardSelection (display, window, header, time);

    selectionwindow = XGetSelectionOwner (display, XmA_CLIPBOARD);

    return (selectionwindow);
}


static int
RegIfMatch(
        Display *display,
        char *format_name,
        char *match_name,
        int format_length )
{
	if ( strcmp( format_name, match_name ) == 0 )
	{
	    RegisterFormat( display, format_name, format_length );
	    return 1;
	}
    return 0;
}

static int
RegisterFormat(
        Display *display,       /* Display id of application passing data */
        char *format_name,      /* Name string for data format */
        int format_length )     /* Format length  8-16-32 */
{
    Window rootwindow;
    Atom formatatom;
    int stored_len;

    /* if format is 32 in XChangeProperty, data must be long array */
    long long_format_length = format_length;

    /* get the atom for the format_name */
    formatatom = ClipboardGetAtomFromFormat( display,
    					         format_name );

    rootwindow = RootWindow( display, 0 );

    if ( ClipboardGetLenFromFormat( display,
				        format_name,
					&stored_len ) == ClipboardSuccess )
    {
	if ( stored_len == format_length )
	    return ClipboardSuccess;

	/* it is already registered, don't allow override */
	return ClipboardFail;
    }

    XChangeProperty( display,
		     rootwindow,
		     formatatom,
		     XA_INTEGER,
		     32,
		     PropModeReplace,
		     (unsigned char*)&long_format_length,
		     1 );

    return ClipboardSuccess;
}

/*---------------------------------------------*/
static void
CleanupHeader(
	      Display * display )
{
   XDeleteProperty (display,
		    RootWindow (display, 0),
		    XInternAtom (display, XmS_MOTIF_CLIP_HEADER, False));

   XFlush(display);
}

/*---------------------------------------------*/
static void
ClipboardError(
        char *key,
        char *message )
{
    XMERROR(key, message );

    exit(1);
}

/*---------------------------------------------*/
/* ARGSUSED */
static void
ClipboardEventHandler(
        Widget widget,
        XtPointer closure,
        XEvent *event,
        Boolean *cont )
{
    enum { XmA_MOTIF_CLIP_MESSAGE, XmA_MOTIF_CLIP_DATA_REQUEST,
	   XmA_MOTIF_CLIP_DATA_DELETE, NUM_ATOMS };
    static char *atom_names[] = {
      XmS_MOTIF_CLIP_MESSAGE, XmS_MOTIF_CLIP_DATA_REQUEST,
      XmS_MOTIF_CLIP_DATA_DELETE };

    XClientMessageEvent *event_rcvd;
    Display *display;
    itemId formatitemid;
    ClipboardFormatItem formatitem;
    unsigned long formatlength;
    Atom formattype, htype;
    long privateitemid;
    XmCutPasteProc callbackroutine = NULL;
    int reason, ret_value;
    Atom atoms[XtNumber(atom_names)];

    event_rcvd = (XClientMessageEvent*)event;

    if ( (event_rcvd->type & 127) != ClientMessage )
    	return ;

    display = XtDisplay(widget);

    assert(XtNumber(atom_names) == NUM_ATOMS);
    XInternAtoms( display, atom_names, XtNumber(atom_names), False, atoms );
    if (event_rcvd->message_type != atoms[XmA_MOTIF_CLIP_MESSAGE])
      return ;

    formatitemid  = event_rcvd->data.l[1];
    privateitemid = event_rcvd->data.l[2];

    /* get the callback routine */
    ret_value = ClipboardFindItem( display,
    				       formatitemid,
    			   	       (XtPointer *) &formatitem,
    			   	       &formatlength,
				       &formattype,
				       0,
    			   	       XM_FORMAT_HEADER_TYPE );

    if ( ret_value != ClipboardSuccess )
    	return ;

    if (cbProcTable == NULL) return;

    if (formatitem->cutByNameCBIndex >= 0) {
      _XmProcessLock();
      callbackroutine = cbProcTable[formatitem->cutByNameCBIndex];
      _XmProcessUnlock();
    }

    XtFree( (char *) formatitem );

    if ( callbackroutine == NULL ) return ;

    reason = 0;

    if ( event_rcvd->data.l[0] == atoms[XmA_MOTIF_CLIP_DATA_REQUEST] )
    	reason = XmCR_CLIPBOARD_DATA_REQUEST;

    if ( event_rcvd->data.l[0] == atoms[XmA_MOTIF_CLIP_DATA_DELETE] )
    	reason = XmCR_CLIPBOARD_DATA_DELETE;

    if ( reason == 0 )
    	return ;

    /* call the callback routine */
    (*callbackroutine)( widget,
    		        (long *) &formatitemid,
    		        &privateitemid,
    			&reason );

    /* if this was a data request, reset the recopy id */
    if (reason == XmCR_CLIPBOARD_DATA_REQUEST)
    {
	unsigned long hlength;
	ClipboardHeader header;

	ClipboardFindItem (display, XM_HEADER_ID, (XtPointer *)&header,
				&hlength, &htype, 0, 0);

	header->recopyId = 0;

	ClipboardReplaceItem (display, XM_HEADER_ID, header, hlength,
				PropModeReplace, 32, True, XA_INTEGER);
    }

    return ;
}

/*---------------------------------------------*/
static int
ClipboardFindItem(
        Display *display,
        itemId itemid,
        XtPointer *outpointer,
        unsigned long *outlength,
	Atom *outtype,
        int *format,
        int rec_type )
{

    Window rootwindow;
    int ret_value;
    Atom itematom;
    int dummy;
    ClipboardPointer ptr;

    if (format == NULL) format = &dummy;

    rootwindow = RootWindow( display, 0 );

    /* convert the id into an atom */
    itematom = ClipboardGetAtomFromId( display, itemid );

    ret_value = GetWindowProperty(display,
				  rootwindow,
				  itematom,
				  outpointer,
				  outlength,
				  outtype,
				  format,
				  FALSE );

    if ( ret_value != ClipboardSuccess ) return ret_value;

    ptr = (ClipboardPointer)(*outpointer);

    if ( rec_type != 0 && ptr && ptr->header.recordType != rec_type )
    {
    	XtFree( (char *) *outpointer );
	CleanupHeader (display);
    	ClipboardError( CLIPBOARD_BAD_DATA_TYPE, BAD_DATA_TYPE );
    	return ClipboardFail;
    }

#ifdef FIX_1529
    if( itemid == XM_HEADER_ID && ptr )
    {
    	ptr->header.selectionTimestamp = (CARD32)ptr->header.selectionTimestamp;
    }
#endif
    return ClipboardSuccess;
}


/*---------------------------------------------*/
static int
GetWindowProperty(
        Display *display,
        Window window,
        Atom property_atom,
        XtPointer *outpointer,
        unsigned long *outlength,
        Atom *type,
        int *format,
#if NeedWidePrototypes
        int delete_flag )
#else
        Boolean delete_flag )
#endif /* NeedWidePrototypes */
{

    int ret_value;
    Atom loc_type;
    unsigned long bytes_left;
    unsigned long cur_length;
    unsigned char *loc_pointer;
    unsigned long this_length;
    char *cur_pointer;
    int loc_format;
    long request_size;
    long offset;
    int byte_length;


    loc_pointer = NULL;
    bytes_left = 1;
    offset = 0;
    cur_length = 0;
    cur_pointer = NULL;

    *outpointer = 0;
    *outlength = 0;

    request_size = MAX_SELECTION_INCR( display );

    while ( bytes_left > 0 )
    {
	/* retrieve the item from the root */
	ret_value = XGetWindowProperty( display,
					window,
					property_atom,
					offset, /*offset*/
					request_size, /*length*/
					FALSE,
					AnyPropertyType,
					&loc_type,
					&loc_format,
					&this_length,
					&bytes_left,
					&loc_pointer );

        if ( ret_value != 0 )
            return ClipboardFail;

        if ( loc_pointer == 0 || this_length == 0 )
        {
            if ( delete_flag )
            {
                XDeleteProperty( display, window, property_atom );
	    }
	    if (loc_pointer != NULL) XFree ((char *)loc_pointer);
            return ClipboardFail;
	  }

        /* convert length according to format */
        byte_length = BYTELENGTH( this_length, loc_format );

	if ( cur_length == 0 )
	{
	    cur_pointer = XtMalloc((size_t)(byte_length + bytes_left));
	    /* Size arg. is truncated if sizeof( long) > sizeof( size_t) */
	    *outpointer = cur_pointer;
	}

	memcpy(cur_pointer, loc_pointer, (size_t) byte_length );
	cur_pointer = cur_pointer + byte_length;
	cur_length  = cur_length  + byte_length;
	offset += loc_format * this_length / 32;

	if (loc_pointer != NULL)
		XFree ((char *)loc_pointer);
    }

    if ( delete_flag )
    {
        XDeleteProperty( display, window, property_atom );
    }

    if ( format != NULL )
    {
	*format = loc_format;
    }

    if ( type != NULL )
    {
      char *temp;
      char *match = XmS_MOTIF_CLIP_ITEM;
      int i, len;
      Boolean equal = True;

      len = strlen(match);
      temp = XGetAtomName(display, loc_type);

      for(i = 0; i < len; i++) {
	if (temp[i] == 0 || temp[i] != match[i]) {
	  equal = False;
	  break;
	}
      }

      if (equal) /* Bogus type left by old Clipboard */
	*type = None; /* Signal bad type */
      else
	*type = loc_type;

      XFree(temp);
    }

    *outlength = cur_length;

    return ClipboardSuccess;
}

/*---------------------------------------------*/
static int
ClipboardRetrieveItem(
        Display *display,
        itemId itemid,
        int add_length,             /* allocate this add'l */
        int def_length,             /* if item non-exist */
        XtPointer *outpointer,
        unsigned long *outlength,
        Atom *outtype,
        int *format,
        int rec_type,
        unsigned long discard )     /* ignore old data */
{

    int ret_value;
    int loc_format;
    unsigned long loclength;
    ClipboardPointer clipboard_pointer;
    Atom loctype;
    XtPointer pointer;

    /* retrieve the item from the root */
    ret_value = ClipboardFindItem(display, itemid, &pointer, &loclength,
				  &loctype, &loc_format, rec_type );

    if (loclength == 0 || ret_value != ClipboardSuccess)
    {
    	*outlength = def_length;
    }else{
	if ( discard == 1 ) loclength = 0;

	*outlength = loclength + add_length;
    }

    /* get local memory for the item */
    clipboard_pointer = (ClipboardPointer)XtMalloc( (size_t) *outlength );

    /* Size arg. is truncated if sizeof( long) > sizeof( size_t) */
    if (ret_value == ClipboardSuccess)
    {
	/* copy the item into the local memory */
	memcpy(clipboard_pointer, pointer, (size_t) loclength );
    }

    *outpointer = (char*)clipboard_pointer;

    if (outtype != NULL) *outtype = loctype;

    /* free memory pointed to by pointer */
    XtFree( (char *) pointer );

    if ( format != 0 )
    {
	*format = loc_format;
    }

    /* return a pointer to the item */
    return ret_value;

}

/*---------------------------------------------*/
static void
ClipboardReplaceItem(
        Display *display,
        itemId itemid,
        XtPointer pointer,
        unsigned long length,
        int mode,
        int format,
#if NeedWidePrototypes
        int free_flag,
#else
        Boolean free_flag,
#endif /* NeedWidePrototypes */
        Atom type)
{
    Window rootwindow;
    Atom itematom;
    XtPointer loc_pointer;
    unsigned long loc_length;
    int loc_mode;
    unsigned int max_req_size;

    loc_pointer = pointer;
    loc_mode = mode;

    rootwindow = RootWindow( display, 0 );

    /* convert the id into an atom */
    itematom = ClipboardGetAtomFromId( display, itemid );

    /* lengths are passed in bytes, but need to specify in format units */
    /* for ChangeProperty */
    loc_length = length / BYTELENGTH(1,format);

    max_req_size = ( MAX_SELECTION_INCR( display ) ) * 8 / format;

    do
    {
	unsigned long next_length;

	if ( loc_length > max_req_size )
	{
	    next_length = max_req_size;
	}else{
	    next_length = loc_length;
	}

	if (type == 0 || type == None) type = itematom;

	/* put the new values in the root */
	XChangeProperty( display,
			 rootwindow,
			 itematom,
			 type,
			 format,
			 loc_mode,
			 (unsigned char*)loc_pointer,
			 (int) next_length ); /* Truncation of next_length.*/

	loc_mode = PropModeAppend;
	loc_length = loc_length - next_length;
	loc_pointer = (char *) loc_pointer + BYTELENGTH(next_length,format);
      }
    while( loc_length > 0 );

    if ( free_flag == True )
    {
	/* note:  you have to depend on the free flag, even if the length */
	/* is zero, the pointer may point to non-zero-length allocated data */
        XtFree( (char *) pointer );
    }
}

/*---------------------------------------------*/
static Atom
ClipboardGetAtomFromId(
        Display *display,
        itemId itemid )
{
    char *item;
    char atomname[ 100 ];

    switch ( (int)itemid )
    {
	case 0:	item = XmS_MOTIF_CLIP_HEADER;
	    break;
	case 1: item = XmS_MOTIF_CLIP_NEXT_ID;
	    break;
	default:
    	    sprintf( atomname, "_MOTIF_CLIP_ITEM_%ld", itemid );
    	    item = atomname;
	    break;
    }

    return XInternAtom( display, item, False );
}
/*---------------------------------------------*/
static Atom
ClipboardGetAtomFromFormat(
        Display *display,
        char *format_name )
{
    char *atomname_format = "_MOTIF_CLIP_FORMAT_%s";
    char *item;
    Atom ret_value;

    item = XtMalloc( strlen( format_name ) + strlen( atomname_format ) + 1 );

    sprintf( item, atomname_format, format_name );

    ret_value = XInternAtom( display, item, False );

    XtFree( (char *) item );

    return ret_value;
}
/*---------------------------------------------*/
static int
ClipboardGetLenFromFormat(
        Display *display,
        char *format_name,
        int *format_length )
{
    Atom format_atom;
    int ret_value;
    Window rootwindow;
    unsigned long outlength;
    unsigned char *outpointer;
    Atom type;
    int format;
    unsigned long bytes_left;

    outpointer = NULL;

    format_atom = ClipboardGetAtomFromFormat( display, format_name );

    rootwindow = RootWindow( display, 0 );

    /* get the format record */
    ret_value = XGetWindowProperty( display,
				    rootwindow,
				    format_atom,
				    0, /*offset*/
				    10000000, /*length*/
				    False,
				    AnyPropertyType,
				    &type,
				    &format,
				    &outlength,
				    &bytes_left,
				    &outpointer );

    if ( outpointer == 0 || outlength == 0 || ret_value !=0 )
    {
	/* if not successful, return warning that format is not registered */
	ret_value = ClipboardFail;

	*format_length = 8;

    }else{
	ret_value = ClipboardSuccess;

	/* return the length of the format */
	*format_length = *((long *)outpointer);
    }

    if (outpointer != NULL)
      XFree((char*)outpointer);

    return ret_value;
}

/*---------------------------------------------*/
static ClipboardHeader
ClipboardOpen(
        Display *display,
        int add_length )
{
    int ret_value;
    unsigned long headerlength;
    ClipboardHeader root_clipboard_header;
    Atom headertype, type;
    long number ;
    unsigned long length;
    XtPointer int_ptr;

    ret_value = ClipboardSuccess;

    if ( add_length == 0 )
    {
	/* get the clipboard header */
	ret_value = ClipboardFindItem(display,
				      XM_HEADER_ID,
				      (XtPointer *) &root_clipboard_header,
				      &headerlength,
				      &headertype,
				      0,
				      0 );
    }

    if ( add_length != 0 || ret_value != ClipboardSuccess )
    {
	/* get the clipboard header (this will allocate memory
	   if doesn't exist) */

	ret_value = ClipboardRetrieveItem(display,
					  XM_HEADER_ID,
					  add_length,
					  sizeof( ClipboardHeaderRec ),
					  (XtPointer *) &root_clipboard_header,
					  &headerlength,
					  NULL,
					  0,
					  0,
					  0 ); /* don't discard old data */
    }

    /* means clipboard header had not been initialized */
    if ( ret_value != ClipboardSuccess )
    {
    	root_clipboard_header->recordType = XM_HEADER_RECORD_TYPE;
    	root_clipboard_header->adjunctHeader = 0;
    	root_clipboard_header->maxItems = 1;
    	root_clipboard_header->currItems = 0;
    	root_clipboard_header->dataItemList =
	  sizeof( ClipboardHeaderRec ) / CONVERT_32_FACTOR;
    	root_clipboard_header->nextPasteItemId = 0;
    	root_clipboard_header->lastCopyItemId = 0;
    	root_clipboard_header->recopyId = 0;
    	root_clipboard_header->oldNextPasteItemId = 0;
    	root_clipboard_header->deletedByCopyId = 0;
	root_clipboard_header->ownSelection = 0;
	root_clipboard_header->selectionTimestamp = CurrentTime;
	root_clipboard_header->copyFromTimestamp  = CurrentTime;
    	root_clipboard_header->foreignCopiedLength = 0;
    	root_clipboard_header->incrementalCopyFrom = 0;
	root_clipboard_header->startCopyCalled = (unsigned long) False;
    }

    /* make sure "next free id" property has been initialized */
    ret_value = ClipboardFindItem(display,
				  XM_NEXT_ID,
				  &int_ptr,
				  &length,
				  &type,
				  0,
				  0 );

    if ( ret_value != ClipboardSuccess )
    {
	number = XM_FIRST_FREE_ID;
    	int_ptr = (XtPointer) &number;

    	/* initialize the next id property */
	ClipboardReplaceItem(display,
			     XM_NEXT_ID,
			     int_ptr,
			     sizeof(long),
			     PropModeReplace,
			     32,
			     False,
			     XA_INTEGER );
    }
    else
    {
    	XtFree( (char*)int_ptr );
    }

    return root_clipboard_header;
}

/*---------------------------------------------*/
static void
ClipboardClose(
        Display *display,
        ClipboardHeader root_clipboard_header )
{
    unsigned long headerlength;

    headerlength = sizeof( ClipboardHeaderRec ) +
    		(root_clipboard_header -> currItems) * sizeof( itemId );

    /* replace the clipboard header */
    ClipboardReplaceItem(display,
			 XM_HEADER_ID,
			 (XtPointer)root_clipboard_header,
			 headerlength,
			 PropModeReplace,
			 32,
			 True,
			 XA_INTEGER );
}

/*---------------------------------------------*/
static void
ClipboardDeleteId(
        Display *display,
        itemId itemid )
{
    Window rootwindow;
    Atom itematom;

    rootwindow = RootWindow( display, 0 );

    itematom = ClipboardGetAtomFromId( display, itemid );

    XDeleteProperty( display, rootwindow, itematom );

}


/*---------------------------------------------*/
static ClipboardFormatItem
ClipboardFindFormat(
        Display *display,       /* Display id of application wanting data */
        ClipboardHeader header,
        char *format,
        itemId itemid,
        int n,                  /* if looking for nth format */
        unsigned long *maxnamelength,/* receives max format name length */
        int *count,             /* receives next paste format count */
        unsigned long *matchlength )      /* receives length of matching format */
{
    ClipboardDataItem queryitem;
    ClipboardFormatItem currformat, matchformat;
    unsigned long reclength ;
    int i, free_flag, index;
    itemId currformatid, queryitemid, *idptr;
    Atom formatatom;
    Atom rectype;

    *count = 0;
    *maxnamelength = 0;

    if ( itemid < 0 ) return 0;

    /* if passed an item id then use that, otherwise use next paste item */
    if ( itemid != 0 )
    {
    	queryitemid = itemid;

    }else{

    	if ( header->currItems == 0 ) return 0;

        queryitemid = header->nextPasteItemId;
    }

    if ( queryitemid == 0 ) return 0;

    /* get the query item */
    if (ClipboardFindItem(display, queryitemid, (XtPointer *) &queryitem,
			  &reclength, &rectype, 0,
			  XM_DATA_ITEM_RECORD_TYPE ) == ClipboardFail )
    	return 0;

    if ( queryitem == 0 )
    {
	CleanupHeader (display);
	ClipboardError( CLIPBOARD_CORRUPT, CORRUPT_DATA_STRUCTURE );
	return 0;
    }

    *count = queryitem->formatCount - queryitem->cancelledFormatCount;

    if ( *count < 0 ) *count = 0;

    /* point to the first format id in the list */
    idptr = (itemId*)((char*)queryitem +
		      queryitem->formatIdList * CONVERT_32_FACTOR);

    matchformat = 0;
    *matchlength = 0;
    index = 1;
    formatatom = XInternAtom( display, format, False );

    /* run through all the formats for the query item looking */
    /* for a name match with the input format name */
    for ( i = 0; i < queryitem->formatCount; i++ )
    {
	currformatid = *idptr;

    	/* free the allocation unless it is the matching format	*/
    	free_flag = 1;

	/* get the next format */
	ClipboardFindItem(display, currformatid,
			  (XtPointer *) &currformat,
			  &reclength,
			  &rectype,
			  0,
			  XM_FORMAT_HEADER_TYPE );

    	if ( currformat == 0 )
    	{
	    CleanupHeader (display);
	    ClipboardError( CLIPBOARD_CORRUPT, CORRUPT_DATA_STRUCTURE );
    	    return 0;
    	}

    	if ( currformat->cancelledFlag == 0 )
    	{
	    /* format has not been cancelled */
	    *maxnamelength = MAX( *maxnamelength,
				  currformat->formatNameLength );

	    if (format != NULL)
    	    {
		if ( currformat->formatNameAtom == formatatom )
		{
		    matchformat = currformat;
    		    free_flag = 0;
    		    *matchlength = reclength;
    		}

    	    }else{
		/* we're looking for the n'th format */
    		if ( index == n )
    		{
		    matchformat = currformat;
    		    free_flag = 0;
    		    *matchlength = reclength;
    		}

		index = index + 1;
    	    }
    	}

    	if (free_flag == 1 )
    	{
    	    XtFree( (char *) currformat );
    	}

    	idptr = idptr + 1;
    }

    XtFree( (char *) queryitem );

    return matchformat;
}

/*---------------------------------------------*/
static void
ClipboardDeleteFormat(
        Display *display,
        itemId formatitemid )
{
    itemId  dataitemid;
    ClipboardDataItem dataitem;
    ClipboardFormatItem formatitem;
    unsigned long length ;
    unsigned long formatlength;
    Atom formattype, type;

    /* first get the format item out of the root */
    ClipboardFindItem( display,
    			   formatitemid,
    			   (XtPointer *) &formatitem,
    			   &formatlength,
			   &formattype,
			   0,
    			   XM_FORMAT_HEADER_TYPE );

    if ( formatitem == 0 )
    {
	CleanupHeader (display);
	ClipboardError( CLIPBOARD_CORRUPT, CORRUPT_DATA_STRUCTURE );
	return;
    }

    if ( ( formatitem->cutByNameFlag == 0 ) ||
    	 ( formatitem->cancelledFlag != 0 ) )
    {
    	/* nothing to do, data not passed by name or already cancelled */
    	XtFree( (char *) formatitem );
    	return;
    }

    dataitemid = formatitem->parentItemId;

    /* now get the data item out of the root */
    ClipboardFindItem(display,
		      dataitemid,
		      (XtPointer *) &dataitem,
		      &length,
		      &type,
		      0,
		      XM_DATA_ITEM_RECORD_TYPE );

    if ( dataitem == 0 )
    {
	CleanupHeader (display);
	ClipboardError( CLIPBOARD_CORRUPT, CORRUPT_DATA_STRUCTURE );
	return;
    }

    dataitem->cancelledFormatCount = dataitem->cancelledFormatCount + 1;

    if ( dataitem->cancelledFormatCount == dataitem->formatCount )
    {
    	/* no formats left, mark the item for delete */
        dataitem->deletePendingFlag = 1;
    }

    /* set the cancel flag on */
    formatitem->cancelledFlag = 1;

    /* return the property on the root window for the item */
    ClipboardReplaceItem(display,
			 formatitemid,
			 (XtPointer)formatitem,
			 formatlength,
			 PropModeReplace,
			 32,
			 True,
			 XA_INTEGER );

    ClipboardReplaceItem(display,
			 dataitemid,
			 (XtPointer)dataitem,
			 length,
			 PropModeReplace,
			 32,
			 True,
			 XA_INTEGER);

}

/*---------------------------------------------*/
static void
ClipboardDeleteFormats(
        Display *display,
        Window window,
        itemId dataitemid )
{
    itemId *deleteptr;
    ClipboardDataItem datalist;
    ClipboardFormatItem formatdata;
    unsigned long length ;
    Atom type;
    int i;

    /* first get the data item out of the root */
    ClipboardFindItem( display,
    			   dataitemid,
    			   (XtPointer *) &datalist,
    			   &length,
			   &type,
			   0,
    			   XM_DATA_ITEM_RECORD_TYPE );

    if ( datalist == 0 )
    {
	CleanupHeader (display);
	ClipboardError( CLIPBOARD_CORRUPT, CORRUPT_DATA_STRUCTURE );
	return;
    }

    deleteptr = (itemId*)((char*) datalist +
			  datalist->formatIdList * CONVERT_32_FACTOR );

    for ( i = 0; i < datalist->formatCount; i++ )
    {
    	/* first delete the format data */
	ClipboardFindItem( display,
			       *deleteptr,
			       (XtPointer *) &formatdata,
			       &length,
			       &type,
			       0,
    			       XM_FORMAT_HEADER_TYPE );

    	if ( formatdata == 0 )
    	{
	    CleanupHeader (display);
	    ClipboardError( CLIPBOARD_CORRUPT, CORRUPT_DATA_STRUCTURE );
    	    return;
    	}

    	if ( formatdata->cutByNameFlag == 1 )
    	{
    	    /* format was cut by name */
	    ClipboardSendMessage( display,
    				      window,
    				      formatdata,
				      XM_DATA_DELETE_MESSAGE );
    	}

    	ClipboardDeleteId( display, formatdata->formatDataId );

        XtFree( (char *) formatdata );

    	/* then delete the format header */
    	ClipboardDeleteId( display, *deleteptr );

    	*deleteptr = 0;

    	deleteptr = deleteptr + 1;
    }

    XtFree( (char *) datalist );
}

/*---------------------------------------------*/
/* ARGSUSED */
 static void
ClipboardDeleteItemLabel(
        Display *display,
        Window window,
        itemId dataitemid )
{
     ClipboardDataItem datalist;
     unsigned long length;
     Atom type;

     /* first get the data item out of the root */
     ClipboardFindItem( display,
                          dataitemid,
                          (XtPointer *) &datalist,
                          &length,
			  &type,
                          0,
                          XM_DATA_ITEM_RECORD_TYPE );

     if ( datalist == 0 )
     {
	CleanupHeader (display);
	ClipboardError( CLIPBOARD_CORRUPT, CORRUPT_DATA_STRUCTURE );
	return;
     }
     /* delete item label */
     ClipboardDeleteId (display, datalist->dataItemLabelId);

     XtFree( (char *) datalist );
 }

/*---------------------------------------------*/
/* ARGSUSED */
static unsigned long
ClipboardIsMarkedForDelete(
        Display *display,
        ClipboardHeader header,
        itemId itemid )
{
    ClipboardDataItem curritem;
    unsigned long return_value, reclength;
    Atom rectype;

    if ( itemid == 0 )
    {
	CleanupHeader (display);
	ClipboardError( CLIPBOARD_CORRUPT, CORRUPT_DATA_STRUCTURE );
    	return 0;
    }

    /* get the next format */
    ClipboardFindItem( display,
			   itemid,
			   (XtPointer *) &curritem,
			   &reclength,
			   &rectype,
			   0,
    			   XM_DATA_ITEM_RECORD_TYPE );

    if ( curritem == 0 )
    {
	CleanupHeader (display);
	ClipboardError( CLIPBOARD_CORRUPT, CORRUPT_DATA_STRUCTURE );
    	return 0;
    }

    return_value = curritem->deletePendingFlag;

    XtFree( (char *) curritem );

    return return_value;
}

/*---------------------------------------------*/
static void
ClipboardDeleteItem(
        Display *display,
        Window window,
        ClipboardHeader header,
        itemId deleteid )
{
    int i;
    itemId *listptr,*thisid, *nextid, nextpasteid;
    int nextpasteindex;
    int lastflag = 0;

    /* find the delete id in the header item list */
    listptr = (itemId*)((char*) header +
			header->dataItemList * CONVERT_32_FACTOR );

    i = 0;

    nextpasteindex = 0;
    nextpasteid    = 0;

    nextid = listptr;
    thisid = nextid;

    /* redo the item list */
    if(    !header->currItems    )
    {   return ;
        }
    while ( i < header->currItems )
    {
    	i++ ;

	if (*nextid == deleteid )
    	{
    	    nextid++;

    	    nextpasteindex = i - 2;

    	    /* if this flag doesn't get reset, then delete item
	       was last item */
    	    lastflag = 1;

    	    continue;
    	}

    	lastflag = 0;

    	*thisid = *nextid;

    	thisid = thisid + 1;
	nextid = nextid + 1;
    }

    *thisid = 0;

    header->currItems = header->currItems - 1;

    /* if we are deleting the next paste item, then we need to find
       a new one */
    if ( header->nextPasteItemId == deleteid )
    {

	if ( lastflag == 1 )
	{
	    nextpasteindex = nextpasteindex - 1;
	}

	/* store this value temporarily */
	i = nextpasteindex;

	/* now find the next paste candidate
	   first try to find next older item to make next paste */
	while ( nextpasteindex >= 0 )
	{
	    thisid = listptr + nextpasteindex;

	    if ( !ClipboardIsMarkedForDelete( display, header, *thisid ) )
	    {
		nextpasteid = *thisid;
		break;
	    }

	    nextpasteindex = nextpasteindex - 1;
	}

	/* if didn't find a next older item, find next newer item */
	if ( nextpasteid == 0 )
	{
	    /* restore this value */
	    nextpasteindex = i;

	    while ( nextpasteindex < header->currItems )
	    {
		thisid = listptr + nextpasteindex;

		if ( !ClipboardIsMarkedForDelete( display, header, *thisid ) )
		{
		    nextpasteid = *thisid;
		    break;
		}

		nextpasteindex = nextpasteindex + 1;
	    }
	}

        header->nextPasteItemId = nextpasteid;
        header->oldNextPasteItemId = 0;
    }
     /* delete the item label */
     ClipboardDeleteItemLabel( display, window, deleteid);

    /* delete all the formats belonging to the data item */
    ClipboardDeleteFormats( display, window, deleteid );

    /* now delete the item itself */
    ClipboardDeleteId( display, deleteid );

    /* Cleanup any callback information for ByName */
    {
      Boolean found = False;

      _XmProcessLock();
      for(i = 0; i < maxCbProcs; i++)
      {
        found = (cbIdTable[i] == deleteid);
	if (found)
            break;
      }
      if (found) {
	cbProcTable[i] = NULL;
	cbIdTable[i] = 0;
      }
      _XmProcessUnlock();
    }
}


/*---------------------------------------------*/
static void
ClipboardDeleteMarked(
        Display *display,
        Window window,
        ClipboardHeader header )
{
    itemId *nextIdPtr;
    unsigned long endi, i;

    /* find the header item list */
    nextIdPtr = (itemId*)((char*) header +
			  header->dataItemList * CONVERT_32_FACTOR);

    i = 0;
    endi = header->currItems;

    /* run through the item list looking for things to delete */
    while( 1 )
    {
    	if ( i >= endi ) break;

    	i = i + 1;

	if ( ClipboardIsMarkedForDelete( display, header, *nextIdPtr ) )
    	{
    	    ClipboardDeleteItem( display, window, header, *nextIdPtr );

    	}else{
    	    nextIdPtr = nextIdPtr + 1;
    	}
    }
}
/*---------------------------------------------*/
/* ARGSUSED */
static void
ClipboardMarkItem(
        Display *display,
        ClipboardHeader header,
        itemId dataitemid,
        unsigned long state )
{
    ClipboardDataItem itemheader;
    unsigned long itemlength;
    Atom itemtype;

    if ( dataitemid == 0 ) return;

    /* get a pointer to the item */
    ClipboardFindItem(display,
		      dataitemid,
		      (XtPointer *) &itemheader,
		      &itemlength,
		      &itemtype,
		      0,
		      XM_DATA_ITEM_RECORD_TYPE );

    if ( itemheader == 0 )
    {
	CleanupHeader (display);
	ClipboardError( CLIPBOARD_CORRUPT, CORRUPT_DATA_STRUCTURE );
    	return;
    }

    /* mark the delete pending flag */
    itemheader->deletePendingFlag = state;

    /* return the item to the root window */
    ClipboardReplaceItem(display, dataitemid, (XtPointer)itemheader,
			 itemlength, PropModeReplace, 32, True,
			 XA_INTEGER );

}

/*---------------------------------------------*/
static int
ClipboardSendMessage(
        Display *display,
        Window window,
        ClipboardFormatItem formatptr,
        int messagetype )
{
    enum { XmA_MOTIF_CLIP_MESSAGE, XmA_MOTIF_CLIP_DATA_REQUEST,
	   XmA_MOTIF_CLIP_DATA_DELETE, NUM_ATOMS };
    static char *atom_names[] = {
      XmS_MOTIF_CLIP_MESSAGE, XmS_MOTIF_CLIP_DATA_REQUEST,
      XmS_MOTIF_CLIP_DATA_DELETE };

    Window widgetwindow;
    XClientMessageEvent event_sent;
    long event_mask = 0;
    unsigned long headerlength;
    ClipboardHeader root_clipboard_header;
    Boolean dummy;
    Atom headertype;
    Atom atoms[XtNumber(atom_names)];

    widgetwindow = formatptr->cutByNameWindow;

    if ( widgetwindow == 0 ) return 0;

    assert(XtNumber(atom_names) == NUM_ATOMS);
    XInternAtoms( display, atom_names, XtNumber(atom_names), False, atoms );
    event_sent.type         = ClientMessage;
    event_sent.window       = widgetwindow;
    event_sent.message_type = atoms[XmA_MOTIF_CLIP_MESSAGE];
    event_sent.format	    = 32;

    switch ( messagetype )
    {
	case XM_DATA_REQUEST_MESSAGE:

	    /* get the clipboard header */
	    ClipboardFindItem( display,
				   XM_HEADER_ID,
				   (XtPointer *) &root_clipboard_header,
				   &headerlength,
				   &headertype,
				   0,
				   0 );

	    /* set the recopy item id in the header (so locking
	       can be circumvented) */

	    root_clipboard_header->recopyId = formatptr->thisFormatId;

	    /* replace the clipboard header */
	    ClipboardReplaceItem(display, XM_HEADER_ID,
				 (XtPointer)root_clipboard_header,
				 headerlength, PropModeReplace,
				 32, True, XA_INTEGER );

            event_sent.data.l[0] = atoms[XmA_MOTIF_CLIP_DATA_REQUEST];
	    break;

	case XM_DATA_DELETE_MESSAGE:
            event_sent.data.l[0] = atoms[XmA_MOTIF_CLIP_DATA_DELETE];
	    break;
    }

    event_sent.data.l[1] = formatptr->thisFormatId;
    event_sent.data.l[2] = formatptr->itemPrivateId;

    /* is this the same application that stored the data? */
    if ( formatptr->windowId == window )
    {
    	/* call the event handler directly to avoid blocking */
    	ClipboardEventHandler( XtWindowToWidget(display,
						formatptr->cutByNameWindow),
			      0,
			      (XEvent *) &event_sent,
			      &dummy );
    }else{

	/* if we aren't in same application that stored the data, then
	   make sure the window still exists */
        if ( !ClipboardWindowExists( display, widgetwindow )) return 0;

	/* send a client message to the window supplied by the user */
	XSendEvent( display, widgetwindow, True, event_mask,
		    (XEvent*)&event_sent );
    }

    return 1;
}

/*---------------------------------------------*/
static int
ClipboardDataIsReady(
        Display *display,
        XEvent *event,
        char *private_info )
{
     XDestroyWindowEvent *destroy_event;
     ClipboardCutByNameInfo cutbynameinfo;
     ClipboardFormatItem formatitem;
     unsigned long formatlength;
     Atom formattype;
     int okay;

     cutbynameinfo = ( ClipboardCutByNameInfo )private_info;

     if ( (event->type & 127) == DestroyNotify )
     {
        destroy_event = (XDestroyWindowEvent*)event;

        if ( destroy_event->window == cutbynameinfo->window )
        {
            cutbynameinfo->window = 0;
            return 1;
        }
     }

     if ( (event->type & 127) != PropertyNotify )
    	return 0;


     /* get the format item */
     ClipboardFindItem( display,
 			   cutbynameinfo->formatitemid,
 			   (XtPointer *) &formatitem,
 			   &formatlength,
			   &formattype,
			   0,
    			   XM_FORMAT_HEADER_TYPE );

     if ( formatitem == 0 )
     {
	CleanupHeader (display);
	ClipboardError( CLIPBOARD_CORRUPT, CORRUPT_DATA_STRUCTURE );
	return 0;
     }

     okay = (int)( formatitem->cutByNameFlag == 0 );

     /* release the allocation */
     XtFree( (char *) formatitem );

     return okay;

}


#if 0
/* This function is currently unused. */

/*---------------------------------------------*/
/* ARGSUSED */
static int
ClipboardRequestorIsReady(
        Display *display,
        XEvent *event,
        char *private_info )
{
    XPropertyEvent *property_event;
    XDestroyWindowEvent *destroy_event;
    ClipboardDestroyInfo info;

    info = ( ClipboardDestroyInfo )private_info;

    if ( (event->type & 127) == DestroyNotify )
    {
        destroy_event = (XDestroyWindowEvent*)event;

        if ( destroy_event->window == info->window )
        {
            info->window = 0;
            return 1;
        }
    }

    if ( (event->type & 127) == PropertyNotify )
    {
        property_event = (XPropertyEvent*)event;

        /* make sure we have right property and are ready */
        if ( property_event->atom == info->property
                            &&
             property_event->state == PropertyDelete )
        {
            return 1;
        }
    }

    return 0;
 }
#endif

/*---------------------------------------------*/
static int
ClipboardGetSelection(Display *display,
		      Window window,
		      Atom target,
		      XtPointer *value,
		      Atom *type,
		      unsigned long *size,
		      int *format)
{
    ClipboardSelectionInfoRec info;
    Widget dest;
    XtAppContext app;
    Atom XmA_CLIPBOARD;

    dest = XtWindowToWidget(display, window);

    if (dest == (Widget) NULL) {
      return(FALSE);
    }

    app = XtWidgetToApplicationContext(dest);

    /* initialize the fields in the info record passed to the */
    /* predicate function */
    info.success = FALSE;
    info.received = FALSE;
    info.data = NULL;
    info.count = 0;
    info.format = 8;
    info.type = None;

    /* ask for the data in the specified format */
    XmA_CLIPBOARD = XInternAtom( display, XmSCLIPBOARD, False );
    XtGetSelectionValue(dest, XmA_CLIPBOARD, target,
			ClipboardReceiveData, &info,
			XtLastTimestampProcessed(display));

#ifdef XTHREADS
    while (XtAppGetExitFlag(app) == False) {
#else
    for(;;) {
#endif
      XEvent event;
      XtInputMask mask;
      if (info.received) break;
#ifndef XTHREADS
      XtAppNextEvent(app, &event);
      XtDispatchEvent(&event);
#else

      while (!(mask = XtAppPending(app)))
        ;  /* Busy waiting - so that we don't lose our lock */
      if (mask & XtIMXEvent) { /* We have an XEvent */
        /* Get the event since we know its there.
         * Note that XtAppNextEvent would also process
         * timers/alternate inputs.
         */
        XtAppNextEvent(app, &event); /* no blocking */
        XtDispatchEvent(&event); /* Process it */
      }
      else /* not an XEvent, process it */
        XtAppProcessEvent(app, mask); /* non blocking */
#endif
   }

    *value = info.data;
    *size = info.count;
    *type = info.type;
    *format = info.format;

    if (*value == NULL || *size == 0)
	return FALSE;

    return TRUE;
}

/*ARGSUSED*/
static void
ClipboardReceiveData(Widget dest, /* unused */
		     XtPointer client_data,
		     Atom *selection, /* unused */
		     Atom *type,
		     XtPointer value,
		     unsigned long *length,
		     int *format)
{
  ClipboardSelectionInfo info;

  info = ( ClipboardSelectionInfo ) client_data;

  info->received = TRUE;

  if ( *type != XT_CONVERT_FAIL )
    {
      info -> format = *format;
      info -> count = BYTELENGTH(*length, *format);
      info -> type = *type;
      info -> data = (char*) value;
      info -> success = TRUE;
    }
  else
    {
      info -> format = 8;
      info -> count = 0;
      info -> type = None;
      info -> data = NULL;
      info -> success = FALSE;
    }
}

static int
ClipboardRequestDataAndWait(
        Display *display,
        Window window,
        ClipboardFormatItem formatptr )
{
    XEvent event_return;
    int dataisready;
    unsigned long maxtime;
    XWindowAttributes rootattributes;
    Window rootwindow;
    ClipboardCutByNameInfoRec cutbynameinfo;
    Boolean timer_expired;
    XtAppContext app_context = NULL;
    XtIntervalId timerid;
    Widget wid;

    rootwindow = RootWindow( display, 0 );

    /* get the current root window event mask */
    XGetWindowAttributes( display, rootwindow, &rootattributes );

    /* select for property notify as well as current mask */
    XSelectInput( display,
		  rootwindow,
		  PropertyChangeMask  |
		  StructureNotifyMask | rootattributes.your_event_mask );

    if ( ClipboardSendMessage( display,
    			           window,
    			           formatptr,
    			           XM_DATA_REQUEST_MESSAGE ) == 0 )
    {
	/* put mask back the way it was */
	XSelectInput( display,
		      rootwindow,
		      rootattributes.your_event_mask );
    	return 0;
    }

    cutbynameinfo.formatitemid = formatptr->thisFormatId;
    cutbynameinfo.window = window;

    dataisready = XCheckIfEvent( display,
    				 &event_return,
    				 ClipboardDataIsReady,
                                 (char*)&cutbynameinfo );

    if ( cutbynameinfo.window == 0 )
    {
        /* this means the cut by name window had been destroyed */
        return 0;
    }

    /* We quit after the timeout to prevent deadlock */
    wid = XtWindowToWidget(display, window);
    if (wid != (Widget) NULL) {
      app_context = XtWidgetToApplicationContext(wid);
      maxtime = XtAppGetSelectionTimeout(app_context);
    } else {
      maxtime = 5000;
    }
    timer_expired = False;
    timerid = XtAppAddTimeOut(app_context, maxtime,
			      ClipboardTimeout, &timer_expired);
#ifdef XTHREADS
    while( !dataisready && !timer_expired &&
		(XtAppGetExitFlag(app_context) == False)) {
#else
    while( !dataisready && !timer_expired) {
#endif
      XtInputMask mask;
#ifndef XTHREADS
      XtAppNextEvent(app_context, &event_return);
      dataisready =
	ClipboardDataIsReady(display, &event_return,
				(char*)&cutbynameinfo);
      XtDispatchEvent(&event_return);
#else
      while (!(mask = XtAppPending(app_context)))
		; /* busy waiting - don't lose lock */
      if (mask & XtIMXEvent) {
	  XtAppNextEvent(app_context, &event_return);
	  dataisready =
	      ClipboardDataIsReady(display, &event_return,
				(char*)&cutbynameinfo);
	  XtDispatchEvent(&event_return);
      }
      else
	  XtAppProcessEvent(app_context, mask);
#endif
    }
    if (! timer_expired) XtRemoveTimeOut(timerid);

    if (! dataisready) return 0; /* Fail,  no response */

    if ( cutbynameinfo.window == 0 )
    {
        /* this means the cut by name window had been destroyed */
        return 0;
    }

    /* put mask back the way it was */
    XSelectInput( display,
		  rootwindow,
		  rootattributes.your_event_mask );

    return 1;
}

/*ARGSUSED*/
static void
ClipboardTimeout(XtPointer client_data,
		 XtIntervalId* timer) /* unused */
{
  Boolean *flag = (Boolean *) client_data;

  *flag = True;
}


/*---------------------------------------------*/
static itemId
ClipboardGetNewItemId(
                        Display *display )
{
    XtPointer propertynumber;
    unsigned long length;
    itemId loc_id;
    Atom type;

    ClipboardFindItem(display, XM_NEXT_ID, &propertynumber, &length,
		      &type, 0, 0 );

    loc_id = ++*((itemId*)propertynumber);

    ClipboardReplaceItem(display, XM_NEXT_ID, propertynumber, length,
			 PropModeReplace, 32, True, XA_INTEGER);

    return loc_id;
}


/*---------------------------------------------*/
static void
ClipboardSetAccess(
        Display *display,
        Window window )
{
    Atom itematom;

    itematom = XInternAtom( display, XmS_MOTIF_CLIP_LOCK_ACCESS_VALID, False );

    /* put the clipboard lock access valid property on window */
    XChangeProperty( display,
		     window,
		     itematom,
		     itematom,
		     8,
		     PropModeReplace,
		     (unsigned char*)"yes",
		     3 );
}

/*---------------------------------------------*/
static int
ClipboardLock(
        Display *display,
        Window window )
{
    enum { XmA_MOTIF_CLIP_LOCK, XmA_CLIPBOARD, NUM_ATOMS };
    static char *atom_names[] = { XmS_MOTIF_CLIP_LOCK, XmSCLIPBOARD };

    ClipboardLockPtr lockptr;
    unsigned long length;
    Window lock_owner;
    Boolean take_lock = False;
    Atom ignoretype;
    Atom atoms[XtNumber(atom_names)];
    _XmDisplayToAppContext(display);

    assert(XtNumber(atom_names) == NUM_ATOMS);
    XInternAtoms(display, atom_names, XtNumber(atom_names), False, atoms);

    _XmAppLock(app);

    lock_owner = XGetSelectionOwner (display, atoms[XmA_MOTIF_CLIP_LOCK]);

    if (lock_owner != window && lock_owner != None) {
	_XmAppUnlock(app);
	return (ClipboardLocked);
    }

    ClipboardFindItem (display, XM_LOCK_ID, (XtPointer *)&lockptr,
                          &length, &ignoretype, 0, 0);

    if (length == 0)	/* create new lock property */
    {
	lockptr = (ClipboardLockPtr)XtMalloc(sizeof(ClipboardLockRec));
	lockptr->lockLevel = 0;
    }

    if (lockptr->lockLevel == 0) /* new or invalid V1.0 lock. Take the lock */
    {
	lockptr->windowId = window;
	lockptr->lockLevel = 1;
	take_lock = True;
    }
    else if (lockptr->windowId == window) /* already have the lock */
    {
	lockptr->lockLevel += 1;
    }
    else	/* another client has the lock */
    {
	if (ClipboardWindowExists (display, lockptr->windowId))
	{
	    XtFree ((char *)lockptr);
	    _XmAppUnlock(app);
	    return (ClipboardLocked);
	}
	else /* locking client has gone away, clipboard may be corrupted */
	{
	    ClipboardHeader header;
	    Window owner = XGetSelectionOwner (display, atoms[XmA_CLIPBOARD]);
	    Time timestamp = ClipboardGetCurrentTime(display);

	    /* Drop the selection if a Motif client owns it */

            header = ClipboardOpen (display, 0);
	    if (header->ownSelection == owner)
	    {
		XSetSelectionOwner (display, XmA_CLIPBOARD, None, timestamp);
	    }
            ClipboardClose (display, header);

	    /* Reset the header property */

	    CleanupHeader (display);
	    header = ClipboardOpen (display, 0);
	    ClipboardClose (display, header);

	    /* Take the lock */

	    lockptr->windowId = window;
	    lockptr->lockLevel = 1;
	    take_lock = True;
	}
    }

    if (take_lock == True) {
	if (XGetSelectionOwner (display, atoms[XmA_MOTIF_CLIP_LOCK]) == None) {
	    XSetSelectionOwner (display, atoms[XmA_MOTIF_CLIP_LOCK], window,
				ClipboardGetCurrentTime(display));
	    if (XGetSelectionOwner (display, atoms[XmA_MOTIF_CLIP_LOCK]) != window) {
		XtFree ((char *)lockptr);
	        _XmAppUnlock(app);
		return (ClipboardLocked);
	    }
	}
	else {
	    XtFree ((char *)lockptr);
	    _XmAppUnlock(app);
	    return (ClipboardLocked);
	}
    }

    ClipboardReplaceItem (display, XM_LOCK_ID, (XtPointer)lockptr,
			  sizeof(ClipboardLockRec), PropModeReplace,
			  32, False, XA_INTEGER );

    ClipboardSetAccess (display, window);

    XtFree ((char *)lockptr);

    _XmAppUnlock(app);
    return (ClipboardSuccess);
}

/*---------------------------------------------*/
static int
ClipboardUnlock(
        Display *display,
        Window window,
#if NeedWidePrototypes
        int all_levels )
#else
        Boolean all_levels )
#endif /* NeedWidePrototypes */
{
    unsigned long length;
    ClipboardLockPtr lockptr;
    Atom _MOTIF_CLIP_LOCK = XInternAtom (display, XmS_MOTIF_CLIP_LOCK, False);
    Window lock_owner = XGetSelectionOwner (display, _MOTIF_CLIP_LOCK);
    Boolean release_lock = False;
    Atom ignoretype;

    if (lock_owner != window && lock_owner != None)
	return (ClipboardFail);

    ClipboardFindItem (display, XM_LOCK_ID, (XtPointer *)&lockptr,
		       &length, &ignoretype, 0, 0);

    if (length == 0) /* There is no lock property */
    {
    	return (ClipboardFail);
    }

    if ( lockptr->windowId != window ) /* Someone else has the lock */
    {
    	XtFree ((char *)lockptr);
    	return (ClipboardFail);
    }

    /* do the unlock */

    if (all_levels == 0)
    {
        lockptr->lockLevel -= 1;
    }else{
        lockptr->lockLevel = 0;
    }

    if (lockptr->lockLevel <= 0)
    {
	length = 0;
	release_lock = True;
    }else{
	length = sizeof(ClipboardLockRec);
    }

    ClipboardReplaceItem (display, XM_LOCK_ID, (XtPointer)lockptr, length,
			  PropModeReplace, 32, False, XA_INTEGER);
    XtFree ((char *)lockptr);


    if (release_lock == True) {
	XSetSelectionOwner (display, _MOTIF_CLIP_LOCK, None,
			    ClipboardGetCurrentTime(display));
    }

    return (ClipboardSuccess);
}

/*---------------------------------------------*/
static int
ClipboardSearchForWindow(
        Display *display,
        Window parentwindow,
        Window window )
{
    /* recursively search the roots descendant tree for the given window */

    Window rootwindow, p_window, *children;
    unsigned int numchildren;
    int found, i;
    Window *windowptr;

    if (XQueryTree( display, parentwindow, &rootwindow, &p_window,
		     &children, &numchildren ) == 0)
    {
	return (0);
    }

    found = 0;
    windowptr = children;

    /* now search through the list for the window */
    for ( i = 0; i < numchildren; i++ )
    {
	if ( *windowptr == window )
    	{
    	    found = 1;
    	}else{
    	    found = ClipboardSearchForWindow( display, *windowptr, window);
   	}
    	if ( found == 1 ) break;
	windowptr = windowptr + 1;
    }

    XtFree( (char*)children );

    return found;
}

/*---------------------------------------------*/
static int
ClipboardWindowExists(
        Display *display,
        Window window )
{
    Window rootwindow;
    Atom itematom;
    int exists;
    unsigned long outlength;
    unsigned char *outpointer;
    Atom type;
    int format;
    unsigned long bytes_left;

    outpointer = NULL;

    rootwindow = RootWindow( display, 0 );

    exists = ClipboardSearchForWindow( display, rootwindow, window );

    if ( exists == 1 )
    {
	/* see if the window has the lock activity property, for if
	   it doesn't then this is a new assignment of the window id
	   and the lock is bogus due to a crash of the application
	   with the original locking window */
       itematom = XInternAtom(display, XmS_MOTIF_CLIP_LOCK_ACCESS_VALID, False);

       XGetWindowProperty( display,
			window,
			itematom,
			0, /*offset*/
			10000000, /*length*/
			False,
			AnyPropertyType,
			&type,
			&format,
			&outlength,
			&bytes_left,
			&outpointer );

	if ( outpointer == 0 || outlength == 0 )
    	{
    	    /* not the same window that locked the clipboard */
	    exists = 0;
    	}

    if(outpointer != NULL)
	   XFree((char*)outpointer);
    }

    return exists;
}
/*---------------------------------------------*/
/* external routines			       */
/*---------------------------------------------*/


/*---------------------------------------------*/
int
XmClipboardBeginCopy(
        Display *display,   /* display id for application passing data */
        Window window,  /* window to receive request for cut by name data */
        XmString label,     /* label to be associated with data item   */
        Widget widget,      /* only for cut by name */
        VoidProc callback,/* addr of callback routine for cut by name */
        long *itemid )      /* received id to identify this data item */
{
    return(XmClipboardStartCopy( display, window, label, CurrentTime,
				widget, (XmCutPasteProc)callback, itemid ));
}

/*---------------------------------------------*/
int
XmClipboardStartCopy(
        Display *display,   /* display id for application passing data */
        Window window,  /* window to receive request for cut by name data */
        XmString label, /* label to be associated with data item   */
        Time timestamp, /* timestamp of event triggering copy to clipboard */
        Widget widget,  /* only for cut by name */
        XmCutPasteProc callback,/* addr of callback routine for cut by name */
        long *itemid )  /* received id to identify this data item */
{
    ClipboardHeader header;
    ClipboardDataItem itemheader;
    unsigned long itemlength;
    itemId loc_itemid;
    int status;
    char *asn1string;
    int asn1strlen;
    _XmDisplayToAppContext(display);

    _XmAppLock(app);
    status = ClipboardLock( display, window );
    if ( status == ClipboardLocked ) {
	_XmAppUnlock(app);
	return ClipboardLocked;
    }

    /* get the clipboard header, make sure clipboard is initialized */
    header = ClipboardOpen( display, 0 );

    header->selectionTimestamp = timestamp;
    header->startCopyCalled = (unsigned long) True;

    /* allocate storage for the data item  */
    itemlength = sizeof( ClipboardDataItemRec );

    itemheader = (ClipboardDataItem)XtMalloc( (size_t) itemlength );

    loc_itemid = ClipboardGetNewItemId( display );

    /* initialize fields in the data item */
    itemheader->thisItemId = loc_itemid;
    itemheader->adjunctData = 0;
    itemheader->recordType = XM_DATA_ITEM_RECORD_TYPE;
    itemheader->displayId = display;
    itemheader->windowId  = window;
    itemheader->dataItemLabelId = ClipboardGetNewItemId( display );
    itemheader->formatIdList = itemlength / CONVERT_32_FACTOR; /* offset */
    itemheader->formatCount = 0;
    itemheader->cancelledFormatCount = 0;
    itemheader->deletePendingFlag = 0;
    itemheader->permanentItemFlag = 0;
    itemheader->cutByNameFlag = 0;
    itemheader->cutByNameCBIndex = -1;
    itemheader->cutByNameWidget = 0;
    itemheader->cutByNameWindow = 0;

    if ( callback != 0 && widget != 0 )
    {
      int i = 0;
      Boolean found = False;

      _XmProcessLock();
      while(i < maxCbProcs && ! found) {
 	if (cbProcTable[i] == NULL)
 	  found = True;
 	else
 	  i++;
      }

      if (! found) {
 	int oldLimit = maxCbProcs;

 	maxCbProcs += 20;

 	/* Need to extend tables */
 	cbProcTable = (XmCutPasteProc *) XtRealloc((char*) cbProcTable,
 						   sizeof(XmCutPasteProc) *
 						   maxCbProcs);
 	cbIdTable = (long *) XtRealloc((char*) cbIdTable,
 				       sizeof(long) * maxCbProcs);
 	for(i = oldLimit; i < maxCbProcs; i++) {
 	  cbProcTable[i] = NULL;
 	  cbIdTable[i] = 0;
 	}
 	i = oldLimit;
      }

      cbProcTable[i] = callback;
      cbIdTable[i] = itemheader->thisItemId;
      _XmProcessUnlock();

      /* set up client message handling if widget passed */
      itemheader->cutByNameCBIndex = i;
      itemheader->cutByNameWidget = widget;
      itemheader->cutByNameWindow = XtWindow( widget );
      ClipboardSetAccess( display, itemheader->cutByNameWindow );
    }

    if (label != NULL) {
      asn1strlen = XmCvtXmStringToByteStream(label,
					     (unsigned char **) &asn1string);

      /* store the label */
      ClipboardReplaceItem(display,
			   itemheader->dataItemLabelId,
			   (XtPointer) asn1string,
			   asn1strlen,
			   PropModeReplace,
			   8,
			   False,
			   XInternAtom(display,
				       XmS_MOTIF_COMPOUND_STRING, False));

      XtFree(asn1string);
    }

    /* return the item to the root window */
    ClipboardReplaceItem(display,
			 loc_itemid,
			 (XtPointer)itemheader,
			 itemlength,
			 PropModeReplace,
			 32,
			 True,
			 XA_INTEGER );

    if ( itemid != 0 )
    {
        *itemid = (long)loc_itemid;
    }

    ClipboardClose( display, header );

    ClipboardUnlock( display, window, 0 );

    _XmAppUnlock(app);
    return ClipboardSuccess;
}

/* The following code is a workaround for UTM to pass in the type of the
   to be copied data.  It is a bug in the clipboard interface that the
   type information cannot be passed in.  Note that when Motif goes to
   multithread safety,  anything calling _XmClipboardPassType will need
   to have a critical section around that call and the call to
   XmClipboardCopy or XmClipboardCopyByName */

static Atom _passed_type = None;

void
_XmClipboardPassType(Atom type)
{
  _XmProcessLock();
  _passed_type = type;
  _XmProcessUnlock();
}

int
XmClipboardCopy(
        Display *display,   /* Display id of application passing data */
        Window window,
        long itemid,        /* id returned from begin copy */
        char *format,       /* Name string for data format */
        XtPointer buffer,   /* Address of buffer holding data in this format */
        unsigned long length,   /* Length of the data */
        long private_id,     /* Private id provide by application */
        long *dataid )       /* Data id returned by clipboard */
{
    ClipboardDataItem itemheader;
    ClipboardHeader header;
    ClipboardFormatItem formatptr;
    char *formatdataptr;
    itemId formatid, formatdataid, *idptr;
    char *to_ptr;
    int status, count, format_len ;
    unsigned long maxname, formatlength, itemlength, formatdatalength;
    Atom type;
    _XmDisplayToAppContext(display);

    _XmAppLock(app);
    _XmProcessLock();
    if (_passed_type != None) {
      type = _passed_type;
      _passed_type = None;
    } else {
      type = GetTypeFromTarget(display, XInternAtom(display, format, False));
    }
    _XmProcessUnlock();

    status = ClipboardLock( display, window );
    if ( status == ClipboardLocked ) {
	_XmAppUnlock(app);
	return ClipboardLocked;
    }

    /* get the clipboard header */
    header = ClipboardOpen( display, 0 );

    if (!header->startCopyCalled) {
	XmeWarning(NULL, XM_CLIPBOARD_MESSAGE1);
        ClipboardUnlock( display, window, 0 );
	_XmAppUnlock(app);
        return ClipboardFail;
    }

    /* first check to see if the format already exists for this item */
    formatptr = ClipboardFindFormat(display, header, format,
				    (itemId) itemid, 0,
				    &maxname, &count, &formatlength );

    /* if format doesn't exist, then have to access the data item
       record */
    if ( formatptr == 0 )
    {
	/* get a pointer to the item */
	status = ClipboardRetrieveItem(display,
				       (itemId)itemid,
				       sizeof( itemId ),
				       0,
				       (XtPointer *) &itemheader,
				       &itemlength,
				       NULL,
				       0,
				       XM_DATA_ITEM_RECORD_TYPE,
				       0 );
	if (status != ClipboardSuccess)
	{
	    ClipboardUnlock( display, window, 0 );
	    _XmAppUnlock(app);
	    return (status);
	}

	itemheader->formatCount = itemheader->formatCount + 1;

	if ((itemheader->formatCount * 2 + 2) >= XM_ITEM_ID_INC) {
	    XmeWarning(NULL, XM_CLIPBOARD_MESSAGE3);
	    XtFree( (char *)itemheader);
	    ClipboardUnlock( display, window, 0 );
	    _XmAppUnlock(app);
	    return ClipboardFail;
	}

	formatlength = sizeof( ClipboardFormatItemRec );

	/* allocate local storage for the data format record */
	formatptr     = (ClipboardFormatItem)XtMalloc( (size_t) formatlength );

	formatid     = ClipboardGetNewItemId( display );
	formatdataid = ClipboardGetNewItemId( display );

	/* put format record id in data item's format id list */
	idptr = (itemId*)( (char *)itemheader + (itemlength - sizeof(itemId)) );

	*idptr = formatid;

	/* initialize the fields in the format record */
	formatptr->recordType = XM_FORMAT_HEADER_TYPE;
	formatptr->formatNameAtom = XInternAtom( display, format, False );
	formatptr->itemLength = 0;
	formatptr->formatNameLength = strlen( format );
	formatptr->formatDataId = formatdataid;
	formatptr->thisFormatId = formatid;
	formatptr->itemPrivateId = private_id;
	formatptr->cancelledFlag = 0;
	formatptr->copiedLength = 0;
	formatptr->parentItemId = itemid;
	formatptr->cutByNameWidget = itemheader->cutByNameWidget;
	formatptr->cutByNameWindow = itemheader->cutByNameWindow;
	formatptr->cutByNameCBIndex = itemheader->cutByNameCBIndex;
	formatptr->windowId = itemheader->windowId;
	formatptr->displayId = itemheader->displayId;

	/* if buffer is null then it is a pass by name */
	if ( buffer != 0 )
	{
	    formatptr->cutByNameFlag = 0;
	    formatdatalength = length;
	}else{
	    itemheader->cutByNameFlag = 1;
	    formatptr->cutByNameFlag = 1;
	    formatdatalength = sizeof(Atom);  /* we want a property stored regardless */
	}

	if( ClipboardGetLenFromFormat( display, format, &format_len )
			== ClipboardFail)
	{
	    /* if it's one of the predefined formats, register
	       it for second try */
	  XmClipboardRegisterFormat(display, format, 0);

	  ClipboardGetLenFromFormat( display, format, &format_len );
	}

	/* return the property on the root window for the item */
	ClipboardReplaceItem(display, itemid, (XtPointer)itemheader,
			     itemlength, PropModeReplace, 32,
			     True, XA_INTEGER );

	formatdataptr = XtMalloc( (size_t) formatdatalength );

        to_ptr = formatdataptr;
    }else{
    	formatid = formatptr->thisFormatId;
    	formatdataid = formatptr->formatDataId;

    	/* the format already existed so get the data and append */
    	ClipboardRetrieveItem(display,
			      formatdataid,
			      (int) length, /* BAD NEWS: truncation here.*/
			      0,
			      (XtPointer *) &formatdataptr,
			      &formatdatalength,
			      NULL,
			      &format_len,
			      0,
			      0 );

	to_ptr = (char *)formatdataptr + (formatdatalength - length);
    }

    if ( buffer != 0 )
    {
	/* copy the format data over to acquired storage */
	memcpy( to_ptr, buffer, (size_t) length );
    }

    formatptr->itemLength += length/((format_len==32)?CONVERT_32_FACTOR:1);

    /* replace the property on the root window for the format data */
    ClipboardReplaceItem(display,
			 formatdataid,
			 formatdataptr,
			 formatdatalength,
			 PropModeReplace,
			 format_len,  /* 8, 16, or 32 */
			 True,
			 type );

    /* replace the property on the root window for the format */
    ClipboardReplaceItem(display,
			 formatid,
			 (XtPointer)formatptr,
			 formatlength,
			 PropModeReplace,
			 32,
			 True,
			 XA_INTEGER);

    if ( dataid != 0 )
    {
        *dataid = formatid;
    }

    ClipboardClose( display, header );

    ClipboardUnlock( display, window, 0 );

    _XmAppUnlock(app);
    return ClipboardSuccess;
}

/*---------------------------------------------*/
int
XmClipboardEndCopy(
        Display *display,
        Window window,
        long itemid )
{
    ClipboardDataItem itemheader;
    ClipboardHeader header;
    itemId *itemlist;
    unsigned long itemlength ;
    long newitemoffset;
    itemId *newitemaddr;
    int status;
    Atom itemtype;
    _XmDisplayToAppContext(display);

    _XmAppLock(app);
    status = ClipboardLock( display, window );
    if ( status == ClipboardLocked ) {
	_XmAppUnlock(app);
	return ClipboardLocked;
    }

    /* get the clipboard header */
    header = ClipboardOpen( display, sizeof( itemId ) );

    if (!header->startCopyCalled) {
	XmeWarning(NULL, XM_CLIPBOARD_MESSAGE2);
        ClipboardUnlock( display, window, 0 );
	_XmAppUnlock(app);
        return ClipboardFail;
    }

    ClipboardDeleteMarked( display, window, header );

    if (header->currItems >= header->maxItems)
    {
	itemlist = (itemId*)((char *) header +
			     header->dataItemList * CONVERT_32_FACTOR);

    	/* mark least recent item for deletion and delete previously mark */
        ClipboardMarkItem( display, header, *itemlist, XM_DELETE );

    	header->deletedByCopyId = *itemlist;
    } else {
	header->deletedByCopyId = 0;
    }

    newitemoffset = header->dataItemList +
    		    (header->currItems * sizeof( itemId ) / CONVERT_32_FACTOR);

    /* stick new item at the bottom of the list */
    newitemaddr = (itemId*)((char *) header + newitemoffset * CONVERT_32_FACTOR);

    *newitemaddr = (itemId)itemid;

    /* new items always become next paste item */
    header->oldNextPasteItemId  = header->nextPasteItemId;
    header->nextPasteItemId = (itemId)itemid;
    header->lastCopyItemId = (itemId)itemid;

    header->currItems = header->currItems + 1;
    header->startCopyCalled = False;

    /* if there was a cut by name format, then set up event handling */
    ClipboardFindItem(display,
		      itemid,
		      (XtPointer *) &itemheader,
		      &itemlength,
		      &itemtype,
		      0,
		      XM_DATA_ITEM_RECORD_TYPE );

    if ( itemheader == 0 )
    {
	CleanupHeader (display);
	ClipboardError( CLIPBOARD_CORRUPT, CORRUPT_DATA_STRUCTURE );
    	return ClipboardFail;
    }

    if ( itemheader->cutByNameWindow != 0 )
    {
        EventMask event_mask = 0;

    	XtAddEventHandler( XtWindowToWidget(display,
					    itemheader->cutByNameWindow),
			  event_mask, TRUE,
			  ClipboardEventHandler, 0 );
    }

    XtFree( (char *) itemheader );

    AssertClipboardSelection(display, window, header,
			     header->selectionTimestamp );

    ClipboardSetNextItemId(display, itemid);

    ClipboardClose( display, header );

    ClipboardUnlock( display, window, 0 );

    _XmAppUnlock(app);
    return ClipboardSuccess;
}

/*---------------------------------------------*/
int
XmClipboardCancelCopy(
        Display *display,
        Window window,
        long itemid )   /* id returned by begin copy */
{
    itemId deleteitemid;
    itemId previous;
    XtPointer int_ptr;
    unsigned long length;
    ClipboardHeader header;
    Atom type;
    _XmDisplayToAppContext(display);

    _XmAppLock(app);
    if ( ClipboardLock( display, window) == ClipboardLocked ) {
	_XmAppUnlock(app);
        return(ClipboardLocked);
    }

    deleteitemid = (itemId)itemid;

    /* first, free up the properties set by the StartCopy and Copy calls */

    /* delete the item label */
    ClipboardDeleteItemLabel( display, window, deleteitemid);

    /* delete all the formats belonging to the data item */
    ClipboardDeleteFormats( display, window, deleteitemid );

    /* now delete the item itself */
    ClipboardDeleteId( display, deleteitemid );

    /*******************************************************************
     * reset the startCopyCalled flag and reset the XM_NEXT_ID property
     * it's value prior to StartCopy.
     *******************************************************************/
    ClipboardFindItem( display,
			XM_NEXT_ID,
			&int_ptr,
			&length,
			&type,
			0,
			0 );

    previous = itemid - 1;
    *(long *) int_ptr = previous;

    ClipboardReplaceItem( display,
			XM_NEXT_ID,
			int_ptr,
			sizeof(long),
			PropModeReplace,
			32,
			True,
			XA_INTEGER );

    header = ClipboardOpen( display, 0);
    header->startCopyCalled = False;
    ClipboardClose( display, header );

    ClipboardUnlock( display, window, 0 );

    _XmAppUnlock(app);
    return(ClipboardSuccess);
}

/*---------------------------------------------*/
int
XmClipboardWithdrawFormat(
        Display *display,
        Window window,
        long data )  /* data id of format no longer provided by application */
{
    int status;
    _XmDisplayToAppContext(display);

    _XmAppLock(app);
    status = ClipboardLock( display, window  );
    if ( status == ClipboardLocked ) {
	_XmAppUnlock(app);
	return ClipboardLocked;
    }

    ClipboardDeleteFormat( display, data );

    ClipboardUnlock( display, window, 0 );

    _XmAppUnlock(app);
    return ClipboardSuccess;
}

/*---------------------------------------------*/
int
XmClipboardCopyByName(
        Display *display,   /* Display id of application passing data */
        Window window,
        long data,          /* Data id returned previously by clipboard */
        XtPointer buffer,   /* Address of buffer holding data in this format */
        unsigned long length,   /* Length of the data */
        long private_id )    /* Private id provide by application */
{
    ClipboardFormatItem formatheader;
    int format;
    char *formatdataptr;
    unsigned long formatlength, formatdatalength;
    char *to_ptr;
    int status, locked;
    unsigned long headerlength;
    ClipboardHeader root_clipboard_header;
    Atom headertype, formattype;
    Atom type;
    _XmDisplayToAppContext(display);

    _XmAppLock(app);
    /* get the clipboard header */
    ClipboardFindItem( display,
			   XM_HEADER_ID,
			   (XtPointer *) &root_clipboard_header,
			   &headerlength,
			   &headertype,
			   0,
    			   0 );

    locked = 0;

    /* if this is a recopy as the result of a callback, then circumvent */
    /* any existing lock */
    if ( root_clipboard_header->recopyId != data )
    {
	status = ClipboardLock( display, window );
	if ( status == ClipboardLocked ){
	    _XmAppUnlock(app);
	    return ClipboardLocked;
	}
    	locked = 1;

    }else{
	root_clipboard_header->recopyId = 0;

	/* replace the clipboard header */
	ClipboardReplaceItem(display,
			     XM_HEADER_ID,
			     (XtPointer)root_clipboard_header,
			     headerlength,
			     PropModeReplace,
			     32,
			     False,
			     XA_INTEGER );
    }

    /* get a pointer to the format */
    if ( ClipboardFindItem(display,
			   data,
			   (XtPointer *) &formatheader,
			   &formatlength,
			   &formattype,
			   0,
			   XM_FORMAT_HEADER_TYPE ) == ClipboardSuccess )
    {
	formatheader->itemPrivateId = private_id;

    	ClipboardRetrieveItem(display,
			      formatheader->formatDataId,
			      (int) length,/* BAD NEWS: truncation here.*/
			      0,
			      (XtPointer *) &formatdataptr,
			      &formatdatalength,
			      &formattype,
			      &format,
			      0,
			      formatheader->cutByNameFlag );

	if (formatheader->cutByNameFlag)
	    formatheader->itemLength = length/((format==32)?CONVERT_32_FACTOR:1);
	else
	    formatheader->itemLength += length/((format==32)?CONVERT_32_FACTOR:1);

	/* if cut by name, discard any old data */
    	formatheader->cutByNameFlag = 0;

        to_ptr = (char *) formatdataptr + (formatdatalength - length);

	/* copy the format data over to acquired storage */
	memcpy( to_ptr, buffer, (size_t) length );

	/* If we've passed in a new type from UTM,  use it,  otherwise
	   we'll use the type on the clipboard (for compatibility with
	   old mechanism) */
	_XmProcessLock();
	if (_passed_type != None) {
	  type = _passed_type;
	  _passed_type = None;
	} else {
	  type = formattype;
	}
	_XmProcessUnlock();

	/* create the property on the root window for the format data */
	ClipboardReplaceItem(display,
			     formatheader->formatDataId,
			     formatdataptr,
			     formatdatalength,
			     PropModeReplace,
			     format,
			     True,
			     type );

	/* change the property on the root window for the format item */
	ClipboardReplaceItem(display,
			     data,
			     (XtPointer)formatheader,
			     formatlength,
			     PropModeReplace,
			     32,
			     True,
			     XA_INTEGER);
    }
    else {
	CleanupHeader (display);
	ClipboardError( CLIPBOARD_CORRUPT, CORRUPT_DATA_STRUCTURE );
    	return ClipboardFail;
    }

    if ( locked )
    {
        ClipboardUnlock( display, window, 0 );
    }

    XtFree( (char *) root_clipboard_header );

    _XmAppUnlock(app);
    return ClipboardSuccess;
}


/*---------------------------------------------*/
int
XmClipboardUndoCopy(
        Display *display,
        Window window )
{
    ClipboardHeader header;
    ClipboardDataItem itemheader;
    unsigned long itemlength;
    itemId itemid;
    int status, undo_okay;
    Atom itemtype;
    _XmDisplayToAppContext(display);

    _XmAppLock(app);
    status = ClipboardLock( display, window );
    if ( status == ClipboardLocked ) {
	_XmAppUnlock(app);
	return ClipboardLocked;
    }

    /* note: second call to undo, undoes the first call */

    /* get the clipboard header */
    header = ClipboardOpen( display, 0 );

    itemid = header->lastCopyItemId;

    undo_okay = 0;

    if ( itemid == 0 )
    {
    	undo_okay = 1;

    } else {
	/* get the item */
	ClipboardFindItem( display,
			       itemid,
			       (XtPointer *) &itemheader,
			       &itemlength,
			       &itemtype,
			       0,
    			       XM_DATA_ITEM_RECORD_TYPE );

	if ( itemheader == 0 )
	{
	   CleanupHeader (display);
	   ClipboardError( CLIPBOARD_CORRUPT, CORRUPT_DATA_STRUCTURE );
	   return ClipboardFail;
	}

	/* if no last copy item
	   or if the item's window or display don't match, then can't undo */
	if ( itemheader->windowId == window )
	{
    	    undo_okay = 1;

	    /* mark last item for delete
	    */
    	    ClipboardMarkItem( display, header, itemid, XM_DELETE );
    	}

    	XtFree( (char*)itemheader );
    }

    if ( undo_okay )
    {
	/* fetch the item marked deleted by the last copy, if any */
	itemid = header->deletedByCopyId;

	/* mark it undeleted */
	ClipboardMarkItem( display, header, itemid, XM_UNDELETE );

	/* switch item marked deleted */
	header->deletedByCopyId = header->lastCopyItemId;
	header->lastCopyItemId = itemid;

	/* switch next paste and old next paste */
	itemid = header->oldNextPasteItemId;
	header->oldNextPasteItemId = header->nextPasteItemId;
	header->nextPasteItemId = itemid;
    }

    ClipboardClose( display, header );

    ClipboardUnlock( display, window, 0 );

    _XmAppUnlock(app);
    return ClipboardSuccess;
}

/*---------------------------------------------*/
int
XmClipboardLock(
        Display *display,
        Window window )     /* identifies application owning lock */
{
    int ret_val;
    _XmDisplayToAppContext(display);
    _XmAppLock(app);
    ret_val = ClipboardLock( display, window );
    _XmAppUnlock(app);
    return ret_val;
}


/*---------------------------------------------*/
int
XmClipboardUnlock(
        Display *display,
        Window window,  /* specifies window owning lock, must match window */
#if NeedWidePrototypes  /* passed to clipboardlock */
        int all_levels )
#else
        Boolean all_levels )
#endif /* NeedWidePrototypes */
{
    int ret_val;
    _XmDisplayToAppContext(display);
    _XmAppLock(app);
    ret_val = ClipboardUnlock( display, window, all_levels );
    _XmAppUnlock(app);
    return ret_val;
}


/*---------------------------------------------*/
int
XmClipboardStartRetrieve(
        Display *display,       /* Display id of application wanting data */
        Window window,
        Time timestamp )
{
    ClipboardHeader header;
    int status;
    _XmDisplayToAppContext(display);

    _XmAppLock(app);
    status = ClipboardLock( display, window );
    if ( status == ClipboardLocked ) {
	_XmAppUnlock(app);
	return ClipboardLocked;
    }

    header = ClipboardOpen( display, 0 );

    header->incrementalCopyFrom = True;
    header->copyFromTimestamp = timestamp;
    header->foreignCopiedLength = 0;

    ClipboardClose( display, header );

    ClipboardUnlock( display, window, 0 );

    _XmAppUnlock(app);
    return ClipboardSuccess;
}

/*---------------------------------------------*/
int
XmClipboardEndRetrieve(
        Display *display,   /* Display id of application wanting data */
        Window window )
{
    ClipboardHeader header;
    _XmDisplayToAppContext(display);

    _XmAppLock(app);
    header = ClipboardOpen( display, 0 );

    header->incrementalCopyFrom = False;
    header->copyFromTimestamp   = CurrentTime;

    ClipboardClose( display, header );

    ClipboardUnlock( display, window, 0 );

    _XmAppUnlock(app);
    return ClipboardSuccess;
}

/*---------------------------------------------*/
int
XmClipboardRetrieve(
        Display *display,        /* Display id of application wanting data */
        Window window,
        char *format,   /* Name string for data format */
        XtPointer buffer, /* Address of buffer to receive data in this format */
        unsigned long length,    /* Length of the data buffer */
        unsigned long *outlength,/* Length of the data transferred to buffer */
        long *private_id )   /* Private id provide by application */
{
  Atom ignoretype;
  int  ret_val;
  _XmDisplayToAppContext(display);

  _XmAppLock(app);
  ret_val = ClipboardRetrieve(display, window, format, buffer,
		   length, outlength, private_id, &ignoretype);
  _XmAppUnlock(app);
  return ret_val;
}

static int
ClipboardRetrieve(Display *display, Window window,
		  char *format, XtPointer buffer,
		  unsigned long length,
		  unsigned long *outlength,
		  long *private_id,
		  Atom *outtype)
{
    ClipboardHeader header;
    ClipboardFormatItem matchformat;
    char *formatdata;
    unsigned long formatdatalength ;
    unsigned long matchformatlength;
    Atom matchformattype;
    int truncate, count;
    unsigned long maxname ;
    itemId matchid;
    char *ptr;
    int status, dataok, outformatsize;
    unsigned long loc_outlength;
    itemId loc_private;
    unsigned long copiedlength, remaininglength;
    Time timestamp;

    status = ClipboardLock( display, window );
    if ( status == ClipboardLocked ) return ClipboardLocked;

    /* get the clipboard header */
    header = ClipboardOpen( display, 0 );
    timestamp = header->copyFromTimestamp;

    loc_outlength = 0;
    loc_private = 0;
    truncate = 0;
    dataok = 0;
    ptr = NULL;

    /* check to see if we need to reclaim the selection */
    InitializeSelection( display, header, window, timestamp );

    /* get the data from clipboard or selection owner */
    if ( WeOwnSelection( display, header ) )
    {
	/* we own the selection */

	/* find the matching format for the next paste item */
	matchformat = ClipboardFindFormat(display, header, format,
					  (itemId) NULL, 0,
					  &maxname, &count,
					  &matchformatlength );
	if (matchformat != 0)
	  {
	    dataok = 1;

	    matchid = matchformat->thisFormatId;

	    if ( matchformat->cutByNameFlag == 1 )
	    {
		/* passed by name */

 	        /* Note:  This code is now partially copied into
		   another function,  ClipboardGetByNameItem,  so
		   updates in this section should be reflected there */

		dataok = ClipboardRequestDataAndWait(display,
						     window,
						     matchformat);
		if ( dataok )
		{
		  /* re-check out matchformat since it may have changed */
		  XtFree( (char *) matchformat );

		  ClipboardFindItem(display,
				    matchid,
				    (XtPointer *) &matchformat,
				    &matchformatlength,
				    &matchformattype,
				    0,
				    XM_FORMAT_HEADER_TYPE );

		  if ( matchformat == 0 )
		  {
 		     CleanupHeader (display);
		     ClipboardError( CLIPBOARD_CORRUPT,
				     CORRUPT_DATA_STRUCTURE );
		     return ClipboardFail;
		   }
		}
	    }

	    if ( dataok )
	    {
	        ClipboardFindItem(display,
				  matchformat->formatDataId,
				  (XtPointer *) &formatdata,
				  &formatdatalength,
				  outtype,
				  0,
				  0 );

		if ( formatdata == 0 )
		{
 		   CleanupHeader (display);
		   ClipboardError( CLIPBOARD_CORRUPT, CORRUPT_DATA_STRUCTURE );
		   return ClipboardFail;
		}

		copiedlength = matchformat->copiedLength;
		ptr = formatdata + copiedlength;

		remaininglength = formatdatalength - copiedlength;

		if ( length < remaininglength )
		{
		    loc_outlength = length;
		    truncate = 1;
		}else{
		    loc_outlength = remaininglength;
		}

		if ( header->incrementalCopyFrom )
		{
		    /* update the copied length */
		    if ( loc_outlength == remaininglength )
		    {
			/* we've copied everything, so reset */
			matchformat->copiedLength = 0;
		    }else{
			matchformat->copiedLength = matchformat->copiedLength
						  + loc_outlength;
		    }
		}

		loc_private = matchformat->itemPrivateId;
	    }

	    ClipboardReplaceItem(display,
				 matchid,
				 (XtPointer)matchformat,
				 matchformatlength,
				 PropModeReplace,
				 32,
				 True,
				 XA_INTEGER );
	}


	/* Unfortunately the clipboard routines don't have a type
	   interface.  Go ask for a type if type is bogus */
	if (*outtype == None)
	  *outtype = GetTypeFromTarget(display,
				       XInternAtom(display, format, False));
    }else{
	/* we don't own the selection, get the data from selection owner */
        if ( ClipboardGetSelection(display, window,
				   XInternAtom(display, format, False),
				   (XtPointer *) &formatdata,
				   outtype, &loc_outlength,
				   &outformatsize ) )
	{
	    /* we're okay */
		dataok = 1;

		/* copiedlength = header->foreignCopiedLength; */
		copiedlength = 0; /* No incr. for now.  Fix after beta */

		ptr = formatdata + copiedlength;

		remaininglength = loc_outlength - copiedlength;

		if ( length < remaininglength )
		{
		    loc_outlength = length;
		    truncate = 1;
		}else{
		    loc_outlength = remaininglength;
		}

		if ( header->incrementalCopyFrom )
		{
		    /* update the copied length */
		    if ( loc_outlength == remaininglength )
		    {
			/* we've copied everything, so reset */
			header->foreignCopiedLength = 0;
		    }else{
			header->foreignCopiedLength =
						header->foreignCopiedLength
						  + loc_outlength;
		    }
		}
	    }
    }

    if ( dataok )
    {
	/* copy the data to the user buffer */
	memcpy( buffer, ptr, (size_t) loc_outlength );

	XtFree( (char *) formatdata );
    }

    /* try to prevent access violation even if outlength is mandatory */
    if ( outlength != 0 )
    {
    	*outlength = loc_outlength;
    }

    if ( private_id != 0 )
    {
	*private_id = loc_private;
    }


    ClipboardClose( display, header );
    ClipboardUnlock( display, window, 0 );

    if (truncate == 1) return ClipboardTruncate;
    if (dataok == 0)   return ClipboardNoData;

    return ClipboardSuccess;
}

/*---------------------------------------------*/
int
XmClipboardInquireCount(
        Display *display,
        Window window,
        int *count,     /* receives number of formats in next paste item */
        unsigned long *maxlength )/* receives max length of format names */
{
    ClipboardHeader header;
    char *alloc_to_free;
    unsigned long loc_maxlength, loc_matchlength;
    unsigned long loc_count_len ;
    int status;
    int loc_count ;
    Atom ignoretype;
    int ignoreformat;
    Time timestamp;
    _XmDisplayToAppContext(display);

    _XmAppLock(app);
    status = ClipboardLock( display, window );
    if ( status == ClipboardLocked ) {
	_XmAppUnlock(app);
	return ClipboardLocked;
    }

    /* get the clipboard header */
    header = ClipboardOpen( display, 0 );

    /* If StartRetrieve wasn't called use latest Timestamp from server */

    if ( header->copyFromTimestamp == CurrentTime )
    {
        timestamp = ClipboardGetCurrentTime(display);
    }else{
        timestamp = header->copyFromTimestamp;
    }

    /* check to see if we need to reclaim the selection */
    InitializeSelection( display, header, window, timestamp );
    loc_maxlength = 0;
    loc_count = 0;

    /* do we own the selection? */
    if ( WeOwnSelection( display, header ) )
    {
	/* yes, find the next paste item, only looking for
	   maxlength and count */
	alloc_to_free = (char*)ClipboardFindFormat( display, header, 0,
							(itemId) NULL, 0,
							&loc_maxlength,
							&loc_count,
							&loc_matchlength );
    }else{
	/* we don't own the selection, get the data from selection owner */
        if ( !ClipboardGetSelection(display, window,
				    XInternAtom(display, XmSTARGETS, False),
				    (XtPointer *) &alloc_to_free,
				    &ignoretype,
				    &loc_count_len,
				    &ignoreformat ) )
        {
	    _XmAppUnlock(app);
            return ClipboardNoData;

        }else{
            /* we obtained a TARGETS type selection conversion */
            Atom *atomptr;
            int i;

            atomptr   = (Atom*)alloc_to_free;

            /* returned count is in bytes, targets are atoms of
	       length sizeof(long) */
            loc_count = (int) loc_count_len / sizeof(Atom);

	    /* max the lengths of all the atom names */
	    for( i = 0; i < loc_count; i++ )
	    {
		    int temp;

		    if ((*atomptr) != (Atom)0)
		    {
		        char *str;

			str =  XGetAtomName( display, *atomptr );
			temp = strlen(str);
			XFree(str);

			if ( temp > loc_maxlength)
			{
			    loc_maxlength = temp;
			}
		    }
		    atomptr++;
	     }
	}
    }

    if ( maxlength != 0 )
    {
	/* user asked for max length of available format names */
	*maxlength = loc_maxlength;
    }

    if ( count != 0 )
    {
	*count = loc_count;
    }

    if ( alloc_to_free != 0 )
    {
	XtFree( (char *) alloc_to_free );
    }

    ClipboardClose( display, header );
    ClipboardUnlock( display, window, 0 );

    _XmAppUnlock(app);
    return ClipboardSuccess;
}


/*---------------------------------------------*/
int
XmClipboardInquireFormat(
        Display *display,       /* Display id of application inquiring */
        Window window,
        int n,                  /* Which format for this data item? */
        XtPointer buffer,       /* Address of buffer to receive format name */
        unsigned long bufferlength, /* Length of the buffer */
        unsigned long *outlength )  /* Receives length copied to name buffer */
{
    ClipboardHeader header;
    ClipboardFormatItem matchformat;
    char *alloc_to_free;
    char *ptr;
    int count;
    unsigned long loc_matchlength, maxname, loc_outlength ;
    int status;
    Atom ignoretype;
    int ignoreformat;
    _XmDisplayToAppContext(display);

    _XmAppLock(app);
    status = ClipboardLock( display, window );
    if ( status == ClipboardLocked ) {
	_XmAppUnlock(app);
	return ClipboardLocked;
    }

    status = ClipboardSuccess;

    /* get the clipboard header */
    header = ClipboardOpen( display, 0 );

    /* check to see if we need to reclaim the selection */
    InitializeSelection( display, header, window,
			     header->copyFromTimestamp );

    ptr = NULL;
    loc_outlength = 0;

    /* do we own the selection? */
    if ( WeOwnSelection( display, header ) )
    {
	/* retrieve the matching format */
	matchformat = ClipboardFindFormat(display, header, 0,
					     (itemId) NULL, n,
					     &maxname, &count,
					     &loc_matchlength );

	if ( matchformat != 0 ) {
	  ptr = XGetAtomName( display, matchformat->formatNameAtom );
	  XtFree( (char *) matchformat );
	} else {
	  status = ClipboardNoData;
	}
    }else{
        /* we don't own the selection, get the data from selection owner */
        if ( !ClipboardGetSelection(display, window,
				    XInternAtom(display, XmSTARGETS, False),
				    (XtPointer *) &alloc_to_free,
				    &ignoretype,
				    &loc_matchlength, &ignoreformat ) )
        {
            *outlength = 0;
	    _XmAppUnlock(app);
            return ClipboardNoData;

        } else {
            /* we obtained a TARGETS type selection conversion */
            Atom *nth_atom;

            nth_atom = (Atom*)alloc_to_free;

            /* returned count is in bytes, targets are atoms
	       of length sizeof(long) */
            loc_matchlength = loc_matchlength / sizeof(Atom);

            if ( loc_matchlength >= n )
            {
                nth_atom = nth_atom + n - 1;

                ptr = XGetAtomName( display, *nth_atom );

                XtFree( (char *) alloc_to_free );
            }
	}
    }

    if ( ptr != 0 )
    {
	loc_outlength = strlen( ptr );

	if ( loc_outlength > bufferlength )
	{
	    status = ClipboardTruncate;

	    loc_outlength = bufferlength;
	}
	strncpy( (char *) buffer, ptr, (unsigned) loc_outlength );
	/* loc_outlenght is truncated above.*/

	XtFree( (char *) ptr );
    }

    if ( outlength != 0 )
    {
    	*outlength = loc_outlength;
    }

    ClipboardClose( display, header );
    ClipboardUnlock( display, window, 0 );

    _XmAppUnlock(app);
    return status;
}


/*---------------------------------------------*/
int
XmClipboardInquireLength(
        Display *display,   /* Display id of application inquiring */
        Window window,
        char *format,       /* Name string for data format */
        unsigned long *length )/* Receives length of the data in that format */
{

    ClipboardHeader header;
    ClipboardFormatItem matchformat;
    char *alloc_to_free;
    int count ;
    unsigned long loc_length, maxname, loc_matchlength;
    Atom ignoretype;
    int ignoreformat;
    int status;
    int format_len;
    _XmDisplayToAppContext(display);

    _XmAppLock(app);
    status = ClipboardLock( display, window );
    if ( status == ClipboardLocked ) {
	_XmAppUnlock(app);
	return ClipboardLocked;
    }

    /* get the clipboard header */
    header = ClipboardOpen( display, 0 );

    /* check to see if we need to reclaim the selection */
    InitializeSelection( display, header, window,
			     header->copyFromTimestamp );

    loc_length = 0;

    /* do we own the selection? */
    if ( WeOwnSelection( display, header ) )
    {
	/* retrieve the next paste item */
	matchformat = ClipboardFindFormat(display, header, format,
					     (itemId) NULL, 0, &maxname,
					     &count, &loc_matchlength);

	/* return the length */
	if ( matchformat != 0 )
	{
	    if ( CONVERT_32_FACTOR != 1 )
	      {
		  ClipboardGetLenFromFormat( display,
					    format,
					    &format_len );
		  loc_length = matchformat->itemLength *
		    ((format_len==32)?CONVERT_32_FACTOR:1);
	      }
	    else
	      {
		  loc_length = matchformat->itemLength;
	      }

	    XtFree( (char *) matchformat );
	} else {
	  status = ClipboardNoData;
	}
    }else{
        /* we don't own the selection, get the data from selection owner */
        if ( !ClipboardGetSelection(display, window,
				    XInternAtom(display, format, False),
				    (XtPointer *) &alloc_to_free,
				    &ignoretype, &loc_length, &ignoreformat ) )
        {
	    _XmAppUnlock(app);
            return ClipboardNoData;

        }else{

            XtFree( (char *) alloc_to_free );
	}
    }

    if ( length != 0 )
    {
    	*length = loc_length;
    }

    ClipboardClose( display, header );
    ClipboardUnlock( display, window, 0 );

    _XmAppUnlock(app);
    return status;
}


/*---------------------------------------------*/
int
XmClipboardInquirePendingItems(
        Display *display,       /* Display id of application passing data */
        Window window,
        char *format,           /* Name string for data format */
        XmClipboardPendingList *list,
        unsigned long *count )  /* Number of items in returned list */
{
    ClipboardHeader header;
    ClipboardFormatItem matchformat;
    XmClipboardPendingList itemlist, nextlistptr;
    itemId *id_ptr;
    int loc_count, i;
    unsigned long maxname, loc_matchlength;
    int status;
    _XmDisplayToAppContext(display);

    _XmAppLock(app);
    status = ClipboardLock( display, window );
    if ( status == ClipboardLocked ) {
	_XmAppUnlock(app);
	return ClipboardLocked;
    }

    if ( list == 0 )
    {
    	/* just get out to avoid access violation */
	ClipboardUnlock( display, window, 0 );
	_XmAppUnlock(app);
        return ClipboardSuccess;
    }

    *list = 0;
    loc_count = 0;

    /* get the clipboard header */
    header = ClipboardOpen( display, 0 );

    id_ptr = (itemId*)((char *) header +
		       header->dataItemList * CONVERT_32_FACTOR);

    itemlist = (XmClipboardPendingList)XtMalloc( (size_t)
    			(header->currItems * sizeof( XmClipboardPendingRec)));

    nextlistptr = itemlist;

    /* run through all the items in the clipboard looking
       for matching formats */
    for ( i = 0; i < header->currItems; i++ )
    {
    	/* if it is marked for delete, skip it */
    	if ( ClipboardIsMarkedForDelete( display, header, *id_ptr ) )
    	{
    	    matchformat = 0;
    	}else{
    	    int dummy;

	    /* see if there is a matching format */
	    matchformat = ClipboardFindFormat( display, header,
						   format, *id_ptr,
						   0, &maxname, &dummy,
    						   &loc_matchlength );
    	}

    	if ( matchformat != 0 )
    	{
    	    /* found matching format */
    	    if ( matchformat->cutByNameFlag == 1 )
    	    {
    		/* it was passed by name so is pending */
    	    	nextlistptr->DataId = matchformat->thisFormatId;
    	    	nextlistptr->PrivateId = matchformat->itemPrivateId;

    		nextlistptr = nextlistptr + 1;
    		loc_count = loc_count + 1;
    	    }

    	    XtFree( (char *) matchformat );
    	}

    	id_ptr = id_ptr + 1;
    }

    ClipboardClose( display, header );

    ClipboardUnlock( display, window, 0 );

    if ( count != 0 )
    {
        *count = loc_count;
    }

    *list  = itemlist;

    _XmAppUnlock(app);
    return status;
}


/*---------------------------------------------*/
int
XmClipboardRegisterFormat(
        Display *display,       /* Display id of application passing data */
        char *format_name,      /* Name string for data format            */
        int format_length )     /* Format length  8-16-32         */
{
    int ret_val;
    _XmDisplayToAppContext(display);

    _XmAppLock(app);
    if ( format_length != 0 &&
	 format_length != 8 && format_length != 16 && format_length != 32 )
    {
    	XmeWarning ( NULL, BAD_FORMAT );
	_XmAppUnlock(app);
	return ClipboardBadFormat;
    }

    if ( format_name == 0  ||  strlen( format_name ) == 0 )
    {
    	XmeWarning ( NULL, BAD_FORMAT_NON_NULL );
    } else {

        /* make sure predefined formats are registered */
        /* use dummy format as a test, if not found then register the rest */
        if ( format_length != 0 )
        {
	    ret_val = RegisterFormat( display, format_name, format_length );
	    _XmAppUnlock(app);
	    return ret_val;
        } else {
          /* caller asking to look through predefines for format name */
          if (
	    RegIfMatch(display, format_name, XmSTARGETS, XM_ATOM) ||
	    RegIfMatch(display, format_name, XmSMULTIPLE, XM_ATOM_PAIR) ||
	    RegIfMatch(display, format_name, XmSTIMESTAMP, XM_INTEGER) ||
	    RegIfMatch(display, format_name, "STRING", XM_STRING) ||
            RegIfMatch(display, format_name, XmSCOMPOUND_TEXT, XM_COMPOUND_TEXT) ||
	    RegIfMatch(display, format_name, XmILIST_LENGTH, XM_INTEGER) ||
	    RegIfMatch(display, format_name, "PIXMAP", XM_DRAWABLE) ||
	    RegIfMatch(display, format_name, "DRAWABLE", XM_DRAWABLE) ||
	    RegIfMatch(display, format_name, "BITMAP", XM_BITMAP) ||
	    RegIfMatch(display, format_name, XmIFOREGROUND, XM_PIXEL) ||
	    RegIfMatch(display, format_name, XmIBACKGROUND, XM_PIXEL) ||
	    RegIfMatch(display, format_name, "COLORMAP", XM_COLORMAP) ||
	    RegIfMatch(display, format_name, XmIODIF, XM_TEXT) ||
	    RegIfMatch(display, format_name, XmIOWNER_OS, XM_TEXT) ||
	    RegIfMatch(display, format_name, XmSFILE_NAME, XM_TEXT) ||
	    RegIfMatch(display, format_name, XmIHOST_NAME, XM_TEXT) ||
	    RegIfMatch(display, format_name, XmICHARACTER_POSITION, XM_SPAN) ||
	    RegIfMatch(display, format_name, XmILINE_NUMBER, XM_SPAN) ||
	    RegIfMatch(display, format_name, XmICOLUMN_NUMBER, XM_SPAN) ||
	    RegIfMatch(display, format_name, XmSLENGTH, XM_INTEGER) ||
	    RegIfMatch(display, format_name, XmIUSER, XM_TEXT) ||
	    RegIfMatch(display, format_name, XmIPROCEDURE, XM_TEXT) ||
	    RegIfMatch(display, format_name, XmIMODULE, XM_TEXT) ||
	    RegIfMatch(display, format_name, XmIPROCESS, XM_INTEGER) ||
	    RegIfMatch(display, format_name, XmITASK, XM_INTEGER) ||
	    RegIfMatch(display, format_name, XmICLASS, XM_TEXT) ||
	    RegIfMatch(display, format_name, XmINAME, XM_TEXT) ||
	    RegIfMatch(display, format_name, XmSCLIENT_WINDOW, XM_WINDOW)
#if XM_UTF8
	    || RegIfMatch(display, format_name, XmSUTF8_STRING, XM_UTF8_STRING)
#endif
	     )  {
	        _XmAppUnlock(app);
	        return ClipboardSuccess;
	        }
        }
    }

    _XmAppUnlock(app);
    return ClipboardFail;
}

/*********************************************************************
 * GetTypeFromTarget
 *
 * This function makes an attempt to get a suitable type for a
 * particular target.  This isn't perfect,  but much better than
 * the old clipboard method which simply used the target as the
 * type.  Most requestors won't care,  and they definitely won't
 * via the clipboard routines,  as these have no type parameter.
 * But UTM and other ICCCM clients care
 *********************************************************************/

static Atom
GetTypeFromTarget(Display *display, Atom target)
{
  enum {
    XmAATOM_PAIR, XmABACKGROUND, XmACHARACTER_POSITION, XmACLASS,
    XmACOLUMN_NUMBER, XmAFOREGROUND, XmAHOST_NAME, XmALINE_NUMBER,
    XmALIST_LENGTH, XmAMODULE, XmANAME, XmANone, XmAODIF, XmAOWNER_OS,
    XmAPIXEL, XmAPROCEDURE, XmAPROCESS, XmASPAN, XmATASK, XmAUSER,
    XmACLIENT_WINDOW, XmADELETE, XmAFILE, XmAFILE_NAME,
    XmAINSERT_PROPERTY, XmAINSERT_SELECTION, XmALENGTH,
    XmALINK_SELECTION, XmAMULTIPLE, XmATARGETS, XmATEXT, XmATIMESTAMP,
    XmA_MOTIF_CLIPBOARD_TARGETS, XmA_MOTIF_DEFERRED_CLIPBOARD_TARGETS,
    XmA_MOTIF_ENCODING_REGISTRY, XmA_MOTIF_EXPORT_TARGETS,
    XmA_MOTIF_RENDER_TABLE,
#if XM_UTF8
    XmAUTF8_STRING,
#endif
    NUM_ATOMS };
  static char *atom_names[] = {
    XmIATOM_PAIR, XmIBACKGROUND, XmICHARACTER_POSITION, XmICLASS,
    XmICOLUMN_NUMBER, XmIFOREGROUND, XmIHOST_NAME, XmILINE_NUMBER,
    XmILIST_LENGTH, XmIMODULE, XmINAME, XmINone, XmIODIF, XmIOWNER_OS,
    XmIPIXEL, XmIPROCEDURE, XmIPROCESS, XmISPAN, XmITASK, XmIUSER,
    XmSCLIENT_WINDOW, XmSDELETE, XmSFILE, XmSFILE_NAME,
    XmSINSERT_PROPERTY, XmSINSERT_SELECTION, XmSLENGTH,
    XmSLINK_SELECTION, XmSMULTIPLE, XmSTARGETS, XmSTEXT, XmSTIMESTAMP,
    XmS_MOTIF_CLIPBOARD_TARGETS, XmS_MOTIF_DEFERRED_CLIPBOARD_TARGETS,
    XmS_MOTIF_ENCODING_REGISTRY, XmS_MOTIF_EXPORT_TARGETS,
    XmS_MOTIF_RENDER_TABLE,
#if XM_UTF8
    XmSUTF8_STRING
#endif
    };
  Atom atoms[XtNumber(atom_names)];

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(display, atom_names, XtNumber(atom_names), False, atoms);

  if (target == atoms[XmATARGETS] ||
      target == atoms[XmA_MOTIF_EXPORT_TARGETS] ||
      target == atoms[XmA_MOTIF_CLIPBOARD_TARGETS] ||
      target == atoms[XmA_MOTIF_DEFERRED_CLIPBOARD_TARGETS])
    return(XA_ATOM);
  if (target == atoms[XmAMULTIPLE])
    return(atoms[XmAATOM_PAIR]);
  if (target == atoms[XmATIMESTAMP] ||
      target == atoms[XmALIST_LENGTH] ||
      target == atoms[XmAPROCESS] ||
      target == atoms[XmATASK] ||
      target == atoms[XmALENGTH])
    return(XA_INTEGER);
  if (target == atoms[XmATEXT] ||
      target == atoms[XmAODIF] ||
      target == atoms[XmAOWNER_OS] ||
      target == atoms[XmAFILE_NAME] ||
      target == atoms[XmAFILE] ||
      target == atoms[XmAHOST_NAME] ||
      target == atoms[XmAUSER] ||
      target == atoms[XmAPROCEDURE] ||
      target == atoms[XmAMODULE] ||
      target == atoms[XmACLASS] ||
      target == atoms[XmANAME]) {
    int ret_status = 0;
    XTextProperty tmp_prop;
    char * tmp_string = "ABC";  /* these are characters in XPCS, so... safe */
    Atom encoding;

    tmp_prop.value = NULL;
    ret_status = XmbTextListToTextProperty(display, &tmp_string, 1,
					   (XICCEncodingStyle)XTextStyle,
					   &tmp_prop);
    if (ret_status == Success)
      encoding = tmp_prop.encoding;
    else
      encoding = None;        /* XmbTextList... should always be able
			       * to convert XPCS characters; but in
			       * case its broken, this prevents a core
			       * dump.
			       */
    if (tmp_prop.value != NULL) XFree((char *)tmp_prop.value);
    return(encoding);
  }
  if (target == XA_PIXMAP) return(XA_DRAWABLE);
  if (target == atoms[XmAFOREGROUND] ||
      target == atoms[XmABACKGROUND])
    return(atoms[XmAPIXEL]);
  if (target == atoms[XmACHARACTER_POSITION] ||
      target == atoms[XmALINE_NUMBER] ||
      target == atoms[XmACOLUMN_NUMBER])
    return(atoms[XmASPAN]);
  if (target == atoms[XmACLIENT_WINDOW])
    return(XA_WINDOW);
  if (target == atoms[XmADELETE] ||
      target == atoms[XmAINSERT_SELECTION] ||
      target == atoms[XmALINK_SELECTION] ||
      target == atoms[XmAINSERT_PROPERTY])
    return(atoms[XmANone]);
  if (target == atoms[XmA_MOTIF_ENCODING_REGISTRY] ||
      target == atoms[XmA_MOTIF_RENDER_TABLE])
    return(XA_STRING);

  return(target); /* by default */
}
