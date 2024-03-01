# BOOMBA
## Project proposal: embedded audio player on freeRTOS-based system

## Team members and responsibilities 
- Christian Bender
- Katherine Cloutier
- Christian Cole
- Noah Mitchell 



## Work involved: 
Bootstrap a FreeRTOS based system on an STM32 dev board with an audio
codec, SD card reader, and headphone amplifier. Port WAV parser/player, and write drivers for
codec (I2S) and headphone driver
## End goal: parse a .WAV file off of the SD card on the system and play it over the headphone out
## Stretch goal: UI with buttons and the OLED screen!


## Overall Description

## System diagram
(with individual components and their interfaces. This visualization should take the initial feedback into account and be more detailed than the Project Idea submission.)

## Major components


### Issue synopsis

- Katherine

Katherine: Parsing wave


Issues
Research and thinking related

Research WAV File Format
- look at past labs that delt with this
- find resources and documentation that may be helful
Design WAV File Header Structure
- define a data struct for WAV file
Validate WAV File Integrity
- validation checks to ensure the WAV file is intact and valid
Implement Header Parsing Function
- create a function to parse the header of a WAV file
Extract Audio Format Information
- extract audio format details from the WAV file header
Get Sample Rate and Bit Depth
- extract sample rate and bit depth information from the WAV file header
Handle Stereo and Mono Format
- compatibility with both stereo and mono audio formats
Implement Error Handling for WAV Parsing
- error conditions and implement error handling
Buffer Management for Audio Data
- storing audio data read from the WAV file
Handle Endianness and Data Alignment
- big endian and little endian endianness and data alignment in the WAV file
Integrate Parsing with Audio Playback
- parsing with audio playback functionality

  
Testing related
Test with Sample WAV File
- evaluate parsing with WAV files of varying sizes
Test with Large WAV Files
- evaluate parsing with WAV files of varying sizes

  
Documentation related 
Document Process 
Update README with Instructions


