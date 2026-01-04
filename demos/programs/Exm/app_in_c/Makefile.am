MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = exm_in_c.man

bindir  = ${prefix}/share/Xm/Exm/app_in_c
datadir = ${prefix}/share/Xm/Exm/app_in_c
mandir  = ${prefix}/share/Xm/Exm/app_in_c

man_MANS         = exm_in_c.man
bin_PROGRAMS     = exm_in_c
AM_CPPFLAGS      = -I$(top_srcdir)/demos/lib $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
exm_in_c_LDADD   = $(top_builddir)/demos/lib/Exm/libExm.a \
                   $(top_builddir)/lib/Xm/libXm.la

