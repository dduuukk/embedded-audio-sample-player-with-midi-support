# Project BOOMBA: Embedded Audio Player On FreeRTOS-based System

## Description
The project involves bootstrapping a FreeRTOS-based audio streaming system on an STM32 development board, which includes the utilization of an audio codec, SD card reader and SD card, STM32 microprocessor, H7 DMA modules, and a headphone amplifier. 

The system architecture utilizes an SD card which interfaces with the STM32 MCU through SDIO. A FATFS file system will be implemented for SD card file access. The MCU will host a .WAV parser responsible for interpreting the header and audio data of the .WAV file from the SD card, transforming it into audio words for playback. This parsed data will then be sent to the codec through a DMA FIFO and an SAI peripheral.

Using the STM32 Serial Audio Interface (SAI) peripheral, (which is I2S-compatible), communication between the STM32 and the codec will be established. An internal DMA channel will act as a FIFO, and will buffer the samples to be sent to the codec. The codec is also connected to the STM32 via I2C, which will be used for configuration. 

The primary objective is to parse a .WAV file stored on an SD card and play it through the headphone output. Achieving this involves sending .WAV data from the SD card to the MCU, processing that data and sending it to the DMA FIFO, and then sending that data to the codec over SAI/I2S. Additionally, if there is time, there is a stretch goal that will involve implementing a user interface with buttons and an OLED screen for enhanced user interaction.

## System diagram
![system diagram](IMG_0088.jpg)

## Team Members And Responsibilities 

### Katherine Cloutier: Parsing wave

1. Research WAV File Format
- look at past labs that delt with this
- find resources and documentation that may be helful
2. Design WAV File Header Structure
- define a data struct for WAV file
3. Validate WAV File Integrity
- validation checks to ensure the WAV file is intact and valid
4. Implement Header Parsing Function
- create a function to parse the header of a WAV file
5. Extract Audio Format Information
- extract audio format details from the WAV file header
6. Get Sample Rate and Bit Depth
- extract sample rate and bit depth information from the WAV file header
7. Handle Stereo and Mono Format
- compatibility with both stereo and mono audio formats
8. Implement Error Handling for WAV Parsing
- error conditions and implement error handling
9. Buffer Management for Audio Data
- storing audio data read from the WAV file
10. Handle Endianness and Data Alignment
- big endian and little endian endianness and data alignment in the WAV file
11. Integrate Parsing with Audio Playback
- parsing with audio playback functionality

### Christian Bender: DMA FIFO Management

Tasks will involve setting up an H7 DMA (Direct Memory Access) as a FIFO (First-In-First-Out) to manage audio samples for the SAI (Serial Audio Interface) FIFO. The DMA will receive buffered .WAV samples processed by the STM32. The DMA FIFO, configured as a larger storage buffer (likely 512-1024 samples), will feed data to the SAI audio 8-word FIFO, which will subsequently output to the audio codec.

To achieve this, reference documentation for the STM32H7 series will be used, such as the resource manual available [here](https://www.st.com/resource/en/reference_manual/dm00314099-stm32h742-stm32h743-753-and-stm32h750-value-line-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf). The DMA will serve as a bridge between the STM32-processed audio samples and the SAI interface FIFO.

Key steps include initializing the DMA to function as a FIFO, establishing communication between the larger DMA FIFO and the SAI audio 8-word FIFO, and getting input samples from the STM37.

This task is a crucial element in the overall project, enabling the smooth flow of audio data through the system. The codec and Christian's work will rely on the successful buffering of samples, and I will rely on the STM32 parsing for audio sample filling.


### Christian Cole

Tasks will involve managing the system's audio codec, the Wolfson WM8731. This codec will first be configured over I2C based on the file it will be recieving samples from. To manage audio, we will simlarly configure the STM32's SAI communication with the codec to manage clock timings and data transfer from the dedicated 8-word SAI FIFO.

To achieve this, the [WM8731's datasheet](https://cdn.sparkfun.com/datasheets/Dev/Arduino/Shields/WolfsonWM8731.pdf) will be utilized to understand the specifics of integration between the STM32H7 and the codec. Likewise, the [STM32H7's resource manual](https://www.st.com/resource/en/reference_manual/dm00314099-stm32h742-stm32h743-753-and-stm32h750-value-line-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf) will be used to manage the SAI system for audio data transfer.

Audio ouput will be available via a direct hardware line-out or a headphone amplifier that has already been designed on the custom development board.


### Noah Mitchell

Tasks will involve determining which filesystem library to use for interfacing with the SD card filesystem in software (potentially FatFS, FreeRTOS+Fat, or LittleFS), writing communication/peripheral drivers for the SD card using the SDMMC peripheral of the STM32H7 (described starting on page 2378 of the [STM32H7 reference manual](https://www.st.com/resource/en/reference_manual/dm00314099-stm32h742-stm32h743-753-and-stm32h750-value-line-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf)), and implementing file system handling functions in a preemption-safe/thread-aware manner. FreeRTOS configuration and task setup will also need to occur. 

The overall goal will be to pass a file pointer to Kat's .WAV parsing code, along with an easy-to-use, thread-safe, performant interface for getting the data out of the file.


<!-- ## Description 
Bootstrap a FreeRTOS based system on an STM32 dev board with an audio
codec, SD card reader, and headphone amplifier. Port WAV parser/player, and write drivers for
codec (I2S) and headphone driver
## End goal: parse a .WAV file off of the SD card on the system and play it over the headphone out
## Stretch goal: UI with buttons and the OLED screen!

- Bootstrap a FreeRTOS based system on an STM32 dev board with an audio
codec, SD card reader, and headphone amplifier. Port WAV parser/player, and write drivers for
codec (I2S) and headphone driver
- end goal: parse a .WAV file off of the SD card on the system and play it over the headphone out
- Stretch goal: UI with buttons and the OLED screen!
- SD card with an SDIO interface to MCU
    - FS interface is FATFS
- On MCU, wav parser that takes in a file pointer to a wav file on the SD card
    - Parses header and audio data into audio words
- Codec and STM32 i2C interface for codec configuration
- SAI (which is i2s compatible) interface between STM32 and Codec, with a DMA in the middle that acts as a FIFO
    - Setup the DMA as a FIFO
- Output the audio to a headphone out






## Work involved: 
 -->
