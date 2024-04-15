add_library(circular-buffer-intf
    INTERFACE
    )

target_include_directories(circular-buffer-intf
    INTERFACE
    .
    )

target_link_libraries(circular-buffer-intf
    INTERFACE
    hal-intf
    core-conf
    )