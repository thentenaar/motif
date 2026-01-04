MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README

bindir  = ${prefix}/share/Xm/ch06/combo_box
datadir = ${prefix}/share/Xm/ch06/combo_box
mandir  = $(prefix)/share/Xm/ch06/combo_box

data_DATA         = $(combo_box_SOURCES) $(EXTRA_DIST)
man_MANS          = combo_box.man
bin_PROGRAMS      = combo_box
combo_box_SOURCES = combo_box.c interface.c
DEFAULT_INCLUDES  = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                    -I$(top_builddir)/include -I$(top_builddir)/lib
AM_CPPFLAGS       = -I$(top_srcdir)/demos/lib $(X_CFLAGS)
combo_box_LDADD   = $(top_builddir)/demos/lib/Xmd/libXmd.la \
                    $(top_builddir)/lib/Xm/libXm.la

