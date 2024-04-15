#ifndef MIDI_PARSER_H
#define MIDI_PARSER_H

#include "circular_buffer.h"
#include "MIDI_event.h"

#define STATUS_BIT_MASK 0x80
#define MSG_TYPE_MASK   0x70
#define CHANNEL_MASK    0x0F
#define DATA_BYTE_MASK  0x7F
#define SYS_RT_MASK     0x07

class midiParser
{
public:
	midiParser();

	bool parse(uint8_t byte, MidiEvent* event_out);

private:
	enum ParserState
	{
		Empty,
		HasStatus,
		HasData0,
		SysEx
	};

	ParserState state;
	MidiEvent currentEvent;
	MidiMessageType runningStatus;
};

#endif