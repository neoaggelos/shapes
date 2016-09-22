# Locate SDL2_mixer library
# This module defines
# SDL2_MIXER_LIBRARY, the name of the library to link against
# SDL2_MIXER_FOUND, if false, do not link to SDL2_mixer
# SDL2_MIXER_INCLUDE_DIR, where to find SDL_mixer.h
#
# $SDL2_MIXER_DIR is an environment variable that would correspond to the:
#   # ./configure --prefix=$SDL2_MIXER_DIR
# used when building the library
#
# -- Aggelos Kolaitis

set(SDL2_MIXER_SEARCH_PATHS
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
    SDL2_MIXER_INCLUDE_DIR SDL_mixer.h
    HINTS $ENV{SDL2_MIXER_DIR}
    PATH_SUFFIXES include/SDL2_mixer include/SDL2 include
    PATHS ${SDL2_MIXER_SEARCH_PATHS}
)

find_library(SDL2_MIXER_LIBRARY
    NAMES SDL2_mixer
    HINTS
    $ENV{SDL2_MIXER_DIR}
    PATH_SUFFIXES lib64 lib
    PATHS ${SDL2_MIXER_SEARCH_PATHS}
    DOC "SDL2_mixer library path"
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
    SDL2_MIXER REQUIRED_VARS SDL2_MIXER_LIBRARY SDL2_MIXER_INCLUDE_DIR
)

if(SDL2_MIXER_LIBRARY AND SDL2_MIXER_INCLUDE_DIR)
    set(SDL2_MIXER_FOUND "YES")
endif(SDL2_MIXER_LIBRARY AND SDL2_MIXER_INCLUDE_DIR)


