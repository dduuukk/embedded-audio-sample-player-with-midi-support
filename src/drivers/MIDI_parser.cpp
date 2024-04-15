#include <stdint.h>
#include <stddef.h>

#include "MIDI_parser.h"

midiParser::midiParser()
{
	state = Empty;
	runningStatus = MessageLast;
}

bool midiParser::parse(uint8_t byte, MidiEvent* event_out)
{
	bool parseComplete = false;

	if ((byte & STATUS_BIT_MASK) && state != SysEx)
	{
		state = Empty;
	}

	switch (state)
	{
	case Empty:
		if (byte & STATUS_BIT_MASK) //handle as a status byte
		{
			currentEvent.channel = byte & CHANNEL_MASK;
			currentEvent.messageType = static_cast<MidiMessageType>((byte & MSG_TYPE_MASK) >> 4);

			if((byte & 0xF8) == 0xF8) //
				currentEvent.messageType = SystemRealTime;

			if (currentEvent.messageType < MessageLast)
			{
				state = HasStatus;

				if (currentEvent.messageType == SystemCommon)
				{
					//handle system common
					//system common includes sysex and the various system common commands
				}
				else if (currentEvent.messageType == SystemRealTime)
				{
					currentEvent.sysRealTimeType = static_cast<SystemRealTimeType>(byte & SYS_RT_MASK);

					state = Empty; //event is complete, send event and go back to start

					if (event_out != nullptr)
						*event_out = currentEvent;

					parseComplete = true;
				}
				else
					runningStatus = currentEvent.messageType;
			}
		}
		else //handle as running status
		{
			currentEvent.messageType = runningStatus;
			currentEvent.data[0] = byte & DATA_BYTE_MASK;

			if (runningStatus == ProgramChange || runningStatus == ChannelPressure)
			{
				state = Empty;

				if (event_out != nullptr)
					*event_out = currentEvent;

				parseComplete = true;
			}
			else
			{
				state = HasData0;
			}
		}
		break;

	case HasStatus:
		if ((byte & STATUS_BIT_MASK) == 0)
		{
			currentEvent.data[0] = byte & DATA_BYTE_MASK;

			if (runningStatus == ProgramChange || runningStatus == ChannelPressure)
			{
				state = Empty;
				
				if (event_out != nullptr)
					*event_out = currentEvent;

				parseComplete = true;
			}
			else
			{
				state = HasData0;
			}

			if (runningStatus == ControlChange && currentEvent.data[0] > 119)
			{
				//handle channel mode message
			}
		}
		else //something broke, gotta go bald
		{
			state = Empty;
		}
		break;

	case HasData0:
		if ((byte & STATUS_BIT_MASK) == 0)
		{
			currentEvent.data[1] = byte & DATA_BYTE_MASK;

			if (runningStatus == NoteOn && currentEvent.data[1] == 0)
			{
				currentEvent.messageType = NoteOff;
			}

			if (event_out != nullptr)
				*event_out = currentEvent;

			parseComplete = true;
		}
		else //something broke, gotta go bald
		{
			state = Empty;
		}

		state = Empty; //we've processed all data for an event regardless at this point
		break;

	case SysEx:
		//handle sysex
		break;

	default:
		break;
	}

	return parseComplete;
}