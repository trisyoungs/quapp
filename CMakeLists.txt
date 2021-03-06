project(Quapp)

set(DESCRIPTION "Quapp")
set(AUTHOR "Tristan Youngs")
set(VERSION_MAJOR "0")
set(VERSION_MINOR "16")

set(CMAKE_BUILD_TYPE "Release")

if(APPLE)
  cmake_minimum_required(VERSION 2.6.0)
  set(CMAKE_OSX_ARCHITECTURES "i386")
  ADD_DEFINITIONS(-D_MAC)
else(APPLE)
  cmake_minimum_required(VERSION 2.4.8)
endif(APPLE)

if(COMMAND cmake_policy)
  cmake_policy(VERSION 2.6)
endif(COMMAND cmake_policy)

# Set target name
if(WIN32)
	set (target_name Quapp)
else(WIN32)
	set (target_name quapp)
endif(WIN32)

# Find required packages for this project
find_package(Qt4 REQUIRED)
find_package(HDF5 REQUIRED)
find_package(ZLIB REQUIRED)
include(FindBISON)

# Perform system-specific setup
# -- Windows
if(WIN32)
  # Find SZIP
  find_package(SZIP REQUIRED)
  # Setup SZIP and FTGL, and GLExt bits and pieces (for custom build machine)
  set(SZIP_DIR "C:/src/szip-2.1")
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "C:/src/szip-2.1/")
  set(FTGL_INCLUDES "C:/src/ftgl-2.1.3~rc5/src")
  set(FT2_INCLUDES "C:/GnuWin32/include" "C:/GnuWin32/include/freetype2")
  set(FTGL_LIBRARIES "C:/src/ftgl-2.1.3~rc5/src/libftgl.a")
  set(FT2_LIBRARIES "C:/GnuWin32/lib/libfreetype.dll.a")
  set(GLEXT_INCLUDES "C:/src/include")
  # Find OpenGL
  find_package(OPENGL REQUIRED)
  # Setup HDF5
  SET(HDF5_LIBS "-LC:/src/hdf5-1.8.10-patch1/bin -lhdf5 -L${SZIP_DIR}/bin -lszip" CACHE STRING "Link-time commands to use HDF5 (Windows only)")
endif(WIN32)

# -- Unix
if(UNIX)
  # Set include location for FreeType2
  set(FT2_INCLUDES "/usr/include/freetype2")
endif(UNIX)

# -- OSX
if(APPLE)
  # Set include location for FreeType2
  set(FT2_INCLUDES "/usr/local/include/freetype2")
  # Find OpenGL and GLUT
  find_package(OPENGL REQUIRED)
  find_package(GLUT REQUIRED)
endif(APPLE)

# Set output paths for libraries and binaries
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(SRCS src/)

# Add global NOMINMAX define for Windows systems, and inhibit console creation
if(WIN32)
	ADD_DEFINITIONS("-DNOMINMAX")
	SET(GUI_TYPE WIN32)
	if(MINGW)
		SET(CMAKE_EXE_LINKER_FLAGS "-mwindows")
	endif(MINGW)
endif(WIN32)

# Set search locations for header files (for both project and external dependencies)
include_directories(
	${CMAKE_BINARY_DIR}/src
	${CMAKE_SOURCE_DIR}/src
	${QT_INCLUDE_DIR}
	${FTGL_INCLUDES}
	${FT2_INCLUDES}
	${GLEXT_INCLUDES}
	${HDF5_INCLUDE_DIRS}
)

# Process CMakeLists.txt in src subdirectory
add_subdirectory(${SRCS})

# Set bundle information for Mac
If(APPLE)
	set(MACOSX_BUNDLE_ICON_FILE  "quapp.icns")
	set(MACOSX_BUNDLE_GUI_IDENTIFIER  "Quapp")
	set(MACOSX_BUNDLE_LONG_VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}")
	set(MACOSX_BUNDLE_BUNDLE_NAME  "Quapp" )
	set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}")
	set(MACOSX_BUNDLE_BUNDLE_VERSION  "${VERSION_MAJOR}.${VERSION_MINOR}")
	set(MACOSX_BUNDLE_COPYRIGHT "${AUTHOR}")
endif(APPLE)

# Tag executable for generation
add_executable(${target_name}  MACOSX_BUNDLE
	${SRCS}/main.cpp 
)

# Link line
link_directories (${LIBRARY_OUTPUT_PATH})
IF(WIN32 OR APPLE)
	target_link_libraries(${target_name} gui render base math expression kernels ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${FTGL_LIBRARIES} ${OPENGL_LIBRARIES} ${FT2_LIBRARIES} ${HDF5_LIBS})
ELSE(WIN32 OR APPLE)
	target_link_libraries(${target_name} gui render base math expression kernels ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${HDF5_LIBRARIES} -lGL -lftgl)
ENDIF(WIN32 OR APPLE)

# -----------------------------------
# Installation with CPack starts here
# -----------------------------------

# Add local dir to module path (so we find out local NSIS.template.in)
set(CPACK_MODULE_PATH ${CPACK_MODULE_PATH} "${CMAKE_SOURCE_DIR}/extra")

####################
####install target
####################
if(NOT ${PREFIX})
  MESSAGE("Install folder ${PREFIX}")
  set(CMAKE_INSTALL_PREFIX "${PREFIX}")
endif(NOT ${PREFIX})

if(UNIX AND NOT APPLE)
  configure_file(${CMAKE_SOURCE_DIR}/desktop.cmake ${CMAKE_BINARY_DIR}/extra/Quapp.desktop)
  install(FILES ${CMAKE_BINARY_DIR}/extra/Quapp.desktop 
       DESTINATION "/usr/share/applications/")
  install(FILES ${CMAKE_SOURCE_DIR}/extra/quapp.png
        DESTINATION share/quapp)
endif(UNIX AND NOT APPLE)

if (APPLE)
  install(TARGETS ${target_name}
  BUNDLE DESTINATION .
  )
else(APPLE)
  install(TARGETS ${target_name}
      RUNTIME DESTINATION bin
   )
endif(APPLE)

# Install extra data
if (WIN32)
   # Install test data
   install(DIRECTORY  "${CMAKE_HOME_DIRECTORY}/test/" 
     DESTINATION test
     REGEX ".svn" EXCLUDE
   )
   # Install default font
   install(FILES  ${CMAKE_SOURCE_DIR}/extra/wright.ttf 
     DESTINATION bin
   )
endif(WIN32)

if(WIN32)
  Find_File(qtgui QtGui4.dll PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)
  Find_File(qtcore QtCore4.dll PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)
  Find_File(qtgl QtOpenGL4.dll PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)
  Find_File(qtsvg QtSvg4.dll PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)
  Find_File(qtxml QtXml4.dll PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)
  Find_File(qsvg qsvg4.dll PATHS ${QT_PLUGINS_DIR}/iconengines NO_DEFAULT_PATH)
  Find_File(mingwgcc libgcc_s_dw2-1.dll PATHS "C:/MinGW/bin" NO_DEFAULT_PATH)
  Find_File(mingwg++ libstdc++-6.dll PATHS "C:/MinGW/bin" NO_DEFAULT_PATH)
  Find_File(ft2 freetype6.dll PATHS "C:/GnuWin32/bin" NO_DEFAULT_PATH)
  Find_File(zlib zlib1.dll PATHS "C:/GnuWin32/bin" NO_DEFAULT_PATH)
  if(NOT qsvg) 
    Find_File(qsvg qsvgicon4.dll PATHS ${QT_PLUGINS_DIR}/iconengines NO_DEFAULT_PATH)
  endif(NOT qsvg)
  Find_File(qsvgie qsvg4.dll PATHS ${QT_PLUGINS_DIR}/imageformats NO_DEFAULT_PATH)
  install(FILES ${qtgui} ${qtcore} ${qtnetwork} ${qtsvg} ${qtxml} ${qtgl} ${mingwgcc} ${mingwg++} ${ft2} ${zlib}  ${CMAKE_CURRENT_SOURCE_DIR}/extra/Quapp.ico
   DESTINATION bin
  )
  install(FILES ${qsvg} 
    DESTINATION bin/iconengines
  )
  install(FILES ${qsvgie} 
    DESTINATION bin/imageformats
  )
ENDIF(WIN32)


if(APPLE)
## install the icon for the bundle
  install(FILES "${CMAKE_SOURCE_DIR}/extra/quapp.icns" 
        DESTINATION "${target_name}.app/Contents/Resources/")
#  install(DIRECTORY  "${CMAKE_HOME_DIRECTORY}/data/"
#     DESTINATION ${target_name}.app/Contents/SharedSupport/
#     REGEX ".svn" EXCLUDE)
  install(FILES "${CMAKE_SOURCE_DIR}/extra/quapp.icns"
        DESTINATION . RENAME ".VolumeIcon.icns")
  install(FILES "${CMAKE_SOURCE_DIR}/extra/background.png"
        DESTINATION .background/) 
  install(FILES "${CMAKE_SOURCE_DIR}/COPYING" 
         DESTINATION . RENAME .COPYING)
  install(FILES "${CMAKE_SOURCE_DIR}/extra/Quapp.DS_Store"
         DESTINATION . RENAME .DS_Store)
#  INSTALL(CODE "set(input_file
#        \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${target_name}.app/Contents/MacOS/${target_name}\")")
#  INSTALL(SCRIPT "${CMAKE_SOURCE_DIR}/CMakeIngestOSXBundleLibraries.cmake")
endif(APPLE)

#######################
## uninstall target
#######################

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
###########################

INCLUDE(InstallRequiredSystemLibraries)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${DESCRIPTION})
SET(CPACK_PACKAGE_VENDOR ${AUTHOR})
SET(CPACK_PACKAGE_CONTACT ${CPACK_PACKAGE_VENDOR})

IF (APPLE)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/README ${CMAKE_BINARY_DIR}/README.txt COPYONLY)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/COPYING ${CMAKE_BINARY_DIR}/COPYING.txt COPYONLY) 
  SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_BINARY_DIR}/README.txt")
  SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_BINARY_DIR}/COPYING.txt")
ELSE(APPLE)
  SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
  SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING") 
ENDIF(APPLE)

SET(CPACK_PACKAGE_VERSION ${VERSION_MAJOR}.${VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "Quapp")
IF(WIN32)
  SET(CPACK_GENERATOR "NSIS;ZIP")
  set(CMAKE_INSTALL_PREFIX "")
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/extra/Quapp.ico")
  set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/extra/Quapp.ico")
  SET(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/extra\\\\Quapp.bmp")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\quapp.ico")  
  SET(CPACK_NSIS_DISPLAY_NAME "Quapp")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.projectaten.net\\\\quapp")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.projectaten.net\\\\quapp")
  SET(CPACK_NSIS_CONTACT "tris@projectaten.net")
  SET(CPACK_NSIS_MODIFY_PATH ON) 
  SET(CPACK_PACKAGE_EXECUTABLES "quapp" "quapp")
  set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\quapp.lnk\\\" \\\"$INSTDIR\\\\bin\\\\quapp.exe\\\" \\\"\\\" \\\"$INSTDIR\\\\bin\\\\quapp.ico\\\"
      StrCmp $INSTALL_DESKTOP \\\"1\\\" 0 +2
      CreateShortCut \\\"$DESKTOP\\\\quapp.lnk\\\" \\\"$INSTDIR\\\\bin\\\\quapp.exe\\\" \\\"\\\" \\\"$INSTDIR\\\\bin\\\\quapp.ico\\\"
"
  )
Endif(WIN32)

IF(UNIX)
  SET(CPACK_GENERATOR "TGZ;TBZ2;RPM;DEB")
Endif(UNIX)

IF(APPLE)
  SET(CPACK_GENERATOR "TGZ")
Endif(APPLE)

MESSAGE("${CPACK_TEMPORARY_DIRECTORY}")
SET(CPACK_CMAKE_GENERATOR ${CMAKE_GENERATOR})

INCLUDE(CPack)


