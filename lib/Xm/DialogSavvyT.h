/* $XConsortium: DialogSavvyT.h /main/5 1995/07/15 20:50:29 drk $ */
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
#ifndef _XmDialogSavvyT_H
#define _XmDialogSavvyT_H

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref XrmQuark XmQTdialogShellSavvy;
/* This trait also requires a resource named "defaultPosition".
   If the child has the trait, the resource will be get and set by 
   the DialogShell ChangeManaged */
   
/* Trait structures and typedefs, place typedefs first */

typedef void (*XmDialogSavvyMapUnmapProc)(Widget wid, 
					  Boolean map_unmap);


/* Version 0: initial release. */

typedef struct _XmDialogSavvyTraitRec	 {
  int			    version;		/* 0 */
  XmDialogSavvyMapUnmapProc callMapUnmapCB;
} XmDialogSavvyTraitRec,*XmDialogSavvyTrait;


/* This macro is part of the trait and is used for the following situation
   DialogShells always mimic the child position on themselves.
   If the SetValues on a bb child position was 0,
   which is always the _current_ position of the bb in a DialogShell,
   Xt does not see a change and therefore not trigerred a geometry request.
   So BB (or any dialogShellSavvy child) has to catch this case
   and change the position request to use a special value in its
   SetValues method, XmDIALOG_SAVVY_FORCE_ORIGIN, to notify the Dialog that 
   it really wants to move in 0 */

#define XmDIALOG_SAVVY_FORCE_ORIGIN ((Position)~0L)

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmDialogSavvyT_H */
