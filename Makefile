GCC = /usr/bin/gcc
CFLAGS = -O2 -Wall -Wno-unused-function -Wno-unused-variable -no-pie `pkg-config --cflags --libs gtk+-3.0` -rdynamic -Wno-unused-result

SRCS = $(wildcard src/*.c) $(wildcard src/ui/*.c) $(wildcard src/display/*.c) $(wildcard src/config/*.c)

BIN_NAME = brightness-manager
ICON_FILE = icon.svg
UI_FILE = winMain.glade
DESKTOP_FILE = brightness-manager.desktop
PIPE_FILE = bm-pipe
CONFIG_FILE = config.json

SHARE_PATH = $(HOME)/.local/share/brightness-manager
BIN_PATH = $(HOME)/.local/bin
DESKTOP_PATH = $(HOME)/.local/share/applications

$(BIN_NAME): $(SRCS)
	$(GCC) $^ -o $@ $(CFLAGS)

run: $(BIN_NAME)
	sudo -u $(USER) ./$^

install: $(BIN_NAME) $(ICON_FILE) $(UI_FILE) $(DESKTOP_FILE)
	mkdir -p $(SHARE_PATH)
	mkdir -p $(BIN_PATH)
	mkdir -p $(DESKTOP_PATH)
	cp -f $(ICON_FILE) $(SHARE_PATH)
	cp -f $(UI_FILE) $(SHARE_PATH)
	cp -f $(DESKTOP_FILE) $(DESKTOP_PATH)
	cp -f $(BIN_NAME) $(BIN_PATH)
	sed -i "s,Icon=.*,Icon=$(SHARE_PATH)/$(ICON_FILE),g" $(DESKTOP_PATH)/$(DESKTOP_FILE)
	sed -i "s,Exec=.*,Exec=sudo -u $(USER) $(BIN_PATH)/$(BIN_NAME),g" $(DESKTOP_PATH)/$(DESKTOP_FILE)

uninstall:
	rm -rf $(SHARE_PATH) $(BIN_PATH)/$(BIN_NAME) $(DESKTOP_PATH)/$(DESKTOP_FILE) $(PIPE_FILE) $(CONFIG_FILE) $(BIN_NAME)
