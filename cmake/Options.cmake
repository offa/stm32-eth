
option(UNITTEST "Build Unit Tests" ON)
message(STATUS "Build Unit Tests : ${UNITTEST}")

option(UNITTEST_VERBOSE "Verbose Unit Tests" OFF)
message(STATUS "Verbose Unit Tests : ${UNITTEST_VERBOSE}")

option(COVERAGE "Enable Coverage" OFF)
message(STATUS "Enable Coverage : ${COVERAGE}")

option(LTO "Enable Link Time Optimization (LTO)" OFF)
message(STATUS "Enable LTO : ${LTO}")

option(BUILD_SYSTEM_DRIVER "Build system files" ${CMAKE_CROSSCOMPILING})
message(STATUS "Build system driver : ${BUILD_SYSTEM_DRIVER}")

option(INTEGRATION_TEST "Build Integration Tests" OFF)
message(STATUS "Build Integration Tests : ${INTEGRATION_TEST}")

option(GSL_CONTRACT_UNENFORCED "GSL contract violations unenforced" ${CMAKE_CROSSCOMPILING})
message(STATUS "Unenforced GSL contracts : ${GSL_CONTRACT_UNENFORCED}")

