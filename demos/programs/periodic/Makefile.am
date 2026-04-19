MAINTAINERCLEANFILES = Makefile.in
CLEANFILES           = periodic.uid
EXTRA_DIST           = README periodic.uil periodic_l.uil XmdPeriodic.ad arrow

bindir  = ${prefix}/share/motif/demos/periodic
datadir = ${prefix}/share/motif/demos/periodic
mandir  = ${prefix}/share/motif/demos/periodic

data_DATA             = $(periodic_SOURCES) $(periodic_DEPENDENCIES) $(EXTRA_DIST)
man_MANS              = periodic.man
bin_PROGRAMS          = periodic
periodic_DEPENDENCIES = periodic.uid
AM_CPPFLAGS           = $(X_CFLAGS)
DEFAULT_INCLUDES      = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                        -I$(top_builddir)/include -I$(top_builddir)/lib
periodic_LDADD        = $(top_builddir)/lib/Mrm/libMrm.la \
                        $(top_builddir)/lib/Xm/libXm.la $(X_LIBS)

.uil.uid:
	$(top_builddir)/clients/uil/uil -I$(srcdir) -I$(top_srcdir)/clients/uil -I$(top_builddir)/clients/uil -o $@ $<

SUFFIXES = .uil .uid

