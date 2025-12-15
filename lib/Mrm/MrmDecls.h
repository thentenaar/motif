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

#ifndef MrmDecls_H
#define MrmDecls_H

#include <X11/Intrinsic.h>

/*----------------------------------*/
/* Error messages                   */
/*----------------------------------*/
/* The symbol _MrmConst is used for constant data that cannot be
 * declared const in the header file because of usage as arguments to
 * routines which have string arguments that are not declared const.
 *
 * So, _XmConst is always defined to be nothing in header files.
 * In the source file, however, _XmConst is defined to be const,
 * so as to allow shared data in a shared library environment.
 */

#ifndef _MrmConst
#define _MrmConst
#endif

externalref _MrmConst char *_MrmMsg_0000;
externalref _MrmConst char *_MrmMsg_0001;
externalref _MrmConst char *_MrmMsg_0002;
externalref _MrmConst char *_MrmMsg_0003;
externalref _MrmConst char *_MrmMsg_0004;
externalref _MrmConst char *_MrmMsg_0005;
externalref _MrmConst char *_MrmMsg_0006;
externalref _MrmConst char *_MrmMsg_0007;
externalref _MrmConst char *_MrmMsg_0008;
externalref _MrmConst char *_MrmMsg_0009;
externalref _MrmConst char *_MrmMsg_0010;
externalref _MrmConst char *_MrmMsg_0011;
externalref _MrmConst char *_MrmMsg_0012;
externalref _MrmConst char *_MrmMsg_0013;
externalref _MrmConst char *_MrmMsg_0014;
externalref _MrmConst char *_MrmMsg_0015;
externalref _MrmConst char *_MrmMsg_0016;
externalref _MrmConst char *_MrmMsg_0017;
externalref _MrmConst char *_MrmMsg_0018;
externalref _MrmConst char *_MrmMsg_0019;
externalref _MrmConst char *_MrmMsg_0020;
externalref _MrmConst char *_MrmMsg_0021;
externalref _MrmConst char *_MrmMsg_0022;
externalref _MrmConst char *_MrmMsg_0023;
externalref _MrmConst char *_MrmMsg_0024;
externalref _MrmConst char *_MrmMsg_0025;
externalref _MrmConst char *_MrmMsg_0026;
externalref _MrmConst char *_MrmMsg_0027;
externalref _MrmConst char *_MrmMsg_0028;
externalref _MrmConst char *_MrmMsg_0029;
externalref _MrmConst char *_MrmMsg_0030;
externalref _MrmConst char *_MrmMsg_0031;
externalref _MrmConst char *_MrmMsg_0032;
externalref _MrmConst char *_MrmMsg_0033;
externalref _MrmConst char *_MrmMsg_0034;
externalref _MrmConst char *_MrmMsg_0035;
externalref _MrmConst char *_MrmMsg_0036;
externalref _MrmConst char *_MrmMsg_0037;
externalref _MrmConst char *_MrmMsg_0038;
externalref _MrmConst char *_MrmMsg_0039;
externalref _MrmConst char *_MrmMsg_0040;
externalref _MrmConst char *_MrmMsg_0041;
externalref _MrmConst char *_MrmMsg_0042;
externalref _MrmConst char *_MrmMsg_0043;
externalref _MrmConst char *_MrmMsg_0044;
externalref _MrmConst char *_MrmMsg_0045;
externalref _MrmConst char *_MrmMsg_0046;
externalref _MrmConst char *_MrmMsg_0047;
externalref _MrmConst char *_MrmMsg_0048;
externalref _MrmConst char *_MrmMsg_0049;
externalref _MrmConst char *_MrmMsg_0050;
externalref _MrmConst char *_MrmMsg_0051;
externalref _MrmConst char *_MrmMsg_0052;
externalref _MrmConst char *_MrmMsg_0053;
externalref _MrmConst char *_MrmMsg_0054;
externalref _MrmConst char *_MrmMsg_0055;
externalref _MrmConst char *_MrmMsg_0056;
externalref _MrmConst char *_MrmMsg_0057;
externalref _MrmConst char *_MrmMsg_0058;
externalref _MrmConst char *_MrmMsg_0059;
externalref _MrmConst char *_MrmMsg_0060;
externalref _MrmConst char *_MrmMsg_0061;
externalref _MrmConst char *_MrmMsg_0062;
externalref _MrmConst char *_MrmMsg_0063;
externalref _MrmConst char *_MrmMsg_0064;
externalref _MrmConst char *_MrmMsg_0065;
externalref _MrmConst char *_MrmMsg_0066;
externalref _MrmConst char *_MrmMsg_0067;
externalref _MrmConst char *_MrmMsg_0068;
externalref _MrmConst char *_MrmMsg_0069;
externalref _MrmConst char *_MrmMsg_0070;
externalref _MrmConst char *_MrmMsg_0071;
externalref _MrmConst char *_MrmMsg_0072;
externalref _MrmConst char *_MrmMsg_0073;
externalref _MrmConst char *_MrmMsg_0074;
externalref _MrmConst char *_MrmMsg_0075;
externalref _MrmConst char *_MrmMsg_0076;
externalref _MrmConst char *_MrmMsg_0077;
externalref _MrmConst char *_MrmMsg_0078;
externalref _MrmConst char *_MrmMsg_0079;
externalref _MrmConst char *_MrmMsg_0080;
externalref _MrmConst char *_MrmMsg_0081;
externalref _MrmConst char *_MrmMsg_0082;
externalref _MrmConst char *_MrmMsg_0083;
externalref _MrmConst char *_MrmMsg_0084;
externalref _MrmConst char *_MrmMsg_0085;
externalref _MrmConst char *_MrmMsg_0086;
externalref _MrmConst char *_MrmMsg_0087;
externalref _MrmConst char *_MrmMsg_0088;
externalref _MrmConst char *_MrmMsg_0089;
externalref _MrmConst char *_MrmMsg_0090;
externalref _MrmConst char *_MrmMsg_0091;
externalref _MrmConst char *_MrmMsg_0092;
externalref _MrmConst char *_MrmMsg_0093;
externalref _MrmConst char *_MrmMsg_0094;
externalref _MrmConst char *_MrmMsg_0095;
externalref _MrmConst char *_MrmMsg_0096;
externalref _MrmConst char *_MrmMsg_0097;
externalref _MrmConst char *_MrmMsg_0098;
externalref _MrmConst char *_MrmMsg_0099;
externalref _MrmConst char *_MrmMsg_0100;
externalref _MrmConst char *_MrmMsg_0101;
externalref _MrmConst char *_MrmMsg_0102;
externalref _MrmConst char *_MrmMsg_0103;
externalref _MrmConst char *_MrmMsg_0104;
externalref _MrmConst char *_MrmMsg_0105;
externalref _MrmConst char *_MrmMsg_0106;
externalref _MrmConst char *_MrmMsg_0107;
externalref _MrmConst char *_MrmMsg_0108;
externalref _MrmConst char *_MrmMsg_0109;
externalref _MrmConst char *_MrmMsg_0110;
externalref _MrmConst char *_MrmMsg_0111;
externalref _MrmConst char *_MrmMsg_0112;
externalref _MrmConst char *_MrmMsg_0113;
externalref _MrmConst char *_MrmMsg_0114;
externalref _MrmConst char *_MrmMsg_0115;
externalref _MrmConst char *_MrmMsg_0116;
externalref _MrmConst char *_MrmMsg_0117;
externalref _MrmConst char *_MrmMsg_0118;
externalref _MrmConst char *_MrmMsg_0119;

/*----------------------------------*/
/* URM external routines (Motif)    */
/*----------------------------------*/
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/* mrminit.c */
extern void MrmInitialize(void);

/* mrmlread.c */
extern Cardinal MrmFetchLiteral(MrmHierarchy hierarchy_id,
                                const char *index,
                                Display *display,
                                XtPointer *value_return,
                                MrmCode *type_return);
extern Cardinal MrmFetchIconLiteral(MrmHierarchy hierarchy_id,
                                    const char *index,
                                    Screen *screen,
                                    Display *display,
                                    Pixel fgpix,
                                    Pixel bgpix,
                                    Pixmap *pixmap_return);
extern Cardinal MrmFetchBitmapLiteral(MrmHierarchy hierarchy_id,
                                      const char *index,
                                      Screen *screen,
                                      Display *display,
                                      Pixmap *pixmap_return,
                                      Dimension *width,
                                      Dimension *height);
extern Cardinal MrmFetchColorLiteral(MrmHierarchy hierarchy_id,
                                     const char *index,
                                     Display *display,
                                     Colormap cmap,
                                     Pixel *pixel_return);
extern Cardinal MrmOpenHierarchy  ( MrmCount num_files , String *name_list , MrmOsOpenParamPtr *os_ext_list , MrmHierarchy *hierarchy_id_return );
extern Cardinal MrmOpenHierarchyPerDisplay  ( Display *display , MrmCount num_files , String *name_list , MrmOsOpenParamPtr *os_ext_list , MrmHierarchy *hierarchy_id_return );
extern Cardinal MrmRegisterNames  ( MrmRegisterArglist reglist ,MrmCount num_reg );
extern Cardinal MrmRegisterNamesInHierarchy  ( MrmHierarchy hierarchy_id , MrmRegisterArglist reglist , MrmCount num_reg );

extern Cardinal MrmRegisterClass(MrmType class_code, /* unused */
                                 const char *class_name, /* unused */
                                 const char *create_name,
                                 MrmWidgetCreateProc creator,
                                 WidgetClass class_record);
extern Cardinal MrmRegisterClassWithCleanup(MrmType class_code, /* unused */
                                            const char *class_name, /* unused */
                                            const char *create_name,
                                            MrmWidgetCreateProc creator,
                                            WidgetClass class_record,
                                            MrmWidgetCleanupProc cleanup);
extern Cardinal MrmCloseHierarchy(MrmHierarchy hierarchy_id);
extern Cardinal MrmFetchInterfaceModule(MrmHierarchy hierarchy_id,
                                        const char *module_name,
                                        Widget parent,
                                        Widget *w_return);
extern Cardinal MrmFetchWidget(MrmHierarchy hierarchy_id,
                               const char *index,
                               Widget parent,
                               Widget *w_return,
                               MrmType *class_return);
extern Cardinal MrmFetchWidgetOverride(MrmHierarchy hierarchy_id,
                                       const char *index,
                                       Widget parent,
                                       const char *ov_name,
                                       ArgList ov_args,
                                       Cardinal ov_num_args,
                                       Widget *w_return,
                                       MrmType *class_return);
extern Cardinal MrmFetchSetValues  ( MrmHierarchy hierarchy_id , Widget w , ArgList args , Cardinal num_args );

/* mrmwci.c */

/* extern Cardinal XmRegisterMrmCallbacks () ; */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#undef _ARGUMENTS

#endif /* MrmDecls_H */
