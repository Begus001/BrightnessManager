GCC = /usr/bin/gcc
CFLAGS = -O2 -Wall -Wno-unused-function -Wno-unused-variable -no-pie `pkg-config --cflags --libs gtk+-3.0` -rdynamic -Wno-unused-result

SRCS = $(wildcard src/*.c) $(wildcard src/ui/*.c) $(wildcard src/display/*.c) $(wildcard src/config/*.c)

BIN_NAME = brightness-manager
ICON_FILE = icon.svg
UI_FILE = winMain.glade
DESKTOP_FILE = brightness-manager.desktop
PIPE_FILE = bm-pipe
CONFIG_FILE = config.json
POLKIT_ACTION_FILE = org.freedesktop.policykit.brightness-manager.policy

ETC_PATH = /etc/brightness-manager
BIN_PATH = /usr/local/bin
DESKTOP_PATH = /usr/share/applications
POLKIT_ACTION_PATH = /usr/share/polkit-1/actions

ID = $(shell id -u)

$(BIN_NAME): $(SRCS)
	$(GCC) $^ -o $@ $(CFLAGS)

run: $(BIN_NAME)
	sudo ./$^

install: $(BIN_NAME) $(ICON_FILE) $(UI_FILE) $(DESKTOP_FILE)
    ifneq ($(ID),0)
		@echo >&2 "\033[1;31mERROR: You have to run install as root\033[1;37m"
		@exit 1
    endif
	mkdir -p $(ETC_PATH)
	cp -f $(ICON_FILE) $(ETC_PATH)
	cp -f $(UI_FILE) $(ETC_PATH)
	cp -f $(DESKTOP_FILE) $(DESKTOP_PATH)
	cp -f $(BIN_NAME) $(BIN_PATH)
	cp -f $(POLKIT_ACTION_FILE) $(POLKIT_ACTION_PATH)
	sed -i "s,Icon=.*,Icon=$(ETC_PATH)/$(ICON_FILE),g" $(DESKTOP_PATH)/$(DESKTOP_FILE)
	sed -i "s,Exec=.*,Exec=pkexec $(BIN_PATH)/$(BIN_NAME),g" $(DESKTOP_PATH)/$(DESKTOP_FILE)

uninstall:
    ifeq ($(ID),0)
		rm -rf $(ETC_PATH) $(BIN_PATH)/$(BIN_NAME) $(DESKTOP_PATH)/$(DESKTOP_FILE) $(PIPE_FILE) $(CONFIG_FILE) $(BIN_NAME) $(POLKIT_ACTION_PATH)/$(POLKIT_ACTION_FILE)
    else
		rm -rf $(PIPE_FILE) $(CONFIG_FILE) $(BIN_NAME)
		@echo >&2 "\033[1;33mWARNING: Only files in the current directory were uninstalled"
		@echo >&2 "WARNING: To uninstall the remaining files, run make uninstall as root"
    endif
	
