
add_cpp_library(stm32-w5100device OBJECT Device.cpp)
link_to_obj(stm32-w5100device SYSTEM stm32hal-api)
