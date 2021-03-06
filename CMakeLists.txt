cmake_minimum_required(VERSION 2.8)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)
project(Shapes)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include("${CMAKE_SOURCE_DIR}/cmake/FindSDLU.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/FindSDL2_mixer.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/FindSDL2_ttf.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/FindSDL2.cmake")

option(PORTABLE "Save scores and settings in the same folder as the executable" OFF)
option(STANDALONE "Load assets from current directory and not source directory" ON)
if (APPLE)
  option(MACOSX_BUNDLE "Build Shapes.app" ON)
endif(APPLE)

find_package(SDL2 REQUIRED)
find_package(SDLU REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)
include_directories(${SDL2_INCLUDE_DIR} ${SDLU_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})
link_libraries(${SDL2MAIN_LIBRARY} ${SDL2_LIBRARY} ${SDLU_LIBRARY} ${SDL2_MIXER_LIBRARY} ${SDL2_TTF_LIBRARY})

file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*)
file(GLOB RC ${CMAKE_SOURCE_DIR}/resource.rc)
file(GLOB ICNS ${CMAKE_SOURCE_DIR}/assets/icon.icns)
file(GLOB TXT_FILES ${CMAKE_SOURCE_DIR}/*.txt)
file(GLOB BMPASSETS ${CMAKE_SOURCE_DIR}/assets/themes/*)
file(GLOB WAVASSETS ${CMAKE_SOURCE_DIR}/assets/wav/*)

source_group(doc FILES ${TXT_FILES})
source_group(assets FILES ${BMPASSETS} ${WAVASSETS})

if (PORTABLE)
  add_definitions("-DPORTABLE")
endif(PORTABLE)

if (WIN32)
  add_executable(Shapes WIN32 ${SOURCES} ${RC} ${TXT_FILES} ${BMPASSETS} ${WAVASSETS})
  if (NOT STANDALONE)
	add_definitions("-DASSETSDIR=\"${CMAKE_SOURCE_DIR}/assets/\"")
  endif()
  install(TARGETS Shapes DESTINATION ${CMAKE_INSTALL_PREFIX})
  install(FILES ${ASSETS} DESTINATION ${CMAKE_INSTALL_PREFIX})
elseif (APPLE AND MACOSX_BUNDLE)
  add_executable(Shapes MACOSX_BUNDLE ${SOURCES} ${TXT_FILES} ${WAVASSETS}
      ${BMPASSETS} ${ICNS})
  set_source_files_properties(${BMPASSETS} PROPERTIES MACOSX_PACKAGE_LOCATION
    "Resources/themes")
  set_source_files_properties(${WAVASSETS} PROPERTIES MACOSX_PACKAGE_LOCATION
    "Resources/wav")
  set_source_files_properties(${ICNS} PROPERTIES MACOSX_PACKAGE_LOCATION
    "Resources")
  set(MACOSX_BUNDLE_ICON_FILE icon.icns)
  install(TARGETS Shapes DESTINATION /Applications)
else()
  add_executable(Shapes ${SOURCES} ${TXT_FILES} ${WAVASSETS} ${BMPASSETS})
  if (STANDALONE)
    add_definitions("-DASSETSDIR=\"${CMAKE_INSTALL_PREFIX}/share/shapes/\"")
  else()
    add_definitions("-DASSETSDIR=\"${CMAKE_SOURCE_DIR}/assets/\"")
  endif()
  install(TARGETS Shapes DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
  install(FILES ${BMPASSETS} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/shapes/themes)
  install(FILES ${WAVASSETS} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/shapes/wav)
endif()

# set startup project for Vs
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Shapes)
