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

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: Translations.c /main/4 1995/07/13 20:34:53 drk $"
#endif
#endif

#include <Xm/Xm.h>
#include <testlib.h>

main(argc, argv)
int argc;
char *argv[];
{
  CommonTestInit(argc, argv);
  
  if(fork())
    ;
  else{
    execl("TranslationsXt","TranslationsXt",(char *)0);
  };
  
  if(fork())
    ;
  else{
    execl("TranslationsUil","TranslationsUil",(char *)0);
  }
  CommonPause ();
  CommonPause ();
  CommonPause ();
  CommonPause ();
  CommonPause ();
  CommonPause ();
  
  XtAppMainLoop(app_context);
}
