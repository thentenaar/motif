#include <testlib.h>

static char *Strings = { 
	"We few, we happy few, we band of brothers;\n"
	"For he to-day that sheds his blood with me\n"
	"Shall be my brother; be he ne'er so vile,\n"
	"This day shall gentle his condition:\n"
	"And gentlemen in England now a-bed\n"
	"Shall think themselves accursed they were not here,\n"
	"And hold their manhoods cheap whiles any speaks\n"
	"That fought with us upon Saint Crispin's day.\n"
	"\tHenry V, Act IV Scene 3, William Shakespeare." };

#define WINDOW_PIXMAP "window.bmp"
#define TEXT_PIXMAP   "text.bmp"

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
  Widget   Text;
  Widget   Window;
  Pixmap   WindPixmap;
  Pixmap   TextPixmap;

  CommonTestInit(argc, argv);

  /* Get the pixmaps for the backgrounds */
  WindPixmap = XmGetPixmap(screen, WINDOW_PIXMAP, BlackPixelOfScreen(screen),
		WhitePixelOfScreen(screen));
  TextPixmap = XmGetPixmap(screen, TEXT_PIXMAP, BlackPixelOfScreen(screen),
		WhitePixelOfScreen(screen));

  if (!WindPixmap)
  {
    (void)printf("Can't make pixmap for file %s!\n", WINDOW_PIXMAP );
    exit(1);
  }

  if (!TextPixmap)
  {
    (void)printf("Can't make pixmap for file %s!\n", TEXT_PIXMAP );
    exit(1);
  }

  /* Make the ScrolledWindow and Text */
  n = 0;
  XtSetArg( Args[n], XmNvalue, Strings ); n++;
  XtSetArg( Args[n], XmNeditable, False ); n++;
  XtSetArg( Args[n], XmNeditMode, XmMULTI_LINE_EDIT ); n++;
  Text = XmCreateScrolledText(Shell1, "Text", Args, n );

  /* Get the Widget for the parent Scrolled Window */
  Window = XtParent(Text);

  /* Set the Text Pixmap and color */
  n = 0;
  XtSetArg(Args[n], XmNbackgroundPixmap, TextPixmap ); n++;
  XtSetValues( Text, Args, n );
  SetWidgetColor( Text, "white" );

  /* Set the Window Pixmap and color */
  n = 0;
  XtSetArg(Args[n], XmNbackgroundPixmap, WindPixmap ); n++;
  XtSetValues( Window, Args, n );
  SetWidgetColor( Window, "pink" );

  XtManageChild(Text);
  XtManageChild(Window);


  /*************
   * Assertion 1.  Now that the text and window have differing pixmaps,
   * lets dump the values for the child scrollbar values.
   */
  PrintValues( Window, "Assertion 1" );
  CommonPause();


  /*************
   * Assertion 2.  Now that the text and window have differing pixmaps,
   * lets dump the values for the child scrollbar values.
   */
  SetWidgetColor( Text, "green" );
  PrintValues( Window, "Assertion 2" );
  CommonPause();
  
  /* Cleanup Time */
  XmDestroyPixmap( screen, WindPixmap );
  XmDestroyPixmap( screen, TextPixmap );
  XtDestroyWidget( XtParent(Text) );

  CommonPause();

  XtAppMainLoop(app_context);
  return 0;
}
