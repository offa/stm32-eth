
find_package(CppUTest REQUIRED)
add_library(CppUTestLib INTERFACE)
target_include_directories(CppUTestLib SYSTEM INTERFACE ${CppUTest_INCLUDE_DIR})
target_link_libraries(CppUTestLib INTERFACE
                        ${CppUTest_LIBRARY}
                        ${CppUTest_Ext_LIBRARY}
                        )


add_subdirectory("mock")


add_library(testmain TestMain.cpp)
target_link_libraries(testmain PUBLIC CppUTestLib build-libs)


function(add_test_suite name)
    add_cpp_executable(${name} ${ARGN})
    add_test(NAME ${name} COMMAND ${name})
    target_link_libraries(${name} PRIVATE testmain)

    if( COVERAGE )
        target_link_libraries(${name} PRIVATE gcov)
    endif()
endfunction()



add_test_suite(CommonTest RegisterValueTest.cpp
                        SocketInterruptTest.cpp
                        ByteTest.cpp
                        SocketHandleTest.cpp
                        NetConfigTest.cpp
                        )


add_test_suite(SocketTest SocketTest.cpp
                        )
target_link_libraries(SocketTest PRIVATE
                        socket
                        spiwriter-mock
                        w5100device-mock
                        platform-mock
                        )


add_test_suite(W5100DeviceTest W5100DeviceTest.cpp
                            W5100RegisterTest.cpp
                            )
target_link_libraries(W5100DeviceTest PRIVATE
                        w5100device
                        spiwriter-mock
                        )


add_test_suite(SpiWriterTest SpiWriterTest.cpp
                            )
target_link_libraries(SpiWriterTest PRIVATE
                            spiwriter
                            platform-mock
                            stm32hal-mock
                            GSL::GSL
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

