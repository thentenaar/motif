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
 *
 */
/*
 * HISTORY
 */
/* $XConsortium: SyntheticI.h /main/6 1996/04/18 12:01:21 daniel $ */
#ifndef _XmSyntheticI_h
#define _XmSyntheticI_h

#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif

/********    Private Function Declarations for Synthetic.c    ********/

extern void _XmBuildResources(
                        XmSyntheticResource **wc_resources_ptr,
                        int *wc_num_resources_ptr,
                        XmSyntheticResource *sc_resources,
                        int sc_num_resources) ;
extern void _XmInitializeSyntheticResources(
                        XmSyntheticResource *resources,
                        int num_resources) ;
extern void _XmPrimitiveGetValuesHook(
                        Widget w,
                        ArgList args,
                        Cardinal *num_args) ;
extern void _XmGadgetGetValuesHook(
                        Widget w,
                        ArgList args,
                        Cardinal *num_args) ;
extern void _XmManagerGetValuesHook(
                        Widget w,
                        ArgList args,
                        Cardinal *num_args) ;
#if XM_PRINTING
extern void _XmPrintShellGetValuesHook(
                        Widget w,
                        ArgList args,
                        Cardinal *num_args) ;
#endif
extern void _XmExtGetValuesHook(
                        Widget w,
                        ArgList args,
                        Cardinal *num_args) ;
extern void _XmExtImportArgs(
                        Widget w,
                        ArgList args,
                        Cardinal *num_args) ;
extern void _XmPrimitiveImportArgs(
                        Widget w,
                        ArgList args,
                        Cardinal *num_args) ;
extern void _XmGadgetImportArgs(
                        Widget w,
                        ArgList args,
                        Cardinal *num_args) ;
extern void _XmGadgetImportSecondaryArgs(
                        Widget w,
                        ArgList args,
                        Cardinal *num_args) ;
extern void _XmManagerImportArgs(
                        Widget w,
                        ArgList args,
                        Cardinal *num_args) ;

/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmSyntheticI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
