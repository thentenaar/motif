/* $XConsortium: Container5.c /main/5 1995/07/15 21:03:46 drk $ */
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
 * 
 */
/*
 * HISTORY
 */

#include <testlib.h>

#define NUM_OBJ    20
#define	NAME_LEN   20


void
main(int argc, char **argv)
{
    Widget              Container5, *IconGad;   
    int			i, num_obj;
    Cardinal		n;
    Arg			args[MAX_ARGS];
    char		test_char;
    int                 test_num;
    char		ContainerName[NAME_LEN + 1];

    ContainerName[0] = '\0';
    test_char = '\0';
    test_num = 0;

    CommonTestInit(argc, argv);

    num_obj = 10;

    if (UserData != NULL) {
	
	if (strcmp(UserData, "a") == 0) {
	    num_obj = 4;
	    test_num = 1;
	}

	if (strcmp(UserData, "b") == 0) {
	    num_obj = 5;
	    test_num = 2;
	}
	
	if (strcmp(UserData, "c") == 0) {
	    num_obj = 9;
	    test_num = 3;
	}
	
	if (strcmp(UserData, "d") == 0) {
	    num_obj = 20;
	    test_num = 4;
	}
	
	if (strcmp(UserData, "e") == 0) {
	    num_obj = 4;
	    test_num = 5;
	}
	
	test_char = *UserData;
	
	free(UserData);
	
    }
    sprintf(ContainerName, "Container5%c", test_char);

    n = 0;

    Container5 = XmCreateContainer(Shell1, ContainerName, args, n);
    XtManageChild(Container5);

    XtRealizeWidget(Shell1);

    CommonPause();

    /* Add a bunch of Icon Gadgets */

    IconGad = (Widget*) XtMalloc(NUM_OBJ*sizeof(Widget));

    for (i = 0; i < num_obj; i++) {
	char        IconName[20] ;
	XmString   icon_name;

	n = 0 ;
	sprintf(IconName, "IconGad%d", i);
	icon_name = XmStringGenerate(IconName, NULL, XmCHARSET_TEXT, NULL); 
	XtSetArg(args[n], XmNlabelString, icon_name); n++;
	IconGad[i] = XmCreateIconGadget(Container5, IconName, args, n);
        XtManageChild(IconGad[i]);
	XmStringFree(icon_name);
    }


    CommonPause();

    if (test_num == 0 || test_num == 1) {
    
	/* Set the Spatial Style to None */

	n = 0;
	XtSetArg(args[n], XmNspatialStyle, XmNONE); n++;
	XtSetValues(Container5, args, n);

	CommonPause();

	/* Unmanage all the children - Change the ResizeModel - and Remanage */

	XtUnmanageChildren(IconGad, num_obj);
    
	n = 0;
	XtSetArg(args[n], XmNspatialStyle, XmGRID); n++;
	XtSetArg(args[n], XmNspatialResizeModel, XmGROW_BALANCED); n++;
	XtSetValues(Container5, args, n);

	XtManageChildren(IconGad, num_obj);

	CommonPause();
    }

    if (test_num == 2) {
	int new_obj;

	/* Test the different ResizeModel - Start by adding children with
	   grow_balanced, then add with grow_major and then grow_minor. */

	new_obj = 4;
	for (i = num_obj; i < (num_obj+new_obj); i++) {
	    char        IconName[20] ;
	    XmString   icon_name;

	    n = 0 ;
	    sprintf(IconName, "IconGad%d", i);
	    icon_name = XmStringGenerate(IconName, NULL, XmCHARSET_TEXT, NULL);
	    XtSetArg(args[n], XmNlabelString, icon_name); n++;
	    IconGad[i] = XmCreateIconGadget(Container5, IconName, args, n);
	    XmStringFree(icon_name);
	}

	XtManageChildren(&IconGad[num_obj], new_obj);
	num_obj = num_obj + new_obj;
	
	CommonPause();

	new_obj = 5;
	n = 0;
	XtSetArg(args[n], XmNspatialResizeModel, XmGROW_MAJOR); n++;
	XtSetValues(Container5, args, n);

	for (i = num_obj; i < (num_obj+new_obj); i++) {
	    char        IconName[20] ;
	    XmString   icon_name;

	    n = 0 ;
	    sprintf(IconName, "IconGad%d", i);
	    icon_name = XmStringGenerate(IconName, NULL, XmCHARSET_TEXT, NULL);
	    XtSetArg(args[n], XmNlabelString, icon_name); n++;
	    IconGad[i] = XmCreateIconGadget(Container5, IconName, args, n);
	    XmStringFree(icon_name);
	}

	XtManageChildren(&IconGad[num_obj], new_obj);
	num_obj = num_obj + new_obj;
	
	CommonPause();

	n = 0;
	XtSetArg(args[n], XmNspatialResizeModel, XmGROW_MINOR); n++;
	XtSetValues(Container5, args, n);

	for (i = num_obj; i < (num_obj+new_obj); i++) {
	    char        IconName[20] ;
	    XmString   icon_name;

	    n = 0 ;
	    sprintf(IconName, "IconGad%d", i);
	    icon_name = XmStringGenerate(IconName, NULL, XmCHARSET_TEXT, NULL);
	    XtSetArg(args[n], XmNlabelString, icon_name); n++;
	    IconGad[i] = XmCreateIconGadget(Container5, IconName, args, n);
	    XmStringFree(icon_name);
	}

	XtManageChildren(&IconGad[num_obj], new_obj);
	num_obj = num_obj + new_obj;
	
    }

    if (test_num == 3) {
	char        IconName[20];
	XmString   icon_name;

	/* Change the spatialStyle to GRID */

	n = 0;
	XtSetArg(args[n], XmNspatialStyle, XmGRID); n++;
	XtSetValues(Container5, args, n);

	CommonPause();

	/* Test the spatialIncludeModel 
	   Add some children and check where they go */

	i = num_obj;
	n = 0 ;
	sprintf(IconName, "IconGad%d", i);
	icon_name = XmStringGenerate(IconName, NULL, XmCHARSET_TEXT, NULL);
	XtSetArg(args[n], XmNlabelString, icon_name); n++;
	IconGad[i] = XmCreateIconGadget(Container5, IconName, args, n);
	XmStringFree(icon_name);
	XtManageChild(IconGad[i]);

	CommonPause();

	/* set the spatialIncludeModel to FIRST_FIT */

	n = 0;
	XtSetArg(args[n], XmNspatialIncludeModel, XmFIRST_FIT); n++;
	XtSetValues(Container5, args, n);

	i = num_obj + 1;
	n = 0 ;
	sprintf(IconName, "IconGad%d", i);
	icon_name = XmStringGenerate(IconName, NULL, XmCHARSET_TEXT, NULL);
	XtSetArg(args[n], XmNlabelString, icon_name); n++;
	IconGad[i] = XmCreateIconGadget(Container5, IconName, args, n);
	XmStringFree(icon_name);
	XtManageChild(IconGad[i]);
	
    }

    CommonPause();

    XtAppMainLoop(app_context);

}

