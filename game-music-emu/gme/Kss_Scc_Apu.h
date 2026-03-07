// Konami SCC sound chip emulator

<<<<<<< HEAD
// $package
=======
// Game_Music_Emu https://bitbucket.org/mpyne/game-music-emu/
>>>>>>> db7344ebf (abc)
#ifndef KSS_SCC_APU_H
#define KSS_SCC_APU_H

#include "blargg_common.h"
#include "Blip_Buffer.h"
<<<<<<< HEAD

class Scc_Apu {
public:
// Basics

	// Sets buffer to generate sound into, or 0 to mute.
	void set_output( Blip_Buffer* );

	// Emulates to time t, then writes data to reg
	enum { reg_count = 0xB0 }; // 0 <= reg < reg_count
	void write( blip_time_t t, int reg, int data );

	// Emulates to time t, then subtracts t from the current time.
	// OK if previous write call had time slightly after t.
	void end_frame( blip_time_t t );

// More features

	// Resets sound chip
	void reset();

	// Same as set_output(), but for a particular channel
	enum { osc_count = 5 };
	void set_output( int chan, Blip_Buffer* );

	// Set overall volume, where 1.0 is normal
	void volume( double );

	// Set treble equalization
	void treble_eq( blip_eq_t const& eq )   { synth.treble_eq( eq ); }

private:
	// noncopyable
	Scc_Apu( const Scc_Apu& );
	Scc_Apu& operator = ( const Scc_Apu& );


// Implementation
public:
	Scc_Apu();
	BLARGG_DISABLE_NOTHROW

private:
	enum { amp_range = 0x8000 };
=======
#include <string.h>

class Scc_Apu {
public:
	// Set buffer to generate all sound into, or disable sound if NULL
	void output( Blip_Buffer* );

	// Reset sound chip
	void reset();

	// Write to register at specified time
	static const int reg_count = 0x90;
	void write( blip_time_t time, int reg, int data );

	// Run sound to specified time, end current time frame, then start a new
	// time frame at time 0. Time frames have no effect on emulation and each
	// can be whatever length is convenient.
	void end_frame( blip_time_t length );

// Additional features

	// Set sound output of specific oscillator to buffer, where index is
	// 0 to 4. If buffer is NULL, the specified oscillator is muted.
	static const int osc_count = 5;
	void osc_output( int index, Blip_Buffer* );

	// Set overall volume (default is 1.0)
	void volume( double );

	// Set treble equalization (see documentation)
	void treble_eq( blip_eq_t const& );

public:
	Scc_Apu();
private:
	static const unsigned int amp_range = 0x8000;
>>>>>>> db7344ebf (abc)
	struct osc_t
	{
		int delay;
		int phase;
		int last_amp;
		Blip_Buffer* output;
	};
	osc_t oscs [osc_count];
	blip_time_t last_time;
	unsigned char regs [reg_count];
<<<<<<< HEAD
	Blip_Synth_Fast synth;
=======
	Blip_Synth<blip_med_quality,1> synth;
>>>>>>> db7344ebf (abc)

	void run_until( blip_time_t );
};

<<<<<<< HEAD
inline void Scc_Apu::set_output( int index, Blip_Buffer* b )
=======
inline void Scc_Apu::volume( double v ) { synth.volume( 0.43 / osc_count / amp_range * v ); }

inline void Scc_Apu::treble_eq( blip_eq_t const& eq ) { synth.treble_eq( eq ); }

inline void Scc_Apu::osc_output( int index, Blip_Buffer* b )
>>>>>>> db7344ebf (abc)
{
	assert( (unsigned) index < osc_count );
	oscs [index].output = b;
}

inline void Scc_Apu::write( blip_time_t time, int addr, int data )
{
<<<<<<< HEAD
	//assert( (unsigned) addr < reg_count );
	assert( ( addr >= 0x9800 && addr <= 0x988F ) || ( addr >= 0xB800 && addr <= 0xB8AF ) );
	run_until( time );

	addr -= 0x9800;
	if ( ( unsigned ) addr < 0x90 )
	{
	    if ( ( unsigned ) addr < 0x60 )
            regs [addr] = data;
        else if ( ( unsigned ) addr < 0x80 )
        {
            regs [addr] = regs[addr + 0x20] = data;
        }
        else if ( ( unsigned ) addr < 0x90 )
        {
            regs [addr + 0x20] = data;
        }
	}
	else
	{
	    addr -= 0xB800 - 0x9800;
	    if ( ( unsigned ) addr < 0xB0 )
            regs [addr] = data;
	}
=======
	assert( (unsigned) addr < reg_count );
	run_until( time );
	regs [addr] = data;
>>>>>>> db7344ebf (abc)
}

inline void Scc_Apu::end_frame( blip_time_t end_time )
{
	if ( end_time > last_time )
		run_until( end_time );
<<<<<<< HEAD

=======
>>>>>>> db7344ebf (abc)
	last_time -= end_time;
	assert( last_time >= 0 );
}

<<<<<<< HEAD
=======
inline void Scc_Apu::output( Blip_Buffer* buf )
{
	for ( int i = 0; i < osc_count; i++ )
		oscs [i].output = buf;
}

inline Scc_Apu::Scc_Apu()
{
	output( 0 );
}

inline void Scc_Apu::reset()
{
	last_time = 0;

	for ( int i = 0; i < osc_count; i++ )
		memset( &oscs [i], 0, offsetof (osc_t,output) );

	memset( regs, 0, sizeof regs );
}

>>>>>>> db7344ebf (abc)
#endif
