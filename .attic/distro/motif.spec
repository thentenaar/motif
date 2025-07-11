%define _unpackaged_files_terminate_build 0
%define intern_name motif

Summary: Motif runtime libraries and executables.
Name: motif
Version: 2.3.5
Release: 1
License: LGPL
Group: System Environment/Libraries
Source:  ftp://ftp.ics.com/pub/Products/Motif/%{intern_name}-%{version}.tar.gz
Source1: xmbind
URL: http://www.motifzone.org/
BuildRoot: %{_tmppath}/%{name}-%{version}-root

BuildPrereq: flex
BuildPrereq: /usr/bin/yacc

Conflicts: lesstif <= 0.92.32-6

Prefix: /usr

%description
This is the Motif %{version} runtime environment. It includes the
Motif shared libraries, needed to run applications which are dynamically
linked against Motif, and the Motif Window Manager "mwm".

%package devel
Summary: Motif development libraries and header files.
Group: Development/Libraries
Conflicts: lesstif-devel <= 0.92.32-6
Requires: motif = %{version}-%{release}

%description devel
This is the Motif %{version} development environment. It includes the
static libraries and header files necessary to build Motif applications.

%package demos
Summary: Motif example code and demo programs.
Group: Development/Libraries
Requires: motif = %{version}-%{release}

%description demos
This is the Motif %{version} example code and demo programs.


%prep
%setup -q -n %{intern_name}-%{version}

%build
#cp /usr/share/libtool/config.* .
CFLAGS="$RPM_OPT_FLAGS -D_FILE_OFFSET_BITS=64" \
./configure \
   --prefix=%{prefix} \
   --libdir=%{prefix}/%{_lib} \
   --enable-static

make clean
make

%install
rm -rf $RPM_BUILD_ROOT

make DESTDIR=$RPM_BUILD_ROOT prefix=%{prefix} install
mkdir -p $RPM_BUILD_ROOT/etc/X11/xinit/xinitrc.d \
         $RPM_BUILD_ROOT/usr/include

install -m 755 %{SOURCE1} $RPM_BUILD_ROOT/etc/X11/xinit/xinitrc.d/

#pushd $RPM_BUILD_ROOT/usr/include/
#ln -s ../X11R6/include/Xm .
#ln -s ../X11R6/include/Mrm .
#ln -s ../X11R6/include/uil .
#popd

rm -fr $RPM_BUILD_ROOT%{prefix}/%{_lib}/*.la \
       $RPM_BUILD_ROOT%{prefix}/share/Xm/doc

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc COPYRIGHT.MOTIF README RELEASE RELNOTES
/etc/X11/xinit/xinitrc.d/xmbind
%{prefix}/bin/mwm
%{prefix}/bin/xmbind
%{prefix}/include/X11/bitmaps/*
%{prefix}/%{_lib}/X11/system.mwmrc
%{prefix}/%{_lib}/X11/bindings
%{prefix}/%{_lib}/libMrm.so.*
%{prefix}/%{_lib}/libUil.so.*
%{prefix}/%{_lib}/lib*.so
%{prefix}/%{_lib}/libXm.so.*
%{prefix}/man/man1/mwm*
%{prefix}/man/man1/xmbind*
%{prefix}/man/man4/mwmrc*

%files devel
%defattr(-,root,root)
/usr/include/Xm
/usr/include/Mrm
/usr/include/uil
%{prefix}/bin/uil
%{prefix}/include/Mrm
%{prefix}/include/Xm
%{prefix}/include/uil
%{prefix}/%{_lib}/lib*.a
%{prefix}/man/man1/uil.1*
%{prefix}/man/man3/*
%{prefix}/man/man5/*

%files demos
%defattr(-,root,root)
%{prefix}/share/Xm/*

%changelog

* Thu Mar 17 2016 Oleksiy Chernyavskyy <ochern@ics.com>
- updated for Motif 2.3.5

* Mon Aug 22 2011 Ihor Hayvuk <ihayvuk@ics.com>
- updated for Motif 2.3.4

* Tue Jun 30 2009 Ihor Hayvuk <ihayvuk@ics.com>
- updated for Motif 2.3.3

* Fri Mar 06 2009 Ihor Hayvuk <ihayvuk@ics.com>
- updated for Motif 2.3.2

* Sat May 03 2008 Ihor Hayvuk <ihayvuk@ics.com>
- updated for Motif 2.3.1

* Thu Nov 14 2007 Yuriy Syrota <ysyrota@softservecom.com>
- updated for Motif 2.3.1

* Thu Sep 16 2005 Yuriy Syrota <ysyrota@softservecom.com>
- updated for Motif 2.3.0 Beta1

* Thu Mar 05 2004 Justin Noel <justin@ics.com>
- updated for Motif 2.2.3
- added demos RPM

* Wed Jan 22 2003 Tim Powers <timp@redhat.com>
- rebuilt

* Tue Jan 21 2003 Thomas Woerner <twoerner@redhat.com> 2.2.2-13
- fix for Xmu/EditRes conflict (bug #80777)
- fix for wml and utf-8 (bug #80271)
- fix for Ext18List (bug #74502)

* Thu Nov 14 2002 Than Ngo <than@redhat.com> 2.2.2-12.2
- add buildprereq byacc and flex (bug #77860)

* Fri Nov  8 2002 Than Ngo <than@redhat.com> 2.2.2-12.1
- fix some build problem

* Mon Aug 27 2002 Than Ngo <than@redhat.com> 2.2.2-12
- Fixed a segmentation fault in mkcatdefs (bug #71955)

* Wed Jul 24 2002 Than Ngo <than@redhat.com> 2.2.2-11
- Added missing symlinks (bug #69117)

* Tue Jul 23 2002 Tim Powers <timp@redhat.com> 2.2.2-10
- build using gcc-3.2-0.1

* Tue Jun 25 2002 Than Ngo <than@redhat.com> 2.2.2-9
- fix to build motif (bug #64176)

* Thu Jun 13 2002 Than Ngo <than@redhat.com> 2.2.2-8
- rebuild in new enviroment

* Sun May 26 2002 Tim Powers <timp@redhat.com>
- automated rebuild

* Thu May 23 2002 Harald Hoyer <harald@redhat.de> 2.2.2-6
- patched ltmain.sh to link properly

* Wed May 22 2002 Harald Hoyer <harald@redhat.de> 2.2.2-6
- specified libraries by full name in files section 
  (libMrm was missing on alpha)

* Tue Mar 26 2002 Than Ngo <than@redhat.com> 2.2.2-5
- update new 2.2.2 from ICS

* Sun Mar 24 2002 Than Ngo <than@redhat.com> 2.2.2-4
- add missing uil

* Fri Mar 22 2002 Tim Powers <timp@redhat.com>
- rebuilt to try and shake some broken deps in the devel package

* Thu Mar 21 2002 Than Ngo <than@redhat.com> 2.2.2-2
- rebuild

* Thu Mar 21 2002 Than Ngo <than@redhat.com> 2.2.2-1
- update to 2.2.2 release

* Mon Feb 22 2002 Than Ngo <than@redhat.com> 2.2.1-3
- conflict with older lesstif

* Mon Feb 22 2002 Than Ngo <than@redhat.com> 2.2.1-2
- fix bug #60816

* Fri Feb 22 2002 Than Ngo <than@redhat.com> 2.2.1-1
- update to 2.2.1 release
- remove somme patches, which are included in 2.2.1

* Fri Feb 22 2002 Tim Powers <timp@redhat.com>
- rebuilt in new environment

* Fri Jan 25 2002 Tim Powers <timp@redhat.com>
- don't obsolete lesstif anymore, play nicely together
- rebuild against new toolchain

* Wed Jan 21 2002 Than Ngo <than@redhat.com> 2.1.30-11
- add some patches from Darrell Commander (supporting largefile)
- fix to build on s390

* Thu Jan 17 2002 Than Ngo <than@redhat.com> 2.1.30-10
- rebuild in 8.0

* Wed Sep  6 2001 Than Ngo <than@redhat.com>
- rebuild for ExtraBinge 7.2

* Thu May 03 2001 Than Ngo <than@redhat.com>
- add 3 official motif patches 
- add rm -rf $RPM_BUILD_ROOT in install section
- remove some old patches which are now in official patches

* Fri Dec 29 2000 Than Ngo <than@redhat.com>
- don't build static debug libraries

* Mon Dec 18 2000 Than Ngo <than@redhat.com>
- bzip2 source

* Mon Jul 24 2000 Than Ngo <than@redhat.de>
- rebuilt against gcc-2.96-44

* Wed Jul 12 2000 Than Ngo <than@redhat.de>
- rebuilt

* Sun Jun 11 2000 Than Ngo <than@redhat.de>
- fix imake to built with gcc-2.96 (thanks Jakup)
- put bitmaps in /usr/X11R6/include/X11/bitmaps
- put bindings in /usr/X11R6/lib/Xm/bindings
- add define -D_GNU_SOURCE to build Motif
- gzip man pages
- cleanup specfile

* Mon May 29 2000 Bernhard Rosenkraenzer <bero@redhat.com>
- Update to patchlevel 2
- remove bindings patch, it's included in pl2

* Tue May 16 2000 Matt Wilson <msw@redhat.com>
- use -fPIC on sparc
- fixed Ngo's "fixes"

* Mon May 15 2000 Ngo Than <than@redhat.de>
- added description.
- fixed spec, added uil stuff.

* Mon May 15 2000 Matt Wilson <msw@redhat.com>
- initialization of spec file.
