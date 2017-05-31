noinst_LIBRARIES = libgui.a

SUFFIXES: .ui .uih .qrc

.ui.o:
	$(QTUIC) -o ui_$*.h $<
	$(QTMOC) -o moc_$*.cpp $(top_srcdir)/src/gui/$*.h
	$(CXX) -I$(top_srcdir)/src -I$(top_srcdir)/src/gui/ $(CXXFLAGS) @GUI_CFLAGS@ -c -o $@ moc_$*.cpp
	rm moc_$*.cpp

.uih.o:
	$(QTMOC) -o $*.cpp -f$< $<
	$(CXX) -I$(top_srcdir)/src -I../ -I./ $(CXXFLAGS) @GUI_CFLAGS@ -c -o $@ $*.cpp
	rm $*.cpp

.qrc.o:
	$(QTRCC) -o $*.cpp -name $* $<
	$(CXX) -I$(top_srcdir)/src -I../ -I./ $(CXXFLAGS) $(QTGUI_CFLAGS) -c -o $@ $*.cpp
	rm $*.cpp

clean-local:
	-rm -f ui_* colourbutton.cpp icons.cpp viewer.cpp 

libgui_a_SOURCES = icons.qrc

libgui_a_SOURCES += display.ui quapp.ui

libgui_a_SOURCES += quapp_audio.cpp quapp_background.cpp quapp_funcs.cpp quapp_objects.cpp quapp_questions.cpp quapp_order.cpp quapp_menus.cpp quapp_run.cpp display_funcs.cpp

libgui_a_SOURCES += colourbutton.uih colourbutton_funcs.cpp

libgui_a_SOURCES += viewer.uih viewer_funcs.cpp

noinst_HEADERS = quapp.h 

INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/gui @GUI_CFLAGS@

