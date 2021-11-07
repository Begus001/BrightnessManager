GCC = /usr/bin/gcc
CFLAGS = -O2 -Wall -Wno-unused-function -Wno-unused-variable -no-pie `pkg-config --cflags --libs gtk+-3.0` -rdynamic -Wno-unused-result -pthread

SRCS = $(wildcard src/*.c) $(wildcard src/ui/*.c) $(wildcard src/display/*.c) $(wildcard src/config/*.c)

BIN_NAME = brightness-manager
ICON_FILE = icon.svg
UI_FILE = winMain.glade
DESKTOP_FILE = brightness-manager.desktop

SHARE_PATH = $(HOME)/.local/share/brightness-manager
BIN_PATH = $(HOME)/.local/bin
DESKTOP_PATH = $(HOME)/.local/share/applications

build:
	$(GCC) $(SRCS) -o $(BIN_NAME) $(CFLAGS)

launch:
	./$(BIN_NAME)

install: build
	mkdir -p $(SHARE_PATH)
	mkdir -p $(BIN_PATH)
	mkdir -p $(DESKTOP_PATH)
	cp $(ICON_FILE) $(SHARE_PATH)
	cp $(UI_FILE) $(SHARE_PATH)
	cp $(BIN_NAME) $(BIN_PATH)
	cp $(DESKTOP_FILE) $(DESKTOP_PATH)
	sed -i "s,Icon=.*,Icon=$(SHARE_PATH)/$(ICON_FILE),g" $(DESKTOP_PATH)/$(DESKTOP_FILE)

uninstall:
	rm -rf $(SHARE_PATH) $(BIN_PATH)/$(BIN_NAME) $(DESKTOP_PATH)/$(DESKTOP_FILE)

all: build launch
