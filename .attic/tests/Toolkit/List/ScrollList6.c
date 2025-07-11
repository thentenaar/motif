#include <testlib.h>

static char *Strings[] = { 
	"We few, we happy few, we band of brothers; ",
	"For he to-day that sheds his blood with me ",
	"Shall be my brother; be he ne'er so vile, ",
	"This day shall gentle his condition: ",
	"And gentlemen in England now a-bed ",
	"Shall think themselves accursed they were not here, ",
	"And hold their manhoods cheap whiles any speaks ",
	"That fought with us upon Saint Crispin's day. ",
	"\tHenry V, Act IV Scene 3, William Shakespeare." };

#define NUM_STRINGS (sizeof(Strings)/sizeof(char *))

#define WINDOW_PIXMAP "window.bmp"
#define LIST_PIXMAP   "list.bmp"

  static void
SetWidgetColor( Widget w, char *color )
{
  Display  *display;
  Colormap colormap;
  XColor   newcolor;
  XColor   unused;
  Arg      Args[MAX_ARGS];
  int      n;

  display = XtDisplay(w);
  colormap = DefaultColormapOfScreen(XtScreen(w));

  XAllocNamedColor( display, colormap, color, &newcolor, &unused );

  n = 0;
  XtSetArg(Args[n], XmNbackground, newcolor.pixel ); n++;
  XtSetValues( w, Args, n );

  return;
}


  static void
PrintValues( Widget window, char *label )
{
  int n;
  Arg Args[MAX_ARGS];
  Widget VerticalSB;
  Pixel SBbackground, SWbackground;
  Pixel SBbottomShadowColor, SWbottomShadowColor;
  Pixel SBtopShadowColor, SWtopShadowColor;
  Pixmap SBbackgroundPixmap, SWbackgroundPixmap;
  Pixmap SBbottomShadowPixmap, SWbottomShadowPixmap;
  Pixmap SBtopShadowPixmap, SWtopShadowPixmap;

  (void)printf( "%s\n", label );

  VerticalSB = XtNameToWidget( window, "VertScrollBar" );

  n = 0;
  XtSetArg( Args[n], XmNbackground, &SBbackground );                 n++;
  XtSetArg( Args[n], XmNbottomShadowColor, &SBbottomShadowColor );   n++;
  XtSetArg( Args[n], XmNtopShadowColor, &SBtopShadowColor );         n++;
  XtSetArg( Args[n], XmNbackgroundPixmap, &SBbackgroundPixmap );     n++;
  XtSetArg( Args[n], XmNbottomShadowPixmap, &SBbottomShadowPixmap ); n++;
  XtSetArg( Args[n], XmNtopShadowPixmap, &SBtopShadowPixmap );       n++;
  XtGetValues( VerticalSB, Args, n );

  n = 0;
  XtSetArg( Args[n], XmNbackground, &SWbackground );                 n++;
  XtSetArg( Args[n], XmNbottomShadowColor, &SWbottomShadowColor );   n++;
  XtSetArg( Args[n], XmNtopShadowColor, &SWtopShadowColor );         n++;
  XtSetArg( Args[n], XmNbackgroundPixmap, &SWbackgroundPixmap );     n++;
  XtSetArg( Args[n], XmNbottomShadowPixmap, &SWbottomShadowPixmap ); n++;
  XtSetArg( Args[n], XmNtopShadowPixmap, &SWtopShadowPixmap );       n++;
  XtGetValues( window, Args, n );

#define CompareValue(desc) \
  (void)printf( "\tScrollBar " #desc " is %s\n", \
		(SW ## desc == SB ## desc) ? "identical" : "different" )

  CompareValue(background);
  CompareValue(bottomShadowColor);
  CompareValue(topShadowColor);
  CompareValue(backgroundPixmap);
  CompareValue(bottomShadowPixmap);
  CompareValue(topShadowPixmap);

#undef CompareValue
  (void)printf( "\n" );

  return;
}
  


  int
main( int argc, char *argv[] )
{
  int      n;
  Arg      Args[MAX_ARGS];
  int      i;
  XmString Items[NUM_STRINGS];
  Widget   List;
  Widget   Window;
  Pixmap   WindPixmap;
  Pixmap   ListPixmap;

  CommonTestInit(argc, argv);

  /* Turn text strings into Motif strings */
  for ( i=0; i<NUM_STRINGS; i++ )
  {
    Items[i] = XmStringLtoRCreate(Strings[i], XmSTRING_DEFAULT_CHARSET );
  }

  /* Get the pixmaps for the backgrounds */
  WindPixmap = XmGetPixmap(screen, WINDOW_PIXMAP, BlackPixelOfScreen(screen),
		WhitePixelOfScreen(screen));
  ListPixmap = XmGetPixmap(screen, LIST_PIXMAP, BlackPixelOfScreen(screen),
		WhitePixelOfScreen(screen));

  if (!WindPixmap)
  {
    (void)printf("Can't make pixmap for file %s!\n", WINDOW_PIXMAP );
    exit(1);
  }

  if (!ListPixmap)
  {
    (void)printf("Can't make pixmap for file %s!\n", LIST_PIXMAP );
    exit(1);
  }

  /* Make the ScrolledWindow and List */
  n = 0;
  XtSetArg(Args[n], XmNitems, Items); n++;
  XtSetArg(Args[n], XmNitemCount, NUM_STRINGS); n++;
  List = XmCreateScrolledList(Shell1, "List", Args, n );
  XtManageChild(List);

  /* Get the Widget for the parent Scrolled Window */
  Window = XtParent(List);

  /* Set the List Pixmap and color */
  n = 0;
  XtSetArg(Args[n], XmNbackgroundPixmap, ListPixmap ); n++;
  XtSetValues( List, Args, n );
  SetWidgetColor( List, "gold" );

  /* Set the Window Pixmap and color */
  n = 0;
  XtSetArg(Args[n], XmNbackgroundPixmap, WindPixmap ); n++;
  XtSetValues( Window, Args, n );
  SetWidgetColor( List, "dark blue" );

  for ( i=0; i<NUM_STRINGS; i++ )
  {
    XmStringFree(Items[i]);
  }


  /*************
   * Assertion 1.  Now that the list and window have differing pixmaps,
   * lets dump the values for the child scrollbar values.
   */
  PrintValues( Window, "Assertion 1" );
  CommonPause();

  /*************
   * Assertion 2.  Now that the list and window have differing pixmaps,
   * lets dump the values for the child scrollbar values.
   */
  SetWidgetColor( List, "green" );
  PrintValues( Window, "Assertion 2" );
  CommonPause();
  
  
  CommonPause();
  XtAppMainLoop(app_context);

  /* Cleanup Time */
  XmDestroyPixmap( screen, WindPixmap );
  XmDestroyPixmap( screen, ListPixmap );
  XtDestroyWidget( List );

  return 0;
}
