datadir = ${prefix}/share/Xm/doc

EXTRA_DIST = LICENSE

MAINTAINERCLEANFILES = Makefile.in \
             configure libtool \
             config.guess \
             config.sub \
             config.log \
             ltconfig ltmain.sh \
             aclocal.m4 \
             config.h.in \
             install-sh \
             missing mkinstalldirs \
             compile \
             motif.pc

pkgconfig_DATA  = motif.pc
ACLOCAL_AMFLAGS = -I m4

SUBDIRS	= config bindings bitmaps doc icons localized lib include tools clients

if WITH_DEMOS
SUBDIRS += demos
endif

if WITH_TESTS
SUBDIRS += tests
endif
