/**
 * Prototypes for obsolete prototypes, split out from Xm.h
 */
#ifdef _Xm_h
#ifndef XM_OBSOLETE_H
#define XM_OBSOLETE_H

#include <Xm/deprecated.h>

/* {{{ Deprecated macros using C99's _Pragma */
#ifdef __GNUC__
#define XmSTRING_COMPONENT_FONTLIST_ELEMENT_TAG \
	_Pragma("GCC warning \"XmSTRING_COMPONENT_FONTLIST_ELEMENT_TAG is deprecated, use XmSTRING_COMPONENT_TAG instead\"") \
	XmSTRING_COMPONENT_TAG
#define XmSTRING_COMPONENT_CHARSET \
	_Pragma("GCC warning \"XmSTRING_COMPONENT_CHARSET is deprecated, use XmSTRING_COMPONENT_TAG instead\"") \
	XmSTRING_COMPONENT_TAG
#elif defined(__clang__)
#define XmSTRING_COMPONENT_FONTLIST_ELEMENT_TAG \
	_Pragma("clang warning \"XmSTRING_COMPONENT_FONTLIST_ELEMENT_TAG is deprecated, use XmSTRING_COMPONENT_TAG instead\"") \
	XmSTRING_COMPONENT_TAG
#define XmSTRING_COMPONENT_CHARSET \
	_Pragma("clang warning \"XmSTRING_COMPONENT_CHARSET is deprecated, use XmSTRING_COMPONENT_TAG instead\"") \
	XmSTRING_COMPONENT_TAG
#else
#define XmSTRING_COMPONENT_FONTLIST_ELEMENT_TAG XmSTRING_COMPONENT_TAG
#define XmSTRING_COMPONENT_CHARSET XmSTRING_COMPONENT_TAG
#endif
/* }}} */

/********    Public Function Declarations for MenuUtil.c    ********/
XM_ALTERNATIVE(Use XtSetValues for XmNmenuCursor on XmScreen instead)
extern void XmSetMenuCursor(Display *display, Cursor cursorId);

XM_ALTERNATIVE(Use XtGetValues for XmNmenuCursor on XmScreen instead)
extern Cursor XmGetMenuCursor(Display *display) ;
/********    End Public Function Declarations    ********/

XM_ALTERNATIVE(Use XtSetValues for XmNhorizontalFontUnit and XmNverticalFontUnit instead)
extern void XmSetFontUnits(Display *display, int h_value, int v_value);

XM_ALTERNATIVE(Use XtSetValues for XmNhorizontalFontUnit or XmNverticalFontUnit instead)
extern void XmSetFontUnit(Display *display, int value);

XM_ALTERNATIVE(Use XmTrackingEvent instead)
extern Widget XmTrackingLocate(Widget widget, Cursor cursor, Boolean confineTo);

XM_ALTERNATIVE(Use XmeResolvePartOffsets instead)
extern void XmResolveAllPartOffsets(WidgetClass w_class, XmOffsetPtr *offset, XmOffsetPtr *constraint_offset);

XM_ALTERNATIVE(Set XmNnavigationType to XmEXCLUSIVE_TAB_GROUP instead)
extern void XmAddTabGroup(Widget tabGroup);

XM_ALTERNATIVE(Set XmNnavigationType to XmNONE instead)
extern void XmRemoveTabGroup(Widget w);

XM_DEPRECATED
extern void XmCvtStringToUnitType(XrmValuePtr args, Cardinal *num_args,
                                  XrmValue *from_val, XrmValue *to_val);

XM_ALTERNATIVE(Use XmStringCreateLocalized instead)
extern XmString XmStringCreateSimple(char *text);

XM_ALTERNATIVE(Use XmStringLtoRCreate instead)
extern XmString XmStringCreateLtoR(char *text, XmStringCharSet charset);

XM_ALTERNATIVE(Use XmStringComponentCreate and XmStringConcat instead)
extern XmString XmStringSegmentCreate(
                        char *text,
                        XmStringCharSet charset,
                        XmStringDirection direction,
                        Boolean separator);

XM_ALTERNATIVE(Use XmStringGetNextTriple instead)
extern XmStringComponentType XmStringGetNextComponent(
                        XmStringContext context,
                        char **text,
                        XmStringCharSet *charset,
                        XmStringDirection *direction,
                        XmStringComponentType *unknown_tag,
                        unsigned short *unknown_length,
                        unsigned char **unknown_value);

XM_ALTERNATIVE(Use XmStringPeekNextTriple instead)
extern XmStringComponentType XmStringPeekNextComponent(XmStringContext context);

XM_ALTERNATIVE(Use XmStringGetNextTriple instead)
extern Boolean XmStringGetNextSegment(
                        XmStringContext context,
                        char **text,
                        XmStringCharSet *charset,
                        XmStringDirection *direction,
                        Boolean *separator);

XM_ALTERNATIVE(Use XmStringUnparse instead)
extern Boolean XmStringGetLtoR(XmString string, XmStringCharSet charset, char **text);

XM_ALTERNATIVE(Use XmStringConcat instead)
extern XmString XmStringNConcat(XmString first, XmString second, int n);

XM_ALTERNATIVE(Use XmStringCopy instead)
extern XmString XmStringNCopy(XmString str, int n);

XM_ALTERNATIVE(Use XmStringCompare instead)
extern Boolean XmStringByteCompare(XmString a1, XmString b1);

XM_ALTERNATIVE(Use XmFontListAppendEntry instead)
extern XmFontList XmFontListCreate(XFontStruct *font, XmStringCharSet charset);

XM_ALTERNATIVE(Use XmFontListAppendEntry instead)
extern XmFontList XmFontListCreate_r(XFontStruct *font,
                                     XmStringCharSet charset, Widget wid);

XM_ALTERNATIVE(Use XmFontListAppendEntry instead)
extern XmFontList XmStringCreateFontList(XFontStruct *font, XmStringCharSet charset);

XM_ALTERNATIVE(Use XmFontListAppendEntry instead)
extern XmFontList XmStringCreateFontList_r(XFontStruct *font,
                                           XmStringCharSet charset, Widget wid);

XM_ALTERNATIVE(Use XmFontListAppendEntry instead)
extern XmFontList XmFontListAdd(XmFontList old, XFontStruct *font,
                                XmStringCharSet charset);

XM_ALTERNATIVE(Use XmFontListNextEntry instead)
extern Boolean XmFontListGetNextFont(
                        XmFontContext context,
                        XmStringCharSet *charset,
                        XFontStruct **font);

#endif /* XM_OBSOLETE_H */
#endif /* _Xm_h */

