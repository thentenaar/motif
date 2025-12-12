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

#ifdef REV_INFO
#ifndef lint
static char *rcsidpageH = "$XConsortium: page.h /main/5 1995/07/14 09:46:34 drk $";
#endif
#endif

#ifndef Page_H
#define Page_H

#define MAXPAGES 100
#define MAXINIT 512

typedef struct {
  char *page;
  char *majorTab;
  char *minorTab;
  char *label;
  Widget minorPB;
  Widget majorPB;
  int 	lasttoppos;
  int   lastcursorpos;
} PageRec, *Page;

/* Options */
typedef struct {
  String	todoFile;
} OptionsRec;

extern OptionsRec options;

#endif /* Page_H */
