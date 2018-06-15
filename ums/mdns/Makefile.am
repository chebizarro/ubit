AUTOMAKE_OPTIONS = foreign no-dependencies
SUBDIRS = mDNSCore mDNSPosix
EXTRA_DIST = APPLE_LICENSE NOTICE README.txt VERSION NOTICE makefile.umake

MDNS_CFLAGS = @mdns_cflags@
AM_CPPFLAGS = -I. -I$(top_srcdir)/ums/mdns/mDNSCore -I$(top_srcdir)/ums/mdns/mDNSPosix $(MDNS_CFLAGS)

mdns_SOURCES = mDNSCore/mDNSClientAPI.h mDNSCore/mDNSPlatformFunctions.h mDNSCore/mDNS.c mDNSCore/mDNSDebug.h mDNSPosix/mDNSPosix.h mDNSPosix/mDNSUNP.h mDNSPosix/mDNSPosix.c mDNSPosix/mDNSUNP.c

libmdns_a_SOURCES = $(mdns_SOURCES)
noinst_LIBRARIES = libmdns.a

