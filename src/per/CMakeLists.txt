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

    add_library(uart-driver
    OBJECT
    UART.cpp
    )

target_link_libraries(uart-driver
    PUBLIC
    uart-intf
    PRIVATE
    core-conf
    hal-intf
    circular-buffer-intf
    midi-handler-intf
    )