# Warning! This code was automatically generated for projects
# without default 'CMakeLists.txt' file.

set(app_sources
	"network.cpp"
	"ota.cpp"
	"semaphores.cpp"

	"main.cpp"
	"lib/UDPFast/UDPFast.cpp"
	"lib/dmx/dmx.cpp"
	"lib/display/display.cpp"
	"lib/display/Adafruit_GFX.cpp"
	"lib/display/Adafruit_SSD1306.cpp"
	"lib/rotary/rotary.cpp"
	"lib/midi/midi.cpp"
	"lib/apcmini/apcmini.cpp"
)

idf_component_register(SRCS ${app_sources} INCLUDE_DIRS ".")


