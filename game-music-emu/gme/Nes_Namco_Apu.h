// Namco 106 sound chip emulator

<<<<<<< HEAD
// Nes_Snd_Emu $vers
=======
// Nes_Snd_Emu 0.1.8
>>>>>>> db7344ebf (abc)
#ifndef NES_NAMCO_APU_H
#define NES_NAMCO_APU_H

#include "blargg_common.h"
#include "Blip_Buffer.h"

struct namco_state_t;

class Nes_Namco_Apu {
public:
	// See Nes_Apu.h for reference.
	void volume( double );
	void treble_eq( const blip_eq_t& );
<<<<<<< HEAD
	void set_output( Blip_Buffer* );
	enum { osc_count = 8 };
	void set_output( int index, Blip_Buffer* );
	void reset();
	void end_frame( blip_time_t );
	
	// Read/write data register is at 0x4800
	enum { data_reg_addr = 0x4800 };
	void write_data( blip_time_t, int );
	int read_data();
	
	// Write-only address register is at 0xF800
	enum { addr_reg_addr = 0xF800 };
	void write_addr( int );
	
	// to do: implement save/restore
	void save_state( namco_state_t* out ) const;
	void load_state( namco_state_t const& );
	
=======
	void output( Blip_Buffer* );
	static const int osc_count = 8;
	void osc_output( int index, Blip_Buffer* );
	void reset();
	void end_frame( blip_time_t );

	// Read/write data register is at 0x4800
	static const unsigned int data_reg_addr = 0x4800;
	void write_data( blip_time_t, int );
	int read_data();

	// Write-only address register is at 0xF800
	static const unsigned int addr_reg_addr = 0xF800;
	void write_addr( int );

	// to do: implement save/restore
	void save_state( namco_state_t* out ) const;
	void load_state( namco_state_t const& );

>>>>>>> db7344ebf (abc)
public:
	Nes_Namco_Apu();
	BLARGG_DISABLE_NOTHROW
private:
	// noncopyable
	Nes_Namco_Apu( const Nes_Namco_Apu& );
	Nes_Namco_Apu& operator = ( const Nes_Namco_Apu& );
<<<<<<< HEAD
	
	struct Namco_Osc {
		int delay;
=======

	struct Namco_Osc {
		int32_t delay;
>>>>>>> db7344ebf (abc)
		Blip_Buffer* output;
		short last_amp;
		short wave_pos;
	};
<<<<<<< HEAD
	
	Namco_Osc oscs [osc_count];
	
	blip_time_t last_time;
	int addr_reg;
	
	enum { reg_count = 0x80 };
	BOOST::uint8_t reg [reg_count];
	Blip_Synth_Norm synth;
	
	BOOST::uint8_t& access();
=======

	Namco_Osc oscs [osc_count];

	blip_time_t last_time;
	int addr_reg;

	static const int reg_count = 0x80;
	uint8_t reg [reg_count];
	Blip_Synth<blip_good_quality,15> synth;

	uint8_t& access();
>>>>>>> db7344ebf (abc)
	void run_until( blip_time_t );
};
/*
struct namco_state_t
{
<<<<<<< HEAD
	BOOST::uint8_t regs [0x80];
	BOOST::uint8_t addr;
	BOOST::uint8_t unused;
	BOOST::uint8_t positions [8];
	BOOST::uint32_t delays [8];
};
*/

inline BOOST::uint8_t& Nes_Namco_Apu::access()
=======
	uint8_t regs [0x80];
	uint8_t addr;
	uint8_t unused;
	uint8_t positions [8];
	uint32_t delays [8];
};
*/

inline uint8_t& Nes_Namco_Apu::access()
>>>>>>> db7344ebf (abc)
{
	int addr = addr_reg & 0x7F;
	if ( addr_reg & 0x80 )
		addr_reg = (addr + 1) | 0x80;
	return reg [addr];
}

<<<<<<< HEAD
inline void Nes_Namco_Apu::volume( double v ) { synth.volume( 0.10 / osc_count / 15 * v ); }
=======
inline void Nes_Namco_Apu::volume( double v ) { synth.volume( 0.10 / osc_count * v ); }
>>>>>>> db7344ebf (abc)

inline void Nes_Namco_Apu::treble_eq( const blip_eq_t& eq ) { synth.treble_eq( eq ); }

inline void Nes_Namco_Apu::write_addr( int v ) { addr_reg = v; }

inline int Nes_Namco_Apu::read_data() { return access(); }

<<<<<<< HEAD
inline void Nes_Namco_Apu::set_output( int i, Blip_Buffer* buf )
=======
inline void Nes_Namco_Apu::osc_output( int i, Blip_Buffer* buf )
>>>>>>> db7344ebf (abc)
{
	assert( (unsigned) i < osc_count );
	oscs [i].output = buf;
}

inline void Nes_Namco_Apu::write_data( blip_time_t time, int data )
{
	run_until( time );
	access() = data;
}

#endif
