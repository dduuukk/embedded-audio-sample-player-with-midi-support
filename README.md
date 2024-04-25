# Embedded Audio Player on FreeRTOS Based System

## Abstract
What, what, status.
What is the objective in this lab? Describe the lab technical aspects, show its aim and the main
findings.

Audio sample player using an STM32H7 embedded platform and custom breakout board.


This project involves the development of a FreeRTOS-based audio streaming system on an STM32 development board. The system integrates various components, including an audio codec, SD card reader, STM32 microprocessor, H7 DMA modules, and a headphone amplifier. Utilizing an SD card for data storage, the system employs a FATFS file system for file access. A .WAV parser within the STM32 MCU interprets .WAV file headers and audio data from the SD card, facilitating audio playback. Communication between the MCU and the codec is established through the STM32 Serial Audio Interface (SAI) peripheral, utilizing I2S compatibility. Additionally, an I2C connection configures the codec. The primary objective is to parse and play .WAV files stored on the SD card. A potential stretch goal involves implementing a user interface with buttons and an OLED screen for enhanced interaction.




## Intro
Briefly outline the area your project applies to.
Highlight what is missing in current approaches.
Define the problem you want to solve and why it's important.

The project delves into the realm of embedded systems, focusing specifically on the development of an audio streaming system using FreeRTOS on an STM32 development board. In embedded systems, integrating audio streaming capabilities poses significant challenges, especially concerning resource utilization and system complexity.

Current approaches often lack a.....


The integration of audio streaming capabilities into embedded systems presents a complex challenge, particularly in environments with stringent resource constraints. In this project, we embark on the development of a sophisticated audio streaming system using a FreeRTOS-based approach on an STM32 development board. Central to the system architecture are components such as an audio codec, SD card reader, STM32 microprocessor, H7 DMA modules, and a headphone amplifier. Leveraging the capabilities of the STM32 MCU and its peripherals, including the Serial Audio Interface (SAI) and Inter-Integrated Circuit (I2C), our goal is to create a robust platform for parsing and playing .WAV files stored on an SD card. This introductory overview provides a glimpse into the intricacies of our system design, highlighting key components and objectives. Additionally, we outline a potential stretch goal involving the implementation of a user interface for enhanced user interaction. Through meticulous integration and software development, we aim to achieve seamless audio playback functionality while exploring avenues for further feature enhancements.












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

To allow an external controller to interface with out system, a MIDI handler has been implemented to translate incoming MIDI over UART data into usable information. The driver handles the reception and storage of UART data, parsing of data into MIDI events, and providing stored MIDI events as needed by the system. This allows for the loading and playing of different .WAV files stored on the SD card upon button press.

The primary objective is to parse a .WAV file stored on an SD card and play it through the headphone output, using MIDI to control the sounds played. Achieving this involves triggering playing using MIDI, sending .WAV data from the SD card to the MCU, processing that data and sending it to the DMA FIFO, and then sending that data to the codec over SAI/I2S.

A system diagram that highlights the interaction between these different components is shown in the below system diagram.

![alt text](docs/boomba-system-midi.png)

## Contributions
Who did what with links to where you can find the design documentation for each component.

@dduuukk (Christian Bender) was responsible for completing the audio driver. The design, implementation, and operation of the audio driver is discussed in more detail [here](src/drivers/sai-design.md).

Katherine Cloutier locked in doing the [WAV file parser](src/wav-parser/Readme-wav-parser.md).

Christian Cole (@cole-ch) was responsible for the development of the audio codec driver and the MIDI handler. More information about audio codec driver can be found [here](docs/i2c-codec-driver/i2c-codec-design.md), and more information about the MIDI handler can be found [here](docs/uart-midi-driver/uart-midi-design.md).



## Implementation Progress
Assess our goals in the project, were our goals met? Where did we fall short?

## Future Work
What comes next?
