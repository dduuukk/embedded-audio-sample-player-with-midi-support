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





/*
data struct for WAV file

*/
struct wave_header
{
  uint32_t chunkID; //"RIFF" (0x52494646 big-endian form) 4
  uint32_t chunkSize; // file size - 8 bytes  4
  uint32_t format; // "WAVE" (0x57415645 big-endian form)  4

  //"WAVE" format, "fmt" and "data"
  //"fmt" describes the sound data's format


  uint32_t subchunk1ID; //"fmt " (0x666d7420 big-endian form) 4
  uint32_t subchunk1Size; // size of theformat chunk 16 for PCM 4
  uint16_t audioFormat; // PCM=1  2
  uint16_t numChannels; // Mono = 1, Stereo = 2, etc. 2
  uint32_t sampleRate; // 8000,44100, etc.  4
  uint32_t byteRate; //= SampleRate * NumChannels * BitsPerSample/8  4
  uint16_t blockAlign; //= NumChannels * BitsPerSample/8   The number of bytes for one sample including all channels  2
  uint16_t bitsPerSample;  // 8 bits = 8, 16 bits = 16, etc.  2

  // data subchunk that contains the size of the data and the actual sound
  uint32_t subchunk2ID; //"data" (0x64617461 big-endian form)  4
  uint32_t subchunk2Size; //= NumSamples * NumChannels * BitsPerSample/8  This is the number of bytes in the data.  4
};


/*
reads the file and puts correcct information into the struct

*/
void read_wave(FILE* fp, struct wave_header* dest)
{




}












/*

validation checks to ensure the WAV file is intact and valid

*/
int validate_wave(struct wave_header* wavHeader)
{

// verify that this is a RIFF file header
  if((wavHeader->chunkID != (0x46464952)))
    return -1;
  //verify that this is WAVE file
  if(wavHeader->format != (0x45564157))
    return -1;

 

    return 0;
}



/*

prints all stuff in wave_header struct

*/
void list_wave(struct wave_header* wavHeader) 
{
        printf("ChunkID: %u\n",wavHeader->chunkID);
        printf("ChunkSize: %d\n",wavHeader->chunkSize);
        printf("Format: %c\n",wavHeader->format);



        printf("SubChunk1ID: %c\n",wavHeader->subchunk1ID);
        printf("Subchunk1Size: %d\n",wavHeader->subchunk1Size);
        printf("AudioFormat: %d\n",wavHeader->audioFormat);
        printf("NumChannels: %d\n",wavHeader->numChannels); 
        printf("SampleRate: %d\n",wavHeader->sampleRate);
        printf("ByteRate: %d\n",wavHeader->byteRate);     
        printf("BlockAlign: %d\n",wavHeader->blockAlign);   
        printf("BitsPerSample: %d\n",wavHeader->bitsPerSample);

        printf("Subchunk2ID: %c\n",wavHeader->subchunk2ID);
        printf("Subchunk2Size: %d\n",wavHeader->subchunk2Size);
 
}



/* From Chris  */
void fifo()
{

}

int main(int argc, char** argv)
{

    FILE* fp; //From Noah 







}

















