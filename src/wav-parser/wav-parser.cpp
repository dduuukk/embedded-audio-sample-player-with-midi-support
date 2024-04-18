#include "wav-parser.h"


#include "fatfs.h"
#include "codec_wm8731.h"


void pr_usage(char* pname) 
{
  //printf("usage: %s WAV_FILE\n", pname);
}





/*
reads the file and puts correcct information into the struct

*/
void read_wave(uint8_t *fp, struct wave_header *dest) {

  // GET BIT DEPTH AND SAMPLE RATE

  // are the destination and file valid?
  if (!dest || !fp) {
    // return -ENOENT;
  }



  //going through the array instead of a file now
  // something like desk.chunk->fp[0] slighty werid thins is theuint 32 to uint 8 combination
  // uint32 means 4 uint 8 combines, bytes 0,1,2,3 put together ...

  dest->chunkID = fp[0] | (fp[1] << 8) | (fp[2] << 16) | (fp[3] << 24);
  dest->chunkSize = fp[4] | (fp[5] << 8) | (fp[6] << 16) | (fp[7] << 24);
  dest->format = fp[8] | (fp[9] << 8) | (fp[10] << 16) | (fp[11] << 24);
  dest->subchunk1ID = fp[12] | (fp[13] << 8) | (fp[14] << 16) | (fp[15] << 24);
  dest->subchunk1Size = fp[16] | (fp[17] << 8) | (fp[18] << 16) | (fp[19] << 24);


  dest->audioFormat = fp[20] | (fp[21] << 8) ;
  dest->numChannels = fp[22] | (fp[23] << 8) ;

  dest->sampleRate = fp[24] | (fp[25] << 8) | (fp[26] << 16) | (fp[27] << 24);
  dest->byteRate = fp[28] | (fp[29] << 8) | (fp[30] << 16) | (fp[31] << 24);


  dest->blockAlign = fp[32] | (fp[33] << 8) ;
  dest->bitsPerSample = fp[34] | (fp[35] << 8) ;


  
  dest->subchunk2ID = fp[36] | (fp[37] << 8) | (fp[38] << 16) | (fp[39] << 24);
  dest->subchunk2Size = fp[40] | (fp[41] << 8) | (fp[42] << 16) | (fp[43] << 24);

/*

  // read header
  int x = 0;
  x = f_lseek(fp, 0);
  x = f_read(fp, &(dest->chunkID), sizeof(uint32_t), NULL);

  x = f_lseek(fp, 4);
  x = f_read(fp, &(dest->chunkSize), sizeof(uint32_t), NULL);

  x = f_lseek(fp, 8);
  x = f_read(fp, &(dest->format), sizeof(uint32_t), NULL);

  x = f_lseek(fp, 12);
  x = f_read(fp, &(dest->subchunk1ID), sizeof(uint32_t), NULL);

  x = f_lseek(fp, 16);
  x = f_read(fp, &(dest->subchunk1Size), sizeof(uint32_t), NULL);



  x = f_lseek(fp, 20);
  x = f_read(fp, &(dest->audioFormat), sizeof(uint16_t), NULL);

  x = f_lseek(fp, 22);
  x = f_read(fp, &(dest->numChannels), sizeof(uint16_t), NULL);




  x = f_lseek(fp, 24);
  x = f_read(fp, &(dest->sampleRate), sizeof(uint32_t),
             NULL); // SAMPLE RATEEEEEEEEEEEEEEEEEEEE for CC
  x = f_lseek(fp, 28);
  x = f_read(fp, &(dest->byteRate), sizeof(uint32_t), NULL);



  x = f_lseek(fp, 32);
  x = f_read(fp, &(dest->blockAlign), sizeof(uint16_t), NULL);

  x = f_lseek(fp, 34);
  x = f_read(fp, &(dest->bitsPerSample), sizeof(uint16_t),
             NULL); // BIT DEPTHHHHHHHHHHHHHHH for CC



  x = f_lseek(fp, 36);
  x = f_read(fp, &(dest->subchunk2ID), sizeof(uint32_t), NULL);

  x = f_lseek(fp, 40);
  x = f_read(fp, &(dest->subchunk2Size), sizeof(uint32_t), NULL);
*/
  // printf("Expected file size: %d, Actual file size: %ld\n", (dest->chunkSize
  // + 8), st.st_size);

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

  configureInputDataLength(wavHeader->bitsPerSample);
  configureSampleRate(wavHeader->sampleRate);

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

  return 0;
}

uint8_t index = 0;

void write_word(uint32_t word) 
{
  uint32_t fifussy[32]; // 32 words in fifo then i will throw it to chris

  fifussy[index] = word;

  
  if (index >= 32)
   {
    /*
      call sainb transmit to pass though array w data
      file sys goes in , data goes out to fifo, put data from struct into array
      pass through bit depth and sample rate

      */

    SAIBDriver.SAINBTransmit(fifussy, sizeof(uint32_t) * 32, 2000);

    index = 0;
  }

  index++;

}

/*
void write_word(int32_t word)
{
  //int x = system("echo \"hello\">/dev/echo0");
  fwrite(&word, sizeof(word), 1, fp2);
  //;
}
for write word
asking you to make array of 32 qudio words an you will fill that array with 32
audio words than call transmit instead of sending it one by one


bby calling transmit ti will send it to chsis fifi

*/

/* @brief Build a 32-bit audio word from a buffer
   @param hdr WAVE header
   @param buf a byte array
   @return 32-bit word */
uint32_t audio_word_from_buf(struct wave_header wavHeader, int8_t *buf) {
  // build word depending on bits per sample, etc
  uint32_t audio_word = 0;

  for (int i = 0; i < wavHeader.bitsPerSample / 8; i++) {

    audio_word |= (uint32_t)((buf[i] + 127) << (8 * ((24 / wavHeader.bitsPerSample - 1) - i)));
  }

  return audio_word << 8;
}

// calculate buffer size for audio data depending on stereo or mono
int8_t play_wave_samples(FIL *fp, struct wave_header *wavHeader,
                         int sample_count, unsigned int start) {

  if (!fp) {
    return -1;
  }

  if (wavHeader->numChannels != 1 && wavHeader->numChannels != 2) {

    return -1;
  }

  int x = f_lseek(fp, 44 + start);

  int8_t totalSamples = wavHeader->sampleRate;
  int8_t bytesPerSample = wavHeader->bitsPerSample / 8;

  int8_t buf[(bytesPerSample)*wavHeader->numChannels];

  int8_t lbuf[bytesPerSample];
  int8_t rbuf[bytesPerSample];

  while (sample_count > 0) {

    x = f_read(fp, buf, ((bytesPerSample)) * wavHeader->numChannels, NULL);

    if (wavHeader->numChannels == 2) // Seperate into two different buffers for
                                     // left and right, for 2-channel audio
    {
      // stereo

      for (int i = 0; i < (bytesPerSample)*wavHeader->numChannels; i += 2) {
        lbuf[i] = buf[i];
        rbuf[i] = buf[i + 1];
      }

      write_word(audio_word_from_buf(*wavHeader, lbuf));
      write_word(audio_word_from_buf(*wavHeader, rbuf));
      sample_count -= 1;

    } else // mono
    {

      write_word(audio_word_from_buf(*wavHeader, buf)); // For the left channel
      write_word(audio_word_from_buf(*wavHeader, buf)); // For the right channel

      sample_count -= 1;

      // return buf[(bytesPerSample)*wavHeader->numChannels]; // For the left
      // channel and the right channel
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
void handleStereoMono(struct wave_header *wavHeader) {

  if (wavHeader->numChannels == 1) // means one audio channel
  {
    // printf("Mono\n");
    // return 0;
  } else if (wavHeader->numChannels == 2) // left and right audio channel
  {
    // printf("Stereo\n");
    // return 0;
  } else {
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
