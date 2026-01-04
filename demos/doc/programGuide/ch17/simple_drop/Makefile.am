MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README

bindir  = ${prefix}/share/Xm/ch17/simple_drop
datadir = ${prefix}/share/Xm/ch17/simple_drop
mandir  = ${prefix}/share/Xm/ch17/simple_drop

data_DATA         = simple_drop.c $(EXTRA_DIST)
man_MANS          = simple_drop.man
bin_PROGRAMS      = simple_drop
DEFAULT_INCLUDES  = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                    -I$(top_builddir)/include -I$(top_builddir)/lib
AM_CPPFLAGS       = -I$(top_srcdir)/demos/lib $(X_CFLAGS)
simple_drop_LDADD = $(top_builddir)/demos/lib/Xmd/libXmd.la \
                    $(top_builddir)/lib/Xm/libXm.la

