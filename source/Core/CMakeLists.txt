# Core components CMakeLists.txt
# This file organizes all the core components of IronOS

# Add subdirectories for core components with custom binary directories
add_subdirectory(Src ${CMAKE_CURRENT_BINARY_DIR}/Src)
add_subdirectory(Threads ${CMAKE_CURRENT_BINARY_DIR}/Threads)
add_subdirectory(Drivers ${CMAKE_CURRENT_BINARY_DIR}/Drivers)
add_subdirectory(BSP ${CMAKE_CURRENT_BINARY_DIR}/BSP)
add_subdirectory(brieflz ${CMAKE_CURRENT_BINARY_DIR}/brieflz)
add_subdirectory(LangSupport ${CMAKE_CURRENT_BINARY_DIR}/LangSupport)

# Include directories for the core components
list(APPEND IRONOS_INCLUDE_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/Inc"
    "${CMAKE_CURRENT_SOURCE_DIR}/BSP"
)

# Add model-specific BSP directory and its subdirectories
list(APPEND IRONOS_INCLUDE_DIRS "${DEVICE_BSP_DIR}")

# Find all BSP include directories for the current model
file(GLOB_RECURSE BSP_INCLUDE_DIRS
    LIST_DIRECTORIES true
    "${DEVICE_BSP_DIR}/*"
)
foreach(DIR ${BSP_INCLUDE_DIRS})
    if(IS_DIRECTORY ${DIR})
        list(APPEND IRONOS_INCLUDE_DIRS "${DIR}")
    endif()
endforeach()

# Find all Threads include directories
file(GLOB_RECURSE THREADS_INCLUDE_DIRS
    LIST_DIRECTORIES true
    "${CMAKE_CURRENT_SOURCE_DIR}/Threads/*"
)
foreach(DIR ${THREADS_INCLUDE_DIRS})
    if(IS_DIRECTORY ${DIR})
        list(APPEND IRONOS_INCLUDE_DIRS "${DIR}")
    endif()
endforeach()

# Find all Drivers include directories
file(GLOB_RECURSE DRIVERS_INCLUDE_DIRS
    LIST_DIRECTORIES true
    "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/*"
)
foreach(DIR ${DRIVERS_INCLUDE_DIRS})
    if(IS_DIRECTORY ${DIR})
        list(APPEND IRONOS_INCLUDE_DIRS "${DIR}")
    endif()
endforeach()

# Find all brieflz include directories
file(GLOB_RECURSE BRIEFLZ_INCLUDE_DIRS
    LIST_DIRECTORIES true
    "${CMAKE_CURRENT_SOURCE_DIR}/brieflz/*"
)
foreach(DIR ${BRIEFLZ_INCLUDE_DIRS})
    if(IS_DIRECTORY ${DIR})
        list(APPEND IRONOS_INCLUDE_DIRS "${DIR}")
    endif()
endforeach()

# Set up include directories globally for all targets
include_directories(${IRONOS_INCLUDE_DIRS})

# Make include directories available to parent scope
set(IRONOS_INCLUDE_DIRS ${IRONOS_INCLUDE_DIRS} PARENT_SCOPE)

# Create translation generation targets
include(${CMAKE_SOURCE_DIR}/cmake/TranslationTargets.cmake)
