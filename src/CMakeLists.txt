# Warning! This code was automatically generated for projects
# without default 'CMakeLists.txt' file.

set(app_sources
	"debug.cpp"
	"main.cpp"

#put this before midi and dmx code
	"hardware/uart_iram/uart_iram.c"

	"hardware/ethernet/ethernet.cpp"
	"hardware/wifi/wifi.cpp"
	"hardware/firmwareUpdate/firmwareUpdate.cpp"
	#"hardware/websocket/websocket.cpp"
	"hardware/UDPFast/UDPFast.cpp"
	"hardware/display/display.cpp"
	"hardware/display/Adafruit_GFX.cpp"
	"hardware/display/Adafruit_SSD1306.cpp"
	"hardware/rotary/rotary.cpp"
	"hardware/midi/midi.cpp"
	"hardware/dmx/dmx.cpp"
	"hardware/apcmini/apcmini.cpp"
	"hardware/pca9685/pca9685.cpp"
	"hardware/sharedResources.cpp"
	"patterns/helpers/tempo/tempo.cpp"

)

idf_component_register(SRCS ${app_sources} INCLUDE_DIRS ".")


