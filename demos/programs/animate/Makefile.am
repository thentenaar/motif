MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/animate
datadir = ${prefix}/share/Xm/animate

EXTRA_DIST = dog.uil plane.uil superman.uil xmanimate.uil \
    superman1.xbm superman2.xbm superman3.xbm superman4.xbm \
    superman5.xbm superman6.xbm XmdAnimate.ad \
    README animate.help animate.man xmanimate.man

xmanimate_SOURCES = xmanimate.c

CLEANFILES =  dog.uid plane.uid superman.uid xmanimate.uid

data_DATA = dog.uid plane.uid superman.uid xmanimate.uid $(xmanimate_SOURCES) \
            $(EXTRA_DIST)

bin_PROGRAMS = xmanimate

LDADD = ../../lib/Xmd/libXmd.la ../../../lib/Mrm/libMrm.la ../../../lib/Xm/libXm.la

INCLUDES = -I$(srcdir)/../../../lib \
           -I$(srcdir)/../../lib \
           -I$(top_builddir)/lib \
           ${X_CFLAGS} 

all: dog.uid plane.uid superman.uid xmanimate.uid

dog.uid: dog.uil
	../../../clients/uil/uil -o $@ $? -I$(srcdir)/../../../clients/uil -I../../../clients/uil

plane.uid: plane.uil
	../../../clients/uil/uil -o $@ $? -I$(srcdir)/../../../clients/uil -I../../../clients/uil

superman.uid: superman.uil
	../../../clients/uil/uil -o $@ $? -I$(srcdir)/../../../clients/uil -I../../../clients/uil

xmanimate.uid: xmanimate.uil
	../../../clients/uil/uil -o $@ $? -I$(srcdir)/../../../clients/uil -I../../../clients/uil

clean-local:
	$(RM) periodic.uid
