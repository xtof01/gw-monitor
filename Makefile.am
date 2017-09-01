%.service: %.service.in
	$(AM_V_GEN)sed -e 's,@sbindir\@,$(sbindir),g' -e 's,@sysconfdir\@,$(sysconfdir),g' $< > $@

sbin_PROGRAMS = ppp-monitor
systemdsystemunit_DATA = ppp-monitor.service

AM_CFLAGS = $(MNL_CFLAGS)

ppp_monitor_SOURCES = ppp-monitor.c outputs.c outputs.h
ppp_monitor_LDADD = $(MNL_LIBS)

CLEANFILES = $(systemdsystemunit_DATA)
EXTRA_DIST = ppp-monitor.service.in
