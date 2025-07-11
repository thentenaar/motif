#include <testlib.h>

#define WINDOW_PIXMAP_1 "window1.bmp"
#define WINDOW_PIXMAP_2 "window2.bmp"
#define WINDOW_PIXMAP_3 "window3.bmp"
#define LARGE_PIXMAP	"large.bmp"

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
  Widget HorizontalSB;
  Pixel  SBbackground, SWbackground;
  Pixel  SBbottomShadowColor, SWbottomShadowColor;
  Pixel  SBtopShadowColor, SWtopShadowColor;
  Pixmap SBbackgroundPixmap, SWbackgroundPixmap;
  Pixmap SBbottomShadowPixmap, SWbottomShadowPixmap;
  Pixmap SBtopShadowPixmap, SWtopShadowPixmap;

  (void)printf( "%s\n", label );

  VerticalSB = XtNameToWidget( window, "VertScrollBar" );
  HorizontalSB = XtNameToWidget( window, "VertScrollBar" );

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
  (void)printf( "\tVertical ScrollBar " #desc " is %s\n", \
		(SW ## desc == SB ## desc) ? "identical" : "different" )

  CompareValue(background);
  CompareValue(bottomShadowColor);
  CompareValue(topShadowColor);
  CompareValue(backgroundPixmap);
  CompareValue(bottomShadowPixmap);
  CompareValue(topShadowPixmap);

#undef CompareValue

  n = 0;
  XtSetArg( Args[n], XmNbackground, &SBbackground );                 n++;
  XtSetArg( Args[n], XmNbottomShadowColor, &SBbottomShadowColor );   n++;
  XtSetArg( Args[n], XmNtopShadowColor, &SBtopShadowColor );         n++;
  XtSetArg( Args[n], XmNbackgroundPixmap, &SBbackgroundPixmap );     n++;
  XtSetArg( Args[n], XmNbottomShadowPixmap, &SBbottomShadowPixmap ); n++;
  XtSetArg( Args[n], XmNtopShadowPixmap, &SBtopShadowPixmap );       n++;
  XtGetValues( HorizontalSB, Args, n );
  (void)printf( "\n" );

#define CompareValue(desc) \
  (void)printf( "\tHorizontal ScrollBar " #desc " is %s\n", \
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
  Widget   Window;
  Widget   Label;
  Widget   VertScrollBar;
  Widget   HorScrollBar;
  Pixmap   WindPixmap1;
  Pixmap   WindPixmap2;
  Pixmap   WindPixmap3;
  Pixmap   Large;

  CommonTestInit(argc, argv);

  /* Get the pixmaps for the backgrounds */
  WindPixmap1 = XmGetPixmap(screen, WINDOW_PIXMAP_1,
		BlackPixelOfScreen(screen),
		WhitePixelOfScreen(screen));
  WindPixmap2 = XmGetPixmap(screen, WINDOW_PIXMAP_2,
		BlackPixelOfScreen(screen),
		WhitePixelOfScreen(screen));
  WindPixmap3 = XmGetPixmap(screen, WINDOW_PIXMAP_3,
		BlackPixelOfScreen(screen),
		WhitePixelOfScreen(screen));
  Large = XmGetPixmap(screen, LARGE_PIXMAP,
		BlackPixelOfScreen(screen),
		WhitePixelOfScreen(screen));

  if (!WindPixmap1)
  {
    (void)printf("Can't make pixmap for file %s!\n", WINDOW_PIXMAP_1 );
    exit(1);
  }

  if (!WindPixmap2)
  {
    (void)printf("Can't make pixmap for file %s!\n", WINDOW_PIXMAP_2 );
    exit(1);
  }

  if (!WindPixmap3)
  {
    (void)printf("Can't make pixmap for file %s!\n", WINDOW_PIXMAP_3 );
    exit(1);
  }

  if (!Large)
  {
    (void)printf("Can't make pixmap for file %s!\n", LARGE_PIXMAP );
    exit(1);
  }

  /* Make the ScrolledWindow */
  n = 0;
  XtSetArg(Args[n], XmNbackgroundPixmap, WindPixmap1 ); n++;
  XtSetArg(Args[n], XmNscrollingPolicy, XmAUTOMATIC ); n++;
  XtSetArg(Args[n], XmNscrollBarDisplayPolicy, XmSTATIC ); n++;
  XtSetArg(Args[n], XmNvisualPolicy, XmCONSTANT ); n++;
  Window = XmCreateScrolledWindow(Shell1, "Window", Args, n );

  n = 0;
  XtSetArg(Args[n], XmNlabelPixmap, Large ); n++;
  XtSetArg(Args[n], XmNlabelType, XmPIXMAP ); n++;
  Label = XmCreateLabel( Window, "Label", Args, n );

  SetWidgetColor( Window, "tomato" );

  XtManageChild(Window);


  /*************
   * Assertion 1.  Now that the window has a pixmap,
   * lets dump the values for the child scrollbar values.
   */
  PrintValues( Window, "Assertion 1" );
  CommonPause();


  /*************
   * Assertion 2.  Change the window values, check the children get them.
   * dump the values for the child scrollbar values.
   */
  SetWidgetColor( Window, "blue" );
  n = 0;
  XtSetArg(Args[n], XmNbackgroundPixmap, WindPixmap2 ); n++;
  XtSetValues( Window, Args, n );
  PrintValues( Window, "Assertion 2" );
  CommonPause();
  
  
  /*************
   * Assertion 3.  Change the childrens values, check they differ from
   * the windows.
   * dump the values for the child scrollbar values.
   */
  VertScrollBar = XtNameToWidget( Window, "VertScrollBar" );
  HorScrollBar = XtNameToWidget( Window, "HorScrollBar" );
  SetWidgetColor( VertScrollBar, "green" );
  SetWidgetColor( HorScrollBar, "green" );
  XtSetArg(Args[n], XmNbackgroundPixmap, WindPixmap3 ); n++;
  XtSetValues( VertScrollBar, Args, n );
  XtSetValues( HorScrollBar, Args, n );
  PrintValues( Window, "Assertion 3" );
  CommonPause();


  /* Exit the program */
  CommonPause();
  XtAppMainLoop(app_context);

  /* Cleanup Time */
  XmDestroyPixmap( screen, WindPixmap1 );
  XmDestroyPixmap( screen, WindPixmap2 );
  XmDestroyPixmap( screen, WindPixmap3 );
  XtDestroyWidget( Window );

  return 0;
}
