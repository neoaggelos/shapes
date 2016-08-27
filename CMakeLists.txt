cmake_minimum_required(VERSION 2.8)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)
project(Shapes)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include("${CMAKE_SOURCE_DIR}/cmake/FindSDLU.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/FindSDL2.cmake")

find_package(SDL2 REQUIRED)
find_package(SDLU REQUIRED)
include_directories(${SDL2_INCLUDE_DIR} ${SDLU_INCLUDE_DIR})
link_libraries(${SDL2MAIN_LIBRARY} ${SDL2_LIBRARY} ${SDLU_LIBRARY})

file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*)
file(GLOB TXT_FILES ${CMAKE_SOURCE_DIR}/*.txt)

add_executable(Shapes WIN32 ${SOURCES} ${TXT_FILES})
link_libraries(${LIBS})

install(TARGETS Shapes DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

## TODO assets directory
