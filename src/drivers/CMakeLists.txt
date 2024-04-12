add_library(sdmmc-driver-intf
INTERFACE
) 

target_include_directories(sdmmc-driver-intf
INTERFACE
.
)

target_link_libraries(sdmmc-driver-intf
INTERFACE
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