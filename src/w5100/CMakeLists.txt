
add_cpp_library(w5100device Device.cpp)
target_link_libraries(w5100device PUBLIC GSL::GSL)

