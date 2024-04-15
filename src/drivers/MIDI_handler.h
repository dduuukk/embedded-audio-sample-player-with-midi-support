#ifndef MIDI_HANDLER_H
#define MIDI_HANDLER_H

#include <stdint.h>
#include <stddef.h>

#include "circular_buffer.h"
#include "MIDI_event.h"
#include "MIDI_parser.h"

template <std::size_t S1, std::size_t S2>
class midiHandler
{
public:
	midiHandler();

	void enqueueByte(uint8_t data);

	uint8_t dequeueByte();

	bool midiRecieveCheckEmpty();

	void parse(uint8_t byte);

	bool midiEventCheckEmpty();

	MidiEvent dequeueEvent();

private:
	circleBuffer<uint8_t, S1> midiRecieveBuffer;
	circleBuffer<MidiEvent, S2> midiEventBuffer;
	midiParser parser;
};

template <std::size_t S1, std::size_t S2> midiHandler<S1, S2>::midiHandler()
{

}

template <std::size_t S1, std::size_t S2> void midiHandler<S1, S2>::enqueueByte(uint8_t data)
{
	midiRecieveBuffer.enqueueData(data);
}

template <std::size_t S1, std::size_t S2> uint8_t midiHandler<S1, S2>::dequeueByte()
{
	return midiRecieveBuffer.dequeueData();
}

template <std::size_t S1, std::size_t S2> bool midiHandler<S1, S2>::midiRecieveCheckEmpty()
{
	return midiRecieveBuffer.checkEmpty();
}

template <std::size_t S1, std::size_t S2> void midiHandler<S1, S2>::parse(uint8_t byte)
{
	MidiEvent event;
	if (parser.parse(byte, &event))
	{
		midiEventBuffer.enqueueData(event);
	}
}

template <std::size_t S1, std::size_t S2> bool midiHandler<S1, S2>::midiEventCheckEmpty()
{
	return midiEventBuffer.checkEmpty();
}

template <std::size_t S1, std::size_t S2> MidiEvent midiHandler<S1, S2>::dequeueEvent()
{
	return midiEventBuffer.dequeueData();
}

#endif