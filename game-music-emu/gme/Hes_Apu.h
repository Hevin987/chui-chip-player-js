// Turbo Grafx 16 (PC Engine) PSG sound chip emulator

<<<<<<< HEAD
// Game_Music_Emu $vers
=======
// Game_Music_Emu https://bitbucket.org/mpyne/game-music-emu/
>>>>>>> db7344ebf (abc)
#ifndef HES_APU_H
#define HES_APU_H

#include "blargg_common.h"
#include "Blip_Buffer.h"

<<<<<<< HEAD
class Hes_Apu {
public:
// Basics

	// Sets buffer(s) to generate sound into, or 0 to mute. If only center is not 0,
	// output is mono.
	void set_output( Blip_Buffer* center, Blip_Buffer* left = NULL, Blip_Buffer* right = NULL );

	// Emulates to time t, then writes data to addr
	void write_data( blip_time_t t, int addr, int data );
	
	// Emulates to time t, then subtracts t from the current time.
	// OK if previous write call had time slightly after t.
	void end_frame( blip_time_t t );
	
// More features
	
	// Resets sound chip
	void reset();
	
	// Same as set_output(), but for a particular channel
	enum { osc_count = 6 }; // 0 <= chan < osc_count
	void set_output( int chan, Blip_Buffer* center, Blip_Buffer* left = NULL, Blip_Buffer* right = NULL );
	
	// Sets treble equalization
	void treble_eq( blip_eq_t const& eq )   { synth.treble_eq( eq ); }
	
	// Sets overall volume, where 1.0 is normal
	void volume( double v )                 { synth.volume( 1.8 / osc_count / amp_range * v ); }
	
	// Registers are at io_addr to io_addr+io_size-1
	enum { io_addr = 0x0800 };
	enum { io_size = 10 };
	
// Implementation
public:
	Hes_Apu();
	typedef BOOST::uint8_t byte;

private:
	enum { amp_range = 0x8000 };
	struct Osc
	{
		byte wave [32];
		int  delay;
		int  period;
		int  phase;
		
		int  noise_delay;
		byte noise;
		unsigned lfsr;
		
		byte control;
		byte balance;
		byte dac;
		short volume [2];
		int last_amp [2];
		
		blip_time_t last_time;
		Blip_Buffer* output [2];
		Blip_Buffer* outputs [3];
	};
	Osc oscs [osc_count];
	int latch;
	int balance;
	Blip_Synth_Fast synth;
	
	void balance_changed( Osc& );
	static void run_osc( Blip_Synth_Fast&, Osc&, blip_time_t );
};

inline void Hes_Apu::set_output( Blip_Buffer* c, Blip_Buffer* l, Blip_Buffer* r )
{
	for ( int i = osc_count; --i >= 0; )
		set_output( i, c, l, r );
}
=======
struct Hes_Osc
{
	unsigned char wave [32];
	short volume [2];
	int last_amp [2];
	int delay;
	int period;
	unsigned char noise;
	unsigned char phase;
	unsigned char balance;
	unsigned char dac;
	blip_time_t last_time;

	Blip_Buffer* outputs [2];
	Blip_Buffer* chans [3];
	unsigned noise_lfsr;
	unsigned char control;

	static const unsigned int amp_range = 0x8000;
	typedef Blip_Synth<blip_med_quality,1> synth_t;

	void run_until( synth_t& synth, blip_time_t );
};

class Hes_Apu {
public:
	void treble_eq( blip_eq_t const& );
	void volume( double );

	static const int osc_count = 6;
	void osc_output( int index, Blip_Buffer* center, Blip_Buffer* left, Blip_Buffer* right );

	void reset();

	static const unsigned int start_addr = 0x0800;
	static const unsigned int end_addr   = 0x0809;
	void write_data( blip_time_t, int addr, int data );

	void end_frame( blip_time_t );

public:
	Hes_Apu();
private:
	Hes_Osc oscs [osc_count];
	int latch;
	int balance;
	Hes_Osc::synth_t synth;

	void balance_changed( Hes_Osc& );
	void recalc_chans();
};

inline void Hes_Apu::volume( double v ) { synth.volume( 1.8 / osc_count / Hes_Osc::amp_range * v ); }

inline void Hes_Apu::treble_eq( blip_eq_t const& eq ) { synth.treble_eq( eq ); }
>>>>>>> db7344ebf (abc)

#endif
