
# Define source groups for IDEs
source_group("Header Files\\geometry" FILES
    ${GEOMETRY_HEADERS}
)

source_group("Source Files\\geometry" FILES
    ${GEOMETRY_SOURCES}
)

# Add new combined contours node
set(GEOMETRY_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/geometry/FindContoursNode.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/geometry/ContoursSelectNode.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/geometry/CombinedContoursNode.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/geometry/geometry.h
)

set(GEOMETRY_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/geometry/FindContoursNode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/geometry/ContoursSelectNode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/geometry/CombinedContoursNode.cpp
)

# Combine all headers and sources
set(ALL_HEADERS
    ${IO_HEADERS}
    ${BASICOPS_HEADERS}
    ${FILTER_HEADERS}
    ${DRAW_HEADERS}
    ${GEOMETRY_HEADERS}
    ${MORPH_HEADERS}
    ${PROCESSING_HEADER}
)

set(ALL_SOURCES
    ${IO_SOURCES}
    ${BASICOPS_SOURCES}
    ${FILTER_SOURCES}
    ${DRAW_SOURCES}
    ${GEOMETRY_SOURCES}
    ${MORPH_SOURCES}
)

# Define source groups for IDEs
source_group("Header Files\\Filter" FILES
    ${FILTER_HEADERS}
)

source_group("Source Files\\Filter" FILES
    ${FILTER_SOURCES}
)

# Add new filter node
set(FILTER_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Filter/BilateralFilter.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Filter/GaussianBlur.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Filter/MedianBlur.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Filter/SobelGradientNode.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Filter/ThresholdNode.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Filter/ColorAnomalyDetectionNode.hpp  # Added new node header
)

set(FILTER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Filter/BilateralFilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Filter/GaussianBlur.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Filter/MedianBlur.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Filter/SobelGradientNode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Filter/ThresholdNode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Filter/ColorAnomalyDetectionNode.cpp  # Added new node source
)
