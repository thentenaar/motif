MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = exm_in_uil.uil
CLEANFILES           = exm_in_uil.uid

bindir  = ${prefix}/share/Xm/Exm/app_in_uil
datadir = ${prefix}/share/Xm/Exm/app_in_uil

data_DATA               = exm_in_uil.c exm_in_uil.uil exm_in_uil.uid
bin_PROGRAMS            = exm_in_uil
exm_in_uil_DEPENDENCIES = exm_in_uil.uid
AM_CPPFLAGS             = -I$(top_srcdir)/demos/lib $(X_CFLAGS)
DEFAULT_INCLUDES        = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                          -I$(top_builddir)/include -I$(top_builddir)/lib
exm_in_uil_LDADD        = $(top_builddir)/demos/lib/Exm/libExmMrm.a \
                          $(top_builddir)/demos/lib/Exm/libExm.a \
				          $(top_builddir)/lib/Mrm/libMrm.la \
                          $(top_builddir)/lib/Xm/libXm.la

.uil.uid:
	$(top_builddir)/clients/uil/uil -wmd $(top_builddir)/demos/lib/Exm/wml/Exm.wmd -I$(srcdir) -o $@ $<

SUFFIXES = .uil .uid

