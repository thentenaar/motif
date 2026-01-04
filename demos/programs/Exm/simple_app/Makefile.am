MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README

bindir  = ${prefix}/share/Xm/Exm/simple_app
datadir = ${prefix}/share/Xm/Exm/simple_app
mandir  = ${prefix}/share/Xm/Exm/simple_app

data_DATA        = simpleDemo.c $(EXTRA_DIST)
man_MANS         = simpleDemo.man
bin_PROGRAMS     = simpleDemo
AM_CPPFLAGS      = -I$(top_srcdir)/demos/lib $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
simpleDemo_LDADD = $(top_builddir)/demos/lib/Exm/libExm.a \
                   $(top_builddir)/lib/Xm/libXm.la

