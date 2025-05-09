cmake_minimum_required(VERSION 3.24)
project(8080EMU C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set the output directory for all build configurations
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Add platform-specific configurations
if(WIN32)
    message(STATUS "Configuring for Windows")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /DWIN32 /O2")
elseif(APPLE)
    message(STATUS "Configuring for macOS")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DAPPLE -O2")
elseif(UNIX)
    message(STATUS "Configuring for Unix/Linux")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUNIX -O2")
endif()

# Add custom target to initialize and update git submodules
find_package(Git REQUIRED)
if(GIT_FOUND)
    message(STATUS "Git found: ${GIT_EXECUTABLE}")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "Failed to initialize git submodules")
    endif()
endif()

# Add SDL3 subdirectory
add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)

# Define source files for different builds
set(SPACEINVADERS_SOURCES src/spaceinvaders.c src/cpu.c src/emulation.c)
set(CPU_TEST_SOURCES src/cpu_test.c src/cpu.c src/emulation.c)

# Release build
add_executable(space_release ${SPACEINVADERS_SOURCES})
target_compile_definitions(space_release PRIVATE NDEBUG)
target_compile_options(space_release PRIVATE -O3)
target_link_libraries(space_release PRIVATE SDL3::SDL3)

# Debug build
add_executable(space_debug ${SPACEINVADERS_SOURCES})
target_compile_options(space_debug PRIVATE -g)
target_link_libraries(space_debug PRIVATE SDL3::SDL3)

# Test build (no SDL3 linking)
add_executable(space_test ${CPU_TEST_SOURCES})
target_compile_definitions(space_test PRIVATE NDEBUG)
target_compile_options(space_test PRIVATE -O3)
