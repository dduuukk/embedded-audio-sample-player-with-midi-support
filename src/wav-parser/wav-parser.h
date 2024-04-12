#ifndef WAVE_H
#define WAVE_H


#include "fatfs.h"
#include "codec_wm8731.h"

struct wave_header
{
    uint32_t chunkID;       //"RIFF" (0x52494646 big-endian form) 4
    uint32_t chunkSize;     // file size - 8 bytes  4
    uint32_t format;        // "WAVE" (0x57415645 big-endian form)  4

    //"WAVE" format, "fmt" and "data"
    //"fmt" describes the sound data's format
    uint32_t subchunk1ID;   //"fmt " (0x666d7420 big-endian form) 4
    uint32_t subchunk1Size; // size of theformat chunk 16 for PCM 4
    uint16_t audioFormat;   // PCM=1  2
    uint16_t numChannels;   // Mono = 1, Stereo = 2, etc. 2

    uint32_t sampleRate;    // 8000,44100, etc.  4
    uint32_t byteRate;      //= SampleRate * NumChannels * BitsPerSample/8  4

    uint16_t blockAlign;    //= NumChannels * BitsPerSample/8   The number of bytes for one sample including all channels  2
    uint16_t bitsPerSample; // 8 bits = 8, 16 bits = 16, etc.  2

    // data subchunk that contains the size of the data and the actual sound
    uint32_t subchunk2ID;   //"data" (0x64617461 big-endian form)  4
    uint32_t subchunk2Size; //= NumSamples * NumChannels * BitsPerSample/8  This is the number of bytes in the data.  4
};

void read_wave(FILE* fp, struct wave_header* dest);
int validate_wave(struct wave_header* wavHeader);
int8_t buffSize(struct wave_header* wavHeader);
void handleStereoMono(struct wave_header* wavHeader);
uint32_t audio_word_from_buf(struct wave_header wavHeader, int8_t* buf);
void write_word(int32_t word);
void pr_usage(char* pname)


#endif /* WAVE_H */