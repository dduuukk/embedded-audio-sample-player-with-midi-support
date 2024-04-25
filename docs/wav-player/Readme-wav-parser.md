# WAV File Parser

This code is a WAV file parser written in C for microcontroller applications. It provides functions to read WAV file headers, validate WAV files, handle stereo and mono formats, and play audio samples. The parser is designed to work with WAV files and can be integrated into projects requiring audio playback functionality.

## Introduction

This WAV file parser is intended for embedded systems where resources are limited, hence it's written in C for efficiency. It allows reading and validating WAV file headers, handling stereo and mono formats, and playing audio samples. The parser assumes a basic understanding of WAV file structure and is designed to be integrated into larger projects for audio playback.

## Usage 

To use this code, include the necessary header files (`wav-parser.h`, `fatfs.h`, `codec_wm8731.h`) in your project. Ensure that you have a compatible microcontroller platform with sufficient memory and processing capabilities.

## Design and Challenges
The beginning code was taken from Lab 5/6 and turned from a Linux platform to run on a BareMetal platform. Due to changes the libc library was not able to be used, this included no use of the printf function, no kernel, and no fseek or f function. To work around this these functions were reimplemented and fseek was changed to f_lseek and fread was changed to f_read. 


## Implementaion 

This was used in tandem with the codec and audio driver. To configure the codec the sample rate and bits per sample were passed through. The audio driver was called to transmit the data array. 


## Functions 

#### `read_wave()`

This function reads the WAV file header and populates a `struct wave_header` with relevant information such as chunk ID, chunk size, format, audio format, sample rate, etc.

#### `validate_wave()`

This function validates whether the WAV file is intact and valid by checking the RIFF file header and WAVE file format. It also configures the input data length and sample rate for the codec.

#### `write_word()`

This function writes a 32-bit word into a buffer. In this implementation, it stores the word in an array and triggers transmission when the array is full.

#### `audio_word_from_buf()`

This function builds a 32-bit audio word from a buffer based on the WAV header information and the buffer content.

#### `play_wave_samples()`

This function plays WAV audio samples. It reads samples from the WAV file and transmits them using the configured codec. It supports both mono and stereo formats.

#### `handleStereoMono()`

This function handles stereo and mono formats, indicating whether the WAV file contains one or two audio channels.


## Lessons learned 


#### Understanding File Formats 

A deep understanding of the WAV file format allowed us to learned about the structure of WAV files, including chunk headers, sample formats, and audio data layout. This understanding was essential for accurately parsing and interpreting the contents of WAV files.

#### Error Handling and Validation  

Validating input data is critical, especially in embedded systems where reliability is paramount. Implementing thorough error checking and validation mechanisms ensured that the parser could handle various WAV file formats and detect potential corruption or errors in input files.








