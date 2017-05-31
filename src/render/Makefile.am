noinst_LIBRARIES = librender.a

AM_YFLAGS = -d

clean-local:
	-rm -f textprimitive_grammar.cc

librender_a_SOURCES = textprimitive_grammar.yy
librender_a_SOURCES += displayobject.cpp fontinstance.cpp glextensions.cpp primitive.cpp primitiveinfo.cpp primitiveinstance.cpp primitivelist.cpp textformat.cpp textfragment.cpp textprimitive.cpp transform.cpp

noinst_HEADERS = displayobject.h fontinstance.h glextensions.h primitive.h primitiveinfo.h primitiveinstance.h primitivelist.h textformat.h textfragment.h textprimitive.h transform.h

INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/gui @GUI_CFLAGS@

