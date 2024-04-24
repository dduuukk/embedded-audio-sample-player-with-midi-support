#ifndef FATFS_H_
#define FATFS_H_

#include "ff.h"
#include "sdmmc-driver.h"

class FatFsIntf {
public:
  FatFsIntf();

private:
  SDMMC sd;
  FATFS *fs;
  char my_path[4] = {};
};

#endif // FATFS_H_