cmake_minimum_required(VERSION 3.12)

project(stm32-eth VERSION 0.9.7)
message(STATUS "~~~ ${PROJECT_NAME} v${PROJECT_VERSION} ~~~")

message(STATUS "Target: ${CMAKE_SYSTEM_PROCESSOR} (${CMAKE_SYSTEM_NAME})")
message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(CMakeParseArguments)
include(Options)
include(BuildType)
include(CompilerOptions)
include(UtilityTargets)
include(ClangFormat)
include(Sanitizer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_common_flags(-Wall
    -Wextra
    -pedantic
    -pedantic-errors
    -Werror
    -Wshadow
    )
add_cpp_flags(-Wold-style-cast
    -Wnull-dereference
    -Wnon-virtual-dtor
    -Woverloaded-virtual
    )

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0)
    message(STATUS "GCC < 10.0 doesn't support Concepts with C++20, fall back to '-fconcepts'")
    add_cpp_flags(-fconcepts)
endif()


if( COVERAGE )
    include(Coverage)
endif()

if( LTO )
    include(LTO)
endif()

if( CMAKE_CROSSCOMPILING )
    set(CMAKE_EXECUTABLE_SUFFIX ".elf")
endif()


add_subdirectory("system")

include_directories("include")
add_subdirectory("src")


if( UNITTEST )
    enable_testing()
    add_subdirectory("test")
endif()

if( INTEGRATIONTEST )
    add_subdirectory("test/integration")
endif()
