cmake_minimum_required(VERSION 3.16)
project(TRIGAEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable verbose output for debugging
set(CMAKE_VERBOSE_MAKEFILE ON)

# ============================================================================
# COMPILER OPTIONS
# ============================================================================

if(MSVC)
    add_compile_options(/W4 /WX- /permissive- /MP /bigobj)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Oi /GL")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
endif()

if(UNIX AND NOT APPLE)
    add_compile_options(-Wall -Wextra -Wpedantic)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

if(APPLE)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

# ============================================================================
# PLATFORM DEFINITIONS - CRITICAL FOR COMPILATION
# ============================================================================

if(WIN32)
    add_compile_definitions(TRIGA_PLATFORM_WINDOWS)
    message(STATUS "Platform: Windows")
elseif(APPLE)
    add_compile_definitions(TRIGA_PLATFORM_MACOS)
    message(STATUS "Platform: macOS")
elseif(UNIX)
    add_compile_definitions(TRIGA_PLATFORM_LINUX)
    message(STATUS "Platform: Linux")
endif()

# ============================================================================
# FIND REQUIRED PACKAGES
# ============================================================================

find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

# ============================================================================
# FETCH EXTERNAL DEPENDENCIES
# ============================================================================

include(FetchContent)

# GLFW - Window management
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
)

# GLM - Math library
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE
)

# spdlog - Logging
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.12.0
    GIT_SHALLOW TRUE
)

# ImGui - UI framework
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.7
    GIT_SHALLOW TRUE
)

# GLAD - OpenGL loader (CRITICAL - WAS MISSING!)
FetchContent_Declare(glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG main
    GIT_SHALLOW TRUE
)

# Make all dependencies available
FetchContent_MakeAvailable(glfw glm spdlog imgui glad)

# ============================================================================
# RECURSIVE SOURCE FILE COLLECTION - THIS WAS THE MAIN PROBLEM
# ============================================================================

file(GLOB_RECURSE TRIGA_SOURCES
    "src/**/*.cpp"
    "src/**/*.cc"
    "src/**/*.cxx"
)

file(GLOB_RECURSE TRIGA_HEADERS
    "include/**/*.h"
    "include/**/*.hpp"
    "src/**/*.h"
    "src/**/*.hpp"
)

list(LENGTH TRIGA_SOURCES SOURCE_COUNT)
list(LENGTH TRIGA_HEADERS HEADER_COUNT)

message(STATUS "======================================")
message(STATUS "TRIGA Engine Build Configuration")
message(STATUS "======================================")
message(STATUS "Found ${SOURCE_COUNT} source files (.cpp/.cc/.cxx)")
message(STATUS "Found ${HEADER_COUNT} header files (.h/.hpp)")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Output Directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "======================================")

# Print all source files for verification
message(VERBOSE "Source files:")
foreach(FILE ${TRIGA_SOURCES})
    message(VERBOSE "  ${FILE}")
endforeach()

# ============================================================================
# CREATE EXECUTABLE WITH ALL SOURCES
# ============================================================================

add_executable(triga_editor ${TRIGA_SOURCES} ${TRIGA_HEADERS})

# Set properties for debugging
set_target_properties(triga_editor PROPERTIES
    DEBUG_POSTFIX "_d"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# ============================================================================
# INCLUDE DIRECTORIES - NOW INCLUDING GLAD
# ============================================================================

target_include_directories(triga_editor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${glm_SOURCE_DIR}
    ${imgui_SOURCE_DIR}
    ${spdlog_SOURCE_DIR}/include
    ${glad_SOURCE_DIR}/include
    ${OPENGL_INCLUDE_DIR}
)

# ============================================================================
# LINK LIBRARIES - NOW INCLUDING GLAD AND IMGUI
# ============================================================================

target_link_libraries(triga_editor PRIVATE
    glfw
    OpenGL::GL
    imgui
    spdlog::spdlog
    glad
    Threads::Threads
)

# ============================================================================
# PLATFORM-SPECIFIC SETTINGS
# ============================================================================

if(WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set_target_properties(triga_editor PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
        WIN32_EXECUTABLE OFF
    )
    
    # Generate PDB files for debugging
    if(MSVC)
        target_compile_options(triga_editor PRIVATE /Zi)
        set_target_properties(triga_editor PROPERTIES
            COMPILE_PDB_NAME_DEBUG "triga_editor_d"
            COMPILE_PDB_NAME_RELEASE "triga_editor"
        )
    endif()
endif()

if(UNIX)
    set_target_properties(triga_editor PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )
endif()

# ============================================================================
# INSTALLATION
# ============================================================================

install(TARGETS triga_editor RUNTIME DESTINATION bin)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../assets")
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../assets"
            DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/..")
endif()

message(STATUS "======================================")
message(STATUS "Build configured successfully!")
message(STATUS "======================================")
