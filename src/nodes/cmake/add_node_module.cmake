# Helper to create a node module library with standard settings.
# Usage: add_node_module(<targetName> [extra_lib1 extra_lib2 ...])
function(add_node_module targetName)
    # ARGN may contain either subdirectory names (to include sources) or extra libraries to link.

    # allow targetName to contain subdirectory (e.g. core/io). Normalize to forward slashes.
    string(REPLACE "\\" "/" targetPath "${targetName}")
    # strip leading/trailing slashes
    string(REGEX REPLACE "^/+" "" targetPath "${targetPath}")
    string(REGEX REPLACE "/+$" "" targetPath "${targetPath}")
    # library target name must be a single token: replace '/' with '_'
    string(REPLACE "/" "_" targetBase "${targetPath}")

    # collect source directories: start with the main target path
    set(source_dirs "${CMAKE_CURRENT_SOURCE_DIR}/${targetPath}")
    # if the target directory contains conventional 'src' or 'include' layout,
    # automatically include their subdirectories as source/include roots.
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${targetPath}/src")
        # append each direct subdirectory under src (and src root)
        list(APPEND source_dirs "${CMAKE_CURRENT_SOURCE_DIR}/${targetPath}/src")
        file(GLOB src_subdirs RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/${targetPath}/src" "${CMAKE_CURRENT_SOURCE_DIR}/${targetPath}/src/*")
        foreach(sd IN LISTS src_subdirs)
            if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${targetPath}/src/${sd}")
                list(APPEND source_dirs "${CMAKE_CURRENT_SOURCE_DIR}/${targetPath}/src/${sd}")
            endif()
        endforeach()
    endif()
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${targetPath}/include")
        # include/include subdirectories will be added to include_dirs later; treat include root as source dir for headers
        list(APPEND source_dirs "${CMAKE_CURRENT_SOURCE_DIR}/${targetPath}/include")
        file(GLOB inc_subdirs RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/${targetPath}/include" "${CMAKE_CURRENT_SOURCE_DIR}/${targetPath}/include/*")
        foreach(id IN LISTS inc_subdirs)
            if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${targetPath}/include/${id}")
                list(APPEND source_dirs "${CMAKE_CURRENT_SOURCE_DIR}/${targetPath}/include/${id}")
            endif()
        endforeach()
    endif()
    set(extra_libs_list "")

    foreach(arg IN LISTS ARGN)
        # if arg refers to an existing directory under CMAKE_CURRENT_SOURCE_DIR, treat it as source dir
        if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${arg}")
            list(APPEND source_dirs "${CMAKE_CURRENT_SOURCE_DIR}/${arg}")
        else()
            list(APPEND extra_libs_list ${arg})
        endif()
    endforeach()

    # gather module sources from all source_dirs
    set(MODULE_SOURCES "")
    foreach(d IN LISTS source_dirs)
        if(EXISTS "${d}")
            file(GLOB_RECURSE DIR_SOURCES "${d}/*.cpp" "${d}/*.cxx" "${d}/*.c" "${d}/*.hpp" "${d}/*.h")
            list(APPEND MODULE_SOURCES ${DIR_SOURCES})
        endif()
    endforeach()

    if(NOT MODULE_SOURCES)
        # Fallback: also try current dir (for compatibility with in-dir CMakeLists)
        file(GLOB MODULE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
    endif()

    add_library(${targetBase} SHARED ${MODULE_SOURCES})
    set_target_properties(${targetBase} PROPERTIES FOLDER "nodes/${targetPath}")

    target_compile_definitions(${targetBase} PRIVATE BUILDING_NODE_PLUGIN)

    # prepare include directories: include each source dir and the base current dir
    list(APPEND include_dirs "${CMAKE_CURRENT_SOURCE_DIR}")
    foreach(d IN LISTS source_dirs)
        if(EXISTS "${d}")
            list(APPEND include_dirs "${d}")
        endif()
    endforeach()
    target_include_directories(${targetBase} PRIVATE ${include_dirs})

    # Always link FlowCore and common headers. Extra libs may include Qt or others.
    if(extra_libs_list)
        target_link_libraries(${targetBase} PRIVATE FlowCore nodes_common_headers ${extra_libs_list})
    else()
        target_link_libraries(${targetBase} PRIVATE FlowCore nodes_common_headers)
    endif()

    # Link OpenCV if available
    if(OpenCV_LIBS)
        target_link_libraries(${targetBase} PRIVATE ${OpenCV_LIBS})
    endif()

    # Post-build: always copy plugin into top-level plugins/nodes so PluginLoader (non-recursive)
    # can find it. Also copy into a namespaced subfolder for organization if targetPath contains '/'.
    add_custom_command(TARGET ${targetBase} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
                "${CMAKE_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/plugins/nodes"
        COMMAND ${CMAKE_COMMAND} -E rename 
                "$<TARGET_FILE:${targetBase}>"
                "${CMAKE_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/plugins/nodes/$<TARGET_FILE_NAME:${targetBase}>"
        COMMENT "Copying ${targetBase} to plugins/nodes"
    )

    # if the target had a subpath, also copy into that subfolder for organization
    string(FIND "${targetPath}" "/" has_slash)
    if(NOT has_slash EQUAL -1)
        add_custom_command(TARGET ${targetBase} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
                    "${CMAKE_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/plugins/nodes/${targetPath}"
            COMMAND ${CMAKE_COMMAND} -E rename
                    "${CMAKE_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/plugins/nodes/$<TARGET_FILE_NAME:${targetBase}>"
                    "${CMAKE_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/plugins/nodes/${targetPath}/$<TARGET_FILE_NAME:${targetBase}>"
            COMMENT "Moving ${targetBase} to plugins/nodes/${targetPath}"
        )
    endif()
endfunction()
