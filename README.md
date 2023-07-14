# Rotary tuning fork

## Hardware

### DY-SV17F

The connections between the Arduino and the DY-SV17F mp3 player is made according to [this guide](https://electropeak.com/learn/interfacing-dy-sv17f-audio-mp3-player-module-with-arduino/).

## Building
This project is developed using Visual Studio Code with the PlatformIO extension. The main benefits compared to Arduino IDE are that we can easily split the project into multiple files in the standard C++ way, keep multiple configurations for different microcontrollers, and that dependencies are automatically synced. Note that we are still using the Arduino platform, so as long as we `#include <Arduino.h>`, all the same functionality will be available.

### Getting started
1. Check out this branch, and then open this folder in VSCode.
2. You should get a notification to install PlatformIO IDE, choose to install it. If you don't get it, it should be listed under project recommendations in the Extensions tab.
3. Wait while it's installing some libraries and configuring the project. The dependency on the DYPlayer library is already listed in `platformio.ini` and will be installed automatically.
4. You're all set!

### Build, upload and monitor
You now have a couple of new UI elements in the bottom bar:

- House: go to the PlatformIO home page, from where you can manage the project, search for and add libraries, etc.
- Checkmark: build the project.
- Arrow: build and upload to your connected microcontroller.
- Trash can: clean the build cache.
- Power plug: open the serial monitor.
- Wide button that says "Default": select environment. Make sure to set it to the microcontroller you want to build for. There are currently environments for Arduino Micro and for ESP32.

Tip: press Ctrl+Shift+P and choose "Upload and monitor" to build, upload, and open the serial monitor in one go.
