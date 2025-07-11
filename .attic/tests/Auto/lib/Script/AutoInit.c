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
static char rcsid[] = "$XConsortium: AutoInit.c /main/10 1995/07/14 11:27:45 drk $"
#endif
#endif
#include <mvslib.h>
#include <testlib.h>
#include "AutoToken.h"
#include "Automation.h"

#define	MULTI_CLICK_INTERVAL	750

void 			InitTestAutomation();
AutoCommandStructPtr    AutoCommandRoot;


void
InitTestAutomation(
        int argc,
        char **argv )
{

    /* Get proper testName for creation of recorded data files */

    strcpy(mvsTestName,argv[0]);

    /* Determine if you are recording screens or playing them back */

    if (AutoRecord)
    	mvsSetVisualMode(VISUAL_MODE_GENERATE_PIXMAPS);
    else if (AutoManual)
    	mvsSetVisualMode(VISUAL_MODE_INTERACTIVE_COMPARE);
    else if (AutoNoCheck)
	mvsSetVisualMode(VISUAL_MODE_DONT_CHECK);
    else
	mvsSetVisualMode(VISUAL_MODE_BATCH_COMPARE);

    /* Clear the color Cache */

    visClearColorCache();

    /* Set Focus Mode for Input Synthesis */

    xisSetFocusMode(KEYBOARD_FOCUS_NATURAL);

    /* Set MultiClick Interval Time */

    xisSetMultiClickTime(MULTI_CLICK_INTERVAL);

    /* Do not do Grabs/UnGrabs on Button Preses/Releases */

    xisPointerGrabMode = POINTER_GRAB_NATURAL;

    /* Setup the Root node of the Comand Structures */

    AutoCommandRoot = (AutoCommandStructPtr) 
				malloc(sizeof(AutoCommandStruct));
    
    AutoCommandRoot->CommandName = -1;
    AutoCommandRoot->Mask[0] = 0;
    AutoCommandRoot->NumMask = 0;
    AutoCommandRoot->ButtonNumber = 0;
    AutoCommandRoot->Key = 0;
    AutoCommandRoot->KeyOrButtonCount = 0;
    AutoCommandRoot->WidgetName = NULL;
    AutoCommandRoot->WidgetComponent = -1;
    AutoCommandRoot->SystemCommand = NULL;
    AutoCommandRoot->XCoord = 0;
    AutoCommandRoot->YCoord = 0;
    AutoCommandRoot->Location = -1;
    AutoCommandRoot->Gravity = 0;

    AutoCommandRoot->next = NULL;

}
