
add_cpp_library(stm32-w5100device OBJECT Device.cpp)
target_include_directories(stm32-w5100device PRIVATE $<TARGET_PROPERTY:GSL::GSL,INTERFACE_INCLUDE_DIRECTORIES>)

