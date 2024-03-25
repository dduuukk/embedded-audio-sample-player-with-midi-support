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

add_library(scmmc-driver
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