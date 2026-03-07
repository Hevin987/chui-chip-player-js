// Band-limited sound synthesis buffer

<<<<<<< HEAD
// Blip_Buffer $vers
#ifndef BLIP_BUFFER_H
#define BLIP_BUFFER_H

#include "blargg_common.h"
#include "Blip_Buffer_impl.h"

typedef int blip_time_t;                    // Source clocks in current time frame
typedef BOOST::int16_t blip_sample_t;       // 16-bit signed output sample
int const blip_default_length = 1000 / 4;   // Default Blip_Buffer length (1/4 second)


//// Sample buffer for band-limited synthesis

class Blip_Buffer : public Blip_Buffer_ {
public:

	// Sets output sample rate and resizes and clears sample buffer
	blargg_err_t set_sample_rate( int samples_per_sec, int msec_length = blip_default_length );
	
	// Sets number of source time units per second
	void clock_rate( int clocks_per_sec );
	
	// Clears buffer and removes all samples
	void clear();
	
	// Use Blip_Synth to add waveform to buffer
	
	// Resamples to time t, then subtracts t from current time. Appends result of resampling
	// to buffer for reading.
	void end_frame( blip_time_t t );
	
	// Number of samples available for reading with read_samples()
	int samples_avail() const;
	
	// Reads at most n samples to out [0 to n-1] and returns number actually read. If stereo
	// is true, writes to out [0], out [2], out [4] etc. instead.
	int read_samples( blip_sample_t out [], int n, bool stereo = false );
	
// More features

	// Sets flag that tells some Multi_Buffer types that sound was added to buffer,
	// so they know that it needs to be mixed in. Only needs to be called once
	// per time frame that sound was added. Not needed if not using Multi_Buffer.
	void set_modified()                 { modified_ = true; }
	
	// Sets high-pass filter frequency, from 0 to 20000 Hz, where higher values reduce bass more
	void bass_freq( int frequency );

	int length() const;         // Length of buffer in milliseconds
	int sample_rate() const;    // Current output sample rate
	int clock_rate() const;     // Number of source time units per second
	int output_latency() const; // Number of samples delay from offset() to read_samples()
	
// Low-level features
	
	// Removes the first n samples
	void remove_samples( int n );
	
	// Returns number of clocks needed until n samples will be available.
	// If buffer cannot even hold n samples, returns number of clocks
	// until buffer becomes full.
	blip_time_t count_clocks( int n ) const;
	
	// Number of samples that should be mixed before calling end_frame( t )
	int count_samples( blip_time_t t ) const;
	
	// Mixes n samples into buffer
	void mix_samples( const blip_sample_t in [], int n );

// Resampled time (sorry, poor documentation right now)
	
	// Resampled time is fixed-point, in terms of output samples.
	
	// Converts clock count to resampled time
	blip_resampled_time_t resampled_duration( int t ) const         { return t * factor_; }
	
	// Converts clock time since beginning of current time frame to resampled time
	blip_resampled_time_t resampled_time( blip_time_t t ) const     { return t * factor_ + offset_; }
	
	// Returns factor that converts clock rate to resampled time
	blip_resampled_time_t clock_rate_factor( int clock_rate ) const;
	
// State save/load

	// Saves state, including high-pass filter and tails of last deltas.
	// All samples must have been read from buffer before calling this
	// (that is, samples_avail() must return 0).
	void save_state( blip_buffer_state_t* out );
	
	// Loads state. State must have been saved from Blip_Buffer with same
	// settings during same run of program; states can NOT be stored on disk.
	// Clears buffer before loading state.
	void load_state( const blip_buffer_state_t& in );

=======
// Blip_Buffer 0.4.1

#ifndef BLIP_BUFFER_H
#define BLIP_BUFFER_H

	// internal
	#include <limits.h>
	#include <stdint.h>
	#if INT_MAX < 0x7FFFFFFF
		#error "int must be at least 32 bits"
	#endif

	typedef int blip_long;
	typedef unsigned blip_ulong;

// Time unit at source clock rate
typedef blip_long blip_time_t;

// Output samples are 16-bit signed, with a range of -32768 to 32767
typedef short blip_sample_t;
enum { blip_sample_max = 32767 };

class Blip_Buffer {
public:
	typedef const char* blargg_err_t;

	// Set output sample rate and buffer length in milliseconds (1/1000 sec, defaults
	// to 1/4 second), then clear buffer. Returns NULL on success, otherwise if there
	// isn't enough memory, returns error without affecting current buffer setup.
	blargg_err_t set_sample_rate( long samples_per_sec, int msec_length = 1000 / 4 );

	// Set number of source time units per second
	void clock_rate( uint32_t );

	// End current time frame of specified duration and make its samples available
	// (along with any still-unread samples) for reading with read_samples(). Begins
	// a new time frame at the end of the current frame.
	void end_frame( blip_time_t time );

	// Read at most 'max_samples' out of buffer into 'dest', removing them from from
	// the buffer. Returns number of samples actually read and removed. If stereo is
	// true, increments 'dest' one extra time after writing each sample, to allow
	// easy interleving of two channels into a stereo output buffer.
	long read_samples( blip_sample_t* dest, long max_samples, int stereo = 0 );

// Additional optional features

	// Current output sample rate
	long sample_rate() const;

	// Length of buffer, in milliseconds
	int length() const;

	// Number of source time units per second
	uint32_t clock_rate() const;

	// Set frequency high-pass filter frequency, where higher values reduce bass more
	void bass_freq( int frequency );

	// Number of samples delay from synthesis to samples read out
	int output_latency() const;

	// Remove all available samples and clear buffer to silence. If 'entire_buffer' is
	// false, just clears out any samples waiting rather than the entire buffer.
	void clear( int entire_buffer = 1 );

	// Number of samples available for reading with read_samples()
	long samples_avail() const;

	// Remove 'count' samples from those waiting to be read
	void remove_samples( long count );

// Experimental features

	// Count number of clocks needed until 'count' samples will be available.
	// If buffer can't even hold 'count' samples, returns number of clocks until
	// buffer becomes full.
	blip_time_t count_clocks( long count ) const;

	// Number of raw samples that can be mixed within frame of specified duration.
	long count_samples( blip_time_t duration ) const;

	// Mix 'count' samples from 'buf' into buffer.
	void mix_samples( blip_sample_t const* buf, long count );

	// not documented yet
	void set_modified() { modified_ = 1; }
	int clear_modified() { int b = modified_; modified_ = 0; return b; }
	typedef blip_ulong blip_resampled_time_t;
	void remove_silence( long count );
	blip_resampled_time_t resampled_duration( int t ) const     { return t * factor_; }
	blip_resampled_time_t resampled_time( blip_time_t t ) const { return t * factor_ + offset_; }
	blip_resampled_time_t clock_rate_factor( uint32_t clock_rate ) const;
public:
	Blip_Buffer();
	~Blip_Buffer();

	// Deprecated
	typedef blip_resampled_time_t resampled_time_t;
	blargg_err_t sample_rate( long r ) { return set_sample_rate( r ); }
	blargg_err_t sample_rate( long r, int msec ) { return set_sample_rate( r, msec ); }
>>>>>>> db7344ebf (abc)
private:
	// noncopyable
	Blip_Buffer( const Blip_Buffer& );
	Blip_Buffer& operator = ( const Blip_Buffer& );
<<<<<<< HEAD

// Implementation
public:
	BLARGG_DISABLE_NOTHROW
	Blip_Buffer();
	~Blip_Buffer();
	void remove_silence( int n );
};


//// Adds amplitude changes to Blip_Buffer

template<int quality,int range> class Blip_Synth;

typedef Blip_Synth<8, 1> Blip_Synth_Fast; // faster, but less equalizer control
typedef Blip_Synth<12,1> Blip_Synth_Norm; // good for most things
typedef Blip_Synth<16,1> Blip_Synth_Good; // sharper filter cutoff

template<int quality,int range>
class Blip_Synth {
public:

	// Sets volume of amplitude delta unit
	void volume( double v )                     { impl.volume_unit( 1.0 / range * v ); }
	
	// Configures low-pass filter
	void treble_eq( const blip_eq_t& eq )       { impl.treble_eq( eq ); }
	
	// Gets/sets default Blip_Buffer
	Blip_Buffer* output() const                 { return impl.buf; }
	void output( Blip_Buffer* b )               { impl.buf = b; impl.last_amp = 0; }
	
	// Extends waveform to time t at current amplitude, then changes its amplitude to a
	// Using this requires a separate Blip_Synth for each waveform.
	void update( blip_time_t t, int a );

// Low-level interface
	
	// If no Blip_Buffer* is specified, uses one set by output() above
	
	// Adds amplitude transition at time t. Delta can be positive or negative.
	// The actual change in amplitude is delta * volume.
	void offset( blip_time_t t, int delta, Blip_Buffer* ) const;
	void offset( blip_time_t t, int delta               ) const { offset( t, delta, impl.buf ); }
	
	// Same as offset(), except code is inlined for higher performance
	void offset_inline( blip_time_t t, int delta, Blip_Buffer* buf ) const { offset_resampled( buf->to_fixed( t ), delta, buf ); }
	void offset_inline( blip_time_t t, int delta                   ) const { offset_resampled( impl.buf->to_fixed( t ), delta, impl.buf ); }
	
	// Works directly in terms of fractional output samples. Use resampled time functions in Blip_Buffer
	// to convert clock counts to resampled time.
	void offset_resampled( blip_resampled_time_t, int delta, Blip_Buffer* ) const;
	
// Implementation
public:
	BLARGG_DISABLE_NOTHROW
=======
public:
	typedef blip_time_t buf_t_;
	blip_ulong factor_;
	blip_resampled_time_t offset_;
	buf_t_* buffer_;
	blip_long buffer_size_;
	blip_long reader_accum_;
	int bass_shift_;
private:
	long sample_rate_;
	uint32_t clock_rate_;
	int bass_freq_;
	int length_;
	int modified_;
	friend class Blip_Reader;
};

#include "blargg_config.h"

// Number of bits in resample ratio fraction. Higher values give a more accurate ratio
// but reduce maximum buffer size.
#ifndef BLIP_BUFFER_ACCURACY
	#define BLIP_BUFFER_ACCURACY 16
#endif

// Number bits in phase offset. Fewer than 6 bits (64 phase offsets) results in
// noticeable broadband noise when synthesizing high frequency square waves.
// Affects size of Blip_Synth objects since they store the waveform directly.
#ifndef BLIP_PHASE_BITS
	#if BLIP_BUFFER_FAST
		#define BLIP_PHASE_BITS 8
	#else
		#define BLIP_PHASE_BITS 6
	#endif
#endif

	// Internal
	typedef blip_ulong blip_resampled_time_t;
	int const blip_widest_impulse_ = 16;
	int const blip_buffer_extra_ = blip_widest_impulse_ + 2;
	int const blip_res = 1 << BLIP_PHASE_BITS;
	class blip_eq_t;

	class Blip_Synth_Fast_ {
	public:
		Blip_Buffer* buf;
		int last_amp;
		int delta_factor;

		void volume_unit( double );
		Blip_Synth_Fast_();
		void treble_eq( blip_eq_t const& ) { }
	};

	class Blip_Synth_ {
	public:
		Blip_Buffer* buf;
		int last_amp;
		int delta_factor;

		void volume_unit( double );
		Blip_Synth_( short* impulses, int width );
		void treble_eq( blip_eq_t const& );
	private:
		double volume_unit_;
		short* const impulses;
		int const width;
		blip_long kernel_unit;
		int impulses_size() const { return blip_res / 2 * width + 1; }
		void adjust_impulse();
	};

// Quality level. Start with blip_good_quality.
const int blip_med_quality  = 8;
const int blip_good_quality = 12;
const int blip_high_quality = 16;

// Range specifies the greatest expected change in amplitude. Calculate it
// by finding the difference between the maximum and minimum expected
// amplitudes (max - min).
template<int quality,int range>
class Blip_Synth {
public:
	// Set overall volume of waveform
	void volume( double v ) { impl.volume_unit( v * (1.0 / (range < 0 ? -range : range)) ); }

	// Configure low-pass filter (see blip_buffer.txt)
	void treble_eq( blip_eq_t const& eq )       { impl.treble_eq( eq ); }

	// Get/set Blip_Buffer used for output
	Blip_Buffer* output() const                 { return impl.buf; }
	void output( Blip_Buffer* b )               { impl.buf = b; impl.last_amp = 0; }

	// Update amplitude of waveform at given time. Using this requires a separate
	// Blip_Synth for each waveform.
	void update( blip_time_t time, int amplitude );

// Low-level interface

	// Add an amplitude transition of specified delta, optionally into specified buffer
	// rather than the one set with output(). Delta can be positive or negative.
	// The actual change in amplitude is delta * (volume / range)
	void offset( blip_time_t, int delta, Blip_Buffer* ) const;
	void offset( blip_time_t t, int delta ) const { offset( t, delta, impl.buf ); }

	// Works directly in terms of fractional output samples. Contact author for more info.
	void offset_resampled( blip_resampled_time_t, int delta, Blip_Buffer* ) const;

	// Same as offset(), except code is inlined for higher performance
	void offset_inline( blip_time_t t, int delta, Blip_Buffer* buf ) const {
		offset_resampled( t * buf->factor_ + buf->offset_, delta, buf );
	}
	void offset_inline( blip_time_t t, int delta ) const {
		offset_resampled( t * impl.buf->factor_ + impl.buf->offset_, delta, impl.buf );
	}
>>>>>>> db7344ebf (abc)

private:
#if BLIP_BUFFER_FAST
	Blip_Synth_Fast_ impl;
<<<<<<< HEAD
	typedef char coeff_t;
#else
	Blip_Synth_ impl;
	typedef short coeff_t;
	// Left halves of first difference of step response for each possible phase
	coeff_t phases [quality / 2 * blip_res];
public:
	Blip_Synth() : impl( phases, quality ) { }
#endif
};


//// Low-pass equalization parameters

class blip_eq_t {
	double treble, kaiser;
	int rolloff_freq, sample_rate, cutoff_freq;
=======
#else
	Blip_Synth_ impl;
	typedef short imp_t;
	imp_t impulses [blip_res * (quality / 2) + 1];
public:
	Blip_Synth() : impl( impulses, quality ) { }
#endif

	// disable broken defaulted constructors, Blip_Synth_ isn't safe to move/copy
	Blip_Synth           (const Blip_Synth  &) = delete;
	Blip_Synth           (      Blip_Synth &&) = delete;
	Blip_Synth& operator=(const Blip_Synth  &) = delete;
};

// Low-pass equalization parameters
class blip_eq_t {
>>>>>>> db7344ebf (abc)
public:
	// Logarithmic rolloff to treble dB at half sampling rate. Negative values reduce
	// treble, small positive values (0 to 5.0) increase treble.
	blip_eq_t( double treble_db = 0 );
<<<<<<< HEAD
	
	// See blip_buffer.txt
	blip_eq_t( double treble, int rolloff_freq, int sample_rate, int cutoff_freq = 0,
			double kaiser = 5.2 );
	
	// Generate center point and right half of impulse response
	virtual void generate( float out [], int count ) const;
	virtual ~blip_eq_t() { }
	
	enum { oversample = blip_res };
	static int calc_count( int quality ) { return (quality - 1) * (oversample / 2) + 1; }
};

#include "Blip_Buffer_impl2.h"
=======

	// See blip_buffer.txt
	blip_eq_t( double treble, long rolloff_freq, long sample_rate, long cutoff_freq = 0 );

private:
	double treble;
	long rolloff_freq;
	long sample_rate;
	long cutoff_freq;
	void generate( float* out, int count ) const;
	friend class Blip_Synth_;
};

int const blip_sample_bits = 30;

// Dummy Blip_Buffer to direct sound output to, for easy muting without
// having to stop sound code.
class Silent_Blip_Buffer : public Blip_Buffer {
	buf_t_ buf [blip_buffer_extra_ + 1];
public:
	// The following cannot be used (an assertion will fail if attempted):
	blargg_err_t set_sample_rate( long samples_per_sec, int msec_length );
	blip_time_t count_clocks( long count ) const;
	void mix_samples( blip_sample_t const* buf, long count );

	Silent_Blip_Buffer();
};

#if (defined(__GNUC__) && (__GNUC__ >= 3)) || \
    (defined(_MSC_VER) && (_MSC_VER >= 1100))
    #define BLIP_RESTRICT __restrict
#else
    #define BLIP_RESTRICT
#endif

// Optimized reading from Blip_Buffer, for use in custom sample output

// Begin reading from buffer. Name should be unique to the current block.
#define BLIP_READER_BEGIN( name, blip_buffer ) \
	const Blip_Buffer::buf_t_* BLIP_RESTRICT name##_reader_buf = (blip_buffer).buffer_;\
	blip_long name##_reader_accum = (blip_buffer).reader_accum_

// Get value to pass to BLIP_READER_NEXT()
#define BLIP_READER_BASS( blip_buffer ) ((blip_buffer).bass_shift_)

// Constant value to use instead of BLIP_READER_BASS(), for slightly more optimal
// code at the cost of having no bass control
int const blip_reader_default_bass = 9;

// Current sample
#define BLIP_READER_READ( name )        (name##_reader_accum >> (blip_sample_bits - 16))

// Current raw sample in full internal resolution
#define BLIP_READER_READ_RAW( name )    (name##_reader_accum)

// Advance to next sample
#define BLIP_READER_NEXT( name, bass ) \
	(void) (name##_reader_accum += *name##_reader_buf++ - (name##_reader_accum >> (bass)))

// End reading samples from buffer. The number of samples read must now be removed
// using Blip_Buffer::remove_samples().
#define BLIP_READER_END( name, blip_buffer ) \
	(void) ((blip_buffer).reader_accum_ = name##_reader_accum)


// Compatibility with older version
const long blip_unscaled = 65535;
const int blip_low_quality  = blip_med_quality;
const int blip_best_quality = blip_high_quality;

// Deprecated; use BLIP_READER macros as follows:
// Blip_Reader r; r.begin( buf ); -> BLIP_READER_BEGIN( r, buf );
// int bass = r.begin( buf )      -> BLIP_READER_BEGIN( r, buf ); int bass = BLIP_READER_BASS( buf );
// r.read()                       -> BLIP_READER_READ( r )
// r.read_raw()                   -> BLIP_READER_READ_RAW( r )
// r.next( bass )                 -> BLIP_READER_NEXT( r, bass )
// r.next()                       -> BLIP_READER_NEXT( r, blip_reader_default_bass )
// r.end( buf )                   -> BLIP_READER_END( r, buf )
class Blip_Reader {
public:
	int begin( Blip_Buffer& );
	blip_long read() const          { return accum >> (blip_sample_bits - 16); }
	blip_long read_raw() const      { return accum; }
	void next( int bass_shift = 9 )         { accum += *buf++ - (accum >> bass_shift); }
	void end( Blip_Buffer& b )              { b.reader_accum_ = accum; }

private:
	const Blip_Buffer::buf_t_* buf;
	blip_long accum;
};

// End of public interface

#include <assert.h>

template<int quality,int range>
inline void Blip_Synth<quality,range>::offset_resampled( blip_resampled_time_t time,
		int delta, Blip_Buffer* blip_buf ) const
{
	// Fails if time is beyond end of Blip_Buffer, due to a bug in caller code or the
	// need for a longer buffer as set by set_sample_rate().
	assert( (blip_long) (time >> BLIP_BUFFER_ACCURACY) < blip_buf->buffer_size_ );
	delta *= impl.delta_factor;
	blip_long* BLIP_RESTRICT buf = blip_buf->buffer_ + (time >> BLIP_BUFFER_ACCURACY);
	int phase = (int) (time >> (BLIP_BUFFER_ACCURACY - BLIP_PHASE_BITS) & (blip_res - 1));

#if BLIP_BUFFER_FAST
	blip_long left = buf [0] + delta;

	// Kind of crappy, but doing shift after multiply results in overflow.
	// Alternate way of delaying multiply by delta_factor results in worse
	// sub-sample resolution.
	blip_long right = (delta >> BLIP_PHASE_BITS) * phase;
	left  -= right;
	right += buf [1];

	buf [0] = left;
	buf [1] = right;
#else

	int const fwd = (blip_widest_impulse_ - quality) / 2;
	int const rev = fwd + quality - 2;
	int const mid = quality / 2 - 1;

	imp_t const* BLIP_RESTRICT imp = impulses + blip_res - phase;

	#if defined (_M_IX86) || defined (_M_IA64) || defined (__i486__) || \
			defined (__x86_64__) || defined (__ia64__) || defined (__i386__)

	// straight forward implementation resulted in better code on GCC for x86

	#define ADD_IMP( out, in ) \
		buf [out] += (blip_long) imp [blip_res * (in)] * delta

	#define BLIP_FWD( i ) {\
		ADD_IMP( fwd     + i, i     );\
		ADD_IMP( fwd + 1 + i, i + 1 );\
	}
	#define BLIP_REV( r ) {\
		ADD_IMP( rev     - r, r + 1 );\
		ADD_IMP( rev + 1 - r, r     );\
	}

		BLIP_FWD( 0 )
		if ( quality > 8  ) BLIP_FWD( 2 )
		if ( quality > 12 ) BLIP_FWD( 4 )
		{
			ADD_IMP( fwd + mid - 1, mid - 1 );
			ADD_IMP( fwd + mid    , mid     );
			imp = impulses + phase;
		}
		if ( quality > 12 ) BLIP_REV( 6 )
		if ( quality > 8  ) BLIP_REV( 4 )
		BLIP_REV( 2 )

		ADD_IMP( rev    , 1 );
		ADD_IMP( rev + 1, 0 );

	#else

	// for RISC processors, help compiler by reading ahead of writes

	#define BLIP_FWD( i ) {\
		blip_long t0 =                       i0 * delta + buf [fwd     + i];\
		blip_long t1 = imp [blip_res * (i + 1)] * delta + buf [fwd + 1 + i];\
		i0 =           imp [blip_res * (i + 2)];\
		buf [fwd     + i] = t0;\
		buf [fwd + 1 + i] = t1;\
	}
	#define BLIP_REV( r ) {\
		blip_long t0 =                 i0 * delta + buf [rev     - r];\
		blip_long t1 = imp [blip_res * r] * delta + buf [rev + 1 - r];\
		i0 =           imp [blip_res * (r - 1)];\
		buf [rev     - r] = t0;\
		buf [rev + 1 - r] = t1;\
	}

		blip_long i0 = *imp;
		BLIP_FWD( 0 )
		if ( quality > 8  ) BLIP_FWD( 2 )
		if ( quality > 12 ) BLIP_FWD( 4 )
		{
			blip_long t0 =                   i0 * delta + buf [fwd + mid - 1];
			blip_long t1 = imp [blip_res * mid] * delta + buf [fwd + mid    ];
			imp = impulses + phase;
			i0 = imp [blip_res * mid];
			buf [fwd + mid - 1] = t0;
			buf [fwd + mid    ] = t1;
		}
		if ( quality > 12 ) BLIP_REV( 6 )
		if ( quality > 8  ) BLIP_REV( 4 )
		BLIP_REV( 2 )

		blip_long t0 =   i0 * delta + buf [rev    ];
		blip_long t1 = *imp * delta + buf [rev + 1];
		buf [rev    ] = t0;
		buf [rev + 1] = t1;
	#endif

#endif
}

#undef BLIP_FWD
#undef BLIP_REV

template<int quality,int range>
#if BLIP_BUFFER_FAST
	inline
#endif
void Blip_Synth<quality,range>::offset( blip_time_t t, int delta, Blip_Buffer* buf ) const
{
	offset_resampled( t * buf->factor_ + buf->offset_, delta, buf );
}

template<int quality,int range>
#if BLIP_BUFFER_FAST
	inline
#endif
void Blip_Synth<quality,range>::update( blip_time_t t, int amp )
{
	int delta = amp - impl.last_amp;
	impl.last_amp = amp;
	offset_resampled( t * impl.buf->factor_ + impl.buf->offset_, delta, impl.buf );
}

inline blip_eq_t::blip_eq_t( double t ) :
		treble( t ), rolloff_freq( 0 ), sample_rate( 44100 ), cutoff_freq( 0 ) { }
inline blip_eq_t::blip_eq_t( double t, long rf, long sr, long cf ) :
		treble( t ), rolloff_freq( rf ), sample_rate( sr ), cutoff_freq( cf ) { }

inline int  Blip_Buffer::length() const         { return length_; }
inline long Blip_Buffer::samples_avail() const
{
    long samples = (long) (offset_ >> BLIP_BUFFER_ACCURACY);
    return samples <= (long) buffer_size_ ? samples : 0;
}
inline long Blip_Buffer::sample_rate() const    { return sample_rate_; }
inline int  Blip_Buffer::output_latency() const { return blip_widest_impulse_ / 2; }
inline uint32_t Blip_Buffer::clock_rate() const     { return clock_rate_; }
inline void Blip_Buffer::clock_rate( uint32_t cps ) { factor_ = clock_rate_factor( clock_rate_ = cps ); }

inline int Blip_Reader::begin( Blip_Buffer& blip_buf )
{
	buf = blip_buf.buffer_;
	accum = blip_buf.reader_accum_;
	return blip_buf.bass_shift_;
}

int const blip_max_length = 0;
int const blip_default_length = 250;
>>>>>>> db7344ebf (abc)

#endif
