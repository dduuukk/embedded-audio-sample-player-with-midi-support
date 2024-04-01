add_library(sai-driver-intf
INTERFACE
) 

target_include_directories(sai-driver-intf
INTERFACE
.
)

target_link_libraries(sai-driver-intf
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
PRIVATE
hal-intf
core-conf
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
