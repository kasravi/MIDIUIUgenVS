// PuppetMaster Input Library (MIDI)
// Copyright 2001 by Jon Parise <jparise@cmu.edu>
//
// Based on code from:
//  Copyright (c) 1999 by Niels Gorisse <niels@bonneville.nl>
//  Modified 2001 by Wil Paredes <paredes@andrew.cmu.edu>

#include "MidiDevice.h"

MidiDevice::MidiDevice()
{
    device = NULL;
    queue_read_pos = 0;
    queue_write_pos = 0;
}

MidiDevice::~MidiDevice()
{
    close();
}

void
MidiDevice::open(unsigned int device_id)
{
    MMRESULT result;

    // Make sure the device ID is valid.
    if (device_id > midiInGetNumDevs()) {
        return;
    }

    result = midiInOpen(&device, device_id, (DWORD)&(midiCallback), (DWORD)(this), CALLBACK_FUNCTION);
    if (result != MMSYSERR_NOERROR) {
        return;
    }

    result = midiInStart(device);
    if (result != MMSYSERR_NOERROR) {
        return;
    }
}

void
MidiDevice::close()
{
    MMRESULT result;

    if (device == NULL) {
        return;
    }

    result = midiInReset(device);
    if (result != MMSYSERR_NOERROR) {
        return;
    }

    result = midiInClose(device);
    if (result != MMSYSERR_NOERROR) {
        return;
    }

    device = NULL;
}

void CALLBACK
MidiDevice::midiCallback(HMIDIIN device, UINT status, DWORD instancePtr, DWORD data, DWORD timestamp)
{
    if (status == MIM_DATA) {
        ((MidiDevice*)instancePtr)->addData(data);
    }
}

void
MidiDevice::addData(unsigned long data)
{
    // Push this piece of data onto the end of the queue
    queue[queue_write_pos] = data;
    queue_write_pos++;
    if (queue_write_pos == MIDI_BUFFERSIZE) {
        queue_write_pos = 0;
    }
}

unsigned int
MidiDevice::poll(unsigned short* buf)
{
    unsigned int count = 0;

     // make sure the object is open
    if (device == NULL) {
         return 0;
    }

    union {
        unsigned long   ulong;      // unsigned long (DWORD)
        unsigned short  ushorts[2]; // unsigned short (jchar)
        unsigned char   uchars[2];  // unsigned char (MIDI)
    } u;

    while ((queue_read_pos != queue_write_pos) && (count < MIDI_BUFFERSIZE)) {
        u.uchars[0] = 63;   // short
        u.uchars[1] = 3;    // Length
        buf[count] = u.ushorts[0];

        // Get the first entry in the queue
        u.ulong = queue[queue_read_pos];
        queue_read_pos++;
        if (queue_read_pos == MIDI_BUFFERSIZE) {
            queue_read_pos = 0;
        }

        buf[count + 1] = u.ushorts[0];
        buf[count + 2] = u.ushorts[1];

        // Advance the counter
        count += 3;
    }

    return count;
}