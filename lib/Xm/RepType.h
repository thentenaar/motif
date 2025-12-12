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
#ifndef _XmRepType_h
#define _XmRepType_h


#include <Xm/Xm.h>


#ifdef __cplusplus
extern "C" {
#endif


#define XmREP_TYPE_INVALID		0x1FFF

typedef unsigned short XmRepTypeId ;

typedef struct
{
    String rep_type_name ;
    String *value_names ;
    unsigned char *values ;
    unsigned char num_values ;
    Boolean reverse_installed ;
    XmRepTypeId rep_type_id ;
    }XmRepTypeEntryRec, *XmRepTypeEntry, XmRepTypeListRec, *XmRepTypeList ;


/********    Public Function Declarations    ********/

extern XmRepTypeId XmRepTypeRegister(
                        String rep_type,
                        String *value_names,
                        unsigned char *values,
                        unsigned char num_values) ;
extern void XmRepTypeAddReverse(
                        XmRepTypeId rep_type_id) ;
extern Boolean XmRepTypeValidValue(
                        XmRepTypeId rep_type_id,
                        unsigned char test_value,
                        Widget enable_default_warning) ;
extern XmRepTypeList XmRepTypeGetRegistered( void ) ;
extern XmRepTypeEntry XmRepTypeGetRecord(
                        XmRepTypeId rep_type_id) ;
extern XmRepTypeId XmRepTypeGetId(
                        String rep_type) ;
extern String * XmRepTypeGetNameList(
                        XmRepTypeId rep_type_id,
                        Boolean use_uppercase_format) ;
extern void XmRepTypeInstallTearOffModelConverter( void ) ;

/********    End Public Function Declarations    ********/



#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmRepType_h */
