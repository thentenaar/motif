/* $XConsortium: XmeString.c /main/5 1996/06/11 17:25:46 pascale $ */
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
static char rcsid[] = ""
#endif
#endif

#include <testlib.h>
#include <Xm/Text.h>
#include <Xm/XmosP.h>
#include <Xm/XmP.h>

/* NOTE: including SpecRender.h may not be necessary in the future */
#include <Xm/SpecRenderT.h>

void  main (argc, argv)
int argc;
char **argv;
{
  Boolean            trace = False;
  register int       n;
  Arg                args[MAX_ARGS];
  XmString           target, xme_str, tmp_str;
  Widget             label, bboard;
  Display            *display;
  XmRendition        Renditions[3];
  XmStringTag        RenditionTags[3];
  Colormap           cmap;
  XColor             color, unused;
  Pixel              pixel_color;
  XmRenderTable      RenderTable=(XmRenderTable)NULL, 
                     render_table_value=(XmRenderTable)NULL;
  XFontStruct        * fs = (XFontStruct *) NULL;
  Font               default_font;
  XmParseMapping     ParseMapping;
  XmParseTable       ParseTable;
  char               *text=(char *) NULL;
  XmStringCharSet    tag;
  XmStringContext    StrContext;
  unsigned int       length;
  XtPointer          value;

  CommonTestInit(argc, argv);

  n = 0;
  XtSetArg(args[n], XmNwidth,  200);  n++;
  XtSetArg(args[n], XmNheight, 200);  n++;
  XtSetValues(Shell1, args, n);

  XtRealizeWidget(Shell1);
  display = XtDisplay(Shell1);

  /* Load a font in the server */
  default_font = XLoadFont(display, "8x13");
  if (default_font == BadAlloc)
    printf("ERROR: XLoadFont failed: BadAlloc\n");
  else if (default_font == BadName)
    printf("ERROR: XLoadFont failed: BadName\n");

  /* Create a rendition for this test */
  XtVaGetValues(Shell1, 
		XmNcolormap, &cmap,
		NULL);

  if (XAllocNamedColor(display, cmap, "blue", &color, &unused))
    pixel_color = color.pixel;
  else
    pixel_color = XmUNSPECIFIED_PIXEL;

  n = 0;
  XtSetArg(args[n], XmNrenditionForeground, pixel_color);  n++;
  XtSetArg(args[n], XmNfontName, "8x13");  n++;
  XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT);  n++;
  XtSetArg(args[n], XmNunderlineType, XmSINGLE_LINE);  n++;
  RenditionTags[0] = (XmStringTag)"Rendition1";
  Renditions[0] = XmRenditionCreate(Shell1, RenditionTags[0], args, n);

  /* initialize the RenderTable */
  RenderTable = XmRenderTableAddRenditions(RenderTable, Renditions,
					   1, XmMERGE_REPLACE);
  if (RenderTable == NULL)
    printf("ERROR: in adding rendition to RenderTable\n");

  /* Check that the default font for the RenderTable is correct */
/*
 *** commented out because this test case core dumps ***
  if (!XmeRenderTableGetDefaultFont(RenderTable, &fs))
    printf("ERROR: XmeRenderTableGetDefaultFont call has failed\n");
  if (fs->fid != default_font)
    printf("ERROR: default font in render table is incorrect\n");
  CommonPause();
*/

  n = 0;
  XtSetArg(args[n], XmNlabelRenderTable, RenderTable);  n++;
  bboard = XmCreateBulletinBoard(Shell1, "Bboard", args, n);
  XtManageChild(bboard);

  /* get the default render table for the bboard widget */
/* 
 *** commented out because this test case core dumps ***
printf("call XmeGetDefaultRenderTable\n");
  render_table_value = XmeGetDefaultRenderTable(label, XmLABEL_RENDER_TABLE);
  if (render_table_value != RenderTable)
    printf("ERROR: render table stored value is incorrect\n");
  CommonPause();
*/

  /* Create and manage label widget */
  n = 0;
  label = XmCreateLabel(bboard, "Label", args, n);
  XtManageChild(label);

  target = XmStringCreateLocalized("Test String");
  xme_str = XmeGetLocalizedString((char *)NULL, label, 
				  XmNlabelString, "Test String");

  if (!XmStringCompare(target, xme_str)) 
    printf("ERROR: creating localized string\n");

  n = 0;
  XtSetArg(args[n], XmNlabelString, xme_str);  n++;
  XtSetValues(label, args, n);
  CommonPause();

  tmp_str = XmStringSegmentCreate("Reverse", XmFONTLIST_DEFAULT_TAG,
				  XmSTRING_DIRECTION_R_TO_L, False);
  n = 0;
  XtSetArg(args[n], XmNlabelString, tmp_str);  n++;
  XtSetValues(label, args, n);
  CommonPause();

  /* this test makes sure that the default locale encoding is valid */
  if (XmeGetEncodingAtom(label) == (Atom)None)
    printf("ERROR: locale encoding is invalid\n");
  tmp_str = XmStringCreateLocalized("locale test");
  n = 0;
  XtSetArg(args[n], XmNlabelString, tmp_str);  n++;
  XtSetValues(label, args, n);
  CommonPause();

  /* create an XmParseTable */
  n = 0;
  XtSetArg(args[n], XmNpattern, (XtPointer)"o");  n++;
  XtSetArg(args[n], XmNpatternType, XmCHARSET_TEXT);  n++;
  XtSetArg(args[n], XmNincludeStatus, XmINVOKE);  n++;
  XtSetArg(args[n], XmNinvokeParseProc, XmeGetNextCharacter);  n++;
  ParseMapping = XmParseMappingCreate(args, n);
  ParseTable = &ParseMapping;

  /* Test for the ParseProc XmeGetNextCharacter - parse the text
   * "Shoot Out, okay" and the XmeGetNextCharacter function will remove
   * instances of the letter "o" (result is "Shot Out, kay").
   */
  tmp_str = XmStringParseText("Shoot Out, okay", NULL, NULL, XmCHARSET_TEXT,
			      ParseTable, 1, (XtPointer)NULL);

  XtSetArg(args[n], XmNlabelString, tmp_str);  n++;
  XtSetValues(label, args, n);
  CommonPause();

  /* Test for the ParseProc XmeGetDirection - parse the text and return
   * an XmString component of type XmSTIRNG_COMPONENT_DIRECTION.
   */
  n = 0;
  XtSetArg(args[n], XmNinvokeParseProc, XmeGetDirection);  n++;
  XtSetArg(args[n], XmNpattern, (XtPointer)"s");  n++;
  XmParseMappingSetValues(ParseMapping, args, n);
  ParseTable = &ParseMapping;
  tmp_str = XmStringParseText("shot down", NULL, NULL, XmCHARSET_TEXT,
			      ParseTable, 1, (XtPointer)NULL);

  if (!XmStringInitContext(&StrContext, tmp_str))
    printf("ERROR: XmStringContext failed to initialize\n");
  int tmp = XmStringGetNextTriple(StrContext, &length, &value);
  if (tmp != XmSTRING_COMPONENT_CHARSET)
    printf("ERROR: incorrect XmString component encountered, expected XmSTRING_COMPONENT_CHARSET");
  tmp = XmStringGetNextTriple(StrContext, &length, &value);
  if (tmp != XmSTRING_COMPONENT_DIRECTION)
    printf("ERROR: incorrect XmString component encountered, expected XmSTRING_COMPONENT_DIRECTION");
  XtSetArg(args[n], XmNlabelString, tmp_str);  n++;
  XtSetValues(label, args, n);
  CommonPause();

  XmStringFree(target);
  XmStringFree(tmp_str);
  XmStringFree(xme_str);

  CommonPause();	/* quit */
  XtAppMainLoop(app_context);
  XtDestroyWidget(Shell1);
}
