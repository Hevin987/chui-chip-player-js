// YM2413 FM sound chip emulator interface

<<<<<<< HEAD
// Game_Music_Emu $vers
#ifndef YM2413_EMU_H
#define YM2413_EMU_H

struct OPLL;

class Ym2413_Emu  {
	void* opll;
public:
	Ym2413_Emu();
	~Ym2413_Emu();
	
	static bool supported() { return true; }
	
	// Sets output sample rate and chip clock rates, in Hz. Returns non-zero
	// if error.
	int set_rate( int sample_rate, int clock_rate );
	
	// Resets to power-up state
	void reset();
	
	// Mutes voice n if bit n (1 << n) of mask is set
	enum { channel_count = 14 };
	void mute_voices( int mask );
	
	// Writes data to addr
	void write( int addr, int data );
	
	// Runs and writes pair_count*2 samples to output
=======
// Game_Music_Emu https://bitbucket.org/mpyne/game-music-emu/
#ifndef YM2413_EMU_H
#define YM2413_EMU_H

class Ym2413_Emu  {
	struct OPLL* opll;
public:
	Ym2413_Emu();
	~Ym2413_Emu();

	// Set output sample rate and chip clock rates, in Hz. Returns non-zero
	// if error.
	int set_rate( double sample_rate, double clock_rate );

	// Reset to power-up state
	void reset();

	// Mute voice n if bit n (1 << n) of mask is set
	enum { channel_count = 14 };
	void mute_voices( int mask );

	// Write 'data' to 'addr'
	void write( int addr, int data );

	// Run and write pair_count samples to output
>>>>>>> db7344ebf (abc)
	typedef short sample_t;
	enum { out_chan_count = 2 }; // stereo
	void run( int pair_count, sample_t* out );
};

#endif
