# Collect all project targets ignoring fetched dependencies and interface
# libraries
function(collect_targets var)
    set(targets)
    collect_targets_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
    set(${var} ${targets} PARENT_SCOPE)
endfunction()

macro(collect_targets_recursive targets dir)
    if(NOT ("${dir}" MATCHES "${PROJECT_BINARY_DIR}/_deps/.*"))
        get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
        foreach(subdir ${subdirectories})
            collect_targets_recursive(${targets} ${subdir})
        endforeach()

        get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
        string(REGEX REPLACE "^$ENV{ROFI_ROOT}/" "" path ${dir})

        foreach(target ${current_targets})
            get_target_property(type ${target} TYPE)
            if (NOT ${type} STREQUAL "INTERFACE_LIBRARY")
                list(TRANSFORM target PREPEND "${path}:")
                list(APPEND ${targets} ${target})
            endif()
        endforeach()
    endif()
endmacro()

function(list_targets)
    collect_targets(targets)
    string(JOIN "\n" t ${targets})
    file(WRITE "${PROJECT_BINARY_DIR}/targets.txt" "${t}")
endfunction()

# Collect all dependent targets of a target
function(collect_dependencies target var)
    set(deps)
    collect_dependencies_recursive("${target}" deps)
    set(filtered_deps)
    set(${var} ${deps} PARENT_SCOPE)
endfunction()

macro(collect_dependencies_recursive target deps)
    get_target_property(type ${target} TYPE)
    if (${type} STREQUAL "INTERFACE_LIBRARY")
        get_target_property(target_deps "${target}" INTERFACE_LINK_LIBRARIES)
    else()
        get_target_property(target_deps "${target}" LINK_LIBRARIES)
    endif()

    foreach(d ${target_deps})
        list(FIND deps "${d}" was_found)
        if (${was_found} EQUAL -1 AND TARGET "${d}")
            get_target_property(type ${d} TYPE)
            if (${type} STREQUAL "INTERFACE_LIBRARY")
                get_target_property(sources "${d}" INTERFACE_SOURCES)
            else()
                get_target_property(sources "${d}" SOURCES)
            endif()
            if (sources)
                list(APPEND deps ${d})
            endif()
            collect_dependencies_recursive("${d}" deps)
        endif()
    endforeach()
endmacro()

function(list_dependencies)
    collect_targets(targets)
    set(content)
    foreach(t ${targets})
        string(REGEX REPLACE ".*:" "" t ${t})
        collect_dependencies("${t}" deps)
        string(APPEND content "${t}: ${deps}\n")
    endforeach()
    file(WRITE "${PROJECT_BINARY_DIR}/dependencies.txt" "${content}")
endfunction()

function (list_sources)
collect_targets(targets)
    set(content)
    foreach(t ${targets})
        string(REGEX REPLACE ".*:" "" t ${t})
        get_target_property(type ${t} TYPE)
        if (${type} STREQUAL "INTERFACE_LIBRARY")
            get_target_property(sources "${t}" INTERFACE_SOURCES)
        else()
            get_target_property(sources "${t}" SOURCES)
        endif()
        get_target_property(target_dir "${t}" SOURCE_DIR)
        set(absolute_sources)
        foreach(s ${sources})
            # file(REAL_PATH "${s}" as BASE_DIRECTORY "${target_dir}")
            get_filename_component(as "${s}" REALPATH BASE_DIR "${target_dir}")
            if(NOT ("${as}" MATCHES "${PROJECT_BINARY_DIR}/_deps/.*"))
                list(APPEND absolute_sources "${as}")
            endif()
        endforeach(s ${sources})

        string(APPEND content "${t}: ${absolute_sources}\n")
    endforeach()
    file(WRITE "${PROJECT_BINARY_DIR}/sources.txt" "${content}")
endfunction()