MAINTAINERCLEANFILES = Makefile.in

noinst_HEADERS   = wsm_funcs.h wsm_proto.h utm_send.h
noinst_LIBRARIES = libWsm.a
libWsm_a_SOURCES = debug.c disp.c free.c pack.c recv.c send.c util.c utm_send.c
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib

