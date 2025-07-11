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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: ColorM1.c /main/8 1995/07/13 18:34:42 drk $"
#endif
#endif
#include <testlib.h>
#include "ColorM1.h"
#include <varargs.h>

static char toggle_flag = 'F';
static unsigned long pixels_return2[5];
static unsigned long plane_masks_return2[1];
static MrmCode	 class ;
static Widget color_main;           
static char  *global_current_file;   

static void default_colors ();
static void initialize_coords();

static void create_colorform(char *resourcestring, char *valuestring, 
                             int item_to_adjust);
static void destroy_colorform(Widget widget, XtPointer client_data, 
			      XtPointer call_data);
static void color_selection_made(Widget widget, XtPointer client_data, 
				 XtPointer call_data);
static void colorlist_selection_made(Widget widget, XtPointer client_data, 
				 XtPointer call_data);
static void scale_value_changed(Widget widget, XtPointer client_data, 
				XtPointer call_data);
static void load_rgb_text(Widget widget, XtPointer client_data, 
			  XtPointer call_data);
static void setcolor_button_pressed(Widget widget, XtPointer client_data, 
				    XtPointer call_data);
static void toggle_button_pressed(Widget widget, XtPointer client_data, 
				  XtPointer call_data);
static void exit_color(Widget widget, XtPointer client_data, 
		       XtPointer call_data);
static void setcolor_colorform(Widget form, char *valuestring);
static char *stringdup(const char *s);
static char *extract_first_xms_segment(XmString cs);
static void fetch(char *widget_index_string, Widget *widgetptr);

static char		*uid_files[]={"ColorM1.uid"};
static int num_uid_files = sizeof(uid_files) / sizeof(char *);

static MrmRegisterArg	callbackvec[] = {
{"setcolor_button_pressed",(caddr_t)setcolor_button_pressed},
{"toggle_button_pressed",(caddr_t)toggle_button_pressed},
{"color_selection_made",(caddr_t)color_selection_made},
{"create_colorform",(caddr_t)create_colorform},
{"destroy_colorform",(caddr_t)destroy_colorform},
{"scale_value_changed",(caddr_t)scale_value_changed},
{"exit_color",(caddr_t)exit_color},
{"load_rgb_text",(caddr_t)load_rgb_text}
};

static MrmCount callbacknum = sizeof(callbackvec) / sizeof(MrmRegisterArg);

static XtResource resources[] = {
{ XtNrgbTextFile, XtCRgbTextFile, XtRString, sizeof(String),
 XtOffset(ApplicationDataPtr,rgb_text_file),XtRString, "/usr/lib/X11/rgb.txt"}
};

Widget fetched;

typedef struct {
  int    item_to_adjust;
  char   colorname[40];
  char   redname[10];
  char   greenname[10];
  char   bluename[10];
  XColor color; 
  unsigned long fg_pixel;/* foreground   */
  unsigned long bg_pixel;/* background   */
  unsigned long ts_pixel;/* topshadow    */
  unsigned long bs_pixel;/* bottomshadow */
  unsigned long arm_pixel;/* arm color */
} ColorData, *ColorDataPtr;


static void create_colorform(char *resourcestring, char *valuestring, 
                             int item_to_adjust)

{
    ColorDataPtr colordata;
    ColorDataPtr colordata2;
    Arg arg[5];
    unsigned long plane_masks_return[1];
    unsigned long pixels_return[4];
    Status status;
    Boolean false = 0;
    XColor color;
    XColor color2;

    fetched = (Widget) 0;
    colordata = (ColorDataPtr) XtCalloc(1,sizeof(ColorData));
    colordata2 = (ColorDataPtr) XtCalloc(1,sizeof(ColorData));
    colordata->item_to_adjust = item_to_adjust;
    colordata2->item_to_adjust = item_to_adjust;
    status = XAllocColorCells(
			  dpy,
			  cmap,
			  false,
			  plane_masks_return,
			  1,
			  pixels_return,
			  4);
    if(!status){
      no_colors_available = 1;
      return;
    }

    /* Allocate colors for color change P.B. */

    status = XAllocColorCells(
			  dpy,
			  cmap,
			  false,
			  plane_masks_return2,
			  1,
			  pixels_return2,
			  5);
    if(!status){
      no_colors_available = 1;
      return;
      }

    colordata->color.pixel = pixels_return[0];
    colordata->color.flags = DoRed | DoGreen | DoBlue;
    colordata->fg_pixel = pixels_return[0];
    colordata->bg_pixel = pixels_return[1];
    colordata->ts_pixel = pixels_return[2];
    colordata->bs_pixel = pixels_return[3];

    initialize_coords();

    fetch("color_form",&fetched);

    /* store userData */
	if ( fetched == NULL ) {
		exit(1);
	} else {
    XtSetArg(arg[0],XmNuserData,colordata);
    XtSetValues(fetched,arg,1);
    setcolor_colorform(fetched,valuestring);
	}

    /* set the background (and other fields) of the color patch */
    /* to the allocated color and associated colors */

    color.pixel = pixels_return[0];
    XQueryColor(dpy,cmap,&color);
    color2.pixel = pixels_return[0];
    XQueryColor(dpy,cmap,&color2);
    color.pixel = pixels_return[2];
    XStoreColor(dpy,cmap,&color);
    color.pixel = pixels_return[3];
    XStoreColor(dpy,cmap,&color);

    /* note that foreground is initialized to contrast with background */
    color.red = 65535 - color.red;
    color.green = 65535 - color.green;
    color.blue = 65535 - color.blue;
    color.pixel = pixels_return[1];
    XStoreColor(dpy,cmap,&color);

    color2.red = 0;
    color2.green = 0;
    color2.blue = 0;
    color2.pixel = pixels_return2[1];
    XStoreColor(dpy,cmap,&color2);

    color2.red = 0;
    color2.green = 0;
    color2.blue = 0;
    color2.pixel = pixels_return2[2];
    XStoreColor(dpy,cmap,&color2);

    color2.red = 0;
    color2.green = 0;
    color2.blue = 0;
    color2.pixel = pixels_return2[3];
    XStoreColor(dpy,cmap,&color2);

    color2.red = 0;
    color2.green = 0;
    color2.blue = 0;
    color2.pixel = pixels_return2[4];
    XStoreColor(dpy,cmap,&color2);

    color2.red = 65535;
    color2.green = 65535;
    color2.blue = 65535;
    color2.pixel = pixels_return2[0];
    XStoreColor(dpy,cmap,&color2);

    XtSetArg(arg[0],XmNbackground,pixels_return[0]);
    XtSetArg(arg[1],XmNforeground,pixels_return[1]);
    XtSetArg(arg[2],XmNtopShadowColor,pixels_return[2]);
    XtSetArg(arg[3],XmNbottomShadowColor,pixels_return[3]);
    XtSetValues(XtNameToWidget(fetched,"color_patch"),arg,4);

    XtSetArg(arg[0],XmNbackground,pixels_return2[0]);
    XtSetArg(arg[1],XmNforeground,pixels_return2[1]);
    XtSetArg(arg[2],XmNtopShadowColor,pixels_return2[2]);
    XtSetArg(arg[3],XmNbottomShadowColor,pixels_return2[3]);
    XtSetArg(arg[4],XmNarmColor,pixels_return2[4]);
    XtSetValues(XtNameToWidget(fetched,"color_change_BB.color_change"),arg,5);

    XtSetArg(arg[0],XmNbackground,pixels_return2[0]);
    XtSetValues(XtNameToWidget(fetched,"color_change_BB"),arg,1);

    XtManageChild(fetched);
}


static void destroy_colorform(Widget widget, XtPointer client_data, 
			      XtPointer call_data)
{
  Arg arg[1];
  ColorDataPtr userdata;

  /* get the userdata and free it*/
  XtSetArg(arg[0],XmNuserData,&userdata);
  XtGetValues(widget,arg,1);
  XtFree((XtPointer) userdata);
}


/* callback notified when selection is made */
static void color_selection_made(Widget widget, XtPointer client_data, 
				 XtPointer call_data)
{
  char *color_string;
  XmSelectionBoxCallbackStruct *callback_data;

  callback_data = (XmSelectionBoxCallbackStruct *) call_data;

  color_string = extract_first_xms_segment(callback_data->value);
  setcolor_colorform(XtParent(widget),color_string);
}


/* callback notified when selection is made */
static void colorlist_selection_made(Widget widget, XtPointer client_data, 
				     XtPointer call_data)
{
  char *color_string;
  XmListCallbackStruct *callback_data;

  callback_data  = (XmListCallbackStruct *) call_data;

  color_string = extract_first_xms_segment(callback_data->item);
  /* the following code will fail if the list is not a descendant of a form */
  while(!XtIsSubclass(widget,xmFormWidgetClass))
    widget = XtParent(widget);
  setcolor_colorform(widget,color_string);
}


static void setcolor_colorform(Widget form, char *valuestring)
{
  Arg arg[1];
  ColorDataPtr colordata;
  Widget scale;
  Widget text;

  XtSetArg(arg[0],XmNuserData,&colordata);
  XtGetValues(form,arg,1);
  /* set the color cell to valuestring */
  if(*valuestring=='#'){
    XParseColor(dpy,cmap,valuestring,&(colordata->color));
    XStoreColor(dpy,cmap,&(colordata->color));
  }
  else 
    XStoreNamedColor(dpy,cmap,valuestring,
		     colordata->color.pixel,
		     colordata->color.flags);
  /* and fill the XColor struct with RGB values for scales */
  XQueryColor(dpy,cmap,&(colordata->color));

  /* based on the values in the color fields, set the slider values */
  scale = XtNameToWidget(form,"slider_box.scale1");
  XtSetArg(arg[0],XmNvalue,(int)(colordata->color.red/COLOR_PCT_MULTIPLIER));
  XtSetValues(scale,arg,1);
  sprintf(colordata->redname,"%4.4x",colordata->color.red);

  scale = XtNameToWidget(form,"slider_box.scale2");
  XtSetArg(arg[0],XmNvalue,(int)(colordata->color.green/COLOR_PCT_MULTIPLIER));
  XtSetValues(scale,arg,1);
  sprintf(colordata->greenname,"%4.4x",colordata->color.green);

  scale = XtNameToWidget(form,"slider_box.scale3");
  XtSetArg(arg[0],XmNvalue,(int)(colordata->color.blue/COLOR_PCT_MULTIPLIER));
  XtSetValues(scale,arg,1);
  sprintf(colordata->bluename,"%4.4x",colordata->color.blue);
    
  sprintf(colordata->colorname,valuestring);
  text = XmSelectionBoxGetChild(
	    XtNameToWidget(form,"color_sb"),
	    XmDIALOG_TEXT);
  XmTextSetString(text,valuestring);
}

static void scale_value_changed (Widget widget, XtPointer client_data,
				 XtPointer call_data)

{
  Widget form;
  Widget patch_widget;
  Widget text_widget;
  ColorDataPtr colordata;
  Arg    arg[2];
  XmScaleCallbackStruct *callback_data;
  char *tag;

  callback_data = (XmScaleCallbackStruct *) call_data;
  tag = (char *) client_data;

  form = XtParent(XtParent(widget));
  patch_widget = XtNameToWidget(form,"color_patch");
  text_widget = XmSelectionBoxGetChild(
	    XtNameToWidget(form,"color_sb"),
	    XmDIALOG_TEXT);

  XtSetArg(arg[0],XmNuserData,&colordata);
  XtGetValues(form,arg,1);

  /* based on the tag, set either the red,green, or blue static variables, */
  /* then set color of label0 */

  if(*tag == 'r'){
    colordata->color.red = callback_data->value * COLOR_PCT_MULTIPLIER;
    sprintf(colordata->redname,"%4.4x",colordata->color.red);
  }
  else if(*tag == 'g'){
    colordata->color.green = callback_data->value * COLOR_PCT_MULTIPLIER;
    sprintf(colordata->greenname,"%4.4x",colordata->color.green);
  }
  else if(*tag == 'b'){
    colordata->color.blue = callback_data->value * COLOR_PCT_MULTIPLIER;
    sprintf(colordata->bluename,"%4.4x",colordata->color.blue);
  }

  /* update the color cell */
  XStoreColor(dpy,cmap,&(colordata->color));

  /* assemble complete hex-string and set editable text to it */
  strcpy(colordata->colorname,"#");
  strcat(colordata->colorname,colordata->redname);
  strcat(colordata->colorname,colordata->greenname);
  strcat(colordata->colorname,colordata->bluename);

  XmTextSetString(text_widget,colordata->colorname);
}

static void toggle_button_pressed(Widget widget, XtPointer client_data, 
				  XtPointer call_data)
{
    XmToggleButtonCallbackStruct *callback_data;

    callback_data = (XmToggleButtonCallbackStruct *) call_data;

    if (callback_data->set == XmSET)
	toggle_flag = * (char *)client_data;
}

static void setcolor_button_pressed(Widget widget, XtPointer client_data, 
				  XtPointer call_data)
{
  Widget parent;
  Widget change_widget;
  XColor color;
  ColorDataPtr colordata;
  Arg arg[1];

  change_widget = XtNameToWidget(fetched,"color_change_BB.color_change");
  parent = XtParent(XtParent(change_widget));

  XtSetArg(arg[0],XmNuserData,&colordata);
  XtGetValues(parent,arg,1);

  /* store rgb values from foreground into appropriate field */
  color.red = colordata->color.red;
  color.green = colordata->color.green;
  color.blue = colordata->color.blue;
  color.flags = colordata->color.flags;

  if(toggle_flag == 'F'){
    XtSetArg(arg[0],XmNforeground,&(color.pixel));
    XtGetValues(change_widget,arg,1);
  }
  else if(toggle_flag == 'B'){
    XtSetArg(arg[0],XmNbackground,&(color.pixel));
    XtGetValues(change_widget,arg,1);
    XtSetArg(arg[0], XmNbackground, color.pixel);
    XtSetValues(XtParent(change_widget), arg, 1);
  }
  else if(toggle_flag == 'X'){
    XtSetArg(arg[0],XmNbottomShadowColor,&(color.pixel));
    XtGetValues(change_widget,arg,1);
  }
  else if(toggle_flag == 'T'){
    XtSetArg(arg[0],XmNtopShadowColor,&(color.pixel));
    XtGetValues(change_widget,arg,1);
  }
  else if(toggle_flag == 'A'){
    XtSetArg(arg[0],XmNarmColor,&(color.pixel));
    XtGetValues(change_widget,arg,1);
  }
  else if(toggle_flag == 'D'){
    default_colors();
    return;
  }
  else return;

  XStoreColor(dpy,cmap,&color);
}


/* this routine is called when the color list is created */
#define MAX_DEFINED_COLORS 500
static void load_rgb_text(Widget widget, XtPointer client_data, 
			  XtPointer call_data)
{
  Arg arg[2];
  FILE *rgb_text;
  int item_count;
  XmString xms[MAX_DEFINED_COLORS]; 
  char buf[360];

  if(!(rgb_text=fopen(data.rgb_text_file,"r"))) {
    return;
  }

  for(item_count=0; (item_count<MAX_DEFINED_COLORS) &&
                    fgets(buf,sizeof(buf),rgb_text); item_count++) {
      char *name;
      /* get correct (4th) field of rgb.txt lines */
      strtok(buf," \t\n");
      strtok(NULL," \t\n");
      strtok(NULL," \t\n");
      name = strtok(NULL,"\t\n");
	  while (isspace(*name) && name != NULL && *name != '\0')
		  name++;
      /* if you don't set the array item, counteract the autoincrement */
      if(name)
	xms[item_count] = XmStringDefCreate(name);
      else item_count--;
  }

  fclose(rgb_text);

  XtSetArg(arg[0],XmNlistItems,xms);
  XtSetArg(arg[1],XmNlistItemCount,item_count);
  XtSetValues(widget,arg,2);

  while(--item_count > 0)
      XmStringFree(xms[item_count]);

  widget = XmSelectionBoxGetChild(widget, XmDIALOG_LIST);
  XtAddCallback(widget,
		XmNsingleSelectionCallback,colorlist_selection_made,NULL);
  XtAddCallback(widget,
		XmNbrowseSelectionCallback,colorlist_selection_made,NULL);
  XtAddCallback(widget,
		XmNdefaultActionCallback,colorlist_selection_made,NULL);

}


static void default_colors()
{
     Arg args[5];
     Pixel background, foreground, top_shadow, bottom_shadow, arm_color;
     Pixel r_foreground, r_top_shadow, r_bottom_shadow, r_arm_color;
     Widget widget;
     int n = 0;
     XColor color;

     widget = XtNameToWidget(fetched,"color_change_BB.color_change");
     XtSetArg(args[n], XmNbackground, &background);		n++;
     XtGetValues(widget, args, n);

     XmGetColors(XtScreen(widget), cmap, background, &r_foreground,
			     &r_top_shadow, &r_bottom_shadow, &r_arm_color);

     XtSetArg(args[n], XmNforeground, &foreground); 		n++; 
     XtSetArg(args[n], XmNtopShadowColor, &top_shadow);		n++;
     XtSetArg(args[n], XmNbottomShadowColor, &bottom_shadow);	n++;
     XtSetArg(args[n], XmNarmColor, &arm_color);		n++;
     XtGetValues(widget, args, n);

     color.pixel = r_foreground;
     XQueryColor(dpy,cmap,&color);
     color.pixel = foreground;
     XStoreColor(dpy, cmap, &color);

     color.pixel = background;
     XQueryColor(dpy,cmap,&color);
     color.pixel = background;
     XStoreColor(dpy, cmap, &color);

     color.pixel = r_top_shadow;
     XQueryColor(dpy,cmap,&color);
     color.pixel = top_shadow;
     XStoreColor(dpy, cmap, &color);

     color.pixel = r_bottom_shadow;
     XQueryColor(dpy,cmap,&color);
     color.pixel = bottom_shadow;
     XStoreColor(dpy, cmap, &color);

     color.pixel = r_arm_color;
     XQueryColor(dpy,cmap,&color);
     color.pixel = arm_color;
     XStoreColor(dpy, cmap, &color);
}


/*
 *  Main program
 */
int main (int argc, char **argv)
{

    Arg arg[1] ;

    MrmInitialize ();

    /*
     *  Initialize the toolkit.  This call returns the id of a "shell,"
     *  whose only child should be the main window of the application.
     */

    CommonTestInit(argc, argv);

    XtSetArg (arg[0], XtNallowShellResize, TRUE) ;
    XtSetValues (Shell1, arg, 1) ;

    if(argc>1 && argv[1]){global_current_file=stringdup(argv[1]);}

    dpy = XtDisplay(Shell1);
    cmap = XDefaultColormap(dpy,0);

    XtGetApplicationResources(Shell1,
			      &data,
			      resources,
			      XtNumber(resources),
			      NULL,
			      0);

    /*
     *  Define the Mrm.hierarchy 
     */

    if (MrmOpenHierarchy (num_uid_files,	            /* number of files	    */
			uid_files, 	     	    /* files     	    */
			NULL,			    /* os_ext_list (null)   */
			&s_MrmHierarchy)	    /* ptr to returned id   */
			!= MrmSUCCESS) {
	printf ("Color: cannot open UID hierarchy.\n\
     Check UIDPATH environment variable\n\
     Goodbye.");
	return(0);
     }

    /*
     * 	Register callback routines 
     */

    if (MrmRegisterNames (callbackvec, callbacknum)
			!= MrmSUCCESS)
			    printf("can't register callback names\n");


    if (MrmFetchWidget (s_MrmHierarchy,
			"color_main",
			Shell1,
			&color_main,
			&class)
			!= MrmSUCCESS)
			    printf("can't fetch main window\n");


    /*
     *  Make the Shell1 widget "manage" the main window.
     *  It will not be "realized" until the Shell1 widget is "realized"
     */

    XtManageChild(color_main);
    
    /*
     *  Realize the Shell1 widget.  This will cause the entire "managed"
     *  widget hierarchy to be displayed.
     */

    XtRealizeWidget(Shell1);

  /* load non-UIL bitmaps, if internalPixmaps resource is true */

    create_colorform(XmNforeground, "white", 0);

    CommonPause();

    XtAppMainLoop(app_context);

    /* UNREACHABLE */
    return (0);
}

static void fetch(char *widget_index_string, Widget *widgetptr)
{
  MrmType class;
  *widgetptr = NULL;
  if(MrmFetchWidget(s_MrmHierarchy,
                widget_index_string,
                Shell1,
                widgetptr,
                &class)!=MrmSUCCESS)
                  printf("fail_cant_fetch\n");
}

static char *extract_first_xms_segment(XmString cs)
{
  XmStringContext context;
  XmStringCharSet charset;
  XmStringDirection direction;
  Boolean separator;
  char *primitive_string;
  XmStringInitContext(&context,cs);
  XmStringGetNextSegment(context,&primitive_string,
           &charset,&direction,&separator);
  XmStringFreeContext(context);
  return(primitive_string);
}


static void initialize_coords()
{
  MrmRegisterArg arg[2];
  static Position x,y;
  /* init_red/green/blue are identifiers in the uil module */
  arg[0].name = "stagger_x";
  arg[1].name = "stagger_y";
  if(!x)
    XtTranslateCoords(Shell1,50,50,&x,&y);
  else {
    x+=50;
    y+=50;
  }
 
  arg[0].value = (XtPointer)(long)(int)x;
  arg[1].value = (XtPointer)(long)(int)y;
  if (MrmRegisterNames (arg, 2)
                        != MrmSUCCESS)
                            printf("can't register identifier names\n");

}

static void exit_color(Widget widget, XtPointer client_data, 
		       XtPointer call_data)
{
	XFreeColors(dpy, cmap, pixels_return2, 5, plane_masks_return2[0]);
	printf("Exiting Color Program ....\n");
	exit(0);
}


/* return newly allocated duplicate copy of an asciiz string */
static char *stringdup(const char *s)
{
  char *dup;
  dup = (char *) XtCalloc(strlen(s)+1,sizeof(char));
  strcpy(dup,s);
  return(dup);
}
