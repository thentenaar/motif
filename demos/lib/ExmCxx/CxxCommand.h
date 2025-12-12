/* $XConsortium: CxxCommand.h /main/4 1995/07/15 20:42:15 drk $ */
/* This may look like C code, but it is really -*- C++ -*- */
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
 * This is the header file for the C++ version of StaticText widget.
 * It contains #ifdef __cplusplus clause to make it possible
 * to include it from a C source file.
 */

#ifndef _CxxCommand_h
#define _CxxCommand_h

#ifdef __cplusplus

#include "CxxString.h"


/**********************************
 * Widget class record definition *
 **********************************/

class ExmCxxCommandClass : public ExmCxxStringClass {

    friend class ExmCxxCommand;

  public:
    
    ExmCxxCommandClass(char*		name,
		      WidgetClass	parent,
		      Cardinal		widgetSize,
		      XtProc		class_init,
		      XtActionList	actions,
		      Cardinal		num_actions,
		      XtResourceList	resources,
		      Cardinal		num_resources,
		      XtInitProc	cxx_cast);
    
    /*
     * static class-methods
     * (must be public : passed as parameter to class_rec static constructor)
     */
    static void		ClassInitialize();

  protected:
    virtual void        class_part_initialize();
};



/***************************
 * Widget class definition *
 ***************************/


struct ExmCxxCommandPart {
  XtCallbackList	activate_callback;

  /* armed is a flag.  If its value is True, then the user has selected the
     widget.  */
  Boolean		armed;

  /* visual_armed is a flag.  If its value is True, then the widget is to
     visually represent its armed state by drawing a certain kind 
     of shadow. */ 
  Boolean		visual_armed;
  
  /* visual_timer holds an integral value representing the time
     (in milliseconds) between when armed is set to True and
     visual_armed is set to True. */ 
  XtIntervalId		visual_timer;
};


class ExmCxxCommand : public ExmCxxString {

    friend class ExmCxxCommandClass;
    friend class ExmCxxCommandActions;
    
  public:
    /*
     * should really be protected but we must reference fields when
     * declaring the static resources array;
     */
    ExmCxxCommandPart	command;
    
  protected:
    
    /*
     * constructors & destructors
     */
    ~ExmCxxCommand();
    
    /*
     * virtual chained class-methods
     */
    // core class part;
    virtual void	initialize(Widget req_w, ArgList, Cardinal*);
    virtual Boolean	set_values(Widget old_w, Widget req_w,
				   ArgList, Cardinal*);
    
    /*
     * virtual self contained class-methods
     */
    // StaticText class part;
    virtual void	draw_shadow();
    
  public:
    
    /*
     * static class-methods
     * (must be public : passed as parameter to class_rec static constructor)
     */
    static void		_MakeCxxWidget(Widget req, Widget new_w,
				       ArgList, Cardinal*);
    /* 
     * Trait methods
     */
    static void		ChangeCB(Widget w, XtCallbackProc activCB,
				 XtPointer closure, Boolean setunset);
    static void		ShowAsDefault(Widget w, XtEnum state);
};

typedef ExmCxxCommandClass	*ExmCxxCommandWidgetClass;
typedef ExmCxxCommand		*ExmCxxCommandWidget;

externalref ExmCxxCommandClass	exmCxxCommandClassRec;

#endif /* __cplusplus */

/* These symbols may be used from C code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
extern Widget			ExmCreateCxxCommand(Widget parent, char *name,
						   ArgList, Cardinal);
#ifdef __cplusplus
}
#endif /* __cplusplus */

externalref WidgetClass		exmCxxCommandWidgetClass;

#ifndef XmIsCxxCommand
#define XmIsCxxCommand(w)	XtIsSubclass(w, exmCxxCommandWidgetClass)
#endif

#endif /* _CxxCommand_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
