lib_LTLIBRARIES = lib/libnexstar.la 

lib_libnexstar_la_SOURCES = src/nexstar.c src/nexstar.h \
			 src/deg2str.c src/deg2str.h

lib_libnexstar_la_LDFLAGS = -version-info 0:1:0 

libnexstar_includedir = $(includedir)/
libnexstar_include_HEADERS = src/nexstar.h \
			     src/deg2str.h
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libnexstar.pc

man7_MANS = man/libnexstar.man
	 
EXTRA_DIST = $(man7_MANS) $(sysconf_DATA) testcode/*.c testcode/Makefile examples/*.c examples/Makefile

#install-exec-hook:
#	cp $(srcdir)/bootscripts/cwhod /etc/init.d/
#	cp $(srcdir)/bootscripts/lvsstatd /etc/init.d/
#	cp $(srcdir)/bootscripts/lvsmgrd /etc/init.d/

#uninstall-hook:
#	rm -f /etc/init.d/lvsmgrd
#	rm -f /etc/init.d/lvsstatd
#	rm -f /etc/init.d/cwhod

