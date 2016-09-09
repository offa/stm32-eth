include(CMakeForceCompiler)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

CMAKE_FORCE_C_COMPILER(arm-none-eabi-gcc GNU)
CMAKE_FORCE_CXX_COMPILER(arm-none-eabi-g++ GNU)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)


set(FLAGS_COMMON "-mcpu=cortex-m4 -mthumb -mfloat-abi=soft -ffunction-sections -fdata-sections -ffreestanding" CACHE INTERNAL "C / C++ common Flags")
set(FLAGS_CXX "-fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics" CACHE INTERNAL "C++ only Flags")
set(FLAGS_LINKER "${FLAGS_COMMON} -nostartfiles -Xlinker --gc-sections --specs=nano.specs -T mem.ld -T libs.ld -T sections.ld -L${PROJECT_SOURCE_DIR}/ldscripts" CACHE INTERNAL "Linker flags")

set(CMAKE_C_FLAGS "${FLAGS_COMMON} ${FLAGS_C}" CACHE STRING "C Compiler Flags")
set(CMAKE_CXX_FLAGS "${FLAGS_COMMON} ${FLAGS_CXX}" CACHE STRING "C++ Compiler Flags")
set(CMAKE_EXE_LINKER_FLAGS "${FLAGS_LINKER}" CACHE STRING "Linker flags")
