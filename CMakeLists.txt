cmake_minimum_required(VERSION 3.11)

project(stm32-eth VERSION 0.9.0)
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

add_common_flags(-Wall
                -Wextra
                -pedantic
                -Werror
                -Wshadow
                )
add_cpp_flags(-Wold-style-cast)


if( COVERAGE )
    include(Coverage)
endif()

if( LTO )
    include(LTO)
endif()

include(Sanitizer)

add_subdirectory("system")


find_package(GSL REQUIRED)

if( GSL_CONTRACT_UNENFORCED )
    target_compile_definitions(GSL::GSL INTERFACE GSL_UNENFORECED_ON_CONTRACT_VIOLATION)
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


