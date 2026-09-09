AC_PREREQ([2.69])
AC_INIT([pfm], [2.4])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_AUX_DIR([build-aux])

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug], [build with debug symbols and no optimization (default: release)])],
  [AS_CASE([$enableval], [yes|no], [], [AC_MSG_ERROR([use yes or no for --enable-debug])])],
  [enable_debug=no])

dnl Set defaults before compiler detection, which otherwise adds -g -O2.
dnl Explicit user CFLAGS/CXXFLAGS continue to override these defaults.
AS_IF([test "x$enable_debug" = xyes],
  [pfm_default_flags="-g -O0"; pfm_build_type=debug],
  [pfm_default_flags="-O2"; pfm_build_type=release])
AS_IF([test "${CFLAGS+set}" != set], [CFLAGS="$pfm_default_flags"])
AS_IF([test "${CXXFLAGS+set}" != set], [CXXFLAGS="$pfm_default_flags"])
AC_MSG_NOTICE([build type: $pfm_build_type])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MKDIR_P

dnl Preserve the warning policy from the hand-written makefile.
CFLAGS="$CFLAGS -Wall -pedantic"
CXXFLAGS="$CXXFLAGS -Wall -pedantic"
AC_SUBST([DEBUG_BUILD], [$enable_debug])

dnl Version bumps are an explicit maintainer action, never a configure side effect.
AC_ARG_VAR([VBUILD], [path to vbuild (needed only for make version)])
AC_PATH_PROG([VBUILD], [vbuild], [vbuild])
AC_SUBST([MAJOR_VERSION], [${PACKAGE_VERSION%%.*}])
AC_SUBST([MINOR_VERSION], [${PACKAGE_VERSION#*.}])

AC_LANG_PUSH([C++])
CXXFLAGS="$CXXFLAGS -std=c++20"
AC_MSG_CHECKING([whether $CXX supports C++20])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <string>
#if __cplusplus < 202002L
#error C++20 is required
#endif
]], [[std::string value = "pfm"; return !value.starts_with("pf");]])],
  [AC_MSG_RESULT([yes])],
  [AC_MSG_RESULT([no]); AC_MSG_ERROR([PFM requires a C++20 compiler and standard library])])

AC_ARG_ENABLE([test-suite],
  [AS_HELP_STRING([--enable-test-suite], [enable PFM's built-in test suite])],
  [AS_CASE([$enableval], [yes|no], [], [AC_MSG_ERROR([use yes or no for --enable-test-suite])])],
  [enable_test_suite=no])
PFM_CPPFLAGS="-DSQLITE_HAS_CODEC"
AS_IF([test "x$enable_debug" = xyes],
  [PFM_CPPFLAGS="$PFM_CPPFLAGS -DRUN_IN_DEBUGGER"])
AS_IF([test "x$enable_test_suite" = xyes],
  [PFM_CPPFLAGS="$PFM_CPPFLAGS -DPFM_TEST_SUITE_ENABLED"])
AC_SUBST([PFM_CPPFLAGS])
CPPFLAGS="$CPPFLAGS $PFM_CPPFLAGS"

dnl Keep dependency probes in C++ mode, as the application includes these headers.
AC_CHECK_HEADERS([readline/readline.h readline/history.h sqlcipher/sqlite3.h gcrypt.h libcred.hpp nlohmann/json.hpp microhttpd.h httpserver.hpp], [],
  [AC_MSG_ERROR([missing required header; install the development dependencies or set CPPFLAGS=-I/path/to/include])])

dnl The current POSIX build uses pthreads for both compilation and linking.
PTHREAD_FLAGS=-pthread
CXXFLAGS="$CXXFLAGS $PTHREAD_FLAGS"
AC_SUBST([PTHREAD_FLAGS])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <pthread.h>
static void *worker(void *) { return nullptr; }]],
  [[pthread_t thread; pthread_create(&thread, nullptr, worker, nullptr); pthread_join(thread, nullptr);]])], [],
  [AC_MSG_ERROR([POSIX threads are required])])

dnl Probe providers before consumers so that static-library link order is valid.
AC_SEARCH_LIBS([tgetent], [tinfo ncurses curses], [],
  [AC_MSG_ERROR([a curses/terminfo development library is required])])
AC_CHECK_LIB([history], [add_history], [], [AC_MSG_ERROR([GNU History is required])])
AC_CHECK_LIB([readline], [readline], [], [AC_MSG_ERROR([GNU Readline is required])])
AC_CHECK_LIB([gcrypt], [gcry_check_version], [], [AC_MSG_ERROR([libgcrypt is required])])
AC_CHECK_LIB([sqlcipher], [sqlite3_key], [], [AC_MSG_ERROR([SQLCipher with encryption support is required])])
AC_CHECK_LIB([microhttpd], [MHD_get_version], [], [AC_MSG_ERROR([libmicrohttpd is required])])

LIBS="-lcred -lhttpserver $LIBS"
AC_MSG_CHECKING([for libcred and libhttpserver])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <string>
#include <libcred.hpp>
#include <httpserver.hpp>
]], [[
std::string password, error;
libcred::get_password("pfm", "configure", &password, &error);
httpserver::webserver server(httpserver::create_webserver(0));
]])], [AC_MSG_RESULT([yes])],
  [AC_MSG_RESULT([no]); AC_MSG_ERROR([cannot link libcred/libhttpserver; set LDFLAGS and LIBS for their locations and dependencies])])
AC_LANG_POP([C++])

AC_PATH_PROG([PANDOC], [pandoc])
AS_IF([test -z "$PANDOC"],
  [AC_MSG_WARN([pandoc was not found; make builds the executable, but make man and make install require pandoc])])

AC_CONFIG_FILES([makefile:Makefile.in])
AC_OUTPUT
