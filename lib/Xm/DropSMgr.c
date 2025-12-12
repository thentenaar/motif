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
static char rcsid[] = "$TOG: DropSMgr.c /main/21 1999/08/11 14:44:57 vipin $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


/*****************************************************************************
 * THE DROPSITE DATABASE
 *
 * Drag and Drop maintains a two way mapping between information records
 * and the dropsites widgets they represent.  There are two kinds of records
 * kept in the database.
 *
 * The first kind of records are associated with a real dropsite.  These
 * contain the information the user passes in via XmDropSiteRegister and
 * modifies via XmDropSiteUpdate.
 *
 * The second kind are clipping records which represent widgets which
 * in some way obscure one or more dropsites.  These are created and destroyed
 * by Drag and Drop as dynamic things (like a ScrolledWindow scrolling)
 * occur.
 *
 * When a new record is created,  it is associated via a hashtable kept
 * in dsm -> dstable to the widget it represents.  This mapping is maintained
 * by the RegisterInfo and UnregisterInfo functions.  Additionally,  the
 * records themselves are kept internally in a compacted form.  This
 * translation occurs in CopyVariantIntoFull and CopyFullIntoVariant.
 *
 * While confusing,  the compression saves significant room.  The fields
 * are accessed by macros (defined in DropSMgrI.h) to avoid the problems
 * of constantly compressing and decompressing the records.
 *
 * WHEN A CHANGE OCCURS
 *
 * When a new DS is added, one is removed,  or a geometry change occurs,
 * the database must be brought up to sync with the real widgets.
 * This is done in a number of stages.
 *
 * RemoveAllClippers(dsm, root) - This removes all the clippers created
 * in the database hierarchy.  The dropsites become a flat list held in
 * the child list of the topmost node.
 *
 * SyncDropSiteGeometry(dsm, root) - Now we go down the flat list and
 * see what updates are needed to the internal geometry information.  This
 * is really needed for PREREGISTER,  as we'll finalize the update by
 * refreshing the information kept on the window properties.
 *
 * DetectAndInsertAllClippers(dsm, root) - We now rebuild the clipper
 * hierarchy.
 *****************************************************************************/

#include <Xm/GadgetP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/ManagerP.h>
#include <Xm/DragC.h>
#include <Xm/DropTrans.h>
#include <Xm/XmosP.h>		/* for bzero */
#include "XmI.h"
#include "DisplayI.h"
#include "DragBSI.h"
#include "DragCI.h"
#include "DragICCI.h"
#include "DragUnderI.h"
#include "DropSMgrI.h"
#include "HashI.h"
#include "MessagesI.h"
#include "PixConvI.h"
#include "RegionI.h"
#include "TraversalI.h"		/* for _XmIntersectionOf() */

#define MESSAGE1 _XmMMsgDropSMgr_0001
#define MESSAGE2 _XmMMsgDropSMgr_0002
#define MESSAGE3 _XmMMsgDropSMgr_0003
#define MESSAGE4 _XmMMsgDropSMgr_0004
#define MESSAGE5 _XmMMsgDropSMgr_0005
#define MESSAGE6 _XmMMsgDropSMgr_0006
#define MESSAGE7 _XmMMsgDropSMgr_0007
#define MESSAGE8 _XmMMsgDropSMgr_0008
#define MESSAGE9 _XmMMsgDropSMgr_0009
#define MESSAGE10 _XmMMsgDropSMgr_0010

/* #define DEBUG */
#ifdef DEBUG
#define DPRINT(x) printf x
void _XmPrintDSTree(XmDropSiteManagerObject dsm, XmDSInfo root);
#else
#define DPRINT(x)
#endif

/********    Static Function Declarations    ********/

static void ClassInit( void ) ;
static void ClassPartInit(
                        WidgetClass wc) ;
static void DropSiteManagerInitialize(
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void Destroy(
                        Widget w) ;
static Boolean SetValues(
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void CreateTable(
                        XmDropSiteManagerObject dsm) ;
static void DestroyTable(
                        XmDropSiteManagerObject dsm) ;
static void RegisterInfo(
                        register XmDropSiteManagerObject dsm,
                        register Widget widget,
                        register XtPointer info) ;
static void UnregisterInfo(
                        register XmDropSiteManagerObject dsm,
                        register XtPointer info) ;
static XtPointer WidgetToInfo(
                        register XmDropSiteManagerObject dsm,
                        register Widget widget) ;
static Boolean Coincident(
                        XmDropSiteManagerObject dsm,
                        Widget w,
                        XmDSClipRect *r) ;
static Boolean IsDescendent(
                        Widget parentW,
                        Widget childW) ;
static void DetectAncestorClippers(
                        XmDropSiteManagerObject dsm,
                        Widget w,
                        XmDSClipRect *r,
                        XmDSInfo info) ;
static void DetectImpliedClipper(
                        XmDropSiteManagerObject dsm,
                        XmDSInfo info) ;
static void DetectAllClippers(
                        XmDropSiteManagerObject dsm,
                        XmDSInfo parentInfo) ;
static Boolean InsertClipper(
                        XmDropSiteManagerObject dsm,
                        XmDSInfo parentInfo,
                        XmDSInfo clipper) ;
static void DetectAndInsertAllClippers(
                        XmDropSiteManagerObject dsm,
                        XmDSInfo root) ;
static void RemoveClipper(
                        XmDropSiteManagerObject dsm,
                        XmDSInfo clipper) ;
static void RemoveAllClippers(
                        XmDropSiteManagerObject dsm,
                        XmDSInfo parentInfo) ;
static void DestroyDSInfo(
                        XmDSInfo info,
                        Boolean substructures) ;
static XmDSInfo CreateShellDSInfo(
                        XmDropSiteManagerObject dsm,
                        Widget widget) ;
static XmDSInfo CreateClipperDSInfo(
                        XmDropSiteManagerObject dsm,
                        Widget clipW) ;
static void InsertInfo(
                        XmDropSiteManagerObject dsm,
                        XtPointer info,
                        XtPointer call_data) ;
static void RemoveInfo(
                        XmDropSiteManagerObject dsm,
                        XtPointer info) ;
static Boolean IntersectWithWidgetAncestors(
                        Widget w,
                        XmRegion r) ;
static Boolean IntersectWithDSInfoAncestors(
                        XmDSInfo parent,
                        XmRegion r) ;
static Boolean CalculateAncestorClip(
                        XmDropSiteManagerObject dsm,
                        XmDSInfo info,
                        XmRegion r) ;
static Boolean PointInDS(
                        XmDropSiteManagerObject dsm,
                        XmDSInfo info,
                        Position x,
                        Position y) ;
static XmDSInfo PointToDSInfo(
                        XmDropSiteManagerObject dsm,
                        XmDSInfo info,
                        Position x,
                        Position y) ;
static void DoAnimation(
                        XmDropSiteManagerObject dsm,
                        XmDragMotionClientData motionData,
                        XtPointer callback) ;
static void ProxyDragProc(
                        XmDropSiteManagerObject dsm,
                        XtPointer client_data,
                        XmDragProcCallbackStruct *callback) ;
static void HandleEnter(
                        XmDropSiteManagerObject dsm,
                        XmDragMotionClientData motionData,
                        XmDragMotionCallbackStruct *callback,
                        XmDSInfo info,
                        unsigned char style) ;
static void HandleMotion(
                        XmDropSiteManagerObject dsm,
                        XmDragMotionClientData motionData,
                        XmDragMotionCallbackStruct *callback,
                        XmDSInfo info,
                        unsigned char style) ;
static void HandleLeave(
                        XmDropSiteManagerObject dsm,
                        XmDragMotionClientData motionData,
                        XmDragMotionCallbackStruct *callback,
                        XmDSInfo info,
                        unsigned char style,
                        Boolean enterPending) ;
static void ProcessMotion(
                        XmDropSiteManagerObject dsm,
                        XtPointer clientData,
                        XtPointer calldata) ;
static void ProcessDrop(
                        XmDropSiteManagerObject dsm,
                        XtPointer clientData,
                        XtPointer cb) ;
static void ChangeOperation(
                        XmDropSiteManagerObject dsm,
                        XtPointer clientData,
                        XtPointer calldata) ;
static void PutDSToStream(
                        XmDropSiteManagerObject dsm,
                        XmDSInfo dsInfo,
                        Boolean last,
                        XtPointer dataPtr) ;
static void GetDSFromDSM(
                        XmDropSiteManagerObject dsm,
                        XmDSInfo parentInfo,
                        Boolean last,
                        XtPointer dataPtr) ;
static int GetTreeFromDSM(
                        XmDropSiteManagerObject dsm,
                        Widget shell,
                        XtPointer dataPtr) ;
static XmDSInfo GetDSFromStream(
                        XmDropSiteManagerObject dsm,
                        XtPointer dataPtr,
                        Boolean *close,
                        unsigned char *type) ;
static void GetNextDS(
                        XmDropSiteManagerObject dsm,
                        XmDSInfo parentInfo,
                        XtPointer dataPtr) ;
static XmDSInfo ReadTree(
                        XmDropSiteManagerObject dsm,
                        XtPointer dataPtr) ;
static void FreeDSTree(
                        XmDSInfo tree) ;
static void ChangeRoot(
                        XmDropSiteManagerObject dsm,
                        XtPointer clientData,
                        XtPointer callData) ;
static int CountDropSites(
                        XmDSInfo info) ;
static void CreateInfo(
                        XmDropSiteManagerObject dsm,
                        Widget widget,
                        ArgList args,
                        Cardinal argCount) ;
static void CopyVariantIntoFull(
                        XmDropSiteManagerObject dsm,
                        XmDSInfo variant,
                        XmDSFullInfo full_info) ;
static void RetrieveInfo(
                        XmDropSiteManagerObject dsm,
                        Widget widget,
                        ArgList args,
                        Cardinal argCount) ;
static void CopyFullIntoVariant(
                        XmDSFullInfo full_info,
                        XmDSInfo variant) ;
static void UpdateInfo(
                        XmDropSiteManagerObject dsm,
                        Widget widget,
                        ArgList args,
                        Cardinal argCount) ;
static void StartUpdate(
                        XmDropSiteManagerObject dsm,
                        Widget refWidget) ;
static void EndUpdate(
                        XmDropSiteManagerObject dsm,
                        Widget refWidget) ;
static void DestroyInfo(
                        XmDropSiteManagerObject dsm,
                        Widget widget) ;
static void SyncDropSiteGeometry(
                        XmDropSiteManagerObject dsm,
                        XmDSInfo info) ;
static void SyncTree(
                        XmDropSiteManagerObject dsm,
                        Widget shell) ;
static void Update(
                        XmDropSiteManagerObject dsm,
                        XtPointer clientData,
                        XtPointer callData) ;
static Boolean HasDropSiteDescendant(
                        XmDropSiteManagerObject dsm,
                        Widget widget) ;
static void DestroyCallback(
                        Widget widget,
                        XtPointer client_data,
                        XtPointer call_data) ;

/********    End Static Function Declarations    ********/

static XtResource resources[] = {
{ XmNnotifyProc, XmCNotifyProc, XmRCallbackProc,
  sizeof(XtCallbackProc),
  XtOffsetOf( struct _XmDropSiteManagerRec, dropManager.notifyProc),
  XmRImmediate, NULL },
{ XmNtreeUpdateProc, XmCTreeUpdateProc, XmRCallbackProc,
  sizeof(XtCallbackProc),
  XtOffsetOf( struct _XmDropSiteManagerRec, dropManager.treeUpdateProc),
  XmRImmediate, NULL },
{ XmNclientData, XmCClientData, XmRPointer,
  sizeof(XtPointer),
  XtOffsetOf( struct _XmDropSiteManagerRec, dropManager.client_data),
  XmRImmediate, NULL },
};

/*  class record definition  */

externaldef(xmdropsitemanagerclassrec)
    XmDropSiteManagerClassRec xmDropSiteManagerClassRec =
{
	{
		(WidgetClass) &objectClassRec,    /* superclass	         */
		"XmDropSiteManager",              /* class_name	         */
		sizeof(XmDropSiteManagerRec),     /* widget_size	         */
		ClassInit, 			  /* class_initialize      */
		ClassPartInit,                    /* class part initialize */
		False,                            /* class_inited          */
		DropSiteManagerInitialize,        /* initialize	         */
		NULL,                             /* initialize_hook       */
		NULL,                             /* obj1                  */
		NULL,							  /* obj2                  */
		0,								  /* obj3	                 */
		resources,                        /* resources	         */
		XtNumber(resources),              /* num_resources         */
		NULLQUARK,                        /* xrm_class	         */
		True,                             /* obj4                  */
		XtExposeCompressSeries,           /* obj5                  */
		True,                             /* obj6                  */
		False,                            /* obj7                  */
		Destroy,                          /* destroy               */
		NULL,                             /* obj8                  */
		NULL,				              /* obj9                  */
		SetValues,                        /* set_values	         */
		NULL,                             /* set_values_hook       */
		NULL,                             /* obj10                 */
		NULL,                             /* get_values_hook       */
		NULL,                             /* obj11    	         */
		XtVersion,                        /* version               */
		NULL,                             /* callback private      */
		NULL,                             /* obj12                 */
		NULL,                             /* obj13                 */
		NULL,				              /* obj14                 */
		NULL,                             /* extension             */
	},
	{
		CreateInfo,          /* createInfo           */
		DestroyInfo,         /* destroyInfo          */
		StartUpdate,         /* startUpdate          */
		RetrieveInfo,        /* retrieveInfo         */
		UpdateInfo,          /* updateInfo           */
		EndUpdate,           /* endUpdate            */

		Update,              /* updateDSM            */

		ProcessMotion,       /* processMotion        */
		ProcessDrop,         /* processDrop          */
		ChangeOperation,     /* operationChanged     */
		ChangeRoot,          /* changeDSRoot         */

		InsertInfo,          /* insertInfo           */
		RemoveInfo,          /* removeInfo           */

		SyncTree,            /* syncTree             */
		GetTreeFromDSM,      /* getTreeFromDSM       */

		CreateTable,         /* createTable          */
		DestroyTable,        /* destroyTable         */
		RegisterInfo,        /* registerInfo         */
		WidgetToInfo,        /* widgetToInfo         */
		UnregisterInfo,      /* unregisterInfo       */

		NULL,                /* extension            */
	},
};

externaldef(xmdropsitemanagerobjectclass) WidgetClass
	xmDropSiteManagerObjectClass = (WidgetClass)
		&xmDropSiteManagerClassRec;

static void
ClassInit( void )
{
    _XmRegisterPixmapConverters();
}

static void
ClassPartInit(
		WidgetClass wc )
{
	/*EMPTY*/
}


static void
DropSiteManagerInitialize(
		Widget rw,
		Widget nw,
		ArgList args,
		Cardinal *num_args )
{
	XmDropSiteManagerObject 	dsm = (XmDropSiteManagerObject)nw;
	XmDSFullInfoRec info_rec;
	XmDSFullInfo info = &(info_rec);

	dsm->dropManager.dragUnderData = NULL;
	dsm->dropManager.curInfo = NULL;
	dsm->dropManager.curTime = 0;
	dsm->dropManager.oldX = dsm->dropManager.curX = 0;
	dsm->dropManager.oldY = dsm->dropManager.curY = 0;
	dsm->dropManager.curDropSiteStatus = XmINVALID_DROP_SITE;
	dsm->dropManager.curDragContext = NULL;
	dsm->dropManager.curAnimate = True;
	dsm->dropManager.curOperations = XmDROP_NOOP;
	dsm->dropManager.curOperation = XmDROP_NOOP;
	dsm->dropManager.curAncestorClipRegion = _XmRegionCreate();
	dsm->dropManager.newAncestorClipRegion = _XmRegionCreate();
	DSMCreateTable(dsm);
	dsm->dropManager.dsRoot = NULL;
	dsm->dropManager.rootX = dsm->dropManager.rootY = 0;
	dsm->dropManager.rootW = dsm->dropManager.rootH = ~0;
	dsm->dropManager.clipperList = NULL;
	dsm->dropManager.updateInfo = NULL;
	dsm->dropManager.updateTimeOutId = 0;
	dsm->dropManager.dragUnderData = NULL;

	/* Patch around broken Xt interfaces */
	XtGetSubresources(nw, info, NULL, NULL, _XmDSResources,
		_XmNumDSResources, NULL, 0);
}

static void
Destroy(
		Widget w )
{
	XmDropSiteManagerObject	dsm = (XmDropSiteManagerObject)w;

	if (dsm->dropManager.updateTimeOutId)
	    XtRemoveTimeOut(dsm->dropManager.updateTimeOutId);

	DSMDestroyTable(dsm);
	_XmRegionDestroy(dsm->dropManager.curAncestorClipRegion);
	_XmRegionDestroy(dsm->dropManager.newAncestorClipRegion);
}

static Boolean
SetValues(
		Widget cw,
		Widget rw,
		Widget nw,
		ArgList args,
		Cardinal *num_args )
{
	/*EMPTY*/
	return False;
}

/* Function for Hash table */
static Boolean
CompareWidgets(XtPointer w1, XtPointer w2)
{
  return(w1 == w2);
}

static XmHashValue
HashWidget(XtPointer w1)
{
  return((XmHashValue)(long)w1);
}

static void
CreateTable(
		XmDropSiteManagerObject dsm )
{
  XtPointer *tab = &(dsm->dropManager.dsTable);

  _XmProcessLock();
  *tab = (XtPointer) _XmAllocHashTable(100, CompareWidgets, HashWidget);
  _XmProcessUnlock();
}

static void
DestroyTable(
        XmDropSiteManagerObject dsm )
{
    XtPointer * tab = &(dsm->dropManager.dsTable);

    _XmProcessLock();
    _XmFreeHashTable((XmHashTable) *tab);
    _XmProcessUnlock();
    *tab = NULL;
}

#define DSTABLE(dsm) ((XmHashTable)(dsm->dropManager.dsTable))

static void
RegisterInfo(
        register XmDropSiteManagerObject dsm,
        register Widget widget,
        register XtPointer info )
{
    register XmHashTable tab;

    if (GetDSRegistered(info)) return;

    DPRINT(("(RegI) Widget %p (%s) info %p (internal %d widget %p)\n",
	    widget, XtName(widget), info, GetDSInternal(info),
	    GetDSWidget(info)));

    tab = DSTABLE(dsm);

    _XmProcessLock();
    /* Resize if the table has many more entries than slots */
    if (_XmHashTableCount(tab) > (2 * _XmHashTableSize(tab)))
      _XmResizeHashTable(tab, 2 * _XmHashTableSize(tab));

    _XmAddHashEntry(tab, widget, info);
    _XmProcessUnlock();

    SetDSRegistered(info, True);
}

static void
UnregisterInfo(
        register XmDropSiteManagerObject dsm,
        register XtPointer info )
{
    XmHashTable tab;
    XtPointer iterator;
    Widget widget = GetDSWidget(info);
    XtPointer data;

    if ((info == NULL) || !GetDSRegistered(info))
      return;

    DPRINT(("(UnregI) Widget %p (%s) info %p (internal %d widget %p)\n",
	    widget, XtName(widget), info, GetDSInternal(info),
	    GetDSWidget(info)));

    tab = DSTABLE(dsm);

    iterator = NULL;

    _XmProcessLock();
    while((data = _XmGetHashEntryIterate(tab, widget, &iterator)) != NULL)
      {
	if (data == info) {
	  _XmRemoveHashIterator(tab, &iterator);
	  break;
	}
      }
      _XmProcessUnlock();

    SetDSRegistered(info, False);
}

static XtPointer
WidgetToInfo(
        register XmDropSiteManagerObject dsm,
        register Widget widget )
{
  XmHashTable tab;
  XmDSInfo info;

  tab = DSTABLE(dsm);

  info = (XmDSInfo) _XmGetHashEntry(tab, widget);

  return((XtPointer) info);
}

static Boolean
Coincident(
        XmDropSiteManagerObject dsm,
        Widget w,
        XmDSClipRect *r )
{
	XRectangle wR;
	Boolean hit = False;

	if (!XtIsShell(w))
	{
		/* r is shell relative, so wR needs to be translated */
		XtTranslateCoords(XtParent(w), XtX(w), XtY(w),
			&(wR.x), &(wR.y));
		wR.x -= dsm->dropManager.rootX;
		wR.y -= dsm->dropManager.rootY;
	}
	else
	{
		wR.x = wR.y = 0;
	}


	wR.width = XtWidth(w);
	wR.height = XtHeight(w);

	if ( !(r->detected & XmDROP_SITE_LEFT_EDGE) && (r->x == wR.x))
	{
		r->detected |= XmDROP_SITE_LEFT_EDGE;
		hit = True;
	}

	if ( !(r->detected & XmDROP_SITE_RIGHT_EDGE) &&
		((r->x + r->width) == (wR.x + wR.width)))
	{
		r->detected |= XmDROP_SITE_RIGHT_EDGE;
		hit = True;
	}

	if ( !(r->detected & XmDROP_SITE_TOP_EDGE) && (r->y == wR.y))
	{
		r->detected |= XmDROP_SITE_TOP_EDGE;
		hit = True;
	}

	if ( !(r->detected & XmDROP_SITE_BOTTOM_EDGE) &&
		((r->y + r->height) == (wR.y + wR.height)))
	{
		r->detected |= XmDROP_SITE_BOTTOM_EDGE;
		hit = True;
	}

	return(hit);
}

static Boolean
IsDescendent(
        Widget parentW,
        Widget childW )
{
	Widget tmp;

	if ((parentW == NULL) || (childW == NULL))
		return(False);

	tmp = XtParent(childW);

	while (tmp != parentW)
	{
		if (XtIsShell(tmp))
			return(False);

		tmp = XtParent(tmp);
	}

	return(True);
}

static void
DetectAncestorClippers(
        XmDropSiteManagerObject dsm,
        Widget w,
        XmDSClipRect *r,
        XmDSInfo info )
{
	/*
	 * We know that r represents the visible region of the dropSite
	 * as clipped by its ancestors in shell relative coordinates.  We
	 * now search for the most ancient ancestor who provides that clip.
	 * We do this by looking from the shell downward for the parent
	 * whose edge is coincident with a clipped edge.
	 *
	 * We can add as many as four clippers to the tree as a result
	 * of this routine.
	 */


	/*
	 * Hygiene.
	 */
	if (w == NULL)
		return;

	if (!XtIsShell(w))
		DetectAncestorClippers(dsm, XtParent(w), r, info);

	/*
	 * We never need to add the shell to the tree as a clipper.
	 * We call Coincident first so that any clipping provided by
	 * the shell is marked in the cliprect structure.
	 */
	if ((Coincident(dsm, w, r)) && (!XtIsShell(w)))
	{
		XmDSInfo clipper;

		/* Have we already put this clipper in the tree? */
		if ((clipper = (XmDSInfo) DSMWidgetToInfo(dsm, w)) != NULL)
			return;

		clipper = CreateClipperDSInfo(dsm, w);
		DSMRegisterInfo(dsm, w, (XtPointer) clipper);
		SetDSParent(clipper, dsm->dropManager.clipperList);
		dsm->dropManager.clipperList = (XtPointer) clipper;
	}
}

static void
DetectImpliedClipper(
        XmDropSiteManagerObject dsm,
        XmDSInfo info )
{
	static XmRegion tmpRegion = NULL;

	if (tmpRegion == NULL)
	{
		tmpRegion = _XmRegionCreate();
	}

	if ((GetDSType(info) == XmDROP_SITE_SIMPLE) && GetDSHasRegion(info))
	{
		Widget w = GetDSWidget(info);
		XRectangle wr, tr, rr;

		/*
		 * This step only has meaning if there is a separately
		 * specified region for this drop site (which can only be done
		 * for simple drop sites).
		 */

		/* The region will be relative to the origin of the widget */
		wr.x = wr.y = 0;
		wr.width = XtWidth(w);
		wr.height = XtHeight(w);

		_XmRegionGetExtents(GetDSRegion(info), &rr);

		_XmIntersectionOf(&wr, &rr, &tr);

		if ((rr.x != tr.x) ||
			(rr.y != tr.y) ||
			(rr.width != tr.width) ||
			(rr.height != tr.height))
		{
			XmDSInfo clipper;
			/*
			 * The implied clipper is magic.  It's in the tree but not
			 * of it.  (It refers to the same widget, but it's not
			 * registered.)
			 */

			clipper = CreateClipperDSInfo(dsm, w);
			SetDSParent(clipper, dsm->dropManager.clipperList);
			dsm->dropManager.clipperList = (XtPointer) clipper;
		}
	}
}

static void
DetectAllClippers(
        XmDropSiteManagerObject dsm,
        XmDSInfo parentInfo )
{
	XmDSInfo childInfo;
	XmDSClipRect extents, clippedExtents;
	int i;
	Widget w;
	static XmRegion tmpR = NULL;

	if (GetDSLeaf(parentInfo))
		return;

	_XmProcessLock();
	if (tmpR == NULL)
	{
		tmpR = _XmRegionCreate();
	}
	_XmProcessUnlock();

	for (i = 0; i < (int)GetDSNumChildren(parentInfo); i++)
	{
		childInfo = (XmDSInfo) GetDSChild(parentInfo, i);
		/*
		 * Because we don't allow composite drop sites to have
		 * arbitrary regions, and Motif doesn't support shaped
		 * widgets, we do a simple rectangle clip detection.
		 *
		 * IntersectWithAncestors expects the region to be in
		 * widget relative coordinates, and returns in shell relative
		 * coordinates (the ultimate ancestor is the shell).
		 */
		_XmRegionGetExtents(GetDSRegion(childInfo),
			(XRectangle *)(&extents));

		_XmProcessLock();
		_XmRegionUnion(GetDSRegion(childInfo), GetDSRegion(childInfo),
			tmpR);

		w = GetDSWidget(childInfo);

		IntersectWithWidgetAncestors(w, tmpR);

		_XmRegionGetExtents(tmpR, (XRectangle *)(&clippedExtents));
		_XmProcessUnlock();

		/* tmpR is now in shell relative position */

		clippedExtents.detected = 0;

		if ((clippedExtents.width < extents.width) ||
			(clippedExtents.height < extents.height))
		{
			/*
			 * We've been clipped.  Find out who did it and add
			 * them to the tree.
			 */
			DetectAncestorClippers(dsm,
				XtParent(GetDSWidget(childInfo)),
				&clippedExtents, childInfo);
		}

		/*
		 * We now have inserted clippers for any ancestors which may
		 * have clipped the widget.  Now we need to check for the
		 * case that the widget itself clips the region.
		 */
		DetectImpliedClipper(dsm, childInfo);

		/* Re-Curse */
		DetectAllClippers(dsm, childInfo);
	}
}

static Boolean
InsertClipper(
        XmDropSiteManagerObject dsm,
        XmDSInfo parentInfo,
		XmDSInfo clipper )
{
	int i;
	XmDSInfo childInfo;

	/*
	 * Do a tail-end recursion which will insert the clipper into
	 * the info tree as a child of its closest ancestor in the tree.
	 */

	if (GetDSLeaf(parentInfo))
		return(False);

	for (i=0; i < (int)GetDSNumChildren(parentInfo); i++)
	{
		childInfo = (XmDSInfo) GetDSChild(parentInfo, i);
		if (InsertClipper(dsm, childInfo, clipper))
			return(True);
	}

	if (IsDescendent(GetDSWidget(parentInfo), GetDSWidget(clipper)))
	{
		i = 0;
		while (i < (int)GetDSNumChildren(parentInfo))
		{
			childInfo = (XmDSInfo) GetDSChild(parentInfo, i);
			if (IsDescendent(GetDSWidget(clipper),
				GetDSWidget(childInfo)))
			{
				RemoveDSChild(parentInfo, childInfo);
				AddDSChild(clipper, childInfo,
					GetDSNumChildren(clipper));
			}
			else
				/*
				 * Because RemoveDSChild monkeys with the num children,
				 * we only increment i if we haven't called
				 * RemoveDSChild.
				 */
				i++;
		}

		AddDSChild(parentInfo, clipper, GetDSNumChildren(parentInfo));

		/* We have inserted the clipper into the tree */
		return(True);
	}
	else
		return(False);
}

static void
DetectAndInsertAllClippers(
        XmDropSiteManagerObject dsm,
        XmDSInfo root )
{
	XmDSInfo clipper;

	if ((!GetDSShell(root)) || (GetDSRemote(root)))
		return;

	DetectAllClippers(dsm, root);

	while ((clipper = (XmDSInfo) dsm->dropManager.clipperList) != NULL)
	{
		dsm->dropManager.clipperList = GetDSParent(clipper);
		(void) InsertClipper(dsm, root, clipper);
	}
}

static void
RemoveClipper(
        XmDropSiteManagerObject dsm,
        XmDSInfo clipper )
{
  XmDSInfo parentInfo = (XmDSInfo) GetDSParent(clipper);
  int i;

  /* Remove the clipper from its parent */
  RemoveDSChild(parentInfo, clipper);

  /*
   * Pull all of the children up into the parent's child
   * list between the clipper and the clipper's sibling.
   */
  for (i = 0; i < (int)GetDSNumChildren(clipper); i++) {
    XmDSInfo childInfo = (XmDSInfo) GetDSChild(clipper, i);
    AddDSChild(parentInfo, childInfo, GetDSNumChildren(parentInfo));
  }

  /*
   * Destroy the clipper
   */
  DSMUnregisterInfo(dsm, clipper);
  DestroyDSInfo(clipper, True);
}

static void
RemoveAllClippers(
        XmDropSiteManagerObject dsm,
        XmDSInfo parentInfo )
{
  XmDSInfo child;
  int i;

  if (!GetDSLeaf(parentInfo))
    {
      i = 0;
      while(i < (int)GetDSNumChildren(parentInfo)) {
	child = (XmDSInfo) GetDSChild(parentInfo, i);
	RemoveAllClippers(dsm, child);
	if (GetDSInternal(child))
	  RemoveClipper(dsm, child);
	/* Only increment i if the current child wasn't
	   removed.  Otherwise we'll skip items in the list
	   unintentionally */
	if (child == (XmDSInfo) GetDSChild(parentInfo, i)) i++;
      }
    }
}

static void
DestroyDSInfo(
        XmDSInfo info,
                        Boolean substructures )
{
	DestroyDS(info, substructures);
}

static XmDSInfo
CreateShellDSInfo(
        XmDropSiteManagerObject dsm,
        Widget widget )
{
	XmDSInfo		info;
	XmRegion region = _XmRegionCreate();
	XRectangle rect;

	info = (XmDSInfo) XtCalloc(1, sizeof(XmDSLocalNoneNodeRec));

	SetDSLeaf(info, True);
	SetDSShell(info, True);
	SetDSAnimationStyle(info, XmDRAG_UNDER_NONE);
	SetDSType(info, XmDROP_SITE_COMPOSITE);
	SetDSInternal(info, True);
	SetDSActivity(info, XmDROP_SITE_INACTIVE);
	SetDSWidget(info, widget);

	rect.x = rect.y = 0;
	rect.width = XtWidth(widget);
	rect.height = XtHeight(widget);
	_XmRegionUnionRectWithRegion(&rect, region, region);
	SetDSRegion(info, region);

	XtAddCallback(widget, XmNdestroyCallback,
		DestroyCallback, dsm);

	return(info);
}

static XmDSInfo
CreateClipperDSInfo(
        XmDropSiteManagerObject dsm,
        Widget clipW )
{
	XmDSInfo info = NULL;
	XmRegion region = _XmRegionCreate();
	XRectangle rect;

	info = (XmDSInfo) XtCalloc(1, sizeof(XmDSLocalNoneNodeRec));

	SetDSLeaf(info, True);
	SetDSInternal(info, True);
	SetDSType(info, XmDROP_SITE_COMPOSITE);
	SetDSAnimationStyle(info, XmDRAG_UNDER_NONE);
	SetDSWidget(info, clipW);
	SetDSActivity(info, XmDROP_SITE_ACTIVE);

	rect.x = rect.y = 0;
	rect.width = XtWidth(clipW);
	rect.height = XtHeight(clipW);
	_XmRegionUnionRectWithRegion(&rect, region, region);
	SetDSRegion(info, region);

	/*
	 * Don't need a destroy callback.  When this widget is destroyed
	 * the drop site children will be destroyed and as a side-effect
	 * of the last drop site being destroyed, the clipper will be
	 * destroyed.
	 */

	return(info);
}


static void
InsertInfo(
        XmDropSiteManagerObject dsm,
        XtPointer info,
        XtPointer call_data )
{
	XmDSInfo	childInfo = (XmDSInfo) info;
	XmDSInfo	parentInfo = NULL;
	Widget		parent = XtParent(GetDSWidget(childInfo));

	while (!(parentInfo = (XmDSInfo) DSMWidgetToInfo(dsm, parent)) &&
		!XtIsShell(parent))
	{
		parent = XtParent(parent);
	}

	if (parentInfo == NULL)
	{
		/*
		 * We've traversed clear back to the shell and not found a
		 * parent for this info.  Therefore this must be the first drop
		 * site to be registered under this shell, and we must create a
		 * parent place holder at for shell
		 */
		parentInfo = CreateShellDSInfo(dsm, parent);
		DSMRegisterInfo(dsm, parent, (XtPointer) parentInfo);
		AddDSChild(parentInfo, childInfo, GetDSNumChildren(parentInfo));
		if ((dsm->dropManager.treeUpdateProc) &&
			(!XtIsRealized(parent) ||
				(_XmGetDragProtocolStyle(parent) == XmDRAG_DYNAMIC)))
		{
			/*
			 * If this is a preregister client and the shell isn't
			 * realized yet, we need to register this shell with the
			 * DragDisplay so the DragDisplay can register a realize
			 * callback on this shell.
			 *
			 * OR
			 *
			 * If this is a dynamic client, we need to notify the Drag-
			 * Display exactly once so that event handlers and such
			 * can be installed on this client.
			 */

			XmDropSiteTreeAddCallbackStruct	outCB;

			outCB.reason = XmCR_DROP_SITE_TREE_ADD;
			outCB.event = NULL;
			outCB.rootShell = parent;
			outCB.numDropSites = 0; /* Unused */
			outCB.numArgsPerDSHint = 0;

			(dsm->dropManager.treeUpdateProc)
			  ((Widget) dsm, NULL, (XtPointer) &outCB);
		}
	}
	else if (GetDSType(parentInfo) == XmDROP_SITE_COMPOSITE)
	{
		AddDSChild(parentInfo, childInfo, GetDSNumChildren(parentInfo));
	}
	else
	{
		XmeWarning(GetDSWidget(childInfo), MESSAGE1);
	}
}

static void
RemoveInfo(
        XmDropSiteManagerObject dsm,
        XtPointer info )
{
	Widget widget = GetDSWidget(info);
	XmDSInfo parentInfo = (XmDSInfo) GetDSParent(info);

	RemoveDSChild(parentInfo, (XmDSInfo) info);

	DSMUnregisterInfo(dsm, info);

	XtRemoveCallback(widget, XmNdestroyCallback, DestroyCallback, dsm);

	if ((parentInfo != NULL) &&
		(GetDSNumChildren(parentInfo) == 0) &&
		(GetDSInternal(parentInfo)))
	{
		if (XtIsShell(GetDSWidget(parentInfo)))
		{
			/*
			 * Need to notify the DragDisplay that this shell no
			 * longer has any drop sites in it.
			 */
			if (dsm->dropManager.treeUpdateProc)
			{
				XmDropSiteTreeAddCallbackStruct	outCB;

				outCB.reason = XmCR_DROP_SITE_TREE_REMOVE;
				outCB.event = NULL;
				outCB.rootShell = GetDSWidget(parentInfo);
				(dsm->dropManager.treeUpdateProc)
				  ((Widget)dsm, NULL, (XtPointer) &outCB);
			}
		}

		DSMDestroyInfo(dsm, GetDSWidget(parentInfo));
	}
}


static Boolean
IntersectWithWidgetAncestors(
        Widget w,
        XmRegion r )
{
	/*
	 * r is the bounding box of the region.  It is in widget relative
	 * coordinates.
	 */
	XRectangle parentR;
	static XmRegion tmpR = NULL;
	Dimension bw = XtBorderWidth(w);

	if (XtIsShell(w))
	{
		return(True);
	}

	_XmProcessLock();
	if (tmpR == NULL)
	{
		tmpR = _XmRegionCreate();
	}
	_XmProcessUnlock();

	/* Translate the coordinates into parent relative coords */
	_XmRegionOffset(r, (XtX(w) + bw), (XtY(w) + bw));

	parentR.x = parentR.y = 0;
	parentR.width = XtWidth(XtParent(w));
	parentR.height = XtHeight(XtParent(w));

	_XmProcessLock();
	_XmRegionClear(tmpR);
	_XmRegionUnionRectWithRegion(&parentR, tmpR, tmpR);

	_XmRegionIntersect(tmpR, r, r);
	_XmProcessUnlock();

	if (!_XmRegionIsEmpty(r))
		return(IntersectWithWidgetAncestors(XtParent(w), r));
	else
		return(False);
}


static Boolean
IntersectWithDSInfoAncestors(
        XmDSInfo parent,
        XmRegion r )
{
	static XmRegion testR = (XmRegion) NULL;
	static XmRegion pR = (XmRegion) NULL;
	Dimension bw;

	_XmProcessLock();
	if (testR == NULL)
	{
		testR = _XmRegionCreate();
		pR = _XmRegionCreate();
	}
	_XmProcessUnlock();

	/*
	 * A simplifying assumption in this code is that the regions
	 * are all relative to the shell widget.  We don't have to
	 * do any fancy translations.
	 *
	 * All that we have to do is successively intersect the drop site
	 * region with its ancestors until we reach the top of the drop
	 * site tree.
	 */

	/*
	 * If got to the top, then there is some part of the
	 * region which is visible.
	 */
	if (parent == NULL)
		return(True);

	_XmProcessLock();
	_XmRegionUnion(GetDSRegion(parent), GetDSRegion(parent), pR);
	_XmProcessUnlock();

	if ((bw = GetDSBorderWidth(parent)) != 0)
	{
		/*
		 * Adjust for the border width ala X clipping
		 * Recall that all Composite's drop rectangles represent the
		 * refW's sensitive area (including border width), but clipping
		 * should be done to the window not the border.  The clip
		 * region is smaller than the sensitive region.
		 */
		_XmProcessLock();
		_XmRegionShrink(pR, bw, bw);
		_XmProcessUnlock();
	}

	_XmProcessLock();
	_XmRegionIntersect(r, pR, r);
	_XmProcessUnlock();

	/* C will ensure that we only recurse if testR is non-empty */
	return((!_XmRegionIsEmpty(r)) &&
		(IntersectWithDSInfoAncestors(
			(XmDSInfo) GetDSParent(parent), r)));
}


static Boolean
CalculateAncestorClip(
        XmDropSiteManagerObject dsm,
        XmDSInfo info,
        XmRegion r )
{
	/*
	 * When this procedure finishes, r will contain the composite
	 * clip region for all ancestors of info.  The clip region will
	 * be in shell relative coordinates.
	 */
	_XmRegionClear(r);

    if (GetDSRemote(info))
	{
		XRectangle universe;

		/* Set it to the "universe" -- which is shell relative */
		universe.x = universe.y = 0;
		universe.width = dsm->dropManager.rootW;
		universe.height = dsm->dropManager.rootH;

		_XmRegionUnionRectWithRegion(&universe, r, r);

		/*
		 * IntersectWithDSInfoAncestors will shoot the universe
		 * through all of the DSInfo ancestors and return us what
		 * is left in r.
		 */
		return(IntersectWithDSInfoAncestors(
			(XmDSInfo) GetDSParent(info), r));
	}
	else
	{
		XRectangle parentR;
		Widget parentW = XtParent(GetDSWidget(info));

		if (parentW == NULL)
			return(True);
		else
		{
			parentR.x = parentR.y = -(XtBorderWidth(parentW));
			parentR.width = XtWidth(parentW);
			parentR.height = XtHeight(parentW);

			_XmRegionUnionRectWithRegion(&parentR, r, r);

			/*
			 * IntersectWithWidgetAncestors will intersect the parent
			 * of info with all successive parents and return us what
			 * is left in r.
			 */
			return(IntersectWithWidgetAncestors(parentW, r));
		}
	}
}


static Boolean
PointInDS(
        XmDropSiteManagerObject dsm,
        XmDSInfo info,
        Position x,
        Position y )
{
	static XmRegion testR = (XmRegion) NULL;
	static XmRegion tmpR = (XmRegion) NULL;
	XmRegion *visR = &(dsm->dropManager.newAncestorClipRegion);
	Widget w = GetDSWidget(info);

	_XmProcessLock();
	if (testR == NULL)
	{
		testR = _XmRegionCreate();
		tmpR = _XmRegionCreate();
	}
	_XmProcessUnlock();

	/*
	 * CalculateAncestorClip will intersect the universe with all of
	 * the ancestors.  If anything is left, it will return true the
	 * intersection will be in tmpR.
	 */

	_XmProcessLock();
	if (!CalculateAncestorClip(dsm, info, tmpR))
	{
		_XmProcessUnlock();
		return(False);
	}
	_XmProcessUnlock();

	if (GetDSRemote(info))
	{
		/*
		 * We know that the region in the info struct is shell relative
		 */
		_XmProcessLock();
		_XmRegionIntersect(tmpR, GetDSRegion(info), testR);
		_XmProcessUnlock();
	}
	else
	{
		Position tmpX, tmpY;

		_XmRegionUnion(GetDSRegion(info), GetDSRegion(info), testR);

		/*
		 * We know that the information is widget
		 * relative so we will have to translate it.
		 */

		XtTranslateCoords(w, 0, 0, &tmpX, &tmpY);

		_XmProcessLock();
		_XmRegionOffset(testR, (tmpX - dsm->dropManager.rootX),
			(tmpY - dsm->dropManager.rootY));
		_XmRegionIntersect(tmpR, testR, testR);
		_XmProcessUnlock();
	}

	_XmProcessLock();
	if ((!_XmRegionIsEmpty(testR)) &&
		(_XmRegionPointInRegion(testR, x, y)))
	{
		_XmRegionUnion(tmpR, tmpR, *visR);
		_XmProcessUnlock();
		return(True);
	}
	else
	{
		_XmProcessUnlock();
		return(False);
	}

	/* _XmProcessUnlock();	*/	/* not reached */
}


static XmDSInfo
PointToDSInfo(
        XmDropSiteManagerObject dsm,
        XmDSInfo info,
        Position x,
        Position y )
{
  unsigned int	i;
  XmDSInfo		child = NULL;

  if (!GetDSLeaf(info))
    {
      /*
       * This should be optimized at some point.
       * CalculateAncestorClip is having to do potentially
       * unneccessary work, because it starts from scratch each time.
       */
      for (i = 0; i < GetDSNumChildren(info); i++)
	{
	  Boolean managed;

	  child = (XmDSInfo) GetDSChild(info,i);

	  if (GetDSRemote(child))
	    managed = True;
	  else {
	    Widget child_widget;
	    Widget parent;

	    child_widget = GetDSWidget(child);
	    parent = XtParent(child_widget);
	    managed = XtIsManaged(child_widget);

	    /* CR 7848,  first check if DS widget is managed and
	       all the parents are managed.  To accomplish this
	       we wander up to the shell and make sure all the widget
	       parents are managed */
	    while(managed && ! XtIsShell(parent)) {
	      managed = XtIsManaged(parent);
	      parent = XtParent(parent);
	    }
	  }

	  if (managed &&
	      PointInDS(dsm, child, x, y) &&
	      GetDSActivity(child) != XmDROP_SITE_INACTIVE)
	    {
	      if (!GetDSLeaf(child))
		{
		  XmDSInfo descendant = PointToDSInfo(dsm, child,
						      x, y);

		  if (descendant != NULL)
		    return(descendant);
		}

	      if (!GetDSInternal(child))
		return(child);
	    }
	}
    }

  return(NULL);
}

static void
DoAnimation(
        XmDropSiteManagerObject dsm,
        XmDragMotionClientData motionData,
        XtPointer callback )
{

  XmDSInfo info = (XmDSInfo) (dsm->dropManager.curInfo);
  XmDSInfo parentInfo = (XmDSInfo) GetDSParent(info);
  Widget w;
  int i, n;
  XmDSInfo child;
  XmAnimationDataRec animationData;
  static XmRegion dsRegion = (XmRegion) NULL;
  static XmRegion clipRegion = (XmRegion) NULL;
  static XmRegion tmpRegion = (XmRegion) NULL;
  Widget dc = dsm->dropManager.curDragContext;
  Boolean sourceIsExternal;
  Dimension bw = 0;
  Arg args[1];

  if (GetDSAnimationStyle(info) == XmDRAG_UNDER_NONE)
    return;

  /*
   * Should we have saved this from the last top level enter?
   */
  n = 0;
  XtSetArg(args[n], XmNsourceIsExternal, &sourceIsExternal); n++;
  XtGetValues(dc, args, n);

  _XmProcessLock();
  if (dsRegion == NULL)
    {
      dsRegion   = _XmRegionCreate();
      clipRegion = _XmRegionCreate();
      tmpRegion  = _XmRegionCreate();
    }
  _XmProcessUnlock();

  if (sourceIsExternal)
    {
      animationData.dragOver = NULL;

      /*
       * The window is expected to the the shell window which will be
       * drawn in with include inferiors.
       */
      animationData.window = XtWindow(GetDSWidget(
						  dsm->dropManager.dsRoot));
      animationData.screen = XtScreen(GetDSWidget(
						  dsm->dropManager.dsRoot));
    }
  else
    {
      animationData.dragOver = motionData->dragOver;
      animationData.window = motionData->window;
      animationData.screen = XtScreen(motionData->dragOver);
    }

  animationData.windowX = dsm->dropManager.rootX;
  animationData.windowY = dsm->dropManager.rootY;
  animationData.saveAddr =
    (XtPointer) &(dsm->dropManager.dragUnderData);

  /* We're going to need a copy. */
  _XmProcessLock();
  _XmRegionUnion(GetDSRegion(info), GetDSRegion(info), dsRegion);
  _XmProcessUnlock();

  bw = GetDSBorderWidth(info);

  if (!GetDSRemote(info))
    {
      Position wX, wY;

      w = GetDSWidget(info);

      XtTranslateCoords(w, 0, 0, &wX, &wY);
      _XmProcessLock();
      _XmRegionOffset(dsRegion, (wX - dsm->dropManager.rootX),
		      (wY - dsm->dropManager.rootY));
      _XmProcessUnlock();
    }

  /* All drawing occurs within the drop site */
  _XmProcessLock();
  _XmRegionUnion(dsRegion, dsRegion, clipRegion);
  _XmProcessUnlock();

  if (bw && !GetDSHasRegion(info))
    {
      /*
       * The region is stored widget relative, and it represents
       * the entire drop-sensitive area of the drop site.  In the
       * case that we provided the region this includes the
       * border area (the x,y position of the bounding box is
       * negative), however, we don't animate the entire
       * sensitive region; we only animate the sensitive region
       * within the border.  Sooo, if we provided the region, and
       * the widget has a border, shrink down the region
       * (which will offset it) before passing it on to the
       * animation code.
       */
      _XmProcessLock();
      _XmRegionShrink(clipRegion, bw, bw);
      _XmProcessUnlock();
    }

  /*
   * trim off anything clipped by ancestors
   * ancestorClip region is in shell relative coordinates.
   */
  _XmProcessLock();
  _XmRegionIntersect(clipRegion,
		     dsm->dropManager.curAncestorClipRegion, clipRegion);
  _XmProcessUnlock();

  /* trim off anything obsucred by a sibling stacked above us */
  if (parentInfo != NULL)
    {
      for (i = 0; i < (int)GetDSNumChildren(parentInfo); i++)
	{
	  child = (XmDSInfo) GetDSChild(parentInfo, i);
	  if (child == info)
	    break;
	  else
	    {
	      if (GetDSRemote(child))
		{
		  /*
		   * Non-local case.  The info region is in shell
		   * relative coordinates.
		   */
		  _XmProcessLock();
		  _XmRegionSubtract(clipRegion, GetDSRegion(child),
				    clipRegion);
		  _XmProcessUnlock();
		}
	      else
		{
		  /*
		   * Local case.  We have to translate the region.
		   */
		  Position wX, wY;
		  Widget sibling = GetDSWidget(child);

		  XtTranslateCoords(sibling, 0, 0, &wX, &wY);
		  _XmProcessLock();
		  _XmRegionUnion(GetDSRegion(child),
				 GetDSRegion(child), tmpRegion);

		  _XmRegionOffset(tmpRegion,
				  (wX - dsm->dropManager.rootX),
				  (wY - dsm->dropManager.rootY));

		  _XmRegionSubtract(clipRegion, tmpRegion,
				    clipRegion);
		  _XmProcessUnlock();
		}
	    }
	}
    }
  _XmProcessLock();
  animationData.clipRegion = clipRegion;
  animationData.dropSiteRegion = dsRegion;
  _XmProcessUnlock();

  _XmDragUnderAnimation((Widget)dsm,
			(XtPointer) &animationData,
			(XtPointer) callback);
}

static void
ProxyDragProc(
        XmDropSiteManagerObject dsm,
		XtPointer client_data,
        XmDragProcCallbackStruct *callback )
{
	XmDSInfo info = (XmDSInfo) dsm->dropManager.curInfo;
	XmDragContext dc = (XmDragContext) callback->dragContext;
	Atom *import_targets = NULL, *export_targets = NULL;
	Cardinal num_import = 0, num_export = 0;
	int n;
	Arg args[10];
	Widget shell;
	unsigned char operations;

	operations = callback->operations & GetDSOperations(info);
	if (XmDROP_MOVE & operations)
	  callback->operation = XmDROP_MOVE;
	else if (XmDROP_COPY & operations)
	  callback->operation = XmDROP_COPY;
	else if (XmDROP_LINK & operations)
	  callback->operation = XmDROP_LINK;
	else
	  callback->operation = XmDROP_NOOP;

	n = 0;
	XtSetArg(args[n], XmNexportTargets, &export_targets); n++;
	XtSetArg(args[n], XmNnumExportTargets, &num_export); n++;
	XtGetValues ((Widget)dc, args, n);

	if (GetDSRemote(info))
		shell = XtParent(dsm);
	else
		shell = GetDSWidget(info);

	while (!XtIsShell(shell))
		shell = XtParent(shell);

	num_import = _XmIndexToTargets(shell,
		GetDSImportTargetsID(info), &import_targets);

	if ((callback->operation != XmDROP_NOOP) &&
		(XmTargetsAreCompatible (XtDisplay (dsm),
			export_targets, num_export, import_targets, num_import)))
		callback->dropSiteStatus = XmVALID_DROP_SITE;
	else
		callback->dropSiteStatus = XmINVALID_DROP_SITE;

	callback->animate = True;
}

static void
HandleEnter(
        XmDropSiteManagerObject dsm,
        XmDragMotionClientData motionData,
        XmDragMotionCallbackStruct *callback,
        XmDSInfo info,
        unsigned char style )
{
	XmDragProcCallbackStruct cbRec;
	Position tmpX, tmpY;
	XRectangle extents;

	cbRec.reason = XmCR_DROP_SITE_ENTER_MESSAGE;
	cbRec.event = (XEvent *) NULL;
	cbRec.timeStamp = callback->timeStamp;
	cbRec.dragContext = dsm->dropManager.curDragContext;
	cbRec.x = dsm->dropManager.curX;
	cbRec.y = dsm->dropManager.curY;
	cbRec.dropSiteStatus = XmVALID_DROP_SITE;
	cbRec.operations = callback->operations;
	cbRec.operation = callback->operation;
	cbRec.animate = True;

	ProxyDragProc(dsm, NULL, &cbRec);

	if ((!GetDSRemote(info)) &&
	    (GetDSDragProc(info) != NULL))
	{
  	        Widget widget = GetDSWidget(info);

		/* Return if this is not a managed widget, CR5215 */
		if (! XtIsManaged(widget)) return;

		/* Make the coordinates widget relative */
		XtTranslateCoords(widget, 0, 0, &tmpX, &tmpY);

		cbRec.x -= tmpX;
		cbRec.y -= tmpY;

		(*(GetDSDragProc(info)))
			(widget, NULL, (XtPointer) &cbRec);
	}

	if ((cbRec.animate) &&
		(cbRec.dropSiteStatus == XmVALID_DROP_SITE))
		DoAnimation(dsm, motionData, (XtPointer) &cbRec);

	dsm->dropManager.curDropSiteStatus = cbRec.dropSiteStatus;
	dsm->dropManager.curAnimate = cbRec.animate;
	dsm->dropManager.curOperations = cbRec.operations;
	dsm->dropManager.curOperation = cbRec.operation;

	if (dsm->dropManager.notifyProc)
	{
		XmDropSiteEnterCallbackStruct	outCB;

		_XmRegionGetExtents(GetDSRegion(info), &extents);

		outCB.reason = XmCR_DROP_SITE_ENTER;
		outCB.event = NULL;
		outCB.timeStamp = cbRec.timeStamp;
		outCB.dropSiteStatus = cbRec.dropSiteStatus;
		outCB.operations = cbRec.operations;
		outCB.operation = cbRec.operation;

		/*
		 * Pass outCB.x and outCB.y as the root relative position
		 * of the entered drop site.  Remote info's are already
		 * in shell coordinates; Local info's are in widget
		 * relative coordinates.
		 */
		if (GetDSRemote(info))
		{
			outCB.x = extents.x + dsm->dropManager.rootX;
			outCB.y = extents.y + dsm->dropManager.rootY;
		}
		else
		{
			Widget	widget = GetDSWidget(info);

			XtTranslateCoords(widget, 0, 0, &tmpX, &tmpY);

			outCB.x = extents.x + tmpX;
			outCB.y = extents.y + tmpY;
		}

		(*(dsm->dropManager.notifyProc))
			((Widget)dsm, dsm->dropManager.client_data,
			 (XtPointer) &outCB);
	}
}


static void
HandleMotion(
        XmDropSiteManagerObject dsm,
        XmDragMotionClientData motionData,
        XmDragMotionCallbackStruct *callback,
        XmDSInfo info,
        unsigned char style )
{
	XmDragProcCallbackStruct cbRec;

	cbRec.reason = XmCR_DROP_SITE_MOTION_MESSAGE;
	cbRec.event = (XEvent *) NULL;
	cbRec.timeStamp = callback->timeStamp;
	cbRec.dragContext = dsm->dropManager.curDragContext;
	cbRec.x = dsm->dropManager.curX;
	cbRec.y = dsm->dropManager.curY;
	cbRec.animate = dsm->dropManager.curAnimate;
	cbRec.dropSiteStatus = dsm->dropManager.curDropSiteStatus;

	if (info != NULL)
	{
		cbRec.operations = dsm->dropManager.curOperations;
		cbRec.operation = dsm->dropManager.curOperation;

		if (	(!GetDSRemote(info)) &&
			(GetDSDragProc(info) != NULL))
		{
			Widget	widget = GetDSWidget(info);
			Position tmpX, tmpY;

			/* Return if this is not a managed widget */
			if (! XtIsManaged(widget)) return;

			/* Make the coordinates widget relative */

			XtTranslateCoords(widget, 0, 0, &tmpX, &tmpY);

			cbRec.x -= tmpX;
			cbRec.y -= tmpY;

			(*(GetDSDragProc(info)))
				(widget, NULL, (XtPointer) &cbRec);
		}

		if ((cbRec.animate) &&
			(cbRec.dropSiteStatus !=
				dsm->dropManager.curDropSiteStatus))
		{
			if (cbRec.dropSiteStatus == XmVALID_DROP_SITE)
				cbRec.reason = XmCR_DROP_SITE_ENTER;
			else
				cbRec.reason = XmCR_DROP_SITE_LEAVE;

			DoAnimation(dsm, motionData, (XtPointer) &cbRec);
			cbRec.reason = XmCR_DROP_SITE_MOTION_MESSAGE;
		}

		dsm->dropManager.curDropSiteStatus = cbRec.dropSiteStatus;
		dsm->dropManager.curAnimate = cbRec.animate;
		dsm->dropManager.curOperations = cbRec.operations;
		dsm->dropManager.curOperation = cbRec.operation;
	}
	else
	{
		cbRec.operations = callback->operations;
		cbRec.operation = callback->operation;
		cbRec.dropSiteStatus = XmNO_DROP_SITE;
	}

	if (dsm->dropManager.notifyProc)
	{
		XmDragMotionCallbackStruct	outCB;

		outCB.reason = XmCR_DRAG_MOTION;
		outCB.event = NULL;
		outCB.timeStamp = cbRec.timeStamp;
		outCB.dropSiteStatus = cbRec.dropSiteStatus;
		outCB.x = dsm->dropManager.curX;
		outCB.y = dsm->dropManager.curY;
		outCB.operations = cbRec.operations;
		outCB.operation = cbRec.operation;

		(*(dsm->dropManager.notifyProc))
			((Widget)dsm, dsm->dropManager.client_data,
			 (XtPointer)&outCB);
	}
}

static void
HandleLeave(
        XmDropSiteManagerObject dsm,
        XmDragMotionClientData motionData,
        XmDragMotionCallbackStruct *callback,
        XmDSInfo info,
        unsigned char style,
        Boolean enterPending )
{
	XmDragProcCallbackStruct cbRec;

	cbRec.reason = XmCR_DROP_SITE_LEAVE_MESSAGE;
	cbRec.event = (XEvent *) NULL;
	cbRec.timeStamp = callback->timeStamp;
	cbRec.dragContext = dsm->dropManager.curDragContext;
	cbRec.x = dsm->dropManager.oldX;
	cbRec.y = dsm->dropManager.oldY;
	cbRec.operations = callback->operations;
	cbRec.operation = callback->operation;
	cbRec.animate = dsm->dropManager.curAnimate;
	cbRec.dropSiteStatus = dsm->dropManager.curDropSiteStatus;

	if (	(!GetDSRemote(info) && (GetDSDragProc(info) != NULL)))
	{
		Widget widget = GetDSWidget(info);
		Position tmpX, tmpY;

		/* Make the coordinates widget relative */

		XtTranslateCoords(widget, 0, 0, &tmpX, &tmpY);

		cbRec.x -= tmpX;
		cbRec.y -= tmpY;

		(*(GetDSDragProc(info)))
			(widget, NULL, (XtPointer) &cbRec);
	}

	if ((cbRec.animate) &&
		(cbRec.dropSiteStatus == XmVALID_DROP_SITE))
		DoAnimation(dsm, motionData, (XtPointer) &cbRec);

	if (dsm->dropManager.notifyProc)
	{
		XmDropSiteEnterPendingCallbackStruct	outCB;

		outCB.reason = XmCR_DROP_SITE_LEAVE;
		outCB.event = callback->event;
		outCB.timeStamp = cbRec.timeStamp;
                outCB.enter_pending = enterPending;

		(*(dsm->dropManager.notifyProc))
			((Widget)dsm, dsm->dropManager.client_data,
			 (XtPointer)&outCB);
	}
}


static void
ProcessMotion(
        XmDropSiteManagerObject dsm,
        XtPointer clientData,
        XtPointer calldata )
{
  XmDragMotionCallbackStruct *callback =
    (XmDragMotionCallbackStruct *) calldata;
  XmDragMotionClientData	motionData =
    (XmDragMotionClientData) clientData;
  Position	x = callback->x, y = callback->y;
  XmDSInfo	dsRoot = (XmDSInfo) (dsm->dropManager.dsRoot);
  XmDSInfo	curDSInfo = (XmDSInfo)(dsm->dropManager.curInfo);
  XmDSInfo	newDSInfo;
  unsigned char style;

  if (dsm->dropManager.curDragContext == NULL)
    {
      XmeWarning((Widget)dsm, MESSAGE2);
      return;
    }

  style = _XmGetActiveProtocolStyle(dsm->dropManager.curDragContext);
  dsm->dropManager.curTime = callback->timeStamp;
  dsm->dropManager.oldX = dsm->dropManager.curX;
  dsm->dropManager.oldY = dsm->dropManager.curY;
  dsm->dropManager.curX = x;
  dsm->dropManager.curY = y;

  if (dsRoot)
    {
      /*
       * Make x and y shell relative, preregister info's are shell
       * relative, and CalculateAncestorClip (called for dynammic)
       * returns a shell relative rectangle.
       */
      x -= dsm->dropManager.rootX;
      y -= dsm->dropManager.rootY;

      newDSInfo =  PointToDSInfo(dsm, dsRoot, x, y);

      if (curDSInfo != newDSInfo)
	{
	  if (curDSInfo) {
	    /* if we are entering a drop site as we leave
	     * the old drop site, we don't want to set
	     * the drop site status to NO_DROP_SITE. We
	     * are using the event field of the callback
	     * (since is is guarenteed to be NULL) for
	     * use as a flag, since we can't modify the
	     * public callback struct.
	     */
	    if (newDSInfo)
	      HandleLeave(dsm, motionData, callback,
			  curDSInfo, style, True);
	    else
	      HandleLeave(dsm, motionData, callback,
			  curDSInfo, style, False);
	  }

	  dsm->dropManager.curInfo = (XtPointer) newDSInfo;
	  _XmRegionUnion(dsm->dropManager.newAncestorClipRegion,
			 dsm->dropManager.newAncestorClipRegion,
			 dsm->dropManager.curAncestorClipRegion);

	  if (newDSInfo)
	    HandleEnter(dsm, motionData, callback,
			newDSInfo, style);

	  return;
	}
    }

  HandleMotion(dsm, motionData, callback, curDSInfo, style);
}

static void
ProcessDrop(
        XmDropSiteManagerObject dsm,
        XtPointer clientData,
        XtPointer cb )
{
	XmDragTopLevelClientData cd =
		(XmDragTopLevelClientData) clientData;
	XmDropStartCallbackStruct *callback =
		(XmDropStartCallbackStruct *) cb;
	XmDropFinishCallbackStruct *finish = (XmDropFinishCallbackStruct *)cb;
	XmDropProcCallbackStruct cbRec;
	Widget	dragContext =
		XmGetDragContext((Widget)dsm, callback->timeStamp);
	XmDSInfo	info = NULL;
	Widget	widget = NULL;
	Position x, y, tmpX, tmpY;
	XmDSInfo savRoot, savInfo;
	XmDSInfo newRoot = (XmDSInfo) DSMWidgetToInfo(dsm, cd->destShell);
	Position savX, savY;
	Dimension savW, savH;
	Time savTime;

	if (dragContext == NULL)
	{
		/*
		 * Can't do a failure transfer.  Just give up.
		 *
		 * Should we send a warning message?
		 */
		return;
	}

	/*
	 * Look out for race conditions.
	 *
	 * This should be enough state saving to allow the drop to occur
	 * in the middle of some other drag.
	 */
	savRoot = (XmDSInfo) dsm->dropManager.dsRoot;
	savInfo = (XmDSInfo) dsm->dropManager.curInfo;
	savX = dsm->dropManager.rootX;
	savY = dsm->dropManager.rootY;
	savW = dsm->dropManager.rootW;
	savH = dsm->dropManager.rootH;
	savTime = dsm->dropManager.curTime;

	dsm->dropManager.curTime = callback->timeStamp;
	dsm->dropManager.dsRoot = (XtPointer) newRoot;
	dsm->dropManager.rootX = cd->xOrigin;
	dsm->dropManager.rootY = cd->yOrigin;
	dsm->dropManager.rootW = cd->width;
	dsm->dropManager.rootH = cd->height;

	x = callback->x - dsm->dropManager.rootX;
	y = callback->y - dsm->dropManager.rootY;

	if (newRoot != NULL)
		info = PointToDSInfo(dsm, (XmDSInfo) dsm->dropManager.dsRoot, x, y);

	if (info != NULL)
        {
            widget = GetDSWidget(info);
        }
	/* Handle error conditions nicely */
	if ((info == NULL) ||
	    ! XtIsManaged(widget) || /* CR 5215 */
	    /* These are wierd conditions */
	    (newRoot == NULL)   ||
	    (GetDSRemote(info)) ||
	    (GetDSDropProc(info) == NULL))
	{
		/* we will do a failure drop transfer */
		Arg		args[4];
		Cardinal	i = 0;

		XtSetArg(args[i], XmNtransferStatus, XmTRANSFER_FAILURE); i++;
		XtSetArg(args[i], XmNnumDropTransfers, 0); i++;
		(void) XmDropTransferStart(dragContext, args, i);

		/* ???
		 * Should we do something interesting with the callback
		 * struct before calling notify in these cases?
		 * ???
		 */
		finish->reason           = XmCR_DROP_FINISH;
		finish->completionStatus = XmDROP_FAILURE;
	}
	else
	{

		/* This will be needed by the ProxyDragProc */
		dsm->dropManager.curInfo = (XtPointer) info;

		/* Make the coordinates widget relative */
		XtTranslateCoords(widget, 0, 0, &tmpX, &tmpY);

		/* Load the dropProcStruct */
		cbRec.reason = XmCR_DROP_MESSAGE;
		cbRec.event = callback->event;
		cbRec.timeStamp = callback->timeStamp;
		cbRec.dragContext = dragContext;

		/* Make the coordinates widget relative */
		XtTranslateCoords(widget, 0, 0, &tmpX, &tmpY);

		cbRec.x = callback->x - tmpX;
		cbRec.y = callback->y - tmpY;


		{ /* Nonsense to pre-load the cbRec correctly */
			XmDragProcCallbackStruct junkRec;

			junkRec.reason = XmCR_DROP_SITE_MOTION_MESSAGE;
			junkRec.event = callback->event;
			junkRec.timeStamp = cbRec.timeStamp;
			junkRec.dragContext = dragContext;
			junkRec.x = cbRec.x;
			junkRec.y = cbRec.y;
			junkRec.dropSiteStatus = dsm->dropManager.curDropSiteStatus;
			junkRec.operation = callback->operation;
			junkRec.operations = callback->operations;
			junkRec.animate = dsm->dropManager.curAnimate;

			ProxyDragProc(dsm, NULL, &junkRec);

			cbRec.dropSiteStatus = junkRec.dropSiteStatus;
			cbRec.operation = junkRec.operation;
			cbRec.operations = junkRec.operations;
		}

		cbRec.dropAction = callback->dropAction;

		/* Call the drop site's drop proc */
		(*(GetDSDropProc(info))) (widget, NULL, (XtPointer) &cbRec);

		callback->operation = cbRec.operation;
		callback->operations = cbRec.operations;
		callback->dropSiteStatus = cbRec.dropSiteStatus;
		callback->dropAction = cbRec.dropAction;
	}

	if (dsm->dropManager.notifyProc)
	{
		(*(dsm->dropManager.notifyProc))
			((Widget)dsm, dsm->dropManager.client_data,
			(XtPointer)callback);
	}

	dsm->dropManager.dsRoot = (XtPointer) savRoot;
	dsm->dropManager.curInfo = (XtPointer) savInfo;
	dsm->dropManager.rootX = savX;
	dsm->dropManager.rootY = savY;
	dsm->dropManager.rootW = savW;
	dsm->dropManager.rootH = savH;
	dsm->dropManager.curTime = savTime;
}

static void
ChangeOperation(
        XmDropSiteManagerObject dsm,
        XtPointer clientData,
        XtPointer calldata )
{
	XmOperationChangedCallbackStruct *callback =
		(XmOperationChangedCallbackStruct *) calldata;
	XmDragMotionClientData	motionData =
		(XmDragMotionClientData) clientData;
	XmDragProcCallbackStruct cbRec;
	XmDSInfo info = (XmDSInfo) dsm->dropManager.curInfo;
	unsigned char style;

	if ((cbRec.dragContext = dsm->dropManager.curDragContext) == NULL)
	{
		XmeWarning((Widget)dsm, MESSAGE3);
		return;
	}
	else
	{
		style = _XmGetActiveProtocolStyle(
			dsm->dropManager.curDragContext);
	}

	cbRec.reason = callback->reason;
	cbRec.event = callback->event;
	cbRec.timeStamp = callback->timeStamp;

	cbRec.x = dsm->dropManager.curX;
	cbRec.y = dsm->dropManager.curY;
	cbRec.dropSiteStatus = dsm->dropManager.curDropSiteStatus;
	cbRec.animate = dsm->dropManager.curAnimate;

	cbRec.operation = callback->operation;
	cbRec.operations = callback->operations;

	if (info != NULL)
	{
		ProxyDragProc(dsm, NULL, &cbRec);

		if ((style == XmDRAG_DYNAMIC) &&
			(!GetDSRemote(info)) &&
			(GetDSDragProc(info) != NULL))
		{
			Widget widget = GetDSWidget(info);
			Position tmpX, tmpY;

			/* Make the coordinates widget relative */
			XtTranslateCoords(widget, 0, 0, &tmpX, &tmpY);

			cbRec.x -= tmpX;
			cbRec.y -= tmpY;

			(*(GetDSDragProc(info)))
				(widget, NULL, (XtPointer) &cbRec);
		}

		if ((cbRec.animate) &&
			(cbRec.dropSiteStatus !=
				dsm->dropManager.curDropSiteStatus))
		{
			if (cbRec.dropSiteStatus == XmVALID_DROP_SITE)
				cbRec.reason = XmCR_DROP_SITE_ENTER_MESSAGE;
			else
				cbRec.reason = XmCR_DROP_SITE_LEAVE_MESSAGE;

			DoAnimation(dsm, motionData, (XtPointer) &cbRec);
			cbRec.reason = callback->reason;
		}

		/* Update the callback rec */
		callback->operations = cbRec.operations;
		callback->operation = cbRec.operation;
		callback->dropSiteStatus = cbRec.dropSiteStatus;

		/* Update the drop site manager */
		dsm->dropManager.curDropSiteStatus = cbRec.dropSiteStatus;
		dsm->dropManager.curAnimate = cbRec.animate;
		dsm->dropManager.curOperations = cbRec.operations;
		dsm->dropManager.curOperation = cbRec.operation;
	}
	else
	{
		callback->dropSiteStatus = XmNO_DROP_SITE;
	}

	if (dsm->dropManager.notifyProc)
	{
		(*(dsm->dropManager.notifyProc))
			((Widget)dsm, dsm->dropManager.client_data,
			(XtPointer)callback);
	}
}

static void
PutDSToStream(
        XmDropSiteManagerObject dsm,
        XmDSInfo dsInfo,
        Boolean last,
        XtPointer dataPtr )
{
	static XmRegion tmpRegion = NULL;
	unsigned char dsType = 0, tType = 0;
	unsigned char unitType = XmPIXELS;
	Position wX, wY;
	Widget w = GetDSWidget(dsInfo);
	Dimension bw = XtBorderWidth(w);
	XmICCDropSiteInfoStruct iccInfo;
	Arg args[30];
	int n;

	_XmProcessLock();
	if (tmpRegion == NULL)
	{
		tmpRegion = _XmRegionCreate();
	}
	_XmProcessUnlock();

	/*
	 * Clear out the info.  This is especially important in the cases
	 * that the widget does not define resources all of the required
	 * animation resources.
	 */
	bzero(((void *) &iccInfo), sizeof(iccInfo));

	if (last)
		tType |= XmDSM_T_CLOSE;
	else
		tType &= ~ XmDSM_T_CLOSE;


	if (GetDSLeaf(dsInfo) || (!GetDSNumChildren(dsInfo)))
		dsType |= XmDSM_DS_LEAF;
	else
		dsType &= ~ XmDSM_DS_LEAF;

	if (GetDSInternal(dsInfo))
		dsType |= XmDSM_DS_INTERNAL;
	else
		dsType &= ~ XmDSM_DS_INTERNAL;

	if (GetDSHasRegion(dsInfo))
		dsType |= XmDSM_DS_HAS_REGION;
	else
		dsType &= ~ XmDSM_DS_HAS_REGION;

	/*
	 * The local drop site tree is always kept in widget relative
	 * coordinates.  We have to put shell relative coordinates on
	 * the wire however, so we to a copy and translate into a tmp
	 * region.
	 */
	XtTranslateCoords(w, 0, 0, &wX, &wY);

	if (GetDSHasRegion(dsInfo))
	{
		_XmProcessLock();
		_XmRegionUnion(GetDSRegion(dsInfo), GetDSRegion(dsInfo),
			tmpRegion);
		_XmProcessUnlock();
	}
	else
	{
		XRectangle rect;

		rect.x = rect.y = -bw;
		rect.width = XtWidth(w) + (2 * bw);
		rect.height = XtHeight(w) + (2 * bw);

		_XmProcessLock();
		_XmRegionClear(tmpRegion);
		_XmRegionUnionRectWithRegion(&rect, tmpRegion, tmpRegion);
		_XmProcessUnlock();
	}

	_XmProcessLock();
	_XmRegionOffset(tmpRegion, (wX - dsm->dropManager.rootX),
		(wY - dsm->dropManager.rootY));
	_XmProcessUnlock();

	/*
	 * We need to pull up the relevant visual information from
	 * the widget so it will be available for correct animation
	 * by a non-local peregister initiator.
	 */
	iccInfo.header.dropType = dsType;
	iccInfo.header.dropActivity = GetDSActivity(dsInfo);
	iccInfo.header.traversalType = tType;
	iccInfo.header.animationStyle = GetDSAnimationStyle(dsInfo);
	iccInfo.header.operations = GetDSOperations(dsInfo);
	iccInfo.header.importTargetsID = GetDSImportTargetsID(dsInfo);
	_XmProcessLock();
	iccInfo.header.region = tmpRegion;
	_XmProcessUnlock();

	/*
	 * We need to retrieve information from the widget. XtGetValues is
	 * too slow, so retrieve the information directly from the widget
	 * instance.
	 *
	 * XtGetValues is used for non-Motif widgets, just in case they provide
	 * Motif-style resources.
	 *
	 * (See also XmDropSiteGetActiveVisuals() )
	 */

	if (XmIsPrimitive(w))
	{
	     XmPrimitiveWidget pw= (XmPrimitiveWidget)w;

	    switch(iccInfo.header.animationStyle)
	    {
		case XmDRAG_UNDER_HIGHLIGHT:
		{
			XmICCDropSiteHighlight info =
				(XmICCDropSiteHighlight)  (&iccInfo);

			info->animation_data.highlightPixmap = XmUNSPECIFIED_PIXMAP;
			if (!GetDSHasRegion(dsInfo))
				info->animation_data.borderWidth =
					pw->core.border_width;
			info->animation_data.highlightThickness =
				pw->primitive.highlight_thickness;
			info->animation_data.highlightColor =
				pw->primitive.highlight_color;
			info->animation_data.highlightPixmap =
				pw->primitive.highlight_pixmap;
			info->animation_data.background =
				pw->core.background_pixel;
		}
		break;
		case XmDRAG_UNDER_SHADOW_IN:
		case XmDRAG_UNDER_SHADOW_OUT:
		{
			XmICCDropSiteShadow info =
				(XmICCDropSiteShadow)  (&iccInfo);

			info->animation_data.topShadowPixmap =
				info->animation_data.bottomShadowPixmap =
				XmUNSPECIFIED_PIXMAP;

			if (!GetDSHasRegion(dsInfo))
				info->animation_data.borderWidth =
					pw->core.border_width;
			info->animation_data.highlightThickness =
				pw->primitive.highlight_thickness;
			info->animation_data.shadowThickness =
				pw->primitive.shadow_thickness;
			info->animation_data.foreground =
				pw->primitive.foreground;
			info->animation_data.topShadowColor =
				pw->primitive.top_shadow_color;
			info->animation_data.topShadowPixmap =
				pw->primitive.top_shadow_pixmap;
			info->animation_data.bottomShadowColor =
				pw->primitive.bottom_shadow_color;
			info->animation_data.bottomShadowPixmap =
				pw->primitive.bottom_shadow_pixmap;
		}
		break;
		case XmDRAG_UNDER_PIXMAP:
		{
			XmICCDropSitePixmap info =
				(XmICCDropSitePixmap)  (&iccInfo);
			XmDSLocalPixmapStyle ps =
				(XmDSLocalPixmapStyle) GetDSLocalAnimationPart(dsInfo);

			info->animation_data.animationPixmapDepth =
				ps->animation_pixmap_depth;
			info->animation_data.animationPixmap =
				ps->animation_pixmap;
			info->animation_data.animationMask = ps->animation_mask;

			if (!GetDSHasRegion(dsInfo))
				info->animation_data.borderWidth =
					pw->core.border_width;
			info->animation_data.highlightThickness =
				pw->primitive.highlight_thickness;
			info->animation_data.shadowThickness =
				pw->primitive.shadow_thickness;
			info->animation_data.foreground =
				pw->primitive.foreground;
			info->animation_data.background =
				pw->core.background_pixel;
		}
		break;
		case XmDRAG_UNDER_NONE:
		{
			XmICCDropSiteNone info =
				(XmICCDropSiteNone)  (&iccInfo);

			if (!GetDSHasRegion(dsInfo))
				info->animation_data.borderWidth = pw->core.border_width;
			else
				info->animation_data.borderWidth = 0;
		}
		default:
		{
			/*EMPTY*/
		}
		break;
	    }
	}
	else if (XmIsManager(w) || XmIsGadget(w))
	{
	    XmManagerWidget mw;
            XmGadget g;
            Boolean is_gadget;

            if (XmIsGadget(w)) {
	       mw = (XmManagerWidget) XtParent(w);
	       g = (XmGadget) w;
               is_gadget = True;
            } else {
	       mw = (XmManagerWidget) w;
	       g = NULL;
               is_gadget = False;
            }

	    switch(iccInfo.header.animationStyle)
	    {
		case XmDRAG_UNDER_HIGHLIGHT:
		{
			XmICCDropSiteHighlight info =
				(XmICCDropSiteHighlight)  (&iccInfo);

			info->animation_data.highlightPixmap = XmUNSPECIFIED_PIXMAP;
			if (!GetDSHasRegion(dsInfo))
			{
                           if (is_gadget)
			      info->animation_data.borderWidth =
					w->core.border_width;
                           else
			      info->animation_data.borderWidth =
					mw->core.border_width;
                        }

		       /* Temporary hack until we support full defaulting */
		        info->animation_data.highlightThickness = 1;
			info->animation_data.highlightColor =
				mw->manager.highlight_color;
			info->animation_data.highlightPixmap =
				mw->manager.highlight_pixmap;
			info->animation_data.background =
				mw->core.background_pixel;
		}
		break;
		case XmDRAG_UNDER_SHADOW_IN:
		case XmDRAG_UNDER_SHADOW_OUT:
		{
			XmICCDropSiteShadow info =
				(XmICCDropSiteShadow)  (&iccInfo);

			info->animation_data.topShadowPixmap =
				info->animation_data.bottomShadowPixmap =
				XmUNSPECIFIED_PIXMAP;

                        if (is_gadget)
			{
			   if (!GetDSHasRegion(dsInfo))
				info->animation_data.borderWidth =
					w->core.border_width;

			   info->animation_data.shadowThickness =
				   g->gadget.shadow_thickness;
                        }
			else
			{
			   if (!GetDSHasRegion(dsInfo))
				info->animation_data.borderWidth =
					mw->core.border_width;

			   info->animation_data.shadowThickness =
				   mw->manager.shadow_thickness;
                        }
			info->animation_data.highlightThickness = 0;
			info->animation_data.foreground =
				mw->manager.foreground;
			info->animation_data.topShadowColor =
				mw->manager.top_shadow_color;
			info->animation_data.topShadowPixmap =
				mw->manager.top_shadow_pixmap;
			info->animation_data.bottomShadowColor =
				mw->manager.bottom_shadow_color;
			info->animation_data.bottomShadowPixmap =
				mw->manager.bottom_shadow_pixmap;
		}
		break;
		case XmDRAG_UNDER_PIXMAP:
		{
			XmICCDropSitePixmap info =
				(XmICCDropSitePixmap)  (&iccInfo);
			XmDSLocalPixmapStyle ps =
				(XmDSLocalPixmapStyle) GetDSLocalAnimationPart(dsInfo);

			info->animation_data.animationPixmapDepth =
				ps->animation_pixmap_depth;
			info->animation_data.animationPixmap =
				ps->animation_pixmap;
			info->animation_data.animationMask = ps->animation_mask;

                        if (is_gadget)
			{
			   if (!GetDSHasRegion(dsInfo))
				info->animation_data.borderWidth =
					w->core.border_width;

			   info->animation_data.shadowThickness =
				   g->gadget.shadow_thickness;
                        }
			else
			{
			   if (!GetDSHasRegion(dsInfo))
				info->animation_data.borderWidth =
					mw->core.border_width;

			   info->animation_data.shadowThickness =
				   mw->manager.shadow_thickness;
                        }
			info->animation_data.highlightThickness = 0;
			info->animation_data.foreground =
				mw->manager.foreground;
			info->animation_data.background =
				mw->core.background_pixel;
		}
		break;
		case XmDRAG_UNDER_NONE:
		{
			XmICCDropSiteNone info =
				(XmICCDropSiteNone)  (&iccInfo);

			if (!GetDSHasRegion(dsInfo))
			{
                           if (is_gadget)
			      info->animation_data.borderWidth =
					w->core.border_width;
                           else
			      info->animation_data.borderWidth =
					mw->core.border_width;
                        } else
			   info->animation_data.borderWidth = 0;
		}
		default:
		{
			/*EMPTY*/
		}
		break;
	    }
	}
	else /* non-Motif subclass */
	{
		n = 0;
		XtSetArg(args[n], XmNunitType, &unitType); n++;
		XtGetValues(w, args, n);

		if (unitType != XmPIXELS) { /* we need values in pixels */
		    n = 0;
		    XtSetArg(args[n], XmNunitType, XmPIXELS); n++;
		    XtSetValues(w, args, n);
		}

	 	switch(iccInfo.header.animationStyle)
	 	{
		    case XmDRAG_UNDER_HIGHLIGHT:
		    {
			XmICCDropSiteHighlight info =
				(XmICCDropSiteHighlight)  (&iccInfo);

			/*
			 * Pre-load a sane pixmap default in case the
			 * widget doesn't have a pixmap resource.
			 */
			info->animation_data.highlightPixmap = XmUNSPECIFIED_PIXMAP;

			n = 0;
			if (!GetDSHasRegion(dsInfo))
			{
				XtSetArg(args[n], XmNborderWidth,
					&(info->animation_data.borderWidth)); n++;
			}
			XtSetArg(args[n], XmNhighlightThickness,
				&(info->animation_data.highlightThickness)); n++;
			XtSetArg(args[n], XmNbackground,
				&(info->animation_data.background)); n++;
			XtSetArg(args[n], XmNhighlightColor,
				&(info->animation_data.highlightColor)); n++;
			XtSetArg(args[n], XmNhighlightPixmap,
				&(info->animation_data.highlightPixmap)); n++;
			XtGetValues(w, args, n);
		    }
		    break;
		    case XmDRAG_UNDER_SHADOW_IN:
		    case XmDRAG_UNDER_SHADOW_OUT:
		    {
			XmICCDropSiteShadow info =
				(XmICCDropSiteShadow)  (&iccInfo);

			/* Pre-load some sane pixmap defaults */
			info->animation_data.topShadowPixmap =
				info->animation_data.bottomShadowPixmap =
				XmUNSPECIFIED_PIXMAP;

			n = 0;
			if (!GetDSHasRegion(dsInfo))
			{
				XtSetArg(args[n], XmNborderWidth,
					&(info->animation_data.borderWidth)); n++;
			}
			XtSetArg(args[n], XmNhighlightThickness,
				&(info->animation_data.highlightThickness)); n++;
			XtSetArg(args[n], XmNshadowThickness,
				&(info->animation_data.shadowThickness)); n++;
			XtSetArg(args[n], XmNforeground,
				&(info->animation_data.foreground)); n++;
			XtSetArg(args[n], XmNtopShadowColor,
				&(info->animation_data.topShadowColor)); n++;
			XtSetArg(args[n], XmNbottomShadowColor,
				&(info->animation_data.bottomShadowColor)); n++;
			XtSetArg(args[n], XmNtopShadowPixmap,
				&(info->animation_data.topShadowPixmap)); n++;
			XtSetArg(args[n], XmNbottomShadowPixmap,
				&(info->animation_data.bottomShadowPixmap)); n++;
			XtGetValues(w, args, n);
		    }
		    break;
		    case XmDRAG_UNDER_PIXMAP:
		    {
			XmICCDropSitePixmap info =
				(XmICCDropSitePixmap)  (&iccInfo);
			XmDSLocalPixmapStyle ps =
				(XmDSLocalPixmapStyle) GetDSLocalAnimationPart(dsInfo);

			info->animation_data.animationPixmapDepth =
				ps->animation_pixmap_depth;
			info->animation_data.animationPixmap =
				ps->animation_pixmap;
			info->animation_data.animationMask = ps->animation_mask;

			n = 0;
			if (!GetDSHasRegion(dsInfo))
			{
				XtSetArg(args[n], XmNborderWidth,
					&(info->animation_data.borderWidth)); n++;
			}
			XtSetArg(args[n], XmNhighlightThickness,
				&(info->animation_data.highlightThickness)); n++;
			XtSetArg(args[n], XmNshadowThickness,
				&(info->animation_data.shadowThickness)); n++;
			XtSetArg(args[n], XmNforeground,
				&(info->animation_data.foreground)); n++;
			XtSetArg(args[n], XmNbackground,
				&(info->animation_data.background)); n++;
			XtGetValues(w, args, n);
		}
		    break;
		    case XmDRAG_UNDER_NONE:
		    {
			XmICCDropSiteNone info =
				(XmICCDropSiteNone)  (&iccInfo);

			if (!GetDSHasRegion(dsInfo))
			{
				n = 0;
				XtSetArg(args[n], XmNborderWidth,
					&(info->animation_data.borderWidth)); n++;
				XtGetValues(w, args, n);
			}
			else
				info->animation_data.borderWidth = 0;
		    }
		    default:
		    {
			/*EMPTY*/
		    }
		    break;

		}

		if (unitType != XmPIXELS) {
		    n = 0;
		    XtSetArg(args[n], XmNunitType, unitType); n++;
		    XtSetValues(w, args, n);
		}
	}
	_XmWriteDSToStream(dsm, dataPtr, &iccInfo);
}

static void
GetDSFromDSM(
        XmDropSiteManagerObject dsm,
        XmDSInfo parentInfo,
        Boolean last,
        XtPointer dataPtr )
{
	XmDSInfo child;
	int i;

	PutDSToStream(dsm, parentInfo, last, dataPtr);

	last = False;
	for (i = 0; i < (int)GetDSNumChildren(parentInfo); i++)
	{
		if ((i + 1) == GetDSNumChildren(parentInfo))
			last = True;

		child = (XmDSInfo) GetDSChild(parentInfo, i);
		if (!GetDSLeaf(child))
			GetDSFromDSM(dsm, child, last, dataPtr);
		else
			PutDSToStream(dsm, child, last, dataPtr);
	}
}

static int
GetTreeFromDSM(
        XmDropSiteManagerObject dsm,
        Widget shell,
        XtPointer dataPtr )
{
	XmDSInfo root = (XmDSInfo) DSMWidgetToInfo(dsm, shell);
	Position shellX, shellY, savX, savY;

	if (root == NULL)
		return(0);
	XtTranslateCoords(shell, 0, 0, &shellX, &shellY);

	/* Save current */
	savX = dsm->dropManager.rootX;
	savY = dsm->dropManager.rootY;

	dsm->dropManager.rootX = shellX;
	dsm->dropManager.rootY = shellY;

	DSMSyncTree(dsm, shell);
	GetDSFromDSM(dsm, root, True, dataPtr);

	dsm->dropManager.rootX = savX;
	dsm->dropManager.rootY = savY;

	return(CountDropSites(root));
}

static XmDSInfo
GetDSFromStream(
        XmDropSiteManagerObject dsm,
        XtPointer dataPtr,
        Boolean *close,
        unsigned char *type )
{
	XmDSInfo info;
	XmICCDropSiteInfoStruct iccInfo;
	size_t size;

	_XmReadDSFromStream(dsm, dataPtr, &iccInfo);

	switch(iccInfo.header.animationStyle)
	  {
	  case XmDRAG_UNDER_HIGHLIGHT:
	    if (iccInfo.header.dropType & XmDSM_DS_LEAF)
	      size = sizeof(XmDSRemoteHighlightLeafRec);
	    else
	      size = sizeof(XmDSRemoteHighlightNodeRec);
	    break;
	  case XmDRAG_UNDER_SHADOW_IN:
	  case XmDRAG_UNDER_SHADOW_OUT:
	    if (iccInfo.header.dropType & XmDSM_DS_LEAF)
	      size = sizeof(XmDSRemoteShadowLeafRec);
	    else
	      size = sizeof(XmDSRemoteShadowNodeRec);
	    break;
	  case XmDRAG_UNDER_PIXMAP:
	    if (iccInfo.header.dropType & XmDSM_DS_LEAF)
	      size = sizeof(XmDSRemotePixmapLeafRec);
	    else
	      size = sizeof(XmDSRemotePixmapNodeRec);
	    break;
	  case XmDRAG_UNDER_NONE:
	    if (iccInfo.header.dropType & XmDSM_DS_LEAF)
	      size = sizeof(XmDSRemoteNoneLeafRec);
	    else
	      size = sizeof(XmDSRemoteNoneNodeRec);
	    break;
	  default:
	    if (iccInfo.header.dropType & XmDSM_DS_LEAF)
	      size = sizeof(XmDSRemoteNoneLeafRec);
	    else
	      size = sizeof(XmDSRemoteNoneNodeRec);
	    break;
	  }

	info = (XmDSInfo) XtCalloc(1, size);

	/* Load the Status fields */

	SetDSRemote(info, True);

	if (iccInfo.header.dropType & XmDSM_DS_LEAF)
	{
		SetDSLeaf(info, True);
		SetDSType(info, XmDROP_SITE_SIMPLE);
	}
	else
	{
		SetDSLeaf(info, False);
		SetDSType(info, XmDROP_SITE_COMPOSITE);
	}

	SetDSAnimationStyle(info, iccInfo.header.animationStyle);

	if (iccInfo.header.dropType & XmDSM_DS_INTERNAL)
		SetDSInternal(info, True);
	else
		SetDSInternal(info, False);

	if (iccInfo.header.dropType & XmDSM_DS_HAS_REGION)
		SetDSHasRegion(info, True);
	else
		SetDSHasRegion(info, False);

	SetDSActivity(info, iccInfo.header.dropActivity);
	SetDSImportTargetsID(info, iccInfo.header.importTargetsID);
	SetDSOperations(info, iccInfo.header.operations);
	SetDSRegion(info, iccInfo.header.region);

	/* Load the animation data */
	switch(GetDSAnimationStyle(info))
	{
		case XmDRAG_UNDER_HIGHLIGHT:
		{
			XmDSRemoteHighlightStyle hs =
				(XmDSRemoteHighlightStyle)
					GetDSRemoteAnimationPart(info);
			XmICCDropSiteHighlight hi =
				(XmICCDropSiteHighlight) (&iccInfo);

			hs->highlight_color = hi->animation_data.highlightColor;
			hs->highlight_pixmap =
				hi->animation_data.highlightPixmap;
			hs->background = hi->animation_data.background;
			hs->highlight_thickness =
				hi->animation_data.highlightThickness;
			hs->border_width =
				hi->animation_data.borderWidth;
		}
		break;
		case XmDRAG_UNDER_SHADOW_IN:
		case XmDRAG_UNDER_SHADOW_OUT:
		{
			XmDSRemoteShadowStyle ss =
				(XmDSRemoteShadowStyle) GetDSRemoteAnimationPart(info);
			XmICCDropSiteShadow si =
				(XmICCDropSiteShadow) (&iccInfo);

			ss->top_shadow_color =
				si->animation_data.topShadowColor;
			ss->top_shadow_pixmap =
				si->animation_data.topShadowPixmap;
			ss->bottom_shadow_color =
				si->animation_data.bottomShadowColor;
			ss->bottom_shadow_pixmap =
				si->animation_data.bottomShadowPixmap;
			ss->foreground = si->animation_data.foreground;
			ss->shadow_thickness =
				si->animation_data.shadowThickness;
			ss->highlight_thickness =
				si->animation_data.highlightThickness;
			ss->border_width = si->animation_data.borderWidth;
		}
		break;
		case XmDRAG_UNDER_PIXMAP:
		{
			XmDSRemotePixmapStyle ps =
				(XmDSRemotePixmapStyle) GetDSRemoteAnimationPart(info);
			XmICCDropSitePixmap pi =
				(XmICCDropSitePixmap) (&iccInfo);

			ps->animation_pixmap =
				pi->animation_data.animationPixmap;
			ps->animation_pixmap_depth =
				pi->animation_data.animationPixmapDepth;
			ps->animation_mask = pi->animation_data.animationMask;
			ps->background = pi->animation_data.background;
			ps->foreground = pi->animation_data.foreground;
			ps->shadow_thickness =
				pi->animation_data.shadowThickness;
			ps->highlight_thickness =
				pi->animation_data.highlightThickness;
			ps->border_width = pi->animation_data.borderWidth;
		}
		break;
		case XmDRAG_UNDER_NONE:
		{
			XmDSRemoteNoneStyle ns =
				(XmDSRemoteNoneStyle) GetDSRemoteAnimationPart(info);
			XmICCDropSiteNone ni =
				(XmICCDropSiteNone) (&iccInfo);

			ns->border_width = ni->animation_data.borderWidth;
		}
		break;
		default:
		break;
	}

	*close = (iccInfo.header.traversalType & XmDSM_T_CLOSE);
	*type = iccInfo.header.dropType;
	return(info);
}

static void
GetNextDS(
        XmDropSiteManagerObject dsm,
        XmDSInfo parentInfo,
        XtPointer dataPtr )
{
	Boolean close = TRUE;
	unsigned char type;
	XmDSInfo new_w = GetDSFromStream(dsm, dataPtr, &close, &type);

	while (!close)
	{
		AddDSChild(parentInfo, new_w, GetDSNumChildren(parentInfo));
		if (! (type & XmDSM_DS_LEAF))
			GetNextDS(dsm, new_w, dataPtr);
		new_w = GetDSFromStream(dsm, dataPtr, &close, &type);
	}

	AddDSChild(parentInfo, new_w, GetDSNumChildren(parentInfo));
	if (! (type & XmDSM_DS_LEAF))
		GetNextDS(dsm, new_w, dataPtr);
}



static XmDSInfo
ReadTree(
        XmDropSiteManagerObject dsm,
        XtPointer dataPtr )
{
	Boolean junkb;
	unsigned char junkc;

	XmDSInfo root = GetDSFromStream(dsm, dataPtr, &junkb, &junkc);
	SetDSShell(root, True);
	GetNextDS(dsm, root, dataPtr);
	return root;
}


static void
FreeDSTree(
        XmDSInfo tree )
{
	int i;
	XmDSInfo child;

	if (!GetDSLeaf(tree))
		for (i = 0; i < (int)GetDSNumChildren(tree); i++)
		{
			child = (XmDSInfo) GetDSChild(tree, i);
			FreeDSTree(child);
		}
	DestroyDSInfo(tree, True);
}

static void
ChangeRoot(
        XmDropSiteManagerObject dsm,
        XtPointer clientData,
        XtPointer callData )
{
	XmDragTopLevelClientData cd =
		(XmDragTopLevelClientData) clientData;
	XmTopLevelEnterCallback callback =
		(XmTopLevelEnterCallback) callData;
	Widget		newRoot = cd->destShell;
	XtPointer	dataPtr = cd->iccInfo;

	dsm->dropManager.curTime = callback->timeStamp;

	if (callback->reason == XmCR_TOP_LEVEL_ENTER)
	{
		/*
		 * We assume that the drag context will not change without
		 * a call to change the root.
		 */
		dsm->dropManager.curDragContext = (Widget) XmGetDragContext(
			(Widget)dsm, callback->timeStamp);

		if (newRoot)
		{
			dsm->dropManager.dsRoot = DSMWidgetToInfo(dsm, newRoot);
			/*
			 * Do we need to do anything for prereg emulation of dyn?
			 */
		}
		else
		{
			dsm->dropManager.dsRoot =
				(XtPointer) ReadTree(dsm, dataPtr);
		}

		dsm->dropManager.rootX = cd->xOrigin;
		dsm->dropManager.rootY = cd->yOrigin;
		dsm->dropManager.rootW = cd->width;
		dsm->dropManager.rootH = cd->height;
	}
	else if (dsm->dropManager.dsRoot)/* XmCR_TOP_LEVEL_LEAVE */
	{
		if (dsm->dropManager.curInfo != NULL)
		{
			XmDragMotionCallbackStruct cbRec ;
			XmDragMotionClientDataStruct cdRec ;
			unsigned char style = _XmGetActiveProtocolStyle(
				dsm->dropManager.curDragContext);

			/* Fake out a motion message from the DragC */
			cbRec.reason = XmCR_DROP_SITE_LEAVE;
			cbRec.event = callback->event;
			cbRec.timeStamp = callback->timeStamp;
			cbRec.x = dsm->dropManager.curX;
			cbRec.y = dsm->dropManager.curY;

			/* These fields are irrelevant on a leave */
			cbRec.operations = cbRec.operation = 0;
			cbRec.dropSiteStatus = 0;

			/* Need these too */
			cdRec.window = cd->window;
			cdRec.dragOver = cd->dragOver;

			HandleLeave(dsm, &cdRec, &cbRec,
				    (XmDSInfo) dsm->dropManager.curInfo,
				    style, False);

			dsm->dropManager.curInfo = NULL;
		}

		if (GetDSRemote((XmDSInfo)(dsm->dropManager.dsRoot)))
			FreeDSTree((XmDSInfo)dsm->dropManager.dsRoot);

		/* Invalidate the root--force errors to show themselves */
		dsm->dropManager.curDragContext = NULL;
		dsm->dropManager.dsRoot = (XtPointer) NULL;
		dsm->dropManager.rootX = (Position) -1;
		dsm->dropManager.rootY = (Position) -1;
		dsm->dropManager.rootW = 0;
		dsm->dropManager.rootH = 0;
	}
}

static int
CountDropSites(
        XmDSInfo info )
{
	int i;
	XmDSInfo child;
	int acc = 1;

	if (!GetDSLeaf(info))
	{
		for (i = 0; i < (int)GetDSNumChildren(info); i++)
		{
			child = (XmDSInfo) GetDSChild(info, i);
			acc += CountDropSites(child);
		}
	}

	return(acc);
}

/* This short resource list is used to initialize just the
   activity member. */
static XtResource mini_resources[] = {
	{   XmNdropSiteActivity, XmCDropSiteActivity, XmRDropSiteActivity,
		sizeof(unsigned char),
		XtOffsetOf( struct _XmDSFullInfoRec, activity),
		XmRImmediate, (XtPointer) XmDROP_SITE_ACTIVE
	},
};

static void
CreateInfo(
        XmDropSiteManagerObject dsm,
        Widget widget,
        ArgList args,
        Cardinal argCount )
{
	XmDSFullInfoRec fullInfoRec;
	XmDSInfo new_info, prev_info;
	XmRegion region = _XmRegionCreate();
	Widget shell = widget;
	size_t size;

	/* zero out the working info struct */
	bzero((void *)(&fullInfoRec), sizeof(fullInfoRec));

	/* Load that puppy */
	SetDSLeaf(&fullInfoRec, True);
	fullInfoRec.widget = widget;
	XtGetSubresources(widget, &fullInfoRec, NULL, NULL, _XmDSResources,
			  _XmNumDSResources, args, argCount);

	/* Handle ignore first. */
	if (fullInfoRec.activity == XmDROP_SITE_IGNORE) {
	  return;
	}

	DSMStartUpdate(dsm, widget);

	/* Do some input validation */

	if ((fullInfoRec.activity == XmDROP_SITE_ACTIVE) &&
		(fullInfoRec.drop_proc == NULL))
	{
		XmeWarning(widget, MESSAGE4);
	}

	if ((fullInfoRec.animation_style == XmDRAG_UNDER_PIXMAP) &&
		(fullInfoRec.animation_pixmap != XmUNSPECIFIED_PIXMAP) &&
		(fullInfoRec.animation_pixmap_depth == 0))
	{
	    /*
	     * They didn't tell us the depth of the pixmaps.  ask for it. */
	    XmeGetPixmapData(XtScreen(widget), fullInfoRec.animation_pixmap,
			     NULL,
			     (int*)&(fullInfoRec.animation_pixmap_depth),
			     NULL, NULL, NULL, NULL, NULL, NULL);
	}

	if ((fullInfoRec.type == XmDROP_SITE_COMPOSITE) &&
		((fullInfoRec.rectangles != NULL) ||
		(fullInfoRec.num_rectangles != 1)))
	{
		XmeWarning(widget, MESSAGE5);
		fullInfoRec.rectangles = NULL;
		fullInfoRec.num_rectangles = 1;
	}

	/* Handle the region*/
	if (fullInfoRec.rectangles == NULL)
	{
		XRectangle rect;
		Dimension bw = XtBorderWidth(widget);

		rect.x = rect.y = -bw;
		rect.width = XtWidth(widget) + (2 * bw);
		rect.height = XtHeight(widget) + (2 * bw);

		_XmRegionUnionRectWithRegion(&rect, region, region);

		fullInfoRec.region = region;

		/*
		 * Leave HasRegion == 0 indicating that we created this
		 * region for the drop site.
		 */
	}
	else
	{
		int i;
		XRectangle *rects = fullInfoRec.rectangles;

		for (i=0; i < fullInfoRec.num_rectangles; i++)
			_XmRegionUnionRectWithRegion(&(rects[i]), region, region);

		fullInfoRec.region = region;
		fullInfoRec.status.has_region = True;
	}

	XtAddCallback(widget, XmNdestroyCallback, DestroyCallback, dsm);

	while(!XtIsShell(shell))
		shell = XtParent(shell);

	fullInfoRec.import_targets_ID = _XmTargetsToIndex(shell,
		fullInfoRec.import_targets, fullInfoRec.num_import_targets);

	switch(fullInfoRec.animation_style)
	{
		case XmDRAG_UNDER_PIXMAP:
			if (fullInfoRec.type == XmDROP_SITE_COMPOSITE)
			  size = sizeof(XmDSLocalPixmapNodeRec);
			else
			  size = sizeof(XmDSLocalPixmapLeafRec);
		break;
		case XmDRAG_UNDER_HIGHLIGHT:
		case XmDRAG_UNDER_SHADOW_IN:
		case XmDRAG_UNDER_SHADOW_OUT:
		case XmDRAG_UNDER_NONE:
		default:
			if (fullInfoRec.type == XmDROP_SITE_COMPOSITE)
			  size = sizeof(XmDSLocalNoneNodeRec);
			else
			  size = sizeof(XmDSLocalNoneLeafRec);
		break;
	}

	new_info = (XmDSInfo) XtCalloc(1, size);

	CopyFullIntoVariant(&fullInfoRec, new_info);

	if ((prev_info = (XmDSInfo) DSMWidgetToInfo(dsm, widget)) == NULL)
	{
		DSMRegisterInfo(dsm, widget, (XtPointer) new_info);
	}
	else
	{
		if (GetDSInternal(prev_info))
		{
		  /*
		   * They are registering a widget for which we already had
		   * to register internally.  The only types of widgets which
		   * we register internally are of type
		   * XmDROP_SITE_COMPOSITE with children!  This means that
		   * they are trying to create their drop sites out of order
		   * (parents must be registered before their children).
		   */
		  XmeWarning(widget, MESSAGE6);
		}
		else
		{
		  XmeWarning(widget, MESSAGE7);
		}

		DestroyDSInfo(new_info, True);
		return;
	}

	DSMInsertInfo(dsm, (XtPointer) new_info, NULL);

	DSMEndUpdate(dsm, widget);
}


static void
CopyVariantIntoFull(
        XmDropSiteManagerObject dsm,
        XmDSInfo variant,
        XmDSFullInfo full_info )
{
	Widget shell;
	Atom *targets;
	Cardinal num_targets;
	long num_rects;
	XRectangle *rects;
int index;

	if (GetDSRemote(variant))
		shell = XtParent(dsm);
	else
		shell = GetDSWidget(variant);

	while (!XtIsShell(shell))
		shell = XtParent(shell);

	/*
	 * Clear the full info back to the default (kind of) state.
	 */
	bzero((void *)(full_info), sizeof(XmDSFullInfoRec));
	full_info->animation_pixmap = XmUNSPECIFIED_PIXMAP;
	full_info->animation_mask = XmUNSPECIFIED_PIXMAP;

	/* Structure copy the status stuff across */
	full_info->status = variant->status;

	full_info->parent = GetDSParent(variant);
	full_info->import_targets_ID = GetDSImportTargetsID(variant);
	full_info->operations = GetDSOperations(variant);
	full_info->region = GetDSRegion(variant);
	full_info->drag_proc = GetDSDragProc(variant);
	full_info->drop_proc = GetDSDropProc(variant);
 	full_info->client_data = GetDSClientData(variant);

	full_info->widget = GetDSWidget(variant);

	full_info->type = GetDSType(variant);
	full_info->animation_style = GetDSAnimationStyle(variant);
	full_info->activity = GetDSActivity(variant);
	index = GetDSImportTargetsID(variant);
	if (index)
	{
		num_targets = _XmIndexToTargets(shell,
			index, &targets);
		full_info->num_import_targets = num_targets;
		full_info->import_targets = targets;
	}
	else
	{
		full_info->num_import_targets = 0;
		full_info->import_targets = NULL;
	}


	_XmRegionGetRectangles(GetDSRegion(variant), &rects, &num_rects);
	full_info->rectangles = rects;
	full_info->num_rectangles = (Cardinal) num_rects;

	if (GetDSRemote(variant))
	{
		switch(GetDSAnimationStyle(variant))
		{
			case XmDRAG_UNDER_HIGHLIGHT:
			{
				XmDSRemoteHighlightStyle hs =
					(XmDSRemoteHighlightStyle)
						GetDSRemoteAnimationPart(variant);

				full_info->highlight_color = hs->highlight_color;
				full_info->highlight_pixmap = hs->highlight_pixmap;
				full_info->background = hs->background;
				full_info->highlight_thickness =
					hs->highlight_thickness;
				full_info->border_width = hs->border_width;
			}
			break;
			case XmDRAG_UNDER_SHADOW_IN:
			case XmDRAG_UNDER_SHADOW_OUT:
			{
				XmDSRemoteShadowStyle ss =
					(XmDSRemoteShadowStyle)
						GetDSRemoteAnimationPart(variant);

				full_info->top_shadow_color = ss->top_shadow_color;
				full_info->top_shadow_pixmap = ss->top_shadow_pixmap;
				full_info->bottom_shadow_color =
					ss->bottom_shadow_color;
				full_info->bottom_shadow_pixmap =
					ss->bottom_shadow_pixmap;
				full_info->foreground = ss->foreground;
				full_info->shadow_thickness = ss->shadow_thickness;
				full_info->highlight_thickness = ss->highlight_thickness;
				full_info->border_width = ss->border_width;
			}
			break;
			case XmDRAG_UNDER_PIXMAP:
			{
				XmDSRemotePixmapStyle ps =
					(XmDSRemotePixmapStyle)
						GetDSRemoteAnimationPart(variant);

				full_info->animation_pixmap = ps->animation_pixmap;
				full_info->animation_pixmap_depth =
					ps->animation_pixmap_depth;
				full_info->animation_mask = ps->animation_mask;
				full_info->background = ps->background;
				full_info->foreground = ps->foreground;
				full_info->shadow_thickness = ps->shadow_thickness;
				full_info->highlight_thickness =
					ps->highlight_thickness;
				full_info->border_width = ps->border_width;
			}
			break;
			case XmDRAG_UNDER_NONE:
			default:
			break;
		}
	}
	else
	{
		switch(GetDSAnimationStyle(variant))
		{
			case XmDRAG_UNDER_HIGHLIGHT:
			break;
			case XmDRAG_UNDER_SHADOW_IN:
			case XmDRAG_UNDER_SHADOW_OUT:
			break;
			case XmDRAG_UNDER_PIXMAP:
			{
				XmDSLocalPixmapStyle ps =
					(XmDSLocalPixmapStyle)
						GetDSLocalAnimationPart(variant);

				full_info->animation_pixmap = ps->animation_pixmap;
				full_info->animation_pixmap_depth =
					ps->animation_pixmap_depth;
				full_info->animation_mask = ps->animation_mask;
			}
			break;
			case XmDRAG_UNDER_NONE:
			default:
			break;
		}
	}
}


static void
RetrieveInfo(
        XmDropSiteManagerObject dsm,
        Widget widget,
        ArgList args,
        Cardinal argCount )
{
	XmDSFullInfoRec full_info_rec;
	XmDSInfo	info;
	int i;
	Boolean freeRects;

	if (XmIsDragContext(widget))
	{
		if (widget != dsm->dropManager.curDragContext)
			return;
		else
			info = (XmDSInfo) (dsm->dropManager.curInfo);
	}
	else
		info = (XmDSInfo) DSMWidgetToInfo(dsm, widget);

	if (info == NULL)
		return;

	CopyVariantIntoFull(dsm, info, &full_info_rec);

	XtGetSubvalues((XtPointer)(&full_info_rec),
	      (XtResourceList)(_XmDSResources), (Cardinal)(_XmNumDSResources),
	      (ArgList)(args), (Cardinal)(argCount));
	freeRects = True;
	for (i = 0 ; i < argCount; i++) {
	    if (strcmp(args[i].name, "dropRectangles") == 0)
		freeRects = False;
	}
	if (freeRects && full_info_rec.rectangles)
	    XtFree((char *) full_info_rec.rectangles);
}

static void
CopyFullIntoVariant(
        XmDSFullInfo full_info,
        XmDSInfo variant )
{
	/*
	 * This procedure assumes that variant is a variant of Local,
	 * there should be no calls to this procedure with variant of
	 * remote.
	 */
	if (GetDSRemote(full_info))
		return;

	/* Magic internal fields */
	SetDSRemote(variant, GetDSRemote(full_info));
	SetDSLeaf(variant, GetDSLeaf(full_info));
	SetDSShell(variant, GetDSShell(full_info));
	SetDSHasRegion(variant, full_info->status.has_region);

	/* Externally visible fields */
	SetDSAnimationStyle(variant, full_info->animation_style);
	SetDSType(variant, full_info->type);
	SetDSActivity(variant, full_info->activity);

	SetDSImportTargetsID(variant, full_info->import_targets_ID);
	SetDSOperations(variant, full_info->operations);
	SetDSRegion(variant, full_info->region);
	SetDSDragProc(variant, full_info->drag_proc);
	SetDSDropProc(variant, full_info->drop_proc);
 	SetDSClientData(variant, full_info->client_data);
	SetDSWidget(variant, full_info->widget);

	switch(full_info->animation_style)
	{
		case XmDRAG_UNDER_HIGHLIGHT:
		break;
		case XmDRAG_UNDER_SHADOW_IN:
		case XmDRAG_UNDER_SHADOW_OUT:
		break;
		case XmDRAG_UNDER_PIXMAP:
		{
			XmDSLocalPixmapStyle ps =
				(XmDSLocalPixmapStyle) GetDSLocalAnimationPart(variant);

			ps->animation_pixmap = full_info->animation_pixmap;
			ps->animation_pixmap_depth =
				full_info->animation_pixmap_depth;
			ps->animation_mask = full_info->animation_mask;
		}
		break;
		case XmDRAG_UNDER_NONE:
		default:
		break;
	}
}


static void
UpdateInfo(
        XmDropSiteManagerObject dsm,
        Widget widget,
        ArgList args,
        Cardinal argCount )
{
	XmDSFullInfoRec	full_info_rec;
	XmDSFullInfo	full_info = &full_info_rec;
	XmDSInfo	info = (XmDSInfo) DSMWidgetToInfo(dsm, widget);
	unsigned char	type;
	XmRegion	old_region;
	XRectangle	*rects;
	long		num_rects;

	if ((info == NULL) || GetDSInternal(info))
		return;

        rects = NULL;

	DSMStartUpdate(dsm, widget);

	CopyVariantIntoFull(dsm, info, full_info);

	/* Save the type and region in case they try to cheat */
	type = GetDSType(info);
	old_region = GetDSRegion(info);

/* BEGIN OSF Fix CR 5335 */
	/*
	 * Set up the rectangle list stuff.
	 */
	rects = full_info->rectangles;
	num_rects = (long) full_info->num_rectangles;
/* END OSF Fix CR 5335 */

	/* Update the info */
        {
	Atom			*old_import_targets;
	Cardinal		old_num_import_targets;

        old_num_import_targets = full_info->num_import_targets;
        old_import_targets = full_info->import_targets;
	XtSetSubvalues(full_info, _XmDSResources, _XmNumDSResources,
		args, argCount);
        if ( (full_info->num_import_targets != old_num_import_targets) ||
	     (full_info->import_targets != old_import_targets) )
		{
		Widget shell = widget;
		while(!(XtIsShell(shell))) shell = XtParent(shell);
		full_info->import_targets_ID = _XmTargetsToIndex(shell,
			full_info->import_targets, full_info->num_import_targets);
		}
        }

	if (full_info->type != type)
	{
		XmeWarning(widget, MESSAGE8);
		full_info->type = type;
	}

	if ((full_info->rectangles != rects) ||
		(full_info->num_rectangles != num_rects))
	{
		if (type == XmDROP_SITE_SIMPLE)
		{
			int i;
			XmRegion new_region = _XmRegionCreate();

			for (i=0; i < full_info->num_rectangles; i++)
				_XmRegionUnionRectWithRegion(
					&(full_info->rectangles[i]), new_region,
					new_region);

			full_info->region = new_region;
			full_info->status.has_region = True;
			_XmRegionDestroy(old_region);
		}
		else
		{
			XmeWarning(widget, MESSAGE9);
		}
	}

	if ((full_info->animation_style == XmDRAG_UNDER_PIXMAP) &&
		(full_info->animation_pixmap_depth == 0))
	{
	    XmeGetPixmapData(XtScreen(GetDSWidget(info)), full_info->animation_pixmap,
			     NULL,
			     (int*)&(full_info->animation_pixmap_depth),
			     NULL, NULL, NULL, NULL, NULL, NULL);
	}

	/*
	 * If the animation style has changed, we need to change info
	 * into a different variant.
	 */

	if (full_info->animation_style != GetDSAnimationStyle(info))
	{
		XmDSInfo new_info;
		size_t size;

		switch (full_info->animation_style)
		{
			case XmDRAG_UNDER_PIXMAP:
				if (GetDSType(info) == XmDROP_SITE_COMPOSITE)
				  size = sizeof(XmDSLocalPixmapNodeRec);
				else
				  size = sizeof(XmDSLocalPixmapLeafRec);
			break;
			case XmDRAG_UNDER_HIGHLIGHT:
			case XmDRAG_UNDER_SHADOW_IN:
			case XmDRAG_UNDER_SHADOW_OUT:
			case XmDRAG_UNDER_NONE:
			default:
				if (GetDSType(info) == XmDROP_SITE_COMPOSITE)
				  size = sizeof(XmDSLocalNoneNodeRec);
				else
				  size = sizeof(XmDSLocalNoneLeafRec);
			break;
		}

		/* Allocate the new info rec */
		new_info = (XmDSInfo) XtCalloc(1, size);

		CopyFullIntoVariant(full_info, new_info);

		/*
		 * Fix the parent pointers of the children
		 */
		SetDSNumChildren(new_info, GetDSNumChildren(info));
		SetDSChildren(new_info, GetDSChildren(info));

		if ((GetDSType(new_info) == XmDROP_SITE_COMPOSITE) &&
			(GetDSNumChildren(new_info)))
		{
			XmDSInfo child;
			int i;

			for (i=0; i < (int)GetDSNumChildren(new_info); i++)
			{
				child = (XmDSInfo) GetDSChild(new_info, i);
				SetDSParent(child, new_info);
			}
		}

		/* Clear the registered bit on the new one */
		SetDSRegistered(new_info, False);

		/* Remove the old one from the hash table */
		DSMUnregisterInfo(dsm, info);

		/* Replace the old one in the drop site tree */
		ReplaceDSChild(info, new_info);

		/* Destroy the old one, but not anything it points to */
		DestroyDSInfo(info, False);

		/* Register the new one */
		DSMRegisterInfo(dsm, widget, (XtPointer) new_info);
	}
	else
	{
		CopyFullIntoVariant(full_info, info);
	}

	DSMEndUpdate(dsm, widget);

	if (rects!=NULL) XtFree ((char *)rects);
}

static void
StartUpdate(
        XmDropSiteManagerObject dsm,
        Widget refWidget )
{
	Widget shell = refWidget;
	XmDSInfo shellInfo;

	while(!(XtIsShell(shell)))
		shell = XtParent(shell);

	shellInfo = (XmDSInfo) DSMWidgetToInfo(dsm, shell);

	if (shellInfo)
		SetDSUpdateLevel(shellInfo, (GetDSUpdateLevel(shellInfo) + 1));
}

static void
EndUpdate(
        XmDropSiteManagerObject dsm,
        Widget refWidget )
{
  _XmDropSiteUpdateInfo dsupdate, oldupdate;
  Boolean found = False;
  Boolean clean;
  Widget shell;
  XmDSInfo shellInfo;

  dsupdate = dsm -> dropManager.updateInfo;
  clean = (dsupdate == NULL);

  shell = refWidget;

  while(!(XtIsShell(shell)))
    shell = XtParent(shell);

  shellInfo = (XmDSInfo) DSMWidgetToInfo(dsm, shell);

  if (shellInfo == NULL) return;

  if (GetDSUpdateLevel(shellInfo) > 0)
    SetDSUpdateLevel(shellInfo, (GetDSUpdateLevel(shellInfo) - 1));

  if (GetDSUpdateLevel(shellInfo) > 0) return;

  /* Fix CR 7976,  losing track of some updates because of bad
     list manipulation */
  oldupdate = dsupdate;

  /* Really,  keep track of toplevel widgets to be updated */
  while(dsupdate) {
    if (dsupdate -> refWidget == shell) {
      found = True;
      break;
    }
    dsupdate = dsupdate -> next;
  }

  if (! found) {
    /* Queue real end update to a timeout */
    dsupdate = (_XmDropSiteUpdateInfo)
      XtMalloc(sizeof(_XmDropSiteUpdateInfoRec));
    dsupdate -> dsm = dsm;
    dsupdate -> refWidget = shell;
    dsupdate -> next = oldupdate;
    dsm -> dropManager.updateInfo = dsupdate;
  }

  /* We don't add a timeout if the record is already marked for update */
  if (clean) {
    dsm -> dropManager.updateTimeOutId =
      XtAppAddTimeOut(XtWidgetToApplicationContext(shell), 0,
		      _XmIEndUpdate, dsm);
  }
}

void
_XmIEndUpdate(XtPointer client_data, XtIntervalId *interval_id)
{
  XmDropSiteManagerObject dsm = (XmDropSiteManagerObject) client_data;
  _XmDropSiteUpdateInfo dsupdate;
  Widget shell;
  XmDSInfo shellInfo;

  /* Remove timeout if this is a forced update */
  if (dsm -> dropManager.updateTimeOutId) {
    if (interval_id == NULL)
      XtRemoveTimeOut(dsm -> dropManager.updateTimeOutId);
    dsm -> dropManager.updateTimeOutId = 0;
  }

  /* Return if all updates have already happened */
  while(dsm -> dropManager.updateInfo != NULL) {
    dsupdate = (_XmDropSiteUpdateInfo) dsm -> dropManager.updateInfo;
    shell = dsupdate -> refWidget;
    dsm -> dropManager.updateInfo = dsupdate -> next;
    XtFree((char*) dsupdate);

    while(!(XtIsShell(shell)))
      shell = XtParent(shell);

    shellInfo = (XmDSInfo) DSMWidgetToInfo(dsm, shell);

    if (shellInfo && XtIsRealized(shell))
      {
	/* This one's for real */

	if (_XmGetDragProtocolStyle(shell) != XmDRAG_DYNAMIC)
	  {
	    XmDropSiteTreeAddCallbackStruct	outCB;

	    /* Gotta' update that window property. */
	    outCB.reason = XmCR_DROP_SITE_TREE_ADD;
	    outCB.event = NULL;
	    outCB.rootShell = shell;
	    outCB.numDropSites = CountDropSites(shellInfo);
	    outCB.numArgsPerDSHint = 0;

	    if (dsm->dropManager.treeUpdateProc)
	      (dsm->dropManager.treeUpdateProc)
		((Widget) dsm, NULL, (XtPointer) &outCB);
	  }
	else
	  {
	    /* We have to Sync the regions with the widgets */
	    SyncTree(dsm, shell);
	  }
      }
  }
}

static void
DestroyInfo(
        XmDropSiteManagerObject dsm,
        Widget widget )
{
	XmDSInfo info = (XmDSInfo) DSMWidgetToInfo(dsm, widget);

	if (info == NULL)
		return;

	DSMStartUpdate(dsm, widget);

	if (info == (XmDSInfo) (dsm->dropManager.curInfo))
	{
		Widget shell;
		XmDragMotionCallbackStruct cbRec ;
		XmDragMotionClientDataStruct cdRec ;
		unsigned char style = _XmGetActiveProtocolStyle(
			dsm->dropManager.curDragContext);

		/* Fake out a motion message from the DragC */
		cbRec.reason = XmCR_DROP_SITE_LEAVE;
		cbRec.event = NULL;
		cbRec.timeStamp = dsm->dropManager.curTime;
		cbRec.x = dsm->dropManager.curX;
		cbRec.y = dsm->dropManager.curY;

		/* These fields are irrelevant on a leave */
		cbRec.operations = cbRec.operation = 0;
		cbRec.dropSiteStatus = 0;

		/* Need these too */
		shell = GetDSWidget(info);

		while (!XtIsShell(shell))
			shell = XtParent(shell);

		cdRec.window = XtWindow(shell);
		cdRec.dragOver = (Widget)
			(((XmDragContext)(dsm->dropManager.curDragContext))
				->drag.curDragOver);

		HandleLeave(dsm, &cdRec, &cbRec,
			    (XmDSInfo) dsm->dropManager.curInfo, style, False);

		dsm->dropManager.curInfo = NULL;
	}

	while(info != NULL) {
	  DSMRemoveInfo(dsm, (XtPointer) info);
	  DestroyDSInfo(info, True);

	  /* This should be NULL now,  otherwise,  keep removing
	     until done */
	  info = (XmDSInfo) DSMWidgetToInfo(dsm, widget);
	}

	DSMEndUpdate(dsm, widget);
}

static void
SyncDropSiteGeometry(
        XmDropSiteManagerObject dsm,
        XmDSInfo info )
{
	XmDSInfo child;

	if (!GetDSLeaf(info))
	{
		int i;

		for (i = 0; i < (int)GetDSNumChildren(info); i++)
		{
			child = (XmDSInfo) GetDSChild(info, i);
			SyncDropSiteGeometry(dsm, child);
		}
	}

	if (!GetDSHasRegion(info))
	{
		Widget w = GetDSWidget(info);
		XRectangle rect;
		Dimension bw = XtBorderWidth(w);

		/* The region is the object rectangle */

		/* assert(GetDSRegion(info) != NULL)  */

		/* The region comes from the widget */

		rect.x = rect.y = -bw;
		rect.width = XtWidth(w) + (2 * bw);
		rect.height = XtHeight(w) + (2 * bw);

		_XmRegionClear(GetDSRegion(info));
		_XmRegionUnionRectWithRegion(&rect, GetDSRegion(info),
			GetDSRegion(info));
	}
}

static void
SyncTree(
        XmDropSiteManagerObject dsm,
        Widget shell )
{
	XmDSInfo saveRoot;
	XmDSInfo root = (XmDSInfo) DSMWidgetToInfo(dsm, shell);
	Position shellX, shellY, savX, savY;

	if ((root == NULL) || (GetDSRemote(root)))
		return;

	/*
	 * Set things up so that the shell coordinates are trivially
	 * available.
	 */
	saveRoot = (XmDSInfo) dsm->dropManager.dsRoot;
	savX = dsm->dropManager.rootX;
	savY = dsm->dropManager.rootY;

	dsm->dropManager.dsRoot = (XtPointer) root;
	XtTranslateCoords(GetDSWidget(root), 0, 0, &shellX, &shellY);
	dsm->dropManager.rootX = shellX;
	dsm->dropManager.rootY = shellY;

	/* Do the work */
	RemoveAllClippers(dsm, root);
	SyncDropSiteGeometry(dsm, root);
	DetectAndInsertAllClippers(dsm, root);

	/* Restore the DSM */
	dsm->dropManager.dsRoot = (XtPointer) saveRoot;
	dsm->dropManager.rootX = savX;
	dsm->dropManager.rootY = savY;
}

void
_XmSyncDropSiteTree(
        Widget shell )
{
	XmDropSiteManagerObject dsm = (XmDropSiteManagerObject)
		_XmGetDropSiteManagerObject((XmDisplay) XmGetXmDisplay(
			XtDisplayOfObject(shell)));

	DSMSyncTree(dsm, shell);
}

static void
Update(
        XmDropSiteManagerObject dsm,
        XtPointer clientData,
        XtPointer callData )
{
	XmAnyCallbackStruct	*callback = (XmAnyCallbackStruct *)callData;

	switch(callback->reason)
	{
		case XmCR_TOP_LEVEL_ENTER:
		case XmCR_TOP_LEVEL_LEAVE:
			DSMChangeRoot(dsm, clientData, callData);
		break;
		case XmCR_DRAG_MOTION:
			DSMProcessMotion(dsm, clientData, callData);
		break;
		case XmCR_DROP_START:
			DSMProcessDrop(dsm, clientData, callData);
		break;
		case XmCR_OPERATION_CHANGED:
			DSMOperationChanged(dsm, clientData, callData);
		default:
		break;
	}
}

void
_XmDSMUpdate(
        XmDropSiteManagerObject dsm,
        XtPointer clientData,
        XtPointer callData )
{
	DSMUpdate(dsm, clientData, callData);
}


int
_XmDSMGetTreeFromDSM(
        XmDropSiteManagerObject dsm,
        Widget shell,
        XtPointer dataPtr )
{
	return(DSMGetTreeFromDSM(dsm, shell, dataPtr));
}

Boolean
_XmDropSiteShell(
        Widget widget )
{
	XmDropSiteManagerObject dsm = (XmDropSiteManagerObject)
		_XmGetDropSiteManagerObject((XmDisplay) XmGetXmDisplay(
			XtDisplayOfObject(widget)));

	if ((XtIsShell(widget)) && (DSMWidgetToInfo(dsm, widget) != NULL))
		return(True);
	else
		return(False);
}

static Boolean
HasDropSiteDescendant(
	XmDropSiteManagerObject dsm,
	Widget widget )
{
	CompositeWidget cw;
	int i;
	Widget child;

	if (!XtIsComposite(widget))
		return(False);

	cw = (CompositeWidget) widget;
	for (i = 0; i < cw->composite.num_children; i++)
	{
		child = cw->composite.children[i];
		if ((DSMWidgetToInfo(dsm, child) != NULL) ||
			(HasDropSiteDescendant(dsm, child)))
		{
			return(True);
		}
	}

	return(False);
}

Boolean
_XmDropSiteWrapperCandidate(
        Widget widget )
{
	XmDropSiteManagerObject dsm = (XmDropSiteManagerObject)
		_XmGetDropSiteManagerObject((XmDisplay) XmGetXmDisplay(
			XtDisplayOfObject(widget)));
	Widget shell;

	if (widget == NULL)
		return(False);

	if (DSMWidgetToInfo(dsm, widget) != NULL)
		return(True);
	else if (!XtIsComposite(widget))
		return(False);

	/*
	 * Make sure that there might be a drop site somewhere in
	 * this shell before traversing the descendants.
	 */
	shell = widget;
	while (!XtIsShell(shell))
		shell = XtParent(shell);

	if (!_XmDropSiteShell(shell))
		return(False);

	return(HasDropSiteDescendant(dsm, widget));
}

static void
DestroyCallback(
        Widget widget,
        XtPointer client_data,
        XtPointer call_data )
{
	XmDropSiteManagerObject dsm = (XmDropSiteManagerObject) client_data;

	DSMDestroyInfo(dsm, widget);

	/* Force Update */
	_XmIEndUpdate((XtPointer) dsm, (XtIntervalId *) NULL);
}

Boolean
XmDropSiteRegistered(
        Widget widget )
{
    XmDropSiteManagerObject dsm;
    XtPointer info;
    _XmWidgetToAppContext(widget);

    _XmAppLock(app);
    dsm = (XmDropSiteManagerObject)
	   _XmGetDropSiteManagerObject((XmDisplay) XmGetXmDisplay(
	                         XtDisplayOfObject(widget)));
    info = DSMWidgetToInfo(dsm, widget);

    if (!info) {
	_XmAppUnlock(app);
	return False;
    }

    _XmAppUnlock(app);
    return True;
}


void
XmDropSiteRegister(
        Widget widget,
        ArgList args,
        Cardinal argCount )
{
  XmDropSiteManagerObject dsm;
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  dsm = (XmDropSiteManagerObject)
    _XmGetDropSiteManagerObject((XmDisplay) XmGetXmDisplay(
	     XtDisplayOfObject(widget)));
  if (XtIsShell(widget)) {
    XmeWarning(widget, MESSAGE10);
  } else
    DSMCreateInfo(dsm, widget, args, argCount);
  _XmAppUnlock(app);
}

void
XmDropSiteUnregister(
        Widget widget )
{
	XmDropSiteManagerObject dsm;
	_XmWidgetToAppContext(widget);

	_XmAppLock(app);
	dsm = (XmDropSiteManagerObject)
	    _XmGetDropSiteManagerObject((XmDisplay) XmGetXmDisplay(
			XtDisplayOfObject(widget)));
	DSMDestroyInfo(dsm, widget);

	/* Force Update */
	_XmIEndUpdate((XtPointer) dsm, (XtIntervalId *) NULL);
	_XmAppUnlock(app);
}

void
XmDropSiteStartUpdate(
        Widget refWidget )
{
	XmDropSiteManagerObject dsm;
	_XmWidgetToAppContext(refWidget);

	_XmAppLock(app);
	dsm = (XmDropSiteManagerObject)
	    _XmGetDropSiteManagerObject((XmDisplay) XmGetXmDisplay(
			XtDisplayOfObject(refWidget)));
	DSMStartUpdate(dsm, refWidget);
	_XmAppUnlock(app);
}

void
XmDropSiteUpdate(
        Widget enclosingWidget,
        ArgList args,
        Cardinal argCount )
{
	XmDropSiteManagerObject dsm;
	_XmWidgetToAppContext(enclosingWidget);

	_XmAppLock(app);
	dsm = (XmDropSiteManagerObject)
	    _XmGetDropSiteManagerObject((XmDisplay) XmGetXmDisplay(
			XtDisplayOfObject(enclosingWidget)));
	DSMUpdateInfo(dsm, enclosingWidget, args, argCount);
	_XmAppUnlock(app);
}

void
XmDropSiteEndUpdate(
        Widget refWidget )
{
	XmDropSiteManagerObject dsm;
	_XmWidgetToAppContext(refWidget);

	_XmAppLock(app);
	dsm = (XmDropSiteManagerObject)
	    _XmGetDropSiteManagerObject((XmDisplay) XmGetXmDisplay(
			XtDisplayOfObject(refWidget)));

	DSMEndUpdate(dsm, refWidget);
	_XmAppUnlock(app);
}

void
XmDropSiteRetrieve(
        Widget enclosingWidget,
        ArgList args,
        Cardinal argCount )
{
	XmDropSiteManagerObject dsm;
	_XmWidgetToAppContext(enclosingWidget);

	_XmAppLock(app);
	dsm = (XmDropSiteManagerObject)
	    _XmGetDropSiteManagerObject((XmDisplay) XmGetXmDisplay(
			XtDisplayOfObject(enclosingWidget)));

	/* Update if dsm is dirty */
	_XmIEndUpdate((XtPointer) dsm, (XtIntervalId *) NULL);

	DSMRetrieveInfo(dsm, enclosingWidget, args, argCount);
	_XmAppUnlock(app);
}

Status
XmDropSiteQueryStackingOrder(
        Widget widget,
        Widget *parent_rtn,
        Widget **children_rtn,
        Cardinal *num_children_rtn )
{
	XmDropSiteManagerObject dsm;
	XmDSInfo info;
	XmDSInfo parentInfo;
	Cardinal num_visible_children = 0; /* visible to application code */
	int i,j;
	_XmWidgetToAppContext(widget);

	_XmAppLock(app);
	dsm = (XmDropSiteManagerObject)
	    _XmGetDropSiteManagerObject((XmDisplay) XmGetXmDisplay(
			XtDisplayOfObject(widget)));
	info = (XmDSInfo) DSMWidgetToInfo(dsm, widget);

	/* Update if dsm is dirty */
	_XmIEndUpdate((XtPointer) dsm, (XtIntervalId *) NULL);

	if (info == NULL) {
		_XmAppUnlock(app);
		return(0);
	}

	if (!GetDSLeaf(info))
	{
		for (i=0; i < (int)GetDSNumChildren(info); i++)
		{
			XmDSInfo child = (XmDSInfo) GetDSChild(info, i);
			if (!GetDSInternal(child))
				num_visible_children++;
		}

		if (num_visible_children)
		{
			*children_rtn = (Widget *) XtMalloc(sizeof(Widget) *
				num_visible_children);

			/* Remember to reverse the order */
			for (j=0, i=(GetDSNumChildren(info) - 1); i >= 0; i--)
			{
				XmDSInfo child = (XmDSInfo) GetDSChild(info, i);
				if (!GetDSInternal(child))
					(*children_rtn)[j++] = GetDSWidget(child);
			}
			/* assert(j == num_visible_children) */
		}
		else
			*children_rtn = NULL;

		*num_children_rtn = num_visible_children;
	}
	else
	{
		*children_rtn = NULL;
		*num_children_rtn = 0;
	}

	parentInfo = (XmDSInfo) GetDSParent(info);

	if (GetDSInternal(parentInfo))
	{
	  *parent_rtn = NULL;
	  while ((parentInfo = (XmDSInfo) GetDSParent(parentInfo)) != NULL)
	    if (!GetDSInternal(parentInfo))
	      *parent_rtn = GetDSWidget(parentInfo);
	}
	else
		*parent_rtn = GetDSWidget(parentInfo);

	_XmAppUnlock(app);
	return(1);
}

void
XmDropSiteConfigureStackingOrder(
        Widget widget,
        Widget sibling,
        Cardinal stack_mode )
{
	XmDropSiteManagerObject dsm;
	XmDSInfo info;
	XmDSInfo parent;
	_XmWidgetToAppContext(widget);

	if (widget == NULL)
		return;

	_XmAppLock(app);
	dsm = (XmDropSiteManagerObject)
		_XmGetDropSiteManagerObject((XmDisplay) XmGetXmDisplay(
			XtDisplayOfObject(widget)));

	info = (XmDSInfo) DSMWidgetToInfo(dsm, widget);

	if ((widget == sibling) || (info == NULL)) {
		_XmAppUnlock(app);
		return;
	}

	parent = (XmDSInfo) GetDSParent(info);

	if (sibling != NULL)
	{
		XmDSInfo sib = (XmDSInfo) DSMWidgetToInfo(dsm, sibling);
		Cardinal index, sib_index;
		int i;

		if ((sib == NULL) ||
			(((XmDSInfo) GetDSParent(sib)) != parent) ||
			(XtParent(widget) != XtParent(sibling))) {
			_XmAppUnlock(app);
			return;
		}

		index = GetDSChildPosition(parent, info);
		sib_index = GetDSChildPosition(parent, sib);

		switch(stack_mode)
		{
			case XmABOVE:
				if (index > sib_index)
					for(i=index; i > sib_index; i--)
						SwapDSChildren(parent, i, i - 1);
				else
					for (i=index; i < (sib_index - 1); i++)
						SwapDSChildren(parent, i, i + 1);
			break;
			case XmBELOW:
				if (index > sib_index)
					for(i=index; i > (sib_index + 1); i--)
						SwapDSChildren(parent, i, i - 1);
				else
					for (i=index; i < sib_index; i++)
						SwapDSChildren(parent, i, i + 1);
			break;
			default:
				/* do nothing */
			break;
		}
	}
	else
	{
		Cardinal index = GetDSChildPosition(parent, info);
		int i;

		switch(stack_mode)
		{
			case XmABOVE:
				for (i=index; i > 0; i--)
					SwapDSChildren(parent, i, i - 1);
			break;
			case XmBELOW:
				for (i=index; i < (int)(GetDSNumChildren(parent) - 1); i++)
					SwapDSChildren(parent, i, i + 1);
			break;
			default:
				/* do nothing */
			break;
		}
	}
	_XmAppUnlock(app);
}

XmDropSiteVisuals
XmDropSiteGetActiveVisuals(
        Widget widget )
{
	XmDropSiteManagerObject dsm;
	XmDSInfo info;
	XmDropSiteVisuals dsv;
	_XmWidgetToAppContext(widget);

	_XmAppLock(app);
	dsm = (XmDropSiteManagerObject)
	    _XmGetDropSiteManagerObject((XmDisplay) XmGetXmDisplay(
			XtDisplayOfObject(widget)));
	info = (XmDSInfo) dsm->dropManager.curInfo;
	dsv = (XmDropSiteVisuals) XtCalloc(1,
		sizeof(XmDropSiteVisualsRec));

	/* Update if dsm is dirty */
	_XmIEndUpdate((XtPointer) dsm, (XtIntervalId *) NULL);

	if (info == NULL)
	{
		XtFree((char *)dsv);
		_XmAppUnlock(app);
		return(NULL);
	}

	if (!GetDSRemote(info))
	{
		Arg args[30];
		int n;
		Widget w;
                unsigned char unitType;
		w = GetDSWidget(info);

		/*
		 * We need to retrieve information from the widget. XtGetValues is
		 * too slow, so retrieve the information directly from the widget
		 * instance.
		 *
		 * XtGetValues is used for gadgets, since part of the instance
		 * structure will be in the cache and not directly accessable.
		 *
		 * XtGetValues is used for non-Motif widgets, just in case they provide
		 * Motif-style resources.
		 *
		 * (See also PutDSToStream() )
		 */

		if (XmIsPrimitive(w))
		{
			XmPrimitiveWidget pw = (XmPrimitiveWidget)w;

			dsv->background = pw->core.background_pixel;
			dsv->foreground = pw->primitive.foreground;
			dsv->topShadowColor = pw->primitive.top_shadow_color;
			dsv->topShadowPixmap = pw->primitive.top_shadow_pixmap;
			dsv->bottomShadowColor = pw->primitive.bottom_shadow_color;
			dsv->bottomShadowPixmap = pw->primitive.bottom_shadow_pixmap;
			dsv->shadowThickness = pw->primitive.shadow_thickness;
			dsv->highlightColor = pw->primitive.highlight_color;
			dsv->highlightPixmap = pw->primitive.highlight_pixmap;
			dsv->highlightThickness = pw->primitive.highlight_thickness;
			if (!GetDSHasRegion(info))
			    dsv->borderWidth = pw->core.border_width;
			else
			    dsv->borderWidth = 0;
		}
		else if (XmIsManager(w))
		{
			XmManagerWidget mw = (XmManagerWidget)w;

			dsv->background = mw->core.background_pixel;
			dsv->foreground = mw->manager.foreground;
			dsv->topShadowColor = mw->manager.top_shadow_color;
			dsv->topShadowPixmap = mw->manager.top_shadow_pixmap;
			dsv->bottomShadowColor = mw->manager.bottom_shadow_color;
			dsv->bottomShadowPixmap = mw->manager.bottom_shadow_pixmap;
			dsv->shadowThickness = mw->manager.shadow_thickness;
			dsv->highlightColor = mw->manager.highlight_color;
			dsv->highlightPixmap = mw->manager.highlight_pixmap;
			/* Temporary hack, until we support full defaulting */
			if (GetDSAnimationStyle(info) == XmDRAG_UNDER_HIGHLIGHT)
			    dsv->highlightThickness = 1;
			else
			    dsv->highlightThickness = 0;
			if (!GetDSHasRegion(info))
			    dsv->borderWidth = mw->core.border_width;
			else
			    dsv->borderWidth = 0;
		}
		else /* XmGadget or non-Motif subclass */
		{
			n = 0;
			XtSetArg(args[n], XmNunitType, &unitType); n++;
			XtGetValues(w, args, n);

                	if (unitType != XmPIXELS) { /* we need values in pixels */
			   n = 0;
			   XtSetArg(args[n], XmNunitType, XmPIXELS); n++;
			   XtSetValues(w, args, n);
                	}

			n = 0;
			XtSetArg(args[n], XmNbackground, &(dsv->background)); n++;
			XtSetArg(args[n], XmNforeground, &(dsv->foreground)); n++;
			XtSetArg(args[n], XmNtopShadowColor,
				&(dsv->topShadowColor)); n++;
			XtSetArg(args[n], XmNtopShadowPixmap,
				&(dsv->topShadowPixmap)); n++;
			XtSetArg(args[n], XmNbottomShadowColor,
				&(dsv->bottomShadowColor)); n++;
			XtSetArg(args[n], XmNbottomShadowPixmap,
				&(dsv->bottomShadowPixmap)); n++;
			XtSetArg(args[n], XmNshadowThickness,
				&(dsv->shadowThickness)); n++;
			XtSetArg(args[n], XmNhighlightColor,
				&(dsv->highlightColor)); n++;
			XtSetArg(args[n], XmNhighlightPixmap,
				&(dsv->highlightPixmap)); n++;
			XtSetArg(args[n], XmNhighlightThickness,
				&(dsv->highlightThickness)); n++;
			if (!GetDSHasRegion(info))
			{
			    XtSetArg(args[n], XmNborderWidth,
				&(dsv->borderWidth)); n++;
			}
			else
			    dsv->borderWidth = 0;

			XtGetValues(w, args, n);

			if (unitType != XmPIXELS) {
			    n = 0;
			    XtSetArg(args[n], XmNunitType, unitType); n++;
			    XtSetValues(w, args, n);
			}
		}
	}
	else
	{
		switch (GetDSAnimationStyle(info))
		{
			case XmDRAG_UNDER_HIGHLIGHT:
			{
				XmDSRemoteHighlightStyle hs =
					(XmDSRemoteHighlightStyle)
						GetDSRemoteAnimationPart(info);

				dsv->highlightColor = hs->highlight_color;
				dsv->highlightPixmap = hs->highlight_pixmap;
				dsv->background = hs->background;
				dsv->highlightThickness = hs->highlight_thickness;
				dsv->borderWidth = hs->border_width;
			}
			break;
			case XmDRAG_UNDER_SHADOW_IN:
			case XmDRAG_UNDER_SHADOW_OUT:
			{
				XmDSRemoteShadowStyle ss =
					(XmDSRemoteShadowStyle)
						GetDSRemoteAnimationPart(info);

				dsv->topShadowColor = ss->top_shadow_color;
				dsv->topShadowPixmap = ss->top_shadow_pixmap;
				dsv->bottomShadowColor = ss->bottom_shadow_color;
				dsv->bottomShadowPixmap = ss->bottom_shadow_pixmap;
				dsv->foreground = ss->foreground;
				dsv->shadowThickness = ss->shadow_thickness;
				dsv->highlightThickness = ss->highlight_thickness;
				dsv->borderWidth = ss->border_width;
			}
			break;
			case XmDRAG_UNDER_PIXMAP:
			{
				XmDSRemotePixmapStyle ps =
					(XmDSRemotePixmapStyle)
						GetDSRemoteAnimationPart(info);

				dsv->background = ps->background;
				dsv->foreground = ps->foreground;
				dsv->shadowThickness = ps->shadow_thickness;
				dsv->highlightThickness = ps->highlight_thickness;
				dsv->borderWidth = ps->border_width;
			}
			break;
			case XmDRAG_UNDER_NONE:
			default:
			break;
		}
	}
	_XmAppUnlock(app);
	return(dsv);
}

Widget
_XmGetActiveDropSite(
        Widget widget )
{
	XmDropSiteManagerObject dsm = (XmDropSiteManagerObject)
		_XmGetDropSiteManagerObject((XmDisplay) XmGetXmDisplay(
			XtDisplayOfObject(widget)));
	XmDSInfo info = (XmDSInfo) dsm->dropManager.curInfo;

	/* Update if dsm is dirty */
	_XmIEndUpdate((XtPointer) dsm, (XtIntervalId *) NULL);

	if ((!XmIsDragContext(widget)) || (GetDSRemote(info)))
		return(NULL);
	else
		return(GetDSWidget(info));

}

#ifdef DEBUG
/**********************************************************************
 * Debugging code for DropSMgr.  This code will print out the current
 * state of the tree
 **********************************************************************/


static Boolean
PrintHashEntry(XmHashKey key, XtPointer value, XtPointer data)
{
  Widget wid = (Widget) key;

  printf("Widget %p (%s) Info %p\n", wid, XtName(wid), value);

  return(False);
}

static void
PrintTree(XmDSInfo rec, int level)
{
  int i;
  Widget wid = GetDSWidget(rec);
  char *name = "";

  /* Indent for level */
  for(i = 0; i < level; i++) printf("  ");

  if (wid != (Widget) NULL) name = XtName(wid);

  printf("%p (internal %d) - widget %p (%s)\n",
	 rec, GetDSInternal(rec), wid, name);

  if (!GetDSLeaf(rec)) {
    int j;

    for(j = 0; j < GetDSNumChildren(rec); j++)
      PrintTree(GetDSChild(rec, j), level + 1);
  }
}

void
_XmPrintDSTree(XmDropSiteManagerObject dsm, XmDSInfo root)
{
  /* First print all the information records in tree format */
  PrintTree(root, 0);

  /* Now print widget to info hash table */
  _XmMapHashTable(DSTABLE(dsm), PrintHashEntry, NULL);
}
#endif /* DEBUG */
