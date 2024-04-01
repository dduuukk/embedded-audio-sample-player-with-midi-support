cmake_minimum_required(VERSION 3.16)
add_subdirectory(cmake-helpers)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

set(TARGET boomba)


set(CMAKE_EXECUTABLE_SUFFIX_C   .elf)
set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf)
set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-helpers/cmake/stm32_gcc.cmake)

project(${TARGET} C CXX ASM)


add_library(hal-intf
    INTERFACE
    )

target_include_directories(hal-intf
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/../build/_deps/stm32-hal-h7-src/Inc/
    ${CMAKE_CURRENT_SOURCE_DIR}/../build/_deps/stm32-cmsis-h7-src/Include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../build/_deps/stm32-cmsis-src/Include/
    )

# TODO add freertos kernel as a submodule, uncomment, and update your port (CM4F is STM32F4, for example)
#add_library(freertos-intf
#    INTERFACE
#    )
#
#target_include_directories(freertos-intf
#    INTERFACE
#    ${CMAKE_CURRENT_SOURCE_DIR}/../FreeRTOS-Kernel/portable/GCC/ARM_CM4F/
#    ${CMAKE_CURRENT_SOURCE_DIR}/../FreeRTOS-Kernel/include/
#    )


stm32_fetch_cmsis(H7)
stm32_fetch_hal(H7)

find_package(HAL COMPONENTS STM32H7 REQUIRED)
find_package(CMSIS COMPONENTS STM32H7 REQUIRED)

# TODO enable FreeRTOS
#find_package(FreeRTOS COMPONENTS ARM_CM4F REQUIRED)

add_executable(${TARGET} blinky.cpp stm32h7xx_hal_conf.h)


target_link_libraries(${TARGET}
        HAL::STM32::H7::M7::RCC
        HAL::STM32::H7::M7::RCCEx
        HAL::STM32::H7::M7::GPIO
        HAL::STM32::H7::M7::CORTEX
        HAL::STM32::H7::M7::PWREx
        CMSIS::STM32::H750IB::M7
        STM32::NoSys
        #FreeRTOS::Timers
        #FreeRTOS::Heap::4
        #FreeRTOS::ARM_CM4F
        #TODO your object libraries go here
    )

# TODO if you want a linker script
# stm32_add_linker_script(${TARGET} PUBLIC deps/libDaisy/core/STM32H750IB_flash.lds)


# Make gcc go brrrr
Set(LINK_OPTIONS -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard --specs=nano.specs --specs=nosys.specs -Wl,-Map=${TARGET}.map,--cref -Wl,--gc-sections -Wl,--print-memory-usage -lc -lm -lnosys)
Set(CFLAGS -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb  -O2 -Wall -Wno-missing-attributes -fasm -fdata-sections -ffunction-sections -Wno-stringop-overflow -g -ggdb -mthumb-interwork)
Set(CPPFLAGS -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -O2 -Wall -Wno-missing-attributes -fasm -fdata-sections -ffunction-sections -Wno-stringop-overflow -g -ggdb -fno-exceptions -fasm -finline -finline-functions-called-once -fshort-enums -fno-move-loop-invariants -fno-unwind-tables -fno-rtti -Wno-register -mthumb-interwork)
Set(ASM_FLAGS -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb  -O2  -Wall -fdata-sections -ffunction-sections -mthumb-interwork)

target_link_options(${TARGET} INTERFACE ${LINK_OPTIONS})
target_compile_options(${TARGET} PUBLIC
    $<$<COMPILE_LANGUAGE:C>: ${CFLAGS}>
    $<$<COMPILE_LANGUAGE:CXX>:${CPPFLAGS}>
    $<$<COMPILE_LANGUAGE:ASM>: ${ASM_FLAGS}>
)

target_compile_definitions(${TARGET} PUBLIC 
    -DCORE_CM7  
    -DSTM32H750xx 
    -DSTM32H750Ix
    -DSTM32H750IB
    -DSTM32H750xB
    -DARM_MATH_CM7 
    -Dflash_layout 
    -DHSE_VALUE=16000000 
    -DUSE_HAL_DRIVER 
    -DUSE_FULL_LL_DRIVER 
    -DDATA_IN_D2_SRAM 
    -DUSE_ARM_DSP
)


add_compile_definitions(STM32H750xx)

add_compile_options(
    ${CPPFLAGS}
    )

# TODO add_subdirectory(folder-name) goes here for all folders!

add_library(core-conf 
    INTERFACE
    )

target_include_directories(core-conf
    INTERFACE
    .
    )

stm32_generate_binary_file(${TARGET})
stm32_print_size_of_target(${TARGET})