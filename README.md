# BrightnessManager

Have you ever wondered why you can't fall asleep at night? For only 9.99 you can buy this software and sleep like a baby.

No, but seriously, BrightnessManager is an application for Linux, which changes the backlighting or luminescence of your monitor based on a time schedule. Ever since I first found out that you can control some display's brightness through software via I2C, I've always wanted something like this. It is the perfect complement to the night light or blue light filter feature offered by some OSes/desktop environments.

<img src="http://begus.ddns.net/img/bm-prev.png" width="33%"><img src="http://begus.ddns.net/img/bm-prev-manual.png" width="33%"><img src="http://begus.ddns.net/img/bm-prev-settings.png" width="33%">


## Compatibility

I've only tested the app on Ubuntu 21.04, but in theory it should work for all Linux systems running GNOME and maybe even other desktop environments. The installation uses desktop files, if they are supported everything should work. I've also tried it on a Lenovo T450 running Ubuntu 20.04, but I couldn't get the brightness to change there despite ddccontrol recognizing the display.

## Installation
### Prerequisites

You'll need GTK+-3.0, which is (obviously) preinstalled on GNOME. To change the brightness of the displays, the application uses ddccontrol. On Ubuntu, simply run:
```
sudo apt install ddccontrol
```
Unfortunately, you'll also need ***root privileges*** to be able to send commands to the displays.

### Installation from source

If you clone the repository, you should just be able to simply run
```
make install
```
and it should be available from the command line as `brightness-manager` and also as an app, which can be found by the GNOME search. Currently, BrightnessManager is only accessible by the user that installed it.

`make install` simply builds the project and copies the generated binary to `$HOME/.local/bin` and the icon and ui files to `$HOME/.local/share/brightness-manager`. Lastly, the desktop file is moved to `$HOME/.local/share/applications` and the correct locations of the icon file and executable are inserted into it.

If you want to use BrightnessManager portably (creates all necessary files in the same directory as the executable), you can just run
```
make
```
and run the generated executable with `./brightness-manager`.

### Installation of pre-built binary

There will soon be binaries with install and uninstall scripts.

### Uninstallation

To uninstall BrightnessManager, you need to run:
```
make uninstall
```
#### WARNING
**This will remove your whole configuration, both in the source directory, as well as in the installed directory.**

Currently, there is no uninstall script, so using the Makefile is the only way to uninstall the application if it was installed using `make install`.

## Usage
### First start

At first start, an autodetect routine is run, which tries to automatically detect the connected monitor's I2C bus IDs and set the `Max Monitors` setting. This can take a few seconds. It is not always successful, in which case read about how to configure the I2C bus IDs manually [here](#manual-i2c-configuration).

### Configuring the displays

At the bottom there is a spin button named `Display` which allows you to choose which monitor you want to configure. For each display you can set the sunset and sunrise times individually, as well as the day and night brightness. If you press the save button (green tick), the current configuration for all your displays will be saved to the configuration file. If you want to discard you changes, simply press cancel (red cross).

By pressing the button `Apply to all`, the configuration of the currently selected display is copied to all displays, but it is not yet saved. So if you want to revert, you can still press cancel or, if you want to save your changes to file, press save.

### What the application does

If the enable switch at the top left is activated, BrightnessManager uses the set time schedule to change the brightness of the displays gradually. For example, if the sunset time is set to 20:00, the daytime brightness to 100 and the nighttime brightness to 0, the application will set the brightness to 100 during the day. 60 minutes (by default, can be configured) before 20:00, so at 19:00, it will start to decrease the brightness of your displays gradually from 100 until it reaches 0 at 20:00. At sunrise, the exact same thing happens in reverse.

### Manually setting the brightness

If you want to manually change the brightness of a display, you can click the display icon, which opens a popover where you can select the display and brightness you want to apply. This will automatically disable the enable switch, as the set time schedule was overridden.

### Run in background

When clicking the default close button on the application window, it hides the window, but continues running in the background. If you want to reshow the window again, just run BrightnessManager again. This will send a message to the already running instance to show itself.

There is, however, a GTK bug, which crashes the application when the window is being reshown. To get around this, the second instance checks if the already running one has crashed and, if it has, just continues running as the main instance.

If you want to exit the application completely, go into the settings popover (gear icon) and click `Exit Application`.

### Settings

In the settings (gear icon), you can customize how often the application should update the brightness of the display (Max Displays) or how long before the set times it should start to gradually taper the brightness (Fade Duration).

You can also set the amount of displays you want to be regulated, which is usually autodetected.

#### Manual I2C configuration

Under the I2C tab, you can rerun the autodetection routine. You can also manually change the I2C bus IDs of each monitor if the autodetect routine failed for some reason. You can try to manually run:
```
sudo ddccontrol -p 2>/dev/null | grep " - Device"
```
If you find at least one entry with `/dev/i2c-X`, where X represents the I2C ID, you can simply enter it in the BrightnessManager and change the max displays to the number of displays detected. If the command doesn't return a device, you probably can't use BrightnessManager on your system. :(
