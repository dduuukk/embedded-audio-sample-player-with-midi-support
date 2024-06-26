#include "wav-parser.h"
#include "codec_wm8731.h"
#include "fatfs.h"
#include "sai-driver.h"
#include <cstdint>

uint16_t DMA_BUFFER_MEM_SECTION outputBuffer[UINT16_MAX];

/*
reads the file and puts correcct information into the struct
*/
void read_wave(uint8_t *fp, struct wave_header *dest) {

  // going through the array instead of a file now
  // something like desk.chunk->fp[0] slighty werid thins is theuint 32 to uint
  // 8 combination uint32 means 4 uint 8 combines, bytes 0,1,2,3 put together
  // ...

  dest->chunkID = fp[0] | (fp[1] << 8) | (fp[2] << 16) | (fp[3] << 24);
  dest->chunkSize = fp[4] | (fp[5] << 8) | (fp[6] << 16) | (fp[7] << 24);
  dest->format = fp[8] | (fp[9] << 8) | (fp[10] << 16) | (fp[11] << 24);
  dest->subchunk1ID = fp[12] | (fp[13] << 8) | (fp[14] << 16) | (fp[15] << 24);
  dest->subchunk1Size =
      fp[16] | (fp[17] << 8) | (fp[18] << 16) | (fp[19] << 24);

  dest->audioFormat = fp[20] | (fp[21] << 8);
  dest->numChannels = fp[22] | (fp[23] << 8);

  dest->sampleRate = fp[24] | (fp[25] << 8) | (fp[26] << 16) | (fp[27] << 24);
  dest->byteRate = fp[28] | (fp[29] << 8) | (fp[30] << 16) | (fp[31] << 24);

  dest->blockAlign = fp[32] | (fp[33] << 8);
  dest->bitsPerSample = fp[34] | (fp[35] << 8);

  dest->subchunk2ID = fp[36] | (fp[37] << 8) | (fp[38] << 16) | (fp[39] << 24);
  dest->subchunk2Size =
      fp[40] | (fp[41] << 8) | (fp[42] << 16) | (fp[43] << 24);
}

/*
validation checks to ensure the WAV file is intact and valid
*/
int validate_wave(struct wave_header *wavHeader) {

  // verify that this is a RIFF file header
  if ((wavHeader->chunkID != (0x46464952))) {
    return -1;
  }
  // verify that this is WAVE file
  if (wavHeader->format != (0x45564157)) {
    return -1;
  }

  return 0;
}

uint32_t indx = 0;
uint32_t indxOut = 0;
uint32_t midTransferCount = 0;

void write_word(uint16_t lWord, uint16_t rWord, SAIDriver &SAIBDriver) {
  // Check if the output buffer is full (i.e. the buffer is uint16 MAX)
  if (indxOut >= UINT16_MAX) {
    // Transmit the buffer to the DMA
    SAIBDriver.txTransmit(reinterpret_cast<uint8_t *>(outputBuffer), UINT16_MAX,
                          0);
    // Wait for the DMA to finish
    while (SAIBDriver.returnDMABusy()) {
      // Block and wait
    }
    // Clear the output buffer
    indxOut = 0;
    midTransferCount++;
  }

  outputBuffer[indxOut] = lWord;
  indxOut++;
  outputBuffer[indxOut] = rWord;
  indxOut++;
}

/* @brief Build a 32-bit audio word from a buffer
   @param hdr WAVE header
   @param buf a byte array
   @return 32-bit word */
uint16_t audio_word_from_buf(struct wave_header wavHeader, uint8_t *buf) {
  // build word depending on bits per sample, etc
  uint16_t audio_word = 0;
  audio_word = (buf[0]) | (buf[1] << 8);

  // // Loop through the number of bytes in a sample, perform shifting
  // for (int i = 0; i < wavHeader.bitsPerSample / 8; i++) {
  //   // audio_word |= (buf[i] << (24 - (i * 8)));
  //   audio_word |= (buf[i] << ((i * 8) + 16));
  // }
  // volatile uint8_t byte1 = buf[0];
  // volatile uint8_t byte2 = buf[1];
  // __asm__ __volatile__("bkpt #0");
  return audio_word;
}

// calculate buffer size for audio data depending on stereo or mono
uint8_t play_wave_samples(uint8_t *fp, struct wave_header *wavHeader,
                          uint32_t sample_count, unsigned int start,
                          SAIDriver &SAIBDriver) {

  if (!fp) {
    return -1;
  }

  if (wavHeader->numChannels != 1 && wavHeader->numChannels != 2) {
    wavHeader->numChannels = 2;
  }

  if (wavHeader->bitsPerSample != 8 && wavHeader->bitsPerSample != 16 &&
      wavHeader->bitsPerSample != 24) {
    wavHeader->bitsPerSample = 16;
  }

  uint8_t bytesPerSample = wavHeader->bitsPerSample / 8;
  uint8_t lbuf[2];
  uint8_t rbuf[2];

  while (indx < sample_count * 2) {
    if (wavHeader->numChannels == 2) // Seperate into two different buffers for
                                     // left and right, for 2-channel audio
    {
      // stereo
      for (uint8_t i = 0; i < bytesPerSample; i++) {
        lbuf[i] = fp[start + indx * bytesPerSample + i];
      }
      indx++;
      for (uint8_t i = 0; i < bytesPerSample; i++) {
        rbuf[i] = fp[start + indx * bytesPerSample + i];
      }
      write_word(audio_word_from_buf(*wavHeader, lbuf),
                 audio_word_from_buf(*wavHeader, rbuf), SAIBDriver);
    } else // mono
    {
      for (uint8_t i = 0; i < bytesPerSample; i++) {
        lbuf[i] = fp[start + indx * bytesPerSample + i];
      }

      write_word(audio_word_from_buf(*wavHeader, lbuf),
                 audio_word_from_buf(*wavHeader, lbuf), SAIBDriver);
    }
    indx++;
  }

  SAIBDriver.txTransmit(reinterpret_cast<uint8_t *>(outputBuffer),
                        sample_count - start - midTransferCount * UINT16_MAX,
                        2000);
  indx = 0;
  indxOut = 0;
  midTransferCount = 0;

  return 0;
}