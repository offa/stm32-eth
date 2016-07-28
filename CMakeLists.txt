cmake_minimum_required(VERSION 3.0)

project(stm32-eth VERSION 0.0.1)
message(STATUS "~~~ ${PROJECT_NAME} v${PROJECT_VERSION} ~~~")


option(UNITTEST "Build Unit Tests" ON)
message(STATUS "Build Unit Tests : ${UNITTEST}")

option(UNITTEST_VERBOSE "Verbose Unit Tests" OFF)
message(STATUS "Verbose Unit Tests : ${UNITTEST_VERBOSE}")

option(COVERAGE "Enable Coverage" OFF)
message(STATUS "Enable Coverage : ${COVERAGE}")


set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Cpp14)

add_definitions(-Wall -Wextra -pedantic -Werror -Wshadow)


if( COVERAGE )
    include(Coverage)
endif()


include_directories("include")
add_subdirectory("src")


if( UNITTEST )
    enable_testing()
    add_subdirectory("test")
endif()


