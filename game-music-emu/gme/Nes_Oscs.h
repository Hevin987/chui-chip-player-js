// Private oscillators used by Nes_Apu

<<<<<<< HEAD
// Nes_Snd_Emu $vers
=======
// Nes_Snd_Emu 0.1.8
>>>>>>> db7344ebf (abc)
#ifndef NES_OSCS_H
#define NES_OSCS_H

#include "blargg_common.h"
#include "Blip_Buffer.h"

class Nes_Apu;

struct Nes_Osc
{
<<<<<<< HEAD
	typedef int nes_time_t;
	
=======
>>>>>>> db7344ebf (abc)
	unsigned char regs [4];
	bool reg_written [4];
	Blip_Buffer* output;
	int length_counter;// length counter (0 if unused by oscillator)
	int delay;      // delay until next (potential) transition
	int last_amp;   // last amplitude oscillator was outputting
<<<<<<< HEAD
	
=======

>>>>>>> db7344ebf (abc)
	void clock_length( int halt_mask );
	int period() const {
		return (regs [3] & 7) * 0x100 + (regs [2] & 0xFF);
	}
	void reset() {
		delay = 0;
		last_amp = 0;
	}
	int update_amp( int amp ) {
		int delta = amp - last_amp;
		last_amp = amp;
		return delta;
	}
};

struct Nes_Envelope : Nes_Osc
{
	int envelope;
	int env_delay;
<<<<<<< HEAD
	
=======

>>>>>>> db7344ebf (abc)
	void clock_envelope();
	int volume() const;
	void reset() {
		envelope = 0;
		env_delay = 0;
		Nes_Osc::reset();
	}
};

// Nes_Square
struct Nes_Square : Nes_Envelope
{
	enum { negate_flag = 0x08 };
	enum { shift_mask = 0x07 };
	enum { phase_range = 8 };
	int phase;
	int sweep_delay;
<<<<<<< HEAD
	
	typedef Blip_Synth_Norm Synth;
	Synth const& synth; // shared between squares
	
	Nes_Square( Synth const* s ) : synth( *s ) { }
	
=======

	typedef Blip_Synth<blip_good_quality,1> Synth;
	Synth const& synth; // shared between squares

	Nes_Square( Synth const* s ) : synth( *s ) { }

>>>>>>> db7344ebf (abc)
	void clock_sweep( int adjust );
	void run( nes_time_t, nes_time_t );
	void reset() {
		sweep_delay = 0;
		Nes_Envelope::reset();
	}
	nes_time_t maintain_phase( nes_time_t time, nes_time_t end_time,
			nes_time_t timer_period );
};

// Nes_Triangle
struct Nes_Triangle : Nes_Osc
{
	enum { phase_range = 16 };
	int phase;
	int linear_counter;
<<<<<<< HEAD
	Blip_Synth_Fast synth;
	
=======
	Blip_Synth<blip_med_quality,1> synth;

>>>>>>> db7344ebf (abc)
	int calc_amp() const;
	void run( nes_time_t, nes_time_t );
	void clock_linear_counter();
	void reset() {
		linear_counter = 0;
		phase = 1;
		Nes_Osc::reset();
	}
	nes_time_t maintain_phase( nes_time_t time, nes_time_t end_time,
			nes_time_t timer_period );
};

// Nes_Noise
struct Nes_Noise : Nes_Envelope
{
	int noise;
<<<<<<< HEAD
	Blip_Synth_Fast synth;
	
=======
	Blip_Synth<blip_med_quality,1> synth;

>>>>>>> db7344ebf (abc)
	void run( nes_time_t, nes_time_t );
	void reset() {
		noise = 1 << 14;
		Nes_Envelope::reset();
	}
};

// Nes_Dmc
struct Nes_Dmc : Nes_Osc
{
	int address;    // address of next byte to read
	int period;
	//int length_counter; // bytes remaining to play (already defined in Nes_Osc)
	int buf;
	int bits_remain;
	int bits;
	bool buf_full;
	bool silence;
<<<<<<< HEAD
	
	enum { loop_flag = 0x40 };
	
	int dac;
	
=======

	enum { loop_flag = 0x40 };

	int dac;

>>>>>>> db7344ebf (abc)
	nes_time_t next_irq;
	bool irq_enabled;
	bool irq_flag;
	bool pal_mode;
	bool nonlinear;
<<<<<<< HEAD
	
	Nes_Apu* apu;
	
	Blip_Synth_Fast synth;
	
	int  update_amp_nonlinear( int dac_in );
=======

	int (*prg_reader)( void*, nes_addr_t ); // needs to be initialized to prg read function
	void* prg_reader_data;

	Nes_Apu* apu;

	Blip_Synth<blip_med_quality,1> synth;

>>>>>>> db7344ebf (abc)
	void start();
	void write_register( int, int );
	void run( nes_time_t, nes_time_t );
	void recalc_irq();
	void fill_buffer();
	void reload_sample();
	void reset();
	int count_reads( nes_time_t, nes_time_t* ) const;
	nes_time_t next_read_time() const;
};

#endif
