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