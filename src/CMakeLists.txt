# Warning! This code was automatically generated for projects
# without default 'CMakeLists.txt' file.

set(app_sources
	"network.cpp"
	"ota.cpp"

	"main.cpp"
	"lib/UDPFast/UDPFast.cpp"
	"lib/dmx/dmx.cpp"
)

idf_component_register(SRCS ${app_sources} INCLUDE_DIRS ".")

