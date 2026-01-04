MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README

bindir  = ${prefix}/share/Xm/ch06/spin_box
datadir = ${prefix}/share/Xm/ch06/spin_box
mandir  = ${prefix}/share/Xm/ch06/spin_box

data_DATA        = $(spin_box_SOURCES) $(EXTRA_DIST)
man_MANS         = spin_box.man
bin_PROGRAMS     = spin_box
spin_box_SOURCES = spin_box.c interface.c
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
AM_CPPFLAGS      = -I$(top_srcdir)/demos/lib $(X_CFLAGS)
spin_box_LDADD   = $(top_builddir)/demos/lib/Xmd/libXmd.la \
                   $(top_builddir)/lib/Xm/libXm.la

