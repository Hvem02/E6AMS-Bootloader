cmake_minimum_required(VERSION 3.6)
project(E6AMS_Bootloader)

set(CMAKE_C_STANDARD 11)

SET(MCU "atmega2560")
SET(F_CPU "16000000")
SET(BOOTSTART "0x3E000")

SET(CMAKE_C_COMPILER avr-gcc)
SET(CMAKE_CXX_COMPILER avr-g++)

SET(CMAKE_C_FLAGS "-mmcu=${MCU} -DF_CPU=${F_CPU} -Os -Wall -Wextra -Werror -Wno-unused-function -Wno-unknown-pragmas -Wno-unused-parameter")
SET(CMAKE_C_LINK_FLAGS "-mmcu=${MCU} -Wl,--section-start=.text=${BOOTSTART}")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(SOURCE_FILES
        main.c
        mainProject/Drivers/hm-10.c
        mainProject/Drivers/uart.c
        mainProject/external_libs/md5.c
        mainProject/Frames/appFrame.c
        mainProject/Frames/dllFrame.c
        mainProject/Layers/appLayer.c
        mainProject/Layers/dllLayer.c)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
