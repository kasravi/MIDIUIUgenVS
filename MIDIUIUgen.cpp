#include "SC_PlugIn.h"
#include "Midi.h"
#include "utils.h"
static InterfaceTable *ft;
Midi *midi;
bool initiated=false;
MidiEvent buf[MAX_MIDI_EVENTS];
//pthread_t Thread;
THREAD Thread;

struct MIDIUIUgenGlobalState {
	uint8 value;
} gMIDIUIUgenGlobals[256][256];

struct MIDIUIUgen : public Unit{
	float m_y1, m_b1, m_lag;
};

void gstate_update_func(void* arg)
{
	for(;;) {
		if(initiated)
		{
			int count = 0;
			if(initiated)
				count = midi->poll(buf);
			for (int i = 0; i < count; i++) {
				(gMIDIUIUgenGlobals[ buf[i].type][ buf[i].channel]).value= buf[i].value;}
		}
		::Sleep(17);
	}
	return ;
}
void MIDIUIUgen_next(MIDIUIUgen *unit, int inNumSamples)
{
	uint8 type = ZIN0(0);
	uint8 channel = ZIN0(1);
    float minval = ZIN0(2);
	float maxval = ZIN0(3);
	float warp = ZIN0(4);
	float lag = ZIN0(5);

	float y1 = unit->m_y1;
	float b1 = unit->m_b1;

	if (lag != unit->m_lag) {
		unit->m_b1 = lag == 0.f ? 0.f : (float)exp(log001 / (lag * unit->mRate->mSampleRate));
		unit->m_lag = lag;
	}
	float value = (float)(gMIDIUIUgenGlobals[type][channel]).value/(float)127;
	if (warp == 0.0) {
		value = (maxval - minval) * value + minval;
	} else {
		value = pow(maxval/minval, value) * minval;
	}
	ZOUT0(0) = y1 = value + b1 * (y1 - value);
	unit->m_y1 = zapgremlins(y1);
	
}
void MIDIUIUgen_Ctor(MIDIUIUgen* unit)
{
	if(!initiated)
	{
		unsigned int midi_device = 0;
		char name[64];
		Print("Number of MIDI Input devices: %d\n", Midi::deviceCount());
		if( Midi::deviceCount()==0)
			return;
		Midi::deviceName(midi_device, name);
		Print("Device Name: %s\n", name);
		midi = new Midi(midi_device);
	    midi->threadBegin();
		initiated=true;
	}
    SETCALC(MIDIUIUgen_next);
	unit->m_b1 = 0.f;
	unit->m_lag = 0.f;
	MIDIUIUgen_next(unit, 1);
}

void MIDIUIUgen_Dtor(MIDIUIUgen* unit)
{
	if(initiated)
	{
		initiated=false;
		midi->threadEnd();
		delete midi;
		Print("MIDI Device Detached\n");
	}
}
PluginLoad(MIDIUIUgen)
{
    ft = inTable;
	THREAD_START(&Thread, gstate_update_func, (void*)0);
	DefineDtorUnit(MIDIUIUgen);
}