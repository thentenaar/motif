MAINTAINERCLEANFILES = Makefile.in Mrm.pc
CLEANFILES = $(CAT_FILES)
EXTRA_DIST = Mrm.msg
DEFAULT_INCLUDES  = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                    -I$(top_builddir)/include -I$(top_builddir)/lib
AM_CPPFLAGS       = -I$(top_srcdir)/lib/Xm -I$(top_builddir)/lib/Xm \
                    $(X_CFLAGS) $(XFT_CFLAGS)
libMrm_la_LDFLAGS = -version-info @ABI_VERSION@
libMrm_la_LIBADD  = ../Xm/libXm.la $(XFT_LIBS) ${X_LIBS}
libMrm_la_SOURCES = $(SRCS)
lib_LTLIBRARIES   = libMrm.la

mrmdir            = $(includedir)/Mrm
mrm_HEADERS       = MrmDecls.h MrmPublic.h MrmAppl.h MrmosI.h
pkgconfig_DATA    = Mrm.pc
noinst_HEADERS    = IDB.h Mrm.h MrmMsgI.h MrmWidget.h

SRCS =	MrmIbuffer.c	MrmIentry.c	MrmIfile.c	MrmIheader.c \
	MrmIindex.c	MrmIindexw.c	MrmIrid.c	MrmIswap.c \
	MrmItop.c	MrmItopw.c	Mrmappl.c \
	Mrmcontext.c	Mrmerror.c	Mrmhier.c	Mrmicon.c \
	Mrminit.c	Mrmlread.c	Mrmlwrite.c	Mrmmodule.c \
	Mrmos.c \
	Mrmptrlist.c	Mrmtable.c	Mrmtime.c	Mrmvm.c \
	Mrmwci.c	Mrmwcrw.c	Mrmwcrwr.c	Mrmwread.c \
	Mrmwrefs.c	Mrmwvalues.c	Mrmwwrite.c	MrmMessages.c


if XM_MSGCAT
CAT_FILES = Mrm.cat MrmMsgCatI.h

$(CAT_FILES): Mrm.msg
	$(mkinstalldirs) $(top_builddir)/localized/C/msg
	$(top_builddir)/localized/util/mkcatdefs MrmMsgCatI.h $(srcdir)/Mrm.msg >$(top_builddir)/localized/C/msg/Mrm.msg
	gencat Mrm.cat $(top_builddir)/localized/C/msg/Mrm.msg
endif

