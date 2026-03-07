<<<<<<< HEAD
// Gb_Snd_Emu $vers. http://www.slack.net/~ant/

#include "Gb_Apu.h"

//#include "gb_apu_logger.h"

/* Copyright (C) 2003-2008 Shay Green. This module is free software; you
=======
// Gb_Snd_Emu 0.1.5. http://www.slack.net/~ant/

#include "Gb_Apu.h"

#include <string.h>
#include <algorithm>

/* Copyright (C) 2003-2006 Shay Green. This module is free software; you
>>>>>>> db7344ebf (abc)
can redistribute it and/or modify it under the terms of the GNU Lesser
General Public License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version. This
module is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details. You should have received a copy of the GNU Lesser General Public
License along with this module; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA */

#include "blargg_source.h"

<<<<<<< HEAD
int const vol_reg    = 0xFF24;
int const stereo_reg = 0xFF25;
int const status_reg = 0xFF26;
int const wave_ram   = 0xFF30;

int const power_mask = 0x80;

void Gb_Apu::treble_eq( blip_eq_t const& eq )
{
	norm_synth.treble_eq( eq );
	fast_synth.treble_eq( eq );
}

inline int Gb_Apu::calc_output( int osc ) const
{
	int bits = regs [stereo_reg - io_addr] >> osc;
	return (bits >> 3 & 2) | (bits & 1);
}

void Gb_Apu::set_output( int i, Blip_Buffer* center, Blip_Buffer* left, Blip_Buffer* right )
{
	// Must be silent (all NULL), mono (left and right NULL), or stereo (none NULL)
	require( !center || (center && !left && !right) || (center && left && right) );
	require( (unsigned) i < osc_count ); // fails if you pass invalid osc index
	
	if ( !center || !left || !right )
	{
		left  = center;
		right = center;
	}
	
	Gb_Osc& o = *oscs [i];
	o.outputs [1] = right;
	o.outputs [2] = left;
	o.outputs [3] = center;
	o.output = o.outputs [calc_output( i )];
}

void Gb_Apu::synth_volume( int iv )
{
	double v = volume_ * 0.60 / osc_count / 15 /*steps*/ / 8 /*master vol range*/ * iv;
	norm_synth.volume( v );
	fast_synth.volume( v );
}

void Gb_Apu::apply_volume()
{
	// TODO: Doesn't handle differing left and right volumes (panning).
	// Not worth the complexity.
	int data  = regs [vol_reg - io_addr];
	int left  = data >> 4 & 7;
	int right = data & 7;
	//if ( data & 0x88 ) dprintf( "Vin: %02X\n", data & 0x88 );
	//if ( left != right ) dprintf( "l: %d r: %d\n", left, right );
	synth_volume( max( left, right ) + 1 );
}

void Gb_Apu::volume( double v )
{
	if ( volume_ != v )
	{
		volume_ = v;
		apply_volume();
	}
}

void Gb_Apu::reset_regs()
{
	for ( int i = 0; i < 0x20; i++ )
		regs [i] = 0;
	
	square1.reset();
	square2.reset();
	wave   .reset();
	noise  .reset();
	
	apply_volume();
}

void Gb_Apu::reset_lengths()
{
	square1.length_ctr = 64;
	square2.length_ctr = 64;
	wave   .length_ctr = 256;
	noise  .length_ctr = 64;
}

void Gb_Apu::reduce_clicks( bool reduce )
{
	reduce_clicks_ = reduce;
	
	// Click reduction makes DAC off generate same output as volume 0
	int dac_off_amp = 0;
	if ( reduce && wave.mode != mode_agb ) // AGB already eliminates clicks
		dac_off_amp = -Gb_Osc::dac_bias;
	
	for ( int i = 0; i < osc_count; i++ )
		oscs [i]->dac_off_amp = dac_off_amp;
	
	// AGB always eliminates clicks on wave channel using same method
	if ( wave.mode == mode_agb )
		wave.dac_off_amp = -Gb_Osc::dac_bias;
}

void Gb_Apu::reset( mode_t mode, bool agb_wave )
{
	// Hardware mode
	if ( agb_wave )
		mode = mode_agb; // using AGB wave features implies AGB hardware
	wave.agb_mask = agb_wave ? 0xFF : 0;
	for ( int i = 0; i < osc_count; i++ )
		oscs [i]->mode = mode;
	reduce_clicks( reduce_clicks_ );
	
	// Reset state
	frame_time  = 0;
	last_time   = 0;
	frame_phase = 0;
	
	reset_regs();
	reset_lengths();
	
	// Load initial wave RAM
	static byte const initial_wave [2] [16] = {
		{0x84,0x40,0x43,0xAA,0x2D,0x78,0x92,0x3C,0x60,0x59,0x59,0xB0,0x34,0xB8,0x2E,0xDA},
		{0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF},
	};
	for ( int b = 2; --b >= 0; )
	{
		// Init both banks (does nothing if not in AGB mode)
		// TODO: verify that this works
		write_register( 0, 0xFF1A, b * 0x40 );
		for ( unsigned i = 0; i < sizeof initial_wave [0]; i++ )
			write_register( 0, i + wave_ram, initial_wave [(mode != mode_dmg)] [i] );
	}
}

void Gb_Apu::set_tempo( double t )
{
	frame_period = 4194304 / 512; // 512 Hz
	if ( t != 1.0 )
		frame_period = t ? blip_time_t (frame_period / t) : blip_time_t(0);
}

Gb_Apu::Gb_Apu()
{
	wave.wave_ram = &regs [wave_ram - io_addr];
	
=======
static unsigned const vol_reg    = 0xFF24;
static unsigned const status_reg = 0xFF26;

using std::min;
using std::max;

Gb_Apu::Gb_Apu()
{
	memset(regs, 0, sizeof(regs));

	square1.synth = &square_synth;
	square2.synth = &square_synth;
	wave.synth  = &other_synth;
	noise.synth = &other_synth;

>>>>>>> db7344ebf (abc)
	oscs [0] = &square1;
	oscs [1] = &square2;
	oscs [2] = &wave;
	oscs [3] = &noise;
<<<<<<< HEAD
	
	for ( int i = osc_count; --i >= 0; )
	{
		Gb_Osc& o = *oscs [i];
		o.regs        = &regs [i * 5];
		o.output      = NULL;
		o.outputs [0] = NULL;
		o.outputs [1] = NULL;
		o.outputs [2] = NULL;
		o.outputs [3] = NULL;
		o.norm_synth  = &norm_synth;
		o.fast_synth  = &fast_synth;
	}
	
	reduce_clicks_ = false;
	set_tempo( 1.0 );
	volume_ = 1.0;
	reset();
}

void Gb_Apu::run_until_( blip_time_t end_time )
{
	if ( !frame_period )
		frame_time += end_time - last_time;
	
	while ( true )
	{
		// run oscillators
		blip_time_t time = end_time;
		if ( time > frame_time )
			time = frame_time;
		
		square1.run( last_time, time );
		square2.run( last_time, time );
		wave   .run( last_time, time );
		noise  .run( last_time, time );
		last_time = time;
		
		if ( time == end_time )
			break;
		
		// run frame sequencer
		assert( frame_period );
		frame_time += frame_period * Gb_Osc::clk_mul;
		switch ( frame_phase++ )
		{
		case 2:
		case 6:
			// 128 Hz
			square1.clock_sweep();
		case 0:
		case 4:
			// 256 Hz
			square1.clock_length();
			square2.clock_length();
			wave   .clock_length();
			noise  .clock_length();
			break;
		
		case 7:
			// 64 Hz
			frame_phase = 0;
			square1.clock_envelope();
			square2.clock_envelope();
			noise  .clock_envelope();
		}
	}
}

inline void Gb_Apu::run_until( blip_time_t time )
{
	require( time >= last_time ); // end_time must not be before previous time
	if ( time > last_time )
		run_until_( time );
=======

	for ( int i = 0; i < osc_count; i++ )
	{
		Gb_Osc& osc = *oscs [i];
		osc.regs = &regs [i * 5];
		osc.output = 0;
		osc.outputs [0] = 0;
		osc.outputs [1] = 0;
		osc.outputs [2] = 0;
		osc.outputs [3] = 0;
	}

	set_tempo( 1.0 );
	volume( 1.0 );
	reset();
}

void Gb_Apu::treble_eq( const blip_eq_t& eq )
{
	square_synth.treble_eq( eq );
	other_synth.treble_eq( eq );
}

void Gb_Apu::osc_output( int index, Blip_Buffer* center, Blip_Buffer* left, Blip_Buffer* right )
{
	require( (unsigned) index < osc_count );
	require( (center && left && right) || (!center && !left && !right) );
	Gb_Osc& osc = *oscs [index];
	osc.outputs [1] = right;
	osc.outputs [2] = left;
	osc.outputs [3] = center;
	osc.output = osc.outputs [osc.output_select];
}

void Gb_Apu::output( Blip_Buffer* center, Blip_Buffer* left, Blip_Buffer* right )
{
	for ( int i = 0; i < osc_count; i++ )
		osc_output( i, center, left, right );
}

void Gb_Apu::update_volume()
{
	// TODO: doesn't handle differing left/right global volume (support would
	// require modification to all oscillator code)
	int data = regs [vol_reg - start_addr];
	double vol = (max( data & 7, data >> 4 & 7 ) + 1) * volume_unit;
	square_synth.volume( vol );
	other_synth.volume( vol );
}

static unsigned char const powerup_regs [0x20] = {
	0x80,0x3F,0x00,0xFF,0xBF, // square 1
	0xFF,0x3F,0x00,0xFF,0xBF, // square 2
	0x7F,0xFF,0x9F,0xFF,0xBF, // wave
	0xFF,0xFF,0x00,0x00,0xBF, // noise
	0x00, // left/right enables
	0x77, // master volume
	0x80, // power
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};

void Gb_Apu::set_tempo( double t )
{
	frame_period = 4194304 / 256; // 256 Hz
	if ( t != 1.0 )
		frame_period = blip_time_t (frame_period / t);
}

void Gb_Apu::reset()
{
	next_frame_time = 0;
	last_time       = 0;
	frame_count     = 0;

	square1.reset();
	square2.reset();
	wave.reset();
	noise.reset();
	noise.bits = 1;
	wave.wave_pos = 0;

	// avoid click at beginning
	regs [vol_reg - start_addr] = 0x77;
	update_volume();

	regs [status_reg - start_addr] = 0x01; // force power
	write_register( 0, status_reg, 0x00 );

	static unsigned char const initial_wave [] = {
		0x84,0x40,0x43,0xAA,0x2D,0x78,0x92,0x3C, // wave table
		0x60,0x59,0x59,0xB0,0x34,0xB8,0x2E,0xDA
	};
	memcpy( wave.wave, initial_wave, sizeof initial_wave );
}

void Gb_Apu::run_until( blip_time_t end_time )
{
	require( end_time >= last_time ); // end_time must not be before previous time
	if ( end_time == last_time )
		return;

	while ( true )
	{
		blip_time_t time = next_frame_time;
		if ( time > end_time )
			time = end_time;

		// run oscillators
		for ( int i = 0; i < osc_count; ++i )
		{
			Gb_Osc& osc = *oscs [i];
			if ( osc.output )
			{
				osc.output->set_modified(); // TODO: misses optimization opportunities?
				int playing = false;
				if ( osc.enabled && osc.volume &&
						(!(osc.regs [4] & osc.len_enabled_mask) || osc.length) )
					playing = -1;
				switch ( i )
				{
				case 0: square1.run( last_time, time, playing ); break;
				case 1: square2.run( last_time, time, playing ); break;
				case 2: wave   .run( last_time, time, playing ); break;
				case 3: noise  .run( last_time, time, playing ); break;
				}
			}
		}
		last_time = time;

		if ( time == end_time )
			break;

		next_frame_time += frame_period;

		// 256 Hz actions
		square1.clock_length();
		square2.clock_length();
		wave.clock_length();
		noise.clock_length();

		frame_count = (frame_count + 1) & 3;
		if ( frame_count == 0 )
		{
			// 64 Hz actions
			square1.clock_envelope();
			square2.clock_envelope();
			noise.clock_envelope();
		}

		if ( frame_count & 1 )
			square1.clock_sweep(); // 128 Hz action
	}
>>>>>>> db7344ebf (abc)
}

void Gb_Apu::end_frame( blip_time_t end_time )
{
<<<<<<< HEAD
	#ifdef LOG_FRAME
		LOG_FRAME( end_time );
	#endif
	
	if ( end_time > last_time )
		run_until( end_time );
	
	frame_time -= end_time;
	assert( frame_time >= 0 );
	
	last_time -= end_time;
	assert( last_time >= 0 );
}

void Gb_Apu::silence_osc( Gb_Osc& o )
{
	int delta = -o.last_amp;
	if ( reduce_clicks_ )
		delta += o.dac_off_amp;
	
	if ( delta )
	{
		o.last_amp = o.dac_off_amp;
		if ( o.output )
		{
			o.output->set_modified();
			fast_synth.offset( last_time, delta, o.output );
		}
	}
}

void Gb_Apu::apply_stereo()
{
	for ( int i = osc_count; --i >= 0; )
	{
		Gb_Osc& o = *oscs [i];
		Blip_Buffer* out = o.outputs [calc_output( i )];
		if ( o.output != out )
		{
			silence_osc( o );
			o.output = out;
		}
	}
}

void Gb_Apu::write_register( blip_time_t time, int addr, int data )
{
	require( (unsigned) data < 0x100 );
	
	int reg = addr - io_addr;
	if ( (unsigned) reg >= io_size )
	{
		require( false );
		return;
	}
	
	#ifdef LOG_WRITE
		LOG_WRITE( time, addr, data );
	#endif
	
	if ( addr < status_reg && !(regs [status_reg - io_addr] & power_mask) )
	{
		// Power is off
		
		// length counters can only be written in DMG mode
		if ( wave.mode != mode_dmg || (reg != 1 && reg != 5+1 && reg != 10+1 && reg != 15+1) )
			return;
		
		if ( reg < 10 )
			data &= 0x3F; // clear square duty
	}
	
	run_until( time );
	
	if ( addr >= wave_ram )
	{
		wave.write( addr, data );
	}
	else
	{
		int old_data = regs [reg];
		regs [reg] = data;
		
		if ( addr < vol_reg )
		{
			// Oscillator
			write_osc( reg, old_data, data );
		}
		else if ( addr == vol_reg && data != old_data )
		{
			// Master volume
			for ( int i = osc_count; --i >= 0; )
				silence_osc( *oscs [i] );
			
			apply_volume();
		}
		else if ( addr == stereo_reg )
		{
			// Stereo panning
			apply_stereo();
		}
		else if ( addr == status_reg && (data ^ old_data) & power_mask )
		{
			// Power control
			frame_phase = 0;
			for ( int i = osc_count; --i >= 0; )
				silence_osc( *oscs [i] );
		
			reset_regs();
			if ( wave.mode != mode_dmg )
				reset_lengths();
			
			regs [status_reg - io_addr] = data;
		}
	}
}

int Gb_Apu::read_register( blip_time_t time, int addr )
{
	if ( addr >= status_reg )
		run_until( time );
	
	int reg = addr - io_addr;
	if ( (unsigned) reg >= io_size )
	{
		require( false );
		return 0;
	}
	
	if ( addr >= wave_ram )
		return wave.read( addr );
	
	// Value read back has some bits always set
	static byte const masks [] = {
		0x80,0x3F,0x00,0xFF,0xBF,
		0xFF,0x3F,0x00,0xFF,0xBF,
		0x7F,0xFF,0x9F,0xFF,0xBF,
		0xFF,0xFF,0x00,0x00,0xBF,
		0x00,0x00,0x70,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
	};
	int mask = masks [reg];
	if ( wave.agb_mask && (reg == 10 || reg == 12) )
		mask = 0x1F; // extra implemented bits in wave regs on AGB
	int data = regs [reg] | mask;
	
	// Status register
	if ( addr == status_reg )
	{
		data &= 0xF0;
		data |= (int) square1.enabled << 0;
		data |= (int) square2.enabled << 1;
		data |= (int) wave   .enabled << 2;
		data |= (int) noise  .enabled << 3;
	}
	
=======
	if ( end_time > last_time )
		run_until( end_time );

	assert( next_frame_time >= end_time );
	next_frame_time -= end_time;

	assert( last_time >= end_time );
	last_time -= end_time;
}

void Gb_Apu::write_register( blip_time_t time, unsigned addr, int data )
{
	require( (unsigned) data < 0x100 );

	int reg = addr - start_addr;
	if ( (unsigned) reg >= register_count )
		return;

	run_until( time );

	int old_reg = regs [reg];
	regs [reg] = data;

	if ( addr < vol_reg )
	{
		write_osc( reg / 5, reg, data );
	}
	else if ( addr == vol_reg && data != old_reg ) // global volume
	{
		// return all oscs to 0
		for ( int i = 0; i < osc_count; i++ )
		{
			Gb_Osc& osc = *oscs [i];
			int amp = osc.last_amp;
			osc.last_amp = 0;
			if ( amp && osc.enabled && osc.output )
				other_synth.offset( time, -amp, osc.output );
		}

		if ( wave.outputs [3] )
			other_synth.offset( time, 30, wave.outputs [3] );

		update_volume();

		if ( wave.outputs [3] )
			other_synth.offset( time, -30, wave.outputs [3] );

		// oscs will update with new amplitude when next run
	}
	else if ( addr == 0xFF25 || addr == status_reg )
	{
		int mask = (regs [status_reg - start_addr] & 0x80) ? ~0 : 0;
		int flags = regs [0xFF25 - start_addr] & mask;

		// left/right assignments
		for ( int i = 0; i < osc_count; i++ )
		{
			Gb_Osc& osc = *oscs [i];
			osc.enabled &= mask;
			int bits = flags >> i;
			Blip_Buffer* old_output = osc.output;
			osc.output_select = (bits >> 3 & 2) | (bits & 1);
			osc.output = osc.outputs [osc.output_select];
			if ( osc.output != old_output )
			{
				int amp = osc.last_amp;
				osc.last_amp = 0;
				if ( amp && old_output )
					other_synth.offset( time, -amp, old_output );
			}
		}

		if ( addr == status_reg && data != old_reg )
		{
			if ( !(data & 0x80) )
			{
				for ( unsigned i = 0; i < sizeof powerup_regs; i++ )
				{
					if ( i != status_reg - start_addr )
						write_register( time, i + start_addr, powerup_regs [i] );
				}
			}
			else
			{
				//debug_printf( "APU powered on\n" );
			}
		}
	}
	else if ( addr >= 0xFF30 )
	{
		int index = (addr & 0x0F) * 2;
		wave.wave [index] = data >> 4;
		wave.wave [index + 1] = data & 0x0F;
	}
}

int Gb_Apu::read_register( blip_time_t time, unsigned addr )
{
	run_until( time );

	int index = addr - start_addr;
	require( (unsigned) index < register_count );
	int data = regs [index];

	if ( addr == status_reg )
	{
		data = (data & 0x80) | 0x70;
		for ( int i = 0; i < osc_count; i++ )
		{
			const Gb_Osc& osc = *oscs [i];
			if ( osc.enabled && (osc.length || !(osc.regs [4] & osc.len_enabled_mask)) )
				data |= 1 << i;
		}
	}

>>>>>>> db7344ebf (abc)
	return data;
}
