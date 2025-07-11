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

/*
 * This file contains time routines used by the performance monitor
 */

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: AutoTime.c /main/7 1996/10/07 15:03:58 drk $"
#endif
#endif


#include <sys/time.h>

#include "testlib.h"
#include "AutoMessages.h"

struct timeval StartTime, EndTime;



void
AutoGetTime(enum WHICH_TIME time_to_get)
{
  struct timezone zone_time;

  if (time_to_get == START_TIME)  {
    if (gettimeofday(&StartTime, &zone_time) != 0)
      AutoMessage(_AutoMessages[WARNMSG126]);
  }
  else 
    if (time_to_get == END_TIME)  {
      if (gettimeofday(&EndTime, &zone_time) != 0)
	  printf("AUTOWARNING** : Could not gettimeofday\n");
      /* COMMENTED OUT UNTIL REPORT GEN BUG IS FIXED
	  AutoMessage(_AutoMessages[WARNMSG126]);
       */
    }
    else
	printf("AUTOWARNING** : AutoGetTime not start or end time\n");
        /*  COMMENTED OUT UNTIL REPORT GEN BUG IS FIXED
        AutoMessage(_AutoMessages[WARNMSG127]);
	 */
}



void
AutoPrintDurationTime(struct timeval *start_time, 
		      struct timeval *end_time)
{

  struct timeval lapsed;
  char msg_string[75];

  if (start_time->tv_usec > end_time->tv_usec)  {
    end_time->tv_usec += 1000000;
    end_time->tv_sec--;
  }

  lapsed.tv_usec = end_time->tv_usec - start_time->tv_usec;
  lapsed.tv_sec = end_time->tv_sec - start_time->tv_sec;


  printf("PERFORMANCE_MESSAGE** : Automation command duration is %ld.%03d seconds\n",
	 (long)lapsed.tv_sec, lapsed.tv_usec%1000);
  /*  COMMENTED OUT UNTIL REPORT GEN BUG IS FIXED
  sprintf(msg_string, _AutoMessages[PERFORMANCE1],
	  lapsed.tv_sec, lapsed.tv_usec%1000000);
  AutoMessage(msg_string); 
   */
}

