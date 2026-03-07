// Sinclair Spectrum AY music file emulator

<<<<<<< HEAD
// Game_Music_Emu $vers
=======
// Game_Music_Emu https://bitbucket.org/mpyne/game-music-emu/
>>>>>>> db7344ebf (abc)
#ifndef AY_EMU_H
#define AY_EMU_H

#include "Classic_Emu.h"
<<<<<<< HEAD
#include "Ay_Core.h"

class Ay_Emu : public Classic_Emu {
public:
	// AY file header
	struct header_t
	{
		enum { size = 0x14 };
		
		byte tag        [8];
		byte vers;
		byte player;
		byte unused     [2];
		byte author     [2];
		byte comment    [2];
=======
#include "Ay_Apu.h"
#include "Ay_Cpu.h"

class Ay_Emu : private Ay_Cpu, public Classic_Emu {
	typedef Ay_Cpu cpu;
public:
	// AY file header
	enum { header_size = 0x14 };
	struct header_t
	{
		byte tag [8];
		byte vers;
		byte player;
		byte unused [2];
		byte author [2];
		byte comment [2];
>>>>>>> db7344ebf (abc)
		byte max_track;
		byte first_track;
		byte track_info [2];
	};
<<<<<<< HEAD
	
	static gme_type_t static_type() { return gme_ay_type; }

// Implementation
public:
	Ay_Emu();
	~Ay_Emu();

	struct file_t {
		header_t const* header;
		byte const* tracks;
		byte const* end;    // end of file data
	};

	blargg_err_t hash_( Hash_Function& out ) const;
	
protected:
	virtual blargg_err_t track_info_( track_info_t*, int track ) const;
	virtual blargg_err_t load_mem_( byte const [], int );
	virtual blargg_err_t start_track_( int );
	virtual blargg_err_t run_clocks( blip_time_t&, int );
	virtual void set_tempo_( double );
	virtual void set_voice( int, Blip_Buffer*, Blip_Buffer*, Blip_Buffer* );
	virtual void update_eq( blip_eq_t const& );

private:
	file_t file;
	Ay_Core core;
	
	void enable_cpc();
	static void enable_cpc_( void* data );
=======

	static gme_type_t static_type() { return gme_ay_type; }
public:
	Ay_Emu();
	~Ay_Emu();
	struct file_t {
		header_t const* header;
		byte const* end;
		byte const* tracks;
	};
protected:
	blargg_err_t track_info_( track_info_t*, int track ) const;
	blargg_err_t load_mem_( byte const*, long );
	blargg_err_t start_track_( int );
	blargg_err_t run_clocks( blip_time_t&, int );
	void set_tempo_( double );
	void set_voice( int, Blip_Buffer*, Blip_Buffer*, Blip_Buffer* );
	void update_eq( blip_eq_t const& );
private:
	file_t file;

	cpu_time_t play_period;
	cpu_time_t next_play;
	Blip_Buffer* beeper_output;
	int beeper_delta;
	int last_beeper;
	int apu_addr;
	int cpc_latch;
	bool spectrum_mode;
	bool cpc_mode;

	// large items
	struct {
		byte padding1 [0x100];
		byte ram [0x10000 + 0x100];
	} mem;
	Ay_Apu apu;
	friend void ay_cpu_out( Ay_Cpu*, cpu_time_t, unsigned addr, int data );
	void cpu_out_misc( cpu_time_t, unsigned addr, int data );
>>>>>>> db7344ebf (abc)
};

#endif
