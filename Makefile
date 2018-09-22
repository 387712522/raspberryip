Target = popotwo
TargetLibs := ./src/raspberrypii2c.a
CC = gcc
CPP = g++
AR = ar
CFLAGS := -I./include -lpthread
CPPFLAGS :=-I./include -lpthread
#LDFLAGS	+= -L./Lib
#LDFLAGS := -L/usr/local/lib
#LDLIBS    := -lwiringPi -lwiringPiDev -lpthread -lm


SRC_CPP := $(wildcard ./src/*.cpp)
SRC_C :=  $(wildcard ./src/*.c)

OBJ_CPP := $(patsubst %.cpp,%.o,$(wildcard ./src/*.cpp))
OBJ_C := $(patsubst %.c,%.o,$(wildcard ./src/*.c))

libs:$(OBJ_C)
	$(AR) -r -o $(TargetLibs) $(OBJ_C)


all: $(OBJ_CPP) libs
	$(CPP) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $(Target) $(OBJ_CPP) $(TargetLibs)

.PHONY:clean

clean:
	rm -rf ./src/*.o
	rm -rf ./src/*.a
	rm -rf popotwo