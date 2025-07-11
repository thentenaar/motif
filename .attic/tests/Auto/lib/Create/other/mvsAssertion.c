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
static char rcsid[] = "$XConsortium: mvsAssertion.c /main/7 1995/07/14 11:18:49 drk $"
#endif
#endif
/***********************************************************************
  	@(#)mvsAssertion.c	1.11.1.1	Date:1/22/91
  	Author: SJS
	History:
	   04/13/90 SJS create
	   08/07/90 SJS add Failure counting in behavior
	   09/23/90 SJS add check for same assertion
	   11/13/90 PSN print label ASSERT in the beginning of each assertion 
			line being printed
	   11/19/90 PSN print different assertion messages for test/create test

	Calls:

	Summary:
		This file contains functions that add an assertion to
	the current assertion set and that clear the set of assertions.
	The assertion contains the Page reference and a short description
	of what is currently being validated. The global assertion arrays
	holds the pointers to each assertion that applies to the current
	validation. On a failure, the contents of this array are then
	printed out. To avoid multiple copies of assertions being given
	on related failures, the assertion count is cleared when a
	failure occurs.

************************************************************************/
#include <AutoMessages.h>
#include "mvslib.h"

#define MAX_ASSERTS_PER_VALIDATION 32
static mvsAssertCnt=0;
static char *mvsAssertArray[MAX_ASSERTS_PER_VALIDATION];

/***********************************************************************
   This function adds the string/assertion into the array of assertions.
   These assertions are printed out if a part of the validation fails.
***********************************************************************/
void mvsAssertion(assertion)
char *assertion;
{
    int i;

    if(mvsAssertCnt >= MAX_ASSERTS_PER_VALIDATION) {
        AutoMessage(_AutoMessages[WARNMSG125]);
    }
    else {
        for(i=0; i<mvsAssertCnt; i++) {
            if(assertion == mvsAssertArray[i]) /* Assertion already done */
            return;
        }
        mvsAssertArray[mvsAssertCnt] = assertion;
        mvsAssertCnt++;
    }
} /* End mvsAssertion() */


/***********************************************************************
    This function zeros the assertion count. This signifies the beginning
   of a new validation set with new assertions to be filled in.
***********************************************************************/
void mvsClearAsserts()
{
    mvsAssertCnt = 0;
} /* End mvsClearAsserts() */


/***********************************************************************
    This function prints out the list of assertions associated with
   the current validation.
***********************************************************************/
void mvsFailAssert()
{
    int i;

    for(i=0; i<mvsAssertCnt; i++) {
        if(mvsInBehavior)
           msg("    ASSERT: %s\n", mvsAssertArray[i]);
	else
	   msg("    %s\n", mvsAssertArray[i]);
    }

    if(mvsInBehavior) {
        mvsFailCnt++;
    }
    mvsClearAsserts();

} /* End mvsFailAssert() */
