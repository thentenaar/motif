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
static char rcsid[] = "$XConsortium: TestUilInit.c /main/6 1995/07/14 11:59:48 drk $"
#endif
#endif
  
#include "testlib.h"
  

/*************************************************************************
 *                                                                       *
 *      Name:           CommonTestUilInit()                              *
 *								 	 *
 *	Purpose:	This routine calls CommonTestInit wich  will     *
 *                      Initialize Xt, create an app context, create     *
 *			a connection to the display and create a         *
 *			toplevel application shell.                      *
 *									 *
 *	Return:		Nothing						 *
 *									 *
 *************************************************************************/

void CommonTestUilInit(int argc, char **argv)
     
{

  CommonTestInit(argc, argv);

}




