<<<<<<< HEAD
// Combination of Fir_Resampler and Stereo_Buffer mixing. Used by Sega FM emulators.

// Game_Music_Emu $vers
#ifndef DUAL_RESAMPLER_H
#define DUAL_RESAMPLER_H

#include "Multi_Buffer.h"

#if GME_VGM_FAST_RESAMPLER
	#include "Downsampler.h"
	typedef Downsampler Dual_Resampler_Downsampler;
#else
	#include "Fir_Resampler.h"
	typedef Fir_Resampler_Norm Dual_Resampler_Downsampler;
#endif

class Dual_Resampler {
public:
	typedef short dsample_t;
	
	blargg_err_t setup( double oversample, double rolloff, double gain );
	double rate() const { return resampler.rate(); }
	blargg_err_t reset( int max_pairs );
	void resize( int pairs_per_frame );
	void clear();
	
    void dual_play( int count, dsample_t out [], Stereo_Buffer&, Stereo_Buffer** secondary_buf_set = NULL, int secondary_buf_set_count = 0 );
	
	blargg_callback<int (*)( void*, blip_time_t, int, dsample_t* )> set_callback;

// Implementation
public:
	Dual_Resampler();
	~Dual_Resampler();

private:
	enum { gain_bits = 14 };
=======
// Combination of Fir_Resampler and Blip_Buffer mixing. Used by Sega FM emulators.

// Game_Music_Emu https://bitbucket.org/mpyne/game-music-emu/
#ifndef DUAL_RESAMPLER_H
#define DUAL_RESAMPLER_H

#include "Fir_Resampler.h"
#include "Blip_Buffer.h"

class Dual_Resampler {
public:
	Dual_Resampler();
	virtual ~Dual_Resampler();

	typedef short dsample_t;

	double setup( double oversample, double rolloff, double gain );
	blargg_err_t reset( int max_pairs );
	void resize( int pairs_per_frame );
	void clear();

	void dual_play( long count, dsample_t* out, Blip_Buffer& );

protected:
	virtual int play_frame( blip_time_t, int pcm_count, dsample_t* pcm_out ) = 0;
private:

>>>>>>> db7344ebf (abc)
	blargg_vector<dsample_t> sample_buf;
	int sample_buf_size;
	int oversamples_per_frame;
	int buf_pos;
<<<<<<< HEAD
	int buffered;
	int resampler_size;
	int gain_;
	
	Dual_Resampler_Downsampler resampler;
    void mix_samples( Stereo_Buffer&, dsample_t [], int, Stereo_Buffer**, int );
	void mix_mono( Stereo_Buffer&, dsample_t [], int );
	void mix_stereo( Stereo_Buffer&, dsample_t [], int );
	void mix_extra_mono( Stereo_Buffer&, dsample_t [], int );
    void mix_extra_stereo( Stereo_Buffer&, dsample_t [], int );
    int play_frame_( Stereo_Buffer&, dsample_t [], Stereo_Buffer**, int );
};

inline blargg_err_t Dual_Resampler::setup( double oversample, double rolloff, double gain )
{
	gain_ = (int) ((1 << gain_bits) * gain);
	return resampler.set_rate( oversample );
=======
	int resampler_size;

	Fir_Resampler<12> resampler;
	void mix_samples( Blip_Buffer&, dsample_t* );
	void play_frame_( Blip_Buffer&, dsample_t* );
};

inline double Dual_Resampler::setup( double oversample, double rolloff, double gain )
{
	return resampler.time_ratio( oversample, rolloff, gain * 0.5 );
}

inline void Dual_Resampler::clear()
{
	buf_pos = sample_buf_size;
	resampler.clear();
>>>>>>> db7344ebf (abc)
}

#endif
