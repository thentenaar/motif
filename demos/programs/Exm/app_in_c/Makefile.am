MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/Exm/app_in_c
datadir = ${prefix}/share/Xm/Exm/app_in_c

man_MANS = exm_in_c.man
bin_PROGRAMS = exm_in_c
exm_in_c_SOURCES = exm_in_c.c

EXTRA_DIST = exm_in_c.man

AM_CPPFLAGS = -I$(srcdir)/../../../../lib \
           -I$(srcdir)/../../../lib \
           -I$(top_builddir)/lib \
            ${X_CFLAGS}
LDADD = ../../../lib/Exm/libExm.a \
	../../../../lib/Xm/libXm.la
