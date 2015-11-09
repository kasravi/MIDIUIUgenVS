// PuppetMaster Input Library (MIDI)
// Copyright 2001 by Jon Parise <jparise@cmu.edu>
//
// Based on code from:
//  Copyright (c) 1999 by Niels Gorisse <niels@bonneville.nl>
//  Modified 2001 by Wil Paredes <paredes@andrew.cmu.edu>

#ifndef __MIDI_DEVICE_H__
#define __MIDI_DEVICE_H__

#include <windows.h>
#include <mmsystem.h>

#define MIDI_BUFFERSIZE 32768

class MidiDevice
{
private:
    HMIDIIN device;

    unsigned long queue[MIDI_BUFFERSIZE];
    unsigned int queue_read_pos;
    unsigned int queue_write_pos;

    void addData(unsigned long data);

    static void CALLBACK midiCallback(HMIDIIN device, UINT status, DWORD instancePtr, DWORD data, DWORD timestamp);

public:
    MidiDevice();
    ~MidiDevice();

    // Device manipulation
    void open(unsigned int device_id);
    void close();

    // Device reading
    unsigned int poll(unsigned short* buf);
};

#endif /* __MIDI_DEVICE_H__ */
