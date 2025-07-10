MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/tooltips
datadir = ${prefix}/share/Xm/tooltips

EXTRA_DIST =
bin_PROGRAMS = tooltips
tooltips_SOURCES = tooltips.c
data_DATA = $(tooltips_SOURCES) $(EXTRA_DIST)
LDADD = ../../../lib/Xm/libXm.la

AM_CPPFLAGS = -I$(srcdir)/../../../lib -I$(top_builddir)/lib ${X_CFLAGS}
