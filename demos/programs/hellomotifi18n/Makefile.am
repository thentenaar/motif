MAINTAINERCLEANFILES = Makefile.in
CLEANFILES           = helloint.uid C/uid/l_strings.uid \
                       english/uid/l_strings.uid  \
                       french/uid/l_strings.uid   \
                       hebrew/uid/l_strings.uid   \
                       japan/uid/l_strings.uid    \
                       japanese/uid/l_strings.uid \
                       swedish/uid/l_strings.uid
EXTRA_DIST           = README helloint.uil
SUBDIRS              = C english french hebrew japan japanese swedish

bindir  = ${prefix}/share/Xm/helloint
datadir = ${prefix}/share/Xm/helloint
mandir  = ${prefix}/share/Xm/helloint

data_DATA             = $(helloint_SOURCES) $(helloint_DEPENDENCIES) $(EXTRA_DIST)
man_MANS              = helloint.man
bin_PROGRAMS          = helloint
helloint_DEPENDENCIES = helloint.uid C/uid/l_strings.uid \
                        english/uid/l_strings.uid  \
                        french/uid/l_strings.uid   \
                        hebrew/uid/l_strings.uid   \
                        japan/uid/l_strings.uid    \
                        japanese/uid/l_strings.uid \
                        swedish/uid/l_strings.uid
AM_CPPFLAGS           = $(X_CFLAGS)
DEFAULT_INCLUDES      = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                        -I$(top_builddir)/include -I$(top_builddir)/lib
helloint_LDADD        = $(top_builddir)/lib/Mrm/libMrm.la \
                        $(top_builddir)/lib/Xm/libXm.la

.uil.uid:
	$(top_builddir)/clients/uil/uil -I$(top_srcdir)/clients/uil -I$(top_builddir)/clients/uil -o $@ $<

SUFFIXES = .uil .uid

