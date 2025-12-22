/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar
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
static char rcsid[] = "$XConsortium: Desktop.c /main/12 1995/07/14 10:17:30 drk $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <Xm/DesktopP.h>
#include <Xm/BaseClassP.h>

typedef struct _XmDesktopCore {
	CorePart core;
	XmDesktopPart desktop;
} *XmDesktopCore;

typedef struct _XmDesktopCoreClass {
	CoreClassPart      core_class;
	XmDesktopClassPart desktop_class;
} *XmDesktopCoreClass;

/**
 * Here we reconcile the duplicitous nature of XmDesktopObjects,
 * and their questionable parentage.
 *
 * XmScreen is XmDesktopObject-like and descends from Core.
 * XmDesktopObject decends from Object.
 *
 * Core's instance part is much larger than Object's.
 *
 */
static XmDesktopPart *desktop_part(Widget w)
{
	if (!w)
		return NULL;

	if (XtIsSubclass(w, xmDesktopClass))
		return &((XmDesktopObject)w)->desktop;

	if (XtIsSubclass(w, coreWidgetClass))
		return &((XmDesktopCore)w)->desktop;

	return NULL;
}

static XmDesktopClassPart *desktop_class_part(Widget w)
{
	if (!w)
		return NULL;

	if (XtIsSubclass(w, xmDesktopClass))
		return &((XmDesktopObjectClass)XtClass(w))->desktop_class;

	if (XtIsSubclass(w, coreWidgetClass))
		return &((XmDesktopCoreClass)XtClass(w))->desktop_class;

	return NULL;
}

/********    Static Function Declarations    ********/

static void ClassPartInitialize(WidgetClass widgetClass);
static void ResParentDestroyed(Widget resParent, XtPointer closure,
                               XtPointer callData);
static void Destroy(Widget wid);
static void InsertChild(Widget wid);
static void DeleteChild(Widget wid);
static void Initialize(Widget requested_widget, Widget new_widget,
                       ArgList args, Cardinal *num_args);

/********    End Static Function Declarations    ********/

static XtResource resources[] = {
	{
		XmNdesktopParent,
		XmCDesktopParent, XmRWidget, sizeof(Widget),
		XtOffsetOf(struct _XmDesktopRec, desktop.parent),
		XmRImmediate, NULL,
	},
	{
		XmNextensionType,
		XmCExtensionType, XmRExtensionType, sizeof(unsigned char),
		XtOffsetOf(struct _XmDesktopRec, ext.extensionType),
		XmRImmediate, (XtPointer)XmDESKTOP_EXTENSION,
	},
};

externaldef(xmdesktopclassrec) XmDesktopClassRec xmDesktopClassRec = {
{ /* core */
	(WidgetClass)&xmExtClassRec, /* superclass           */
	"Desktop",                   /* class_name           */
	sizeof(XmDesktopRec),        /* size                 */
	NULL,                        /* Class Initializer    */
	ClassPartInitialize,         /* class_part_init      */
	FALSE,                       /* Class init'ed ?      */
	Initialize,                  /* initialize           */
	NULL,                        /* initialize_notify    */
	NULL,                        /* realize              */
	NULL,                        /* actions              */
	0,                           /* num_actions          */
	resources,                   /* resources            */
	XtNumber(resources),         /* resource_count       */
	NULLQUARK,                   /* xrm_class            */
	FALSE,                       /* compress_motion      */
	XtExposeNoCompress,          /* compress_exposure    */
	FALSE,                       /* compress_enterleave  */
	FALSE,                       /* visible_interest     */
	Destroy,                     /* destroy              */
	NULL,                        /* resize               */
	NULL,                        /* expose               */
	NULL,                        /* set_values           */
	NULL,                        /* set_values_hook      */
	NULL,                        /* set_values_almost    */
	NULL,                        /* get_values_hook      */
	NULL,                        /* accept_focus         */
	XtVersion,                   /* intrinsics version   */
	NULL,                        /* callback offsets     */
	NULL,                        /* tm_table             */
	NULL,                        /* query_geometry       */
	NULL,                        /* display_accelerator  */
	NULL                         /* extension            */
},
{ /* ext */
	NULL,                        /* synthetic resources  */
	0,                           /* num syn resources    */
	NULL,                        /* extension            */
},
{ /* desktop */
	NULL,                        /* child_class          */
	InsertChild,                 /* insert_child         */
	DeleteChild,                 /* delete_child         */
	NULL,                        /* extension            */
}};

externaldef(xmdesktopclass) WidgetClass
      xmDesktopClass = (WidgetClass)&xmDesktopClassRec;

static void ClassPartInitialize(WidgetClass wc)
{
	WidgetClass i;
	XmDesktopClassPart *part = NULL, *super = NULL;

	for (i = wc; i; i = i->core_class.superclass) {
		if (i != xmDesktopClass && i != coreWidgetClass && i != objectClass)
			continue;

		part = &((XmDesktopObjectClass)wc)->desktop_class;
		if (wc->core_class.superclass)
			super = &((XmDesktopObjectClass)(wc->core_class.superclass))->desktop_class;
		break;
	}

	/* Nothing to inherit from */
	if (!part || !super)
		return;

	if (part->child_class == XmInheritClass)
		part->child_class = super->child_class;

	if (part->insert_child == XtInheritInsertChild)
		part->insert_child = super->insert_child;

	if (part->delete_child == XtInheritDeleteChild)
		part->delete_child = super->delete_child;
}

static void ResParentDestroyed(Widget resParent, XtPointer closure,
                               XtPointer callData)
{
	XmExtObject me = (XmExtObject)closure;

	if (me && !me->object.being_destroyed)
		XtDestroyWidget((Widget)me);
}

static void Destroy(Widget wid)
{
	Widget parent, resparent;
	XmDesktopObject desk = (XmDesktopObject)wid;
	XmDesktopPart *part;
	XmDesktopClassPart *cpart;

	if (!(part = desktop_part(wid)))
		return;

	if ((cpart = desktop_class_part(part->parent)) && cpart->delete_child)
		cpart->delete_child(wid);

	/**
	 * if we were created as a sibling of our primary then we have a
	 * destroy callback on them.
	 */
	if (XtIsSubclass(wid, xmDesktopClass)     &&
	    (resparent = desk->ext.logicalParent) &&
	    !resparent->core.being_destroyed) {
			XtRemoveCallback(resparent, XmNdestroyCallback,
			                 ResParentDestroyed, (XtPointer)wid);
		}

	XtFree((XtPointer)part->children);
}

static void InsertChild(Widget wid)
{
	Cardinal pos;
	XtPointer p;
	XmDesktopPart *part;

	if (wid && wid->core.being_destroyed)
		return;

	if (!(part = desktop_part(wid)) || !(part = desktop_part(part->parent)))
		return;

	/* Allocate more space if we're out of slots */
	pos = part->num_children;
	if (part->num_children == part->num_slots) {
		part->num_slots += 2 + (part->num_slots >> 1);
		p = (WidgetList)XtRealloc(
			(XtPointer)part->children,
			part->num_slots * sizeof(Widget)
		);

		if (!p) {
			part->num_slots = pos;
			return;
		}

		part->children = p;
	}

	part->children[pos] = wid;
	part->num_children++;
}

static void DeleteChild(Widget wid)
{
	Cardinal i;
	XmDesktopPart *part;

	if (!(part = desktop_part(wid)) || !(part = desktop_part(part->parent)))
		return;

	/* Get the widget's position in the list */
	for (i = 0; i < part->num_children; i++) {
		if (part->children[i] == wid)
			break;
	}

	if (i == part->num_children)
		return;

	/* Shift the rest of the list down */
	part->children[i] = NULL;
	if (i < part->num_children - 1) {
		memmove(part->children + i,
		        part->children + i + 1,
		        sizeof(*part->children) * (part->num_children - i - 1));
	}

	part->num_children--;
}

/************************************************************************
 *
 *  DesktopInitialize
 *
 ************************************************************************/
static void Initialize(Widget requested_widget, Widget new_widget,
                       ArgList args, Cardinal *num_args)
{
	Widget parent;
	XmDesktopPart *part;
	XmDesktopClassPart *cpart;

	(void)args;
	(void)num_args;
	(void)requested_widget;
	if (!(part = desktop_part(new_widget)))
		return;

	part->num_slots    = 0;
	part->num_children = 0;
	part->children     = NULL;

	cpart = desktop_class_part(part->parent);
	if (cpart && cpart->insert_child)
		cpart->insert_child(new_widget);
}

