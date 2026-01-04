MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README XmdDraw.ad draw.help

bindir  = ${prefix}/share/Xm/draw
datadir = ${prefix}/share/Xm/draw
mandir  = ${prefix}/share/Xm/draw

data_DATA        = $(draw_SOURCES) $(EXTRA_DIST)
man_MANS         = draw.man
bin_PROGRAMS     = draw
AM_CPPFLAGS      = -I$(top_srcdir)/demos/lib $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
draw_LDADD       = $(top_builddir)/demos/lib/Xmd/libXmd.la \
                   $(top_builddir)/lib/Xm/libXm.la

