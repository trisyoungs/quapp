add_library(base
  lineparser.cpp
  messenger.cpp
  question.cpp
  questionset.cpp
  quiz.cpp
  quiz_io.cpp
  segment.cpp
  session.cpp
  sysfunc.cpp
  team.cpp
  lineparser.h
  messenger.h
  question.h
  questionset.h
  quiz.h
  segment.h
  session.h
  sysfunc.h
  team.h
)

include_directories(
../
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_BINARY_DIR}/src
)
