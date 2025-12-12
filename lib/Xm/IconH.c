/* $XConsortium: IconH.c /main/5 1995/07/15 20:52:17 drk $ */
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
/*
 * IconH.c: The IconHeader widget methods.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <Xm/IconHP.h>

/********    Static Function Declarations    ********/

static	void			ClassPartInitialize(
					WidgetClass	wc);
static Widget GetContainerParent(Widget);

/********    End Static Function Declarations    ********/

static	XtResource		resources[] = 
	{
		{
		XmNcontainerID,XmCContainerID,XmRWidget,
		sizeof(Widget),
		XtOffset(XmIconHeader,iconh.container_ID),
			XmRImmediate,(XtPointer)NULL},

	};

/* That should not be necessary, but inheriting extension is
   not very well understood yet */

static XmGadgetClassExtRec GadClassExtRec = {
    NULL,
    NULLQUARK,
    XmGadgetClassExtVersion,
    sizeof(XmGadgetClassExtRec),
    XmInheritBaselineProc,                  /* widget_baseline */
    XmInheritDisplayRectProc,               /* widget_display_rect */
    XmInheritMarginsProc,                   /* widget_margins */
};


externaldef( xmiconheaderclassrec) XmIconHeaderClassRec	xmIconHeaderClassRec =
{	/* RectObjClassPart */
	{	
		(WidgetClass) &xmIconGadgetClassRec, /* superclass	*/
		"XmIconHeader",			/* class_name		*/
		sizeof (XmIconHeaderRec),	/* widget_size		*/
		NULL,				/* class_initialize	*/
		ClassPartInitialize,		/* class_part_initialize*/
		False,				/* class_inited		*/
		NULL,			        /* initialize		*/
		NULL,				/* initialize_hook	*/
		NULL,				/* realize		*/
		NULL,				/* actions		*/
		0,				/* num_actions		*/
		resources,			/* resources		*/
		XtNumber (resources),		/* num_resources	*/
		NULLQUARK,			/* xrm_class		*/
		True,				/* compress_motion	*/
		True,				/* compress_exposure	*/
		True,				/* compress_enterleave	*/
		False,				/* visible_interest	*/
		NULL,		 	        /* destroy		*/
		NULL,				/* resize		*/
		XtInheritExpose,	        /* expose		*/
		NULL,			        /* set_values		*/
		NULL,				/* set_values_hook	*/
		XtInheritSetValuesAlmost,	/* set_values_almost	*/
		NULL,				/* get_values_hook	*/
		NULL,				/* accept_focus		*/
		XtVersion,			/* version		*/
		NULL,				/* callback private	*/
		NULL,				/* tm_table		*/
		XtInheritQueryGeometry,		/* query_geometry	*/
		NULL,				/* display_accelerator	*/
		NULL,				/* extension		*/
	},

	/* XmGadget Class Part */
	{
	XmInheritBorderHighlight,		/* border_highlight	*/
	XmInheritBorderUnhighlight,		/* border_unhighlight	*/
	NULL,					/* arm_and_activate	*/
	XmInheritInputDispatch,			/* input_dispatch	*/
	XmInheritVisualChange,			/* visual_change	*/
	NULL,				        /* get_resources	*/
	0,		                        /* num_get_resources	*/
	NULL,					/* class_cache_part	*/
	(XtPointer)&GadClassExtRec,             /* extension		*/
	},
	/* XmIconGadget Class Part */
	{
	    GetContainerParent,		        /* get_container_parent	*/
	    NULL,		                /* extension	*/
        },
	/* XmIconHeader Class Part */
	{
	    NULL,		                /* extension	*/
        },
};

externaldef(xmiconheaderclass) WidgetClass
	xmIconHeaderClass=(WidgetClass)&xmIconHeaderClassRec;




/*----------------
| RectObj methods |
----------------*/

/************************************************************************
 * ClassPartInitialize
 *      Parms(IconGadgetClass)
 *              returns void
 *
 *      Set Motif Fast subclass initialize bit.
 ************************************************************************/
static void
ClassPartInitialize(
	WidgetClass	wc)
{
    _XmFastSubclassInit(wc,XmICONHEADER_BIT);

}


/************************************************************************
 * GetContainerParent class method
 *
 ************************************************************************/
static Widget
GetContainerParent(
	Widget		wid)
{
    return (((XmIconHeader)(wid))->iconh.container_ID);
}


/*-------------------
| External functions |
-------------------*/
/************************************************************************
 * XmCreateIconHeader
 * 
 * Create an instance of a xmIconHeaderClass widget and
 * return it's id.
 ************************************************************************/
Widget
XmCreateIconHeader(
	Widget		parent,
	char		*name,
	ArgList		arglist,
	Cardinal	argcount)
{
	return(XtCreateWidget(name,xmIconHeaderClass,parent,arglist,argcount));
}
