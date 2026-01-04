MAINTAINERCLEANFILES = Makefile.in
CLEANFILES           = English.uid French.uid German.uid
EXTRA_DIST           = README English.df French.df German.df Fileview.ad \
                       English.uil French.uil German.uil

bindir  = ${prefix}/share/Xm/fileview
datadir = ${prefix}/share/Xm/fileview

noinst_HEADERS        = fileview.h file.h fileE.h main.h mainE.h text.h textE.h
data_DATA             = $(fileview_SOURCES) $(EXTRA_DIST)
bin_PROGRAMS          = fileview
fileview_SOURCES      = main.c text.c file.c
fileview_DEPENDENCIES = English.uid French.uid German.uid
AM_CPPFLAGS           = $(X_CFLAGS)
DEFAULT_INCLUDES      = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                        -I$(top_builddir)/include -I$(top_builddir)/lib
fileview_LDADD        = $(top_builddir)/lib/Mrm/libMrm.la \
                        $(top_builddir)/lib/Xm/libXm.la

.uil.uid:
	$(top_builddir)/clients/uil/uil -I$(top_srcdir)/clients/uil -I$(top_builddir)/clients/uil -o $@ $<

SUFFIXES = .uil .uid

