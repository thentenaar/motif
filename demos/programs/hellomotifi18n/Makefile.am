MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/helloint
datadir = ${prefix}/share/Xm/helloint

SUBDIRS = C english french hebrew japan japanese swedish
EXTRA_DIST = README helloint.uil helloint.man
CLEANFILES = helloint.uid
helloint_SOURCES = helloint.c
bin_PROGRAMS = helloint
data_DATA = $(EXTRA_DIST) $(helloint_SOURCES) helloint.uid
helloint_DEPENDENCIES = helloint.uid C/uid/l_strings.uid english/uid/l_strings.uid french/uid/l_strings.uid hebrew/uid/l_strings.uid japan/uid/l_strings.uid japanese/uid/l_strings.uid swedish/uid/l_strings.uid

AM_CPPFLAGS = -I$(top_srcdir)/lib -I$(top_builddir)/lib ${X_CFLAGS}
LDADD = $(top_builddir)/lib/Mrm/libMrm.la $(top_builddir)/lib/Xm/libXm.la

SUFFIXES = .uil .uid

.uil.uid:
	$(top_builddir)/clients/uil/uil $< -I$(srcdir) -o $@
