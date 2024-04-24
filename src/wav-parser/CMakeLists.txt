add_library(wav-parser-intf
INTERFACE
)

target_include_directories(wav-parser-intf
INTERFACE
.
)

target_link_libraries(wav-parser-intf
INTERFACE
fatfs-intf
fatfs-intf
wm8731-driver-intf
hal-intf
core-conf
)

add_library(wav-parser
OBJECT
wav-parser.cpp
)

target_link_libraries(wav-parser
PUBLIC
wav-parser-intf
PRIVATE
)