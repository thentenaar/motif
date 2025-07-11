#
#	Terminal Emulation
#
# neither machine nor uname are supported on all our platforms
# but uname seems to win more often.

setenv MACHINE `uname`
if ($status) then
	setenv MACHINE `machine`
endif
if ($status) then
	unsetenv MACHINE
	echo "Warning: cannot set machine type"
endif

set noglob
switch ($MACHINE)
   case mmax*:
	eval `tset -I -Q -s -m :xterms`
	stty erase ""
	stty susp 
	stty intr 
	#informix stuff
	setenv INFORMIXDIR /users/informix
	setenv SQLEXEC $INFORMIXDIR/lib/sqlexec
	setenv DBPATH /users/ifmxmgr/npirs/dbdir:/users/ifmxmgr/npirs/frm
	setenv DBDATE MDY2/
	set path = ( ${INFORMIXDIR}/bin $path )
	#end informix stuff
        breaksw
#hp 700 now
   case HP-UX*:
   case hp700*:
	stty erase 
	stty intr 
	stty kill 
	stty susp 
	stty echoe
	eval `tset -I -Q -s -m :xterm`
        breaksw
   case SunOS*:
   case sparc*:
	stty erase 
	stty susp 
	stty intr 
	eval `tset -I -Q -s -m :xterm`
        limit coredumpsize 512
	breaksw
   case IRIX*:
   case sgi*:
	stty erase 
	stty kill 
	stty susp 
	stty intr 
	eval `tset -I -Q -s -m :xterm`
        limit coredumpsize 512
	breaksw
   case osf1*:
   case OSF1*:
	stty echoe
	stty erase \
	stty kill 
	stty intr 
	eval `tset -I -Q -s -m :xterm`
        limit coredumpsize 512
	breaksw
   case default:
	stty erase 
	stty kill 
	stty susp 
	stty intr 
	eval `tset -I -Q -s -m :xterm`
        breaksw	
endsw


setenv	EDITOR  vi				# default editor 
setenv	MAILHOST osf.osf.org 			# for nbiff 
setenv  TZ est5edt
setenv  QA_REGDIR 20Final_reg
setenv	MB /project/motif/build

unset noglob
# END of .login script
