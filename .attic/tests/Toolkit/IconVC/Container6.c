#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: Container6.c /main/2 1996/07/26 11:52:57 schuldy $"
#endif
#endif

#include <testlib.h>

#define	NAME_LEN	20

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

#define NUM_OBJ 	sizeof (BitmapPaths) / sizeof (BitmapPaths[0])

void
main(int argc, char **argv)
{
    Widget              Container, *IconGad;   
    int			i;
    const int		num_obj = NUM_OBJ;
    Cardinal		n;
    Arg			args[MAX_ARGS];
    char		ContainerName[NAME_LEN + 1];

    ContainerName[0] = '\0';

    CommonTestInit(argc, argv);

    n = 0;

    Container = XmCreateContainer(Shell1, ContainerName, args, n);
    XtManageChild(Container);

    XtRealizeWidget(Shell1);
    XtRealizeWidget(Container);

    /* Add a bunch of Icon Gadgets */

    IconGad = (Widget*) XtMalloc(num_obj * sizeof(Widget));

    for (i = 0; i <  num_obj; i++) {
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

	XtSetArg(args[n], XmNlabelString, icon_name); n++;
	XtSetArg(args[n], XmNlargeIconPixmap, pixmap); n++;

	IconGad[i] = XmCreateIconGadget(Container, IconName, args, n);
        XtRealizeWidget(IconGad[i]);
        XtManageChild(IconGad[i]);
	XmStringFree(icon_name);
    }

    CommonPause();	/* Warning */
    CommonPause();	/* Assertion 1 */
    CommonPause();	/* Assertion 2 */

    XtAppMainLoop(app_context);

    for (i = 0; i <  num_obj; i++) {
       XtDestroyWidget( IconGad[i] );
    }
    XtFree( (char *)IconGad );

    XtDestroyWidget( Container );

    return;
}
