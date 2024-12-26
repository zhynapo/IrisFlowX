
# nodes/CMakeLists.txt

include(cmake/add_node_module.cmake)

# 1. 定义一个公共的接口库 (INTERFACE)，仅用于包含路径
# 这样 image, morphology 等模块只需要 link 这个 target 就能拿到 Data.hpp
add_library(nodes_common_headers INTERFACE)
target_include_directories(nodes_common_headers INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set_target_properties(nodes_common_headers PROPERTIES FOLDER "nodes")

# 2. 进入子模块
# add common utilities library and link into core
add_subdirectory(utils)
add_node_module(core nodes_utils common)
add_node_module(Analytics nodes_utils common)
add_node_module(Processing nodes_utils common)

