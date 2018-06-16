cmake_minimum_required(VERSION 3.6)
project(E6AMS)

set(CMAKE_C_STANDARD 11)

SET(MCU "atmega2560")
SET(F_CPU "16000000")

SET(CMAKE_C_COMPILER avr-gcc)
SET(CMAKE_CXX_COMPILER avr-g++)

SET(CMAKE_C_FLAGS "-mmcu=${MCU} -DF_CPU=${F_CPU} -Os -Wall -Wextra -Werror -Wno-unused-function -Wno-unknown-pragmas -Wno-unused-parameter")
SET(CMAKE_C_LINK_FLAGS "-mmcu=${MCU}")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(
        CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=output.map"
)

set(SOURCE_FILES main.c drivers/uart.c drivers/uart.h drivers/hm-10.c drivers/hm-10.h drivers/buttonInterface.c drivers/buttonInterface.h Layers/dllLayer.c Layers/dllLayer.h Frames/dllFrame.c Frames/dllFrame.h Frames/appFrame.c Frames/appFrame.h external_libs/md5.c external_libs/md5.h external_libs/global.h Layers/appLayer.c Layers/appLayer.h Library/command.h)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
