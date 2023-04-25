cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)

if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
endif()

set(LIBRARY_NAME libdart_vlc_core)
project(${LIBRARY_NAME} LANGUAGES C CXX)

SET(LIBRARY_OUTPUT_PATH ${TEMP_OUT_PATH})

set(LIBVLC_FILES
    sdk
    libvlc.dll
    libvlccore.dll
    plugins
)
set(LIBVLC_ARCHIVE_ROOT "vlc-${LIBVLC_VERSION}/")
list(TRANSFORM LIBVLC_FILES PREPEND ${LIBVLC_ARCHIVE_ROOT})

add_library(${LIBRARY_NAME} STATIC
  core.cc
  api/api.cc
  player/player.cc
  player/player_state.cc
  media_source/media.cc
  media_source/playlist.cc
  equalizer/equalizer.cc
  devices/devices.cc
  broadcast/broadcast.cc
  chromecast/chromecast.cc
  record/record.cc
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${LIBVLCPP_SOURCE}")
include_directories("${LIBVLC_SOURCE}/sdk/include")

ADD_CUSTOM_COMMAND(
   TARGET ${LIBRARY_NAME}
   POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_BIN_DIR}
   COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_BIN_DIR}/plugins
   COMMAND ${CMAKE_COMMAND} -E copy_directory ${LIBVLC_SOURCE}/plugins ${OUTPUT_BIN_DIR}/plugins
   COMMAND ${CMAKE_COMMAND} -E copy ${LIBVLC_SOURCE}/libvlc.dll ${OUTPUT_BIN_DIR}
   COMMAND ${CMAKE_COMMAND} -E copy ${LIBVLC_SOURCE}/libvlccore.dll ${OUTPUT_BIN_DIR}

   COMMAND ${CMAKE_COMMAND} -E make_directory ${TEMP_OUT_PATH}
   COMMAND ${CMAKE_COMMAND} -E make_directory ${TEMP_OUT_PATH}/plugins
   COMMAND ${CMAKE_COMMAND} -E copy_directory ${LIBVLC_SOURCE}/plugins ${TEMP_OUT_PATH}/plugins
   COMMAND ${CMAKE_COMMAND} -E copy ${LIBVLC_SOURCE}/libvlc.dll ${TEMP_OUT_PATH}
   COMMAND ${CMAKE_COMMAND} -E copy ${LIBVLC_SOURCE}/libvlccore.dll ${TEMP_OUT_PATH}
) 