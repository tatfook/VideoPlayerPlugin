# CMakeList.txt: VideoPlayerPlugin 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

project (VideoPlayerPlugin)

SET(VideoPlayerPlugin_SRCS
	VideoPlayerPlugin.cpp
	VideoPlayerPlugin.h
	VideoPlayerPlugin.def

    VideoHelper.h
    VideoHelper.cpp
)

# add the binary tree to the search path for include files
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${DART_CORE_FLODERPATH}")
include_directories("${ParaEngineClient_SOURCE_DIR}")
include_directories("${ParaEngineClient_SOURCE_DIR}/Core")
include_directories("${ParaEngineClient_SOURCE_DIR}/util")
include_directories("${LIBVLCPP_SOURCE}")
include_directories("${LIBVLC_SOURCE}/sdk/include")
include_directories("${TEMP_OUT_PATH}")
include_directories("${BoostRoot_DIR}")

# 将源代码添加到此项目的可执行文件。
add_library (VideoPlayerPlugin SHARED ${VideoPlayerPlugin_SRCS})

target_link_libraries(VideoPlayerPlugin PRIVATE
    libvlc
    libvlccore

	libdart_vlc_core
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET VideoPlayerPlugin PROPERTY CXX_STANDARD 20)
endif()

#set_target_properties(VideoPlayerPlugin PROPERTIES OUTPUT_NAME "VideoPlayerPlugin")
#set_target_properties(VideoPlayerPlugin PROPERTIES DEBUG_OUTPUT_NAME "VideoPlayerPlugin_d")

if(MSVC)
	# disable SAFESEH
    SET(CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS} /D \"SAFESEH:NO\"")
    SET(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
    SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")
endif()

# Copy files Link the executable to the libraries. 
ADD_CUSTOM_COMMAND(
   TARGET VideoPlayerPlugin
   POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_BIN_DIR}
   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:VideoPlayerPlugin> ${OUTPUT_BIN_DIR}
)

SET(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)