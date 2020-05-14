todo format this document

make sure you dont have another branch (or master) of espidf installed (i am using a branch here, but platformio doesnt get that)
to uninstall, type

pio platform uninstall espressif32

the ch340 chip to upload new firmware adn the usb hos to connect to midi both use the same serial port. This means that it works best if you disconnect the device from the usb host port before uploading

to get started create a link to settings.h from one of the settings that you want to use, eg:
ln -s settings/selftest.h settings/settings.h