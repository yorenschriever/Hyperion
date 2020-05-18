# Warning! This code was automatically generated for projects
# without default 'CMakeLists.txt' file.

set(app_sources
	"network.cpp"
	"ota.cpp"
	"semaphores.cpp"

	"main.cpp"
	"hardware/UDPFast/UDPFast.cpp"
	"hardware/dmx/dmx.cpp"
	"hardware/display/display.cpp"
	"hardware/display/Adafruit_GFX.cpp"
	"hardware/display/Adafruit_SSD1306.cpp"
	"hardware/rotary/rotary.cpp"
	"hardware/midi/midi.cpp"
	"hardware/apcmini/apcmini.cpp"
)

idf_component_register(SRCS ${app_sources} INCLUDE_DIRS ".")


