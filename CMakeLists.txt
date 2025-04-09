cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME TapCAN2)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# download CPM if needed
include(cmake/CPM.cmake) 

include(${CPM_DOWNLOAD_LOCATION})


# LVGL Library
set(LV_CONF_INCLUDE_SIMPLE OFF)

include(Libs.cmake)

# LVGL Library Compile Definitions
target_compile_definitions(lvgl PRIVATE    
    LV_CONF_PATH=${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc/lv_conf.h
    LV_KCONFIG_IGNORE=1
    STM32H725xx
)

target_include_directories(lvgl PRIVATE
    Drivers/CMSIS/Include 
    Drivers/CMSIS/Device/ST/STM32H7xx/Include    
)


# Squareline Studio Generated UI Code Library

add_subdirectory(Core/Src/display/squareline_generated)

# UI Library Compile Definitions
target_compile_definitions(ui PRIVATE    
    LV_CONF_PATH=${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc/lv_conf.h
    LV_KCONFIG_IGNORE=1
)

target_include_directories(ui PUBLIC
    ${CPM_PACKAGE_lvgl_SOURCE_DIR}        
    #Core/Inc
)


# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    Core/Src/lcd_init.c
    #Core/Src/IS25LX128.c
    #Core/Src/S27KL0642.c    
    #Core/Src/TapCAN2_480x854.c
    Core/Src/display/display.c 
    Core/Src/ram_test.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    # Add user defined include paths
    ${CPM_PACKAGE_lvgl_SOURCE_DIR}    
    Core/Src/display/squareline_generated
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
    # Add user defined symbols      
)


# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx    
    # Add user defined libraries
    ui
    lvgl        
)

#add_custom_command(
#    TARGET ${CMAKE_PROJECT_NAME}
#    POST_BUILD
#    COMMAND arm-none-eabi-objdump -S $<TARGET_OBJECTS:${CMAKE_PROJECT_NAME}> -l > ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.lst
#)