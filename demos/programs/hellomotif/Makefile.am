MAINTAINERCLEANFILES = Makefile.in
CLEANFILES           = hellomotif.uid
EXTRA_DIST           = README hellomotif.uil hellomotif.uid

bindir  = ${prefix}/share/Xm/hellomotif
datadir = ${prefix}/share/Xm/hellomotif

data_DATA               = $(hellomotif_SOURCES) $(EXTRA_DIST)
bin_PROGRAMS            = hellomotif
hellomotif_DEPENDENCIES = hellomotif.uid
AM_CPPFLAGS             = $(X_CFLAGS)
DEFAULT_INCLUDES        = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                          -I$(top_builddir)/include -I$(top_builddir)/lib
hellomotif_LDADD        = $(top_builddir)/lib/Mrm/libMrm.la \
                          $(top_builddir)/lib/Xm/libXm.la

.uil.uid:
	$(top_builddir)/clients/uil/uil -I$(top_srcdir)/clients/uil -I$(top_builddir)/clients/uil -o $@ $<

SUFFIXES = .uil .uid

