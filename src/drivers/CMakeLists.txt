add_library(sai-driver-intf
INTERFACE
) 

target_include_directories(sai-driver-intf
INTERFACE
.
)

add_library(sdmmc-driver-intf
INTERFACE
) 

target_include_directories(sdmmc-driver-intf
INTERFACE
.
)

target_link_libraries(sai-driver-intf
INTERFACE
hal-intf
)
target_link_libraries(sdmmc-driver-intf
INTERFACE
hal-intf
)

add_library(sai-driver
OBJECT 
sai-driver.cpp
)

target_link_libraries(sai-driver
PUBLIC
sai-driver-intf
core-conf
hal-intf
)
add_library(sdmmc-driver
OBJECT 
sdmmc-driver.cpp
)

target_link_libraries(sdmmc-driver
PUBLIC
sdmmc-driver-intf
PRIVATE
hal-intf
core-conf
)

add_library(fatfs-interface-driver-intf
INTERFACE
)

target_include_directories(fatfs-interface-driver-intf
INTERFACE
.
)

target_link_libraries(fatfs-interface-driver-intf
INTERFACE
sdmmc-driver-intf
fatfs-intf
)

add_library(fatfs-interface-driver
OBJECT
fatfs.cpp
)

target_link_libraries(fatfs-interface-driver
PUBLIC
fatfs-interface-driver-intf
PRIVATE
sdmmc-driver-intf
fatfs-intf
core-conf
hal-intf
)

add_library(wm8731-driver-intf
    INTERFACE
    )

target_include_directories(wm8731-driver-intf
    INTERFACE
    .
    )

target_link_libraries(wm8731-driver-intf
    INTERFACE
    hal-intf
    )

add_library(wm8731-driver
    OBJECT
    codec_wm8731.cpp
    )

target_link_libraries(wm8731-driver
    PUBLIC
    wm8731-driver-intf
    PRIVATE
    core-conf
    hal-intf
    )

add_library(midi-handler-intf
    INTERFACE
    )

target_include_directories(midi-handler-intf
    INTERFACE
    .
    )

target_link_libraries(midi-handler-intf
    INTERFACE
    hal-intf
    core-conf
    circular-buffer-intf
    )

add_library(midi-parser-intf
    INTERFACE
    )

target_include_directories(midi-parser-intf
    INTERFACE
    .
    )

target_link_libraries(midi-parser-intf
    INTERFACE
    hal-intf
    circular-buffer-intf
    )

add_library(midi-parser
    OBJECT
    MIDI_parser.cpp
    )

target_link_libraries(midi-parser
    PUBLIC
    midi-parser-intf
    PRIVATE
    core-conf
    hal-intf
    )
