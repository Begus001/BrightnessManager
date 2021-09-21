GCC = /usr/bin/gcc
CFLAGS = -O2 -Wall -Wno-unused-function -Wno-unused-variable -no-pie `pkg-config --cflags --libs gtk+-3.0` -rdynamic -Wno-unused-result -ggdb -DGDK_DEBUG=nograbs -pthread -lcjson

SRCS = $(wildcard src/*.c) $(wildcard src/ui/*.c) $(wildcard src/display/*.c) $(wildcard src/config/*.c)

BIN_NAME = brightness-manager

build:
	$(GCC) $(SRCS) -o $(BIN_NAME) $(CFLAGS)

launch:
	./$(BIN_NAME)

all: build launch
