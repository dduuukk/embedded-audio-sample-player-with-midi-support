#include "ParseWav.h"

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


void pr_usage(char* pname)
{
  //printf("usage: %s WAV_FILE\n", pname);
}





/*
reads the file and puts correcct information into the struct

*/
void read_wave(FILE *fp, struct wave_header *dest)
{

  // GET BIT DEPTH AND SAMPLE RATE

  // are the destination and file valid?
  if (!dest || !fp)
  {
    // return -ENOENT;
  }

  rewind(fp); // Resets the file pointer

  struct stat st;
  fstat(fileno(fp), &st);
  if (!(S_ISREG(st.st_mode) && st.st_size > 44))
  {
    // return -1;
  }

  // read header
  int x = 0;
  x = fseek(fp, 0, SEEK_SET);
  x = f_read(&(dest->chunkID), sizeof(uint32_t), 1, fp);

  x = fseek(fp, 4, SEEK_SET);
  x = f_read(&(dest->chunkSize), sizeof(uint32_t), 1, fp);

  x = fseek(fp, 8, SEEK_SET);
  x = f_read(&(dest->format), sizeof(uint32_t), 1, fp);

  x = fseek(fp, 12, SEEK_SET);
  x = f_read(&(dest->subchunk1ID), sizeof(uint32_t), 1, fp);

  x = fseek(fp, 16, SEEK_SET);
  x = f_read(&(dest->subchunk1Size), sizeof(uint32_t), 1, fp);

  x = fseek(fp, 20, SEEK_SET);
  x = f_read(&(dest->audioFormat), sizeof(uint16_t), 1, fp);

  x = fseek(fp, 22, SEEK_SET);
  x = f_read(&(dest->numChannels), sizeof(uint16_t), 1, fp);

  x = fseek(fp, 24, SEEK_SET);
  x = f_read(&(dest->sampleRate), sizeof(uint32_t), 1, fp); // SAMPLE RATEEEEEEEEEEEEEEEEEEEE for CC
  x = fseek(fp, 28, SEEK_SET);
  x = f_read(&(dest->byteRate), sizeof(uint32_t), 1, fp);

  x = fseek(fp, 32, SEEK_SET);
  x = f_read(&(dest->blockAlign), sizeof(uint16_t), 1, fp);

  x = fseek(fp, 34, SEEK_SET);
  x = f_read(&(dest->bitsPerSample), sizeof(uint16_t), 1, fp); // BIT DEPTHHHHHHHHHHHHHHH for CC

  x = fseek(fp, 36, SEEK_SET);
  x = f_read(&(dest->subchunk2ID), sizeof(uint32_t), 1, fp);

  x = fseek(fp, 40, SEEK_SET);
  x = f_read(&(dest->subchunk2Size), sizeof(uint32_t), 1, fp);

  // printf("Expected file size: %d, Actual file size: %ld\n", (dest->chunkSize + 8), st.st_size);

  // return !((dest->chunkSize + 8) == st.st_size);
}
/*

validation checks to ensure the WAV file is intact and valid

*/
int validate_wave(struct wave_header *wavHeader)
{

  // verify that this is a RIFF file header
  if ((wavHeader->chunkID != (0x46464952)))
  {

    // return -1;
  }
  // verify that this is WAVE file
  if (wavHeader->format != (0x45564157))
  {

    // return -1;
  }

  /*
   * @param length Input data length, 0x00 for 16-bits,
   *                                  0x04 for 20-bits,
   *                                  0x08 for 24-bits,
   *                                  0x0B for 32-bits
   * */

  void configureInputDataLength(wavHeader->bitsPerSample);
  void configureSampleRate(wavHeader->sampleRate);

  /*


      * @param length Input data length, 0x00 for 16-bits,
      *                                  0x04 for 20-bits,
      *                                  0x08 for 24-bits,
      *                                  0x0B for 32-bits

      void configureInputDataLength(uint8_t length);

      /**
      * Quick configuration of the codec sample rate
      *
      * Allows for quick configuration of the sample rate for the codec's DAC and ADC.
      * Input parameter can be plain hex or the defined macros. Refer to the datasheet
      * for detailed information about setting the sample rates. Function only writes
      * to the bits specific to the bypass mode.
      *
      * @param sampleRate Sample rate mode of the DAC and ADC

      void configureSampleRate(uint8_t sampleRate);


  call codec. configure sample rate and pass in the sample rate

  codec.configure bitdepth pass in bit depth

  */

  // return 0;
}

#define index 0

void write_word(int32_t word)
{
  int32_t fifussy[32]; // 32 words in fifo then i will throw it to chris

  fifussy[index] = word;

  index++

      if (sizeof(fifussy) >= 32)
  {

    f_write(words, sizeof(int32_t), sizeof(fifussy), fp2);

    for (int i = 0; i < 32; i++)
    {
      fifussy[i] = 0;
    }
  }
}

/*
void write_word(int32_t word)
{
  //int x = system("echo \"hello\">/dev/echo0");
  fwrite(&word, sizeof(word), 1, fp2);
  //;
}
for write word
asking you to make array of 32 qudio words an you will fill that array with 32 audio words than call transmit
instead of sending it one by one


bby calling transmit ti will send it to chsis fifi

*/

/* @brief Build a 32-bit audio word from a buffer
   @param hdr WAVE header
   @param buf a byte array
   @return 32-bit word */
uint32_t audio_word_from_buf(struct wave_header wavHeader, int8_t *buf)
{
  // build word depending on bits per sample, etc
  uint32_t audio_word = 0;

  for (int i = 0; i < hdr.bitsPerSample / 8; i++)
  {

    audio_word |= (uint32_t)((buf[i] + 127) << (8 * ((24 / hdr.bitsPerSample - 1) - i)));
  }

  return audio_word << 8;
}

// calculate buffer size for audio data depending on stereo or mono
int8_t play_wave_samples(FILE *fp, struct wave_header *wavHeader, int sample_count, unsigned int start)
{

  if (!fp)
  {
    return -EINVAL;
  }

  if (hdr.numChannels != 1 && hdr.numChannels != 2)
  {

    return -EINVAL;
  }

  int x = fseek(fp, 44 + start, SEEK_SET);

  int8_t totalSamples = wavHeader->sampleRate;
  int8_t bytesPerSample = wavHeader->bitsPerSample / 8;
  int8_t bufferSize = totalSamples * wavHeader->numChannels * bytesPerSample;

  int8_t buf[(bytesPerSample)*wavHeader->numChannels];

  int8_t lbuf[(bytesPerSample)];
  int8_t rbuf[(bytesPerSample)];

  while (sample_count > 0)
  {

    x = fread(buf, sizeof(int8_t), ((bytesPerSample)) * wavHeader->numChannels, fp);

    if (wavHeader->numChannels == 2) // Seperate into two different buffers for left and right, for 2-channel audio
    {
      // mono

      for (int i = 0; i < (bytesPerSample)*wavHeader->numChannels; i += 2)
      {
        lbuf[i] = buf[i];
        rbuf[i] = buf[i + 1];
      }

      write_word(audio_word_from_buf(wavHeader, lbuf));
      write_word(audio_word_from_buf(wavHeader, rbuf));
      sample_count -= 1;

      return lrbuf[(bytesPerSample)];
    }
    else // stereo
    {

      write_word(audio_word_from_buf(wavHeader, buf)); // For the left channel
      write_word(audio_word_from_buf(wavHeader, buf)); // For the right channel

      sample_count -= 1;
      i += 2;

      // return buf[(bytesPerSample)*wavHeader->numChannels]; // For the left channel and the right channel
    }
  }

  return 0;
}

/*

from buff stays same



play wave samples stays ame

*/

/*
Handle Stereo and Mono Format
Mono = 1, Stereo = 2

*/
void handleStereoMono(struct wave_header *wavHeader)
{

  if (wavHeader->numChannels == 1) // means one audio channel
  {
    // printf("Mono\n");
    // return 0;
  }
  else if (wavHeader->numChannels == 2) // left and right audio channel
  {
    // printf("Stereo\n");
    // return 0;
  }
  else
  {
    // printf("Error\n");
    // return -1;
  }
}

/*

prints all stuff in wave_header struct
no print >:((


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
*/



/*

main is same basiclly just no pcm and to i2s


*/

int main(int argc, char **argv)
{


  int err;

  FILE *fp; // From Noah


  struct wave_header wavHeader;

  unsigned int sample_rate;

    if (argc < 2)
    {
      // fail, print usage
      pr_usage(argv[0]);
      return 1;
    }
  
  
  // open file
  fp = fopen(argv[1], "r");



// read file header
  if(read_wave_header(fp, &wavHeader) != 0)
  {
    printf("Error: Incorrect File Format\n");
    return -1;
  }

  // parse file header, verify that is wave
  if(parse_wave_header(wavHeader) != 0)
  {
    printf("Error: Incorrect WAV format\n");
    return -1;
  }


 

  // TODO play sound (from pre-lab 5a)
  play_wave_samples(fp, wavHeader, -1, 0);
  

  fclose(fp);
  


  return 0;













}
