MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = README simple_drop.man

noinst_PROGRAMS = simple_drop

simple_drop_SOURCES = simple_drop.c 


INCLUDES = -I$(top_builddir)/lib -I$(top_srcdir)/lib -I$(top_srcdir)/demos/lib $(X_CFLAGS)

LDADD = $(top_builddir)/demos/lib/Xmd/libXmd.la $(top_builddir)/lib/Xm/libXm.la
