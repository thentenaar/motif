MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/getsubres
datadir = ${prefix}/share/Xm/getsubres

bin_PROGRAMS = getsubres
EXTRA_DIST = getsubres.help getsubres.man README
getsubres_SOURCES = getsubres.c
data_DATA = $(EXTRA_DIST) $(getsubres_SOURCES)
LDADD = ../../lib/Xmd/libXmd.la ../../../lib/Xm/libXm.la
INCLUDES = -I$(srcdir)/../../lib -I$(srcdir)/../../../lib -I$(top_builddir)/lib ${X_CFLAGS}
