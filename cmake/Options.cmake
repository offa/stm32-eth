function(print_option opt text)
    set(options ENABLED_ONLY)
    cmake_parse_arguments(PRINT_OPTION "${options}" "" "" ${ARGN})

    if( NOT (PRINT_OPTION_ENABLED_ONLY AND NOT ${opt}) )
        message(STATUS "${text} : ${${opt}}")
    endif()
endfunction()


option(UNITTEST "Build Unit Tests" ON)
option(UNITTEST_VERBOSE "Verbose Unit Tests" OFF)
option(UNITTEST_JUNIT "Create JUnit XML Files of Unit Tests" OFF)
print_option(UNITTEST "Build Unit Tests")
print_option(UNITTEST_VERBOSE "Verbose Unit Tests")
print_option(UNITTEST_JUNIT "JUnit XML output enabled" ENABLED_ONLY)

option(COVERAGE "Enable Coverage" OFF)
print_option(COVERAGE "Enable Coverage")

option(LTO "Enable Link Time Optimization (LTO)" OFF)
print_option(LTO "Enable LTO")

option(BUILD_SYSTEM_DRIVER "Build system files" ${CMAKE_CROSSCOMPILING})
print_option(BUILD_SYSTEM_DRIVER "Build system driver")

option(INTEGRATIONTEST "Build Integration Tests" OFF)
print_option(INTEGRATIONTEST "Build Integration Tests")

option(SANITIZER_ASAN "Enable ASan" OFF)
print_option(SANITIZER_ASAN "ASan")

option(SANITIZER_UBSAN "Enable UBSan" OFF)
print_option(SANITIZER_UBSAN "UBSan")
