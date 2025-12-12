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
#ifndef _XmI_h
#define _XmI_h

#ifndef _XmNO_BC_INCL
#define _XmNO_BC_INCL
#endif

#include <stdio.h>
#include <limits.h>
#include <Xm/XmP.h>
#include "XmStrDefsI.h"

#ifdef __cplusplus
extern "C" {
#endif

extern Boolean _init_modifiers;
extern unsigned int NumLockMask;
extern unsigned int ScrollLockMask;

void _XmInitModifiers (void);
#define _XmCheckInitModifiers() 						\
    { 									\
	if (_init_modifiers) 						\
	{ 								\
	    _XmInitModifiers(); 					\
	    _init_modifiers = FALSE; 					\
	}								\
    }

#ifndef DEBUG
# define assert(assert_exp)
#elif (defined(__STDC__) && !defined(UNIXCPP)) || defined(ANSICPP)
#include <stdlib.h>
# define assert(assert_exp)						\
  (((assert_exp) ? (void) 0 :						\
    (void) (fprintf(stderr, "assert(%s) failed at line %d in %s\n",	\
                    #assert_exp, __LINE__, __FILE__), abort())))
#else
#include <stdlib.h>
# define assert(assert_exp)						\
  (((assert_exp) ? 0 :							\
    (void) (fprintf(stderr, "assert(%s) failed at line %d in %s\n",	\
		    "assert_exp", __LINE__, __FILE__), abort())))
#endif


#define ASSIGN_MAX(a, b) 	((a) = ((a) > (b) ? (a) : (b)))
#define ASSIGN_MIN(a, b) 	((a) = ((a) < (b) ? (a) : (b)))

#ifndef MAX
#define MAX(x,y)	((x) > (y) ? (x) : (y))
#endif

#ifndef MIN
#define MIN(x,y)	((x) > (y) ? (y) : (x))
#endif

#ifndef ABS
#define ABS(x)		(((x) >= 0) ? (x) : -(x))
#endif

#define GMode(g)	    ((g)->request_mode)
#define IsX(g)		    (GMode (g) & CWX)
#define IsY(g)		    (GMode (g) & CWY)
#define IsWidth(g)	    (GMode (g) & CWWidth)
#define IsHeight(g)	    (GMode (g) & CWHeight)
#define IsBorder(g)	    (GMode (g) & CWBorderWidth)
#define IsWidthHeight(g)    (GMode (g) & (CWWidth | CWHeight))
#define IsQueryOnly(g)      (GMode (g) & XtCWQueryOnly)

#define XmStrlen(s)      ((s) ? strlen(s) : 0)


#define XmStackAlloc(size, stack_cache_array)	\
    ((((char*)(stack_cache_array) != NULL) &&	\
     ((size) <= sizeof(stack_cache_array)))	\
     ?  (char *)(stack_cache_array)		\
     :  XtMalloc((unsigned)(size)))

#define XmStackFree(pointer, stack_cache_array) \
    if ((pointer) != ((char*)(stack_cache_array))) XtFree(pointer);


/******** _XmCreateImage ********/

#ifdef NO_XM_1_2_BC

/* The _XmCreateImage macro is used to create XImage with client
   specific data for the bit and byte order.
   We still have to do the following because XCreateImage
   will stuff here display specific data and we want
   client specific values (i.e the bit orders we used for
   creating the bitmap data in Motif) -- BUG 4262 */
/* Used in Motif 1.2 in DragIcon.c, MessageB.c, ReadImage.c and
   ImageCache.c */

#define _XmCreateImage(IMAGE, DISPLAY, DATA, WIDTH, HEIGHT, BYTE_ORDER) {\
    IMAGE = XCreateImage(DISPLAY,\
			 DefaultVisual(DISPLAY, DefaultScreen(DISPLAY)),\
			 1,\
			 XYBitmap,\
			 0,\
			 DATA,\
			 WIDTH, HEIGHT,\
			 8,\
			 (WIDTH+7) >> 3);\
    IMAGE->byte_order = BYTE_ORDER;\
    IMAGE->bitmap_unit = 8;\
    IMAGE->bitmap_bit_order = LSBFirst;\
}

#endif /* NO_XM_1_2_BC */


/****************************************************************
 *
 *  Macros for Right-to-left Layout
 *
 ****************************************************************/

#define GetLayout(w)     (_XmGetLayoutDirection((Widget)(w)))
#define LayoutM(w)       (XmIsManager(w) ? \
			  ((XmManagerWidget)w)->manager.string_direction : \
			  GetLayout(w))
#define LayoutP(w)       (XmIsPrimitive(w) ? \
			  XmPrim_layout_direction(((XmPrimitiveWidget)w)) :\
			  GetLayout(w))
#define LayoutG(w)       (XmIsGadget(w) ? \
			  ((XmGadget)w)->gadget.layout_direction : \
			  GetLayout(w))

#define LayoutIsRtoL(w)      \
  (XmDirectionMatchPartial(GetLayout(w), XmRIGHT_TO_LEFT, XmHORIZONTAL_MASK))
#define LayoutIsRtoLM(w)     \
  (XmDirectionMatchPartial(LayoutM(w), XmRIGHT_TO_LEFT, XmHORIZONTAL_MASK))
#define LayoutIsRtoLP(w)     \
  (XmDirectionMatchPartial(LayoutP(w), XmRIGHT_TO_LEFT, XmHORIZONTAL_MASK))
#define LayoutIsRtoLG(w)     \
  (XmDirectionMatchPartial(LayoutG(w), XmRIGHT_TO_LEFT, XmHORIZONTAL_MASK))


/********    Private Function Declarations for Direction.c    ********/

extern void _XmDirectionDefault(Widget widget,
  			        int offset,
  			        XrmValue *value );
extern void _XmFromLayoutDirection(
                        Widget widget,
                        int offset,
                        XtArgVal *value) ;

extern XmImportOperator _XmToLayoutDirection(
                        Widget widget,
                        int offset,
                        XtArgVal *value) ;
extern XmDirection _XmGetLayoutDirection(Widget w);


/********    Private Function Declarations for thickness  ********/
extern void _XmSetThickness(
                        Widget widget,
                        int offset,
                        XrmValue *value) ;
extern void _XmSetThicknessDefault0(
                        Widget widget,
                        int offset,
                        XrmValue *value) ;

/********    Private Function Declarations for Xm.c    ********/

extern void _XmReOrderResourceList(
			WidgetClass widget_class,
			String res_name,
                        String insert_after) ;
extern void _XmSocorro(
                        Widget w,
                        XEvent *event,
                        String *params,
                        Cardinal *num_params) ;
extern Boolean _XmParentProcess(
                        Widget widget,
                        XmParentProcessData data) ;
extern void _XmClearShadowType(
                        Widget w,
                        Dimension old_width,
                        Dimension old_height,
                        Dimension old_shadow_thickness,
                        Dimension old_highlight_thickness) ;
#ifdef NO_XM_1_2_BC
extern void _XmDestroyParentCallback(
                        Widget w,
                        XtPointer client_data,
                        XtPointer call_data) ;
#endif
extern Time _XmValidTimestamp(
			Widget w);
extern void _XmWarningMsg(Widget w,
                          char *type,
			  char *message,
			  char **params,
			  Cardinal num_params);
extern Display *_XmGetDefaultDisplay(void);
extern Boolean _XmIsISO10646(Display *dpy,
                             XFontStruct *font);
extern XChar2b* _XmUtf8ToUcs2(char *draw_text,
                              size_t seg_len,
			      size_t *ret_str_len);
extern Pixel _XmAssignInsensitiveColor(Widget w);

/********    End Private Function Declarations    ********/

/********    Conditionally defined macros for thread_safe Motif ******/
#if defined(XTHREADS) && defined(XUSE_MTSAFE_API)

# define _XmWidgetToAppContext(w) \
        XtAppContext app = XtWidgetToApplicationContext(w)

# define _XmDisplayToAppContext(d) \
        XtAppContext app = XtDisplayToApplicationContext(d)

# define _XmAppLock(app)	XtAppLock(app)
# define _XmAppUnlock(app)	XtAppUnlock(app)
# define _XmProcessLock()	XtProcessLock()
# define _XmProcessUnlock()	XtProcessUnlock()

/* Remove use of _XtProcessLock when Xt provides API to query its MT-status */
extern void (*_XtProcessLock)();
# define _XmIsThreadInitialized() (_XtProcessLock)

#else

# define _XmWidgetToAppContext(w)
# define _XmDisplayToAppContext(d)
# define _XmAppLock(app)
# define _XmAppUnlock(app)
# define _XmProcessLock()
# define _XmProcessUnlock()
# define _XmIsThreadInitialized()	(FALSE)

#endif /* XTHREADS && XUSE_MTSAFE_API */


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif


#endif /* _XmI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
