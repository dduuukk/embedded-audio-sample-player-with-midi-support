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