# CMakeList.txt: TestCpp 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

project (TestCpp)

include_directories("${project_root_path}/VideoPlayerPlugin")

include_directories("${ParaEngineClient_SOURCE_DIR}")
include_directories("${ParaEngineClient_SOURCE_DIR}/Core")
include_directories("${ParaEngineClient_SOURCE_DIR}/util")

#add_definitions(-PE_CORE_EXPORTING)

# 将源代码添加到此项目的可执行文件。
add_executable (TestCpp "main.cpp" )

target_link_libraries(TestCpp PRIVATE
    VideoPlayerPlugin
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TestCpp PROPERTY CXX_STANDARD 20)
endif()

set_target_properties(TestCpp PROPERTIES OUTPUT_NAME "TestCpp")
set_target_properties(TestCpp PROPERTIES DEBUG_OUTPUT_NAME "TestCpp_d")
