
add_cpp_library(stm32-w5100device Device.cpp)
target_link_libraries(stm32-w5100device PUBLIC GSL::GSL)

