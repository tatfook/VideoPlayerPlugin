# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
#   set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
# endif()

# if(CMAKE_BUILD_TYPE)
# message("aaaaa CMAKE_BUILD_TYPE:")
# message(${CMAKE_BUILD_TYPE})
# endif()


#SET(CMAKE_DEBUG_POSTFIX "_d")

project ("VideoPlayerPlugin")

set(project_root_path ${PROJECT_SOURCE_DIR})
set(TEMP_OUT_PATH ${project_root_path}/out/temp_output)

SET(EXECUTABLE_OUTPUT_PATH ${TEMP_OUT_PATH})
SET(LIBRARY_OUTPUT_PATH ${TEMP_OUT_PATH})

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
set(OUTPUT_BIN_DIR   ${project_root_path}/release/bin64/Mod/VideoPlayerPlugin)
else()
set(OUTPUT_BIN_DIR   ${project_root_path}/release/Mod/VideoPlayerPlugin)
endif()

set(DART_CORE_NAME dart_vlc_core)
set(DART_CORE_FLODERPATH ${project_root_path}/${DART_CORE_NAME})

# Note: include the ParaEngine/NPLRuntime include directory
if ("$ENV{NPLRUNTIME_ROOT}" STREQUAL "")
	set(ParaEngineClient_SOURCE_DIR ${CLIENT_SOURCE_DIR}/trunk/ParaEngineClient)
else()
	set(ParaEngineClient_SOURCE_DIR $ENV{NPLRUNTIME_ROOT}/Client/trunk/ParaEngineClient)
endif()

set(PACKAGE_DIR "${project_root_path}/extern_packages")

set(LIBVLC_VERSION "3.0.16")


if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set(LIBVLC_SOURCE "${PACKAGE_DIR}/vlc-${LIBVLC_VERSION}/win64")
else()
    set(LIBVLC_SOURCE "${PACKAGE_DIR}/vlc-${LIBVLC_VERSION}/win32")
endif()
set(LIBVLCPP_SOURCE "${PACKAGE_DIR}/libvlcpp-master")

#set(VLC_PLUGIN_PATH ${LIBVLC_SOURCE}/plugins)

link_directories("${TEMP_OUT_PATH}")
link_directories("${LIBVLC_SOURCE}/sdk/lib/")

# if(CMAKE_BUILD_TYPE)
# link_directories("${TEMP_OUT_PATH}/${CMAKE_BUILD_TYPE}")
# endif()

set (BoostRoot_DIR "$ENV{BOOST_ROOT}")

# 包含子项目。
add_subdirectory ("VideoPlayerPlugin")
add_subdirectory ("dart_vlc_core")
add_subdirectory ("TestCpp")