<<<<<<< HEAD
// Game_Music_Emu $vers. http://www.slack.net/~ant/

#include "Effects_Buffer.h"

/* Copyright (C) 2006-2007 Shay Green. This module is free software; you
=======
// Game_Music_Emu https://bitbucket.org/mpyne/game-music-emu/

#include "Effects_Buffer.h"

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

#ifdef BLARGG_ENABLE_OPTIMIZER
	#include BLARGG_ENABLE_OPTIMIZER
#endif

<<<<<<< HEAD
int const fixed_shift = 12;
#define TO_FIXED( f )   fixed_t ((f) * ((fixed_t) 1 << fixed_shift))
#define FROM_FIXED( f ) ((f) >> fixed_shift)

int const max_read = 2560; // determines minimum delay

Effects_Buffer::Effects_Buffer( int max_bufs, int echo_size_ ) : Multi_Buffer( stereo )
{
	echo_size   = max( max_read * (int) stereo, echo_size_ & ~1 );
	clock_rate_ = 0;
	bass_freq_  = 90;
	bufs        = NULL;
	bufs_size   = 0;
	bufs_max    = max( max_bufs, (int) extra_chans );
	no_echo     = true;
	no_effects  = true;
	
	// defaults
	config_.enabled   = false;
	config_.delay [0] = 120;
	config_.delay [1] = 122;
	config_.feedback  = 0.2f;
	config_.treble    = 0.4f;
	
	static float const sep = 0.8f;
	config_.side_chans [0].pan = -sep;
	config_.side_chans [1].pan = +sep;
	config_.side_chans [0].vol = 1.0f;
	config_.side_chans [1].vol = 1.0f;

	memset( &s, 0, sizeof s );
	clear();
}

Effects_Buffer::~Effects_Buffer()
{
	delete_bufs();
}

// avoid using new []
blargg_err_t Effects_Buffer::new_bufs( int size )
{
	bufs = (buf_t*) malloc( size * sizeof *bufs );
	CHECK_ALLOC( bufs );
	for ( int i = 0; i < size; i++ )
		new (bufs + i) buf_t;
	bufs_size = size;
	return blargg_ok;
}

void Effects_Buffer::delete_bufs()
{
	if ( bufs )
	{
		for ( int i = bufs_size; --i >= 0; )
			bufs [i].~buf_t();
		free( bufs );
		bufs = NULL;
	}
	bufs_size = 0;
}

blargg_err_t Effects_Buffer::set_sample_rate( int rate, int msec )
{
	// extra to allow farther past-the-end pointers
	mixer.samples_read = 0;
	RETURN_ERR( echo.resize( echo_size + stereo ) );
	return Multi_Buffer::set_sample_rate( rate, msec );
}

void Effects_Buffer::clock_rate( int rate )
{
	clock_rate_ = rate;
	for ( int i = bufs_size; --i >= 0; )
		bufs [i].clock_rate( clock_rate_ );
=======
typedef int32_t fixed_t;

using std::min;
using std::max;

#define TO_FIXED( f )   fixed_t ((f) * (1L << 15) + 0.5)
#define FMUL( x, y )    (((x) * (y)) >> 15)

static const unsigned echo_size = 4096;
static const unsigned echo_mask = echo_size - 1;
static_assert( (echo_size & echo_mask) == 0, "echo_size must be a power of 2" );

static const unsigned reverb_size = 8192 * 2;
static const unsigned reverb_mask = reverb_size - 1;
static_assert( (reverb_size & reverb_mask) == 0, "reverb_size must be a power of 2" );

Effects_Buffer::config_t::config_t()
{
	pan_1           = -0.15f;
	pan_2           =  0.15f;
	reverb_delay    = 88.0f;
	reverb_level    = 0.12f;
	echo_delay      = 61.0f;
	echo_level      = 0.10f;
	delay_variance  = 18.0f;
	effects_enabled = false;
}

void Effects_Buffer::set_depth( double d )
{
	float f = (float) d;
	config_t c;
	c.pan_1             = -0.6f * f;
	c.pan_2             =  0.6f * f;
	c.reverb_delay      = 880 * 0.1f;
	c.echo_delay        = 610 * 0.1f;
	if ( f > 0.5 )
		f = 0.5; // TODO: more linear reduction of extreme reverb/echo
	c.reverb_level      = 0.5f * f;
	c.echo_level        = 0.30f * f;
	c.delay_variance    = 180 * 0.1f;
	c.effects_enabled   = (d > 0.0f);
	config( c );
}

Effects_Buffer::Effects_Buffer( int num_voices, bool center_only )
	: Multi_Buffer( 2*num_voices )
	, max_voices(num_voices)
	, bufs(max_voices * (center_only ? (max_buf_count - 4) : max_buf_count))
	, chan_types(max_voices * chan_types_count)
	, stereo_remain(0)
	, effect_remain(0)
	// TODO: Reorder buf_count to be initialized before bufs to factor out channel sizing
	, buf_count(max_voices * (center_only ? (max_buf_count - 4) : max_buf_count))
	, effects_enabled(false)
	, reverb_buf(max_voices, std::vector<blip_sample_t>(reverb_size))
	, echo_buf(max_voices, std::vector<blip_sample_t>(echo_size))
	, reverb_pos(max_voices)
	, echo_pos(max_voices)
{
	set_depth( 0 );
}

Effects_Buffer::~Effects_Buffer()
{}

blargg_err_t Effects_Buffer::set_sample_rate( long rate, int msec ) noexcept
{
	for(int i=0; i<max_voices; i++)
	{
		if ( !echo_buf[i].size() )
		{
			echo_buf[i].resize( echo_size );
		}

		if ( !reverb_buf[i].size() )
		{
			reverb_buf[i].resize( reverb_size );
		}

		if ( !echo_buf[i].size() || !reverb_buf[i].size() )
		{
			return "Out of memory";
		}
	}

	for ( int i = 0; i < buf_count; i++ )
		RETURN_ERR( bufs [i].set_sample_rate( rate, msec ) );

	config( config_ );
	clear();

	return Multi_Buffer::set_sample_rate( bufs [0].sample_rate(), bufs [0].length() );
}

void Effects_Buffer::clock_rate( uint32_t rate )
{
	for ( int i = 0; i < buf_count; i++ )
		bufs [i].clock_rate( rate );
>>>>>>> db7344ebf (abc)
}

void Effects_Buffer::bass_freq( int freq )
{
<<<<<<< HEAD
	bass_freq_ = freq;
	for ( int i = bufs_size; --i >= 0; )
		bufs [i].bass_freq( bass_freq_ );
}

blargg_err_t Effects_Buffer::set_channel_count( int count, int const types [] )
{
	RETURN_ERR( Multi_Buffer::set_channel_count( count, types ) );
	
	delete_bufs();
	
	mixer.samples_read = 0;
	
	RETURN_ERR( chans.resize( count + extra_chans ) );
	
	RETURN_ERR( new_bufs( min( bufs_max, count + extra_chans ) ) );
	
	for ( int i = bufs_size; --i >= 0; )
		RETURN_ERR( bufs [i].set_sample_rate( sample_rate(), length() ) );
	
	for ( int i = chans.size(); --i >= 0; )
	{
		chan_t& ch = chans [i];
		ch.cfg.vol      = 1.0f;
		ch.cfg.pan      = 0.0f;
		ch.cfg.surround = false;
		ch.cfg.echo     = false;
	}
	// side channels with echo
	chans [2].cfg.echo = true;
	chans [3].cfg.echo = true;
	
	clock_rate( clock_rate_ );
	bass_freq( bass_freq_ );
	apply_config();
	clear();
	
	return blargg_ok;
}

void Effects_Buffer::clear_echo()
{
	if ( echo.size() )
		memset( echo.begin(), 0, echo.size() * sizeof echo [0] );
=======
	for ( int i = 0; i < buf_count; i++ )
		bufs [i].bass_freq( freq );
>>>>>>> db7344ebf (abc)
}

void Effects_Buffer::clear()
{
<<<<<<< HEAD
	echo_pos       = 0;
	s.low_pass [0] = 0;
	s.low_pass [1] = 0;
	mixer.samples_read = 0;
	
	for ( int i = bufs_size; --i >= 0; )
		bufs [i].clear();
	clear_echo();
}

Effects_Buffer::channel_t Effects_Buffer::channel( int i )
{
	i += extra_chans;
	require( extra_chans <= i && i < (int) chans.size() );
	return chans [i].channel;
}


// Configuration

// 3 wave positions with/without surround, 2 multi (one with same config as wave)
int const simple_bufs = 3 * 2 + 2 - 1;

Simple_Effects_Buffer::Simple_Effects_Buffer() :
	Effects_Buffer( extra_chans + simple_bufs, 18 * 1024 )
{
	config_.echo     = 0.20f;
	config_.stereo   = 0.20f;
	config_.surround = true;
	config_.enabled  = false;
}

void Simple_Effects_Buffer::apply_config()
{
	Effects_Buffer::config_t& c = Effects_Buffer::config();
	
	c.enabled = config_.enabled;
	if ( c.enabled )
	{
		c.delay [0] = 120;
		c.delay [1] = 122;
		c.feedback  = config_.echo * 0.7f;
		c.treble    = 0.6f - 0.3f * config_.echo;
		
		float sep = config_.stereo + 0.80f;
		if ( sep > 1.0f )
			sep = 1.0f;
		
		c.side_chans [0].pan = -sep;
		c.side_chans [1].pan = +sep;
		
		for ( int i = channel_count(); --i >= 0; )
		{
			chan_config_t& ch = Effects_Buffer::chan_config( i );
			
			ch.pan      = 0.0f;
			ch.surround = config_.surround;
			ch.echo     = false;
			
			int const type = (channel_types() ? channel_types() [i] : 0);
			if ( !(type & noise_type) )
			{
				int index = (type & type_index_mask) % 6 - 3;
				if ( index < 0 )
				{
					index += 3;
					ch.surround = false;
					ch.echo     = true;
				}
				if ( index >= 1 )
				{
					ch.pan = config_.stereo;
					if ( index == 1 )
						ch.pan = -ch.pan;
				}
			}
			else if ( type & 1 )
			{
				ch.surround = false;
			}
		}
	}
	
	Effects_Buffer::apply_config();
}

int Effects_Buffer::min_delay() const
{
	require( sample_rate() );
	return max_read * 1000 / sample_rate();
}

int Effects_Buffer::max_delay() const
{
	require( sample_rate() );
	return (echo_size / stereo - max_read) * 1000 / sample_rate();
}

void Effects_Buffer::apply_config()
{
	int i;
	
	if ( !bufs_size )
		return;
	
	s.treble = TO_FIXED( config_.treble );
	
	bool echo_dirty = false;
	
	fixed_t old_feedback = s.feedback;
	s.feedback = TO_FIXED( config_.feedback );
	if ( !old_feedback && s.feedback )
		echo_dirty = true;
	
	// delays
	for ( i = stereo; --i >= 0; )
	{
		int delay = config_.delay [i] * sample_rate() / 1000 * stereo;
		delay = max( delay, (int) (max_read * stereo) );
		delay = min( delay, (int) (echo_size - max_read * stereo) );
		if ( s.delay [i] != delay )
		{
			s.delay [i] = delay;
			echo_dirty = true;
		}
	}
	
	// side channels
	for ( i = 2; --i >= 0; )
	{
		chans [i+2].cfg.vol = chans [i].cfg.vol = config_.side_chans [i].vol * 0.5f;
		chans [i+2].cfg.pan = chans [i].cfg.pan = config_.side_chans [i].pan;
	}
	
	// convert volumes
	for ( i = chans.size(); --i >= 0; )
	{
		chan_t& ch = chans [i];
		ch.vol [0] = TO_FIXED( ch.cfg.vol - ch.cfg.vol * ch.cfg.pan );
		ch.vol [1] = TO_FIXED( ch.cfg.vol + ch.cfg.vol * ch.cfg.pan );
		if ( ch.cfg.surround )
			ch.vol [0] = -ch.vol [0];
	}
	
	assign_buffers();
	
	// set side channels
	for ( i = chans.size(); --i >= 0; )
	{
		chan_t& ch = chans [i];
		ch.channel.left  = chans [ch.cfg.echo*2  ].channel.center;
		ch.channel.right = chans [ch.cfg.echo*2+1].channel.center;
	}
	
	bool old_echo = !no_echo && !no_effects;
	
	// determine whether effects and echo are needed at all
	no_effects = true;
	no_echo    = true;
	for ( i = chans.size(); --i >= extra_chans; )
	{
		chan_t& ch = chans [i];
		if ( ch.cfg.echo && s.feedback )
			no_echo = false;
		
		if ( ch.vol [0] != TO_FIXED( 1 ) || ch.vol [1] != TO_FIXED( 1 ) )
			no_effects = false;
	}
	if ( !no_echo )
		no_effects = false;
	
	if (    chans [0].vol [0] != TO_FIXED( 1 ) ||
			chans [0].vol [1] != TO_FIXED( 0 ) ||
			chans [1].vol [0] != TO_FIXED( 0 ) ||
			chans [1].vol [1] != TO_FIXED( 1 ) )
		no_effects = false;
	
	if ( !config_.enabled )
		no_effects = true;
	
	if ( no_effects )
	{
		for ( i = chans.size(); --i >= 0; )
		{
			chan_t& ch = chans [i];
			ch.channel.center = &bufs [2];
			ch.channel.left   = &bufs [0];
			ch.channel.right  = &bufs [1];
		}
	}
	
	mixer.bufs [0] = &bufs [0];
	mixer.bufs [1] = &bufs [1];
	mixer.bufs [2] = &bufs [2];
	
	if ( echo_dirty || (!old_echo && (!no_echo && !no_effects)) )
		clear_echo();
	
	channels_changed();
}

void Effects_Buffer::assign_buffers()
{
	// assign channels to buffers
	int buf_count = 0;
	for ( int i = 0; i < (int) chans.size(); i++ )
	{
		// put second two side channels at end to give priority to main channels
		// in case closest matching is necessary
		int x = i;
		if ( i > 1 )
			x += 2;
		if ( x >= (int) chans.size() )
			x -= (chans.size() - 2);
		chan_t& ch = chans [x];
		
		int b = 0;
		for ( ; b < buf_count; b++ )
		{
			if (    ch.vol [0] == bufs [b].vol [0] &&
					ch.vol [1] == bufs [b].vol [1] &&
					(ch.cfg.echo == bufs [b].echo || !s.feedback) )
				break;
		}
		
		if ( b >= buf_count )
		{
			if ( buf_count < bufs_max )
			{
				bufs [b].vol [0] = ch.vol [0];
				bufs [b].vol [1] = ch.vol [1];
				bufs [b].echo    = ch.cfg.echo;
				buf_count++;
			}
			else
			{
				// TODO: this is a mess, needs refinement
				dprintf( "Effects_Buffer ran out of buffers; using closest match\n" );
				b = 0;
				fixed_t best_dist = TO_FIXED( 8 );
				for ( int h = buf_count; --h >= 0; )
				{
					#define CALC_LEVELS( vols, sum, diff, surround ) \
					fixed_t sum, diff;\
					bool surround = false;\
					{\
						fixed_t vol_0 = vols [0];\
						if ( vol_0 < 0 ) vol_0 = -vol_0, surround = true;\
						fixed_t vol_1 = vols [1];\
						if ( vol_1 < 0 ) vol_1 = -vol_1, surround = true;\
						sum  = vol_0 + vol_1;\
						diff = vol_0 - vol_1;\
					}
					CALC_LEVELS( ch.vol,       ch_sum,  ch_diff,  ch_surround );
					CALC_LEVELS( bufs [h].vol, buf_sum, buf_diff, buf_surround );
					
					fixed_t dist = abs( ch_sum - buf_sum ) + abs( ch_diff - buf_diff );
					
					if ( ch_surround != buf_surround )
						dist += TO_FIXED( 1 ) / 2;
					
					if ( s.feedback && ch.cfg.echo != bufs [h].echo )
						dist += TO_FIXED( 1 ) / 2;
					
					if ( best_dist > dist )
					{
						best_dist = dist;
						b = h;
					}
				}
			}
		}
		
		//dprintf( "ch %d->buf %d\n", x, b );
		ch.channel.center = &bufs [b];
	}
}


// Mixing

void Effects_Buffer::end_frame( blip_time_t time )
{
	for ( int i = bufs_size; --i >= 0; )
		bufs [i].end_frame( time );
}

int Effects_Buffer::read_samples( blip_sample_t out [], int out_size )
{
	out_size = min( out_size, samples_avail() );
	
	int pair_count = int (out_size >> 1);
	require( pair_count * stereo == out_size ); // must read an even number of samples
	if ( pair_count )
	{
		if ( no_effects )
		{
			mixer.read_pairs( out, pair_count );
		}
		else
		{
			int pairs_remain = pair_count;
			do
			{
				// mix at most max_read pairs at a time
				int count = max_read;
				if ( count > pairs_remain )
					count = pairs_remain;
				
				if ( no_echo )
				{
					// optimization: clear echo here to keep mix_effects() a leaf function
					echo_pos = 0;
					memset( echo.begin(), 0, count * stereo * sizeof echo [0] );
				}
				mix_effects( out, count );
				
				int new_echo_pos = echo_pos + count * stereo;
				if ( new_echo_pos >= echo_size )
					new_echo_pos -= echo_size;
				echo_pos = new_echo_pos;
				assert( echo_pos < echo_size );
				
				out += count * stereo;
				mixer.samples_read += count;
				pairs_remain -= count;
			}
			while ( pairs_remain );
		}
		
		if ( samples_avail() <= 0 || immediate_removal() )
		{
			for ( int i = bufs_size; --i >= 0; )
			{
				buf_t& b = bufs [i];
				// TODO: might miss non-silence settling since it checks END of last read
				if ( b.non_silent() )
					b.remove_samples( mixer.samples_read );
				else
					b.remove_silence( mixer.samples_read );
			}
			mixer.samples_read = 0;
		}
	}
	return out_size;
}

void Effects_Buffer::mix_effects( blip_sample_t out_ [], int pair_count )
{
	typedef fixed_t stereo_fixed_t [stereo];
	
	// add channels with echo, do echo, add channels without echo, then convert to 16-bit and output
	int echo_phase = 1;
	do
	{
		// mix any modified buffers
		{
			buf_t* buf = bufs;
			int bufs_remain = bufs_size;
			do
			{
				if ( buf->non_silent() && buf->echo == echo_phase )
				{
					stereo_fixed_t* BLARGG_RESTRICT out = (stereo_fixed_t*) &echo [echo_pos];
					int const bass = BLIP_READER_BASS( *buf );
					BLIP_READER_BEGIN( in, *buf );
					BLIP_READER_ADJ_( in, mixer.samples_read );
					fixed_t const vol_0 = buf->vol [0];
					fixed_t const vol_1 = buf->vol [1];
					
					int count = (unsigned) (echo_size - echo_pos) / stereo;
					int remain = pair_count;
					if ( count > remain )
						count = remain;
					do
					{
						remain -= count;
						BLIP_READER_ADJ_( in, count );
						
						out += count;
						int offset = -count;
						do
						{
							fixed_t s = BLIP_READER_READ( in );
							BLIP_READER_NEXT_IDX_( in, bass, offset );
							
							out [offset] [0] += s * vol_0;
							out [offset] [1] += s * vol_1;
						}
						while ( ++offset );
						
						out = (stereo_fixed_t*) echo.begin();
						count = remain;
					}
					while ( remain );
					
					BLIP_READER_END( in, *buf );
				}
				buf++;
			}
			while ( --bufs_remain );
		}
		
		// add echo
		if ( echo_phase && !no_echo )
		{
			fixed_t const feedback = s.feedback;
			fixed_t const treble   = s.treble;
			
			int i = 1;
			do
			{
				fixed_t low_pass = s.low_pass [i];
				
				fixed_t* echo_end = &echo [echo_size + i];
				fixed_t const* BLARGG_RESTRICT in_pos = &echo [echo_pos + i];
				int out_offset = echo_pos + i + s.delay [i];
				if ( out_offset >= echo_size )
					out_offset -= echo_size;
				assert( out_offset < echo_size );
				fixed_t* BLARGG_RESTRICT out_pos = &echo [out_offset];
				
				// break into up to three chunks to avoid having to handle wrap-around
				// in middle of core loop
				int remain = pair_count;
				do
				{
					fixed_t const* pos = in_pos;
					if ( pos < out_pos )
						pos = out_pos;
					int count = (unsigned) ((char*) echo_end - (char const*) pos) /
							(unsigned) (stereo * sizeof (fixed_t));
					if ( count > remain )
						count = remain;
					remain -= count;
					
					in_pos  += count * stereo;
					out_pos += count * stereo;
					int offset = -count;
					do
					{
						low_pass += FROM_FIXED( in_pos [offset * stereo] - low_pass ) * treble;
						out_pos [offset * stereo] = FROM_FIXED( low_pass ) * feedback;
					}
					while ( ++offset );
					
					if (  in_pos >= echo_end )  in_pos -= echo_size;
					if ( out_pos >= echo_end ) out_pos -= echo_size;
				}
				while ( remain );
				
				s.low_pass [i] = low_pass;
			}
			while ( --i >= 0 );
		}
	}
	while ( --echo_phase >= 0 );
	
	// clamp to 16 bits
	{
		stereo_fixed_t const* BLARGG_RESTRICT in = (stereo_fixed_t*) &echo [echo_pos];
		typedef blip_sample_t stereo_blip_sample_t [stereo];
		stereo_blip_sample_t* BLARGG_RESTRICT out = (stereo_blip_sample_t*) out_;
		int count = (unsigned) (echo_size - echo_pos) / (unsigned) stereo;
		int remain = pair_count;
		if ( count > remain )
			count = remain;
		do
		{
			remain -= count;
			in  += count;
			out += count;
			int offset = -count;
			do
			{
				fixed_t in_0 = FROM_FIXED( in [offset] [0] );
				fixed_t in_1 = FROM_FIXED( in [offset] [1] );
				
				BLIP_CLAMP( in_0, in_0 );
				out [offset] [0] = (blip_sample_t) in_0;
				
				BLIP_CLAMP( in_1, in_1 );
				out [offset] [1] = (blip_sample_t) in_1;
			}
			while ( ++offset );
			
			in = (stereo_fixed_t*) echo.begin();
			count = remain;
		}
		while ( remain );
	}
}
=======
	stereo_remain = 0;
	effect_remain = 0;

	for(int i=0; i<max_voices; i++)
	{
		if ( echo_buf[i].size() )
			memset( &echo_buf[i][0], 0, echo_size * sizeof echo_buf[i][0] );

		if ( reverb_buf[i].size() )
			memset( &reverb_buf[i][0], 0, reverb_size * sizeof reverb_buf[i][0] );
	}

	for ( int i = 0; i < buf_count; i++ )
		bufs [i].clear();
}

inline int pin_range( int n, int max, int min = 0 )
{
	if ( n < min )
		return min;
	if ( n > max )
		return max;
	return n;
}

void Effects_Buffer::config( const config_t& cfg )
{
	channels_changed();

	// clear echo and reverb buffers
	// ensure the echo/reverb buffers have already been allocated, so this method can be
	// called before set_sample_rate is called
	if ( !config_.effects_enabled && cfg.effects_enabled && echo_buf[0].size() )
	{
		for(int i=0; i<max_voices; i++)
		{
			memset( &echo_buf[i][0], 0, echo_size * sizeof echo_buf[i][0] );
			memset( &reverb_buf[i][0], 0, reverb_size * sizeof reverb_buf[i][0] );
		}
	}

	config_ = cfg;

	if ( config_.effects_enabled )
	{
		// convert to internal format

		chans.pan_1_levels [0] = TO_FIXED( 1 ) - TO_FIXED( config_.pan_1 );
		chans.pan_1_levels [1] = TO_FIXED( 2 ) - chans.pan_1_levels [0];

		chans.pan_2_levels [0] = TO_FIXED( 1 ) - TO_FIXED( config_.pan_2 );
		chans.pan_2_levels [1] = TO_FIXED( 2 ) - chans.pan_2_levels [0];

		chans.reverb_level = TO_FIXED( config_.reverb_level );
		chans.echo_level = TO_FIXED( config_.echo_level );

		int delay_offset = int (1.0 / 2000 * config_.delay_variance * sample_rate());

		int reverb_sample_delay = int (1.0 / 1000 * config_.reverb_delay * sample_rate());
		chans.reverb_delay_l = pin_range( reverb_size -
				(reverb_sample_delay - delay_offset) * 2, reverb_size - 2, 0 );
		chans.reverb_delay_r = pin_range( reverb_size + 1 -
				(reverb_sample_delay + delay_offset) * 2, reverb_size - 1, 1 );

		int echo_sample_delay = int (1.0 / 1000 * config_.echo_delay * sample_rate());
		chans.echo_delay_l = pin_range( echo_size - 1 - (echo_sample_delay - delay_offset),
				echo_size - 1 );
		chans.echo_delay_r = pin_range( echo_size - 1 - (echo_sample_delay + delay_offset),
				echo_size - 1 );

		for(int i=0; i<max_voices; i++)
		{
			chan_types [i*chan_types_count+0].center = &bufs [i*max_buf_count+0];
			chan_types [i*chan_types_count+0].left   = &bufs [i*max_buf_count+3];
			chan_types [i*chan_types_count+0].right  = &bufs [i*max_buf_count+4];

			chan_types [i*chan_types_count+1].center = &bufs [i*max_buf_count+1];
			chan_types [i*chan_types_count+1].left   = &bufs [i*max_buf_count+3];
			chan_types [i*chan_types_count+1].right  = &bufs [i*max_buf_count+4];

			chan_types [i*chan_types_count+2].center = &bufs [i*max_buf_count+2];
			chan_types [i*chan_types_count+2].left   = &bufs [i*max_buf_count+5];
			chan_types [i*chan_types_count+2].right  = &bufs [i*max_buf_count+6];
		}
		blaarg_static_assert( chan_types_count >= 3, "Need at least three audio channels for effects processing" );
	}
	else
	{
		for(int i=0; i<max_voices; i++)
		{
			// set up outputs
			for ( int j = 0; j < chan_types_count; j++ )
			{
				channel_t& c = chan_types [i*chan_types_count+j];
				c.center = &bufs [i*max_buf_count+0];
				c.left   = &bufs [i*max_buf_count+1];
				c.right  = &bufs [i*max_buf_count+2];
			}
		}
	}

	if ( buf_count < max_buf_count ) // if center_only
	{
		for(int i=0; i<max_voices; i++)
		{
			for ( int j = 0; j < chan_types_count; j++ )
			{
				channel_t& c = chan_types [i*chan_types_count+j];
				c.left   = c.center;
				c.right  = c.center;
			}
		}
	}
}

Effects_Buffer::channel_t Effects_Buffer::channel( int i, int type )
{
	int out = chan_types_count-1;
	if ( !type )
	{
		out = i % 5;
		if ( out > chan_types_count-1 )
			out = chan_types_count-1;
	}
	else if ( !(type & noise_type) && (type & type_index_mask) % 3 != 0 )
	{
		out = type & 1;
	}
	return chan_types [(i%max_voices)*chan_types_count+out];
}

void Effects_Buffer::end_frame( blip_time_t clock_count )
{
	int bufs_used = 0;
	int stereo_mask = (config_.effects_enabled ? 0x78 : 0x06);

	const int buf_count_per_voice = buf_count/max_voices;
	for ( int v = 0; v < max_voices; v++ ) // foreach voice
	{
		for ( int i = 0; i < buf_count_per_voice; i++) // foreach buffer of that voice
		{
			bufs_used |= bufs [v*buf_count_per_voice + i].clear_modified() << i;
			bufs [v*buf_count_per_voice + i].end_frame( clock_count );

			if ( (bufs_used & stereo_mask) && buf_count == max_voices*max_buf_count )
				stereo_remain = max(stereo_remain, bufs [v*buf_count_per_voice + i].samples_avail() + bufs [v*buf_count_per_voice + i].output_latency());
			if ( effects_enabled || config_.effects_enabled )
				effect_remain = max(effect_remain, bufs [v*buf_count_per_voice + i].samples_avail() + bufs [v*buf_count_per_voice + i].output_latency());
		}
		bufs_used = 0;
	}

	effects_enabled = config_.effects_enabled;
}

long Effects_Buffer::samples_avail() const
{
	return bufs [0].samples_avail() * 2;
}

long Effects_Buffer::read_samples( blip_sample_t* out, long total_samples )
{
	const int n_channels = max_voices * 2;
	const int buf_count_per_voice = buf_count/max_voices;

	require( total_samples % n_channels == 0 ); // as many items needed to fill at least one frame

	long remain = bufs [0].samples_avail();
	total_samples = remain = min( remain, total_samples/n_channels );

	while ( remain )
	{
		int active_bufs = buf_count_per_voice;
		long count = remain;

		// optimizing mixing to skip any channels which had nothing added
		if ( effect_remain )
		{
			if ( count > effect_remain )
				count = effect_remain;

			if ( stereo_remain )
			{
				mix_enhanced( out, count );
			}
			else
			{
				mix_mono_enhanced( out, count );
				active_bufs = 3;
			}
		}
		else if ( stereo_remain )
		{
			mix_stereo( out, count );
			active_bufs = 3;
		}
		else
		{
			mix_mono( out, count );
			active_bufs = 1;
		}

		out += count * n_channels;
		remain -= count;

		stereo_remain -= count;
		if ( stereo_remain < 0 )
			stereo_remain = 0;

		effect_remain -= count;
		if ( effect_remain < 0 )
			effect_remain = 0;

		// skip the output from any buffers that didn't contribute to the sound output
		// during this frame (e.g. if we only render mono then only the very first buf
		// is 'active')
		for ( int v = 0; v < max_voices; v++ ) // foreach voice
		{
			for ( int i = 0; i < buf_count_per_voice; i++) // foreach buffer of that voice
			{
				if ( i < active_bufs )
					bufs [v*buf_count_per_voice + i].remove_samples( count );
				else // keep time synchronized
					bufs [v*buf_count_per_voice + i].remove_silence( count );
			}
		}
	}

	return total_samples * n_channels;
}

void Effects_Buffer::mix_mono( blip_sample_t* out_, int32_t count )
{
    for(int i=0; i<max_voices; i++)
    {
	blip_sample_t* BLIP_RESTRICT out = out_;
	int const bass = BLIP_READER_BASS( bufs [i*max_buf_count+0] );
	BLIP_READER_BEGIN( c, bufs [i*max_buf_count+0] );

	// unrolled loop
	for ( int32_t n = count >> 1; n; --n )
	{
		int32_t cs0 = BLIP_READER_READ( c );
		BLIP_READER_NEXT( c, bass );

		int32_t cs1 = BLIP_READER_READ( c );
		BLIP_READER_NEXT( c, bass );

		if ( (int16_t) cs0 != cs0 )
			cs0 = 0x7FFF - (cs0 >> 24);
		((uint32_t*) out) [i] = ((uint16_t) cs0) | (uint16_t(cs0) << 16);

		if ( (int16_t) cs1 != cs1 )
			cs1 = 0x7FFF - (cs1 >> 24);
		((uint32_t*) out) [i+max_voices] = ((uint16_t) cs1) | (uint16_t(cs1) << 16);
		out += max_voices*4;
	}

	if ( count & 1 )
	{
		int s = BLIP_READER_READ( c );
		BLIP_READER_NEXT( c, bass );
		if ( (int16_t) s != s )
			s = 0x7FFF - (s >> 24);
		out [i*2+0] = s;
		out [i*2+1] = s;
	}

	BLIP_READER_END( c, bufs [i*max_buf_count+0] );
    }
}

void Effects_Buffer::mix_stereo( blip_sample_t* out_, int32_t frames )
{
    for(int i=0; i<max_voices; i++)
    {
	blip_sample_t* BLIP_RESTRICT out = out_;
	int const bass = BLIP_READER_BASS( bufs [i*max_buf_count+0] );
	BLIP_READER_BEGIN( c, bufs [i*max_buf_count+0] );
	BLIP_READER_BEGIN( l, bufs [i*max_buf_count+1] );
	BLIP_READER_BEGIN( r, bufs [i*max_buf_count+2] );

	int count = frames;
	while ( count-- )
	{
		int cs = BLIP_READER_READ( c );
		BLIP_READER_NEXT( c, bass );
		int left = cs + BLIP_READER_READ( l );
		int right = cs + BLIP_READER_READ( r );
		BLIP_READER_NEXT( l, bass );
		BLIP_READER_NEXT( r, bass );

		if ( (int16_t) left != left )
			left = 0x7FFF - (left >> 24);

		if ( (int16_t) right != right )
			right = 0x7FFF - (right >> 24);

		out [i*2+0] = left;
		out [i*2+1] = right;

		out += max_voices*2;

	}

	BLIP_READER_END( r, bufs [i*max_buf_count+2] );
	BLIP_READER_END( l, bufs [i*max_buf_count+1] );
	BLIP_READER_END( c, bufs [i*max_buf_count+0] );
    }
}

void Effects_Buffer::mix_mono_enhanced( blip_sample_t* out_, int32_t frames )
{
	for(int i=0; i<max_voices; i++)
	{
	blip_sample_t* BLIP_RESTRICT out = out_;
	int const bass = BLIP_READER_BASS( bufs [i*max_buf_count+2] );
	BLIP_READER_BEGIN( center, bufs [i*max_buf_count+2] );
	BLIP_READER_BEGIN( sq1, bufs [i*max_buf_count+0] );
	BLIP_READER_BEGIN( sq2, bufs [i*max_buf_count+1] );

	blip_sample_t* const reverb_buf = &this->reverb_buf[i][0];
	blip_sample_t* const echo_buf = &this->echo_buf[i][0];
	int echo_pos = this->echo_pos[i];
	int reverb_pos = this->reverb_pos[i];

	int count = frames;
	while ( count-- )
	{
		int sum1_s = BLIP_READER_READ( sq1 );
		int sum2_s = BLIP_READER_READ( sq2 );

		BLIP_READER_NEXT( sq1, bass );
		BLIP_READER_NEXT( sq2, bass );

		int new_reverb_l = FMUL( sum1_s, chans.pan_1_levels [0] ) +
				FMUL( sum2_s, chans.pan_2_levels [0] ) +
				reverb_buf [(reverb_pos + chans.reverb_delay_l) & reverb_mask];

		int new_reverb_r = FMUL( sum1_s, chans.pan_1_levels [1] ) +
				FMUL( sum2_s, chans.pan_2_levels [1] ) +
				reverb_buf [(reverb_pos + chans.reverb_delay_r) & reverb_mask];

		fixed_t reverb_level = chans.reverb_level;
		reverb_buf [reverb_pos] = (blip_sample_t) FMUL( new_reverb_l, reverb_level );
		reverb_buf [reverb_pos + 1] = (blip_sample_t) FMUL( new_reverb_r, reverb_level );
		reverb_pos = (reverb_pos + 2) & reverb_mask;

		int sum3_s = BLIP_READER_READ( center );
		BLIP_READER_NEXT( center, bass );

		int left = new_reverb_l + sum3_s + FMUL( chans.echo_level,
				echo_buf [(echo_pos + chans.echo_delay_l) & echo_mask] );
		int right = new_reverb_r + sum3_s + FMUL( chans.echo_level,
				echo_buf [(echo_pos + chans.echo_delay_r) & echo_mask] );

		echo_buf [echo_pos] = sum3_s;
		echo_pos = (echo_pos + 1) & echo_mask;

		if ( (int16_t) left != left )
			left = 0x7FFF - (left >> 24);

		if ( (int16_t) right != right )
			right = 0x7FFF - (right >> 24);

		out [i*2+0] = left;
		out [i*2+1] = right;
		out += max_voices*2;
	}
	this->reverb_pos[i] = reverb_pos;
	this->echo_pos[i] = echo_pos;

	BLIP_READER_END( sq1, bufs [i*max_buf_count+0] );
	BLIP_READER_END( sq2, bufs [i*max_buf_count+1] );
	BLIP_READER_END( center, bufs [i*max_buf_count+2] );
    }
}

void Effects_Buffer::mix_enhanced( blip_sample_t* out_, int32_t frames )
{
    for(int i=0; i<max_voices; i++)
    {
	blip_sample_t* BLIP_RESTRICT out = out_;
	int const bass = BLIP_READER_BASS( bufs [i*max_buf_count+2] );
	BLIP_READER_BEGIN( center, bufs [i*max_buf_count+2] );
	BLIP_READER_BEGIN( l1, bufs [i*max_buf_count+3] );
	BLIP_READER_BEGIN( r1, bufs [i*max_buf_count+4] );
	BLIP_READER_BEGIN( l2, bufs [i*max_buf_count+5] );
	BLIP_READER_BEGIN( r2, bufs [i*max_buf_count+6] );
	BLIP_READER_BEGIN( sq1, bufs [i*max_buf_count+0] );
	BLIP_READER_BEGIN( sq2, bufs [i*max_buf_count+1] );

	blip_sample_t* const reverb_buf = &this->reverb_buf[i][0];
	blip_sample_t* const echo_buf = &this->echo_buf[i][0];
	int echo_pos = this->echo_pos[i];
	int reverb_pos = this->reverb_pos[i];

	int count = frames;
	while ( count-- )
	{
		int sum1_s = BLIP_READER_READ( sq1 );
		int sum2_s = BLIP_READER_READ( sq2 );

		BLIP_READER_NEXT( sq1, bass );
		BLIP_READER_NEXT( sq2, bass );

		int new_reverb_l = FMUL( sum1_s, chans.pan_1_levels [0] ) +
				FMUL( sum2_s, chans.pan_2_levels [0] ) + BLIP_READER_READ( l1 ) +
				reverb_buf [(reverb_pos + chans.reverb_delay_l) & reverb_mask];

		int new_reverb_r = FMUL( sum1_s, chans.pan_1_levels [1] ) +
				FMUL( sum2_s, chans.pan_2_levels [1] ) + BLIP_READER_READ( r1 ) +
				reverb_buf [(reverb_pos + chans.reverb_delay_r) & reverb_mask];

		BLIP_READER_NEXT( l1, bass );
		BLIP_READER_NEXT( r1, bass );

		fixed_t reverb_level = chans.reverb_level;
		reverb_buf [reverb_pos] = (blip_sample_t) FMUL( new_reverb_l, reverb_level );
		reverb_buf [reverb_pos + 1] = (blip_sample_t) FMUL( new_reverb_r, reverb_level );
		reverb_pos = (reverb_pos + 2) & reverb_mask;

		int sum3_s = BLIP_READER_READ( center );
		BLIP_READER_NEXT( center, bass );

		int left = new_reverb_l + sum3_s + BLIP_READER_READ( l2 ) + FMUL( chans.echo_level,
				echo_buf [(echo_pos + chans.echo_delay_l) & echo_mask] );
		int right = new_reverb_r + sum3_s + BLIP_READER_READ( r2 ) + FMUL( chans.echo_level,
				echo_buf [(echo_pos + chans.echo_delay_r) & echo_mask] );

		BLIP_READER_NEXT( l2, bass );
		BLIP_READER_NEXT( r2, bass );

		echo_buf [echo_pos] = sum3_s;
		echo_pos = (echo_pos + 1) & echo_mask;

		if ( (int16_t) left != left )
			left = 0x7FFF - (left >> 24);

		if ( (int16_t) right != right )
			right = 0x7FFF - (right >> 24);

		out [i*2+0] = left;
		out [i*2+1] = right;

		out += max_voices*2;
	}
	this->reverb_pos[i] = reverb_pos;
	this->echo_pos[i] = echo_pos;

	BLIP_READER_END( l1, bufs [i*max_buf_count+3] );
	BLIP_READER_END( r1, bufs [i*max_buf_count+4] );
	BLIP_READER_END( l2, bufs [i*max_buf_count+5] );
	BLIP_READER_END( r2, bufs [i*max_buf_count+6] );
	BLIP_READER_END( sq1, bufs [i*max_buf_count+0] );
	BLIP_READER_END( sq2, bufs [i*max_buf_count+1] );
	BLIP_READER_END( center, bufs [i*max_buf_count+2] );
    }
}

>>>>>>> db7344ebf (abc)
