// Multi-channel sound buffer interface, and basic mono and stereo buffers

<<<<<<< HEAD
// Blip_Buffer $vers
=======
// Blip_Buffer 0.4.1
>>>>>>> db7344ebf (abc)
#ifndef MULTI_BUFFER_H
#define MULTI_BUFFER_H

#include "blargg_common.h"
#include "Blip_Buffer.h"

// Interface to one or more Blip_Buffers mapped to one or more channels
// consisting of left, center, and right buffers.
class Multi_Buffer {
public:
<<<<<<< HEAD

	// 1=mono, 2=stereo
	Multi_Buffer( int samples_per_frame );
	virtual ~Multi_Buffer()                             { }
	
	// Sets the number of channels available and optionally their types
	// (type information used by Effects_Buffer)
	enum { type_index_mask = 0xFF };
	enum { wave_type = 0x100, noise_type = 0x200, mixed_type = wave_type | noise_type };
	virtual blargg_err_t set_channel_count( int, int const types [] = NULL );
	int channel_count() const                           { return channel_count_; }
	
	// Gets indexed channel, from 0 to channel_count()-1
=======
	Multi_Buffer( int samples_per_frame );
	virtual ~Multi_Buffer() { }

	// Set the number of channels available
	virtual blargg_err_t set_channel_count( int );

	// Get indexed channel, from 0 to channel count - 1
>>>>>>> db7344ebf (abc)
	struct channel_t {
		Blip_Buffer* center;
		Blip_Buffer* left;
		Blip_Buffer* right;
	};
<<<<<<< HEAD
	virtual channel_t channel( int index )              BLARGG_PURE( ; )
	
	// Number of samples per output frame (1 = mono, 2 = stereo)
	int samples_per_frame() const;
	
	// Count of changes to channel configuration. Incremented whenever
	// a change is made to any of the Blip_Buffers for any channel.
	unsigned channels_changed_count()                   { return channels_changed_count_; }
	
	// See Blip_Buffer.h
	virtual blargg_err_t set_sample_rate( int rate, int msec = blip_default_length ) BLARGG_PURE( ; )
	int sample_rate() const;
	int length() const;
	virtual void clock_rate( int )                      BLARGG_PURE( ; )
	virtual void bass_freq( int )                       BLARGG_PURE( ; )
	virtual void clear()                                BLARGG_PURE( ; )
	virtual void end_frame( blip_time_t )               BLARGG_PURE( ; )
	virtual int read_samples( blip_sample_t [], int )   BLARGG_PURE( ; )
	virtual int samples_avail() const                   BLARGG_PURE( ; )

=======
	enum { type_index_mask = 0xFF };
	enum { wave_type = 0x100, noise_type = 0x200, mixed_type = wave_type | noise_type };
	virtual channel_t channel( int index, int type ) = 0;

	// See Blip_Buffer.h
	virtual blargg_err_t set_sample_rate( long rate, int msec = blip_default_length ) = 0;
	virtual void clock_rate( uint32_t ) = 0;
	virtual void bass_freq( int ) = 0;
	virtual void clear() = 0;
	long sample_rate() const;

	// Length of buffer, in milliseconds
	int length() const;

	// See Blip_Buffer.h
	virtual void end_frame( blip_time_t ) = 0;

	// Number of samples per output frame (1 = mono, 2 = stereo)
	int samples_per_frame() const;

	// Count of changes to channel configuration. Incremented whenever
	// a change is made to any of the Blip_Buffers for any channel.
	unsigned channels_changed_count() { return channels_changed_count_; }

	// See Blip_Buffer.h
	virtual long read_samples( blip_sample_t*, long ) = 0;
	virtual long samples_avail() const = 0;

public:
	BLARGG_DISABLE_NOTHROW
protected:
	void channels_changed() { channels_changed_count_++; }
>>>>>>> db7344ebf (abc)
private:
	// noncopyable
	Multi_Buffer( const Multi_Buffer& );
	Multi_Buffer& operator = ( const Multi_Buffer& );

<<<<<<< HEAD
// Implementation
public:
	BLARGG_DISABLE_NOTHROW
	void disable_immediate_removal()                    { immediate_removal_ = false; }

protected:
	bool immediate_removal() const                      { return immediate_removal_; }
	int const* channel_types() const                    { return channel_types_; }
	void channels_changed()                             { channels_changed_count_++; }
		
private:
	unsigned channels_changed_count_;
	int sample_rate_;
	int length_;
	int channel_count_;
	int const samples_per_frame_;
	int const* channel_types_;
	bool immediate_removal_;
};


// Uses a single buffer and outputs mono samples.
class Mono_Buffer : public Multi_Buffer {
public:
	// Buffer used for all channels
	Blip_Buffer* center()                       { return &buf; }
	
// Implementation
public:
	Mono_Buffer();
	~Mono_Buffer();
	virtual blargg_err_t set_sample_rate( int rate, int msec = blip_default_length );
	virtual void clock_rate( int rate )                     { buf.clock_rate( rate ); }
	virtual void bass_freq( int freq )                      { buf.bass_freq( freq ); }
	virtual void clear()                                    { buf.clear(); }
	virtual int samples_avail() const                       { return buf.samples_avail(); }
	virtual int read_samples( blip_sample_t p [], int s )   { return buf.read_samples( p, s ); }
	virtual channel_t channel( int )                        { return chan; }
	virtual void end_frame( blip_time_t t )                 { buf.end_frame( t ); }

private:
	Blip_Buffer buf;
	channel_t chan;
};

	class Tracked_Blip_Buffer : public Blip_Buffer {
	public:
		// Non-zero if buffer still has non-silent samples in it. Requires that you call
		// set_modified() appropriately.
		unsigned non_silent() const;
		
		// remove_samples( samples_avail() )
		void remove_all_samples();
		
	// Implementation
	public:
		BLARGG_DISABLE_NOTHROW
		int read_samples( blip_sample_t [], int );
		void remove_silence( int );
		void remove_samples( int );
		Tracked_Blip_Buffer();
		void clear();
		void end_frame( blip_time_t );
	
	private:
		int last_non_silence;
		
		delta_t unsettled() const { return integrator() >> delta_bits; }
		void remove_( int );
	};
	
	class Stereo_Mixer {
	public:
		Tracked_Blip_Buffer* bufs [3];
		int samples_read;
		
		Stereo_Mixer() : samples_read( 0 ) { }
		void read_pairs( blip_sample_t out [], int count );
	
	private:
		void mix_mono  ( blip_sample_t out [], int pair_count );
		void mix_stereo( blip_sample_t out [], int pair_count );
	};


// Uses three buffers (one for center) and outputs stereo sample pairs.
class Stereo_Buffer : public Multi_Buffer {
public:
	
	// Buffers used for all channels
	Blip_Buffer* center()   { return &bufs [2]; }
	Blip_Buffer* left()     { return &bufs [0]; }
	Blip_Buffer* right()    { return &bufs [1]; }
	
// Implementation
public:
	Stereo_Buffer();
	~Stereo_Buffer();
	virtual blargg_err_t set_sample_rate( int, int msec = blip_default_length );
	virtual void clock_rate( int );
	virtual void bass_freq( int );
	virtual void clear();
	virtual channel_t channel( int )            { return chan; }
	virtual void end_frame( blip_time_t );
	virtual int samples_avail() const           { return (bufs [0].samples_avail() - mixer.samples_read) * 2; }
	virtual int read_samples( blip_sample_t [], int );
	
private:
	enum { bufs_size = 3 };
	typedef Tracked_Blip_Buffer buf_t;
	buf_t bufs [bufs_size];
	Stereo_Mixer mixer;
	channel_t chan;
	int samples_avail_;
};

=======
	unsigned channels_changed_count_;
	long sample_rate_;
	int length_;
	int const samples_per_frame_;
};

// Uses a single buffer and outputs mono samples.
class Mono_Buffer : public Multi_Buffer {
	Blip_Buffer buf;
	channel_t chan;
public:
	// Buffer used for all channels
	Blip_Buffer* center() { return &buf; }

public:
	Mono_Buffer();
	~Mono_Buffer();
	blargg_err_t set_sample_rate( long rate, int msec = blip_default_length ) override;
	void clock_rate( uint32_t rate ) override { buf.clock_rate( rate ); }
	void bass_freq( int freq ) override { buf.bass_freq( freq ); }
	void clear() override { buf.clear(); }
	long samples_avail() const override { return buf.samples_avail(); }
	long read_samples( blip_sample_t* p, long s ) override { return buf.read_samples( p, s ); }
	channel_t channel( int, int ) override { return chan; }
	void end_frame( blip_time_t t ) override { buf.end_frame( t ); }
};

// Uses three buffers (one for center) and outputs stereo sample pairs.
class Stereo_Buffer : public Multi_Buffer {
public:

	// Buffers used for all channels
	Blip_Buffer* center()       { return &bufs [0]; }
	Blip_Buffer* left()         { return &bufs [1]; }
	Blip_Buffer* right()        { return &bufs [2]; }

public:
	Stereo_Buffer();
	~Stereo_Buffer();
	blargg_err_t set_sample_rate( long, int msec = blip_default_length ) override;
	void clock_rate( uint32_t ) override;
	void bass_freq( int ) override;
	void clear() override;
	channel_t channel( int, int ) override { return chan; }
	void end_frame( blip_time_t ) override;

	long samples_avail() const override { return bufs [0].samples_avail() * 2; }
	long read_samples( blip_sample_t*, long ) override;

private:
	enum { buf_count = 3 };
	Blip_Buffer bufs [buf_count];
	channel_t chan;
	int stereo_added;
	int was_stereo;

	void mix_stereo_no_center( blip_sample_t*, int32_t );
	void mix_stereo( blip_sample_t*, int32_t );
	void mix_mono( blip_sample_t*, int32_t );
};
>>>>>>> db7344ebf (abc)

// Silent_Buffer generates no samples, useful where no sound is wanted
class Silent_Buffer : public Multi_Buffer {
	channel_t chan;
public:
	Silent_Buffer();
<<<<<<< HEAD
	virtual blargg_err_t set_sample_rate( int rate, int msec = blip_default_length );
	virtual void clock_rate( int )                  { }
	virtual void bass_freq( int )                   { }
	virtual void clear()                            { }
	virtual channel_t channel( int )                { return chan; }
	virtual void end_frame( blip_time_t )           { }
	virtual int samples_avail() const               { return 0; }
	virtual int read_samples( blip_sample_t [], int ) { return 0; }
};


inline blargg_err_t Multi_Buffer::set_sample_rate( int rate, int msec )
{
	sample_rate_ = rate;
	length_ = msec;
	return blargg_ok;
}

inline int  Multi_Buffer::samples_per_frame() const             { return samples_per_frame_; }
inline int  Multi_Buffer::sample_rate() const                   { return sample_rate_; }
inline int  Multi_Buffer::length() const                        { return length_; }
inline void Multi_Buffer::clock_rate( int )                     { }
inline void Multi_Buffer::bass_freq( int )                      { }
inline void Multi_Buffer::clear()                               { }
inline void Multi_Buffer::end_frame( blip_time_t )              { }
inline int  Multi_Buffer::read_samples( blip_sample_t [], int ) { return 0; }
inline int  Multi_Buffer::samples_avail() const                 { return 0; }

inline blargg_err_t Multi_Buffer::set_channel_count( int n, int const types [] )
{
	channel_count_ = n;
	channel_types_ = types;
	return blargg_ok;
}

inline blargg_err_t Silent_Buffer::set_sample_rate( int rate, int msec )
=======
	blargg_err_t set_sample_rate( long rate, int msec = blip_default_length ) override;
	void clock_rate( uint32_t ) override { }
	void bass_freq( int ) override { }
	void clear() override { }
	channel_t channel( int, int ) override { return chan; }
	void end_frame( blip_time_t ) override { }
	long samples_avail() const override { return 0; }
	long read_samples( blip_sample_t*, long ) override { return 0; }
};


inline blargg_err_t Multi_Buffer::set_sample_rate( long rate, int msec )
{
	sample_rate_ = rate;
	length_ = msec;
	return 0;
}

inline blargg_err_t Silent_Buffer::set_sample_rate( long rate, int msec )
>>>>>>> db7344ebf (abc)
{
	return Multi_Buffer::set_sample_rate( rate, msec );
}

<<<<<<< HEAD
=======
inline int Multi_Buffer::samples_per_frame() const { return samples_per_frame_; }

inline long Multi_Buffer::sample_rate() const { return sample_rate_; }

inline int Multi_Buffer::length() const { return length_; }

>>>>>>> db7344ebf (abc)
#endif
