#
# src/Makefile.am
#

sbin_PROGRAMS = dnc
dnc_SOURCES = dnc.c session.c main.c
dnc_SOURCES +=
dnc_LDFLAGS = -rdynamic -I/usr/include/dnds/asn1/ @DNDSLIBS_LIBS@
##@DNDSLIBS_LIBS@
dnc_CFLAGS =  -DCONFIG_FILE=\"@sysconfdir@/dnc.conf\"
dnc_CFLAGS += -ldnds -ludt -I/usr/include/dnds/asn1/ @DNDSLIBS_CFLAGS@
##@DNDSLIBS_CFLAGS@
