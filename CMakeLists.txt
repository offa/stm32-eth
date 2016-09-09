cmake_minimum_required(VERSION 3.0)

project(stm32-eth VERSION 0.0.3)
message(STATUS "~~~ ${PROJECT_NAME} v${PROJECT_VERSION} ~~~")


option(UNITTEST "Build Unit Tests" ON)
message(STATUS "Build Unit Tests : ${UNITTEST}")

option(UNITTEST_VERBOSE "Verbose Unit Tests" OFF)
message(STATUS "Verbose Unit Tests : ${UNITTEST_VERBOSE}")

option(COVERAGE "Enable Coverage" OFF)
message(STATUS "Enable Coverage : ${COVERAGE}")


set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Compiler)
include(Cpp14)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    add_common_flag(-DDEBUG
                    -DUSE_FULL_ASSERT
                    -DTRACE
                    -DOS_USE_TRACE_SEMIHOSTING_DEBUG
                    )
endif()

add_common_flag(-Wall -Wextra -pedantic -Werror -Wshadow)

if( COVERAGE )
    include(Coverage)
endif()


add_definitions(-DSTM32F407xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000)
include_directories(SYSTEM "system/include"
                            "system/include/stm32f4-hal"
                            "system/include/cmsis"
                            )
add_subdirectory("system")


include_directories("include")
add_subdirectory("src")


if( UNITTEST )
    enable_testing()
    add_subdirectory("test")
endif()


