
function(add_mock name src)
    add_cpp_library(${name} ${src})
    target_link_libraries(${name} PRIVATE CppUTest::CppUTest CppUTest::Mock)
endfunction()

add_mock(spiwriter-mock SpiWriterMock.cpp)
add_mock(w5100device-mock W5100DeviceMock.cpp)
target_link_libraries(w5100device-mock PRIVATE GSL)
add_mock(stm32hal-mock Stm32HalMock.cpp)
add_mock(platform-mock PlatformMock.cpp)

