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
 * 
 */

#ifndef _XmExt_h_
#define _XmExt_h_


/************************************************************
*	INCLUDE FILES
*************************************************************/

#include <Xm/Xm.h>

#if defined(__cplusplus)
extern "C" {
#endif



typedef Widget (*XmWidgetFunc)(Widget);


/************************************************************
*	TYPEDEFS AND DEFINES
*************************************************************/

/*
 * General name definitions.
 */


#define XmNbadActionParameters "badActionParameters"
#define XmNbadActionParametersMsg \
    "Bad parameters passed to an action routine of widget '%s'."

#define XmNbadMotionParams "badMotionParams"
#define XmNbadMotionParamsMsg \
"%s - Motion Action : must have exactly 1 parameter, either '%s' or '%s'."

#define XmNbadRowPixmap "badRowPixmap"
#define XmNbadRowPixmapMsg "Extended List: Row pixmaps must have depth of 1."

#define XmNbadXlfdFont "badXlfdFont"
#define XmNbadXlfdFontMsg \
"%s: All Xlfd fonts must contain 14 hyphens\n'%s' is not valid."

#define XmNcellNotEmpty "cellNotEmpty"
#define XmNcellNotEmptyMsg "XmIconBox: Cell %s is not empty"

#define XmNcolorNameTooLong "colorNameTooLong"
#define XmNcolorNameTooLongMsg \
  "Color name '%s' is too long, truncated to '%s'."

#define XmNcontextSaveFailed "contextSaveFailed"
#define XmNcontextSaveFailedMsg "Internal Error: Could not save context data."

#define XmNconversionFailure "conversionFailure"
#define XmNconversionFailureMsg \
	"%s: Unable to perform string to %s conversion."

#define XmNcouldNotFindFamilyData "couldNotFindFamilyData"
#define XmNcouldNotFindFamilyDataMsg \
  "Could not find family data for family '%s'."

#define XmNforceGreaterThanZero "forceGreaterThanZero"
#define XmNforceGreaterThanZeroMsg \
"XmNvisibleItemCount must be greater than zero being reset to one (1)."
    
#define XmNinsertBeforeNotSibling "insertBeforeNotSibling"
#define XmNinsertBeforeNotSiblingMsg "XmHierarchy: InsertBefore Widget\
 is not a sibling of '%s'.\nInserting child at end of list."
    
#define XmNnoComboShell "noComboShell"
#define XmNnoComboShellMsg \
    "Combination Box: When using a custom combo box a shell must be provided."

#define XmNnoEmptyCells "noEmptyCells"
#define XmNnoEmptyCellsMsg "XmIconBox: Could not find any empty cells."

#define XmNnoGadgetSupport "noGadgetSupport"
#define XmNnoGadgetSupportMsg "Widget does not support gadget children."
    
#define XmNpixEditBadImageCreate "pixEditBadImageCreate"
#define XmNpixEditBadImageCreateMsg "Pixmap Editor: Can't allocate image data"

#define XmNsameAsImageOrPix "sameAsImageOrPix"
#define XmNsameAsImageOrPixMsg \
"%s : The bitmapMode resource can only be changed at the same time as the pixmap or image"

#define XmNselfOrOutsideOfApplicationDrop "selfOrOutsideOfApplicationDrop"
#define XmNselfOrOutsideOfApplicationDropMsg \
    "Attempt to drop into illegal object."

#define XmNstaticTranslationsMsg \
    "Cannot change translation table after initialization."

#define XmNstaticScrollbarsMsg \
    "Cannot change scrollbars after initialization."

#define XmNtextVerifyFailed "textVerifyFailed"
#define XmNtextVerifyFailedMsg "Combination Box: Text item validation failed."

#define XmNunexpectedEvent "unexpectedEvent"
#define XmNunexpectedEventMsg "Unexpected Event Type %s.\n"

#define XmNunparsableColor "unparsableColor"
#define XmNunparsableColorMsg \
  "Could not parse the color name '%s'."

#define XmNnodeParentIsSelf "nodeParentIsSelf"
#define XmNnodeParentIsSelfMsg \
  "The node parent cannot be self referential."

#define XmNstringGetFailed "stringGetFailed"
#define XmNstringGetFailedMsg "Unable to get MB string from XmString."

/************************************************************
*	MACROS
*************************************************************/

/************************************************************
*	GLOBAL DECLARATIONS
*************************************************************/

/************************************************************
*	EXTERNAL DECLARATIONS
*************************************************************/
	  

void XmCopyISOLatin1Lowered(char *, char *);

int XmCompareISOLatin1(char*, char*);

Boolean XmCompareXtWidgetGeometryToWidget(XtWidgetGeometry*, Widget);

Boolean XmCompareXtWidgetGeometry(XtWidgetGeometry*, XtWidgetGeometry*);


#if defined(__cplusplus)
}
#endif

#if defined(VMS) || defined(__VMS)
#include <X11/apienvrst.h>
#endif

#endif /* __Ext_h__ */
