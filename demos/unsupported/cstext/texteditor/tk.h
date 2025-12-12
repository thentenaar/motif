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
/*   $XConsortium: tk.h /main/6 1995/07/14 09:45:34 drk $ */
/************************************************************
 *     tk.h -- toolkit-specific dialogue layer
 ************************************************************/

#ifndef _tk_h
#define _tk_h

#include <Xm/Xm.h>
#include "tkdef.h"


extern void TkInit();
extern void TkBeep();
extern void TkExit();
extern void TkUpdateStatus(char *str);

extern Boolean TkTextChanged();
extern void TkTextActUnchangedSince();
extern Boolean TkTextChangedSince();

extern void TkTextClear();
extern void TkTextStore(char *txt);
extern int TkTextFormat();
extern size_t TkTextRetrieve(char **txt, int format);

extern void TkAskFileToOpen();
extern void TkAskFileToSave();
extern void TkAskFileToCopy();
extern void TkAskFileToMove();
extern void TkDoneAskingFile();
extern void TkArrangeToOpen();

extern void TkAskSave();
extern void TkDoneAskingSave();

extern void TkWarn(enum warn_reasons reason);
extern void TkWarnAndAskFileToSave(enum warn_reasons reason);
extern void TkQuestionRemove();

extern XmRenderTable TkUpdateRenditions(Widget w, XmRenderTable rt);

extern void TkRegisterCallbacks();


#endif /* _tk_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
