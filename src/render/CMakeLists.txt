bison_target(TextPrimitiveParser textprimitive_grammar.yy ${CMAKE_CURRENT_BINARY_DIR}/textprimitive_grammar.cc COMPILE_FLAGS -y )

add_library(render
  ${BISON_TextPrimitiveParser_OUTPUTS}
  displayobject.cpp
  fontinstance.cpp
  glextensions.cpp
  primitive.cpp
  primitiveinfo.cpp
  primitiveinstance.cpp
  primitivelist.cpp
  textformat.cpp
  textfragment.cpp
  textprimitive.cpp
  transform.cpp
  displayobject.h
  fontinstance.h
  glextensions.h
  primitive.h
  primitiveinfo.h
  primitiveinstance.h
  primitivelist.h
  textformat.h
  textfragment.h
  textprimitive.h
  transform.h
)

include_directories(
./
../
${CMAKE_CURRENT_BINARY_DIR} 
${CMAKE_CURRENT_BINARY_DIR}/../
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src/
)

