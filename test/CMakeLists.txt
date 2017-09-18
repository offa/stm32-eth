
find_package(CppUTest REQUIRED)

include_directories(SYSTEM ${CppUTest_INCLUDE_DIR})
add_subdirectory("mock")

add_library(testmain TestMain.cpp)
target_link_libraries(testmain ${CppUTest_LIBRARY} ${CppUTest_Ext_LIBRARY})


function(add_test_suite name src)
    add_executable(${name} ${${src}})
    add_test(NAME ${name} COMMAND ${name})
    target_link_libraries(${name} testmain)
endfunction()



list(APPEND COMMONTEST_SRC RegisterValueTest.cpp
                                    SocketInterruptTest.cpp
                                    ByteTest.cpp
                                    SocketHandleTest.cpp
                                    NetConfigTest.cpp
                                    )
add_test_suite(CommonTest COMMONTEST_SRC)


list(APPEND SOCKETTEST_SRC SocketTest.cpp
                    $<TARGET_OBJECTS:socket>
                    )
add_test_suite(SocketTest SOCKETTEST_SRC)
target_link_libraries(SocketTest spiwriter-mock
                                w5100device-mock
                                platform-mock
                                )


list(APPEND W5100TEST_SRC W5100DeviceTest.cpp
                        W5100RegisterTest.cpp
                        $<TARGET_OBJECTS:w5100device>
                        )
add_test_suite(W5100DeviceTest W5100TEST_SRC)
target_link_libraries(W5100DeviceTest spiwriter-mock)


list(APPEND SPIWRITERTEST_SRC SpiWriterTest.cpp
                            $<TARGET_OBJECTS:spiwriter>
                            )
add_test_suite(SpiWriterTest SPIWRITERTEST_SRC)
target_link_libraries(SpiWriterTest platform-mock stm32hal-mock)



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

