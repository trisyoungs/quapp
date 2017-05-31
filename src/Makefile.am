bin_PROGRAMS = quapp

SUBDIRS = gui templates base math render

noinst_HEADERS = version.h

# Target 'quapp'

quapp_SOURCES = main.cpp

INCLUDES = -I../ @GUI_CFLAGS@

quapp_LDADD = gui/libgui.a base/libbase.a render/librender.a math/libmath.a @MAC_LIBS@ @GUI_LDLIBS@
