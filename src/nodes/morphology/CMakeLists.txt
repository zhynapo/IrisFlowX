
file(GLOB_RECURSE MORPH_SRC CONFIGURE_DEPENDS
    *.cpp
)

add_library(IrisFlowNodesMorphology ${MORPH_SRC})

target_include_directories(IrisFlowNodesMorphology
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(IrisFlowNodesMorphology
    PUBLIC
        IrisFlowFramework
        OpenCV::opencv_world
)