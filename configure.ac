AC_INIT([libbitcoin-protocol], [0.1.0], [mienkphi@gmail.com])
AC_USE_SYSTEM_EXTENSIONS
AC_LANG(C++)
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror])
AM_PROG_AR
LT_INIT
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_GNU_SOURCE
AX_CXX_COMPILE_STDCXX_11(noext,mandatory)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Boost doesn't publish package configs but has m4 tests.
# The current versions produce incorrect error message text.
AX_BOOST_BASE([1.50],,
    [AC_MSG_ERROR([Boost 1.50 or later is required but was not found.])])

PKG_PROG_PKG_CONFIG

PKG_CHECK_MODULES([bitcoin], [libbitcoin >= 2.4.0])
PKG_CHECK_MODULES([protobuf], [protobuf >= 2.6.0])
PKG_CHECK_MODULES([czmqpp], [libczmq++ >= 0.4.1])

# build tests
AC_ARG_WITH([tests], [AS_HELP_STRING([--with-tests],
    [require build with tests. Requires pkg-config [default=no]])],
    [enable_tests_ext=$withval],
    [enable_tests_ext=no])

# conditionally require test dependencies
if test "x$enable_tests_ext" != "xno"; then
    AX_BOOST_UNIT_TEST_FRAMEWORK
fi

AM_CONDITIONAL(ENABLE_TESTS, test "x$enable_tests_ext" != "xno")

# Do not add contextual build parameters in configuration.
# http://stackoverflow.com/a/4680578
OPTIMIZATIONS="-fvisibility=internal"
WARNINGS="-Wall -Wextra -Wpedantic -Wno-missing-braces"
SAFEGUARDS="-fvisibility-inlines-hidden -fstack-protector-all"
EXCEPTIONS="-Wno-unused-function -Wno-unused-parameter"
AM_CXXFLAGS=$WARNINGS" "$SAFEGUARDS" "$OPTIMIZATIONS" "$EXCEPTIONS
AC_SUBST([AM_CXXFLAGS])

AC_ARG_WITH([pkgconfigdir], AS_HELP_STRING([--with-pkgconfigdir=PATH],
    [Path to the pkgconfig directory [[LIBDIR/pkgconfig]]]),
    [pkgconfigdir="$withval"], [pkgconfigdir='${libdir}/pkgconfig'])
AC_SUBST([pkgconfigdir])

AC_CONFIG_FILES([Makefile libbitcoin-protocol.pc])
AC_OUTPUT
