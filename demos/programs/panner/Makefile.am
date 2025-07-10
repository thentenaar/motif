MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/panner
datadir = ${prefix}/share/Xm/panner

bin_PROGRAMS = panner
panner_SOURCES = panner.c
EXTRA_DIST = README panner.man
data_DATA = $(EXTRA_DIST) $(panner_SOURCES)
LDADD = ../../../lib/Xm/libXm.la

AM_CPPFLAGS = -I$(srcdir)/../../../lib -I$(top_builddir)/lib ${X_CFLAGS}
