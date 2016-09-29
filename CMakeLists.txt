cmake_minimum_required(VERSION 3.0)

project(stm32-eth VERSION 0.0.5)
message(STATUS "~~~ ${PROJECT_NAME} v${PROJECT_VERSION} ~~~")

message(STATUS "Target: ${CMAKE_SYSTEM_PROCESSOR} (${CMAKE_SYSTEM_NAME})")
message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")

option(UNITTEST "Build Unit Tests" ON)
message(STATUS "Build Unit Tests : ${UNITTEST}")

option(UNITTEST_VERBOSE "Verbose Unit Tests" OFF)
message(STATUS "Verbose Unit Tests : ${UNITTEST_VERBOSE}")

option(COVERAGE "Enable Coverage" OFF)
message(STATUS "Enable Coverage : ${COVERAGE}")

option(LTO "Enable Link Time Optimization (LTO)" OFF)
message(STATUS "Enable LTO : ${LTO}")

option(INTEGRATION_TEST "Build Integration Tests" OFF)
message(STATUS "Build Integration Tests: ${INTEGRATION_TEST}")


set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Compiler)
include(Cpp14)
include(Target)

add_common_flag(-Wall -Wextra -pedantic -Werror -Wshadow)


include_directories("include")
add_subdirectory("src")


if( COVERAGE )
    include(Coverage)
endif()

if( UNITTEST )
    enable_testing()
    add_subdirectory("test")
endif()

if( INTEGRATION_TEST )
    add_subdirectory("test/integration")
endif()

