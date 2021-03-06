#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.60)

# Set program name, version, bug-address and source directory
m4_define([QUAPP_VERSION],[r16])
AC_INIT(quapp,QUAPP_VERSION,tris@projectaten.net)
AC_CONFIG_SRCDIR([src/])

# Check system type and set defines
# Specify a single file in the source tree so we can check that --srcdir points to the right place
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADER(config.h)
AC_CANONICAL_TARGET
MAC_LIBS=""
if test "$target_vendor" = "apple"; then
  AC_DEFINE([IS_MAC], [], [Whether operating system is an Apple])
  MAC_LIBS="-framework OpenGL"
fi
AC_SUBST(MAC_LIBS)

AM_INIT_AUTOMAKE(quapp,QUAPP_VERSION)

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_YACC


#
# Additional Configuration Options
#

# QT Tools
QTMOC="moc"
QTRCC="rcc"
QTUIC="uic"
AC_ARG_WITH([qtmoc],[AC_HELP_STRING([--with-qtmoc=<executable>],
    [Specify Qt4 Meta-Object Compiler tool (moc) [default="moc"]])], ,
    [with_qtmoc="moc"])
AC_ARG_WITH([qtuic],[AC_HELP_STRING([--with-qtuic=<executable>],
    [Specify Qt4 User Interface Compiler tool (uic) [default="uic"]])], ,
    [with_qtuic="uic"])
AC_ARG_WITH([qtrcc],[AC_HELP_STRING([--with-qtrcc=<executable],
    [Specify Qt4 Resource Compiler tool (rcc) [default="rcc"]])], ,
    [with_qtrcc="rcc"])
QTMOC="$with_qtmoc"
QTRCC="$with_qtrcc"
QTUIC="$with_qtuic"

# Build Directory (used when packaging)
BUILDDIR=""
AC_ARG_WITH([build-dir],[AC_HELP_STRING([--with-build-dir=<dir>],
    [Specify location of build environment when packaging [default=""]])], ,
    [with_build_dir=""])
BUILDDIR="$with_build_dir"

# Install Directory (used when packaging - should be '$PREFIX - $BUILDDIR')
INSTALLDIR="/usr/local"
AC_ARG_WITH([install-dir],[AC_HELP_STRING([--with-install-dir=<dir>],
    [Specify 'clean' instal dir prefix, i.e. without any BUILDDIR [default="/usr/local"]])], ,
    [with_install_dir="/usr/local"])
# If --prefix has been give, use this value by default
if test "$prefix" != "NONE"; then
  INSTALLDIR="$prefix"
fi

if test "$with_install_dir" != "/usr/local"; then
  INSTALLDIR="$with_install_dir"
fi

PKG_CHECK_MODULES(FREETYPE, freetype2 >= 2.0)
PKG_CHECK_MODULES(FTGL, ftgl >= 2.0)


# QT-FRAMEWORK  (Mac only)
AC_ARG_WITH([qt],[AC_HELP_STRING([--with-qt=framework|fink|<none>],
    [Determines which installation of Qt4 to use on OS X (should not be set for Linux)[default=<none>]])], ,
    [with_qt=])

# Pre-initialise pkg-config  (unless using Qt framework on the Mac)
if test "$with_qt" != "framework"; then
  PKG_PROG_PKG_CONFIG
fi

# Set up compilation for Qt GUI
if test "$with_qt" = "framework"; then
  QTGUI_LIBS="-framework QtGui -framework QtOpenGL -framework QtCore"
  QTGUI_CFLAGS="-F QtGui -F QtOpenGl -F QtCore"
fi
if test "$with_qt" = "fink"; then
  QTGUI_LIBS="-L/sw/lib/qt4-x11/lib -lQtGui -lQtCore -lQtOpenGL"
  QTGUI_CFLAGS="-I/sw/lib/qt4-x11/include -I/usr/X11R6/include"
  QTOPENGL_CFLAGS="-I/sw/lib/qt4-x11/include"
  QTOPENGL_LIBS="-I/sw/lib/qt4-x11/lib"
  PKG_CHECK_MODULES(QTGUI, QtGui >= 4.1.0)
  PKG_CHECK_MODULES(QTOPENGL, QtOpenGL >= 4.1.0)
  PKG_CHECK_MODULES(QTMULTIMEDIA, QtMultimedia >= 4.1.0)
fi
if test "x$with_qt" = "x"; then
  PKG_CHECK_MODULES(QTGUI, QtGui >= 4.1.0)
  PKG_CHECK_MODULES(QTOPENGL, QtOpenGL >= 4.1.0)
  PKG_CHECK_MODULES(QTMULTIMEDIA, QtMultimedia >= 4.1.0)
fi
GUI_LDLIBS="$FTGL_LIBS $FREETYPE_LIBS $QTGUI_LIBS $QTOPENGL_LIBS $QTMULTIMEDIA_LIBS -lncurses -lGL -lfreetype -lhdf5"
GUI_CFLAGS="$FREETYPE_CFLAGS $QTGUI_CFLAGS"


VERSION=QUAPP_VERSION
# Perform Makefile substitutions
AC_SUBST(GUI_LDLIBS)
AC_SUBST(GUI_CFLAGS)
AC_SUBST(QTMOC)
AC_SUBST(QTUIC)
AC_SUBST(QTRCC)
AC_SUBST(BUILDDIR)
AC_SUBST(INSTALLDIR)
AC_SUBST(VERSION)


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_STRUCT_TM


# Checks for library functions.
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([floor pow select sqrt strchr strrchr strstr])


# Define Makefiles in AC_OUTPUT 
AC_OUTPUT(Makefile
src/Makefile
src/gui/Makefile
src/base/Makefile
src/render/Makefile
src/math/Makefile
src/templates/Makefile
)
