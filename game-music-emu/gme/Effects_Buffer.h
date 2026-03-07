<<<<<<< HEAD
// Multi-channel effects buffer with echo and individual panning for each channel

// Game_Music_Emu $vers
=======
// Multi-channel effects buffer with panning, echo and reverb

// Game_Music_Emu https://bitbucket.org/mpyne/game-music-emu/
>>>>>>> db7344ebf (abc)
#ifndef EFFECTS_BUFFER_H
#define EFFECTS_BUFFER_H

#include "Multi_Buffer.h"

<<<<<<< HEAD
// See Simple_Effects_Buffer (below) for a simpler interface

class Effects_Buffer : public Multi_Buffer {
public:
	// To reduce memory usage, fewer buffers can be used (with a best-fit
	// approach if there are too few), and maximum echo delay can be reduced
	Effects_Buffer( int max_bufs = 32, int echo_size = 24 * 1024 );
	
	struct pan_vol_t
	{
		float vol; // 0.0 = silent, 0.5 = half volume, 1.0 = normal
		float pan; // -1.0 = left, 0.0 = center, +1.0 = right
	};
	
	// Global configuration
	struct config_t
	{
		bool enabled; // false = disable all effects
		
		// Current sound is echoed at adjustable left/right delay,
		// with reduced treble and volume (feedback). 
		float treble;   // 1.0 = full treble, 0.1 = very little, 0.0 = silent
		int delay [2];  // left, right delays (msec)
		float feedback; // 0.0 = no echo, 0.5 = each echo half previous, 1.0 = cacophony
		pan_vol_t side_chans [2]; // left and right side channel volume and pan
	};
	config_t& config() { return config_; }
	
	// Limits of delay (msec)
	int min_delay() const;
	int max_delay() const;
	
	// Per-channel configuration. Two or more channels with matching parameters are
	// optimized to internally use the same buffer.
	struct chan_config_t : pan_vol_t
	{
		// (inherited from pan_vol_t)
		//float vol;        // these only affect center channel
		//float pan;
		bool surround;  // if true, negates left volume to put sound in back
		bool echo;      // false = channel doesn't have any echo
	};
	chan_config_t& chan_config( int i ) { return chans [i + extra_chans].cfg; }
	
	// Applies any changes made to config() and chan_config()
	virtual void apply_config();
	
// Implementation
public:
	~Effects_Buffer();
	blargg_err_t set_sample_rate( int samples_per_sec, int msec = blip_default_length );
	blargg_err_t set_channel_count( int, int const* = NULL );
	void clock_rate( int );
	void bass_freq( int );
	void clear();
	channel_t channel( int );
	void end_frame( blip_time_t );
	int read_samples( blip_sample_t [], int );
	int samples_avail() const { return (bufs [0].samples_avail() - mixer.samples_read) * 2; }
	enum { stereo = 2 };
	typedef int fixed_t;

protected:
	enum { extra_chans = stereo * stereo };

private:
	config_t config_;
	int clock_rate_;
	int bass_freq_;
	
	int echo_size;
	
	struct chan_t
	{
		fixed_t vol [stereo];
		chan_config_t cfg;
		channel_t channel;
	};
	blargg_vector<chan_t> chans;
	
	struct buf_t : Tracked_Blip_Buffer
	{
		// nasty: Blip_Buffer has something called fixed_t
		Effects_Buffer::fixed_t vol [stereo];
		bool echo;
		
		void* operator new ( size_t, void* p ) { return p; }
		void operator delete ( void* ) { }
		
		~buf_t() { }
	};
	buf_t* bufs;
	int bufs_size;
	int bufs_max; // bufs_size <= bufs_max, to limit memory usage
	Stereo_Mixer mixer;
	
	struct {
		int delay [stereo];
		fixed_t treble;
		fixed_t feedback;
		fixed_t low_pass [stereo];
	} s;
	
	blargg_vector<fixed_t> echo;
	int echo_pos;
	
	bool no_effects;
	bool no_echo;
	
	void assign_buffers();
	void clear_echo();
	void mix_effects( blip_sample_t out [], int pair_count );
	blargg_err_t new_bufs( int size );
	void delete_bufs();
};

// Simpler interface and lower memory usage
class Simple_Effects_Buffer : public Effects_Buffer {
public:
	struct config_t
	{
		bool enabled;   // false = disable all effects
		
		float echo;     // 0.0 = none, 1.0 = lots
		float stereo;   // 0.0 = channels in center, 1.0 = channels on left/right
		bool surround;  // true = put some channels in back
	};
	config_t& config() { return config_; }
	
	// Applies any changes made to config()
	void apply_config();
	
// Implementation
public:
	Simple_Effects_Buffer();
private:
	config_t config_;
	void chan_config(); // hide
=======
#include <vector>

// Effects_Buffer uses several buffers and outputs stereo sample pairs.
class Effects_Buffer : public Multi_Buffer {
public:
	// nVoices indicates the number of voices for which buffers will be allocated
	// to make Effects_Buffer work as "mix everything to one", nVoices will be 1
	// If center_only is true, only center buffers are created and
	// less memory is used.
	Effects_Buffer( int nVoices = 1, bool center_only = false );

	// Channel  Effect    Center Pan
	// ---------------------------------
	//    0,5    reverb       pan_1
	//    1,6    reverb       pan_2
	//    2,7    echo         -
	//    3      echo         -
	//    4      echo         -

	// Channel configuration
	struct config_t {
		double pan_1;           // -1.0 = left, 0.0 = center, 1.0 = right
		double pan_2;
		double echo_delay;      // msec
		double echo_level;      // 0.0 to 1.0
		double reverb_delay;    // msec
		double delay_variance;  // difference between left/right delays (msec)
		double reverb_level;    // 0.0 to 1.0
		bool effects_enabled;   // if false, use optimized simple mixer
		config_t();
	};

	// Set configuration of buffer
	virtual void config( const config_t& );
	void set_depth( double );

public:
	~Effects_Buffer();
	blargg_err_t set_sample_rate( long samples_per_sec, int msec = blip_default_length ) noexcept override;
	void clock_rate( uint32_t ) override;
	void bass_freq( int ) override;
	void clear() override;
	channel_t channel( int, int ) override;
	void end_frame( blip_time_t ) override;
	long read_samples( blip_sample_t*, long ) override;
	long samples_avail() const override;
private:
	typedef long fixed_t;
	int max_voices;
	enum { max_buf_count = 7 };
	std::vector<Blip_Buffer> bufs;
	enum { chan_types_count = 3 };
	std::vector<channel_t> chan_types;
	config_t config_;
	long stereo_remain;
	long effect_remain;
	int buf_count;
	bool effects_enabled;

	std::vector<std::vector<blip_sample_t> > reverb_buf;
	std::vector<std::vector<blip_sample_t> > echo_buf;
	std::vector<int> reverb_pos;
	std::vector<int> echo_pos;

	struct {
		fixed_t pan_1_levels [2];
		fixed_t pan_2_levels [2];
		int echo_delay_l;
		int echo_delay_r;
		fixed_t echo_level;
		int reverb_delay_l;
		int reverb_delay_r;
		fixed_t reverb_level;
	} chans;

	void mix_mono( blip_sample_t*, int32_t );
	void mix_stereo( blip_sample_t*, int32_t );
	void mix_enhanced( blip_sample_t*, int32_t );
	void mix_mono_enhanced( blip_sample_t*, int32_t );
>>>>>>> db7344ebf (abc)
};

#endif
