## SD Driver Design
### Design
The filesystem and SD stack provide non-volatile data storage for our system, enabling us to access this data at runtime with minimal system load and latency through a standard, user-friendly library interface. A significant amount of complexity is abstracted away, and much of the functionality is implemented by the ST HAL (which provides physical layer communication interfaces through the H7's SDMMC peripheral) and the standard FatFS library (which provides high-level interfaces such as f_read, f_write, and f_lseek). Our work in this area is largely concentrated in configuring the SDMMC peripheral to properly communicate with the SD card, and in the block driver that translates between the filesystem's request for a block of data and the ST HAL's SDMMC peripheral commands. 

We chose to use the internal DMA of the SDMMC for this project for a few reasons, the largest being performance. In order to drive the SD card at 100MHz (the highest speed our SD card can achieve), we needed to use the internal SD transciever block and the IDMA. This is a hardware requirement for achieving this frequency. We also used it to take a significant load off of the CPU by allowing read operations to be handled asynchronously. This will, in the future/once we integrate FreeRTOS, allow us to sleep the filesystem thread while a transfer is occuring and perform other useful work on the core. Currently this is impossible due to the blocking implementation of the FatFS library, though. This required us to adapt the generic bsp_sd_diskio and sd_diskio from templates provided by ST to use the DMA-based HAL functions instead of the blocking or interrupt based ones.


### Implementation

Starting from the top level and descending down:

FatFsIntf is the wrapper for the initialization of the entire SD stack. It contains the FATFS instance (the FatFS library's main object), as well as the SDMMC driver instanc, which it constructs. Once both of these objects are initialized, it links the SD block driver to the SD volume through FatFS's `FATFS_LinkDriver` function, and mounts the volume using `f_mount`. 

FatFS itself is a library supporting the Fat32 and ExFat filesystems on embedded platforms. It is a blocking filesystem implementation, but supports threaded applications and thread-safe operation via config options which we do not currently have enabled. Credit to ElmChan for this project. A mirror is available [here](https://github.com/abbrev/fatfs)

The next layer is the block driver layer. These are modified versions of the generic templates provided by ST Microelectronics for their boards, with modifications made based on available examples from other sources such as Electrosmith's implementation. These translate between FatFS's block read requests and physical commands going out to the SD card via SDMMC through ST HAL functions. The modifications made were to support the use of the SDMMC's IDMA and other optimizations, such as using the SD transciever block. 

The SDMMC configuration happens in the SDMMC class, which configures the hardware peripheral to performantly send commands to and retrieve data from the SD card. We talk to the SD card over a SPI interface with a command Tx line and a 4-bit-wide Rx bus, clocked at 100MHz! It's as fast as we could push the hardware, and we needed to use the SD transciever block as a result. This is all set up in the SDMMMC config driver.

### Usage Definition
In order to initialize the filesystem, a FatFsIntf object must be created. This will initialize the SDMMC peripheral, create the FatFS fileystem interface, link the block driver to the SD card's volume, and mount the filesystem.

```c++
FatFsIntf fs = FatFsIntf();
```

From there, the user can declare a file pointer and call f_open to open files, f_read to read data into a buffer, and f_close to close files

```c++
FIL* fp;
f_open(fp, "my_wav.wav", FA_READ | FA_OPEN_EXISTING);
f_read(fp, *destination, bytesToRead, &bytesRead);
```

PLEASE NOTE that bytesToRead MUST be a multiple of 2048 for adequate performance, and that each read operation has a significant delay before the start of data transfer (on the order of 500us-4ms, depending on prior card state), and the user should do their best to request data in as large of batches as possible, rather than making many small read requests. Additionally note that the destination pointer MUST be in `DMA_BUFFER_MEM_SECTION` for it to be DMA-accessible and therefore writeable by the filesystem worker. This can be ensured via the following declaration pattern. All uses of `DMA_BUFFFER_MEM_SECTION` MUST total to less than 512KB of storage due to AXI SRAM size constraints.

```c++
uint8_t FATFS_BUFFER_MEM_SECTION destination[BUFFER_SIZE];
```