/* $XConsortium: AutoMessages.h /main/4 1995/07/15 20:59:54 drk $ */
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



/* header file to be included in AutoMessages.c */

/* array declaration */
extern char * _AutoMessages[];

/* enumeration of offsets into the array */

/* signal messages */

typedef enum {	  
          SIGMSG1 = 0,
          SIGMSG2,
	  SIGMSG3,
	  SIGMSG4,
          SIGMSG5,
          SIGMSG6,
       	  SIGMSG7,
       	  SIGMSG8, 
       	  SIGMSG9, 
       	  SIGMSG10,
       	  SIGMSG11,
       	  SIGMSG12,
	  SIGMSGLAST=SIGMSG12
      } SIGNAL_MESSAGES;
	  
	  /* script messages */
typedef enum  {
          SCRMSG1 = SIGMSGLAST+1,
          SCRMSG2, 
 	  SCRMSG3, 
	  SCRMSG4, 
	  SCRMSG5, 
	  SCRMSG6, 
	  SCRMSG7, 
	  SCRMSG8, 
 	  SCRMSG9, 
 	  SCRMSG10,
 	  SCRMSG11,
          SCRMSG12,
          SCRMSG13,
          SCRMSG14,
	  SCRMSG15,
	  SCRMSG16,
	  SCRMSG17,
	  SCRMSG18,
	  SCRMSG19,
	  SCRMSG20,
 	  SCRMSG21,
	  SCRMSG22,
	  SCRMSG23,
 	  SCRMSG24,
	  SCRMSG25,
	  SCRMSG26,
	  SCRMSG27,
	  SCRMSG28,
	  SCRMSG29,
	  SCRMSG30,
	  SCRMSG31,
	  SCRMSG32,	 
	  SCRMSG33,
	  SCRMSG34,
	  SCRMSG35, 
 	  SCRMSG36,	
	  SCRMSG37, 
	  SCRMSG38, 
	  SCRMSG39, 
	  SCRMSG40,	
	  SCRMSG41,
	  SCRMSGLAST=SCRMSG41
      } SCR_MESSAGES;

	  /* warning messages */

typedef enum  {
	  WARNMSG1 = SCRMSGLAST+1,
	  WARNMSG2, 
	  WARNMSG3, 
	  WARNMSG4, 
	  WARNMSG5, 
	  WARNMSG6, 
	  WARNMSG7, 
	  WARNMSG8, 
	  WARNMSG9, 
	  WARNMSG10,
	  WARNMSG11,
	  WARNMSG12,
	  WARNMSG13,
	  WARNMSG14,
	  WARNMSG15,
	  WARNMSG16,
	  WARNMSG17,
	  WARNMSG18,
	  WARNMSG19,
	  WARNMSG20,
	  WARNMSG21,
	  WARNMSG22,
	  WARNMSG23,
	  WARNMSG24,
	  WARNMSG25,
	  WARNMSG26,
	  WARNMSG27,
	  WARNMSG28,
	  WARNMSG29,
	  WARNMSG30,
	  WARNMSG31,
	  WARNMSG32,
	  WARNMSG33,
	  WARNMSG34,
	  WARNMSG35,
	  WARNMSG36,
	  WARNMSG37,
	  WARNMSG38,
	  WARNMSG39,
	  WARNMSG40,
	  WARNMSG41,
	  WARNMSG42,
	  WARNMSG43,
	  WARNMSG44,
	  WARNMSG45,
	  WARNMSG46,
	  WARNMSG47,
	  WARNMSG48,
	  WARNMSG49,
	  WARNMSG50,
	  WARNMSG51,
	  WARNMSG52,
	  WARNMSG53,
	  WARNMSG54,
	  WARNMSG55,
	  WARNMSG56,
	  WARNMSG57,
	  WARNMSG58,
	  WARNMSG59,
	  WARNMSG60,
	  WARNMSG61,
	  WARNMSG62,
	  WARNMSG63,
	  WARNMSG64,
	  WARNMSG65,
	  WARNMSG66,
	  WARNMSG67,
	  WARNMSG68,
	  WARNMSG69,
	  WARNMSG70,
	  WARNMSG71,
	  WARNMSG72,
	  WARNMSG73,
	  WARNMSG74,
	  WARNMSG75,
	  WARNMSG76,
	  WARNMSG77,
	  WARNMSG78,
	  WARNMSG79,
	  WARNMSG80,
	  WARNMSG81,
	  WARNMSG82,
	  WARNMSG83,
	  WARNMSG84,
	  WARNMSG85,
	  WARNMSG86,
	  WARNMSG87,
	  WARNMSG88,
	  WARNMSG89,
	  WARNMSG90,
	  WARNMSG91,
	  WARNMSG92,
	  WARNMSG93,
	  WARNMSG94,
	  WARNMSG95,
	  WARNMSG96,
	  WARNMSG97,
	  WARNMSG98,
	  WARNMSG99,
	  WARNMSG100,
	  WARNMSG101,
	  WARNMSG102,
	  WARNMSG103,
	  WARNMSG104,
	  WARNMSG105, 
	  WARNMSG106,
	  WARNMSG107, 
	  WARNMSG108, 
	  WARNMSG109, 
	  WARNMSG110, 
	  WARNMSG111, 
	  WARNMSG112, 
	  WARNMSG113, 
	  WARNMSG114, 
	  WARNMSG115, 
	  WARNMSG116, 
	  WARNMSG117, 
	  WARNMSG118, 
	  WARNMSG119, 
	  WARNMSG120, 
	  WARNMSG121, 
	  WARNMSG122, 
	  WARNMSG123, 
	  WARNMSG124, 
	  WARNMSG125, 
	  WARNMSG126, 
	  WARNMSG127, 	  
	  WARNMSG128, 	  
	  WARNMSG129, 
	  WARNMSG130,
	  WARNMSGLAST = WARNMSG130
      } WARN_MESSAGES;

	  /* visual error messages */

typedef enum  {
          VISMSG1 = WARNMSGLAST+1,
	  VISMSG2, 
	  VISMSG3, 
	  VISMSG4, 
	  VISMSG5, 
          VISMSG6, 
          VISMSG7, 
          VISMSG8, 
          VISMSG9, 
          VISMSG10,
	  VISMSG11,
          VISMSG12,
          VISMSG13,
          VISMSG14,
          VISMSG15,
          VISMSG16,
          VISMSG17,
          VISMSG18,
          VISMSG19,
          VISMSG20,
          VISMSG21,
          VISMSG22,
          VISMSG23,
 	  VISMSG24,
	  VISMSG25, 
 	  VISMSG26, 
 	  VISMSG27, 
 	  VISMSG28, 
 	  VISMSG29, 
 	  VISMSG30, 
 	  VISMSG31,
 	  VISMSG32, 
 	  VISMSG33, 
 	  VISMSG34, 
 	  VISMSG35, 
 	  VISMSG36, 
 	  VISMSG37, 
 	  VISMSG38, 
 	  VISMSG39, 
 	  VISMSG40, 
 	  VISMSG41, 
 	  VISMSG42,
 	  VISMSG43, 
 	  VISMSG44, 
 	  VISMSG45, 
          VISMSG46,
          VISMSG47,
	  VISMSG48,
          VISMSGLAST = VISMSG48
	} VIS_MESSAGES;

         /* informational messages. */

typedef enum   {
          INFOMSG1 = VISMSGLAST+1,
 	  INFOMSG2, 
 	  INFOMSG3, 
	  INFOMSGLAST = INFOMSG3
       } INFO_MESSAGES;

	  /* performance messages */

typedef enum   {
	  PERFORMANCE1 = INFOMSGLAST+1,
	  PERFORMANCELAST = PERFORMANCE1
       } PERFORMANCE_MESSAGES;



