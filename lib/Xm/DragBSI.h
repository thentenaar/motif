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

#ifndef _XmDragBSI_h
#define _XmDragBSI_h

#include <Xm/XmP.h>
#include <X11/Xmd.h>		/* for CARD32, B32, etc. */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * defalut values for XmNnumImportTargets and XmNimportTargets. 
 */
#define _XmDefaultNumImportTargets	0;
#define _XmDefaultImportTargets		NULL;


/*
 *  atoms and targets table structures
 */

typedef struct {
  Atom		atom;
  Time		time;
} xmAtomsTableEntryRec, *xmAtomsTableEntry;

typedef struct {
  Cardinal	numEntries;
  xmAtomsTableEntry entries;
} xmAtomsTableRec, *xmAtomsTable;

typedef struct {
    Cardinal	numTargets;
    Atom	*targets;
} xmTargetsTableEntryRec, *xmTargetsTableEntry;

typedef struct {
    Cardinal	numEntries;
    xmTargetsTableEntry entries;
} xmTargetsTableRec, *xmTargetsTable;

/*
 *  The following are structures for property access.
 *  They must have 64-bit multiple lengths to support 64-bit architectures.
 */

typedef struct {
    CARD32	atom B32;
    CARD16	name_length B16;
    CARD16	pad B16;
} xmMotifAtomPairRec;

typedef struct {
    BYTE	byte_order;
    BYTE	protocol_version;
    CARD16	num_atom_pairs B16;
    CARD32	heap_offset B32;
    /* xmMotifAtomPairRec 	 atomPairs[];	*/
} xmMotifAtomPairPropertyRec;

typedef struct {
    CARD32	atom B32;
    CARD32	time B32;
} xmMotifAtomsTableRec;

typedef struct {
    BYTE	byte_order;
    BYTE	protocol_version;
    CARD16	num_atoms B16;
    CARD32	heap_offset B32;
    /* xmMotifAtomsTableRec atoms[]; 	*/
} xmMotifAtomsPropertyRec;

typedef struct {
    BYTE	byte_order;
    BYTE	protocol_version;
    CARD16	num_target_lists B16;
    CARD32	heap_offset B32;
} xmMotifTargetsPropertyRec;

/********    Private Function Declarations for DragBS.c   ********/

extern void _XmInitTargetsTable( 
                        Display *display) ;
extern void _XmClearDisplayTables (Display *display);
extern Cardinal _XmIndexToTargets( 
                        Widget shell,
                        Cardinal t_index,
                        Atom **targetsRtn) ;
extern Cardinal _XmTargetsToIndex( 
                        Widget shell,
                        Atom *targets,
                        Cardinal numTargets) ;
extern Atom _XmAllocMotifAtom( 
                        Widget shell,
                        Time time) ;
extern void _XmFreeMotifAtom( 
                        Widget shell,
                        Atom atom) ;
extern void _XmDestroyMotifWindow( 
                        Display *dpy) ;
extern Window _XmGetDragProxyWindow(
			Display *display) ;

/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmDragBSI_h */
