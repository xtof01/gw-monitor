AC_INIT([ppp-monitor], [0.1], [christof@efkemann.net])
AC_CONFIG_AUX_DIR([.ac])

AM_INIT_AUTOMAKE([1.11 -Wall foreign dist-xz no-dist-gzip])
AM_SILENT_RULES([yes])
AC_LANG([C])
AC_PROG_CC
AC_PROG_INSTALL
AC_CHECK_HEADER(ev.h, [], [AC_MSG_ERROR([ev.h not found])])
AC_CHECK_LIB([ev], [ev_run], [], [AC_MSG_ERROR([libev not found])])

PKG_CHECK_MODULES([MNL], [libmnl])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
