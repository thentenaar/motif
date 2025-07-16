dnl SYNOPSIS
dnl
dnl AX_CFLAGS - Set compiler flags
dnl
dnl DESCRIPTION
dnl
dnl This macro checks a number of warning options for GCC and
dnl Clang, and adds supoprted options to CFLAGS. -Werror doesn't
dnl get added here, simply because having -Werror and -Wstrict-prototypes
dnl will cause compilations from autoconf to fail.
dnl
dnl COVERAGE
dnl
dnl This macro exports ax_cc_gcov_command as the proper gcov command
dnl to invoke (i.e. llvm-cov gcov on clang.)
dnl

AC_DEFUN([AX_CFLAGS],[
	AX_REQUIRE_DEFINED([AX_APPEND_COMPILE_FLAGS])
	AX_REQUIRE_DEFINED([AX_CHECK_COMPILE_FLAG])
	AC_LANG_PUSH([C])

	dnl Clang uses -Q, GCC doesn't support it.
	AX_CHECK_COMPILE_FLAG([-Werror -Qunused-arguments],dnl
	                      [ax_cc_clang="yes"],dnl
	                      [ax_cc_clang="no"])

	AS_IF([test "$ax_cc_clang" == "yes"],[
		ax_cc_gcov_command="llvm-cov gcov"
		AX_APPEND_COMPILE_FLAGS([ dnl
			-fno-strict-aliasing dnl
			-Wall dnl
			-Wno-unused dnl
			-Wno-comment dnl
			-Wno-unused-result dnl
			-Wno-misleading-indentation dnl
			-Wno-implicit-fallthrough dnl
			-Wno-int-in-bool-context dnl
			-Wno-padded dnl
			-Wno-switch-enum dnl
			-Wno-disabled-macro-expansion dnl
			-Wno-format-nonliteral dnl
			-Wno-long-long dnl
			-Wno-format dnl
			-Wno-extra-semi-stmt dnl
			-Wno-documentation dnl
			-Qunused-arguments dnl
		])
	],[	dnl Assume GCC
		ax_cc_gcov_command="gcov"
		AX_APPEND_COMPILE_FLAGS([ dnl
			-fno-strict-aliasing dnl
			-Wall dnl
			-Wshadow dnl
			-Wwrite-strings dnl
			-Wcast-align dnl
			-Wformat-security dnl
			-Wno-unused dnl
			-Wno-comment dnl
			-Wno-unused-result dnl
			-Wno-misleading-indentation dnl
			-Wno-implicit-fallthrough dnl
			-Wno-int-in-bool-context dnl
			-Wno-conversion dnl
			-Wno-discarded-qualifiers dnl
			-Wno-strict-prototypes dnl
			-Wno-missing-prototypes dnl
			-Wno-missing-declarations dnl
			-Wno-nested-externs dnl
			-Wno-cast-qual dnl
			-Wno-redundant-decls dnl
			-Wno-bad-function-cast dnl
		])
	])

	AC_SUBST([ax_cc_gcov_command])
	AC_LANG_POP([C])
]) dnl AX_CFLAGS
