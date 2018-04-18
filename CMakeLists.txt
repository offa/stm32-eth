cmake_minimum_required(VERSION 3.10.1)

project(stm32-eth VERSION 0.8.0)
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


if( ${BUILD_TYPE_DEBUG} )
    add_definitions(-DDEBUG -DUSE_FULL_ASSERT -DTRACE)
endif()

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


add_definitions(-DSTM32F407xx
                -DUSE_HAL_DRIVER
                -DHSE_VALUE=8000000
                -DOS_USE_TRACE_SEMIHOSTING_DEBUG
                )
include_directories(SYSTEM "system/include"
                            "system/include/stm32f4-hal"
                            "system/include/cmsis"
                            )

if( INTEGRATIONTEST OR BUILD_SYSTEM_DRIVER )
    add_subdirectory("system")
endif()


find_package(GSL REQUIRED)
set_target_properties(GSL::GSL PROPERTIES INTERFACE_COMPILE_DEFINITIONS GSL_UNENFORECED_ON_CONTRACT_VIOLATION)


include_directories("include")
add_subdirectory("src")


if( UNITTEST )
    enable_testing()
    add_subdirectory("test")
endif()

if( INTEGRATIONTEST )
    add_subdirectory("test/integration")
endif()


