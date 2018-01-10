cmake_minimum_required(VERSION 3.9.4)

project(stm32-eth VERSION 0.7.1)
message(STATUS "~~~ ${PROJECT_NAME} v${PROJECT_VERSION} ~~~")

message(STATUS "Target: ${CMAKE_SYSTEM_PROCESSOR} (${CMAKE_SYSTEM_NAME})")
message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Options)
include(BuildType)
include(Compiler)
include(CppStd)


if( ${BUILD_TYPE_DEBUG} )
    add_definitions(-DDEBUG -DUSE_FULL_ASSERT -DTRACE)
endif()


add_compile_options(-Wall
                    -Wextra
                    -pedantic
                    -Werror
                    -Wshadow
                    )
add_cxx_flag(-Wold-style-cast)

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

if( INTEGRATION_TEST OR BUILD_SYSTEM_DRIVER )
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

