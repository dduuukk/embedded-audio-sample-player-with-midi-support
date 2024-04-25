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
In exploring related work for our project, several notable efforts in audio streaming and sampling on the Daisy and STM32H7 platforms were found. 

Firstly, the [Daisy Library](https://github.com/electro-smith/libDaisy) stands out, offering drivers which enable various audio processes on the Daisy development board, such as audio file streaming and MIDI parsing. We used this library as reference during the development of our code.

Other software solutions exist that also resemble our board's functionality. For instance, Daisy offers a [.WAV player](https://github.com/electro-smith/DaisyExamples/blob/master/seed/WavPlayer/WavPlayer.cpp) that, similar to our implementation, streams audio files from an SD card. Additionally, [Daisy-based MIDI controllers](https://github.com/heavyweight87/MidiController) exist, though primarily designed for other purposes such as guitar pedals, provide similar functionality in terms of parsing MIDI data.

Our implementation serves two primary purposes. Firstly, it sets the stage for a future soft synth project on the Daisy board, laying essential groundwork and drivers for future development. By bypassing the Daisy library, we gain finer control over development and ensure compatibility with custom hardware, enabling greater efficiency and integration. Secondly, this is a personal project, and was fun to complete.

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

@dduuukk (Christian Bender) was responsible for completing the audio driver. The design, implementation, and operation of the audio driver is discussed in more detail [here](docs/sai-audio-driver/sai-design.md).

Katherine Cloutier locked in doing the [WAV file parser](docs/wav-player/Readme-wav-parser.md).



## Implementation Progress
Assess our goals in the project, were our goals met? Where did we fall short?

## Future Work
What comes next?
