cmake_minimum_required(VERSION 3.0)

project(stm32-eth VERSION 0.3.0)
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
message(STATUS "Build Integration Tests : ${INTEGRATION_TEST}")

if( CMAKE_CROSSCOMPILING )
    set(CONTRACTS_UNENFORCED ON)
else()
    set(CONTRACTS_UNENFORCED OFF)
endif()

option(GSL_CONTRACT_UNENFORCED "GSL contract violations unenforced" ${CONTRACTS_UNENFORCED})
message(STATUS "Unenforced GSL contracts : ${GSL_CONTRACT_UNENFORCED}")


set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(BuildType)
include(Compiler)
include(Cpp14)


if( ${BUILD_TYPE_DEBUG} )
    add_definitions(-DDEBUG -DUSE_FULL_ASSERT -DTRACE)
endif()


add_common_flag(-Wall
                -Wextra
                -pedantic
                -Werror
                -Wshadow
                )

if( COVERAGE )
    include(Coverage)
endif()


add_definitions(-DSTM32F407xx
                -DUSE_HAL_DRIVER
                -DHSE_VALUE=8000000
                -DOS_USE_TRACE_SEMIHOSTING_DEBUG
                )
include_directories(SYSTEM "system/include"
                            "system/include/stm32f4-hal"
                            "system/include/cmsis"
                            )

if( CMAKE_CROSSCOMPILING OR INTEGRATION_TEST )
    add_subdirectory("system")
endif()


find_package(GSL REQUIRED)
include_directories(${GSL_INCLUDE_DIR})

if( GSL_CONTRACT_UNENFORCED )
    add_definitions(-DGSL_UNENFORCED_ON_CONTRACT_VIOLATION)
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

