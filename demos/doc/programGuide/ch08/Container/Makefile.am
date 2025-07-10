MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = README container.man

noinst_PROGRAMS = container

container_SOURCES = container.c  interface.c

AM_CPPFLAGS = -I$(top_builddir)/lib -I$(top_srcdir)/lib -I$(top_srcdir)/demos/lib $(X_CFLAGS)

LDADD = $(top_builddir)/demos/lib/Xmd/libXmd.la $(top_builddir)/lib/Xm/libXm.la
