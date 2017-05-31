include_directories(
./
../
${CMAKE_CURRENT_BINARY_DIR} 
${CMAKE_CURRENT_BINARY_DIR}/../
${QT_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/src/gui 
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src/
)

# Source files (not Qt UI files or custom .h [uih])
SET(qtproject_SRCS
  colourbutton_funcs.cpp
  viewer_funcs.cpp
  quapp_angle.cpp
  quapp_background.cpp
  quapp_funcs.cpp
  quapp_objects.cpp
  quapp_questions.cpp
  quapp_run.cpp
  quapp_order.cpp
  quapp_menus.cpp
)

SET(pro_MOC_HDRS
  colourbutton.uih

  quapp.h
)
QT4_WRAP_CPP(pro_MOC_SRCS ${pro_MOC_HDRS} OPTIONS -f)

SET(pro_UIS
  quapp.ui
)
QT4_WRAP_UI(pro_UIS_H ${pro_UIS})

set(pro_RES_QRC
  icons.qrc 
)
QT4_ADD_RESOURCES(pro_RES ${pro_RES_QRC})

add_library(gui
${pro_RES}
${pro_UIS_H}
${qtproject_SRCS}
${pro_MOC_SRCS}
)

