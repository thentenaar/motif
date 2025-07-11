#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: Container7.c /main/2 1996/07/26 11:53:06 schuldy $"
#endif
#endif

#include <testlib.h>

char * BitmapPaths[] =
{
  "../bitmaps/circles1.bmp",
  "../bitmaps/collapsedState.bmp",
  "../bitmaps/dp.bmp",
  "../bitmaps/expandedState.bmp",
  "../bitmaps/gnu.bmp",
  "../bitmaps/mtn.bmp",
  "../bitmaps/null.bmp",
  "../bitmaps/smiley.bmp",
  "../bitmaps/warn.bmp"
};

#define NUM_PIXMAP 	sizeof (BitmapPaths) / sizeof (BitmapPaths[0])
#define NUM_LABEL	NUM_PIXMAP

/* Some jiggery pokery to make the icons surround a central point */
#define NUM_ELEMS	(NUM_PIXMAP + NUM_LABEL)

#define START_X		50
#define START_Y		50
#define INCR_X		100
#define INCR_Y		100
#define ELEM_IN_ROW	3

#define CURR_COL(n)	((int)(((n))%ELEM_IN_ROW))
#define CURR_ROW(n)	((int)(((n))/ELEM_IN_ROW))

#define PIXEL_X(n)	(START_X + INCR_X * CURR_COL(n))
#define PIXEL_Y(n)	(START_Y + INCR_Y * CURR_ROW(n))

  void
RestoreXY( Widget w, int x, int y )
{
    int n;
    Arg args[MAX_ARGS];

    n = 0;
    XtSetArg( args[n], XmNwidth, x ); n++;
    XtSetArg( args[n], XmNheight, y ); n++;
    XtSetValues( w, args, n );

    return;
}

  void
UnselectAll( Widget w )
{
    int n;
    Arg args[MAX_ARGS];

    n = 0;
    XtSetArg( args[n], XmNselectedObjects, NULL ); n++;
    XtSetArg( args[n], XmNselectedObjectCount, 0 ); n++;
    XtSetValues( w, args, n );

    return;
}
 

  int
main(int argc, char **argv)
{
    Widget              ScrollWin, Container, *IconGad;   
    int			i;
    Cardinal		n;
    Arg			args[MAX_ARGS];
    Dimension		x_size, y_size;

    CommonTestInit(argc, argv);

    n = 0;
    XtSetArg( args[n], XmNx, 20 ); n++;
    ScrollWin = XmCreateScrolledWindow(Shell1, "ScrollWin7", args, n);
    XtManageChild(ScrollWin);

    XtSetArg(args[n], XmNselectionPolicy, XmMULTIPLE_SELECT); n++;
    XtSetArg(args[n], XmNspatialStyle, XmNONE ); n++;
    Container = XmCreateContainer(ScrollWin, "Container7", args, n);
    XtManageChild(Container);

    /* Add a bunch of Icon Gadgets */

    IconGad = (Widget*) XtMalloc(NUM_ELEMS * sizeof(Widget));

    /* Put some pixmaps in the Container */
    for (i = 0; i <  NUM_PIXMAP; i++) {
	char        IconName[20] ;
	XmString   icon_name;
	Pixmap	pixmap;

	n = 0 ;
	sprintf(IconName, "IconGad%d", i);
	icon_name = XmStringGenerate(IconName, NULL, XmCHARSET_TEXT, NULL); 

   	/* make icons from pixmap files */

	pixmap = XmGetPixmap(screen, BitmapPaths[i],
	  BlackPixelOfScreen(screen),
	  WhitePixelOfScreen(screen));

	if (!pixmap)
	{
	  printf("Can't make pixmap for file %s!\n",
	    BitmapPaths[i]);
	  exit(1);
	}

	XtSetArg(args[n], XmNx, PIXEL_X(i) ); n++;
	XtSetArg(args[n], XmNy, PIXEL_Y(i) ); n++;
	XtSetArg(args[n], XmNlabelString, icon_name); n++;
	XtSetArg(args[n], XmNlargeIconPixmap, pixmap); n++;

	IconGad[i] = XmCreateIconGadget(Container, IconName, args, n);
        XtManageChild(IconGad[i]);
	XmStringFree(icon_name);
    }

    /* Put some labels in the Container */
    for (i = NUM_PIXMAP; i <  NUM_ELEMS; i++) {
	char        LabelName[20] ;
	XmString   label_name;

	n = 0 ;
	sprintf(LabelName, "LabelName%d", i);
	label_name = XmStringGenerate(LabelName, NULL, XmCHARSET_TEXT, NULL); 

	XtSetArg(args[n], XmNx, PIXEL_X(i) ); n++;
	XtSetArg(args[n], XmNy, PIXEL_Y(i) ); n++;
	XtSetArg(args[n], XmNlabelString, label_name); n++;
	XtSetArg(args[n], XmNlabelType, XmSTRING); n++;

	IconGad[i] =
		XmCreateIconGadget(Container, LabelName, args, n);
        XtManageChild(IconGad[i]);
	XmStringFree(label_name);
    }

    XtRealizeWidget(Shell1);

    /* Find out the default size for X and Y */
    n = 0;
    XtSetArg( args[n], XmNwidth, &x_size ); n++;
    XtSetArg( args[n], XmNheight, &y_size ); n++;
    XtGetValues( ScrollWin, args, n );

    /*************************
     * Assertions begin
     */

    /* Assertions 1 and 2 */
    n = 0;
    XtSetArg( args[n], XmNwidth, 175 ); n++;
    XtSetValues( ScrollWin, args, n );
    CommonPause();
    RestoreXY( ScrollWin, x_size, y_size );
    UnselectAll( Container );

    /* Assertions 3 and 4 */
    n = 0;
    XtSetArg( args[n], XmNwidth, 400 ); n++;
    XtSetValues( ScrollWin, args, n );
    CommonPause();
    RestoreXY( ScrollWin, x_size, y_size );
    UnselectAll( Container );

    /* Assertions 5 and 6 */
    n = 0;
    XtSetArg( args[n], XmNheight, 400 ); n++;
    XtSetValues( ScrollWin, args, n );
    CommonPause();
    RestoreXY( ScrollWin, x_size, y_size );
    UnselectAll( Container );

    /* Assertions 7 and 8 */
    CommonPause();

    XtAppMainLoop(app_context);


    for ( i=0; i < NUM_ELEMS; i++ )
      XtDestroyWidget( IconGad[i] );

    XtDestroyWidget( Container );
    XtDestroyWidget( ScrollWin );
    XtFree( (char *)IconGad );

}
