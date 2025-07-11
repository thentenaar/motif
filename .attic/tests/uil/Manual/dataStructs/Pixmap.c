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
static char rcsid[] = "$XConsortium: Pixmap.c /main/4 1995/07/13 20:34:09 drk $"
#endif
#endif

#include "Pixmap.h"

main(argc, argv)
     int argc;
     char *argv[];
{ 	
    int		i, j, k,
    		errors = 0,
    		data_errors = 0,
    		total_errors = 0;

    char	err_msg[80],
    		error_message[1000];

    String	null_str = "";

/*
 * Initialize Mrm
 */
    MrmInitialize();

/*
 *  Initialize the toolkit.  This call returns the id of the "toplevel" widget.
 */

    toplevel = XtInitialize("Pixmap",		/* application name	   */
			    "genericclass",	/* application class	   */
			    NULL, 0,		/* options		   */
			    &argc, argv);	/* command line parameters */

/*
 *  Open the Mrm hierarchy
 */

    if(MrmOpenHierarchy(filename_num,		/* number of files	*/
			filename_vec,		/* files		*/
			NULL,			/* os_ext_list (null)	*/
			&s_MrmHierarchy)	/* ptr to returned id	*/
       != MrmSUCCESS) 
    {
	error_proc("Can't open hierarchy\n");
    }

/*
 * Get display, screen, depth, colormap, black, white
 */

    dsp = XtDisplay(toplevel);

    screen = DefaultScreenOfDisplay(dsp);

    draw = RootWindow(dsp, DefaultScreen(dsp));

    depth = DefaultDepth(dsp, DefaultScreen(dsp));

    cmap = DefaultColormap(dsp, DefaultScreen(dsp));

    XAllocNamedColor(dsp,		/* display			*/
		     cmap,		/* colormap			*/
		     "white",		/* color name			*/
		     &colorcell_def,	/* RGB & pixel values from cmap	*/
		     &rgb_db_def);	/* exact RGB values from db     */

    white = colorcell_def.pixel;

    XAllocNamedColor(dsp,
		     cmap,
		     "black",
		     &colorcell_def,
		     &rgb_db_def);

    black = colorcell_def.pixel;

/*
 * Fetch UIL icon literals and test against equivalent Xlib-generated pixmaps
 */
    for(i=0 ; i<=1 ; i++)
    {
	if(MrmFetchIconLiteral(s_MrmHierarchy,	/* id of uid hierarchy	*/
			       icon_names[i],	/* uil name of icon	*/
			       screen,		/* screen		*/
			       dsp,		/* display		*/
			       black,		/* foreground color	*/
			       white,		/* background color	*/
			       &UIL_pixmap	/* pixmap		*/
			       ) != MrmSUCCESS)

	{
	    error_proc("Can't fetch icon literal");
	}

	X_pixmap = XCreatePixmapFromBitmapData(dsp,		/* display  */
					       draw,		/* drawable */
					       bitmaps[i],	/* bitmap   */
					       bitmap_width,	/* width    */
					       bitmap_height,	/* height   */
					       black,		/* fg color */
					       white,		/* bg color */
					       depth);		/* depth    */

/*
 * Get images from pixmaps
 */

	if((UIL_image = XGetImage(dsp,			/* display	*/
				  UIL_pixmap,		/* pixmap	*/
				  0, 0,			/* x, y coords	*/
				  bitmap_width,		/* width	*/
				  bitmap_height,	/* height	*/
				  AllPlanes,		/* plane mask	*/
				  XYPixmap)) == NULL)	/* format	*/
	{
	    error_proc("Can't get image from UIL pixmap");
	}

	if((X_image = XGetImage(dsp,
				X_pixmap,
				0, 0,
				bitmap_width,
				bitmap_height,
				AllPlanes,
				XYPixmap)) == NULL)
	{
	    error_proc("Can't get image from X pixmap");
	}

	strcpy(error_message, null_str);

/*
 * Compare fields of XImage structures and construct error message
 */
	
	errors = 0;

	if(X_image->width != UIL_image->width)
	{
	    errors++;
	    
	    sprintf(err_msg, "width            \t%8d\t%8d\n",
		    X_image->width, UIL_image->width);
	    strcat(error_message, err_msg);
	}
	    
	if(X_image->height != UIL_image->height)
	{
	    errors++;
	    
	    sprintf(err_msg, "height           \t%8d\t%8d\n",
		    X_image->height, UIL_image->height);
	    strcat(error_message, err_msg);
	}
	    
	if(X_image->xoffset != UIL_image->xoffset)
	{
	    errors++;
	    
	    sprintf(err_msg, "xoffset          \t%8d\t%8d\n",
		    X_image->xoffset, UIL_image->xoffset);
	    strcat(error_message, err_msg);
	}
	    
	if(X_image->format != UIL_image->format)
	{
	    errors++;

	    sprintf(err_msg, "format           \t%8s\t%8s\n",
		    format[X_image->format], format[UIL_image->format]);
	    strcat(error_message, err_msg);
	}
	    
	if(X_image->byte_order != UIL_image->byte_order)
	{
	    errors++;
	    
	    sprintf(err_msg, "byte_order       \t%8s\t%8s\n",
		    byte_order[X_image->byte_order],
		    byte_order[UIL_image->byte_order]);
	    strcat(error_message, err_msg);
	}
	    
	if(X_image->bitmap_unit != UIL_image->bitmap_unit)
	{
	    errors++;
	    
	    sprintf(err_msg, "bitmap_unit      \t%8d\t%8d\n",
		    X_image->bitmap_unit, UIL_image->bitmap_unit);
	    strcat(error_message, err_msg);
	}
	    
	if(X_image->bitmap_bit_order != UIL_image->bitmap_bit_order)
	{
	    errors++;
	    
	    sprintf(err_msg, "bitmap_bit_order \t%8d\t%8d\n",
		    X_image->bitmap_bit_order, UIL_image->bitmap_bit_order);
	    strcat(error_message, err_msg);
	}
	    
	if(X_image->bitmap_pad != UIL_image->bitmap_pad)
	{
	    errors++;
	    
	    sprintf(err_msg, "bitmap_pad       \t%8d\t%8d\n",
		    X_image->bitmap_pad, UIL_image->bitmap_pad);
	    strcat(error_message, err_msg);
	}
	    
	if(X_image->depth != UIL_image->depth)
	{
	    errors++;
	    
	    sprintf(err_msg, "depth            \t%8d\t%8d\n",
		    X_image->depth, UIL_image->depth);
	    strcat(error_message, err_msg);
	}
	    
	if(X_image->bytes_per_line != UIL_image->bytes_per_line)
	{
	    errors++;
	    
	    sprintf(err_msg, "bytes_per_line   \t%8d\t%8d\n",
		    X_image->bytes_per_line, UIL_image->bytes_per_line);
	    strcat(error_message, err_msg);
	}
	    
	if(X_image->bits_per_pixel != UIL_image->bits_per_pixel)
	{
	    errors++;
	    
	    sprintf(err_msg, "bits_per_pixel   \t%8d\t%8d\n",
		    X_image->bits_per_pixel, UIL_image->bits_per_pixel);
	    strcat(error_message, err_msg);
	}

	if((X_image->format == ZPixmap) || (UIL_image->format == ZPixmap))
	{
	    if(X_image->red_mask != UIL_image->red_mask)
	    {
		errors++;
	    
		sprintf(err_msg, "red_mask         \t%8u\t%8u\n",
			X_image->red_mask, UIL_image->red_mask);
		strcat(error_message, err_msg);
	    }
	    
	    if(X_image->green_mask != UIL_image->green_mask)
	    {
		errors++;
	    
		sprintf(err_msg, "green_mask       \t%8u\t%8u\n",
			X_image->green_mask, UIL_image->green_mask);
		strcat(error_message, err_msg);
	    }
	    
	    if(X_image->blue_mask != UIL_image->blue_mask)
	    {
		errors++;
	    
		sprintf(err_msg, "blue_mask        \t%8u\t%8u\n",
			X_image->blue_mask, UIL_image->blue_mask);
		strcat(error_message, err_msg);
	    }

	}

	if(X_image->f.create_image != UIL_image->f.create_image)
	{
	    errors++;
	    
	    sprintf(err_msg, "f.create_image() \t   different addresses\n");
	    strcat(error_message, err_msg);
	}

	if(X_image->f.destroy_image != UIL_image->f.destroy_image)
	{
	    errors++;
	    
	    sprintf(err_msg, "f.destroy_image()\t   different addresses\n");
	    strcat(error_message, err_msg);
	}

	if(X_image->f.put_pixel != UIL_image->f.put_pixel)
	{
	    errors++;
	    
	    sprintf(err_msg, "f.put_pixel()    \t   different addresses\n");
	    strcat(error_message, err_msg);
	}

	if(X_image->f.get_pixel != UIL_image->f.get_pixel)
	{
	    errors++;
	    
	    sprintf(err_msg, "f.get_pixel()    \t   different addresses\n");
	    strcat(error_message, err_msg);
	}

	if(X_image->f.sub_image != UIL_image->f.sub_image)
	{
	    errors++;
	    
	    sprintf(err_msg, "f.sub_image()    \t   different addresses\n");
	    strcat(error_message, err_msg);
	}

	if(X_image->f.add_pixel != UIL_image->f.add_pixel)
	{
	    errors++;
	    
	    sprintf(err_msg, "f.add_pixel()    \t   different addresses\n");
	    strcat(error_message, err_msg);
	}

/*
 * Compare bitmap data
 */

	data_errors = 0;
	
	for(j=0 ; j<bitmap_height ; j++)
	{
	    for(k=0 ; k<bitmap_width ; k++)
	    {
		if(XGetPixel(X_image, k, j) != XGetPixel(UIL_image, k, j))
		{
		    data_errors++;
		}
	    }
	}

	if(data_errors)
	{
	    sprintf(err_msg, "\ndata            \t   bitmap data differs\n");
	    strcat(error_message, err_msg);

	    errors++;
	}

/*
 * Print header for test results
 */

    fprintf(stdout,"\n\nPixmap Test Results:\n\n");

/*
 * If there were any errors, print error messages
 */

	fprintf(stdout,"Part %d: XCreatePixmapFromBitmap() Xlib ", i+1);
	fprintf(stdout,"function\n");
	fprintf(stdout,"------            vs\n        ");
	fprintf(stdout,"%s function in UIL.\n\n", which[i]);

	if(errors)
	{
	    total_errors += errors;

	    fprintf(stdout,"\nThe following is a list of the fields of the ");
	    fprintf(stdout,"XImage structure which differ\nbetween pixmap ");
	    fprintf(stdout,"images generated by the CreatePixmapFromBitmap()");
	    fprintf(stdout," Xlib\nfunction and the %s function ", which[i]);
	    fprintf(stdout,"in UIL, both using the same bitmap.\n\n");
	
	    fprintf(stdout, "                \t  Xlib  \t  UIL\n");
	    fprintf(stdout, "Field           \t Value  \t Value\n");
	    fprintf(stdout, "----------------\t--------\t--------\n\n");

	    fprintf(stdout, "%s\n", error_message);

	    fprintf(stdout,"\nPart %d: Test failed with %d error(s).\n\n", i+1,
		    errors);
	}
	else
	{
	    fprintf(stdout,"        Test passed.\n\n");
	}

/*
 * Free pixmaps
 */

	XFreePixmap(dsp, X_pixmap);
	XFreePixmap(dsp, UIL_pixmap);

    }

/*
 * Print final results
 */

    if(total_errors)
    {
	fprintf(stdout, "\nPixmap: test failed with %d error(s).\n\n",
		total_errors);
    }
    else
    {
	fprintf(stdout, "\nPixmap: all tests passed.\n\n");
    }

/*
 * Close Mrm Hierarchy
 */

    MrmCloseHierarchy(s_MrmHierarchy);

}

/*
 * Error handling
 */

void error_proc(error_string)
     String	error_string;
{
    fprintf(stdout,"Mrm error: %s\n", error_string);
    fflush(stdout);
}
