// Private oscillators used by Sms_Apu

// Sms_Snd_Emu 0.1.4
#ifndef SMS_OSCS_H
#define SMS_OSCS_H

#include "blargg_common.h"
#include "Blip_Buffer.h"

struct Sms_Osc
{
	Blip_Buffer* outputs [4]; // NULL, right, left, center
	Blip_Buffer* output;
	int output_select;
<<<<<<< HEAD
	
	int delay;
	int last_amp;
	int volume;
	
=======

	int delay;
	int last_amp;
	int volume;

>>>>>>> db7344ebf (abc)
	Sms_Osc();
	void reset();
};

struct Sms_Square : Sms_Osc
{
	int period;
	int phase;
<<<<<<< HEAD
	
	typedef Blip_Synth<blip_good_quality,1> Synth;
	const Synth* synth;
	
=======

	typedef Blip_Synth<blip_good_quality,1> Synth;
	const Synth* synth;

>>>>>>> db7344ebf (abc)
	void reset();
	void run( blip_time_t, blip_time_t );
};

struct Sms_Noise : Sms_Osc
{
	const int* period;
	unsigned shifter;
	unsigned feedback;
<<<<<<< HEAD
	
	typedef Blip_Synth<blip_med_quality,1> Synth;
	Synth synth;
	
=======

	typedef Blip_Synth<blip_med_quality,1> Synth;
	Synth synth;

>>>>>>> db7344ebf (abc)
	void reset();
	void run( blip_time_t, blip_time_t );
};

#endif
