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
 * Motif Release 1.2
*/

#ifdef WSM
#ifdef DEBUGGER
extern void PrintFormatted(char *f, char *s0, char *s1, char *s2, char *s3, char *s4, char *s5, char *s6, char *s7, char *s8, char *s9);
#endif /* DEBUGGER */
#endif /* WSM */
extern void WmInitErrorHandler (Display *display);
extern int WmXErrorHandler (Display *display, XErrorEvent *errorEvent);
extern int WmXIOErrorHandler (Display *display);
extern _X_NORETURN void WmXtErrorHandler (char *message);
extern void WmXtWarningHandler (char *message);
extern void Warning (char *message);
#if XM_MSGCAT
extern char * GetMessage(int set, int n, char * s);
#endif
#ifdef WSM
/****************************   eof    ***************************/
#endif /* WSM */
