/* $XConsortium: Container7a.c /main/2 1996/05/08 23:34:11 drk $ */

/* Motif 2.0 QATS IconVC Container7 Automated Test */

#include <testlib.h>
#include <Xm/XmAll.h>

int
main(int argc, char *argv[] )
{
  int n;
  Arg args[MAX_ARGS];
  Widget icon, scrolledWindow, container;
  Pixmap pixmap;
  XmString  str;

  CommonTestInit(argc, argv);

  scrolledWindow = XmCreateScrolledWindow(Shell1, "ScrollC7", args, n);
  XtManageChild(scrolledWindow);

  /* setup Container options */

  n = 0;
  XtSetArg(args[n], XmNselectionPolicy, XmMULTIPLE_SELECT); n++;


  /* create Container */

  container = XmCreateContainer(scrolledWindow, "ContainerC7",
    args, n);
  XtManageChild(container);
  
  XtAppMainLoop(app_context);


/*
  XmDestroyContainer(container);
  XmDestroyScrollWindow(scrolledWindow);
*/

  CommonPause();

  return 0;
}
