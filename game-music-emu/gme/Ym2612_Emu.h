<<<<<<< HEAD
// YM2612 FM sound chip emulator

// Game_Music_Emu $vers
#ifndef YM2612_EMU_H
#define YM2612_EMU_H

// Gens is buggy and inaccurate, but faster
// #define USE_GENS
#ifdef USE_GENS
struct Ym2612_Impl;
#else
typedef void Ym2612_Impl;
#endif

class Ym2612_Emu  {
	Ym2612_Impl* impl;
public:
	Ym2612_Emu() { impl = 0; }
	~Ym2612_Emu();
	
	// Sets sample rate and chip clock rate, in Hz. Returns non-zero
	// if error. If clock_rate=0, uses sample_rate*144
	const char* set_rate( double sample_rate, double clock_rate = 0 );
	
	// Resets to power-up state
	void reset();
	
	// Mutes voice n if bit n (1 << n) of mask is set
	enum { channel_count = 6 };
	void mute_voices( int mask );
	
	// Writes addr to register 0 then data to register 1
	void write0( int addr, int data );
	
	// Writes addr to register 2 then data to register 3
	void write1( int addr, int data );
	
	// Runs and adds pair_count*2 samples into current output buffer contents
	typedef short sample_t;
	enum { out_chan_count = 2 }; // stereo
	void run( int pair_count, sample_t* out );
};

#endif
=======
// YM2612 FM sound chip emulator interface

// Game_Music_Emu https://bitbucket.org/mpyne/game-music-emu/

#if !defined(VGM_YM2612_GENS) && !defined(VGM_YM2612_NUKED) && !defined(VGM_YM2612_MAME)
#define VGM_YM2612_NUKED
#endif

#ifdef VGM_YM2612_GENS // LGPL v2.1+ license
# if defined(VGM_YM2612_NUKED) || defined(VGM_YM2612_MAME)
#   error Only one of VGM_YM2612_GENS, VGM_YM2612_NUKED or VGM_YM2612_MAME can be defined
# endif
#include "Ym2612_GENS.h"
typedef Ym2612_GENS_Emu Ym2612_Emu;
#endif

#ifdef VGM_YM2612_NUKED // LGPL v2.1+ license
# if defined(VGM_YM2612_GENS) || defined(VGM_YM2612_MAME)
#   error Only one of VGM_YM2612_GENS, VGM_YM2612_NUKED or VGM_YM2612_MAME can be defined
# endif
#include "Ym2612_Nuked.h"
typedef Ym2612_Nuked_Emu Ym2612_Emu;
#endif

#ifdef VGM_YM2612_MAME // GPL v2+ license
# if defined(VGM_YM2612_GENS) || defined(VGM_YM2612_NUKED)
#   error Only one of VGM_YM2612_GENS, VGM_YM2612_NUKED or VGM_YM2612_MAME can be defined
# endif
#include "Ym2612_MAME.h"
typedef Ym2612_MAME_Emu Ym2612_Emu;
#endif

>>>>>>> db7344ebf (abc)
