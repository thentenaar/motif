/* $XConsortium: Display.c /main/2 1996/07/31 16:31:21 schuldy $ */

/* Test created to Check XmNenableThinThickness
*/

#include <testlib.h>

  void
PrintValues( Widget w, char *label )
{
  Arg     args[4];
  Dimension shadowThickness;
  Dimension highlightThickness;
  Dimension detailShadowThickness;
  Dimension sashShadowThickness;

  (void)printf( "For %s:\n", label );

  if ( strcmp(label,"CascadeButton") == 0
    || strcmp(label,"CascadeButtonGadget") == 0
    || strcmp(label,"DrawnButton") == 0
    || strcmp(label,"PushButton") == 0
    || strcmp(label,"PushButtonGadget") == 0
    || strcmp(label,"Text") == 0
    || strcmp(label,"List") == 0
    || strcmp(label,"Scale") == 0 )
  {
    XtSetArg( args[0], XmNshadowThickness, &shadowThickness );
    XtSetArg( args[1], XmNhighlightThickness, &highlightThickness );
    XtGetValues( w, args, 2 );
    (void)printf( "\tShadow thickness value is \"%d\"\n",
		  shadowThickness );
    (void)printf( "\tHighlight thickness value is \"%d\"\n",
		  highlightThickness );
    return;
  }
  else if ( strcmp(label,"PanedWindow") == 0 )
  {
    XtSetArg( args[0], XmNsashShadowThickness, &sashShadowThickness );
    XtGetValues( w, args, 1 );
    (void)printf( "\tSash Shadow thickness value is \"%d\"\n",
		  sashShadowThickness );
    return;
  }
  else if ( strcmp(label,"ScrollBar") == 0 
    || strcmp(label,"SelectionBox") == 0 )
  {
    XtSetArg( args[0], XmNshadowThickness, &shadowThickness );
    XtGetValues( w, args, 1 );
    (void)printf( "\tShadow thickness value is \"%d\"\n",
		  shadowThickness );
    return;
  }
  else if ( strcmp(label,"ArrowButton") == 0
    || strcmp(label,"ArrowButtonGadget") == 0 )
  {
    XtSetArg( args[0], XmNdetailShadowThickness, &detailShadowThickness );
    XtSetArg( args[1], XmNshadowThickness, &shadowThickness );
    XtSetArg( args[2], XmNhighlightThickness, &highlightThickness );
    XtGetValues( w, args, 3 );
    (void)printf( "\tDetail shadow thickness value is \"%d\"\n",
		  detailShadowThickness );
    (void)printf( "\tShadow thickness value is \"%d\"\n",
		  shadowThickness );
    (void)printf( "\tHighlight thickness value is \"%d\"\n",
		  highlightThickness );
    return;
  }
  else if ( strcmp(label,"ToggleButton") == 0
    || strcmp(label,"ToggleButtonGadget" ) == 0 )
  {
    XtSetArg( args[0], XmNdetailShadowThickness, &detailShadowThickness );
    XtSetArg( args[1], XmNhighlightThickness, &highlightThickness );
    XtGetValues( w, args, 2 );
    (void)printf( "\tDetail shadow thickness value is \"%d\"\n",
		  detailShadowThickness );
    (void)printf( "\tHighlight thickness value is \"%d\"\n",
		  highlightThickness );
    return;
  }
  else
  {
    (void)fprintf( stderr, "Coding Error in test: Unknown type %s\n",
		label );
    exit(1);
  }

  /*NOTREACHED*/
  return;
}

#define DoWidget( widname ) \
  { \
    Widget widgetname; \
    widgetname = XmCreate ## widname ( Shell1, #widname, NULL, 0 ); \
    PrintValues( widgetname, #widname ); \
    CommonPause (); \
    XtDestroyWidget( widgetname ); \
  }

#define DoGadget( gadname, parenttype ) \
  { \
    Widget gadgetname; \
    Widget manager;    \
    manager = XmCreate ## parenttype( Shell1, "GadgetManager", NULL, 0 ); \
    gadgetname = XmCreate ## gadname ( manager, #gadname, NULL, 0 ); \
    XtManageChild( gadgetname ); \
    PrintValues( gadgetname, #gadname ); \
    CommonPause(); \
    XtDestroyWidget( gadgetname ); \
    XtDestroyWidget( manager ); \
  }

  int
main (int argc, char **argv)
{
  CommonTestInit (argc,argv);

  XtRealizeWidget (Shell1);

  DoWidget(List);
  DoWidget(CascadeButton);
  DoWidget(Text);
  DoWidget(PanedWindow);
  DoWidget(Scale);
  DoWidget(ScrollBar);
  DoWidget(SelectionBox);
  DoWidget(ArrowButton);
  DoWidget(ToggleButton);
  DoWidget(DrawnButton);
  DoWidget(PushButton);

  DoGadget(CascadeButtonGadget, MenuBar);
  DoGadget(PushButtonGadget, RowColumn);
  DoGadget(ArrowButtonGadget, RowColumn);
  DoGadget(ToggleButtonGadget, RowColumn);

  CommonPause();	/* Just to clean up */

  XtAppMainLoop (app_context);

  /* Cleanup to simplify memory testing usage */
  XtDestroyWidget( Shell1 );

  return 0;
}
