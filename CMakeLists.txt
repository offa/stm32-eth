cmake_minimum_required(VERSION 3.12)

project(stm32-eth VERSION 0.9.1)
message(STATUS "~~~ ${PROJECT_NAME} v${PROJECT_VERSION} ~~~")

message(STATUS "Target: ${CMAKE_SYSTEM_PROCESSOR} (${CMAKE_SYSTEM_NAME})")
message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(CMakeParseArguments)
include(Options)
include(BuildType)
include(CompilerOptions)
include(Compiler)
include(UtilityTargets)
include(ClangFormat)

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


if( COVERAGE )
    include(Coverage)
endif()

if( LTO )
    include(LTO)
endif()

include(Sanitizer)

add_subdirectory("system")


find_package(Microsoft.GSL CONFIG REQUIRED)

if( GSL_CONTRACT_UNENFORCED )
    target_compile_definitions(Microsoft.GSL::GSL INTERFACE GSL_UNENFORECED_ON_CONTRACT_VIOLATION)
endif()


include_directories("include")
add_subdirectory("src")


if( UNITTEST )
    enable_testing()
    add_subdirectory("test")
endif()

if( INTEGRATIONTEST )
    add_subdirectory("test/integration")
endif()
