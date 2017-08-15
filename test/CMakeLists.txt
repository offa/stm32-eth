
find_package(CppUTest REQUIRED)

include_directories(SYSTEM ${CppUTest_INCLUDE_DIR})
add_subdirectory("mock")

add_library(testmain OBJECT TestMain.cpp)


function(add_test_suite name src)
    add_executable(${name} ${${src}}
                                $<TARGET_OBJECTS:testmain>
                                )
    add_test(NAME ${name} COMMAND ${name})
    target_link_libraries(${name} ${CppUTest_LIBRARY}
                                    ${CppUTest_Ext_LIBRARY}
                                    )
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
                    $<TARGET_OBJECTS:w5100device-mock>
                    $<TARGET_OBJECTS:platform-mock>
                    $<TARGET_OBJECTS:spiwriter-mock>
                    )
add_test_suite(SocketTest SOCKETTEST_SRC)


list(APPEND W5100TEST_SRC W5100DeviceTest.cpp
                        W5100RegisterTest.cpp
                        $<TARGET_OBJECTS:w5100device>
                        $<TARGET_OBJECTS:spiwriter-mock>
                        )
add_test_suite(W5100DeviceTest W5100TEST_SRC)


list(APPEND SPIWRITERTEST_SRC SpiWriterTest.cpp
                            $<TARGET_OBJECTS:stm32hal-mock>
                            $<TARGET_OBJECTS:spiwriter>
                            $<TARGET_OBJECTS:platform-mock>
                            )
add_test_suite(SpiWriterTest SPIWRITERTEST_SRC)



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

