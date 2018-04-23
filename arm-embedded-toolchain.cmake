
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)


set(FLAGS_COMMON "-mcpu=cortex-m4 -mthumb -mfloat-abi=soft -ffunction-sections -fdata-sections -ffreestanding")
set(FLAGS_CXX "-fno-unwind-tables -fno-exceptions -fno-rtti")
set(FLAGS_C "")
set(FLAGS_LINKER "-nostartfiles -Xlinker --gc-sections --specs=nano.specs --specs=nosys.specs -T mem.ld -T libs.ld -T sections.ld -L${PROJECT_SOURCE_DIR}/ldscripts -Wl,-Map,${PROJECT_NAME}.map")

set(CMAKE_C_FLAGS_INIT "${FLAGS_COMMON} ${FLAGS_C}" CACHE STRING "C Compiler Flags")
set(CMAKE_CXX_FLAGS_INIT "${FLAGS_COMMON} ${FLAGS_CXX}" CACHE STRING "C++ Compiler Flags")
set(CMAKE_EXE_LINKER_FLAGS_INIT "${FLAGS_COMMON} ${FLAGS_LINKER}" CACHE STRING "Linker Flags")

