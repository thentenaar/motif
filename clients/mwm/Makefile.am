MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = Mwm.msg $(rc_DATA)

SUBDIRS = WmWsmLib

rcdir = $(MWMRCDIR)

rc_DATA = system.mwmrc

bin_PROGRAMS = mwm
mwm_LDADD = ./WmWsmLib/libWsm.a ../../lib/Xm/libXm.la

AM_CPPFLAGS = -DLARGECURSORS -DR2_COMPAT -DUNMAP_ON_RESTART \
              -DCDE_INSTALLATION_TOP=\"@CDE_INSTALLATION_TOP@\" \
              -DCDE_CONFIGURATION_TOP=\"@CDE_CONFIGURATION_TOP@\" \
              -DBITMAPDIR=\"@INCDIR@/bitmaps\" \
              -DMWMRCDIR=\"$(MWMRCDIR)\" \
              -I$(top_builddir)/lib \
              -I$(srcdir)/../../lib ${X_CFLAGS}

SRCS1=	WmCDInfo.c	WmCDecor.c	WmCEvent.c\
	WmCPlace.c	WmColormap.c	WmError.c\
	WmEvent.c	WmFeedback.c	\
	WmFunction.c	WmGraphics.c	WmIDecor.c\
	WmIPlace.c	WmIconBox.c	\
	WmKeyFocus.c	WmMain.c	WmManage.c\
	WmProperty.c	WmResCvt.c\
	WmResParse.c	WmResource.c	WmSignal.c	            \
	WmWinConf.c	WmWinInfo.c	WmWinList.c	WmWinState.c \
	WmWsm.c		WmXSMP.c	version.c WmCmd.c WmImage.c WmInitWs.c WmMenu.c WmProtocol.c

noinst_HEADERS = WmBitmap.h \
WmCDInfo.h \
WmCDecor.h \
WmCEvent.h \
WmCPlace.h \
WmCmd.h \
WmColormap.h \
WmDebug.h \
WmError.h \
WmEvent.h \
WmFeedback.h \
WmFunction.h \
WmGlobal.h \
WmGraphics.h \
WmIBitmap.h \
WmICCC.h \
WmIDecor.h \
WmIPlace.h \
WmIconBox.h \
WmImage.h \
WmInitWs.h \
WmKeyFocus.h \
WmManage.h \
WmMenu.h \
WmOL.h \
WmProperty.h \
WmProtocol.h \
WmResCvt.h \
WmResNames.h \
WmResParse.h \
WmResource.h \
WmSignal.h \
WmWinConf.h \
WmWinInfo.h \
WmWinList.h \
WmWinState.h \
WmWsm.h \
WmXSMP.h

mwm_SOURCES = $(SRCS1)

if XM_MSGCAT
CAT_FILES = Mwm.cat MwmMsgCatI.h

$(CAT_FILES): Mwm.msg
	$(mkinstalldirs) $(top_builddir)/localized/C/msg
	$(top_builddir)/localized/util/mkcatdefs MwmMsgCatI.h $(srcdir)/Mwm.msg >$(top_builddir)/localized/C/msg/Mwm.msg
	gencat Mwm.cat $(top_builddir)/localized/C/msg/Mwm.msg
endif
