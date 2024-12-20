
# nodes/CMakeLists.txt

# 1. 定义一个公共的接口库 (INTERFACE)，仅用于包含路径
# 这样 image, morphology 等模块只需要 link 这个 target 就能拿到 Data.hpp
add_library(nodes_common_headers INTERFACE)
target_include_directories(nodes_common_headers INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set_target_properties(nodes_common_headers PROPERTIES FOLDER "nodes")



# 2. 进入子模块
add_subdirectory(image)
add_subdirectory(core)
add_subdirectory(processing)
add_subdirectory(analysis)
add_subdirectory(vis)
add_subdirectory(utils)
#add_subdirectory(morphology)
# add_subdirectory(basic)

#add_subdirectory(basic)
#add_subdirectory(image)
#add_subdirectory(morphology)
#add_subdirectory(geometry)
#add_subdirectory(io)
#add_subdirectory(atlas)