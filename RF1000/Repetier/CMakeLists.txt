# import the Arduino Toolchain
set(CMAKE_TOOLCHAIN_FILE cmake/ArduinoToolchain.cmake)
cmake_minimum_required(VERSION 2.8)
project(Repetier-Firmware C CXX)

print_board_list()
print_programmer_list()

# setup the board
#set(ARDUINO_DEFAULT_BOARD mega2560) # Default Board ID, when not specified
set(ARDUINO_DEFAULT_BOARD uno) # Default Board ID, when not specified
set(ARDUINO_DEFAULT_PORT /dev/ttyUSB0) # Default Port, when not specified

# generate the firmware from the sketch
generate_arduino_firmware(Repetier SKETCH Repetier.ino)
