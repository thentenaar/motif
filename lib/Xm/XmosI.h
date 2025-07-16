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
 *
 */
/*
 * HISTORY
 */
/* $XConsortium: XmosI.h /main/6 1995/07/13 18:28:56 drk $ */
#ifndef _XmosI_h
#define _XmosI_h

#include <stddef.h>
#include <Xm/XmosP.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Vendor dependent macro for XmCvtXmStringToCT */
/* Sample implementation treats unmapped charsets as locale encoded text. */
#define _XmOSProcessUnmappedCharsetAndText(tag, ctext, sep, outc, outl, prev) \
	processCharsetAndText(XmFONTLIST_DEFAULT_TAG, (ctext), (sep), \
			      (outc), (outl), (prev))


/********    Private Function Declarations    ********/

extern String _XmOSFindPatternPart(
                        String fileSpec) ;
extern void _XmOSQualifyFileSpec(
                        String dirSpec,
                        String filterSpec,
                        String *pQualifiedDir,
                        String *pQualifiedPattern) ;
extern void _XmOSGetDirEntries(
                        String qualifiedDir,
                        String matchPattern,
                        unsigned char fileType,
                        Boolean matchDotsLiterally,
                        Boolean listWithFullPath,
                        String **pEntries,
                        unsigned int *pNumEntries,
                        unsigned int *pNumAlloc) ;
extern void _XmOSBuildFileList(
                        String dirPath,
                        String pattern,
                        unsigned char typeMask,
                        String **pEntries,
                        unsigned int *pNumEntries,
                        unsigned int *pNumAlloc) ;
extern int _XmOSFileCompare(
                        XmConst void *sp1,
                        XmConst void *sp2) ;
extern String _XmOSInitPath(
                        String file_name,
                        String env_pathname,
                        Boolean *user_path) ;
extern String _XmOSBuildFileName(
			String file,
			String path) ;
extern int _XmOSPutenv(
		       char *string);
extern void _XmOSGenerateMaskName(
				  String imageName,
				  String maskNameBuf,
				  size_t buf_len);

extern Status _XmOSGetInitialCharsDirection(XtPointer     characters,
					    XmTextType    type,
					    XmStringTag   locale,
					    unsigned int *num_bytes,
					    XmDirection  *direction) ;

extern XmDirection _XmOSGetCharDirection(XtPointer   character,
					 XmTextType  type,
					 XmStringTag locale) ;

extern int _XmOSKeySymToCharacter(KeySym keysym,
				  char	 *locale,
				  char	 *buffer);
extern void _XmOSFindPathParts(String path,
			       String *filenameRtn,
			       String *suffixRtn);
extern Boolean _XmOSAbsolutePathName(
                        String path,
                        String *pathRtn,
                        String buf) ;

/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmosI_h */
