# Embedded Audio Sample Player with MIDI Support

## Abstract

This project involves the development of a bare-metal audio sample player with MIDI support on an STM32H7 development board. The primary objective is to parse and play .WAV files stored on the SD card when a MIDI keypress is recieved. Currently the system successfully parses MIDI messages, reads .wav files off of the SD card, and streams audio through the codec to a line out and a headphone out. We successfully minimized latency, with imperceptible delay between keypress and sample start, and achieved a 95% similarity index between our hardware output and the expected output (with the minimal differences likely being a product of D/A conversion rather than any implementation error)


## Intro
Briefly outline the area your project applies to.
Highlight what is missing in current approaches.
Define the problem you want to solve and why it's important.

The project delves into the realm of bare-metal embedded systems, focusing specifically on the development of an audio sample player system using an STM32H7 development board, the Electrosmith Daisy, and our own custom PCB. In embedded systems, integrating audio streaming capabilities poses significant challenges, especially concerning resource utilization, latency, and system complexity.

This project involves the development of a bare-metal audio sample player with MIDI support on an STM32H7 development board. The system integrates various hardware components, including an audio codec, SD card reader, STM32H750 ARM microprocessor, and UART MIDI interface. The system employs a FATFS file system for interaction with the SD card. A .WAV parser within the STM32 MCU interprets .WAV file headers and audio data from the SD card, facilitating audio playback. Communication between the MCU and the codec is established through the STM32 Serial Audio Interface (SAI) peripheral, utilizing I2S compatibility. Additionally, an I2C connection configures the codec. The primary objective is to parse and play .WAV files stored on the SD card when a MIDI keypress is recieved.

This is a problem solved by many other samplers, such as the Teenage Engineering PO-133, and various software solutions, but our goal was to use this project to bootstrap our custom hardware.


## Related Work
Highlight similar approaches.
What is the reason for our new implementation?

Daisy library? https://github.com/electro-smith/libDaisy

Daisy wav player: https://github.com/electro-smith/DaisyExamples/blob/master/seed/WavPlayer/WavPlayer.cpp

Daisy based midi controller: https://github.com/heavyweight87/MidiController


## Top Level Design Overview
This project involved bootstrapping an audio streaming system on an STM32 development board, which includes the utilization of an audio codec, SD card reader and SD card, STM32 microprocessor, H7 DMA modules, MIDI, and a headphone amplifier. 

The system architecture utilizes an SD card which interfaces with the STM32 MCU through SDIO. A FATFS file system has been implemented for SD card file access. The MCU hosts a .WAV parser responsible for interpreting the header and audio data of the .WAV file from the SD card, transforming it into audio words for playback. This parsed data is then be sent to the codec through a DMA FIFO and an SAI peripheral.

Using the STM32 Serial Audio Interface (SAI) peripheral, (which is I2S-compatible), communication between the STM32 and the codec is established. An internal DMA channel acts as a FIFO, and buffers the samples to be sent to the codec. The codec is also connected to the STM32 via I2C, which is used for configuration. 

A MIDI keyboard interacts with the .WAV player through UART and a MIDI parser, which loads and plays different .WAV files stored on the SD card upon button press.

The primary objective is to parse a .WAV file stored on an SD card and play it through the headphone output, using MIDI to control the sounds played. Achieving this involves triggering playing using MIDI, sending .WAV data from the SD card to the MCU, processing that data and sending it to the DMA FIFO, and then sending that data to the codec over SAI/I2S.

A system diagram that highlights the interaction between these different components is shown in the below system diagram.

![alt text](docs/boomba-system-midi.png)

## Contributions
Who did what with links to where you can find the design documentation for each component.

@dduuukk (Christian Bender) was responsible for completing the audio driver. The design, implementation, and operation of the audio driver is discussed in more detail [here](src/drivers/sai-design.md).

Katherine Cloutier implemented the [WAV file parser](src/wav-parser/Readme-wav-parser.md).



## Implementation Progress
Assess our goals in the project, were our goals met? Where did we fall short?

## Future Work
What comes next?
