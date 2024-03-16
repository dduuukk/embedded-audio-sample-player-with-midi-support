/*
Katherine Cloutier: Parsing wave




*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <alsa/asoundlib.h>
#define SND_CARD "default"

#include <libzed/axi_gpio.h>
#include <libzed/zed_common.h>
#include <libzed/mmap_regs.h>
#include <fcntl.h>
#include <sys/mman.h>





// Wave header from lab 5
struct wave_header
{
  uint32_t chunkID;
  uint32_t chunkSize;
  uint32_t format;

  uint32_t subchunk1ID;
  uint32_t subchunk1Size;
  uint16_t audioFormat;
  uint16_t numChannels;
  uint32_t SampleRate;
  uint32_t byteRate;
  uint16_t blockAlign;
  uint16_t bitsPerSample;

  uint32_t subchunk2ID;
  uint32_t subchunk2Size;
};


int check_file_name(char *filename); //checks file name



void list(char **array) //prints all stuff in wave_header struct
{
    wavHeader wave_header;
    FILE *pFile;


    if(check_file_name(array[2]) == 0)
    {
        printf("wrong file name\n");
        exit(1);
    }
    pFile = fopen (array[2] ,"r");



    if( pFile != NULL)
    {
        fread(&wave_header, sizeof(wave_header), 1, pFile);
        fclose(pFile);
        printf("ChunkID: %c%c%c%c\n",wave_header.chunkID);
        printf("ChunkSize: %d\n",wave_header.chunkSize);
        printf("Format: %c%c%c%c\n",wave_header.format);

        printf("SubChunk1ID: %c%c%c%c\n",wave_header.subchunk1ID);
        printf("Subchunk1Size: %d\n",wave_header.subchunk1Size);
        printf("AudioFormat: %d\n",wave_header.audioFormat);
        printf("NumChannels: %d\n",wave_header.numChannels); 
        printf("SampleRate: %d\n",wave_header.sampleRate);
        printf("ByteRate: %d\n",wave_header.byteRate);     
        printf("BlockAlign: %d\n",wave_header.blockAlign);   
        printf("BitsPerSample: %d\n",wave_header.bitsPerSample);

        printf("Subchunk2ID: %c%c%c%c\n",wave_header.subchunk2ID);
        printf("Subchunk2Size: %d\n",wave_header.subchunk2Size);
    }
    else
    {
        printf("This file doesn't exit\n");
        exit(1);
    }
}


















