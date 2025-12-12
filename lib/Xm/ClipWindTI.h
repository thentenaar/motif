/* $XConsortium: ClipWindTI.h /main/1 1996/10/10 11:36:14 cde-osf $ */
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
 * (c) Copyright 1989, 1990, 1991, 1992, 1993 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
 */

#ifndef _XmClipWindowTI_H
#define _XmClipWindowTI_H

#include <Xm/Xm.h>
#include <Xm/NavigatorT.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref XrmQuark _XmQTclipWindow;

/* Version 0: initial release. */

typedef struct _XmClipWindowTraitRec {
  int				    version;		/* 0 */
} XmClipWindowTraitRec, *XmClipWindowTrait;

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmClipWindowTI_H */

