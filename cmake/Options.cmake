macro(print_option _option _text)
    message(STATUS "${_text} : ${${_option}}")
endmacro()


option(UNITTEST "Build Unit Tests" ON)
print_option(UNITTEST "Build Unit Tests")

option(UNITTEST_VERBOSE "Verbose Unit Tests" OFF)
print_option(UNITTEST_VERBOSE "Verbose Unit Tests")

option(COVERAGE "Enable Coverage" OFF)
print_option(COVERAGE "Enable Coverage")

option(LTO "Enable Link Time Optimization (LTO)" OFF)
print_option(LTO "Enable LTO")

option(BUILD_SYSTEM_DRIVER "Build system files" ${CMAKE_CROSSCOMPILING})
print_option(BUILD_SYSTEM_DRIVER "Build system driver")

option(INTEGRATION_TEST "Build Integration Tests" OFF)
print_option(INTEGRATION_TEST "Build Integration Tests")

option(GSL_CONTRACT_UNENFORCED "GSL contract violations unenforced" ${CMAKE_CROSSCOMPILING})
print_option(GSL_CONTRACT_UNENFORCED "Unenforced GSL contracts")

