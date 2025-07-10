MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/setDate
datadir = ${prefix}/share/Xm/setDate

bin_PROGRAMS = setDate
EXTRA_DIST = XmdSetDate.ad README setDate.man
setDate_SOURCES = setDate.c
data_DATA = $(EXTRA_DIST) $(setDate_SOURCES)
LDADD = ../../../lib/Xm/libXm.la

AM_CPPFLAGS = -I$(srcdir)/../../../lib -I$(top_builddir)/lib ${X_CFLAGS}
