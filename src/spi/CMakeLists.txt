
add_cpp_library(stm32-spiwriter OBJECT SpiWriter.cpp)
link_to_obj(stm32-spiwriter stm32hal-api)

