%.service: %.service.in $(top_builddir)/config.status
	$(AM_V_GEN)$(SED)				\
	    -e 's|[@]sbindir[@]|$(sbindir)|g'		\
	    -e 's|[@]sysconfdir[@]|$(sysconfdir)|g'	\
	    < $< > $@-t &&				\
	    mv $@-t $@

install-data-local:
	$(MKDIR_P) $(DESTDIR)$(sysconfdir)/sysconfig
	$(INSTALL_DATA) $(srcdir)/gw-monitor.sysconfig \
	    $(DESTDIR)$(sysconfdir)/sysconfig/gw-monitor

uninstall-local:
	rm -f $(DESTDIR)$(sysconfdir)/sysconfig/gw-monitor
	rmdir $(DESTDIR)$(sysconfdir)/sysconfig || :

sbin_PROGRAMS = gw-monitor
systemdsystemunit_DATA = gw-monitor.service

AM_CFLAGS = $(MNL_CFLAGS)

gw_monitor_SOURCES = gw-monitor.c outputs.c outputs.h
gw_monitor_LDADD = $(MNL_LIBS)

CLEANFILES = $(systemdsystemunit_DATA)
EXTRA_DIST = gw-monitor.service.in gw-monitor.sysconfig
