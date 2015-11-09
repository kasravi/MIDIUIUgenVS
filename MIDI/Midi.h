// PuppetMaster Input Library (MIDI)
// Copyright 2001 by Jon Parise <jparise@cmu.edu>

#ifndef __MIDI_H__
#define __MIDI_H__

#include <windows.h>

#include "MidiDevice.h"

#define MIDI_POLL_INTERVAL  10      // milliseconds
#define MAX_MIDI_EVENTS     8192

struct MidiEvent {
    int type;
    int channel;
    int value;
};

// Define DLL import / export macros
#ifdef MIDI_EXPORTS
#define MIDI_API __declspec(dllexport)
#else
#define MIDI_API __declspec(dllimport)
#endif

// Export this class in MIDI.dll
class MIDI_API Midi {
private:
    unsigned int    device_id;

    MidiEvent       events[MAX_MIDI_EVENTS];
    unsigned int    events_read_pos;
    unsigned int    events_write_pos;

    // Threading stuff
    HANDLE          threadHandle;
    DWORD           threadID;
    bool            isRunning;

    void decode(unsigned short *buf, int size);

public:
	Midi(unsigned int device_id);

    unsigned int poll(MidiEvent buf[MAX_MIDI_EVENTS]);

    // Thread Interface
    DWORD threadProc();
    void threadBegin();
    void threadEnd();

    // Device information
    static void deviceName(unsigned int num, char *name);
    static UINT deviceCount();
};

static DWORD WINAPI midiThreadProc(Midi *instance);

#endif /* __MIDI_H__ */