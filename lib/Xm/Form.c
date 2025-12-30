/**
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
static char rcsid[] = "$TOG: Form.c /main/19 1998/03/25 12:24:56 csn $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>		/* for abs, float operation... */
#include <Xm/DrawP.h>
#include <Xm/FormP.h>
#include <Xm/DialogS.h>
#include <Xm/VaSimpleP.h>
#include "XmI.h"
#include "MessagesI.h"
#include "RepTypeI.h"
#include "GeoUtilsI.h"
#include "GMUtilsI.h"

#define MESSAGE1	_XmMMsgForm_0000
#define MESSAGE5	_XmMMsgForm_0002
#define MESSAGE7 	_XmMMsgForm_0003

/*  Useful macros  */

#define Value(a) (really ? (a)->value : (a)->tempValue)

#define AssignValue(a, v)  (really ? (a->value = (int) (v))\
			           : (a->tempValue = (int) (v)))

#define GetFormConstraint(w) \
	(&((XmFormConstraintPtr) (w)->core.constraints)->form)

#define NE(x) (oldc->x != newc->x)

#define ANY(x) (NE(att[LEFT].x) || NE(att[RIGHT].x) || \
		NE(att[TOP].x)  || NE(att[BOTTOM].x))

#define SIBLINGS(w,s)	(((w != NULL) && (s != NULL)) &&\
			 (XtParent(w) == XtParent(s)))

#define ATTACHED_WIDGET(cons, j) \
  (cons->att[j].w)

#define IS_ATTACHED_WIDGET(cons, j) \
  ((cons->att[j].type == XmATTACH_WIDGET || cons->att[j].type == XmATTACH_OPPOSITE_WIDGET) \
  && cons->att[j].w != (Widget) NULL)

/* convenient magic numbers */

#define MAX_LOOP 10000

#define LEFT	_XmFORM_LEFT
#define RIGHT	_XmFORM_RIGHT
#define TOP	_XmFORM_TOP
#define BOTTOM	_XmFORM_BOTTOM
#define FIRST_ATTACHMENT  LEFT
#define LAST_ATTACHMENT   BOTTOM

/********    Static Function Declarations    ********/

static void FromTopOffset(
                        Widget w,
                        int offset,
                        XtArgVal *value) ;
static void FromBottomOffset(
                        Widget w,
                        int offset,
                        XtArgVal *value) ;
static void FromLeftOffset(
                        Widget w,
                        int offset,
                        XtArgVal *value) ;
static void FromRightOffset(
                        Widget w,
                        int offset,
                        XtArgVal *value) ;
static void MarginWidthOut(
                        Widget wid,
                        int offset,
                        XtArgVal *value) ;
static void MarginHeightOut(
                        Widget wid,
                        int offset,
                        XtArgVal *value) ;
static void ClassPartInitialize(
                        WidgetClass wc) ;
static Boolean SyncEdges(
                        XmFormWidget fw,
                        Widget last_child,
                        Dimension *form_width,
                        Dimension *form_height,
                        Widget instigator,
                        XtWidgetGeometry *geometry) ;
static Boolean CalcFormSizeWithChange(
                        XmFormWidget fw,
                        Dimension *w,
                        Dimension *h,
                        Widget c,
                        XtWidgetGeometry *g) ;
static void CalcFormSize(
                        XmFormWidget fw,
                        Dimension *w,
                        Dimension *h) ;
static XtGeometryResult GeometryManager(
                        Widget w,
                        XtWidgetGeometry *desired,
                        XtWidgetGeometry *allowed) ;
static XtGeometryResult QueryGeometry(
                        Widget wid,
                        XtWidgetGeometry *req,
                        XtWidgetGeometry *ret) ;
static void Resize(
                        Widget wid) ;
static void Redisplay(
                        Widget fw,
                        XEvent *event,
                        Region region) ;
static void UpdateAttachments(
                        XmFormWidget fw,
                        Widget wid,
                        Widget instigator,
                        XtWidgetGeometry* inst_geometry);
static void PlaceChild(
                        XmFormWidget fw,
                        Widget child,
                        Widget instigator,
                        XtWidgetGeometry* inst_geometry);
static void PlaceChildren(
                        XmFormWidget fw,
                        Widget instigator,
                        XtWidgetGeometry *inst_geometry) ;
static void ChangeManaged(
                        Widget wid) ;
static void GetSize(
                        XmFormWidget fw,
		        XtWidgetGeometry *g,
                        Widget w,
                        XtWidgetGeometry *desired) ;
static void ChangeIfNeeded(
                        XmFormWidget fw,
                        Widget w,
                        XtWidgetGeometry *desired) ;
static void DeleteChild(
                        Widget child) ;
static Boolean SetValues(
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void SetValuesAlmost(
                        Widget cw,
                        Widget nw,
                        XtWidgetGeometry *req,
                        XtWidgetGeometry *rep) ;
static Boolean ConstraintSetValues(
                        Widget old,
                        Widget ref,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void Initialize(
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void ConstraintInitialize(
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void CheckConstraints(
                        Widget w) ;
static void SortChildren(
                        XmFormWidget fw) ;
static void CalcEdgeValues(
                        Widget w,
                        Boolean really,
                        Widget instigator,
                        XtWidgetGeometry *inst_geometry,
                        Dimension *form_width,
                        Dimension *form_height) ;
static float CheckBottomBase(
                        Widget sibling,
                        Boolean opposite) ;
static float CheckRightBase(
                        Widget sibling,
                        Boolean opposite) ;
static float CheckLeftBase(
                        Widget sibling,
                        Boolean opposite) ;
static void CalcEdgeValue(
                        XmFormWidget fw,
                        Widget w,
                        Dimension size,
                        Dimension border_width,
                        int which,
                        Boolean really,
                        Dimension *fwidth,
                        Dimension *fheight) ;
static void ComputeAttachment(
			XmFormWidget fw,
                        Widget w,
                        Dimension size,
                        Dimension border_width,
                        int which,
                        Boolean really,
                        Dimension *fwidth,
                        Dimension *fheight) ;
static int GetFormOffset(
                        XmFormWidget fw,
                        int which,
                        XmFormAttachment a) ;
static Boolean IsWidgetAttachedOneHorizontalSide(
                        Widget w);
static Boolean IsWidgetAttachedOneVerticalSide(
                        Widget w);

/********    End Static Function Declarations    ********/


static XtResource resources[] =
{
   {
       XmNmarginWidth, XmCMarginWidth, XmRHorizontalDimension,
       sizeof(Dimension),
       XtOffsetOf( struct _XmFormRec, bulletin_board.margin_width),
       XmRImmediate, (XtPointer) XmINVALID_DIMENSION
   },
   {
       XmNmarginHeight, XmCMarginHeight, XmRVerticalDimension,
       sizeof(Dimension),
       XtOffsetOf( struct _XmFormRec, bulletin_board.margin_height),
       XmRImmediate, (XtPointer) XmINVALID_DIMENSION
   },
   {
       XmNhorizontalSpacing, XmCSpacing, XmRHorizontalDimension,
       sizeof(Dimension),
       XtOffsetOf( struct _XmFormRec, form.horizontal_spacing),
       XmRImmediate, (XtPointer) 0
   },
   {
       XmNverticalSpacing, XmCSpacing, XmRVerticalDimension,
       sizeof(Dimension),
       XtOffsetOf( struct _XmFormRec, form.vertical_spacing),
       XmRImmediate, (XtPointer) 0
   },

   {
       XmNfractionBase, XmCMaxValue, XmRInt, sizeof(int),
       XtOffsetOf( struct _XmFormRec, form.fraction_base),
       XmRImmediate, (XtPointer) 100
   },

   {
       XmNrubberPositioning, XmCRubberPositioning, XmRBoolean,
       sizeof(Boolean),
       XtOffsetOf( struct _XmFormRec, form.rubber_positioning),
       XmRImmediate, (XtPointer) False
   },

};


/*  Definition for resources that need special processing in get values  */

static XmSyntheticResource syn_resources[] =
{
   { XmNmarginWidth,
     sizeof (Dimension),
     XtOffsetOf( struct _XmFormRec, bulletin_board.margin_width),
	 MarginWidthOut,
	 XmeToHorizontalPixels },

   { XmNmarginHeight,
     sizeof (Dimension),
     XtOffsetOf( struct _XmFormRec, bulletin_board.margin_height),
	 MarginHeightOut,
	 XmeToVerticalPixels },

   { XmNhorizontalSpacing,
     sizeof (Dimension),
     XtOffsetOf( struct _XmFormRec, form.horizontal_spacing),
     XmeFromHorizontalPixels,
	 XmeToHorizontalPixels },

   { XmNverticalSpacing,
     sizeof (Dimension),
     XtOffsetOf( struct _XmFormRec, form.vertical_spacing),
     XmeFromVerticalPixels,
	 XmeToVerticalPixels },
};


/*  The constraint resource list  */

static XtResource constraints[] =
{
   {
      XmNtopAttachment, XmCAttachment, XmRAttachment, sizeof(unsigned char),
      XtOffsetOf( struct _XmFormConstraintRec, form.att[TOP].type),
      XmRImmediate, (XtPointer) XmATTACH_NONE
   },

   {
      XmNbottomAttachment, XmCAttachment, XmRAttachment,
      sizeof(unsigned char),
      XtOffsetOf( struct _XmFormConstraintRec, form.att[BOTTOM].type),
      XmRImmediate, (XtPointer) XmATTACH_NONE
   },

   {
      XmNleftAttachment, XmCAttachment, XmRAttachment,
      sizeof(unsigned char),
      XtOffsetOf( struct _XmFormConstraintRec, form.att[LEFT].type),
      XmRImmediate, (XtPointer) XmATTACH_NONE
   },

   {
      XmNrightAttachment, XmCAttachment, XmRAttachment,
      sizeof(unsigned char),
      XtOffsetOf( struct _XmFormConstraintRec, form.att[RIGHT].type),
      XmRImmediate, (XtPointer) XmATTACH_NONE
   },

   {
      XmNtopWidget, XmCWidget, XmRWidget, sizeof(Widget),
      XtOffsetOf( struct _XmFormConstraintRec, form.att[TOP].w),
      XmRImmediate, (XtPointer) NULL
   },

   {
      XmNbottomWidget, XmCWidget, XmRWidget, sizeof(Widget),
      XtOffsetOf( struct _XmFormConstraintRec, form.att[BOTTOM].w),
      XmRImmediate, (XtPointer) NULL
   },

   {
      XmNleftWidget, XmCWidget, XmRWidget, sizeof(Widget),
      XtOffsetOf( struct _XmFormConstraintRec, form.att[LEFT].w),
      XmRImmediate, (XtPointer) NULL
   },

   {
      XmNrightWidget, XmCWidget, XmRWidget, sizeof(Widget),
      XtOffsetOf( struct _XmFormConstraintRec, form.att[RIGHT].w),
      XmRImmediate, (XtPointer) NULL
   },

   {
      XmNtopPosition, XmCPosition, XmRInt, sizeof(int),
      XtOffsetOf( struct _XmFormConstraintRec, form.att[TOP].percent),
      XmRImmediate, (XtPointer) 0
   },

   {
      XmNbottomPosition, XmCPosition, XmRInt, sizeof(int),
      XtOffsetOf( struct _XmFormConstraintRec, form.att[BOTTOM].percent),
      XmRImmediate, (XtPointer) 0
   },

   {
      XmNleftPosition, XmCPosition, XmRInt, sizeof(int),
      XtOffsetOf( struct _XmFormConstraintRec, form.att[LEFT].percent),
      XmRImmediate, (XtPointer) 0
   },

   {
      XmNrightPosition, XmCPosition, XmRInt, sizeof(int),
      XtOffsetOf( struct _XmFormConstraintRec, form.att[RIGHT].percent),
      XmRImmediate, (XtPointer) 0
   },

   {
      XmNtopOffset, XmCOffset, XmRVerticalInt, sizeof(int),
      XtOffsetOf( struct _XmFormConstraintRec, form.att[TOP].offset),
      XmRImmediate, (XtPointer) XmINVALID_DIMENSION
   },

   {
      XmNbottomOffset, XmCOffset, XmRVerticalInt, sizeof(int),
      XtOffsetOf( struct _XmFormConstraintRec, form.att[BOTTOM].offset),
      XmRImmediate, (XtPointer) XmINVALID_DIMENSION
   },

   {
      XmNleftOffset, XmCOffset, XmRHorizontalInt, sizeof(int),
      XtOffsetOf( struct _XmFormConstraintRec, form.att[LEFT].offset),
      XmRImmediate, (XtPointer) XmINVALID_DIMENSION
   },

   {
      XmNrightOffset, XmCOffset, XmRHorizontalInt, sizeof(int),
      XtOffsetOf( struct _XmFormConstraintRec, form.att[RIGHT].offset),
      XmRImmediate, (XtPointer) XmINVALID_DIMENSION
   },

   {
      XmNresizable, XmCBoolean, XmRBoolean, sizeof(Boolean),
      XtOffsetOf( struct _XmFormConstraintRec, form.resizable),
      XmRImmediate, (XtPointer) True
   }
};


/*  Definition for constraint resources that need special  */
/*  processing in get values                               */

static XmSyntheticResource syn_constraint_resources[] =
{

   { XmNtopOffset,
     sizeof (int),
     XtOffsetOf( struct _XmFormConstraintRec, form.att[TOP].offset),
     FromTopOffset,
	 XmeToVerticalPixels },

   {  XmNbottomOffset,
      sizeof (int),
      XtOffsetOf( struct _XmFormConstraintRec, form.att[BOTTOM].offset),
     FromBottomOffset,
	 XmeToVerticalPixels },

   {  XmNleftOffset,
      sizeof (int),
      XtOffsetOf( struct _XmFormConstraintRec, form.att[LEFT].offset),
     FromLeftOffset,
	 XmeToHorizontalPixels },

   {  XmNrightOffset,
      sizeof (int),
      XtOffsetOf( struct _XmFormConstraintRec, form.att[RIGHT].offset),
     FromRightOffset,
	 XmeToHorizontalPixels },
};



/*  Static initialization of the attached dialog box widget class record.  */

externaldef(xmformclassrec) XmFormClassRec xmFormClassRec =
{
   {                    /* core_class fields    */
      (WidgetClass) &xmBulletinBoardClassRec,   /* superclass   */
      "XmForm",                 /* class_name           */
      sizeof(XmFormRec),        /* widget_size          */
      (XtProc)NULL,             /* class_initialize */
      ClassPartInitialize,      /* class init part proc */
      False,                    /* class_inited         */
      Initialize,               /* initialize           */
      (XtArgsProc)NULL,         /* initialize_notify    */
      XtInheritRealize,         /* realize              */
      NULL,                     /* actions              */
      0,                        /* num_actions          */
      resources,                /* resources            */
      XtNumber(resources),      /* num_resources        */
      NULLQUARK,                /* xrm_class            */
      True,                     /* compress_motion  */
      XtExposeCompressMaximal,  /* compress_exposure    */
      False,                    /* compress_enterleave  */
      False,                    /* visible_interest     */
      (XtWidgetProc)NULL,       /* destroy              */
      Resize,                   /* resize               */
      Redisplay,                /* expose               */
      SetValues,                /* set_values           */
      (XtArgsFunc)NULL,         /* set_values_hook      */
      SetValuesAlmost,          /* set_values_almost    */
      (XtArgsProc)NULL,         /* get_values_hook      */
      (XtAcceptFocusProc)NULL,  /* accept_focus         */
      XtVersion,                /* version      */
      NULL,                     /* callback_private */
      XtInheritTranslations,    /* tm_table             */
      QueryGeometry,            /* Query Geometry proc  */
      (XtStringProc)NULL,       /* disp accelerator     */
      NULL,                     /* extension            */
   },

   {                    /* composite_class fields */
      GeometryManager,      /* geometry_manager       */
      ChangeManaged,        /* change_managed         */
      XtInheritInsertChild, /* insert_child           */
      DeleteChild,          /* delete_child           */
      NULL,                 /* extension              */
   },

   {                    /* constraint_class fields */
      constraints,                  /* constraint resource     */
      XtNumber(constraints),        /* number of constraints   */
      sizeof(XmFormConstraintRec),  /* size of constraint      */
      ConstraintInitialize,         /* initialization          */
      (XtWidgetProc)NULL,           /* destroy proc            */
      ConstraintSetValues,          /* set_values proc         */
      NULL,                         /* extension               */
   },

   {                        /* manager_class fields   */
      XtInheritTranslations,                /* translations           */
      syn_resources,                        /* syn_resources          */
      XtNumber(syn_resources),              /* num_syn_resources      */
      syn_constraint_resources,             /* syn_cont_resources     */
      XtNumber(syn_constraint_resources),   /* num_syn_cont_resources */
      XmInheritParentProcess,               /* parent_process         */
      NULL,                                 /* extension              */
   },

   {                        /* bulletin_board_class fields */
      FALSE,                                /* always_install_accelerators */
      (XmGeoCreateProc)NULL,                /* geo_matrix_create  */
      XmInheritFocusMovedProc,              /* focus_moved_proc   */
      NULL,                                 /* extension          */
   },

   {                        /* form_class fields  */
      (XtPointer) NULL,                     /* extension          */
   }
};


externaldef(xmformwidgetclass) WidgetClass
	xmFormWidgetClass = (WidgetClass) &xmFormClassRec;

static void
FromTopOffset(
	Widget w,
	int offset,
	XtArgVal *value)
{
    XmFormWidget fw = (XmFormWidget) w->core.parent;
    XmFormConstraint fc = GetFormConstraint(w);

    *value = (XtArgVal) GetFormOffset(fw, TOP, fc->att);
    XmeFromVerticalPixels(w, offset, value);
}

static void
FromBottomOffset(
	Widget w,
	int offset,
	XtArgVal *value)
{
    XmFormWidget fw = (XmFormWidget) w->core.parent;
    XmFormConstraint fc = GetFormConstraint(w);

    *value = (XtArgVal) GetFormOffset(fw, BOTTOM, fc->att);
    XmeFromVerticalPixels(w, offset, value);
}

static void
FromLeftOffset(
	Widget w,
	int offset,
	XtArgVal *value)
{
    XmFormWidget fw = (XmFormWidget) w->core.parent;
    XmFormConstraint fc = GetFormConstraint(w);

    *value = (XtArgVal) GetFormOffset(fw, LEFT, fc->att);
    XmeFromHorizontalPixels(w, offset, value);
}

static void
FromRightOffset(
	Widget w,
	int offset,
	XtArgVal *value)
{
    XmFormWidget fw = (XmFormWidget) w->core.parent;
    XmFormConstraint fc = GetFormConstraint(w);

    *value = (XtArgVal) GetFormOffset(fw, RIGHT, fc->att);
    XmeFromHorizontalPixels(w, offset, value);
}



static void
MarginWidthOut(
        Widget wid,
        int offset,
        XtArgVal *value )
{
        XmFormWidget fw = (XmFormWidget) wid ;

	if (fw->bulletin_board.margin_width == XmINVALID_DIMENSION)
		*value = 0;
	else
		XmeFromHorizontalPixels((Widget) fw, offset, value);
}

static void
MarginHeightOut(
        Widget wid,
        int offset,
        XtArgVal *value )
{
        XmFormWidget fw = (XmFormWidget) wid ;

	if (fw->bulletin_board.margin_height == XmINVALID_DIMENSION)
		*value = 0;
	else
		XmeFromVerticalPixels((Widget) fw, offset, value);
}

/************************************************************************
 *
 *  ClassPartInitialize
 *	Set up the fast subclassing.
 *
 ************************************************************************/
static void
ClassPartInitialize(
        WidgetClass wc )
{
   _XmFastSubclassInit (wc, XmFORM_BIT);
}




/************************************************************************
 *
 *  CalcFormSizeWithChange
 *	Find size of a bounding box which will include all of the
 *	children, including the child which may change
 *
 ************************************************************************/
static Boolean
SyncEdges(
        XmFormWidget fw,
        Widget last_child,
        Dimension *form_width,
        Dimension *form_height,
        Widget instigator,
        XtWidgetGeometry *geometry )
{
	Widget child;
	XmFormConstraint c;
	long int loop_count;
	Dimension tmp_w = *form_width, tmp_h = *form_height;
	Dimension sav_w, sav_h;
	Boolean settled = FALSE;
	Boolean finished = TRUE;

	sav_w = tmp_w;
	sav_h = tmp_h;

	loop_count = 0;
	while (!settled)
	{
		/*
		 * Contradictory constraints can cause the constraint
		 * processing to go into endless oscillation.  This means that
		 * proper exit condition for this loop is never satisfied.
		 * But, infinite loops are a bad thing, even if is the result
		 * of a careless user.  We therefore have added a loop counter
		 * to ensure that this loop will terminate.
		 *
		 * There are problems with this however.  In the worst case
		 * this procedure could need to loop fw->composite.num_children!
		 * times.  Unfortunately, numbers like 100! don't fit integer
		 * space well; neither will current architectures complete that
		 * many loops before the sun burns out.
		 *
		 * Soooo, we will wait for an arbitrarily large number of
		 * iterations to go by before we give up.  This allows us to
		 * claim that the procedure will always complete, and the number
		 * is large enough to accomodate all but the very large and
		 * very pathological Form widget configurations.
		 *
		 * This is gross, but it's either do this or risk truly
		 * infinite loops.
		 */

		if (loop_count++ > MAX_LOOP)
			break;

		for (child = fw->form.first_child;
			child != NULL;
			child = c->next_sibling)
		{
			if (!XtIsManaged (child))
				break;

			c = GetFormConstraint(child);

			CalcEdgeValues(child, FALSE, instigator, geometry,
				&tmp_w, &tmp_h);

			if (child == last_child)
				break;
		}

		if ((sav_w == tmp_w) && (sav_h == tmp_h))
			settled = TRUE;
		else
		{
			sav_w = tmp_w;
			sav_h = tmp_h;
		}
	}


	if (loop_count > MAX_LOOP)
	{
		XmeWarning( (Widget) fw, MESSAGE7);
		finished = FALSE;
	}

	*form_width = sav_w;
	*form_height = sav_h;

	return(finished);
}

static Boolean
CalcFormSizeWithChange(
        XmFormWidget fw,
        Dimension *w,
        Dimension *h,
        Widget c,
        XtWidgetGeometry *g )
{
	Dimension junkh = fw->core.height;
	Dimension junkw = fw->core.width;
	Widget child;
	XmFormConstraint fc;
	int tmp;

	if (h == NULL) h = &junkh;
	if (w == NULL) w = &junkw;

	/* Place children, but don't do it for real--just get new size */

	for(child = fw->form.first_child;
		child != NULL;
		child = fc->next_sibling)
	{
		if (!XtIsManaged (child))
			break;

		fc = GetFormConstraint(child);

		CalcEdgeValues(child, False, c, g, w, h);
		if (!SyncEdges(fw, child, w, h, c, g))
			return(False);
	}

	for(child = fw->form.first_child;
		child != NULL;
		child = fc->next_sibling)
	{
		if (!XtIsManaged (child))
			break;

		fc = GetFormConstraint(child);

		tmp = fc->att[RIGHT].tempValue;
		if (fc->att[RIGHT].type == XmATTACH_FORM)
			tmp += GetFormOffset(fw, RIGHT, fc->att);
		if (tmp > 0)
			ASSIGN_MAX(*w, tmp);

		tmp = fc->att[BOTTOM].tempValue ;
		if (fc->att[BOTTOM].type == XmATTACH_FORM)
			tmp += GetFormOffset(fw, BOTTOM, fc->att);
		if (tmp > 0)
			ASSIGN_MAX (*h, tmp);
	}

	if (!(*w))
		*w = 1;
	if (!(*h))
		*h = 1;

	if (*w != XtWidth(fw) || *h != XtHeight(fw))
		return True;
	else
		return False;
}




/************************************************************************
 *
 *  CalcFormSize
 *	Find size of a bounding box which will include all of the children.
 *
 ************************************************************************/
static void
CalcFormSize(
        XmFormWidget fw,
        Dimension *w,
        Dimension *h )
{
	Widget child;
	Dimension junkh = fw->core.height;
	Dimension junkw = fw->core.width;
	XmFormConstraint fc;
	int tmp;

	if (h == NULL) h = &junkh;
	if (w == NULL) w = &junkw;

	for (child = fw->form.first_child;
		child != NULL;
		child = fc->next_sibling)
	{
		if (!XtIsManaged (child))
			break;

		fc = GetFormConstraint(child);

		CalcEdgeValues(child, False, NULL, NULL, w, h);
		if (!SyncEdges(fw, child, w, h, NULL, NULL))
			break;
	}

	for(child = fw->form.first_child;
		child != NULL;
		child = fc->next_sibling)
	{
		if (!XtIsManaged (child))
			break;

		fc = GetFormConstraint(child);

		tmp = fc->att[RIGHT].tempValue;
		if (fc->att[RIGHT].type == XmATTACH_FORM)
			tmp += GetFormOffset(fw, RIGHT, fc->att);
		if (tmp > 0)
			ASSIGN_MAX(*w, tmp);

		tmp = fc->att[BOTTOM].tempValue ;
		if (fc->att[BOTTOM].type == XmATTACH_FORM)
			tmp += GetFormOffset(fw, BOTTOM, fc->att);
		if (tmp > 0)
			ASSIGN_MAX(*h, tmp);
	}

	if (!(*w))
		*w = 1;
	if (!(*h))
		*h = 1;
}



/************************************************************************
 *
 *  GeometryManager
 *
 ************************************************************************/
static XtGeometryResult
GeometryManager(
        Widget w,
        XtWidgetGeometry *desired,
        XtWidgetGeometry *allowed )
{
    int size_req ;
    XtWidgetGeometry original;
    XtGeometryResult reply = XtGeometryNo;
    XmFormWidget fw = (XmFormWidget) XtParent(w);
    XmFormConstraint c = GetFormConstraint(w);

    /* sooo confusing... */
    if (fw->form.processing_constraints) {
	fw->form.processing_constraints = FALSE;
	PlaceChildren (fw, NULL, NULL);
	return(XtGeometryNo);
    }

    /*
     * Fix for 4854 - If the widget is not resizable, do not set the
     *                preferred_width or the preferred_height.
     */
    if ((desired->request_mode & CWWidth) &&
 	!(desired->request_mode & XtCWQueryOnly) && c->resizable)
  	c->preferred_width = desired->width;
    if ((desired->request_mode & CWHeight) &&
 	!(desired->request_mode & XtCWQueryOnly) && c->resizable)
  	c->preferred_height = desired->height;

    if (desired->request_mode == (CWX | CWY)) return(XtGeometryNo);

    original.x = w->core.x;
    original.y = w->core.y;
    original.width = w->core.width;
    original.height = w->core.height;
    original.border_width = w->core.border_width;
    original.request_mode = (CWX | CWY | CWWidth | CWHeight | CWBorderWidth);

    size_req = desired->request_mode &
	              (CWX | CWY | CWWidth | CWHeight | CWBorderWidth);
    if (size_req && c->resizable) {
	XtWidgetGeometry g, r;
	XtGeometryResult res ;

	/* get the size the Form wants to be */
	GetSize(fw, &g, w, desired);

	/* GetSize takes care of the resizePolicy resource as well
	   and returns the desired change in g if any.
	   g.request_mode might be 0 at this point, which means the
	   Form doesn't want to change, but we still want to
	   see if the child request can be accomodated into
	   the current Form size, so we move forward with the
	   0 in request mode.  */

	/* if the child requesed query only, propagate */
	if (desired->request_mode & XtCWQueryOnly)
	    g.request_mode |= XtCWQueryOnly;

	/* Let's make a request to the parent.
	   At this point, we do not want to accept any compromise
	   because we're not sure our kid will accept it in turn.
	   So use the Xt API, no _XmMakeGeometryRequest.
	   If g.request_mode is 0, nothing will happen, yes
	   will be returned */
	res = XtMakeGeometryRequest((Widget)fw, &g, &r);

	/* check that a real request has failed or that if request_mode was 0,
	   consider that a No */
	if (!g.request_mode || (res != XtGeometryYes)) {
	    Dimension orig_form_width, orig_form_height  ;

	    /* let's save the original form size first */
	    orig_form_width = fw->core.width ;
	    orig_form_height = fw->core.height ;

	    if (res == XtGeometryAlmost) {
		/* let's try the proposal. Stuff the Form with it so that
		   PlaceChildren does the layout base on it */
		if (r.request_mode | CWWidth) fw->core.width = r.width ;
		if (r.request_mode | CWHeight) fw->core.height = r.height ;
	    }
	    /* else it's No and we keep the same size */

	    /* let's see if there is a chance that the child request
	       be honored: the needed form overall size is smaller than
	       the current or proposed size (now in the core field) */

	    if ((g.width <= fw->core.width) &&
		(g.height <= fw->core.height)) {
		/* ok, now we'are going to try to place the kid
		   using the new Form size for the layout */
		PlaceChildren (fw, w, desired);

		/* now, we check if the requestor has gotten
		   what it asked for.  The logic is that:
		   width = req_width iff req_width
		   height = req_height iff req_height */
		if (((desired->request_mode & CWWidth &&
		      desired->width == w->core.width) ||
		     ! (desired->request_mode & CWWidth)) &&
		    ((desired->request_mode & CWHeight &&
		      desired->height == w->core.height) ||
		     ! (desired->request_mode & CWHeight))) {
		    /* ok, the kid request has been honored, although the
		       Form couldn't change its own size, let's
		       return Yes to the kid and ask the Form's parent
		       the needed size if Almost was returned */
		    if (res == XtGeometryAlmost) {
			/* let's backup the original Form size first */
			fw->core.width = orig_form_width  ;
			fw->core.height = orig_form_height;

			/* success guaranteed */
			XtMakeGeometryRequest((Widget)fw, &r, NULL);

			/* no need to do the layout PlaceCHildren, it
			   has already been done using the correct size */
		    }

		    /* simply return Yes, since PlaceChildren already
		       change the kid core fields */
		    reply = XtGeometryYes ;

		} else {
		    /* the kid hasn't gotten what it asked for in the
		       current Form geometry, we have to backup
		       everything and return either No or Almost
		       to the kid */

		    if ((w->core.width != original.width) ||
			(w->core.height != original.height)) {

			allowed->request_mode = desired->request_mode;
			allowed->sibling = desired->sibling;
			allowed->stack_mode = desired->stack_mode;
			allowed->x = w->core.x;
			allowed->y = w->core.y;
			allowed->width = w->core.width;
			allowed->height = w->core.height;
			allowed->border_width = w->core.border_width;
			reply = XtGeometryAlmost;

		    } else reply = XtGeometryNo;

		    /* backup the kid geometry */
		    w->core.x = original.x;
		    w->core.y = original.y;
		    w->core.width = original.width;
		    w->core.height = original.height;
		    w->core.border_width = original.border_width;

		    /* backup the Form and the layout too */
		    fw->core.width = orig_form_width  ;
		    fw->core.height = orig_form_height;

		    PlaceChildren (fw, w, &original);
		}
	    } else {
		/* the size the Form wants for the kid request is bigger than
		   its current or proposed size, return No to the kid */
		/* backup the original Form size first */
		fw->core.width = orig_form_width  ;
		fw->core.height = orig_form_height;
		/* we haden't changed anything else, just return No */
		reply = XtGeometryNo;

                if (((IsWidgetAttachedOneHorizontalSide(w)) &&
                     (desired->request_mode & CWWidth)) ||
                    ((IsWidgetAttachedOneVerticalSide(w)) &&
                     (desired->request_mode & CWHeight)))
		  { PlaceChildren (fw, w, desired);
		    reply = XtGeometryYes;
		  }
	    }
	} else {
	    /* ok, we got a Yes form the Form's parent, let's relayout
	       using the new size, except if query only was specified */

	    if (!(desired->request_mode & XtCWQueryOnly)) {
		/* Reposition the widget only if not QueryOnly */
		PlaceChildren (fw, w, desired);
	    }
	    reply = XtGeometryYes;
	}
    }

    /* let's deal with stacking order */

    if (desired->request_mode & (CWSibling | CWStackMode)) {
	/* always honor stand alone stack requests */
	if (!size_req) reply = XtGeometryYes;
	else
	    /* the request concerned size as well, see if it was denied.
	       if so, propose the stack request alone */
	    if (reply != XtGeometryYes) {
		allowed->request_mode = desired->request_mode;
		allowed->sibling = desired->sibling;
		allowed->stack_mode = desired->stack_mode;
		allowed->x = w->core.x;
		allowed->y = w->core.y;
		allowed->width = w->core.width;
		allowed->height = w->core.height;
		allowed->border_width = w->core.border_width;
		reply = XtGeometryAlmost;
	    }
    }

    /* deal with the shadow resize without Expose */
    if ( fw->bulletin_board.old_shadow_thickness &&
	(fw->bulletin_board.old_width != fw->core.width ||
	 fw->bulletin_board.old_height != fw->core.height) )
	{
	    _XmClearShadowType ((Widget) fw, fw->bulletin_board.old_width,
				fw->bulletin_board.old_height,
				fw->bulletin_board.old_shadow_thickness, 0);
	}

    fw->bulletin_board.old_width = fw->core.width;
    fw->bulletin_board.old_height = fw->core.height;
    fw->bulletin_board.old_shadow_thickness = fw->manager.shadow_thickness;


    return (reply);
}


/************************************************************************
 *
 *  QueryGeometry
 *
 ************************************************************************/
static XtGeometryResult
QueryGeometry(
        Widget widget,
        XtWidgetGeometry *intended,
        XtWidgetGeometry *desired )
{
    Dimension width = 0, height = 0 ;
    XmFormWidget fw = (XmFormWidget) widget ;

    /* first determine what is the desired size, using the resize_policy. */
    if (fw->bulletin_board.resize_policy == XmRESIZE_NONE) {
	desired->width = XtWidth(widget) ;
	desired->height = XtHeight(widget) ;
    } else {
	SortChildren(fw);
	if (GMode( intended) & CWWidth) width = intended->width;
	if (GMode( intended) & CWHeight) height = intended->height;

	if (!XtIsRealized((Widget)fw))
	{
		int i;
		Widget child;
		XmFormConstraint c;

		for (i = 0; i < fw->composite.num_children; i++)
		{
			child = fw->composite.children[i];
			c = GetFormConstraint(child);
			c->preferred_width = XtWidth(child);
			c->preferred_height = XtHeight(child);
		}
	}

	CalcFormSize(fw, &width, &height);
	if ((fw->bulletin_board.resize_policy == XmRESIZE_GROW) &&
	    ((width < XtWidth(widget)) ||
	     (height < XtHeight(widget)))) {
	    desired->width = XtWidth(widget) ;
	    desired->height = XtHeight(widget) ;
	} else {
	    desired->width = width ;
	    desired->height = height ;
	}
    }

    /* deal with user initial size setting */
    if (!XtIsRealized(widget))  {
	if (XtWidth(widget) != 0) desired->width = XtWidth(widget) ;
	if (XtHeight(widget) != 0) desired->height = XtHeight(widget) ;
    }

    return XmeReplyToQueryGeometry(widget, intended, desired) ;
}




/************************************************************************
 *
 *  Resize
 *	This routine is called by the parent's geometry manager after it
 *	has ok'd the resize request AND resized the form window.  This
 *	routine is responsible for implementing the size given.
 *
 ************************************************************************/
static void
Resize(
        Widget wid )
{
    XmFormWidget fw = (XmFormWidget) wid ;
    Boolean      draw_shadow = False;

    /* clear the shadow if its needed (will check if its now larger) */
    _XmClearShadowType ((Widget )fw, fw->bulletin_board.old_width,
			fw->bulletin_board.old_height,
			fw->bulletin_board.old_shadow_thickness, 0);

	/*
	 * if it is now smaller, redraw the shadow since there may not be a
	 * redisplay
	 */
    if ((fw->bulletin_board.old_height > fw->core.height) ||
	(fw->bulletin_board.old_width > fw->core.width))
	draw_shadow = True;


    fw->bulletin_board.old_width = fw->core.width;
    fw->bulletin_board.old_height = fw->core.height;
    fw->bulletin_board.old_shadow_thickness =
	fw->manager.shadow_thickness;


    PlaceChildren (fw, NULL, NULL) ;

    if ((draw_shadow) && (XtIsRealized((Widget)fw)))
	XmeDrawShadows  (XtDisplay (fw), XtWindow (fw),
			 fw->manager.top_shadow_GC,
			 fw->manager.bottom_shadow_GC,
			 0, 0, fw->core.width, fw->core.height,
			 fw->manager.shadow_thickness,
			 fw->bulletin_board.shadow_type);
}


static void
Redisplay(
        Widget fw,
        XEvent *event,
        Region region )
{
    XmeRedisplayGadgets( fw, event, region);

    XmeDrawShadows (XtDisplay((Widget)fw),
		    XtWindow((Widget)fw),
		    ((XmFormWidget) fw)->manager.top_shadow_GC,
		    ((XmFormWidget) fw)->manager.bottom_shadow_GC,
		    0, 0,
		    XtWidth(fw), XtHeight(fw),
		    ((XmFormWidget) fw)->manager.shadow_thickness,
		    ((XmFormWidget) fw)->bulletin_board.shadow_type);
}



/************************************************************************
 *
 *  PlaceChildren
 *	Position all children according to their constraints.
 *      Return desired width and height.
 *
 ************************************************************************/
static void
PlaceChildren(
        XmFormWidget fw,
        Widget instigator,
        XtWidgetGeometry *inst_geometry )
{
    Widget child;

    for (child = fw->form.first_child;
        child != NULL;
        child = (GetFormConstraint(child))->next_sibling) {
      /* Place child itself */
      PlaceChild(fw, child, instigator, inst_geometry);

      /* Update all attached widgets
       * according to the placement of a child */
      UpdateAttachments(fw, child, instigator, inst_geometry);
    }
}

static void
UpdateAttachments(
    XmFormWidget fw,
    Widget wid,
    Widget instigator,
    XtWidgetGeometry* inst_geometry)
{
  XmFormConstraint c;
  c = GetFormConstraint(wid);

  if (IS_ATTACHED_WIDGET(c, LEFT))
      PlaceChild(fw, ATTACHED_WIDGET(c, LEFT), instigator, inst_geometry);
  if (IS_ATTACHED_WIDGET(c, RIGHT))
      PlaceChild(fw, ATTACHED_WIDGET(c, RIGHT), instigator, inst_geometry);
  if (IS_ATTACHED_WIDGET(c, TOP))
      PlaceChild(fw, ATTACHED_WIDGET(c, TOP), instigator, inst_geometry);
  if (IS_ATTACHED_WIDGET(c, BOTTOM))
      PlaceChild(fw, ATTACHED_WIDGET(c, BOTTOM), instigator, inst_geometry);
}

static void
PlaceChild(
        XmFormWidget fw,
        Widget child,
        Widget instigator,
        XtWidgetGeometry* inst_geometry)
{
  XmFormConstraint c;
  int height, width;
  Dimension border_width;
  int near_edge;

  if (!XtIsManaged(child))
	    return;

	c = GetFormConstraint(child);

	CalcEdgeValues(child, TRUE, instigator, inst_geometry,
		       NULL, NULL);

	if ((child == instigator) &&
	    (inst_geometry->request_mode & CWBorderWidth))
	    border_width = inst_geometry->border_width;
	else
	    border_width = ((RectObj) child)->rectangle.border_width;

        if (LayoutIsRtoLM(fw)) {
	  /* switch the meanings of left and right attachements */
	  width = c->att[LEFT].value - c->att[RIGHT].value - (2 * border_width);
	  near_edge = RIGHT;
	} else {
	  width = c->att[RIGHT].value - c->att[LEFT].value - (2 * border_width);
	  near_edge = LEFT;
	}
	height = c->att[BOTTOM].value - c->att[TOP].value
	    - (2 * border_width);

	if (width <= 0) width = 1;
	if (height <= 0) height = 1;

	if ((c->att[near_edge].value != ((RectObj) child)->rectangle.x) ||
	    (c->att[TOP].value != ((RectObj) child)->rectangle.y)  ||
	    (width != ((RectObj) child)->rectangle.width)          ||
	    (height != ((RectObj) child)->rectangle.height)        ||
	    (border_width != ((RectObj) child)->rectangle.border_width)) {

	  /* Yes policy everywhere, so don't resize the instigator */
	  if (child != instigator) {
	    XmeConfigureObject(child,
			       c->att[near_edge].value,
			       c->att[TOP].value,
			       width, height, border_width);
	  } else {
	    XmeConfigureObject(child,
			       c->att[near_edge].value,
			       c->att[TOP].value,
			       child->core.width, child->core.height,
			       child->core.border_width);
	    child->core.width = width ;
	    child->core.height = height ;
	    child->core.border_width = border_width ;
	  }
	}
}

/************************************************************************
 *
 *  ChangeManaged
 *	Something changed in the set of managed children, so place
 *	the children and change the form widget size to reflect new size,
 *	if possible.
 *
 ************************************************************************/
static void
ChangeManaged(
        Widget wid )
{
    XmFormWidget fw = (XmFormWidget) wid ;
    XtWidgetGeometry g;
    int i, j, k;
    XmFormConstraint c;
    Widget w, child;

    /*
     * The following code works around a bug in the intrinsics
     * destroy processing.  The child is unmanaged before anything
     * else (destroy callbacks) so we have to handle the destroy
     * inside of changemanaged instead of in a destroy callback
     */
    for (k = 0; k < fw->composite.num_children; k++) {
	child = fw->composite.children[k];

	if (child->core.being_destroyed) {
	    /*  If anyone depends on this child,
		make into a dependency on form  */


	    for (i = 0; i < fw->composite.num_children; i++) {
		w = fw->composite.children[i];
		c = GetFormConstraint(w);

		for (j = FIRST_ATTACHMENT; j < (LAST_ATTACHMENT + 1); j++)  {
		    if (((c->att[j].type == XmATTACH_WIDGET) &&
			 (c->att[j].w == child))
			||
			((c->att[j].type == XmATTACH_OPPOSITE_WIDGET) &&
			 (c->att[j].w == child))) {
			switch (j)
			    {
			    case LEFT:
				c->att[j].type = XmATTACH_FORM;
				c->att[j].offset = w->core.x;
				break;
			    case TOP:
				c->att[j].type = XmATTACH_FORM;
				c->att[j].offset = w->core.y;
				break;
			    default:
				c->att[j].type = XmATTACH_NONE;
				break;
			    }
			c->att[j].w = NULL;
		    }
		}
	    }
	}
    }

    SortChildren (fw);

    /* Don't use XtRealizedWidget(form) as a test to initialize the
       preferred geometry, since when you realize the form before its
       kid, everything goes to the ground.
       Here we initialize a field if it hasn't been done already,
       the XmINVALID_DIMENSION has been set in ConstraintInitialize */

    for (i = 0; i < fw->composite.num_children; i++) {
	child = fw->composite.children[i];
	c = GetFormConstraint(child);
	if (c->preferred_width == XmINVALID_DIMENSION)
	    c->preferred_width = XtWidth(child);
	if (c->preferred_height == XmINVALID_DIMENSION)
	    c->preferred_height = XtHeight(child);
    }

    if (!XtIsRealized((Widget)fw))
	{
	    /* First time through */
	    Dimension x = 0, h = 0;

	    g.request_mode = 0;
	    g.width = (fw->core.width ? fw->core.width : 1);
	    g.height = (fw->core.height ? fw->core.height : 1);

	    if (!XtWidth(fw) && XtHeight(fw))
		{
		    CalcFormSize(fw, &x, NULL);
		    g.width = x;
		    g.request_mode |= CWWidth;
		}
	    else if (XtWidth(fw) && !XtHeight(fw))
		{
		    CalcFormSize(fw, NULL, &h);
		    g.height = h;
		    g.request_mode |= CWHeight;
		}
	    else if (!XtWidth(fw) && !XtHeight(fw))
		{
		    CalcFormSize(fw, &x, &h);
		    g.width = x;
		    g.height = h;
		    g.request_mode |= (CWWidth | CWHeight);
		}

	    if (g.request_mode != 0)
		_XmMakeGeometryRequest((Widget) fw, &g);

	    PlaceChildren (fw, NULL, NULL);
	}
    else
	{
	    ChangeIfNeeded(fw, NULL, NULL);
	    PlaceChildren (fw, NULL, NULL);
	}

    fw->bulletin_board.old_width = fw->core.width;
    fw->bulletin_board.old_height = fw->core.height;
    fw->bulletin_board.old_shadow_thickness =
	fw->manager.shadow_thickness;

    XmeNavigChangeManaged((Widget) fw);
}




/************************************************************************
 *
 *  GetSize
 *
 *
 ************************************************************************/
static void
GetSize(
        XmFormWidget fw,
	XtWidgetGeometry * g,
	Widget w,
        XtWidgetGeometry *desired )
{
    Boolean grow_ok = fw->bulletin_board.resize_policy != XmRESIZE_NONE,
            shrink_ok = fw->bulletin_board.resize_policy == XmRESIZE_ANY;

    g->request_mode = 0;
    g->width = 0;
    g->height = 0;

    /* Compute the desired size of the form */
    if (CalcFormSizeWithChange(fw, &g->width, &g->height, w, desired)) {

	/* there is a change - check resize policy first */
	if ((g->width > fw->core.width && !grow_ok) ||
	    (g->width < fw->core.width && !shrink_ok) ||
	    (g->height > fw->core.height && !grow_ok) ||
	    (g->height < fw->core.height && !shrink_ok))
	    return ; /* exit with request_mode = 0 */

	if (g->width != fw->core.width) g->request_mode |= CWWidth;
	if (g->height != fw->core.height) g->request_mode |= CWHeight;
    }

}

/************************************************************************
 *
 *  ChangeIfNeeded
 *	Returns whether to honor widget w's resize request; only returns
 *      False if resize would require form to change size but it cannot.
 *	Form changes size as a side effect.
 *
 ************************************************************************/
static void
ChangeIfNeeded(
        XmFormWidget fw,
        Widget w,
        XtWidgetGeometry *desired )
{
    XtWidgetGeometry g;

    /* find out the desired size of the form, using the
       children attachment and the resizePolicy */
    GetSize(fw, &g, w, desired);

    _XmMakeGeometryRequest((Widget)fw, &g) ;

    if ( fw->bulletin_board.old_shadow_thickness &&
	(fw->bulletin_board.old_width != fw->core.width ||
	 fw->bulletin_board.old_height != fw->core.height) )
	{
	    _XmClearShadowType ((Widget) fw, fw->bulletin_board.old_width,
				fw->bulletin_board.old_height,
				fw->bulletin_board.old_shadow_thickness, 0);
	}

    fw->bulletin_board.old_width = fw->core.width;
    fw->bulletin_board.old_height = fw->core.height;
    fw->bulletin_board.old_shadow_thickness = fw->manager.shadow_thickness;
}



/************************************************************************
 *
 *  DeleteChild
 *	Delete a single widget from a parent widget
 *
 ************************************************************************/
static void
DeleteChild(
        Widget child )
{
    XtWidgetProc delete_child;

    if (!XtIsRectObj(child)) return;

    _XmProcessLock();
    delete_child = ((CompositeWidgetClass) xmFormClassRec.core_class.superclass)->
			    composite_class.delete_child;
    _XmProcessUnlock();

    (*delete_child)(child);

    SortChildren((XmFormWidget) XtParent(child));
}




/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/
static Boolean
SetValues(
        Widget cw,
        Widget rw,
        Widget nw,
        ArgList args,		/* unused */
        Cardinal * num_args )	/* unused */
{
        XmFormWidget old = (XmFormWidget) cw ;
        XmFormWidget new_w = (XmFormWidget) nw ;

	Boolean returnFlag = FALSE;
	Dimension w = 0;
	Dimension h = 0;


	/*  Check for invalid fraction base  */

	if (new_w->form.fraction_base == 0)
	{
		XmeWarning( (Widget) new_w, MESSAGE1);
		new_w->form.fraction_base = old->form.fraction_base;
	}

	if (XtIsRealized((Widget)new_w))
	{
	    /* SetValues of width and/or height = 0, signals the form to  */
	    /* recompute its bounding box and grow if it needs.           */

		if ((XtWidth(new_w) != XtWidth(old)) ||
			(XtHeight(new_w) != XtHeight(old)))
		{
			if ((XtWidth(new_w) == 0) || (XtHeight(new_w) == 0))
			{
				CalcFormSize (new_w, &w, &h);
				if (XtWidth(new_w) == 0) XtWidth(new_w) = w;
				if (XtHeight(new_w) == 0) XtHeight(new_w) = h;

			}
			else
			{
				w = XtWidth(new_w);
				h = XtHeight(new_w);
			}
		}


		/*  If default distance has changed, or the
			fraction base has changed, recalculate size.  */

		if ((new_w->form.horizontal_spacing !=
				old->form.horizontal_spacing)   ||
			(new_w->bulletin_board.margin_width !=
				old->bulletin_board.margin_width)     ||
			(new_w->form.vertical_spacing !=
				old->form.vertical_spacing)     ||
			(new_w->bulletin_board.margin_height !=
				old->bulletin_board.margin_height)     ||
			(new_w->form.fraction_base !=
				old->form.fraction_base))
		{
			CalcFormSize(new_w, &w, &h);
			XtWidth(new_w) = w;
			XtHeight(new_w) = h;
		}
	}

	return(returnFlag);
}

static void
SetValuesAlmost(
        Widget cw,		/* unused */
        Widget nw,
        XtWidgetGeometry *req,
        XtWidgetGeometry *rep )
{
	XmFormWidget new_w = (XmFormWidget) nw ;

	if (!rep->request_mode)
		PlaceChildren(new_w, NULL, NULL);

	*req = *rep;
}




/************************************************************************
 *
 *  ConstraintSetValues
 *	If any values change, what we do is place everything again.
 *
 ************************************************************************/
static Boolean
ConstraintSetValues(
        Widget old,
        Widget ref,	/* unused */
        Widget new_w,
        ArgList args,		/* unused */
        Cardinal * num_args )	/* unused */
{
    XmFormWidget fw = (XmFormWidget) XtParent(new_w);
    XmFormConstraint oldc, newc;
    int i;

    if (!XtIsRectObj(new_w))
	return(FALSE);

    oldc = GetFormConstraint(old),
    newc = GetFormConstraint(new_w);

   /* This fragment has been removed in order to fix bug #1298
    if (XtWidth(new_w) != XtWidth(old))
	newc->preferred_width = XtWidth(new_w);
    if (XtHeight(new_w) != XtHeight(old))
	newc->preferred_height = XtHeight(new_w);
    */

    /*  Validate the attachement type.  */

    for (i = FIRST_ATTACHMENT; i < (LAST_ATTACHMENT + 1); i++) {
	if (newc->att[i].type != oldc->att[i].type) {
	    if(    !XmRepTypeValidValue( XmRID_ATTACHMENT,
					newc->att[i].type, new_w)    )
		{
		    newc->att[i].type = oldc->att[i].type;
		}
	}
	if ((newc->att[i].type == XmATTACH_WIDGET) ||
	    (newc->att[i].type == XmATTACH_OPPOSITE_WIDGET)) {

	    while ((newc->att[i].w) && !SIBLINGS(newc->att[i].w, new_w)) {
		newc->att[i].w = XtParent(newc->att[i].w);
	    }
	}
    }

    /* Re do the layout only if we have to */
    if ((XtIsRealized((Widget)fw)) && (XtIsManaged(new_w)) &&
	(ANY(type) || ANY(w) || ANY(percent) || ANY(offset)))
	{
	    XtWidgetGeometry g;

	    g.request_mode = 0;

	    if (XtWidth(new_w) != XtWidth(old))
		{
		    g.request_mode |= CWWidth;
		    g.width = XtWidth(new_w);
		}

	    if (XtHeight(new_w) != XtHeight(old))
		{
		    g.request_mode |= CWHeight;
		    g.height = XtHeight(new_w);
		}

	    if (XtBorderWidth(new_w) != XtBorderWidth(old))
		{
		    g.request_mode |= CWBorderWidth;
		    g.border_width = XtBorderWidth(new_w);
		}

	    fw->form.processing_constraints = TRUE;
	    SortChildren(fw);
	    ChangeIfNeeded(fw, new_w, &g);
	    PlaceChildren(fw, new_w, &g);
	    new_w->core.x ++ ; /* Force a call to the GeometryManager.
				  There are cases where a change in
				  constraints does not result in a change
				  in the geometry of new_w. As a result,
				  processing_constraint stays True and
				  everything is screwed up...
				  Note that this change in x is only
				  temporary since Xt will reset it to
				  its old value before calling the GM */
	}

    return (False);
}




/************************************************************************
 *
 *  Initialize
 *	The form widget specific initialization.
 *
 ************************************************************************/
static void
Initialize(
        Widget rw,		/* unused */
        Widget nw,
        ArgList args,		/* unused */
        Cardinal * num_args )	/* unused */
{
        XmFormWidget new_w = (XmFormWidget) nw ;

	new_w->form.first_child = NULL;


	if (new_w->form.fraction_base == 0)
	{
		new_w->form.fraction_base = 100;
		XmeWarning( (Widget) new_w, MESSAGE1);
	}

	new_w->form.processing_constraints = FALSE;

	/* Set up for shadow drawing */

	new_w->bulletin_board.old_width = XtWidth(new_w);
	new_w->bulletin_board.old_height = XtHeight(new_w);
	new_w->bulletin_board.old_shadow_thickness =
		new_w->manager.shadow_thickness;
}

/************************************************************************
 *
 *  ConstraintInitialize
 *
 ************************************************************************/
static void
ConstraintInitialize(
        Widget req,		/* unused */
        Widget new_w,
        ArgList args,		/* unused */
        Cardinal * num_args )	/* unused */
{
    XmFormConstraint nc;
    int i;

    if (!XtIsRectObj(new_w)) return;

    nc = GetFormConstraint(new_w);

    /*  Validate the attachement type.  */

    for (i = FIRST_ATTACHMENT; i < (LAST_ATTACHMENT + 1); i++){
	if(!XmRepTypeValidValue( XmRID_ATTACHMENT, nc->att[i].type, new_w))
	    {
		nc->att[i].type = XmATTACH_NONE;
	    }
	if ((nc->att[i].type == XmATTACH_WIDGET) ||
	    (nc->att[i].type == XmATTACH_OPPOSITE_WIDGET)) {

	    while ((nc->att[i].w) && !SIBLINGS(nc->att[i].w, new_w)) {
		nc->att[i].w = XtParent(nc->att[i].w);
	    }
	}
	nc->att[i].value = nc->att[i].tempValue = 0 ;
    }

    /* set the preferred geometry to some magic value that will help
       us find in ChangeManaged that it's the first time this kid is
       going thru layout */
    /* this code used to set the current geometry as preferred,
       I don't see why it was needed, since the preferred field are
       always used after the changemanaged is called */
    nc->preferred_width = XmINVALID_DIMENSION;
    nc->preferred_height = XmINVALID_DIMENSION;
}




/************************************************************************
 *
 *  CheckConstraints
 *
 ************************************************************************/
static void
CheckConstraints(
        Widget w )
{
  XmFormConstraint c = GetFormConstraint(w);
  XmFormWidget fw = (XmFormWidget) XtParent(w);
  XmFormAttachment left = &c->att[LEFT], right = &c->att[RIGHT],
  top = &c->att[TOP], bottom = &c->att[BOTTOM] ;
  XmFormAttachment a;
  int which;
  int wid, ht;

  if (left->type == XmATTACH_NONE && right->type == XmATTACH_NONE)
    {
      if (fw->form.rubber_positioning)
	left->type = right->type = XmATTACH_SELF;
      else
	{
	  left->type = XmATTACH_FORM;
	  left->offset = w->core.x;
	}
    }

  if (top->type == XmATTACH_NONE && bottom->type == XmATTACH_NONE)
    {
      if (fw->form.rubber_positioning)
	top->type = bottom->type = XmATTACH_SELF;
      else
	{
	  top->type = XmATTACH_FORM;
	  top->offset = w->core.y;
	}
    }

  for (which = FIRST_ATTACHMENT; which < (LAST_ATTACHMENT + 1); which++)
    {
      a = &c->att[which];

      switch (a->type)
	{
	case XmATTACH_NONE:
	case XmATTACH_FORM:
	case XmATTACH_OPPOSITE_FORM:
	  a->w = NULL;
	  a->percent = 0;
	  break;

	case XmATTACH_SELF:
	  a->offset = 0;
	  a->w = NULL;
	  a->type = XmATTACH_POSITION;
	  a->percent = 0;	/* default in case wid or ht are 0, although
				   behavior in that case is poorly defined */

	  wid = w->core.x + w->core.width
	    + (2 * w->core.border_width);
	  ht = w->core.y + w->core.height
	    + (2 * w->core.border_width);

	  if (wid < fw->core.width)
	    wid = fw->core.width;
	  if (ht  < fw->core.height)
	    ht  = fw->core.height;

	  switch (which)
	    {
	    case LEFT:
		if (wid != 0)
		    a->percent = (w->core.x
				  * fw->form.fraction_base) / wid;
		break;

	    case TOP:
		if (ht != 0)
		    a->percent = (w->core.y
				  * fw->form.fraction_base) / ht;
		break;

	    case RIGHT:
		if (wid != 0)
		    a->percent = ((w->core.x + w->core.width +
				   2 * w->core.border_width) *
				  fw->form.fraction_base) / wid;
		break;

	    case BOTTOM:
		if (ht != 0)
		    a->percent = ((w->core.y + w->core.height +
				   2 * w->core.border_width) *
				  fw->form.fraction_base) / ht;
		break;
	    }
	  break;

	case XmATTACH_POSITION:
	  a->w = NULL;
	  break;

	case XmATTACH_WIDGET:
	case XmATTACH_OPPOSITE_WIDGET:
	  a->percent = 0;
	  break;
	}
    }
}




/************************************************************************
 *
 *  SortChildren
 *
 ************************************************************************/
static void
SortChildren(
        XmFormWidget fw )
{
	int i, j;
	Widget child = NULL;
	XmFormConstraint c = NULL, c1 = NULL;
	int sortedCount = 0;
	Widget last_child, att_widget;
	Boolean sortable;


	fw->form.first_child = NULL;

	for (i = 0; i < fw->composite.num_children; i++)
	{
		child = fw->composite.children[i];
		if (!XtIsRectObj(child))
			continue;
		c = GetFormConstraint(child);

		if (XtIsManaged(child))
		{
			c->sorted = False;
			c->next_sibling = NULL;
		}
		else
		{
			c->next_sibling = fw->form.first_child;
			fw->form.first_child = child;
			c->sorted = True;
			sortedCount++;
		}

		CheckConstraints(child);
	}


	/* THIS IS PROBABLY WRONG AND SHOULD BE FIXED SOMEDAY             */
	/* WHY SHOULD UNMANAGED CHILDREN BE ALLOWED AS ATTACHMENT POINTS  */
	/* FOR MANAGED CHILDREN???                                        */

	/* While there are unsorted children, find one with only sorted  */
	/* predecessors and put it in the list.  This algorithm works    */
	/* particularly well if the order is already correct             */

	last_child = NULL;

	for ( ; sortedCount != fw->composite.num_children; sortedCount++)
	{
		sortable = False;

		for (i = 0; !sortable && i < fw->composite.num_children; i++)
		{
			child = fw->composite.children[i];
			if (!XtIsRectObj(child))
				continue;
			c = GetFormConstraint(child);

			if (c->sorted)
				continue;

			sortable = True;

			for (j = FIRST_ATTACHMENT; j < (LAST_ATTACHMENT + 1); j++)
			{
				if ((c->att[j].type == XmATTACH_WIDGET) ||
				(c->att[j].type == XmATTACH_OPPOSITE_WIDGET))
				{
					att_widget = c->att[j].w;

					if ((SIBLINGS(att_widget, child)) &&
						(XtIsRectObj(att_widget)))
					{
						c1 = GetFormConstraint (att_widget);

						if (!c1->sorted)
							sortable = False;
					}
				}
			}
		}

		if (sortable)
		{
			/*  We have found a sortable child...add to sorted list.  */

			if (last_child == NULL)
			{
				c->next_sibling = fw->form.first_child;
				fw->form.first_child = child;
			}
			else
			{
				c1 = GetFormConstraint(last_child);
				c->next_sibling = c1->next_sibling;
				c1->next_sibling = child;
			}

			last_child = child;
			c->sorted = True;
		}
	}

  /*Add other children that haven't been sorted*/
	for (i = 0; i < fw->composite.num_children; i++)
	{
		child = fw->composite.children[i];

    c = GetFormConstraint(child);

    if (!XtIsRectObj(child) || c->sorted)
			continue;

    if(!c->sorted) {
			if (last_child == NULL)
			{
				c->next_sibling = fw->form.first_child;
				fw->form.first_child = child;
			  }
			else
			{
				c1 = GetFormConstraint(last_child);
				c->next_sibling = c1->next_sibling;
				c1->next_sibling = child;
			}

			last_child = child;
			c->sorted = True;
		}
  }
}




/************************************************************************
 *
 *  CalcEdgeValues
 *
 ************************************************************************/
static void
CalcEdgeValues(
        Widget w,
        Boolean really,
        Widget instigator,
        XtWidgetGeometry *inst_geometry,
        Dimension *form_width,
        Dimension *form_height )
{
	XmFormConstraint c = GetFormConstraint (w);
	XmFormWidget fw = (XmFormWidget) XtParent (w);
	XmFormAttachment left = &c->att[LEFT], right = &c->att[RIGHT],
		top = &c->att[TOP], bottom = &c->att[BOTTOM] ;
	Dimension width, height, border_width;

	if (w == instigator)
	{
		if (inst_geometry->request_mode & CWWidth)
			width = inst_geometry->width;
		else
			width = w->core.width;
		if (inst_geometry->request_mode & CWHeight)
			height = inst_geometry->height;
		else
			height = w->core.height;
		if (inst_geometry->request_mode & CWBorderWidth)
			border_width = inst_geometry->border_width;
		else
			border_width = w->core.border_width;
	}
	else if (!fw->form.processing_constraints)
	{
		/*
		* If we just use the widget's current geometry we will
		* effectively be grow only.  That would not be correct.
		*
		* Instead we will use our idea of the child's preferred
		* size.
		*/
		width = c->preferred_width;
		height = c->preferred_height;
		border_width = w->core.border_width;
	}
	else
	{
		width = w->core.width;
		height = w->core.height;
		border_width = w->core.border_width;
	}

	width += border_width * 2;
	height += border_width * 2;


	if (width <= 0) width = 1;
	if (height <= 0) height = 1;

	if (left->type != XmATTACH_NONE)
	{
		if (right->type != XmATTACH_NONE)	    /* LEFT and right are attached */
		{
			CalcEdgeValue(fw, w, width, border_width,
				LEFT, really, form_width, form_height);
			CalcEdgeValue(fw, w, width, border_width,
				RIGHT, really, form_width, form_height);
		}

		else 	/*  LEFT attached, compute right  */
		{
			CalcEdgeValue(fw, w, width, border_width,
				LEFT, really, form_width, form_height);
			ComputeAttachment(fw, w, width, border_width, RIGHT, really,
			form_width, form_height);
		}
	}
	else
	{
		if (right->type != XmATTACH_NONE)    /* RIGHT attached, compute left */
		{
		CalcEdgeValue(fw, w, width, border_width,
			RIGHT, really, form_width, form_height);
		ComputeAttachment(fw, w, width, border_width,
			LEFT, really, form_width, form_height);
		}
	}

	if (top->type != XmATTACH_NONE)
	{
		if (bottom->type != XmATTACH_NONE)   /* TOP and bottom are attached */
		{
			CalcEdgeValue(fw, w, height, border_width,
				TOP, really, form_width, form_height);
			CalcEdgeValue(fw, w, height, border_width,
				BOTTOM, really, form_width, form_height);
		}
		else	/* TOP attached, compute bottom */
		{
			CalcEdgeValue(fw, w, height, border_width,
				TOP, really, form_width, form_height);
			ComputeAttachment(fw, w, height, border_width,
				BOTTOM, really, form_width, form_height);
		}
	}
	else
	{
		if (bottom->type != XmATTACH_NONE)   /* BOTTOM attached, compute top */
		{
			CalcEdgeValue(fw, w, height, border_width,
				BOTTOM, really, form_width, form_height);
			ComputeAttachment(fw, w, height, border_width,
				TOP, really, form_width, form_height);
		}
	}
}



/*********************************************************************
 *
 * CheckBottomBase
 *
 *********************************************************************/
static float
CheckBottomBase(
        Widget sibling,
        Boolean opposite )
{
	XmFormWidget fw = (XmFormWidget) sibling->core.parent;
	XmFormConstraint c = GetFormConstraint(sibling);
	Boolean flag = FALSE;
	float return_val;

	if (!opposite)
	{
		switch (c->att[TOP].type)
		{
			case XmATTACH_POSITION:
				return_val = (float) c->att[TOP].percent /
				    (float) fw->form.fraction_base ;
			break;
			case XmATTACH_NONE:
				switch (c->att[BOTTOM].type)
				{
					case XmATTACH_FORM:
						return_val = 1.0;
					break;
					case XmATTACH_POSITION:
						return_val = (float) c->att[BOTTOM].percent /
							(float) fw->form.fraction_base ;
					break;
					case XmATTACH_OPPOSITE_WIDGET:
						flag = TRUE;
					case XmATTACH_WIDGET:
						if (SIBLINGS(c->att[BOTTOM].w, sibling))
							return_val =
								CheckBottomBase(c->att[BOTTOM].w, flag);
						else
						{
							if (flag)
								return_val = 0.0;
							else
								return_val = 1.0;
						}
					break;
					default:
						return_val = 0.0;
					break;
				}
			break;
			case XmATTACH_OPPOSITE_FORM:
				return_val = 1.0;
			break;
			default:
				return_val = 0.0;
			break;
		}
	}
	else
	{
		switch(c->att[BOTTOM].type)
		{
			case XmATTACH_NONE:
				if (c->att[TOP].type == XmATTACH_POSITION)
					return_val = (float) c->att[TOP].percent /
						(float) fw->form.fraction_base;
				else
					return_val = 0.0;
			break;
			case XmATTACH_POSITION:
				return_val = (float) c->att[BOTTOM].percent /
				    (float) fw->form.fraction_base;
			break;
			case XmATTACH_OPPOSITE_WIDGET:
				flag = TRUE;
			case XmATTACH_WIDGET:
				if (SIBLINGS(c->att[BOTTOM].w, sibling))
					return_val =
						CheckBottomBase(c->att[BOTTOM].w, flag);
				else
				{
					if (flag)
						return_val = 0.0;
					else
						return_val = 1.0;
				}
			break;
			case XmATTACH_FORM:
				return_val = 1.0;
			break;
			default:
				return_val = 0.0;
			break;
		}
	}

	return(return_val);
}


/*********************************************************************
 *
 * CheckRightBase
 *
 *********************************************************************/
static float
CheckRightBase(
        Widget sibling,
        Boolean opposite )
{
	XmFormWidget fw = (XmFormWidget) sibling->core.parent;
	XmFormConstraint c = GetFormConstraint(sibling);
	Boolean flag = FALSE;
	float return_val;

	if (!opposite)
	{
		switch (c->att[LEFT].type)
		{
			case XmATTACH_POSITION:
				return_val =  (float) c->att[LEFT].percent /
				    (float) fw->form.fraction_base;
			break;
			case XmATTACH_NONE:
				switch (c->att[RIGHT].type)
				{
					case XmATTACH_FORM:
						return_val = 1.0;
					break;
					case XmATTACH_POSITION:
						return_val = (float) c->att[RIGHT].percent /
							(float) fw->form.fraction_base ;
					break;
					case XmATTACH_OPPOSITE_WIDGET:
						flag = TRUE;
					case XmATTACH_WIDGET:
						if (SIBLINGS(c->att[RIGHT].w, sibling))
							return_val =
								CheckRightBase(c->att[RIGHT].w, flag);
						else
						{
							if (flag)
								return_val =  0.0;
							else
								return_val = 1.0;
						}
					break;
					default:
						return_val =  0.0;
					break;
				}
			break;
			case XmATTACH_OPPOSITE_FORM:
				return_val = 1.0;
			break;
			default:
				return_val = 0.0;
			break;
		}
	}
	else
	{
		switch(c->att[RIGHT].type)
		{
			case XmATTACH_NONE:
				if (c->att[LEFT].type == XmATTACH_POSITION)
					return_val = (float) c->att[LEFT].percent /
						(float) fw->form.fraction_base ;
				else
					return_val = 0.0;
			break;
			case XmATTACH_POSITION:
				return_val = (float) c->att[RIGHT].percent /
				    (float) fw->form.fraction_base;
			break;
			case XmATTACH_OPPOSITE_WIDGET:
				flag = TRUE;
			case XmATTACH_WIDGET:
				if (SIBLINGS(c->att[RIGHT].w, sibling))
					return_val =
						CheckRightBase(c->att[RIGHT].w, flag);
				else
				{
					if (flag)
						return_val =  0.0;
					else
						return_val = 1.0;
				}
			break;
			case XmATTACH_FORM:
				return_val = 1.0;
			break;
			default:
				return_val = 0.0;
			break;
		}
	}

	return(return_val);
}

/*********************************************************************
 *
 * CheckLeftBase
 *
 *********************************************************************/
static float
CheckLeftBase(
        Widget sibling,
        Boolean opposite )
{
	XmFormWidget fw = (XmFormWidget) sibling->core.parent;
	XmFormConstraint c = GetFormConstraint(sibling);
	Boolean flag = FALSE;
	float return_val;

	if (!opposite)
	{
		switch (c->att[RIGHT].type)
		{
			case XmATTACH_POSITION:
				return_val =  (float) c->att[RIGHT].percent /
				    (float) fw->form.fraction_base;
			break;
			case XmATTACH_NONE:
				switch (c->att[LEFT].type)
				{
					case XmATTACH_FORM:
						return_val = 1.0;
					break;
					case XmATTACH_POSITION:
						return_val = (float) c->att[LEFT].percent /
							(float) fw->form.fraction_base ;
					break;
					case XmATTACH_OPPOSITE_WIDGET:
						flag = TRUE;
					case XmATTACH_WIDGET:
						if (SIBLINGS(c->att[LEFT].w, sibling))
							return_val =
								CheckLeftBase(c->att[LEFT].w, flag);
						else
						{
							if (flag)
								return_val =  0.0;
							else
								return_val = 1.0;
						}
					break;
					default:
						return_val =  0.0;
					break;
				}
			break;
			case XmATTACH_OPPOSITE_FORM:
				return_val = 1.0;
			break;
			default:
				return_val = 0.0;
			break;
		}
	}
	else
	{
		switch(c->att[LEFT].type)
		{
			case XmATTACH_NONE:
				if (c->att[RIGHT].type == XmATTACH_POSITION)
					return_val = (float) c->att[RIGHT].percent /
						(float) fw->form.fraction_base ;
				else
					return_val = 0.0;
			break;
			case XmATTACH_POSITION:
				return_val = (float) c->att[LEFT].percent /
				    (float) fw->form.fraction_base;
			break;
			case XmATTACH_OPPOSITE_WIDGET:
				flag = TRUE;
			case XmATTACH_WIDGET:
				if (SIBLINGS(c->att[LEFT].w, sibling))
					return_val =
						CheckLeftBase(c->att[LEFT].w, flag);
				else
				{
					if (flag)
						return_val =  0.0;
					else
						return_val = 1.0;
				}
			break;
			case XmATTACH_FORM:
				return_val = 1.0;
			break;
			default:
				return_val = 0.0;
			break;
		}
	}

	return(return_val);
}

/*********************************************************************
 *
 *  CalcEdgeValue
 *     Note that Left attachment apply to the right side in a
 *     Right-to-Left environment. Slightly confusing, but it is more a
 *     matter of switching right and left in the computing.
 *
 *********************************************************************/
static void
CalcEdgeValue(
        XmFormWidget fw,
        Widget w,
        Dimension size,
        Dimension border_width,	/* unused */
        int which,
        Boolean really,
        Dimension *fwidth,
        Dimension *fheight )
{
  float scale;
  XmFormAttachment att = GetFormConstraint(w)->att;
  XmFormAttachment a = att + which;
  XmFormConstraint c;
  XmFormAttachment ref;
  float factor;
  int temp1, temp2, temp3;
  int ctype;


  ctype = a->type;

  if ((ctype == XmATTACH_WIDGET) && !(SIBLINGS(a->w, w)))
    ctype = XmATTACH_FORM;

  if ((ctype == XmATTACH_OPPOSITE_WIDGET) && !(SIBLINGS(a->w, w)))
    ctype = XmATTACH_OPPOSITE_FORM;

  if (LayoutIsRtoLM(fw))
    switch (ctype)
      {
      case XmATTACH_FORM:
	a->w = NULL;
	a->percent = 0;

	switch (which)
	  {
	  case RIGHT:
	  case TOP:
	    AssignValue(a, GetFormOffset(fw, which, att));
	    break;

	  case LEFT:
	    if (fwidth != NULL) {
	      if ((att + RIGHT)->type == XmATTACH_NONE) {
		temp1 = *fwidth - (GetFormOffset(fw, which, att));
		temp2 = temp1 - size;
		if (temp2 < 0) {
		  *fwidth += abs(temp2);
		  AssignValue(a, (temp1 + abs(temp2)));
		} else
		  AssignValue(a, temp1);
	      } else {
		temp1 = *fwidth - (GetFormOffset(fw, which, att));
		temp2 = Value(att + RIGHT);
		temp3 = temp1 - temp2 - size;
		if (temp3 < 0) {
		  *fwidth += abs(temp3);
		  AssignValue(a, (*fwidth - size - GetFormOffset(fw, which, att)));
		} else
		  AssignValue(a, temp1);
	      }
	    } else
	      AssignValue(a, fw->core.width - GetFormOffset(fw,which,att));
	    break;
	  case BOTTOM:
	    if (fheight != NULL) {
	      if ((att + TOP)->type == XmATTACH_NONE) {
		temp1 = *fheight - GetFormOffset(fw, which, att);
		temp2 = temp1 - size;
		if (temp2 < 0) {
		  *fheight += abs(temp2);
		  AssignValue(a, (temp1 + abs(temp2)));
		} else
		  AssignValue(a, temp1);
	      } else {
		temp1 = *fheight - GetFormOffset(fw, which, att);
		temp2 = Value(att + TOP);
		temp3 = temp1 - temp2 - size;
		if (temp3 < 0) {
		  *fheight += abs(temp3);
		  AssignValue(a, (*fheight - size - GetFormOffset(fw, which, att)));
		} else
		  AssignValue(a, temp1);
	      }
	    } else
	      AssignValue(a, fw->core.height - GetFormOffset(fw,which,att));
	    break;
	  }
	return;

      case XmATTACH_OPPOSITE_FORM:
	a->w = NULL;
	a->percent = 0;

	switch (which)
	  {
	  case RIGHT:
	    if (fwidth) {
	      temp1 = *fwidth + GetFormOffset(fw, which, att);
	      if (temp1 < 0) {
		*fwidth += abs(temp1);
		temp1 = 0;
	      }
	      AssignValue(a, temp1);
	    } else
	      AssignValue(a, fw->core.width + GetFormOffset(fw, which, att));
	    break;

	  case TOP:
	    if (fheight) {
	      temp1 = *fheight + GetFormOffset(fw, which, att);
	      if (temp1 < 0) {
		*fheight += abs(temp1);
		temp1 = 0;
	      }
	      AssignValue(a, temp1);
	    } else
	      AssignValue(a, fw->core.height + GetFormOffset(fw, which, att));
	    break;

	  case LEFT:
	  case BOTTOM:
	    AssignValue(a, -(GetFormOffset(fw, which, att)));
	    break;
	  }
	return;

      case XmATTACH_POSITION:
	scale = ((float) a->percent) / fw->form.fraction_base;
	a->w = 0;

	switch (which)
	  {
	  case RIGHT:
	    scale = 1.0 - scale;
	    if (fwidth != NULL)
	      AssignValue(a, (*fwidth * scale + 0.5) +
			  GetFormOffset(fw, which, att));
	    else
	      AssignValue(a, (fw->core.width * scale + 0.5) +
			  GetFormOffset(fw, which, att));
	    break;
	  case TOP:
	    if (fheight != NULL)
	      AssignValue(a, (*fheight * scale + 0.5) +
			  GetFormOffset(fw, which, att));
	    else
	      AssignValue(a, (fw->core.height * scale + 0.5) +
			  GetFormOffset(fw, which, att));
	    break;
	  case LEFT:
	    scale = 1.0 - scale;
	    if (fwidth != NULL)
	      if ((att + RIGHT)->type != XmATTACH_NONE) {
		temp1 = (int) (((*fwidth  * scale) + 0.5) -
			       GetFormOffset(fw, which, att));
		temp2 = Value(att + RIGHT);
		temp3 = temp1 - temp2 - size;
		if (temp3 < 0) {
		  if (!scale)
		    scale = 1.0;

		  *fwidth += (Dimension) (((1.0 / scale) * abs(temp3)) + 0.5);
		}
		AssignValue(a, (temp2 + size));
	      } else
		AssignValue(a, (*fwidth * scale + 0.5) -
			    GetFormOffset(fw, which, att));
	    else
	      AssignValue(a, (fw->core.width * scale + 0.5) -
			  GetFormOffset(fw, which, att));
	    break;
	  case BOTTOM:
	    if (fheight != NULL)
	      if ((att + TOP)->type != XmATTACH_NONE) {
		temp1 = (int) (((*fheight  * scale) + 0.5)
			       - GetFormOffset(fw, which, att));
		temp2 = Value(att + TOP);
		temp3 = temp1 - temp2 - size;
		if (temp3 < 0) {
		  if (!scale)
		    scale = 1.0;

		  *fheight += (Dimension) (((1.0 / scale) * abs(temp3)) + 0.5);
		}
		AssignValue(a, (temp2 + size));
	      } else
		AssignValue(a, (*fheight * scale + 0.5) -
			    GetFormOffset(fw, which, att));
	    else
	      AssignValue(a, (fw->core.height * scale + 0.5) -
			  GetFormOffset(fw, which, att));
	  }
	return;

      case XmATTACH_WIDGET:
	a->percent = 0;
	c = GetFormConstraint(a->w);

	switch (which)
	  {
	  case RIGHT:
	    ref = &c->att[LEFT];
	    AssignValue(a, Value(ref) + GetFormOffset(fw, which, att));
	    break;

	  case TOP:
	    ref = &c->att[BOTTOM];
	    AssignValue(a, Value(ref) + GetFormOffset(fw, which, att));
	    break;

	  case LEFT:
	    ref = &c->att[RIGHT];
	    if (att[RIGHT].type != XmATTACH_NONE) {
	      temp1 = Value(ref) - GetFormOffset(fw, which, att);
	      temp2 = temp1 - Value(&(att[RIGHT]));
	      temp3 = temp2 - size;
	      if ((fwidth) && (temp3 < 0)) {
		factor = CheckLeftBase(a->w, FALSE);
		*fwidth += (Dimension) ((factor * abs(temp3)) + 0.5);
		temp1 = Value(&(att[RIGHT])) + size;
	      }
	      AssignValue(a, temp1);
	    } else {
	      temp1 = Value(ref) - GetFormOffset(fw, which, att);
	      AssignValue(a, temp1);
	    }
	    break;

	  case BOTTOM:
	    ref = &c->att[TOP];
	    if (att[TOP].type != XmATTACH_NONE) {
	      temp1 = Value(ref) - GetFormOffset(fw, which, att);
	      temp2 = temp1 - Value((&att[TOP]));
	      temp3 = temp2 - size;
	      if ((fheight) && (temp3 < 0)) {
		factor = CheckBottomBase(a->w, FALSE);
		*fheight += (Dimension) ((factor * abs(temp3)) + 0.5);
		temp1 = Value((&att[TOP])) + size;
	      }
	      AssignValue(a, temp1);
	    } else {
	      temp1 = Value(ref) - GetFormOffset(fw, which, att);
	      AssignValue(a, temp1);
	    }
	    break;
	  }
	return;

      case XmATTACH_OPPOSITE_WIDGET:
	a->percent = 0;
	c = GetFormConstraint(a->w);

	switch (which)
	  {
	  case RIGHT:
	    ref = &c->att[RIGHT];
	    AssignValue(a, Value(ref) + GetFormOffset(fw, which, att));
	    break;

	  case TOP:
	    ref = &c->att[TOP];
	    AssignValue(a, Value(ref) + GetFormOffset(fw, which, att));
	    break;

	  case LEFT:
	    ref = &c->att[LEFT];
	    AssignValue(a, Value(ref) - GetFormOffset(fw, which, att));
	    break;

	  case BOTTOM:
	    ref = &c->att[BOTTOM];
	    AssignValue(a, Value(ref) - GetFormOffset(fw, which, att));
	    break;
	  }
	return;
      }
  else /* Left to Right environment */
    switch (ctype)
      {
      case XmATTACH_FORM:
	a->w = NULL;
	a->percent = 0;

	switch (which)
	  {
	  case LEFT:
	  case TOP:
	    AssignValue(a, GetFormOffset(fw, which, att));
	    break;

	  case RIGHT:
	    if (fwidth != NULL) {
	      if ((att + LEFT)->type == XmATTACH_NONE) {
		temp1 = *fwidth - (GetFormOffset(fw, which, att));
		temp2 = temp1 - size;
		if (temp2 < 0) {
		  *fwidth += abs(temp2);
		  AssignValue(a, (temp1 + abs(temp2)));
		} else
		  AssignValue(a, temp1);
	      } else {
		temp1 = *fwidth - (GetFormOffset(fw, which, att));
		temp2 = Value(att + LEFT);
		temp3 = temp1 - temp2 - size;
		if (temp3 < 0) {
		  *fwidth += abs(temp3);
		  AssignValue(a, (*fwidth - size - GetFormOffset(fw, which, att)));
		} else
		  AssignValue(a, temp1);
	      }
	    } else
	      AssignValue(a, fw->core.width - GetFormOffset(fw,which,att));
	    break;
	  case BOTTOM:
	    if (fheight != NULL) {
	      if ((att + TOP)->type == XmATTACH_NONE) {
		temp1 = *fheight - GetFormOffset(fw, which, att);
		temp2 = temp1 - size;
		if (temp2 < 0) {
		  *fheight += abs(temp2);
		  AssignValue(a, (temp1 + abs(temp2)));
		} else
		  AssignValue(a, temp1);
	      } else {
		temp1 = *fheight - GetFormOffset(fw, which, att);
		temp2 = Value(att + TOP);
		temp3 = temp1 - temp2 - size;
		if (temp3 < 0) {
		  *fheight += abs(temp3);
		  AssignValue(a, (*fheight - size - GetFormOffset(fw, which, att)));
		} else
		  AssignValue(a, temp1);
	      }
	    } else
	      AssignValue(a, fw->core.height - GetFormOffset(fw,which,att));
	    break;
	  }
	return;

      case XmATTACH_OPPOSITE_FORM:
	a->w = NULL;
	a->percent = 0;

	switch (which)
	  {
	  case LEFT:
	    if (fwidth) {
	      temp1 = *fwidth + GetFormOffset(fw, which, att);
	      if (temp1 < 0) {
		*fwidth += abs(temp1);
		temp1 = 0;
	      }
	      AssignValue(a, temp1);
	    } else
	      AssignValue(a, fw->core.width + GetFormOffset(fw, which, att));
	    break;

	  case TOP:
	    if (fheight) {
	      temp1 = *fheight + GetFormOffset(fw, which, att);
	      if (temp1 < 0) {
		*fheight += abs(temp1);
		temp1 = 0;
	      }
	      AssignValue(a, temp1);
	    } else
	      AssignValue(a, fw->core.height + GetFormOffset(fw, which, att));
	    break;

	  case RIGHT:
	  case BOTTOM:
	    AssignValue(a, -(GetFormOffset(fw, which, att)));
	    break;
	  }
	return;

      case XmATTACH_POSITION:
	scale = ((float) a->percent) / fw->form.fraction_base;
	a->w = 0;

	switch (which)
	  {
	  case LEFT:
	    if (fwidth != NULL)
	      AssignValue(a, (*fwidth * scale + 0.5) + GetFormOffset(fw, which, att));
	    else
	      AssignValue(a, (fw->core.width * scale + 0.5) +
			  GetFormOffset(fw, which, att));
	    break;
	  case TOP:
	    if (fheight != NULL)
	      AssignValue(a, (*fheight * scale + 0.5) +
			  GetFormOffset(fw, which, att));
	    else
	      AssignValue(a, (fw->core.height * scale + 0.5) +
			  GetFormOffset(fw, which, att));
	    break;
	  case RIGHT:
	    if (fwidth != NULL)
	      if ((att + LEFT)->type != XmATTACH_NONE) {
		temp1 = (int) (((*fwidth  * scale) + 0.5) -
			       GetFormOffset(fw, which, att));
		temp2 = Value(att + LEFT);
		temp3 = temp1 - temp2 - size;
		if (temp3 < 0) {
		  if (!scale)
		    scale = 1.0;

		  *fwidth += (Dimension) (((1.0 / scale) * abs(temp3)) + 0.5);
		}
		AssignValue(a, (temp2 + size));
	      } else
		AssignValue(a, (*fwidth * scale + 0.5) -
			    GetFormOffset(fw, which, att));
	    else
	      AssignValue(a, (fw->core.width * scale + 0.5) -
			  GetFormOffset(fw, which, att));
	    break;
	  case BOTTOM:
	    if (fheight != NULL)
	      if ((att + TOP)->type != XmATTACH_NONE) {
		temp1 = (int) (((*fheight  * scale) + 0.5)
			       - GetFormOffset(fw, which, att));
		temp2 = Value(att + TOP);
		temp3 = temp1 - temp2 - size;
		if (temp3 < 0) {
		  if (!scale)
		    scale = 1.0;

		  *fheight += (Dimension) (((1.0 / scale) * abs(temp3)) + 0.5);
		}
		AssignValue(a, (temp2 + size));
	      } else
		AssignValue(a, (*fheight * scale + 0.5) -
			    GetFormOffset(fw, which, att));
	    else
	      AssignValue(a, (fw->core.height * scale + 0.5) -
			  GetFormOffset(fw, which, att));
	  }
	return;

      case XmATTACH_WIDGET:
	a->percent = 0;
	c = GetFormConstraint(a->w);

	switch (which)
	  {
	  case LEFT:
	    ref = &c->att[RIGHT];
	    AssignValue(a, Value(ref) + GetFormOffset(fw, which, att));
	    break;

	  case TOP:
	    ref = &c->att[BOTTOM];
	    AssignValue(a, Value(ref) + GetFormOffset(fw, which, att));
	    break;

	  case RIGHT:
	    ref = &c->att[LEFT];
	    if (att[LEFT].type != XmATTACH_NONE) {
	      temp1 = Value(ref) - GetFormOffset(fw, which, att);
	      temp2 = temp1 - Value(&(att[LEFT]));
	      temp3 = temp2 - size;
	      if ((fwidth) && (temp3 < 0)) {
		factor = CheckRightBase(a->w, FALSE);
		*fwidth += (Dimension) ((factor * abs(temp3)) + 0.5);
		temp1 = Value(&(att[LEFT])) + size;
	      }
	      AssignValue(a, temp1);
	    } else {
	      temp1 = Value(ref) - GetFormOffset(fw, which, att);
	      AssignValue(a, temp1);
	    }
	    break;

	  case BOTTOM:
	    ref = &c->att[TOP];
	    if (att[TOP].type != XmATTACH_NONE) {
	      temp1 = Value(ref) - GetFormOffset(fw, which, att);
	      temp2 = temp1 - Value((&att[TOP]));
	      temp3 = temp2 - size;
	      if ((fheight) && (temp3 < 0)) {
		factor = CheckBottomBase(a->w, FALSE);
		*fheight += (Dimension) ((factor * abs(temp3)) + 0.5);
		temp1 = Value((&att[TOP])) + size;
	      }
	      AssignValue(a, temp1);
	    } else {
	      temp1 = Value(ref) - GetFormOffset(fw, which, att);
	      AssignValue(a, temp1);
	    }
	    break;
	  }
	return;

      case XmATTACH_OPPOSITE_WIDGET:
	a->percent = 0;
	c = GetFormConstraint(a->w);

	switch (which)
	  {
	  case LEFT:
	    ref = &c->att[LEFT];
	    AssignValue(a, Value(ref) + GetFormOffset(fw, which, att));
	    break;

	  case TOP:
	    ref = &c->att[TOP];
	    AssignValue(a, Value(ref) + GetFormOffset(fw, which, att));
	    break;

	  case RIGHT:
	    ref = &c->att[RIGHT];
	    AssignValue(a, Value(ref) - GetFormOffset(fw, which, att));
	    break;

	  case BOTTOM:
	    ref = &c->att[BOTTOM];
	    AssignValue(a, Value(ref) - GetFormOffset(fw, which, att));
	    break;
	  }
	return;
      }

}




/************************************************************************
 *
 *  ComputeAttachment
 *     Note that Left attachment apply to the right side in a
 *     Right-to-Left environment. Slightly confusing, but it is more a
 *     matter of switching right and left in the computing.
 *
 ************************************************************************/
static void
ComputeAttachment(
        XmFormWidget fw,
        Widget w,
        Dimension size,
        Dimension border_width,	/* unused */
        int which,
        Boolean really,
        Dimension *fwidth,
        Dimension *fheight )
{
   XmFormConstraint c = GetFormConstraint(w);
   XmFormAttachment a = &c->att[which];
   int temp;

   if (LayoutIsRtoLM(fw))
     switch (which)
       {
       case RIGHT:
	 temp =  Value(&c->att[LEFT]) - size;
	 if ((fwidth != NULL) && (temp < 0))
	   {
	     *fwidth += abs(temp);
	     temp = 0;
	   }
         AssignValue(a,temp);
	 break;

       case LEFT:
	 temp = Value(&c->att[RIGHT]) + size;
	 if ((fwidth != NULL) && (temp > 0) && (temp > *fwidth))
	   *fwidth += (temp - *fwidth);
         AssignValue(a,temp);
	 break;

       case TOP:
	 temp = Value(&c->att[BOTTOM]) - size;
	 if ((fheight != NULL) && (temp < 0))
	   {
	     *fheight += abs(temp);
	     temp = 0;
	   }
         AssignValue(a, temp);
	 break;

       case BOTTOM:
	 temp = Value(&c->att[TOP]) + size;
	 if ((fheight != NULL) && (temp > 0) && (temp > *fheight))
	   *fheight += (temp - *fheight);
         AssignValue(a, temp);
	 break;
       }
   else /* Left to right */
     switch (which)
       {
       case LEFT:
	 temp =  Value(&c->att[RIGHT]) - size;
	 if ((fwidth != NULL) && (temp < 0))
	   {
	     *fwidth += abs(temp);
	     temp = 0;
	   }
	 AssignValue(a,temp);
	 break;

       case RIGHT:
	 temp = Value(&c->att[LEFT]) + size;
	 if ((fwidth != NULL) && (temp > 0) && (temp > *fwidth))
	   *fwidth += (temp - *fwidth);
	 AssignValue(a,temp);
	 break;

       case TOP:
	 temp = Value(&c->att[BOTTOM]) - size;
	 if ((fheight != NULL) && (temp < 0))
	   {
	     *fheight += abs(temp);
	     temp = 0;
	   }
	 AssignValue(a, temp);
	 break;

       case BOTTOM:
	 temp = Value(&c->att[TOP]) + size;
	 if ((fheight != NULL) && (temp > 0) && (temp > *fheight))
	   *fheight += (temp - *fheight);
	 AssignValue(a, temp);
	 break;
       }
}




/************************************************************************
 *
 *  GetFormOffset
 *
 ************************************************************************/
static int
GetFormOffset(
        XmFormWidget fw,
        int which,
        XmFormAttachment att )
{
    int o;

    o = att[which].offset;

    if (o == XmINVALID_DIMENSION) {
	switch (att[which].type) {
	case XmATTACH_NONE:
	case XmATTACH_SELF:
	case XmATTACH_POSITION:
	    o = 0;
	    break;

	case XmATTACH_FORM:
	case XmATTACH_OPPOSITE_FORM:
	    if ((which == LEFT) || (which == RIGHT))
		{
		    if (fw->bulletin_board.margin_width
			== XmINVALID_DIMENSION)
			o = fw->form.horizontal_spacing;
		    else
			o = fw->bulletin_board.margin_width;
		}
	    else
		{
		    if (fw->bulletin_board.margin_height
			== XmINVALID_DIMENSION)
			o = fw->form.vertical_spacing;
		    else
			o = fw->bulletin_board.margin_height;
		}
	    break;

	case XmATTACH_WIDGET:
	case XmATTACH_OPPOSITE_WIDGET:
	    if ((which == LEFT) || (which == RIGHT))
		o = fw->form.horizontal_spacing;
	    else
		o = fw->form.vertical_spacing;
	    break;
	}
    }

    return o;
}




/************************************************************************
 *
 *		Application Accessible External Functions
 *
 ************************************************************************/


/************************************************************************
 *
 *  XmCreateForm
 *	Create an instance of a form and return the widget id.
 *
 ************************************************************************/
Widget
XmCreateForm(
        Widget parent,
        char *name,
        ArgList arglist,
        Cardinal argcount )
{
   return(XtCreateWidget(name, xmFormWidgetClass, parent, arglist, argcount));
}

Widget
XmVaCreateForm(
        Widget parent,
        char *name,
        ...)
{
    Widget w;
    va_list var;
    int count;

    Va_start(var,name);
    count = XmeCountVaListSimple(var);
    va_end(var);


    Va_start(var, name);
    w = XmeVLCreateWidget(name,
                         xmFormWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedForm(
        Widget parent,
        char *name,
        ...)
{
    Widget w = NULL;
    va_list var;
    int count;

    Va_start(var, name);
    count = XmeCountVaListSimple(var);
    va_end(var);

    Va_start(var, name);
    w = XmeVLCreateWidget(name,
                         xmFormWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}

/************************************************************************
 *
 *  XmCreateFormDialog
 *	Create an instance of a form dialog and return the widget id.
 *
 ************************************************************************/
Widget
XmCreateFormDialog(
        Widget parent,
        char *name,
        ArgList arglist,
        Cardinal argcount )
{
   return XmeCreateClassDialog (xmFormWidgetClass,
				parent, name, arglist, argcount) ;
}

/************************************************************************
 *
 *  IsWidgetAttachedOneHorizontalSide
 *	Checking the attachments of widget in horizontal direction.
 *  Returns True if only one (left or right) side of widget is attached
 *
 ************************************************************************/
static Boolean IsWidgetAttachedOneHorizontalSide(Widget w)
  { if (w != NULL)
      { XmFormConstraint c = GetFormConstraint(w);
        if (((c->att[LEFT].type == XmATTACH_NONE) &&
             (c->att[RIGHT].type != XmATTACH_NONE)) ||
            ((c->att[LEFT].type != XmATTACH_NONE) &&
             (c->att[RIGHT].type == XmATTACH_NONE)))
          return True;

      }
    return False;
  }


static Boolean IsWidgetAttachedOneVerticalSide(Widget w)
  { if (w != NULL)
      { XmFormConstraint c = GetFormConstraint(w);
        if (((c->att[TOP].type == XmATTACH_NONE) &&
             (c->att[BOTTOM].type != XmATTACH_NONE)) ||
            ((c->att[TOP].type != XmATTACH_NONE) &&
             (c->att[BOTTOM].type == XmATTACH_NONE)))
          return True;

      }
    return False;
  }

