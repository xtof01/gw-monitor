AC_INIT([gw-monitor], [0.4], [christof@efkemann.net])
AC_CONFIG_AUX_DIR([.ac])

AM_INIT_AUTOMAKE([foreign 1.11 -Wall -Wno-portability dist-xz no-dist-gzip])
AM_SILENT_RULES([yes])
AC_LANG([C])
AC_PROG_CC_C99
AC_PROG_SED
AC_PROG_INSTALL
AC_PROG_MKDIR_P
AC_CHECK_HEADER(ev.h, [], [AC_MSG_ERROR([ev.h not found])])
AC_CHECK_LIB([ev], [ev_run], [], [AC_MSG_ERROR([libev not found])])

PKG_CHECK_MODULES([MNL], [libmnl >= 1.0])

AC_ARG_WITH([systemdsystemunitdir],
        AS_HELP_STRING([--with-systemdsystemunitdir=DIR], [Directory for systemd service files]),
        [],
        [with_systemdsystemunitdir=$($PKG_CONFIG --variable=systemdsystemunitdir systemd)])
if test "x$with_systemdsystemunitdir" != xno; then
        AC_SUBST([systemdsystemunitdir], [$with_systemdsystemunitdir])
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
