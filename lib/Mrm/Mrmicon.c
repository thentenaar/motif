/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option)
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: Mrmicon.c /main/14 1996/11/13 14:01:43 drk $"
#endif
#endif

/*
 *++
 *  FACILITY:
 *
 *      UIL Resource Manager (URM)
 *
 *  ABSTRACT:
 *
 *	This module contains routines which operate on URM icon images -
 *	RGMIconImage structs.
 *
 *--
 */


/*
 *
 *  INCLUDE FILES
 *
 */

#include <stdio.h>
#include <Mrm/MrmAppl.h>
#include <Mrm/Mrm.h>
#include "MrmMsgI.h"




/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine creates and returns an X pixmap from a URM icon
 *	for some widget. It uses the foreground and background obtainable
 *	from the widget as required.
 *
 *  FORMAL PARAMETERS:
 *
 *	icon		URM icon image to be converted to X pixmap
 *	screen		screen to use for pixmap
 *	display		display to use for pixmap
 *	fgpix		foreground color for pixmap
 *	bgpix		background color for pixmap
 *	pixmap		to return resulting X pixmap
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	URM status
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
UrmCreatePixmap (RGMIconImagePtr	icon,
		 Screen			*screen,
		 Display		*display,
		 Pixel			fgpix,
		 Pixel			bgpix,
		 Pixel			*pixmap,
		 Widget			parent)
{

  /*
   *  Local variables
   */
  Cardinal		result;		/* function results */
  RGMColorTablePtr	ctable;		/* color table in icon */
  unsigned		maxbits;	/* # bits required for X image */
  int			srcpix;		/* # bits per pixel in icon (actual) */
  int			dds ;

  /*
   * Convert the color table colors to pixels.
   */
  ctable = icon->color_table.ctptr;
  result =
    Urm__RealizeColorTable (screen, display, fgpix, bgpix, ctable, parent);
  if ( result != MrmSUCCESS ) return result;

  /*
   * Use the depth of screen to infer the number of bits required to
   * hold the pixels in X format. The ZPixmap format only supports 1, 8, 16,
   * and 32 bit pixels, so we always map to one of these sizes.
   */
  if (parent) dds = parent->core.depth;
  else dds = DefaultDepthOfScreen(screen) ;

  if (dds == 1)
    maxbits = 1;
  else if (dds <= 8)
    maxbits = 8;
  else if (dds <= 16)
    maxbits = 16;
  else
    maxbits = 32;


  /*
   * See if the icon can be mapped as a bitmap. This will be true if the
   * color table has only FG/BG entries, or uses only those entries.
   */
  if ( ctable->count <= 2 ) maxbits = 1;


  /*
   * Compute the number of bits available in the icon pixmap
   */
  switch ( icon->pixel_size )
    {
    case URMPixelSize1Bit:
      srcpix = 1;
      break;
    case URMPixelSize2Bit:
      srcpix = 2;
      break;
    case URMPixelSize4Bit:
      srcpix = 4;
      break;
    case URMPixelSize8Bit:
      srcpix = 8;
      break;
    default:
      return MrmNOT_VALID;
    }

  /*
   * Map the icon image pixmap from color table indices to Pixel values.
   * There are three cases:
   *	maxbits == 1; a bitmap is possible, so map to a bitmap.
   *	maxbits <= # bits/pixel in pixmap. Map in place. In fact,
   *		only works if maxbits == srcpix == 8.
   *	maxbits > # bits/pixel. Map to allocated pixmap.
   *
   * Each of the three routines which performs these mappings completes
   * the entire process of doing the mapping and creating the X pixmap.
   */
  if ( maxbits == 1 )
    return Urm__MapIconBitmap
      (icon, srcpix, ctable, screen, display, pixmap);
  if ((maxbits == 8) && (srcpix == 8))
    return Urm__MapIconReplace
      (icon, srcpix, ctable, screen, display, pixmap, parent);
  if ( maxbits > srcpix )
    return Urm__MapIconAllocate
      (icon, srcpix, maxbits, ctable, screen, display, pixmap, parent);

  return MrmNOT_VALID;
}


/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine creates and returns an X pixmap of depth 1 from a URM icon
 *	for some widget.
 *
 *  FORMAL PARAMETERS:
 *
 *	icon		URM icon image to be converted to X pixmap
 *	screen		screen to use for pixmap
 *	display		display to use for pixmap
 *	pixmap		to return resulting X pixmap
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	URM status
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
UrmCreateBitmap (RGMIconImagePtr	icon,
		 Screen			*screen,
		 Display		*display,
		 Pixel			*pixmap)
{

  /*
   *  Local variables
   */
  int			srcpix;		/* # bits per pixel in icon (actual) */


  /*
   * Compute the number of bits available in the icon pixmap
   */
  switch ( icon->pixel_size )
    {
    case URMPixelSize1Bit:
      srcpix = URMPixelSize1Bit;
      break;
    default:
      return MrmNOT_VALID;
    }

  return Urm__MapIconBitmapDepth1 (icon, srcpix, screen, display, pixmap);

}


/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine creates and returns an X pixmap from the X bitmap file
 *	name specified.  It uses the foreground and background as needed.
 *
 *
 *  FORMAL PARAMETERS:
 *
 *	filename	file containing the bitmap to be converted to X pixmap
 *	screen		screen to use for pixmap
 *	display		display to use for pixmap - not used
 *	fgint		foreground color for pixmap
 *	bgint		background color for pixmap
 *	pixmap		to return resulting X pixmap
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	URM status
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Urm__CW_ReadBitmapFile (String			filename,
			Screen			*screen,
			Pixel			fgint,
			Pixel			bgint,
			Pixmap			*pixmap,
			Widget			parent)
{
  int		depth;
  char		err_msg[300];

  /*
  **  Create a pixmap from a X bitmap file specification
  */
  depth = parent ? parent->core.depth : DefaultDepthOfScreen(screen);

  *pixmap = XmGetPixmapByDepth(screen, filename, fgint, bgint, depth);

  if (*pixmap == XmUNSPECIFIED_PIXMAP)
    {
      pixmap = 0;
      sprintf (err_msg, _MrmMMsg_0033, filename);
      return Urm__UT_Error ("UrmReadBitmapFile", err_msg,
			    NULL, NULL, MrmFAILURE);
    }

  return MrmSUCCESS;
}




/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine maps the pixmap in the icon into a bitmap. The
 *	bitmap is written over pixmap data. The X pixmap is then
 *	constructed as an XYBitmap.
 *
 *  FORMAL PARAMETERS:
 *
 *	icon		the IconImage being converted
 *	srcpix		number of bits/pixel in icon
 *	ctable		the color table for (in) icon
 *	screen		screen for the X pixmap
 *	display		display for the X pixmap
 *	pixmap		to return the result
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Urm__MapIconBitmap(RGMIconImagePtr		icon,
		   int				srcpix,
		   RGMColorTablePtr		ctable,
		   Screen			*screen,
		   Display			*display,
		   Pixmap			*pixmap)
{

  /*
   *  Local variables
   */
  Pixel			fgpix;		/* foreground pixel value */
  int			iconwid;	/* icon width */
  int			srclinebyt;	/* # bytes per icon line */
  int			dstlinebyt;	/* # bytes per bitmap line */
  char			*srcbytptr;	/* image byte pointer */
  char			*dstbytptr;	/* bitmap pointer */
  int			lin;		/* icon line number */
  int			byt;		/* line byte number */
  int			pix;		/* line pixel number */
  unsigned char		srcbyt;		/* icon image byte */
  unsigned char		dstbyt;		/* mapped image byte */
  int			tndx;		/* color table index */
  XImage		*imagep;	/* X image */
  GC			gc;
  XGCValues		gcValues;
  int			endian;		/* to determine which endian. */

  /*
   * Overwrite the icon data with a bitmap. Use 0 for background, 1 for
   * foreground.
   */
  fgpix = ctable->item[URMColorTableFG].color_pixel;
  iconwid = icon->width;
  srclinebyt = (iconwid*srcpix+7) / 8;
  dstlinebyt = (iconwid+7) / 8;
  srcbytptr = icon->pixel_data.pdptr;
  for ( lin=0 ; lin<icon->height ; lin++ )
    {
      pix = 0;
      dstbytptr = icon->pixel_data.pdptr + lin*dstlinebyt;
      dstbyt = 0;
      for ( byt=0 ; byt<srclinebyt ; byt++ )
        {
	  srcbyt = *srcbytptr;
	  switch ( icon->pixel_size )
            {
            case URMPixelSize1Bit:
	      *dstbytptr = srcbyt;
	      srcbytptr += 1;
	      dstbytptr += 1;
	      pix += 8;
	      /*
	       *  NOTE: The original algorithm used here cleared any
	       *        unused bits of the last byte.  I don't think
	       *        the protocol requires that...
	       if ( pix > iconwid )
	       */
	      continue;
            case URMPixelSize2Bit:
	      tndx = srcbyt & 0x3;
	      if ( pix < iconwid )
		if ( ctable->item[tndx].color_pixel == fgpix)
		  dstbyt |= 1<<(pix%8);
	      pix += 1;
	      srcbyt >>= 2;
	      tndx = srcbyt & 0x3;
	      if ( pix < iconwid )
		if ( ctable->item[tndx].color_pixel == fgpix)
		  dstbyt |= 1<<(pix%8);
	      pix += 1;
	      srcbyt >>= 2;
	      tndx = srcbyt & 0x3;
	      if ( pix < iconwid )
		if ( ctable->item[tndx].color_pixel == fgpix)
		  dstbyt |= 1<<(pix%8);
	      pix += 1;
	      srcbyt >>= 2;
	      tndx = srcbyt & 0x3;
	      if ( pix < iconwid )
		if ( ctable->item[tndx].color_pixel == fgpix)
		  dstbyt |= 1<<(pix%8);
	      pix += 1;
	      break;
            case URMPixelSize4Bit:
	      tndx = srcbyt & 0xF;
	      if ( pix < iconwid )
		if ( ctable->item[tndx].color_pixel == fgpix)
		  dstbyt |= 1<<(pix%8);
	      pix += 1;
	      srcbyt >>= 4;
	      tndx = srcbyt & 0xF;
	      if ( pix < iconwid )
		if ( ctable->item[tndx].color_pixel == fgpix)
		  dstbyt |= 1<<(pix%8);
	      pix += 1;
	      break;
            case URMPixelSize8Bit:
	      if ( pix < iconwid )
		if ( ctable->item[srcbyt].color_pixel == fgpix)
		  dstbyt |= 1<<(pix%8);
	      pix += 1;
	      break;
            }
	  /*
	   *  NOTE:  The "1Bit" case CONTINUEs directly and will never
	   *         reach this point in the loop...
	   */
	  srcbytptr += 1;
	  if ( pix%8 == 0 )
            {
	      *dstbytptr = dstbyt;
	      dstbytptr += 1;
	      dstbyt = 0;
            }
        }
      if ( pix%8 != 0 )
        *dstbytptr = dstbyt;
    }


  imagep = XCreateImage(display,
			DefaultVisualOfScreen(screen),
			1,
			XYBitmap,
			0,
			icon->pixel_data.pdptr,
			(unsigned int)icon->width,
			(unsigned int)icon->height,
			8,
			dstlinebyt);

  if ( imagep == NULL )
    return Urm__UT_Error ("Urm__MapIconBitmap", _MrmMMsg_0034,
			  NULL, NULL, MrmFAILURE);

  /*
   * Well, Uil creates a uid icon in its byteorder, on his side
   * XCreateImage creates an image in the byte_order of the server, so
   * its not correct, we have to adjust the byte_order fields of this
   * image to match those of the icon.
   */

  endian = 1;
  imagep->bitmap_unit = 8 ;
  if (*(char *) &endian) {
    imagep->byte_order = LSBFirst ;
    imagep->bitmap_bit_order = LSBFirst ;
  }
  else {
    imagep->byte_order = MSBFirst;
    imagep->bitmap_bit_order = LSBFirst ;
  }

  *pixmap = XCreatePixmap (display,
			   RootWindowOfScreen(screen),
			   icon->width,
			   icon->height,
			   (unsigned)DefaultDepthOfScreen(screen));
  if ( *pixmap == (Pixmap)0)
    {
      XFree((char*)imagep);
      return Urm__UT_Error ("Urm__MapIconBitmap", _MrmMMsg_0035,
			    NULL, NULL, MrmFAILURE);
    }

  /*
   * Build a gc to use when drawing into the pixmap
   */
  gcValues.foreground = ctable->item[URMColorTableFG].color_pixel;
  gcValues.background = ctable->item[URMColorTableBG].color_pixel;
  gcValues.fill_style = FillTiled;
  gcValues.tile       = *pixmap;

  gc = XCreateGC (display,
		  RootWindowOfScreen (screen),
		  GCForeground | GCBackground | GCFillStyle | GCTile,
		  &gcValues);
  if ( gc == NULL )
    return Urm__UT_Error ("Urm__MapIconBitmap", _MrmMMsg_0036,
			  NULL, NULL, MrmFAILURE);

  /*
   * Put bits into the pixmap
   */
  XPutImage (display,
	     *pixmap,
	     gc,
	     imagep,
	     0, 0,				/* source x, y */
	     0, 0, icon->width, icon->height);	/* dest, loc & size */

  XFreeGC (display, gc);
  XFree ((char*)imagep);

  /*
   * Successfully created
   */
  return MrmSUCCESS;

}


/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine maps the pixmap in the icon into a bitmap. The
 *	bitmap is written over pixmap data. The X pixmap is then
 *	constructed as an XYBitmap.
 *
 *  FORMAL PARAMETERS:
 *
 *	icon		the IconImage being converted
 *	srcpix		number of bits/pixel in icon
 *	screen		screen for the X pixmap
 *	display		display for the X pixmap
 *	pixmap		to return the result
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Urm__MapIconBitmapDepth1 (RGMIconImagePtr	icon,
			  int			srcpix,
			  Screen		*screen,
			  Display		*display,
			  Pixmap		*pixmap)
{

  /*
   *  Local variables
   */
  int			iconwid;	/* icon width */
  int			srclinebyt;	/* # bytes per icon line */
  int			dstlinebyt;	/* # bytes per bitmap line */
  char			*srcbytptr;	/* image byte pointer */
  char			*dstbytptr;	/* bitmap pointer */
  int			lin;		/* icon line number */
  int			byt;		/* line byte number */
  int			pix;		/* line pixel number */
  unsigned char		srcbyt;		/* icon image byte */
  unsigned char		dstbyt;		/* mapped image byte */
  XImage		*imagep;	/* X image */
  GC			gc;
  XGCValues		gcValues;
  int			endian;		/* to determine which endian. */


  /*
   * Overwrite the icon data with a bitmap. Use 0 for background, 1 for
   * foreground.
   */
  iconwid = icon->width;
  srclinebyt = (iconwid*srcpix+7) / 8;
  dstlinebyt = (iconwid+7) / 8;
  srcbytptr = icon->pixel_data.pdptr;
  for ( lin=0 ; lin<icon->height ; lin++ )
    {
      pix = 0;
      dstbytptr = icon->pixel_data.pdptr + lin*dstlinebyt;
      dstbyt = 0;
      for ( byt=0 ; byt<srclinebyt ; byt++ )
        {
	  srcbyt = *srcbytptr;
	  switch ( icon->pixel_size )
            {
            case URMPixelSize1Bit:
	      *dstbytptr = srcbyt;
	      srcbytptr += 1;
	      dstbytptr += 1;
	      pix += 8;
	      /*
	       *  NOTE: The original algorithm used here cleared any
	       *        unused bits of the last byte.  I don't think
	       *        the protocol requires that...
	       if ( pix > iconwid )
	       */
	      continue;
	    default:
	      return MrmNOT_VALID;
            }
        }
      if ( pix%8 != 0 )
        *dstbytptr = dstbyt;
    }


  imagep = XCreateImage(display,
			DefaultVisualOfScreen(screen),
			1,
			XYBitmap,
			0,
			icon->pixel_data.pdptr,
			(unsigned int)icon->width,
			(unsigned int)icon->height,
			8,
			dstlinebyt);

  if ( imagep == NULL )
    return Urm__UT_Error ("Urm__MapIconBitmapDepth1", _MrmMMsg_0034,
			  NULL, NULL, MrmFAILURE);

  /* Well, Uil creates a uid icon in its byteorder, on his side
     XCreateImage creates an image in the byte_order of the server, so
     its not correct, we have to adjust the byte_order fields of this
     image to match those of the icon. */

  endian = 1;
  imagep->bitmap_unit = 8 ;
  if (*(char *) &endian) {
    imagep->byte_order = LSBFirst ;
    imagep->bitmap_bit_order = LSBFirst ;
  }
  else {
    imagep->byte_order = MSBFirst;
    imagep->bitmap_bit_order = LSBFirst ;
  }

  *pixmap = XCreatePixmap (display,
			   RootWindowOfScreen(screen),
			   icon->width,
			   icon->height,
			   1);
  if ( *pixmap == (Pixmap)0)
    {
      XFree((char*)imagep);
      return Urm__UT_Error ("Urm__MapIconBitmapDepth1", _MrmMMsg_0035,
			    NULL, NULL, MrmFAILURE);
    }


  /*
   * Build a gc to use when drawing into the pixmap
   */
  gcValues.foreground = 1;
  gcValues.background = 0;
  gcValues.fill_style = FillTiled;
  gcValues.tile       = *pixmap;

  gc = XCreateGC (display,
		  *pixmap,
		  GCForeground | GCBackground | GCFillStyle | GCTile,
		  &gcValues);

  if ( gc == NULL )
    return Urm__UT_Error ("Urm__MapIconBitmapDepth1", _MrmMMsg_0036,
			  NULL, NULL, MrmFAILURE);

  /*
   * Put bits into the pixmap
   */
  XPutImage (display,
	     *pixmap,
	     gc,
	     imagep,
	     0, 0,				/* source x, y */
	     0, 0, icon->width, icon->height);	/* dest, loc & size */


  XFreeGC (display, gc);
  XFree ((char *)imagep);

  /*
   * Successfully created
   */
  return MrmSUCCESS;
}


/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine maps the pixmap in the icon into a ZPixmap. The
 *	ZPixmap is written over the icon image pixmap data. The X pixmap
 *	is then constructed as a ZPixmap from the overwritten data.
 *
 *	This routine is called only when srcpix == 8.
 *
 *  FORMAL PARAMETERS:
 *
 *	icon		the IconImage being converted
 *	srcpix		number of bits/pixel in icon
 *	ctable		the color table for (in) icon
 *	screen		screen for the X pixmap
 *	display		display for the X pixmap
 *	pixmap		to return the result
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Urm__MapIconReplace (RGMIconImagePtr		icon,
		     int			srcpix,
		     RGMColorTablePtr		ctable,
		     Screen			*screen,
		     Display			*display,
		     Pixmap			*pixmap,
		     Widget			parent)
{

  /*
   *  Local variables
   */
  int			iconwid;	/* icon width */
  int			linebyt;	/* # bytes per icon line */
  char			*bytptr;	/* image byte pointer */
  int			lin;		/* icon line number */
  int			byt;		/* line byte number */
  int			pix;		/* line pixel number */
  unsigned char		srcbyt;		/* icon image byte */
  XImage		*imagep;	/* X image */
  GC			gc;
  XGCValues		gcValues;
  int			depth;		/* depth of screen  */

  /*
   * Overwrite the pixmap with actual Pixel values. The source and destination
   * bit widths are the same (==8), and the same pointer can be used for
   * both source and destination.
   */
  iconwid = icon->width;
  linebyt = (iconwid*srcpix+7) / 8;
  bytptr = icon->pixel_data.pdptr;
  for ( lin=0 ; lin<icon->height ; lin++ )
    {
      pix = 0;
      for ( byt=0 ; byt<linebyt ; byt++ )
        {
	  srcbyt = *bytptr;
	  if ( pix < iconwid )
	    *bytptr = ctable->item[srcbyt].color_pixel;
	  pix += 1;
	  bytptr += 1;
        }
    }

  depth = parent ? parent->core.depth : DefaultDepthOfScreen(screen);

  imagep = XCreateImage(display,
			DefaultVisualOfScreen(screen),
			depth,
			ZPixmap,
			0,
			icon->pixel_data.pdptr,
			(unsigned int)icon->width,
			(unsigned int)icon->height,
			srcpix,
			linebyt);

  if ( imagep == NULL )
    return Urm__UT_Error ("Urm__MapIconReplace", _MrmMMsg_0034,
			  NULL, NULL, MrmFAILURE);

  *pixmap = XCreatePixmap (display,
			   RootWindowOfScreen(screen),
			   icon->width,
			   icon->height,
			   (unsigned)depth);
  if ( *pixmap == (Pixmap)0)
    {
      XFree((char*)imagep);
      return Urm__UT_Error ("Urm__MapIconReplace", _MrmMMsg_0035,
			    NULL, NULL, MrmFAILURE);
    }

  /*
   * Build a gc to use when drawing into the pixmap
   */
  gcValues.foreground = ctable->item[URMColorTableFG].color_pixel;
  gcValues.background = ctable->item[URMColorTableBG].color_pixel;
  gcValues.fill_style = FillTiled;
  gcValues.tile       = *pixmap;

  gc = XCreateGC (display,
		  RootWindowOfScreen (screen),
		  GCForeground | GCBackground | GCFillStyle | GCTile,
		  &gcValues);
  if ( gc == NULL )
    return Urm__UT_Error ("Urm__MapIconReplace", _MrmMMsg_0036,
			  NULL, NULL, MrmFAILURE);

  /*
   * Put bits into the pixmap
   */
  XPutImage (display,
	     *pixmap,
	     gc,
	     imagep,
	     0, 0,				/* source x, y */
	     0, 0, icon->width, icon->height);	/* dest, loc & size */

  XFreeGC (display, gc);
  XFree((char*)imagep);

  /*
   * Successfully created
   */
  return MrmSUCCESS;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine maps the pixmap in the icon into a ZPixmap. The
 *	ZPixmap is allocated 1 byte per pixel, as the icon image pixmap
 *	has too few bits per pixel to be replace in situ. The X pixmap
 *	is created from the allocated ZPixmap.
 *
 *  FORMAL PARAMETERS:
 *
 *	icon		the IconImage being converted
 *	srcpix		number of bits/pixel in icon
 *	dstpix		number of bits/pixel in resulting image
 *	ctable		the color table for (in) icon
 *	screen		screen for the X pixmap
 *	display		display for the X pixmap
 *	pixmap		to return the result
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Urm__MapIconAllocate (RGMIconImagePtr		icon,
		      int			srcpix,
		      int			dstpix,
		      RGMColorTablePtr		ctable,
		      Screen			*screen,
		      Display			*display,
		      Pixmap			*pixmap,
		      Widget			parent)
{

  /*
   *  Local variables
   */
  int			iconwid;	/* icon width */
  int			iconhgt;	/* icon height */
  char			*alloc_pixmap;	/* allocated pixmap */
  int			dstbytsize;	/* # 8-bit bytes per bitmap byte */
  int			srclinebyt;	/* # bytes per icon line */
  char			*srcbytptr;	/* image byte pointer */
  int			lin;		/* icon line number */
  int			byt;		/* line byte number */
  int			bit;		/* bit loop index */
  int			pix;		/* line pixel number */
  unsigned char		srcbyt;		/* icon image byte */
  int			bitmask = 0;    /* mask all but significant bits */
  int			num_bits = 0;   /* real (not coded) pixel size */
  int			tndx;		/* color table index */
  XImage		*imagep;	/* X image */
  GC			gc;
  XGCValues		gcValues;
  int			depth;		/* depth of screen */

  /*
   * Allocate a new pixmap image.
   */
  iconwid = icon->width;
  iconhgt = icon->height;
  if (dstpix <= 8) dstpix = 8;
  else if (dstpix <= 16) dstpix = 16;
  else dstpix = 32;
  dstbytsize = dstpix / 8;
  alloc_pixmap = (char *) XtMalloc (iconwid * iconhgt * dstbytsize);
  if (alloc_pixmap == NULL)
    return Urm__UT_Error ("Urm__MapIconAllocate", _MrmMMsg_0037,
			  NULL, NULL, MrmFAILURE);
  srclinebyt = (iconwid * srcpix + 7) / 8;
  srcbytptr = icon->pixel_data.pdptr;

  depth = parent ? parent->core.depth : DefaultDepthOfScreen(screen);

  imagep = XCreateImage(display,
			DefaultVisualOfScreen(screen),
			depth,
			ZPixmap,
			0,
			alloc_pixmap,
			iconwid,
			iconhgt,
			dstpix,
			0);	/* Let Xlib calculate it. */

  if ( imagep == NULL )
    {
      XtFree (alloc_pixmap);
      return Urm__UT_Error ("Urm__MapIconAllocate", _MrmMMsg_0034,
			    NULL, NULL, MrmFAILURE);
    }

  if(icon->pixel_size == URMPixelSize1Bit)
    { num_bits = 1; bitmask=0x1; }
  else if(icon->pixel_size == URMPixelSize2Bit)
    { num_bits = 2; bitmask=0x3; }
  else if(icon->pixel_size == URMPixelSize4Bit)
    { num_bits = 4; bitmask=0xF; }
  else if(icon->pixel_size == URMPixelSize8Bit)
    { num_bits = 8; bitmask=0xFF; }
  for ( pix=0,lin=0 ; lin<icon->height ; pix=0,lin++ )
    {
      for ( byt=0 ; byt<srclinebyt ; byt++,srcbytptr++ )
	{
	  srcbyt = *srcbytptr;
	  for ( bit=0 ; bit<8 ; bit+=num_bits )
	    {
	      tndx = srcbyt & bitmask;
	      if ( pix < iconwid )
		XPutPixel(imagep, pix, lin, ctable->item[tndx].color_pixel);
	      pix++;
	      srcbyt >>= num_bits;
	    }
	}
    }

  *pixmap = XCreatePixmap (display,
			   RootWindowOfScreen(screen),
			   iconwid,
			   iconhgt,
			   (unsigned)depth);
  if ( *pixmap == (Pixmap)0)
    {
      XtFree (alloc_pixmap);
      XFree((char*)imagep);
      return Urm__UT_Error ("Urm__MapIconAllocate", _MrmMMsg_0035,
			    NULL, NULL, MrmFAILURE);
    }

  /*
   * Build a gc to use when drawing into the pixmap
   */
  gcValues.foreground = ctable->item[URMColorTableFG].color_pixel;
  gcValues.background = ctable->item[URMColorTableBG].color_pixel;
  gcValues.fill_style = FillTiled;
  gcValues.tile       = *pixmap;

  gc = XCreateGC (display,
		  RootWindowOfScreen (screen),
		  GCForeground | GCBackground | GCFillStyle | GCTile,
		  &gcValues);
  if ( gc == NULL )
    {
      XtFree (alloc_pixmap);
      return Urm__UT_Error ("Urm__MapIconAllocate", _MrmMMsg_0036,
			    NULL, NULL, MrmFAILURE);
    }

  /*
   * Put bits into the pixmap
   */
  XPutImage (display,
	     *pixmap,
	     gc,
	     imagep,
	     0, 0,				/* source x, y */
	     0, 0, iconwid, iconhgt);		/* dest, loc & size */

  /*
   * don't deallocate with XDestroyImage, which would destroy alloc_pixmap
   */
  XFree((char*)imagep);
  XFreeGC (display, gc);
  XtFree (alloc_pixmap);

  /*
   * Successfully created
   */
  return MrmSUCCESS;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine sets the Pixel values corresponding to each of the
 *	entries in the color table. Foreground and background are set by
 *	querying the widget for those values, or falling back on
 *	Black/WhitePixelOfScreen. All other colors are set by honoring
 *	FG/BG setting for monochrome devices, or by getting the xolor
 *	Pixel values from X.
 *
 *  FORMAL PARAMETERS:
 *
 *	screen		screen to use for color table
 *	display		display to use for color table
 *	fgpix		foreground color for color table
 *	bgpix		background color for color table
 *	ctable		the color table
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Urm__RealizeColorTable (Screen			*screen,
			Display			*display,
			Pixel			fgpix,
			Pixel			bgpix,
			RGMColorTablePtr	ctable,
			Widget			parent)
{

  /*
   *  Local variables
   */
  Cardinal		result = 0;		/* function results */
  Cardinal		ndx;		/* loop index */
  RGMColorTableEntryPtr	citem;		/* color table entry */
  Colormap		cmap;		/* default color map */
  int			depth;		/* # planes in screen */
  char			err_msg[300];


  /*
   * Load the foreground and background pixel values.
   */
  ctable->item[URMColorTableFG].color_pixel = fgpix;
  ctable->item[URMColorTableBG].color_pixel = bgpix;

  /*
   * Get the Pixel for each defined color. Honor the FG/BG specification
   * if present on monochrome displays. Otherwise, get the Pixel value for
   * the color. Use the FG/BG specification as a fallback for unfound
   * colors in non-monochrome. If no reasonable color Pixel can be found,
   * return an error.
   */
  cmap = parent ? parent->core.colormap : DefaultColormapOfScreen(screen);
  depth = parent ? parent->core.depth : DefaultDepthOfScreen(screen);

  for ( ndx=URMColorTableUserMin ; ndx<ctable->count ; ndx++ )
    {
      citem = &ctable->item[ndx];
      if ( depth == 1 )
        switch ( citem->color_item.cptr->mono_state )
	  {
	  case URMColorMonochromeUnspecified:
	    switch (citem->color_item.cptr->desc_type)
	      {
	      case URMColorDescTypeName:
		result = Urm__UT_GetNamedColorPixel
		  (display, cmap, citem->color_item.cptr, &citem->color_pixel,
		   ctable->item[URMColorTableBG].color_pixel);
		if ( result != MrmSUCCESS) {
		  /* we use PARTIAL_SUCCESS only to indicate
		     the color allocation failed and we've
		     substituted the fallback color. We still
		     want a warning, though */
		  if (result == MrmPARTIAL_SUCCESS) {
		    result = MrmSUCCESS;
		    sprintf (err_msg, _MrmMMsg_0038,
			     citem->color_item.cptr->desc.name);
		    return Urm__UT_Error ("Urm__RealizeColorTable",
					  err_msg, NULL, NULL, result);
		  } else {
		    sprintf (err_msg, _MrmMMsg_0038,
			     citem->color_item.cptr->desc.name);
		    return Urm__UT_Error ("Urm__RealizeColorTable",
					  err_msg, NULL, NULL, result);
		  }
		}
		break;
	      case URMColorDescTypeRGB:
		result = Urm__UT_GetColorPixel
		  (display, cmap, citem->color_item.cptr, &citem->color_pixel,
		   ctable->item[URMColorTableBG].color_pixel);
		if ( result != MrmSUCCESS ) {
		  /* we use PARTIAL_SUCCESS only to indicate
		     the color allocation failed and we've
		     substituted the fallback color. We still
		     want a warning, though */
		  if (result == MrmPARTIAL_SUCCESS) {
		    result = MrmSUCCESS;
		    sprintf (err_msg, _MrmMMsg_0038,
			     citem->color_item.cptr->desc.name);
		    return Urm__UT_Error ("Urm__RealizeColorTable",
					  err_msg, NULL, NULL, result);
		  } else {
		    sprintf (err_msg, _MrmMMsg_0039,
			     citem->color_item.cptr->desc.rgb.red,
			     citem->color_item.cptr->desc.rgb.green,
			     citem->color_item.cptr->desc.rgb.blue) ;
		    return Urm__UT_Error ("Urm__RealizeColorTable",
					  err_msg, NULL, NULL, result);
		  }
		}
		break;
	      default:
		strcpy(err_msg, _MrmMMsg_0040);
		return Urm__UT_Error ("Urm__RelizeColorTable",
				      err_msg, NULL, NULL, MrmFAILURE) ;
	      }
	    break;
	  case URMColorMonochromeForeground:
	    citem->color_pixel =
	      ctable->item[URMColorTableFG].color_pixel;
	    break;
	  case URMColorMonochromeBackground:
	    citem->color_pixel =
	      ctable->item[URMColorTableBG].color_pixel;
	    break;
	  default:
	    sprintf (err_msg, _MrmMMsg_0041,
		     citem->color_item.cptr->mono_state);
	    return Urm__UT_Error ("Urm__RealizeColorTable",
				  err_msg, NULL, NULL, result);
	  }
      else
        {
	  switch (citem->color_item.cptr->desc_type)
	    {
	    case URMColorDescTypeName:
	      result = Urm__UT_GetNamedColorPixel
		(display, cmap, citem->color_item.cptr, &citem->color_pixel,
		 ((citem->color_item.cptr->mono_state ==
		   URMColorMonochromeForeground) ?
		  ctable->item[URMColorTableFG].color_pixel :
		  ctable->item[URMColorTableBG].color_pixel));
	      if (result != MrmSUCCESS) {
		/* we use PARTIAL_SUCCESS only to indicate
		   the color allocation failed and we've
		   substituted the fallback color. We still
		   want a warning, though */
		if (result == MrmPARTIAL_SUCCESS) {
		  result = MrmSUCCESS;
		  sprintf (err_msg, _MrmMMsg_0038,
			   citem->color_item.cptr->desc.name);
		  return Urm__UT_Error ("Urm__RealizeColorTable",
					err_msg, NULL, NULL, result);
		} else {
		  sprintf (err_msg, _MrmMMsg_0038,
			   citem->color_item.cptr->desc.name);
		  Urm__UT_Error ("Urm__RealizeColorTable",
				 err_msg, NULL, NULL, result);
		}
	      }
	      break;
	    case URMColorDescTypeRGB:
	      result = Urm__UT_GetColorPixel
		(display, cmap, citem->color_item.cptr, &citem->color_pixel,
		 ctable->item[URMColorTableBG].color_pixel);
	      if (result != MrmSUCCESS) {
		/* we use PARTIAL_SUCCESS only to indicate
		   the color allocation failed and we've
		   substituted the fallback color. We still
		   want a warning, though */
		if (result == MrmPARTIAL_SUCCESS) {
		  result = MrmSUCCESS;
		  sprintf (err_msg, _MrmMMsg_0038,
			   citem->color_item.cptr->desc.name);
		  return Urm__UT_Error ("Urm__RealizeColorTable",
					err_msg, NULL, NULL, result);
		} else {
		  sprintf (err_msg, _MrmMMsg_0039,
			   citem->color_item.cptr->desc.rgb.red,
			   citem->color_item.cptr->desc.rgb.green,
			   citem->color_item.cptr->desc.rgb.blue) ;
		  Urm__UT_Error ("Urm__RealizeColorTable",
				 err_msg, NULL, NULL, result);
		}
	      }
	      break;
	    default:
	      result = MrmFAILURE;
	      strcpy(err_msg, _MrmMMsg_0040);
	      Urm__UT_Error ("Urm__RelizeColorTable",
			     err_msg, NULL, NULL, MrmFAILURE) ;
	    }
	  if ( result != MrmSUCCESS )
            {
	      switch ( citem->color_item.cptr->mono_state )
                {
                case URMColorMonochromeForeground:
		  citem->color_pixel =
		    ctable->item[URMColorTableFG].color_pixel;
		  break;
                case URMColorMonochromeBackground:
		  citem->color_pixel =
		    ctable->item[URMColorTableBG].color_pixel;
		  break;
                default:
		  return result;
                }
            }
        }
    }

  return MrmSUCCESS;
}

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine makes a copy of a URM icon into a memory block
 *	which has been pre-allocated. The block must be big enough
 *	to hold both the header and the bit vector.
 *
 *  FORMAL PARAMETERS:
 *
 *	dst_icon	the memory block to receive the copy
 *	src_icon	the URM icon descriptor to be copied.
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */


RGMIconImagePtr
UrmCopyAllocatedIconImage (RGMIconImagePtr	dst_icon,
			   RGMIconImagePtr	src_icon)
{

  /*
   * Copy the header and bit vector into the new memory block.
   */
  dst_icon->validation = URMIconImageValid;
  dst_icon->pixel_size = src_icon->pixel_size;
  dst_icon->width = src_icon->width;
  dst_icon->height = src_icon->height;
  dst_icon->hot_x = src_icon->hot_x;
  dst_icon->hot_y = src_icon->hot_y;

  /*
   * Copy the color table as an immediate. It is allocated immediately
   * after the image header.
   */

  /*
   * Copy the pixel data
   */

  return dst_icon;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine attempts to look up the name of a color, and return
 *	the pixel value required as a widget arglist value. It will use
 *	the default color map if necessary, and returns the closest color
 *	supported by the hardware (as defined by X), not the exact database
 *	definition. If the screen is depth 1 (monochrome), the routine will
 *	honor the monochrome rendition specified in the color descriptor.
 *
 *  FORMAL PARAMETERS:
 *
 *	display		specifies the X server connection
 *	cmap		color map ID. If NULL, the default color map is used
 *	colorptr	color descriptor
 *	pixel_return	to return the pixel value for the color
 *      fallback        fallback color to use in case of alloc failure
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	   color found and translated
 *	MrmNOT_FOUND	   conversion failure
 *      MrmPARTIAL_SUCCESS internal only, for allocation failure
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Urm__UT_GetNamedColorPixel (Display		*display,
			    Colormap		cmap,
			    RGMColorDescPtr	colorptr,
			    Pixel		*pixel_return,
			    Pixel		fallback)
{

  /*
   *  Local variables
   */
  XColor		screen_def;	/* realizable values */
  XColor		exact_def;	/* exact values */
  int			status;		/* function return */


  if ( cmap == (Colormap)0)
    cmap = DefaultColormap (display, DefaultScreen(display));

  /* CR 9891: Support new pixel constants. */
  if (XmeNamesAreEqual(colorptr->desc.name, "default_select_color"))
    {
      *pixel_return = XmDEFAULT_SELECT_COLOR;
      return MrmSUCCESS;
    }
  else if (XmeNamesAreEqual(colorptr->desc.name, "reversed_ground_colors"))
    {
      *pixel_return = XmREVERSED_GROUND_COLORS;
      return MrmSUCCESS;
    }
  else if (XmeNamesAreEqual(colorptr->desc.name, "highlight_color"))
    {
      *pixel_return = XmHIGHLIGHT_COLOR;
      return MrmSUCCESS;
    }

  status = XAllocNamedColor
    (display, cmap, colorptr->desc.name, &screen_def, &exact_def);

  if ( status == 0) {
    if (fallback) {
      *pixel_return = fallback;
      return MrmPARTIAL_SUCCESS;
    }
    else
      return MrmFAILURE;
  }
  *pixel_return = screen_def.pixel;
  return MrmSUCCESS;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine attempts to look up the RGB values of a color, and return
 *	the pixel value required as a widget arglist value. It will use
 *	the default color map if necessary, and returns the closest color
 *	supported by the hardware (as defined by X), not the exact database
 *	definition. If the screen is depth 1 (monochrome), the routine will
 *	honor the monochrome rendition specified in the color descriptor.
 *
 *  FORMAL PARAMETERS:
 *
 *	display		specifies the X server connection
 *	cmap		color map ID. If NULL, the default color map is used
 *	colorptr	color descriptor
 *	pixel_return	to return the pixel value for the color
 *      fallback        fallback color to use in case of alloc failure
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	MrmSUCCESS	color found and translated
 *	MrmNOT_FOUND	conversion failure
 *      MrmPARTIAL_SUCCESS internal only, for allocation failure
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
Urm__UT_GetColorPixel (Display			*display,
		       Colormap			cmap,
		       RGMColorDescPtr		colorptr,
		       Pixel			*pixel_return,
		       Pixel			fallback)
{

  /*
   *  Local variables
   */
  XColor		screen_in_out;	/* realizable values */
  int			status;		/* function return */


  if ( cmap == (Colormap)0)
    cmap = DefaultColormap (display, DefaultScreen(display));
  screen_in_out.red = colorptr->desc.rgb.red;
  screen_in_out.green = colorptr->desc.rgb.green;
  screen_in_out.blue = colorptr->desc.rgb.blue;
  status = XAllocColor (display, cmap, &screen_in_out);

  if ( status == 0) {
    if (fallback) {
      *pixel_return = fallback;
      return MrmPARTIAL_SUCCESS;
    } else
      return MrmFAILURE;
  }
  *pixel_return = screen_in_out.pixel;
  return MrmSUCCESS;

}



/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine computes the number of bytes used by a URM icon
 *
 *  FORMAL PARAMETERS:
 *
 *	icon		URM icon image
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	# bytes in the image
 *
 *  SIDE EFFECTS:
 *
 *--
 */

Cardinal
UrmIconImageSize (RGMIconImagePtr	icon)
{

  /*
   *  Local variables
   */
  int			bytes_per_line;	/* # bytes for padded width */
  int			raster_len;	/* bytes in image */
  Cardinal		size;		/* # bytes in descriptor */


  bytes_per_line = (icon->width+7) / 8;
  raster_len = bytes_per_line * icon->height;
  size = sizeof(RGMIconImage) + (raster_len-1)*sizeof(char);
  return size;

}


/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine computes the number of bytes necessary to store
 *	the given color table in a single memory block.
 *
 *  FORMAL PARAMETERS:
 *
 *	ctable		An allocated color table
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *	table size in bytes
 *
 *  SIDE EFFECTS:
 *
 *--
 */

/*ARGSUSED*/
Cardinal
UrmColorTableSize (RGMColorTablePtr	ctable)	/* unused */
{

  return sizeof(RGMColorTable);

}
