#ifndef MIDI_EVENT_H
#define MIDI_EVENT_H

#include <stdint.h>
#include <stddef.h>

enum MidiMessageType
{
    NoteOff,
    NoteOn,
    KeyPressure,
    ControlChange,
    ProgramChange,
    ChannelPressure,
    PitchBend,
    SystemCommon,
    SystemRealTime,
    ChannelMode,
    MessageLast
};

enum SystemCommonType
{
    SystemExclusive,
    MTCQuarterFrame,
    SongPositionPointer,
    SongSelect,
    SCUndefined0,
    SCUndefined1,
    TuneRequest,
    SysExEnd,
    SystemCommonLast
};

enum SystemRealTimeType
{
    TimingClock,
    SRTUndefined0,
    Start,
    Continue,
    Stop,
    SRTUndefined1,
    ActiveSensing,
    Reset,
    SystemRealTimeLast
};

enum ChannelModeType
{
    AllSoundOff,
    ResetAllControllers,
    LocalControl,
    AllNotesOff,
    OmniModeOff,
    OmniModeOn,
    MonoModeOn,
    PolyModeOn,
    ChannelModeLast
};

struct MidiEvent
{
    MidiMessageType messageType;
    uint8_t channel;
    uint8_t data[2];
    SystemCommonType sysComType;
    SystemRealTimeType sysRealTimeType;
    ChannelModeType channelModeType;
};
#endif