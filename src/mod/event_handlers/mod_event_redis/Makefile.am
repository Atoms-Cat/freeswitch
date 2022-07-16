include $(top_srcdir)/build/modmake.rulesam
MODNAME=mod_event_redis

mod_LTLIBRARIES = mod_event_redis.la
mod_event_redis_la_SOURCES  = mod_event_redis.c
mod_event_redis_la_CFLAGS   = $(AM_CFLAGS)
mod_event_redis_la_LIBADD   = $(switch_builddir)/libfreeswitch.la
mod_event_redis_la_LDFLAGS  = -avoid-version -module -no-undefined -shared
