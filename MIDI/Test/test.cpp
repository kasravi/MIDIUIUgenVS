// MIDI Input Library Test Application
// Copyright 2001 by Jon Parise <jparise@cmu.edu>
//
// $Id: test.cpp,v 1.1 2001/10/02 19:44:21 jon Exp $

#include <stdio.h>

#include "../Midi.h"

void
midi_test()
{
    unsigned int midi_device = 0;
    char name[64];

    printf("Number of MIDI Input devices: %d\n", Midi::deviceCount());

    Midi::deviceName(midi_device, name);
    printf("Device Name: %s\n", name);

    Midi *midi = new Midi(midi_device);
    MidiEvent buf[MAX_MIDI_EVENTS];

    midi->threadBegin();
    printf("Sleeping ...\n");
    Sleep(5 * 1000);
    int count = midi->poll(buf);
    printf("Waking ...\n");
    midi->threadEnd();

    printf("Dumping buffer:\n");
    for (int i = 0; i < count; i++) {
        printf("Type: %02d  Channel: %02d  Value: %03d\n", buf[i].type, buf[i].channel, buf[i].value);
    }

    delete midi;
}

void
main()
{
    printf("MIDI Input Library Test Application\n");
    printf("Jon Parise <jparise@cmu.edu>\n\n");

    midi_test();
    //midi_device_test();
}