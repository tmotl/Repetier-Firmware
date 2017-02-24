# import the Arduino Toolchain
set(CMAKE_TOOLCHAIN_FILE cmake/ArduinoToolchain.cmake)
cmake_minimum_required(VERSION 2.8)
project(Repetier-Firmware C CXX)

print_board_list()
print_programmer_list()

# setup the board
set(ARDUINO_DEFAULT_BOARD mega2560)
set(ARDUINO_DEFAULT_PORT /dev/ttyUSB0)

# generate the firmware from the sketch
generate_arduino_firmware(Repetier SKETCH Repetier.ino)
