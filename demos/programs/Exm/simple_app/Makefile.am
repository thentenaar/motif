MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/Exm/simple_app
datadir = ${prefix}/share/Xm/Exm/simple_app

data_DATA = README
man_MANS = simpleDemo.man
bin_PROGRAMS = simpleDemo
simpleDemo_SOURCES = simpleDemo.c

EXTRA_DIST = simpleDemo.man README

AM_CPPFLAGS = -I$(srcdir)/../../../../lib \
           -I$(srcdir)/../../../lib \
           -I$(top_builddir)/lib \
           ${X_CFLAGS}
LDADD = ../../../../lib/Xm/libXm.la \
        ../../../lib/Exm/libExm.a -lX11
