
function(add_mock name src)
    add_library(${name} ${src})
    target_link_libraries(${name} PRIVATE CppUTestLib)
endfunction()

add_mock(spiwriter-mock SpiWriterMock.cpp)
add_mock(w5100device-mock W5100DeviceMock.cpp)
add_mock(stm32hal-mock Stm32HalMock.cpp)
add_mock(platform-mock PlatformMock.cpp)
