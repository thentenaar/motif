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

#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>

#include <Xm/Xm.h>
#include <Mrm/MrmPublic.h>
#include <Xm/MainW.h>
#include <Xm/Label.h>
#include <Xm/Frame.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/Separator.h>
#include <Xm/Form.h>
#include <Xm/CascadeB.h>
#include <Xm/MessageB.h>
#include <Xm/PanedW.h>
#include <Xm/SelectioB.h>
#include <Xm/FileSB.h>
#include <Xm/Text.h>
/*
#include <Xm/TextF.h>
*/

#define main_h

#define DECLAREGLOBAL
#include "fileview.h"
#undef DECLAREGLOBAL

#include "mainE.h"
#include "textE.h"

/*
 * Local variables
 */
#define UIL_FILE_COUNT 1

static char *uid_files[UIL_FILE_COUNT] = {NULL};

static MrmHierarchy theUIDdatabase = NULL;  /* MRM database hierarchy id */
/*
static MRMRegisterArg regvec[] = {
        {"exit_proc", (caddr_t) ExitCallback},
};

static MrmCount regnum = sizeof(regvec) / sizeof(MRMRegisterArg);
*/

typedef enum 
	{AIX=0, DGUX=1, HPUX=2, IRIX=3, IRIX64=4, OSF1=5, 
	   Solaris=6, SunOS=7, SonyNews=8, SysV=9, 
	   UXP=10, 
	   MaxOS=11 }
	OS;	

static String OSList[] = {
/* AIX */		"AIX",
/* DGUX */		"dgux",
/* HPUX */		"HP-UX",
/* SGI IRIX */		"IRIX",
/* SGI IRIX64 */	"IRIX64",
/* DEC OSF1 */		"OSF1",
/* Solaris */		"Solaris",
/* SunOS */		"SunOS",
/* Sony NEWS */		"NEWS-OS",
/* Novell SysV */	"UNIX_SV",
/* Fujitsu UXP */	"UNIX_System_V",
  		"Other"
};

static OS CurrentOS;

typedef enum {C=0,
		English=1, French=2, German=3, Spanish=4, 
		Hebrew=5,
		Japanese=6, Korean=7,
		Russian=8,
		MaxLanguage=9}
	Language;	

static String LangList[] = {
   "C locale",
   "English",
   "French",
   "German",
   "Spanish",
   "Hebrew",
   "Japanese",
   "Korean",
   "Russian",
   "Javanais"
};

static Widget locale_dialog = NULL;
static Widget locale_box;
static Widget os_box = NULL;
static String LocaleName = NULL;
static Language SelectedLocale;

static Boolean SupportedLocale [MaxOS+1][MaxLanguage] =
{
/* 		 C    Englis French German Spanis Hebrew Japan  Korea  Russi */
{/* AIX */	True, True,  True,  True,  True,  True,  True,  True,  True},
{/* DGUX */	True, True,  True,  True,  True,  False, True,  True,  True},
{/* HPUX */	True, True,  True,  True,  True,  True,  True,  True,  True},
{/* IRIX */	True, True,  True,  True,  True,  True,  True,  True,  True},
{/* IRIX64 */ 	True, True,  True,  True,  True,  True,  True,  True,  True},
{/* OSF1 */ 	True, True,  True,  True,  True,  True,  True,  True,  True},
{/* Solaris */	True, True,  True,  True,  True,  True,  True,  True,  True},
{/* SunOS */ 	True, True,  True,  True,  True,  True,  True,  True,  True},
{/* Sony News */	True, True,  False, False, False, False, True,  True,  False},
{/* USL Sys V */	True, True,  True,  True,  True,  False, True,  False, False},
{/* Fujitsu */	True, False, False, False, False, True,  False, False, False},
{/* dummy */     True, False, False, False, False, False, False, False, False}
};

static String LocaleTable [MaxOS+1][MaxLanguage] =
{
{/* AIX */	"C", "en_US.8859-1", "fr_FR.8859-1", "gr_DE.8859-1",
		"iw_IL.ISO8859-8", "jp_JP.euc", "kr_KR", 
		"ru_RU.ISO8859-2", "es_ES.ISO8859-1"},
{/* DGUX */	"C", "en_US.850", "fr_FR.850", "de_DE.850", "es_ES.850",
		NULL,
		"ja_JP.euc", "ko_KR.euc", "ru_RU.8859-5"},
{/* HPUX */	"C", "american", "french", "german",  "spanish",
  		"hebrew.iso88598",
		"japanese", "korean", "russian"},
{/* IRIX */	"C", "en_US.8859-1", "fr_FR.8859-1", "gr_DE.8859-1",
		"iw_IL.ISO8859-8", "jp_JP.euc", "kr_KR",
		"ru_RU.ISO8859-2", "es_ES.ISO8859-1"},
{/* IRIX64 */ 	"C", "en_US.8859-1", "fr_FR.8859-1", "gr_DE.8859-1", 
		"iw_IL.ISO8859-8", "jp_JP.euc", "kr_KR", 
		"ru_RU.ISO8859-2", "es_ES.ISO8859-1"},
{/* OSF1 */ 	"C", "en_US.8859-1", "fr_FR.8859-1",
  		"de_DE.8859-1", "es_ES.ISO8859-1",
		"iw_IL.ISO8859-8",
		"jp_JP.SJIS", "kr_KR.eucKR", 
		"ru_RU.ISO8859-2"},
{/* Solaris */	"C", "en_US.8859-1", "fr_FR.8859-1", "gr_DE.8859-1",
		"iw_IL.ISO8859-8", "jp_JP.euc", "kr_KR", 
		"ru_RU.ISO8859-2", "es_ES.ISO8859-1"},
{/* SunOS */ 	"C", "en_US.8859-1", "fr_FR.8859-1", "gr_DE.8859-1",
		"iw_IL.ISO8859-8", "jp_JP.euc", "kr_KR", 
		"ru_RU.ISO8859-2", "es_ES.ISO8859-1"},
{/* Sony NEWS */"C", "en_US.88591", NULL, NULL,	NULL,
		"jp_JP.EUC", "kr_KR.EUC", 
		NULL, NULL},
{/* Novell */	"C", "en_US", "fr", "de", "es", NULL, "ja", NULL, NULL},
{/* Fujitsu */	"C", "C", NULL, NULL, NULL, NULL, "ja_JP", NULL, NULL},
{/* dummy */     "C", "", "", "", "", "", "", "", ""}
};

/* 
 * Application Resources
 */

global struct _FileViewResources {
   String version;
   int bell_duration;
   int bell_percent;
} FileViewResources;

static XtResource FileViewResourcesSpecs[] =
{
   { "checkVersion", XtCString, XtRString, sizeof(String),
       XtOffsetOf( struct _FileViewResources, version), XtRImmediate, 
       (XtPointer) "NoValue" },
   {
      "bellDuration", XmCMaxValue, XmRInt, sizeof(int),
      XtOffsetOf( struct _FileViewResources, bell_duration),
      XmRImmediate, (XtPointer)  100
   },
   {
      "bellPercent", XmCMaxValue, XmRInt, sizeof(int),
      XtOffsetOf( struct _FileViewResources, bell_percent),
      XmRImmediate, (XtPointer) 100
   }
};

static char myClass[] = "Fileview";
static char myName[] = "fileview" ;
static int save_argc;
static char ** save_argv;

static String MyLanguageProc(Display * dpy, String xnl,
			     XtAppContext theContext);

static ViewPtr NewFileShell(Widget parent, ViewPtr this,
			   int argc, char *argv[]);

static Widget CreateMenuBarEntry(Widget menubar, String entry, String names[],
		XtCallbackProc procs[], XtPointer private[], int count);

static void ExitCallback(Widget button, Widget root,
			 XmPushButtonCallbackStruct *call_data);

static void CloseCallback(Widget button, ViewPtr this,
			 XmPushButtonCallbackStruct *call_data);

static void HelpCallback(Widget	widget, ViewPtr this,
			 XmPushButtonCallbackStruct *call_data);

static void OpenNewShellCallback(Widget widget, ViewPtr this,
			 XmPushButtonCallbackStruct *call_data);

static void OpenFileCallback(Widget widget, ViewPtr this,
			     XmPushButtonCallbackStruct *call_data);

static Widget CreateFileSelectionBox(ViewPtr this);

static void FileCancelCallback(Widget fsb, ViewPtr this,
			XmFileSelectionBoxCallbackStruct *call_data);


#undef main_h
