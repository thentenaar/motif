MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/earth
datadir = ${prefix}/share/Xm/earth

bin_PROGRAMS = earth
earth_SOURCES = earth.c
EXTRA_DIST = terre.xbm earth.man README
data_DATA = $(EXTRA_DIST) $(earth_SOURCES)
LDADD = ../../../lib/Xm/libXm.la

AM_CPPFLAGS = -I$(srcdir)/../../../lib -I$(top_builddir)/lib ${X_CFLAGS}
