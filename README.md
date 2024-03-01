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

Katherine: Parsing wave


Issues
Research and thinking related

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

  
Testing related
12. Test with Sample WAV File
- evaluate parsing with WAV files of varying sizes
13. Test with Large WAV Files
- evaluate parsing with WAV files of varying sizes

  
Documentation related 
14. Document Process 
15. Update README with Instructions


