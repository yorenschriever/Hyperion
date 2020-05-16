# What is it
This is a light driver that drives leds strips, dmx, and pwm signals. The signal be routed from the inputs or generated locally. 
The device is based on an Olimex ESP32-POE. It supports DMX, led strips like ws2812, ws2801, apa106, but can also drive  "dumb" lights like non-addressable led strips or incandescent lights using PWM. It can get its input from DMX, ethernet (ArtNet), SD card or generate patterns locally. Patterns can be controlled with an usb midi device, WiFi or BLE. 

[https://www.olimex.com/Products/IoT/ESP32/ESP32-POE/open-source-hardware](https://www.olimex.com/Products/IoT/ESP32/ESP32-POE/open-source-hardware)

# Getting started

## Building the project
**IDE**
This project is written is visual studio code using the platform-io IDE. The easiest way to get started is to download visual studio code, and search for 'platformIO IDE' in the extensions tab. After installation i still had to add the command line tools to my path, even though the installation guide says otherwise.
[https://code.visualstudio.com/](https://code.visualstudio.com/)
[https://platformio.org/platformio-ide](https://platformio.org/platformio-ide)
[https://docs.platformio.org/en/latest/core/installation.html#install-shell-commands](https://docs.platformio.org/en/latest/core/installation.html#install-shell-commands)

**Platform**
This project uses a branch of the ESPIDF. If you already are working with ESPIDF this will cause a conflict. I solved that by uninstalling my existing installation first. 

    pio platform uninstall espressif32

**Source code**
Now you can checkout the project

    git clone git@github.com:yorenschriever/Hyperion.git

**Configuration**
The project contains multiple example configuration files. The project looks for a file called configurations/configuration.h. To get started you can create a symlink to selftest.h. On unix use:

    ln -s configurations/selftest.h configurations/configuration.h

Or on windows use:

    mklink configurations\selftest.h configurations\configuration.h
**Building**
Now you are ready to build.  Click the checkmark icon in the status bar, or type:

    pio run
   This will download all dependencies and then builds the project.

## uploading
You can upload new firmware over the network (ethernet / wifi) or over USB. 

**Network**
You can program the device over ethernet or WiFi. For that to work you have to be on the same network, and have compatible ip settings. (DHCP is disabled by default, check the settings in network.cpp. This will become a configuration (or runtime) setting in the near future)

You can upload by clicking the arrow icon in the status bar, or by typing

    pio run -t network

The device creates a mDNS record for 'Hyperion.local' on startup, and this command will upload to that address. If you have multiple devices on the network you have to modify the upload command to use the specific ip address. 

Upload works best if you do not blast the device with pixel data during the upload. If you uploaded firmware that is not stable enough to run the firmware update during normal operation, you can boot the device in a 'safe mode' by pressing down the rotary button during power up. This will startup the device without starting the pipes and with a minimum of drivers. 

**USB**
You can upload over USB as a fallback if uploading over the network fails. To do this you have to open the device and connect to the micro-usb port on the esp32-poe. You need to have CH340 drivers, they can be downloaded here: [https://www.olimex.com/Products/IoT/ESP32/ESP32-POE/open-source-hardware](https://www.olimex.com/Products/IoT/ESP32/ESP32-POE/open-source-hardware)

The micro-usb port on the esp32-poe and the usb-midi input on the front are sharing the same pins on the esp32. This means that your upload will fail if you have a midi usb device connected. The midi port on the front must be disconnected.

To upload over usb type: 

    pio run -t usb


## Connections
**Power**
The device has 2 power inputs, a 2.1mm barrel connector on the front, and an Anderson PowerPole at the back. They are connected in series, so only one needs to be connected. Both accept input voltage between 7 and 28 volt. If you are not using the PWM outputs, the device less than 500mA, and you can use front connector. If you are connecting lights to the PWM outputs, it is best to use the PowerPole connector.

**Ethernet**
The ethernet port can use used to send or receive led data (in the form of raw UDP packets or ArtNet), diagnostic data, or firmware updates. 

**MIDI**
A midi controller can be connected to device to select patterns, or to change pattern parameters. It can also be used to sync patterns to a midi clock.

**DMX**
Pixel data can be sent or received through the rj-45 dmx connector. Some devices have an rj45 port, so you can hook them up with a network cable, but for classic dmx connectors you need an adapter like ADJ ACR­J453PM or ADJ ACRJ453PFM. An independent DMX input and output are wired to this port, and you can use them at the same time. The DMX output is connected to the orange/brown pairs, and the DMX input to green/blue. This means that if you use a normal network cable, you are connected to the DMX output, and if you use a cross cable you will be connected to the input. 

**PWM**
The 12 PWM channels can handle 5A each, with a maximum of 30A combined. They use N-channel mosfets, so you can use use them with common-anode (CA) led strips. The mosfets are temperature and over-current protected. The indicator leds are wired through the mosfets, so if the protection kicks in, these leds reflect the actual output status.

**LED strips**
A Octo-ws2811 compatible connector is available to drive led strips, But the device is not limited to ws2811 strips only. It can drive all clockless (ws2812 sk6812) and clocked strips (ws2801, apa106)

**WiFi**
WiFi connectivity makes it possible to control pattern parameters remotely, for example from a tablet. It can also be used to transmit pixel data if the connection is reliable enough.

**BLE**
BLE can be used to send pattern parameter data from apps or other smart devices

**SD card**
The ESP32-poe pcb has a SD cardslot. An sd card could be used to store large pre-rendered patterns, or different firmware versions. 


## Architecture
**Hardware**
--image--

**Software**
Words in *Italics* in this chapter refer to classes in the project.

**Signal routing**
Signals are routed from *Inputs* to *Outputs* using a Pipe. *Inputs* provide a stream of bytes to the *Pipe*. The *Pipe* can transform the data and then send it to the *Output*. The *Output* then sends out the data to hardware drivers. 
You create as many pipes as you like. They are configured in the *Configuration* files. Each pipe needs to have 1 *Input* and 1 *Output*. Optionally you can also provide a transfer function and a Lookup table (*LUT*). The transfer function can be used to convert between different data representations. For example from *RGB* to *BGR* or *RGB* to *Monochrome*. Lookup tables can be used to apply gamma correction, colour correction, and correct for other non-linearities in the lamps you are driving. 

**Patterns**
Some inputs will get their data from the hardware ports, but there are also inputs that generate the pixel data on the device. You provide one or multiple *Pattern* objects to these inputs. You can write as many patterns as you like

## Make your own content

To create your won content, you have to create you own configuration file in configuration folder, and create a symlink to it. Now you can start defining your own pipes. You can create as many as you like. All inputs and output classes have a small description of what they do in the code comments.

Your configuration file can also instantiate lookup tables. You can refer to them when creating the pipes. 

Patterns can be added in the Patterns folder. There a couple of helper classes available. Examples for them can be found in rgbPatterns.h



