
function(add_mock name src)
    add_cpp_library(${name} ${src})
    target_link_libraries(${name} PRIVATE CppUTest CppUTestExt)
endfunction()

add_mock(spiwriter-mock SpiWriterMock.cpp)
target_link_libraries(spiwriter-mock PUBLIC stm32hal-api)

add_mock(w5100device-mock W5100DeviceMock.cpp)
target_link_libraries(w5100device-mock PUBLIC Microsoft.GSL::Microsoft.GSL)

add_mock(stm32hal-mock Stm32HalMock.cpp)
target_link_libraries(stm32hal-mock PUBLIC stm32hal-api)

add_mock(platform-mock PlatformMock.cpp)
target_link_libraries(platform-mock PUBLIC stm32hal-api)
