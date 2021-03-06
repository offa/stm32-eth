
find_package(CppUTest REQUIRED)

if( NOT TARGET CppUTest::CppUTest )
    add_library(CppUTest::CppUTest INTERFACE IMPORTED)
    target_link_libraries(CppUTest::CppUTest INTERFACE CppUTest)
endif()

if( NOT TARGET CppUTest::CppUTestExt )
    add_library(CppUTest::CppUTestExt INTERFACE IMPORTED)
    target_link_libraries(CppUTest::CppUTestExt INTERFACE CppUTestExt)
endif()


if( SANITIZER_ENABLED )
    set_target_properties(CppUTest::CppUTest PROPERTIES
                            INTERFACE_COMPILE_DEFINITIONS
                                CPPUTEST_MEM_LEAK_DETECTION_DISABLED
                            )
endif()

add_subdirectory("mock")


add_library(testmain TestMain.cpp)
target_link_libraries(testmain PUBLIC
    CppUTest::CppUTest
    CppUTest::CppUTestExt
    build-libs
    )


function(add_test_suite)
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCE DEPENDS)
    cmake_parse_arguments(TEST_OPTION "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if( NOT TEST_OPTION_NAME )
        message(FATAL_ERROR "No name passed")
    endif()

    if( NOT TEST_OPTION_SOURCE )
        message(FATAL_ERROR "No sources passed")
    endif()

    add_cpp_executable(${TEST_OPTION_NAME} ${TEST_OPTION_SOURCE})
    add_test(NAME ${TEST_OPTION_NAME} COMMAND ${TEST_OPTION_NAME})
    target_link_libraries(${TEST_OPTION_NAME} PRIVATE testmain)

    if( TEST_OPTION_DEPENDS )
        target_link_libraries(${TEST_OPTION_NAME} PRIVATE ${TEST_OPTION_DEPENDS})
    endif()

    if( COVERAGE )
        target_link_libraries(${TEST_OPTION_NAME} PRIVATE gcov)
    endif()
endfunction()



add_test_suite(NAME CommonTest
                SOURCE
                    RegisterValueTest.cpp
                    SocketInterruptTest.cpp
                    ByteTest.cpp
                    SocketHandleTest.cpp
                    NetConfigTest.cpp
                )


add_test_suite(NAME SocketTest
                SOURCE
                    SocketTest.cpp
                    $<TARGET_OBJECTS:stm32-socket>
                DEPENDS
                    spiwriter-mock
                    w5100device-mock
                    platform-mock
                )


add_test_suite(NAME W5100DeviceTest
                SOURCE
                    W5100DeviceTest.cpp
                    W5100RegisterTest.cpp
                    $<TARGET_OBJECTS:stm32-w5100device>
                DEPENDS
                    spiwriter-mock
                )


add_test_suite(NAME SpiWriterTest
                SOURCE
                    SpiWriterTest.cpp
                    $<TARGET_OBJECTS:stm32-spiwriter>
                DEPENDS
                    platform-mock
                    stm32hal-mock
                )



set(TEST_FLAGS -c)

if( UNITTEST_VERBOSE )
    set(TEST_FLAGS ${TEST_FLAGS} -v)
endif()

if( UNITTEST_JUNIT )
    set(TEST_FLAGS ${TEST_FLAGS} -ojunit -k ${PROJECT_NAME})
endif()


add_custom_target(unittest CommonTest ${TEST_FLAGS}
                    COMMAND SocketTest ${TEST_FLAGS}
                    COMMAND W5100DeviceTest ${TEST_FLAGS}
                    COMMAND SpiWriterTest ${TEST_FLAGS}

                    COMMENT "Running unittests\n\n"
                    VERBATIM
                    )
