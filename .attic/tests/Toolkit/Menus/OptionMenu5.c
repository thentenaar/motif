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
static char rcsid[] = "$XConsortium: OptionMenu5.c /main/8 1995/07/13 18:50:41 drk $"
#endif
#endif


#include <testlib.h>

#include <X11/bitmaps/star>
#include <X11/bitmaps/opendot>
#include <X11/bitmaps/black>
#include <X11/bitmaps/boxes>
#include <X11/bitmaps/icon>
#include <X11/bitmaps/xlogo16>
#include <X11/bitmaps/xlogo32>
#include <X11/bitmaps/wide_weave>
#include <X11/bitmaps/target>

#define NUM_CHILDREN 6
#define NUM_VALUES 8

Pixmap
makepixmap(Widget toplevel, char bits[], int width, int height)
{
  return (Pixmap)XCreatePixmapFromBitmapData(XtDisplay(toplevel),
			   DefaultRootWindow(XtDisplay(toplevel)),
                           bits,
                           width,
                           height,
                           BlackPixelOfScreen(XtScreen(toplevel)),
                           WhitePixelOfScreen(XtScreen(toplevel)),
                           DefaultDepthOfScreen(XtScreen(toplevel)));
}

/*
** change the labelString and labelPixmap for the object by cycling through
** a pre-prepared set of values
*/
/*ARGSUSED*/

void cycle_values (Widget w, XtPointer client_data, XtPointer call_data)
{
  Widget entry = (Widget) client_data;
  static char *orig_strings[NUM_VALUES] = {
    "1",
    "second in series",
    "third in series",
    "fourth in series",
    "fifth in series",
    "sixth in series",
    "the seventh in the series",
    "last in series"
    };

  static struct bitmap_data {
    char *bits;
    int width;
    int height;
  } orig_pixmap[] = 
    {
      {(char *) star_bits, star_width, star_height},
      {(char *) opendot_bits, opendot_width, opendot_height},
      {(char *) black_bits, black_width, black_height},
      {(char *) boxes_bits, boxes_width, boxes_height},
      {(char *) icon_bits, icon_width, icon_height},
      {(char *) xlogo16_bits, xlogo16_width, xlogo16_height},
      {(char *) xlogo32_bits, xlogo32_width, xlogo32_height},
      {(char *) wide_weave_bits, wide_weave_width, wide_weave_height},
    };

  static XmString strings[NUM_VALUES];
  static Pixmap pixmaps[NUM_VALUES];
  Arg args[5];
  int n;
  int i;
  static Boolean values_set = False;
  static int which_one = 0;

  if (!values_set)
    {
      values_set = True;
      for (i=0; i < NUM_VALUES; i++)
	{
	  strings[i] = XmStringCreateSimple(orig_strings[i]);
	  pixmaps[i] = makepixmap(w,orig_pixmap[i].bits,
				orig_pixmap[i].width,orig_pixmap[i].height);
	}
      /* gosh; remember to destroy these ! */
    }
  n = 0;
  XtSetArg(args[n], XmNlabelString, strings[which_one]); 	n++;
  XtSetArg(args[n], XmNlabelPixmap, pixmaps[which_one]); 	n++;
  XtSetValues (entry, args, n);

  which_one++;
  which_one %= NUM_VALUES;
}

/*ARGSUSED*/
void change_sensitive (Widget w, XtPointer client_data, XtPointer call_data)
{
  Widget option = (Widget) client_data;
  XmToggleButtonCallbackStruct *info = 
    (XmToggleButtonCallbackStruct *)call_data;
  Arg args[1];
  int n;
  XmString tcs;

  n = 0;
  XtSetArg (args[n], XmNsensitive, info->set); 		n++;
  XtSetValues (option, args, n);

  if (! info->set)
     tcs = XmStringCreateSimple("Option's XmNsensitive = False");
  else
     tcs = XmStringCreateSimple("Option's XmNsensitive = True");

  n = 0;
  XtSetArg(args[n], XmNlabelString, tcs);		n++;
  XtSetValues(w, args, n);

}

/*ARGSUSED*/
void change_labelType(Widget w, XtPointer client_data, XtPointer call_data)
{
  Widget *entries = (Widget *)client_data;
  XmToggleButtonCallbackStruct *info =
      (XmToggleButtonCallbackStruct *)call_data;
  unsigned char labelType;
  XmString tcs;
  int i, n;
  Arg args[2];

  /* find the value from the first, change it in all the others */
  n = 0;
  XtSetArg(args[n], XmNlabelType, &labelType); 		n++;
  XtGetValues(entries[0], args, n);

  if (XmSTRING == labelType) 
    XtSetArg(args[0], XmNlabelType, XmPIXMAP); 
  else	
    XtSetArg(args[0], XmNlabelType, XmSTRING); 

  for (i=0; i < NUM_CHILDREN; i++)
    XtSetValues(entries[i], args, 1);

  if (! info->set)
     tcs = XmStringCreateSimple("Menu items' XmNlabelType = XmPIXMAP");
  else
     tcs = XmStringCreateSimple("Menu items' XmNlabelType = XmSTRING");

  n = 0;
  XtSetArg(args[n], XmNlabelString, tcs);		n++;
  XtSetValues(w, args, n);
}


main(argc, argv)
     int argc;
     char *argv[];
{  
  Widget form, rc, option, buttons[NUM_CHILDREN], entries[NUM_CHILDREN], 		toggles;
  Widget sensitive, labelType;
  Widget pulldown, bb;
  Arg args[10]; 
  int n;
  int i;
  XmString dummy;

  /* initialize toolkit; classname is XMcommon; sets global Shell1  */

  CommonTestInit(argc, argv);
  
  n = 0;
  form = XmCreateForm (Shell1, "form", args, n);		n++;

  n = 0;
  toggles = XmCreateForm (form, "toggles", args, n);		n++;

  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE);		n++;
  XtSetArg(args[n], XmNtopPosition, 1);				n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION);	n++;
  XtSetArg(args[n], XmNbottomPosition, 23);			n++;
  XtSetArg(args[n], XmNfractionBase, 100);			n++;
  XtSetValues (toggles, args, n);

  n = 0;
  rc = XmCreateForm (form, "rc", args, n);
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE);		n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION);	n++;
  XtSetArg(args[n], XmNtopPosition, 24);			n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION);	n++;
  XtSetArg(args[n], XmNbottomPosition, 64);			n++;
  XtSetArg(args[n], XmNfractionBase, 100);			n++;
  XtSetValues (rc, args, n);

  dummy = XmStringCreateSimple("Option's XmNsensitive = True");

  n = 0;
  XtSetArg(args[n], XmNset, True); 				n++;
  XtSetArg(args[n], XmNlabelString, dummy);			n++;
  sensitive = XmCreateToggleButton(toggles, "sensitivity", args, n);
  XtManageChild(sensitive);
  XmStringFree(dummy);

  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION);	n++;
  XtSetArg(args[n], XmNbottomPosition, 50);			n++;
  XtSetValues (sensitive, args, n);

  dummy = XmStringCreateSimple("Menu items' XmNlabelType = XmSTRING");

  n = 0;
  XtSetArg(args[n], XmNset, True); 				n++;
  XtSetArg(args[n], XmNlabelString, dummy);			n++;
  labelType = XmCreateToggleButton(toggles, "labelType", args, n);
  XtManageChild(labelType);
  XmStringFree(dummy);

  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);		n++;
  XtSetArg(args[n], XmNtopPosition, 50);			n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION);	n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
  XtSetValues (labelType, args, n);

  n = 0;
  bb = XmCreateBulletinBoard(form, "bb", args, n);
  XtManageChild(bb);

  n = 0;
  XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
  XtSetArg(args[n], XmNrightAttachment, XmATTACH_NONE);		n++;
  XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION);	n++;
  XtSetArg(args[n], XmNtopPosition, 65);			n++;
  XtSetArg(args[n], XmNbottomAttachment, XmATTACH_POSITION);	n++;
  XtSetArg(args[n], XmNbottomPosition, 99);			n++;
  XtSetValues (bb, args, n);

  n = 0;
  pulldown = XmCreatePulldownMenu(bb, "pulldown", args, n);

  dummy = XmStringCreateSimple("Label/LabelG Menu Items");

  n = 0;
  XtSetArg(args[n],XmNsubMenuId, pulldown);			n++;
  XtSetArg(args[n], XmNlabelString, dummy);			n++;
  option=XmCreateOptionMenu(bb, "option_menu", args, n);
  XmStringFree(dummy);

  XtAddCallback(sensitive, XmNvalueChangedCallback, change_sensitive,
	        (Widget)option);
  XtAddCallback(labelType, XmNvalueChangedCallback, change_labelType,
	        (Widget *)entries);

  /* enumerate interesting subclasses of Label and LabelGadget */
  entries[0] = XmCreateLabelGadget (pulldown, "lg", NULL, 0);
  entries[1] = XmCreateLabel (pulldown, "label", NULL, 0);
  entries[2] = XmCreatePushButtonGadget (pulldown, "pbg",NULL,0);
  entries[3] = XmCreatePushButton (pulldown, "pb", NULL, 0);
  entries[4] = XmCreateToggleButtonGadget (pulldown, "tbg", NULL, 0);
  entries[5] = XmCreateToggleButton (pulldown, "tb", NULL, 0);

  {
	/* give an initial harmless XmNlabelPixmap */
  int i;
  Pixmap target = makepixmap(Shell1,target_bits, target_width,target_height);

  XtSetArg(args[0], XmNlabelPixmap, target);
  for (i=0; i< NUM_CHILDREN; i++)
	XtSetValues(entries[i], args,1);
  }

  /* if the option appears as the name then the 
  ** updating of the label is busted 
  */
  for (i=0; i < NUM_CHILDREN; i++) {
	char buffer[100];
	char lab[50];

	sprintf(buffer, "Change pulldown entry #%d", i);
	sprintf(lab, "entry%d", i);

	dummy = XmStringCreateSimple(buffer);

	n = 0;
	XtSetArg(args[n], XmNlabelString, dummy);		n++;
	buttons[i] = XmCreatePushButton(rc, lab, args, n);
  	XmStringFree(dummy);

	XtAddCallback(buttons[i], XmNactivateCallback, cycle_values, 
		      entries[i]);
	n = 0;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_POSITION);	n++;
	XtSetArg(args[n], XmNtopPosition, i*100/NUM_CHILDREN);	n++;	
	XtSetValues(buttons[i], args, n);

  }

  XtManageChildren(buttons, NUM_CHILDREN);
  XtManageChildren(entries, NUM_CHILDREN);

  XtManageChild(rc);
  XtManageChild(toggles);
  XtManageChild(form);
  XtManageChild(option);

  /* if we realize after setting the labels, there is no problem; it works.
  ** The sequence of realizing is important only because the option menu
  ** sets the cascade label at realize time, so we set afterwards so that if
  ** the name as the label shows we know it's wrong.  
  */
#ifdef DONT_SHOW_INITIAL_BUG
  XtRealizeWidget(Shell1);
#endif

  {
/* confirm that we can change the labels programmatically and have the
** labels appear correctly in the XmOptionButtonGadget before the menu has
** been popped up by the user . So we set them here to what they should 
** appear as. We should see "Label Gadget" echoed.
** Optimally, we will also see correct geometry for the CBG.
*/
  static char *strings[NUM_VALUES]=
	{
	  "Label Gadget", 
	  "Label",
	  "Push Button Gadget",
	  "Push Button",
	  "Toggle Button Gadget",
	  "Toggle Button"
	};

  int i;
  XmString dummy;

  for (i=0; i< NUM_CHILDREN; i++)
	{
	dummy = XmStringCreateSimple(strings[i]);
    
	n = 0;
	XtSetArg(args[n], XmNlabelString, dummy);		n++;
	XtSetValues(entries[i], args, n);
	XmStringFree(dummy);
	}   
  }

#ifndef DONT_SHOW_INITIAL_BUG
  XtRealizeWidget(Shell1);
#endif

  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();
  CommonPause();

/* declared elsewhere */
  XtAppMainLoop(app_context);
}
