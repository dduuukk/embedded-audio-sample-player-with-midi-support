add_library(uart-intf
    INTERFACE
    )

target_include_directories(uart-intf
    INTERFACE
    .
    )

target_link_libraries(uart-intf
    INTERFACE
    hal-intf
    core-conf
    )