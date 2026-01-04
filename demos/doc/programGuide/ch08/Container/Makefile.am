MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README

bindir  = ${prefix}/share/Xm/ch08/container
datadir = ${prefix}/share/Xm/ch08/container
mandir  = ${prefix}/share/Xm/ch08/container

data_DATA         = $(container_SOURCES) $(EXTRA_DIST)
man_MANS          = container.man
bin_PROGRAMS      = container
container_SOURCES = container.c  interface.c
DEFAULT_INCLUDES  = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                    -I$(top_builddir)/include -I$(top_builddir)/lib
AM_CPPFLAGS       = -I$(top_srcdir)/demos/lib $(X_CFLAGS)
container_LDADD   = $(top_builddir)/demos/lib/Xmd/libXmd.la \
                    $(top_builddir)/lib/Xm/libXm.la

