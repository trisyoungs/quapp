SUBDIRS = src

SUFFIXES: .dt

.dt.desktop:
	sed -e "s:PREFIX:@INSTALLDIR@:" -e "s:VERSION:@VERSION@:" $< > $@

EXTRA_DIST = TODO CMakeLists.txt cmake_uninstall.cmake.in src/CMakeLists.txt src/*/CMakeLists.txt src/gui/icons/* extra/wright.ttf extra/Quapp.dt extra/quapp.png

quappdir = $(datadir)/quapp
quapp_DATA = extra/quapp.png extra/wright.ttf

desktopdir = @BUILDDIR@/usr/share/applications
desktop_DATA = extra/Quapp.desktop

