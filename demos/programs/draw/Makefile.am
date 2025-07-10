MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/draw
datadir = ${prefix}/share/Xm/draw

bin_PROGRAMS = draw
draw_SOURCES = draw.c
EXTRA_DIST = XmdDraw.ad README draw.help draw.man
data_DATA = $(EXTRA_DIST) $(draw_SOURCES)
LDADD = ../../lib/Xmd/libXmd.la ../../../lib/Xm/libXm.la

AM_CPPFLAGS = -I$(srcdir)/../../lib -I$(srcdir)/../../../lib -I$(top_builddir)/lib ${X_CFLAGS}
