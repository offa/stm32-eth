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


if( LTO )
    add_common_flag(-flto)
    add_linker_flag(-flto)
endif()


if( CMAKE_BUILD_TYPE )
    string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

    if( "${BUILD_TYPE}" STREQUAL "debug")
        add_definitions(-DDEBUG -DUSE_FULL_ASSERT -DTRACE)
    endif()
endif()


add_common_flag(-Wall -Wextra -pedantic -Werror -Wshadow)

if( COVERAGE )
    include(Coverage)
endif()


add_definitions(-DSTM32F407xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -DOS_USE_TRACE_SEMIHOSTING_DEBUG)
include_directories(SYSTEM "system/include"
                            "system/include/stm32f4-hal"
                            "system/include/cmsis"
                            )
if( CMAKE_CROSSCOMPILING OR INTEGRATION_TEST )
    add_subdirectory("system")
endif()


include_directories("include")
add_subdirectory("src")


if( UNITTEST )
    enable_testing()
    add_subdirectory("test")
endif()

if( INTEGRATION_TEST )
    add_subdirectory("test/integration")
endif()

