# Locate SDL2_ttf library
# This module defines
# SDL2_TTF_LIBRARY, the name of the library to link against
# SDL2_TTF_FOUND, if false, do not link to SDL2_ttf
# SDL2_TTF_INCLUDE_DIR, where to find SDL_ttf.h
#
# $SDL2_TTF_DIR is an environment variable that would correspond to the:
#   # ./configure --prefix=$SDL2_TTF_DIR
# used when building the library
#
# -- Aggelos Kolaitis

set(SDL2_TTF_SEARCH_PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local
    /usr
    /sw
    /opt/local
    /opt/csw
    /opt
)

find_path(
    SDL2_TTF_INCLUDE_DIR SDL_ttf.h
    HINTS $ENV{SDL2_TTF_DIR}
    PATH_SUFFIXES include/SDL2_ttf include/SDL2 include
    PATHS ${SDL2_TTF_SEARCH_PATHS}
)

find_library(SDL2_TTF_LIBRARY
    NAMES SDL2_ttf
    HINTS
    $ENV{SDL2_TTF_DIR}
    PATH_SUFFIXES lib64 lib
    PATHS ${SDL2_TTF_SEARCH_PATHS}
    DOC "SDL2_ttf library path"
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    SDL2_TTF REQUIRED_VARS SDL2_TTF_LIBRARY SDL2_TTF_INCLUDE_DIR
)

if(SDL2_TTF_LIBRARY AND SDL2_TTF_INCLUDE_DIR)
    set(SDL2_TTF_FOUND "YES")
endif(SDL2_TTF_LIBRARY AND SDL2_TTF_INCLUDE_DIR)


