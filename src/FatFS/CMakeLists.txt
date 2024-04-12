add_library(fatfs-intf
INTERFACE
)

target_include_directories(fatfs-intf
INTERFACE
.
)   

target_link_libraries(fatfs-intf
INTERFACE
)

add_library(fatfs
OBJECT
bsp_sd_diskio.c
diskio.c
ff_gen_drv.c
ff.c
sd_diskio.c
option/ccsbcs.c
)

target_link_libraries(fatfs
PUBLIC
fatfs-intf
PRIVATE
core-conf
hal-intf
)