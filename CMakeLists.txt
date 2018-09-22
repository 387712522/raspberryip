cmake_minimum_required(VERSION 3.12)
project(raspberryi2c C)
include_directories(/home/BaoPuCao/raspberrypi/tools/arm-bcm2708/arm-bcm2708hardfp-linux-gnueabi/arm-bcm2708hardfp-linux-gnueabi/sysroot/usr/include)
include_directories(include/)
set(CMAKE_C_STANDARD 99)

add_executable(raspberryi2c src/main.cpp include/raspberrypii2c.h src/raspberrypii2c.c include/def.h)