#include "fatfs.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"
#include "sdmmc-driver.h"

FatFsIntf::FatFsIntf() : sd(), fs() {

  if (FATFS_LinkDriver(&SD_Driver, my_path) != FR_OK) {
    __asm__ __volatile__("bkpt #0");
  }

  if (f_mount(&fs, my_path, 1) != FR_OK) {
    __asm__ __volatile__("bkpt #0");
  }
}

extern "C" {
DWORD get_fattime(void) { return 0; }
}
