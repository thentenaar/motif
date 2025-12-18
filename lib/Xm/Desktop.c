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

static void ClassPartInitialize(WidgetClass wid_class)
{
	XmDesktopClassPartPtr wc, super = NULL;

	wc = (XmDesktopClassPartPtr)&((XmDesktopObjectClass)wid_class)->desktop_class;
	if (wid_class != xmDesktopClass) {
		wid_class = wid_class->core_class.superclass;
		super = (XmDesktopClassPartPtr)&((XmDesktopObjectClass)wid_class)->desktop_class;
	}

	if (!super)
		return;

	if (wc->child_class == XmInheritClass)
		wc->child_class = super->child_class;

	if (wc->insert_child == XtInheritInsertChild)
		wc->insert_child = super->insert_child;

	if (wc->delete_child == XtInheritDeleteChild)
		wc->delete_child = super->delete_child;
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
	XmDesktopObjectClass pclass;

	if ((parent = desk->desktop.parent)) {
		pclass = (XmDesktopObjectClass)XtClass(parent);
		if (pclass->desktop_class.delete_child)
			pclass->desktop_class.delete_child(wid);
	}

	/**
	 * if we were created as a sibling of our primary then we have a
	 * destroy callback on them.
	 */
	if ((resparent = desk->ext.logicalParent) && !resparent->core.being_destroyed) {
		XtRemoveCallback(resparent, XmNdestroyCallback,
		                 ResParentDestroyed, (XtPointer)wid);
	}

	XtFree((XtPointer)desk->desktop.children);
}

static void InsertChild(Widget wid)
{
	Cardinal pos;
	XtPointer p;
	XmDesktopObject cw, w = (XmDesktopObject)wid;

	if (!wid || !(cw = (XmDesktopObject)w->desktop.parent))
		return;

	/* Allocate more space if we're out of slots */
	pos = cw->desktop.num_children;
	if (cw->desktop.num_children == cw->desktop.num_slots) {
		cw->desktop.num_slots += 2 + (cw->desktop.num_slots >> 1);
		p = (WidgetList)XtRealloc(
			(XtPointer)cw->desktop.children,
			cw->desktop.num_slots * sizeof(Widget)
		);

		if (!p) {
			cw->desktop.num_slots = pos;
			return;
		}

		cw->desktop.children = p;
	}

	cw->desktop.children[pos] = wid;
	cw->desktop.num_children++;
}

static void DeleteChild(Widget wid)
{
	Cardinal i;
	XmDesktopObject cw, w = (XmDesktopObject)wid;

	if (!wid || !(cw = (XmDesktopObject)w->desktop.parent))
		return;

	/* Get the widget's position in the list */
	for (i = 0; i < cw->desktop.num_children; i++) {
		if (cw->desktop.children[i] == wid)
			break;
	}

	if (i == cw->desktop.num_children)
		return;

	/* Shift the rest of the list down */
	cw->desktop.children[i] = NULL;
	if (i < cw->desktop.num_children - 1) {
		memmove(cw->desktop.children + i,
		        cw->desktop.children + i + 1,
		        sizeof(*cw->desktop.children) * (cw->desktop.num_children - i - 1));
	}

	cw->desktop.num_children--;
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
	XmDesktopObject desk = (XmDesktopObject)new_widget;
	XmDesktopObjectClass pclass;

	(void)args;
	(void)num_args;
	(void)requested_widget;
	desk->desktop.num_children = 0;
	desk->desktop.children     = NULL;
	desk->desktop.num_slots    = 0;

	if (!(parent = desk->desktop.parent))
		return;

	pclass = (XmDesktopObjectClass)XtClass(parent);
	if (pclass->desktop_class.insert_child)
		pclass->desktop_class.insert_child(new_widget);
}

