# Common utilities for node modules
# Provide a library target `nodes_utils` so other node modules can link against it
add_library(nodes_utils STATIC
    src/utils.cpp
)

set_target_properties(nodes_utils PROPERTIES FOLDER "nodes/utils")

target_include_directories(nodes_utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(nodes_utils
    PUBLIC
        nodes_common_headers
        ${OpenCV_LIBS}
)
