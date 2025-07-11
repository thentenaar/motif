/* 
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
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
/*   $XConsortium: ClassTables.h /main/4 1995/07/13 18:00:10 drk $ */


#include <testlib.h>

#define NUM_LEGAL_CHILDREN 22
#define NUM_LEGAL_PARENTS 5

typedef struct {
  char * string;
  WidgetClass class;
} StringClassPair;

static StringClassPair parent_table[] = {
  { "xmBulletinBoardWidgetClass", (WidgetClass) NULL },
  { "xmDrawingAreaWidgetClass", (WidgetClass) NULL },
  { "xmFrameWidgetClass", (WidgetClass) NULL  },
  { "xmMainWindowWidgetClass", (WidgetClass) NULL },
  { "xmFormWidgetClass", (WidgetClass) NULL }
};

static StringClassPair child_table[] = {
  { "xmScaleWidgetClass",  (WidgetClass) NULL },
  { "xmScrollBarWidgetClass",  (WidgetClass) NULL },
  { "xmMessageBoxWidgetClass",  (WidgetClass) NULL },
  { "xmSelectionBoxWidgetClass",  (WidgetClass) NULL },
  { "xmCommandWidgetClass",  (WidgetClass) NULL },
  { "xmFileSelectionBoxWidgetClass",  (WidgetClass) NULL },
  { "xmArrowButtonWidgetClass",  (WidgetClass) NULL },
  { "xmListWidgetClass",  (WidgetClass) NULL },
  { "xmSeparatorWidgetClass",  (WidgetClass) NULL },
  { "xmTextWidgetClass",  (WidgetClass) NULL },
  { "xmTextFieldWidgetClass",  (WidgetClass) NULL },
  { "xmLabelWidgetClass",  (WidgetClass) NULL },
  { "xmDrawnButtonWidgetClass",  (WidgetClass) NULL },
  { "xmPushButtonWidgetClass",  (WidgetClass) NULL },
  { "xmToggleButtonWidgetClass",  (WidgetClass) NULL },
  { "xmArrowButtonGadgetClass",  (WidgetClass) NULL },
  { "xmSeparatorGadgetClass",  (WidgetClass) NULL },
  { "xmLabelGadgetClass",  (WidgetClass) NULL },
  { "xmPushButtonGadgetClass",  (WidgetClass) NULL },
  { "xmToggleButtonGadgetClass", (WidgetClass) NULL },
  { "xmBulletinBoardWidgetClass", (WidgetClass) NULL },
  { "xmFormWidgetClass", (WidgetClass) NULL }
};



WidgetClass parent_classes[5];



