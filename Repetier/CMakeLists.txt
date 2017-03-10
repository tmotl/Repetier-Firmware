cmake_minimum_required(VERSION 2.8)

# this line is needed only if you use boards with selectable cpu (like mega, pro etc)
# and this line should go before call to "cmake/ArduinoToolchain.cmake"
# this is durty hack and should be fixed somewhen, because it should go to 
# particular cmake subdirectory
set(ARDUINO_CPU atmega2560)

set(CMAKE_TOOLCHAIN_FILE cmake/ArduinoToolchain.cmake) # Arduino Toolchain

# Set a variable for commands below
set(PROJECT_NAME Repetier)

# Define your project and language
project(${PROJECT_NAME} C CXX)

print_board_list()
print_programmer_list()

# add libraries to project
#link_directories(${ARDUINO_SDK}/libraries)

set(${PROJECT_NAME}_BOARD mega)

# Define the source code
setup_arduino_sketch(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/Repetier.ino SKETCHSRC)
aux_source_directory(${CMAKE_SOURCE_DIR} repetier_sources)
include_directories(${CMAKE_SOURCE_DIR})
set(${PROJECT_NAME}_SRCS ${SKETCHSRC} ${repetier_sources})

# Command to generate code arduino firmware (.hex file)
generate_arduino_firmware(${PROJECT_NAME})

