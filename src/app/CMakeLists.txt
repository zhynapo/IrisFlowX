file(GLOB_RECURSE APP_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)
file(GLOB_RECURSE APP_SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graph/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graph/subgraph/*.cpp"
)

qt_add_executable(IrisFlowApp ${APP_SRC} ${APP_HEADERS})

target_include_directories(IrisFlowApp
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(IrisFlowApp
    PRIVATE
        FlowCore 
#        nodes_image  # 链接具体的节点库
        common
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        ${OpenCV_LIBS}
        IrisX::Atlas
)

