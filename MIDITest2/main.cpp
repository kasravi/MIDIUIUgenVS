// MIDI Input Library Test Application
// Copyright 2001 by Jon Parise <jparise@cmu.edu>
//
// $Id: test.cpp,v 1.1 2001/10/02 19:44:21 jon Exp $

#include <stdio.h>
#include <conio.h>
#include "Midi.h"

char * getline(void) {
	char * line = (char*)malloc(100), *linep = line;
	size_t lenmax = 100, len = lenmax;
	int c;

	if (line == NULL)
		return NULL;

	for (;;) {
		c = fgetc(stdin);
		if (c == EOF)
			break;

		if (--len == 0) {
			len = lenmax;
			char * linen = (char*)realloc(linep, lenmax *= 2);

			if (linen == NULL) {
				free(linep);
				return NULL;
			}
			line = linen + (line - linep);
			linep = linen;
		}

		if ((*line++ = c) == '\n')
			break;
	}
	*line = '\0';
	return linep;
}

void
midi_test()
{
	unsigned int midi_device = 0;
	char name[64];

	printf("Number of MIDI Input devices: %d\n", Midi::deviceCount());

	for (int i = 0; i < Midi::deviceCount(); i++)
	{
		Midi::deviceName(i, name);
		printf("%d+Device Name: %s\n", i, name);
	}
	//printf("please select your preferred midi device: ");
	//char * inputMidiDevice = getline();
	/*try
	{
		midi_device = atoi(inputMidiDevice);
	}
	catch (int e){
		printf("Incorrect Midi Device!");
		getch();
		exit(0);
	}*/
	//Midi::deviceName(midi_device, name);
	//printf("Your Device Name: %s\n", name);
	Midi *midi[10];
		
	MidiEvent buf[MAX_MIDI_EVENTS];
	int device_count = Midi::deviceCount();
	for (int i = 0; i < device_count; i++)
	{
		midi[i] = new Midi(i);
		midi[i]->threadBegin();
	}
	printf("Sleeping ...\n");
	//Sleep(50 * 1000);
	while (!_kbhit())
	{
		for (int j = 0; j < device_count; j++)
		{
			int count = midi[j]->poll(buf);
			for (int i = 0; i < count; i++) {
				printf("device:%02d Type: %02d  Channel: %02d  Value: %03d\n",j, buf[i].type, buf[i].channel, buf[i].value);
			}
		}

	}
	printf("Waking ...\n");
	midi[1]->threadEnd();

	/*printf("Dumping buffer:\n");
	for (int i = 0; i < count; i++) {
	printf("Type: %02d  Channel: %02d  Value: %03d\n", buf[i].type, buf[i].channel, buf[i].value);
	*/

	delete midi[1];
}

void
main()
{
	midi_test();
	//midi_device_test();
}