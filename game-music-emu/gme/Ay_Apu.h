// AY-3-8910 sound chip emulator

<<<<<<< HEAD
// $package
=======
// Game_Music_Emu https://bitbucket.org/mpyne/game-music-emu/
>>>>>>> db7344ebf (abc)
#ifndef AY_APU_H
#define AY_APU_H

#include "blargg_common.h"
#include "Blip_Buffer.h"

class Ay_Apu {
public:
<<<<<<< HEAD
// Basics
	enum Ay_Apu_Type
	{
		Ay8910 = 0,
		Ay8912,
		Ay8913,
		Ay8914,
		Ym2149 = 0x10,
		Ym3439,
		Ymz284,
		Ymz294,
		Ym2203 = 0x20,
		Ym2608,
		Ym2610,
		Ym2610b
	};

	void set_type( Ay_Apu_Type type ) { type_ = type; }

	// Sets buffer to generate sound into, or 0 to mute.
	void set_output( Blip_Buffer* );
	
	// Writes to address register
	void write_addr( int data )                 { addr_ = data & 0x0F; }
	
	// Emulates to time t, then writes to current data register
	void write_data( blip_time_t t, int data )  { run_until( t ); write_data_( addr_, data ); }
	
	// Emulates to time t, then subtracts t from the current time.
	// OK if previous write call had time slightly after t.
	void end_frame( blip_time_t t );
	
// More features
	
	// Reads from current data register
	int read();
	
	// Resets sound chip
	void reset();
	
	// Number of registers
	enum { reg_count = 16 };
	
	// Same as set_output(), but for a particular channel
	enum { osc_count = 3 };
	void set_output( int chan, Blip_Buffer* );
	
	// Sets overall volume, where 1.0 is normal
	void volume( double v )                     { synth_.volume( 0.7/osc_count/amp_range * v ); }
	
	// Sets treble equalization
	void treble_eq( blip_eq_t const& eq )       { synth_.treble_eq( eq ); }
	
private:
	// noncopyable
	Ay_Apu( const Ay_Apu& );
	Ay_Apu& operator = ( const Ay_Apu& );

// Implementation
public:
	Ay_Apu();
	BLARGG_DISABLE_NOTHROW
	typedef BOOST::uint8_t byte;

private:
	struct osc_t
	{
		blip_time_t  period;
		blip_time_t  delay;
		short        last_amp;
		short        phase;
		Blip_Buffer* output;
	} oscs [osc_count];

	Ay_Apu_Type type_;

	blip_time_t last_time;
	byte        addr_;
	byte        regs [reg_count];
	
	blip_time_t noise_delay;
	unsigned    noise_lfsr;
	
	blip_time_t env_delay;
	byte const* env_wave;
	int         env_pos;
	byte        env_modes [8] [48]; // values already passed through volume table
	
	void write_data_( int addr, int data );
	void run_until( blip_time_t );
	
public:
	enum { amp_range = 255 };
	Blip_Synth_Norm synth_; // used by Ay_Core for beeper sound
};

inline void Ay_Apu::set_output( int i, Blip_Buffer* out )
{
	assert( (unsigned) i < osc_count );
	oscs [i].output = out;
=======
	// Set buffer to generate all sound into, or disable sound if NULL
	void output( Blip_Buffer* );

	// Reset sound chip
	void reset();

	// Write to register at specified time
	static const unsigned int reg_count = 16;
	void write( blip_time_t time, int addr, int data );

	// Run sound to specified time, end current time frame, then start a new
	// time frame at time 0. Time frames have no effect on emulation and each
	// can be whatever length is convenient.
	void end_frame( blip_time_t length );

// Additional features

	// Set sound output of specific oscillator to buffer, where index is
	// 0, 1, or 2. If buffer is NULL, the specified oscillator is muted.
	static const int osc_count = 3;
	void osc_output( int index, Blip_Buffer* );

	// Set overall volume (default is 1.0)
	void volume( double );

	// Set treble equalization (see documentation)
	void treble_eq( blip_eq_t const& );

public:
	Ay_Apu();
	typedef unsigned char byte;
private:
	struct osc_t
	{
		blip_time_t period;
		blip_time_t delay;
		short last_amp;
		short phase;
		Blip_Buffer* output;
	} oscs [osc_count];
	blip_time_t last_time;
	byte regs [reg_count];

	struct {
		blip_time_t delay;
		uint32_t lfsr;
	} noise;

	struct {
		blip_time_t delay;
		byte const* wave;
		int pos;
		byte modes [8] [48]; // values already passed through volume table
	} env;

	void run_until( blip_time_t );
	void write_data_( int addr, int data );
public:
	static const int amp_range = 255;
	Blip_Synth<blip_good_quality,1> synth_;
};

inline void Ay_Apu::volume( double v ) { synth_.volume( 0.7 / osc_count / amp_range * v ); }

inline void Ay_Apu::treble_eq( blip_eq_t const& eq ) { synth_.treble_eq( eq ); }

inline void Ay_Apu::write( blip_time_t time, int addr, int data )
{
	run_until( time );
	write_data_( addr, data );
}

inline void Ay_Apu::osc_output( int i, Blip_Buffer* buf )
{
	assert( (unsigned) i < osc_count );
	oscs [i].output = buf;
}

inline void Ay_Apu::output( Blip_Buffer* buf )
{
	osc_output( 0, buf );
	osc_output( 1, buf );
	osc_output( 2, buf );
>>>>>>> db7344ebf (abc)
}

inline void Ay_Apu::end_frame( blip_time_t time )
{
	if ( time > last_time )
		run_until( time );
<<<<<<< HEAD
	
	last_time -= time;
	assert( last_time >= 0 );
=======

	assert( last_time >= time );
	last_time -= time;
>>>>>>> db7344ebf (abc)
}

#endif
