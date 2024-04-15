#include "fatfs.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"
#include "sdmmc-driver.h"
#include <cstdint>

uint8_t FATFS_BUFFER_MEM_SECTION buff[512];
FATFS FATFS_BUFFER_MEM_SECTION mfs;

FatFsIntf::FatFsIntf() : sd() {

  fs = &mfs;

  if (FATFS_LinkDriver(&SD_Driver, my_path) != FR_OK) {
    __asm__ __volatile__("bkpt #0");
  }

  FRESULT s = f_mount(fs, my_path, 0);

  if (s) {
    __asm__ __volatile__("bkpt #0");
  }

  switch (s) {
  case FR_DISK_ERR:
    __asm__ __volatile__("bkpt #0");
    break;
  case FR_INT_ERR:
    __asm__ __volatile__("bkpt #0");
    break;
  case FR_NOT_READY:
    __asm__ __volatile__("bkpt #0");
    break;
  case FR_NO_FILESYSTEM:
    __asm__ __volatile__("bkpt #0");
    break;
  case FR_MKFS_ABORTED:
    __asm__ __volatile__("bkpt #0");
    break;
  case FR_TIMEOUT:
    __asm__ __volatile__("bkpt #0");
    break;
  case FR_LOCKED:
    __asm__ __volatile__("bkpt #0");
    break;
  case FR_NOT_ENOUGH_CORE:
    __asm__ __volatile__("bkpt #0");
    break;
  case FR_TOO_MANY_OPEN_FILES:
    __asm__ __volatile__("bkpt #0");
    break;
  case FR_INVALID_PARAMETER:
    __asm__ __volatile__("bkpt #0");
    break;

  default:
    break;
  }

  FIL fp;

  UINT bRead;

  if (f_open(&fp, "my_wav.wav", FA_READ | FA_OPEN_EXISTING) != FR_OK) {
    __asm__ __volatile__("bkpt #0");
  }
  if (f_read(&fp, buff, 512, &bRead) != FR_OK) {
    __asm__ __volatile__("bkpt #0");
  }
}

extern "C" {
DWORD get_fattime(void) { return 0; }
}
