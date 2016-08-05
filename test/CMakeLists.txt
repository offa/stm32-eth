

find_package(Threads REQUIRED)
find_package(CppUTest REQUIRED)

include_directories(SYSTEM ${CppUTest_INCLUDE_DIR})

add_subdirectory("mock")

add_library(testmain OBJECT TestMain.cpp)


macro(_add_test _NAME _SRC_LIST)
    add_executable(${_NAME} ${${_SRC_LIST}}
                                $<TARGET_OBJECTS:testmain>
                                )
    add_test(NAME ${_NAME} COMMAND ${_NAME})
    target_link_libraries(${_NAME} ${CppUTest_LIBRARY}
                                    ${CppUTest_Ext_LIBRARY}
                                    ${CMAKE_THREAD_LIBS_INIT}
                                    )
    target_include_directories(${_NAME} SYSTEM PUBLIC ${CppUTest_INCLUDE_DIR})
endmacro()



list(APPEND SOCKETTEST_SRC SocketTest.cpp
                    $<TARGET_OBJECTS:socket>
                    $<TARGET_OBJECTS:w5100device-mock>
                    )
_add_test(SocketTest SOCKETTEST_SRC)


list(APPEND W5100TEST_SRC W5100DeviceTest.cpp
                        $<TARGET_OBJECTS:w5100device>
                        $<TARGET_OBJECTS:spi-mock>
                        )
_add_test(W5100DeviceTest W5100TEST_SRC)


list(APPEND BYTETEST_SRC ByteTest.cpp)
_add_test(ByteTest BYTETEST_SRC)


set(TEST_FLAGS -c)

if( UNITTEST_VERBOSE )
    set(TEST_FLAGS ${TEST_FLAGS} -v)
endif()


add_custom_target(unittest SocketTest ${TEST_FLAGS}
                    COMMAND W5100DeviceTest ${TEST_FLAGS}
                    COMMAND ByteTest ${TEST_FLAGS}

                    COMMENT "Running unittests\n\n"
                    VERBATIM
                    )


