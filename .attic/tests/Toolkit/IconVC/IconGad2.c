#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: IconGad2.c /main/1 1996/03/27 10:02:31 schuldy $"
#endif
#endif

#include <testlib.h>
#include <Xm/XmAll.h>

/* do this the hard way */
/* #define	DBG	printf("%d\n",cp++); */
#define	DBG

static int cp;

  int
main(int argc, char *argv[] )
{
  int n;
  XmDirection xmlayoutdir;

  Arg args[MAX_ARGS];
  Widget icon, mainWindow;
  Pixmap pixmap;
  XmString  str;

  CommonTestInit(argc, argv);

DBG

  pixmap = XmGetPixmap(screen, "IconGad.bmp", BlackPixelOfScreen(screen),
			WhitePixelOfScreen(screen) );

DBG

  if ( !pixmap ) {
    fprintf( stderr, "Couldn't fetch the damn pixmap\n" );
    exit(1);
  }

  mainWindow = XmCreateMainWindow(Shell1, "mainWindow", NULL, 0);
  if (!mainWindow)
  {
    fprintf(stderr,  "Can't create mainWindow\n");
    exit (3);
  }

  XtManageChild(mainWindow);

DBG

  /* Set the arguments for the icon widget, create and manage it */

  str = XmStringCreateSimple("IconLabel");

  n = 0;
  XtSetArg(args[n], XmNlabelString, str); n++;
  XtSetArg(args[n], XmNlargeIconPixmap,  pixmap);  n++;

DBG

  icon = XmCreateIconGadget(mainWindow, "IconGad2", args, n);
  if ( !icon ) {
    fprintf( stderr, "Couldn't create the damn icon\n" );
    exit(2);
  }

DBG

  XmStringFree(str);

  XtManageChild(icon);

DBG

  XtRealizeWidget(icon);

  XtSetArg(args[0], XmNlayoutDirection, &xmlayoutdir);
  XtGetValues(icon, args, 1);

  if (XmDirectionMatch(XmRIGHT_TO_LEFT, xmlayoutdir) == True)
    printf("R to L\n");

  if (XmDirectionMatch(XmLEFT_TO_RIGHT, xmlayoutdir) == True)
    printf("L to R\n");

DBG

  sleep (5);

DBG
 
  CommonPause();	/* TP 1 */

  CommonPause(); 	/* TP 2 */


/* begin TP 3 */

  XtSetArg(args[0], XmNalignment, XmALIGNMENT_BEGINNING);

  XtSetValues(icon, args, 1);

  CommonPause();	/* TP 3 */


/* begin TP 4 */

  XtSetArg(args[0], XmNalignment, XmALIGNMENT_END);

  XtSetValues(icon, args, 1);

  CommonPause();	/* TP 4 */


/* begin TP 5 */

  XtSetArg(args[0], XmNspacing, 20);

  XtSetValues(icon, args, 1);

  CommonPause();	/* TP 5 */


/* begin TP 6 */

  XtSetArg(args[0], XmNmarginHeight, 10);

  XtSetValues(icon, args, 1);

  CommonPause();	/* TP 6 */


/* begin TP 7 */

  XtSetArg(args[0], XmNmarginWidth, 0);

  XtSetValues(icon, args, 1);

  CommonPause();	/* TP 7 */


			/* "Exit" test here */
DBG

  XtAppMainLoop(app_context);

DBG

  /*
   * Clean up to test more easily for memory leaks
   */

  XmDestroyPixmap(screen, pixmap);
  XtDestroyWidget(icon);

DBG

  return(0);
}
