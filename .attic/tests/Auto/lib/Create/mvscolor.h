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
/*   $XConsortium: mvscolor.h /main/7 1995/07/14 11:26:40 drk $ */
#ifndef MVS_COLOR

#define MVS_COLOR

#include <X11/Xproto.h>

#define NUM_GRAYSCALE_COLORS 5
#define BLACK_COLOR_TABLE_INDEX 0
#define WHITE_COLOR_TABLE_INDEX 1
#define INBUF_SIZE              100000
#define OUTBUF_SIZE             10000

#ifndef DECLARE_VISLIB_GLOBALS_HERE

  visGLOBAL MvsColorTableRecord mvsColorTable[];
  visGLOBAL MvsPixmapTableRecord mvsPixmapTable[];

#else

  visGLOBAL MvsColorTableRecord mvsColorTable[] = {

   /* NOTE: Case does not matter to the string-to-pixel conversion function  */
   /*       since it forces all letters to lower case before looking up the  */
   /*       name in the table.  However, compound words matter for .XDefaults*/
   /*       file references. So both forms of string are expected to be in   */
   /*       in the rgb.txt file (which may be generated from this data).     */
   /*       The first letter of each compound word has been capitalized only */
   /*       as a convenience to the readers of this file.                    */

    /* Red     Green   Blue   Name1                 Name2 (for .XDefaults) */
    /*------  ------  ------  -------------------   -----------------------*/

    /* Black and White Part first */

    {      0,      0,      0, "black",              "Black"                  },
    {    255,    255,    255, "white",              "White"                  },

    /* Then Grey-Scale Part */

    {     84,     84,     84, "dim gray",           "DimGray"                },
    {    168,    168,    168, "light gray",         "LightGray"              },
    {    192,    192,    192, "gray",               "Gray"                   },

    /* Then Rest of Colors */

    {    112,    219,    147, "aquamarine",         "Aquamarine"             },
    {      0,      0,    255, "blue",               "Blue"                   },
    {    159,     95,    159, "blue violet",        "BlueViolet"             },
    {    165,     42,     42, "brown",              "Brown"                  },
    {     95,    159,    159, "cadet blue",         "CadetBlue"              },
    {    255,    127,      0, "coral",              "Coral"                  },
    {     66,     66,    111, "cornflower blue",    "CornflowerBlue"         },
    {      0,    255,    255, "cyan",               "Cyan"                   },
    {     47,     79,     47, "dark green",         "DarkGreen"              },
    {     79,     79,     47, "dark olive green",   "DarkOliveGreen"         },
    {    153,     50,    204, "dark orchid",        "DarkOrchid"             },
    {    107,     35,    142, "dark slate blue",    "DarkSlateBlue"          },
    {     47,     79,     79, "dark slate gray",    "DarkSlateGray"          },
    {     47,     79,     79, "dark slate grey",    "DarkSlateGrey"          },
    {    112,    147,    219, "dark turquoise",     "DarkTurquoise"          },
    {    142,     35,     35, "firebrick",          "Firebrick"              },
    {     35,    142,     35, "forest green",       "ForestGreen"            },
    {    204,    127,     50, "gold",               "Gold"                   },
    {    219,    219,    112, "goldenrod",          "Goldenrod"              },
    {      0,    255,      0, "green",              "Green"                  },
    {    147,    219,    112, "green yellow",       "GreenYellow"            },
    {     79,     47,     47, "indian red",         "IndianRed"              },
    {    159,    159,     95, "khaki",              "Khaki"                  },
    {    191,    216,    216, "light blue",         "LightBlue"              },
    {    143,    143,    188, "light steel blue",   "LightSteelBlue"         },
    {     50,    204,     50, "lime green",         "LimeGreen"              },
    {    255,      0,    255, "magenta",            "Magenta"                },
    {    142,     35,    107, "maroon",             "Maroon"                 },
    {     50,    204,    153, "medium aquamarine",  "MediumAquamarine"       },
    {     50,     50,    204, "medium blue",        "MediumBlue"             },
    {    107,    142,     35, "medium forest green","MediumForestGreen"      },
    {    234,    234,    173, "medium goldenrod",   "MediumGoldenrod"        },
    {    147,    112,    219, "medium orchid",      "MediumOrchid"           },
    {     66,    111,     66, "medium sea green",   "MediumSeaGreen"         },
    {    127,    255,      0, "medium spring green","MediumSpringGreen"      },
    {    127,      0,    255, "medium slate blue",  "MediumSlateBlue"        },
    {    219,    112,    147, "medium violet red",  "MediumVioletRed"        },
    {     47,     47,     79, "midnight blue",      "MidnightBlue"           },
    {     35,     35,    142, "navy",               "Navy"                   },
    {     35,     35,    142, "navy blue",          "NavyBlue"               },
    {    204,     50,     50, "orange",             "Orange"                 }, 
    {    255,      0,    127, "orange red",         "OrangeRed"              },
    {    219,    112,    219, "orchid",             "Orchid"                 },
    {    143,    188,    143, "pale green",         "PaleGreen"              },
    {    188,    143,    143, "pink",               "Pink"                   },
    {    234,    173,    234, "plum",               "Plum"                   },
    {    255,      0,      0, "red",                "Red"                    },
    {    111,     66,     66, "salmon",             "Salmon"                 },
    {     35,    142,    107, "sea green",          "SeaGreen"               },
    {    142,    107,     35, "sienna",             "Sienna"                 },
    {     50,    153,    204, "sky blue",           "SkyBlue"                },
    {      0,    127,    255, "slate blue",         "SlateBlue"              },
    {      0,    255,    127, "spring green",       "SpringGreen"            },
    {     35,    107,    142, "steel blue",         "SteelBlue"              },
    {    219,    147,    112, "tan",                "Tan"                    },
    {    216,    191,    216, "thistle",            "Thistle"                },
    {    173,    234,    234, "turquoise",          "Turquoise"              },
    {     79,     47,     79, "violet",             "Violet"                 },
    {    204,     50,    153, "violet red",         "VioletRed"              },
    {    216,    216,    191, "wheat",              "Wheat"                  },
    {    255,    255,      0, "yellow",             "Yellow"                 },
    {    153,    204,     50, "yellow green",       "YellowGreen"            }
};



  visGLOBAL MvsPixmapTableRecord mvsPixmapTable[] = {
   /* NAME           PIXMAP  WIDTH     HEIGHT       PIXMAP (1=black,0=white) */
   /* -------------- ------ --------- ----------- -------------------------- */
    {  "foreground",     0,     1,        1,         { 1 }                },

    {  "background",     0,     1,        1,         { 0 }                },

    {  "25_foreground",  0,     4,        2,         { 0,0,0,1,
                                                       0,1,0,0 }          },

    {  "50_foreground",  0,     2,        2,         { 0,1,
                                                       1,0 }              },

    {  "75_foreground",  0,     4,        2,         { 1,1,1,0,
                                                       1,0,1,1 }          },

    {  "horizontal_tile",0,     1,        2,         { 0,
                                                       1 }                },

    {  "vertical_tile",  0,     2,        1,         { 0, 1 }             },

    {  "slant_right",    0,     4,        4,         { 1,1,1,0,
                                                       1,1,0,1,
                                                       1,0,1,1,
                                                       0,1,1,1 }          },

    {  "slant_left",     0,     4,        4,         { 0,1,1,1,
                                                       1,0,1,1,
                                                       1,1,0,1,
                                                       1,1,1,0 }          }
};
#endif

visGLOBAL int mvsNumTablePixmaps 
                  visINIT(sizeof(mvsPixmapTable) / sizeof(MvsPixmapTableRecord));

visGLOBAL int mvsNumTableColors 
                  visINIT(sizeof(mvsColorTable) / sizeof(MvsColorTableRecord));

visGLOBAL short mvsBigChars
                  visINIT(sizeof(unsigned char) - 1);

#endif /* MVS_COLOR - don't put anything beyond this ! */
