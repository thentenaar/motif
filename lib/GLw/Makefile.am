
MAINTAINERCLEANFILES = Makefile.in glw.pc
AM_CPPFLAGS          = -I.. $(GL_CFLAGS) $(XFT_CFLAGS) $(X_CFLAGS)
libGLw_la_LDFLAGS    = -version-info @GLW_VERSION@ -no-undefined
libGLw_la_LIBADD     = ../Xm/libXm.la $(GL_LIBS) $(XFT_LIBS) $(X_LIBS)
libGLw_la_SOURCES    = GLwDrawA.c GLwMDrawA.c
lib_LTLIBRARIES      = libGLw.la
pkgconfig_DATA       = glw.pc
glwdir               = $(includedir)/GLw
glw_HEADERS          = GLwDrawA.h GLwMDrawA.h GLwDrawAP.h GLwMDrawAP.h

