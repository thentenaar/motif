MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = README combo_box.man

noinst_PROGRAMS = combo_box

combo_box_SOURCES = combo_box.c interface.c

AM_CPPFLAGS = -I$(top_builddir)/lib -I$(top_srcdir)/lib -I$(top_srcdir)/demos/lib $(X_CFLAGS)

LDADD = $(top_builddir)/demos/lib/Xmd/libXmd.la $(top_builddir)/lib/Xm/libXm.la
