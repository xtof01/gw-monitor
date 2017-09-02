%.service: %.service.in $(top_builddir)/config.status
	$(AM_V_GEN)sed					\
	    -e 's|[@]sbindir[@]|$(sbindir)|g'		\
	    -e 's|[@]sysconfdir[@]|$(sysconfdir)|g'	\
	    < $< > $@-t &&				\
	    mv $@-t $@

install-data-local:
	$(MKDIR_P) $(DESTDIR)$(sysconfdir)/sysconfig
	$(INSTALL_DATA) $(srcdir)/ppp-monitor.sysconfig \
	    $(DESTDIR)$(sysconfdir)/sysconfig/ppp-monitor

uninstall-local:
	rm -f $(DESTDIR)$(sysconfdir)/sysconfig/ppp-monitor
	rmdir $(DESTDIR)$(sysconfdir)/sysconfig || :

sbin_PROGRAMS = ppp-monitor
systemdsystemunit_DATA = ppp-monitor.service

AM_CFLAGS = $(MNL_CFLAGS)

ppp_monitor_SOURCES = ppp-monitor.c outputs.c outputs.h
ppp_monitor_LDADD = $(MNL_LIBS)

CLEANFILES = $(systemdsystemunit_DATA)
EXTRA_DIST = ppp-monitor.service.in ppp-monitor.sysconfig
