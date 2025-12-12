/* $XConsortium: wsmDebug.h /main/5 1995/07/15 20:46:32 drk $ */
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

#ifndef wsmDebug_h
#define wsmDebug_h

#include "wsm.h"

#ifdef WSM_DEBUG

#ifndef PRINT
#define PRINT printf
#endif

#else

#ifndef PRINT
    static int noop (char *fmt, ...) { return 0; }
#define PRINT noop
#endif

#endif


extern void print_window(
WorkWindow*
);


extern void print_all_window();


extern void print_space_windows(
Space*
);

extern void print_values(
XrmValue*,
XrmQuarkList,
XrmQuark,
int
);

extern void print_all_values(
XrmValue**,
XrmQuarkList,
XrmQuarkList,
int
);

extern void print_win_data(
WSMWinData*,
int
);



extern void print_internal_win_data(
WSMWinData*,
int
);


extern void print_request(
WSMRequest*
);

extern void print_reply(
WSMReply*
);


extern void print_reply_start_state(
WSMGetStateReply*
);


extern void PrintSpace(
char *
);


extern void PrintAllSpaces();

#endif
