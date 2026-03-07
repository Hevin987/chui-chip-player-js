<<<<<<< HEAD
// Blip_Buffer $vers. http://www.slack.net/~ant/

#include "Multi_Buffer.h"

/* Copyright (C) 2003-2008 Shay Green. This module is free software; you
=======
// Blip_Buffer 0.4.1. http://www.slack.net/~ant/

#include "Multi_Buffer.h"

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
Multi_Buffer::Multi_Buffer( int spf ) : samples_per_frame_( spf )
{
	length_                 = 0;
	sample_rate_            = 0;
	channels_changed_count_ = 1;
	channel_types_          = NULL;
	channel_count_          = 0;
	immediate_removal_      = true;
}

Multi_Buffer::channel_t Multi_Buffer::channel( int /*index*/ )
{
	channel_t ch;
	ch.center = ch.left = ch.right = NULL;
	return ch;
}
=======
#ifdef BLARGG_ENABLE_OPTIMIZER
	#include BLARGG_ENABLE_OPTIMIZER
#endif

Multi_Buffer::Multi_Buffer( int spf ) : samples_per_frame_( spf )
{
	length_ = 0;
	sample_rate_ = 0;
	channels_changed_count_ = 1;
}

blargg_err_t Multi_Buffer::set_channel_count( int ) { return 0; }
>>>>>>> db7344ebf (abc)

// Silent_Buffer

Silent_Buffer::Silent_Buffer() : Multi_Buffer( 1 ) // 0 channels would probably confuse
{
	// TODO: better to use empty Blip_Buffer so caller never has to check for NULL?
<<<<<<< HEAD
	chan.left   = NULL;
	chan.center = NULL;
	chan.right  = NULL;
=======
	chan.left   = 0;
	chan.center = 0;
	chan.right  = 0;
>>>>>>> db7344ebf (abc)
}

// Mono_Buffer

Mono_Buffer::Mono_Buffer() : Multi_Buffer( 1 )
{
	chan.center = &buf;
	chan.left   = &buf;
	chan.right  = &buf;
}

Mono_Buffer::~Mono_Buffer() { }

<<<<<<< HEAD
blargg_err_t Mono_Buffer::set_sample_rate( int rate, int msec )
=======
blargg_err_t Mono_Buffer::set_sample_rate( long rate, int msec )
>>>>>>> db7344ebf (abc)
{
	RETURN_ERR( buf.set_sample_rate( rate, msec ) );
	return Multi_Buffer::set_sample_rate( buf.sample_rate(), buf.length() );
}

<<<<<<< HEAD

// Tracked_Blip_Buffer

int const blip_buffer_extra = 32; // TODO: explain why this value

Tracked_Blip_Buffer::Tracked_Blip_Buffer()
{
	last_non_silence = 0;
}

void Tracked_Blip_Buffer::clear()
{
	last_non_silence = 0;
	Blip_Buffer::clear();
}

void Tracked_Blip_Buffer::end_frame( blip_time_t t )
{
	Blip_Buffer::end_frame( t );
	if ( modified() )
	{
		clear_modified();
		last_non_silence = samples_avail() + blip_buffer_extra;
	}
}

unsigned Tracked_Blip_Buffer::non_silent() const
{
	return last_non_silence | unsettled();
}

inline void Tracked_Blip_Buffer::remove_( int n )
{
	if ( (last_non_silence -= n) < 0 )
		last_non_silence = 0;
}

void Tracked_Blip_Buffer::remove_silence( int n )
{
	remove_( n );
	Blip_Buffer::remove_silence( n );
}

void Tracked_Blip_Buffer::remove_samples( int n )
{
	remove_( n );
	Blip_Buffer::remove_samples( n );
}

void Tracked_Blip_Buffer::remove_all_samples()
{
	int avail = samples_avail();
	if ( !non_silent() )
		remove_silence( avail );
	else
		remove_samples( avail );
}

int Tracked_Blip_Buffer::read_samples( blip_sample_t out [], int count )
{
	count = Blip_Buffer::read_samples( out, count );
	remove_( count );
	return count;
}

// Stereo_Buffer

int const stereo = 2;

Stereo_Buffer::Stereo_Buffer() : Multi_Buffer( 2 )
{
	chan.center = mixer.bufs [2] = &bufs [2];
	chan.left   = mixer.bufs [0] = &bufs [0];
	chan.right  = mixer.bufs [1] = &bufs [1];
	mixer.samples_read = 0;
=======
// Stereo_Buffer

Stereo_Buffer::Stereo_Buffer() : Multi_Buffer( 2 )
{
	chan.center = &bufs [0];
	chan.left = &bufs [1];
	chan.right = &bufs [2];
>>>>>>> db7344ebf (abc)
}

Stereo_Buffer::~Stereo_Buffer() { }

<<<<<<< HEAD
blargg_err_t Stereo_Buffer::set_sample_rate( int rate, int msec )
{
	mixer.samples_read = 0;
	for ( int i = bufs_size; --i >= 0; )
=======
blargg_err_t Stereo_Buffer::set_sample_rate( long rate, int msec )
{
	for ( int i = 0; i < buf_count; i++ )
>>>>>>> db7344ebf (abc)
		RETURN_ERR( bufs [i].set_sample_rate( rate, msec ) );
	return Multi_Buffer::set_sample_rate( bufs [0].sample_rate(), bufs [0].length() );
}

<<<<<<< HEAD
void Stereo_Buffer::clock_rate( int rate )
{
	for ( int i = bufs_size; --i >= 0; )
=======
void Stereo_Buffer::clock_rate( uint32_t rate )
{
	for ( int i = 0; i < buf_count; i++ )
>>>>>>> db7344ebf (abc)
		bufs [i].clock_rate( rate );
}

void Stereo_Buffer::bass_freq( int bass )
{
<<<<<<< HEAD
	for ( int i = bufs_size; --i >= 0; )
=======
	for ( int i = 0; i < buf_count; i++ )
>>>>>>> db7344ebf (abc)
		bufs [i].bass_freq( bass );
}

void Stereo_Buffer::clear()
{
<<<<<<< HEAD
	mixer.samples_read = 0;
	for ( int i = bufs_size; --i >= 0; )
		bufs [i].clear();
}

void Stereo_Buffer::end_frame( blip_time_t time )
{
	for ( int i = bufs_size; --i >= 0; )
		bufs [i].end_frame( time );
}

int Stereo_Buffer::read_samples( blip_sample_t out [], int out_size )
{
	require( (out_size & 1) == 0 ); // must read an even number of samples
	out_size = min( out_size, samples_avail() );

	int pair_count = int (out_size >> 1);
	if ( pair_count )
	{
		mixer.read_pairs( out, pair_count );
		
		if ( samples_avail() <= 0 || immediate_removal() )
		{
			for ( int i = bufs_size; --i >= 0; )
			{
				buf_t& b = bufs [i];
				// TODO: might miss non-silence settling since it checks END of last read
				if ( !b.non_silent() )
					b.remove_silence( mixer.samples_read );
				else
					b.remove_samples( mixer.samples_read );
			}
			mixer.samples_read = 0;
		}
	}
	return out_size;
}


// Stereo_Mixer

// mixers use a single index value to improve performance on register-challenged processors
// offset goes from negative to zero

void Stereo_Mixer::read_pairs( blip_sample_t out [], int count )
{
	// TODO: if caller never marks buffers as modified, uses mono
	// except that buffer isn't cleared, so caller can encounter
	// subtle problems and not realize the cause.
	samples_read += count;
	if ( bufs [0]->non_silent() | bufs [1]->non_silent() )
		mix_stereo( out, count );
	else
		mix_mono( out, count );
}

void Stereo_Mixer::mix_mono( blip_sample_t out_ [], int count )
{
	int const bass = bufs [2]->highpass_shift();
	Blip_Buffer::delta_t const* center = bufs [2]->read_pos() + samples_read;
	int center_sum = bufs [2]->integrator();
	
	typedef blip_sample_t stereo_blip_sample_t [stereo];
	stereo_blip_sample_t* BLARGG_RESTRICT out = (stereo_blip_sample_t*) out_ + count;
	int offset = -count;
	do
	{
		int s = center_sum >> bufs [2]->delta_bits;
		
		center_sum -= center_sum >> bass;
		center_sum += center [offset];
		
		BLIP_CLAMP( s, s );
		
		out [offset] [0] = (blip_sample_t) s;
		out [offset] [1] = (blip_sample_t) s;
	}
	while ( ++offset );
	
	bufs [2]->set_integrator( center_sum );
}

void Stereo_Mixer::mix_stereo( blip_sample_t out_ [], int count )
{
	blip_sample_t* BLARGG_RESTRICT out = out_ + count * stereo;
	
	// do left + center and right + center separately to reduce register load
	Tracked_Blip_Buffer* const* buf = &bufs [2];
	while ( true ) // loop runs twice
	{
		--buf;
		--out;
		
		int const bass = bufs [2]->highpass_shift();
		Blip_Buffer::delta_t const* side = (*buf)->read_pos() + samples_read;
		Blip_Buffer::delta_t const* center = bufs [2]->read_pos() + samples_read;
	
		int side_sum = (*buf)->integrator();
		int center_sum = bufs [2]->integrator();
		
		int offset = -count;
		do
		{
			int s = (center_sum + side_sum) >> Blip_Buffer::delta_bits;
			
			side_sum   -= side_sum   >> bass;
			center_sum -= center_sum >> bass;
			
			side_sum   += side   [offset];
			center_sum += center [offset];
			
			BLIP_CLAMP( s, s );
			
			++offset; // before write since out is decremented to slightly before end
			out [offset * stereo] = (blip_sample_t) s;
		}
		while ( offset );
		
		(*buf)->set_integrator( side_sum );
		
		if ( buf != bufs )
			continue;
		
		// only end center once
		bufs [2]->set_integrator( center_sum );
		break;
	}
=======
	stereo_added = 0;
	was_stereo   = false;
	for ( int i = 0; i < buf_count; i++ )
		bufs [i].clear();
}

void Stereo_Buffer::end_frame( blip_time_t clock_count )
{
	stereo_added = 0;
	for ( int i = 0; i < buf_count; i++ )
	{
		stereo_added |= bufs [i].clear_modified() << i;
		bufs [i].end_frame( clock_count );
	}
}

long Stereo_Buffer::read_samples( blip_sample_t* out, long count )
{
	require( !(count & 1) ); // count must be even
	count = (unsigned) count / 2;

	long avail = bufs [0].samples_avail();
	if ( count > avail )
		count = avail;
	if ( count )
	{
		int bufs_used = stereo_added | was_stereo;
		//debug_printf( "%X\n", bufs_used );
		if ( bufs_used <= 1 )
		{
			mix_mono( out, count );
			bufs [0].remove_samples( count );
			bufs [1].remove_silence( count );
			bufs [2].remove_silence( count );
		}
		else if ( bufs_used & 1 )
		{
			mix_stereo( out, count );
			bufs [0].remove_samples( count );
			bufs [1].remove_samples( count );
			bufs [2].remove_samples( count );
		}
		else
		{
			mix_stereo_no_center( out, count );
			bufs [0].remove_silence( count );
			bufs [1].remove_samples( count );
			bufs [2].remove_samples( count );
		}

		// to do: this might miss opportunities for optimization
		if ( !bufs [0].samples_avail() )
		{
			was_stereo   = stereo_added;
			stereo_added = 0;
		}
	}

	return count * 2;
}

void Stereo_Buffer::mix_stereo( blip_sample_t* out_, int32_t count )
{
	blip_sample_t* BLIP_RESTRICT out = out_;
	int const bass = BLIP_READER_BASS( bufs [1] );
	BLIP_READER_BEGIN( left, bufs [1] );
	BLIP_READER_BEGIN( right, bufs [2] );
	BLIP_READER_BEGIN( center, bufs [0] );

	for ( ; count; --count )
	{
		int c = BLIP_READER_READ( center );
		int32_t l = c + BLIP_READER_READ( left );
		int32_t r = c + BLIP_READER_READ( right );
		if ( (int16_t) l != l )
			l = 0x7FFF - (l >> 24);

		BLIP_READER_NEXT( center, bass );
		if ( (int16_t) r != r )
			r = 0x7FFF - (r >> 24);

		BLIP_READER_NEXT( left, bass );
		BLIP_READER_NEXT( right, bass );

		out [0] = l;
		out [1] = r;
		out += 2;
	}

	BLIP_READER_END( center, bufs [0] );
	BLIP_READER_END( right, bufs [2] );
	BLIP_READER_END( left, bufs [1] );
}

void Stereo_Buffer::mix_stereo_no_center( blip_sample_t* out_, int32_t count )
{
	blip_sample_t* BLIP_RESTRICT out = out_;
	int const bass = BLIP_READER_BASS( bufs [1] );
	BLIP_READER_BEGIN( left, bufs [1] );
	BLIP_READER_BEGIN( right, bufs [2] );

	for ( ; count; --count )
	{
		int32_t l = BLIP_READER_READ( left );
		if ( (int16_t) l != l )
			l = 0x7FFF - (l >> 24);

		int32_t r = BLIP_READER_READ( right );
		if ( (int16_t) r != r )
			r = 0x7FFF - (r >> 24);

		BLIP_READER_NEXT( left, bass );
		BLIP_READER_NEXT( right, bass );

		out [0] = l;
		out [1] = r;
		out += 2;
	}

	BLIP_READER_END( right, bufs [2] );
	BLIP_READER_END( left, bufs [1] );
}

void Stereo_Buffer::mix_mono( blip_sample_t* out_, int32_t count )
{
	blip_sample_t* BLIP_RESTRICT out = out_;
	int const bass = BLIP_READER_BASS( bufs [0] );
	BLIP_READER_BEGIN( center, bufs [0] );

	for ( ; count; --count )
	{
		int32_t s = BLIP_READER_READ( center );
		if ( (int16_t) s != s )
			s = 0x7FFF - (s >> 24);

		BLIP_READER_NEXT( center, bass );
		out [0] = s;
		out [1] = s;
		out += 2;
	}

	BLIP_READER_END( center, bufs [0] );
>>>>>>> db7344ebf (abc)
}
