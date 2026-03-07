<<<<<<< HEAD
// NES CPU emulator

// $package
=======
// NES 6502 CPU emulator

// Game_Music_Emu https://bitbucket.org/mpyne/game-music-emu/
>>>>>>> db7344ebf (abc)
#ifndef NES_CPU_H
#define NES_CPU_H

#include "blargg_common.h"

<<<<<<< HEAD
class Nes_Cpu {
public:
	typedef BOOST::uint8_t byte;
	typedef int time_t;
	typedef int addr_t;
	enum { future_time = INT_MAX/2 + 1 };
	
	// Clears registers and maps all pages to unmapped_page
	void reset( void const* unmapped_page = NULL );
	
	// Maps code memory (memory accessed via the program counter). Start and size
	// must be multiple of page_size. If mirror_size is non-zero, the first
	// mirror_size bytes are repeated over the range. mirror_size must be a
	// multiple of page_size.
	enum { page_bits = 11 };
	enum { page_size = 1 << page_bits };
	void map_code( addr_t start, int size, void const* code, int mirror_size = 0 );
	
	// Accesses emulated memory as CPU does
	byte const* get_code( addr_t ) const;
	
	// NES 6502 registers. NOT kept updated during emulation.
	struct registers_t {
		BOOST::uint16_t pc;
		byte a;
		byte x;
		byte y;
		byte flags;
		byte sp;
	};
	registers_t r;
	
	// Time of beginning of next instruction to be executed
	time_t time() const             { return cpu_state->time + cpu_state->base; }
	void set_time( time_t t )       { cpu_state->time = t - cpu_state->base; }
	void adjust_time( int delta )   { cpu_state->time += delta; }
	
	// Clocks past end (negative if before)
	int time_past_end() const       { return cpu_state->time; }
	
	// Time of next IRQ
	time_t irq_time() const         { return irq_time_; }
	void set_irq_time( time_t );
	
	// Emulation stops once time >= end_time
	time_t end_time() const         { return end_time_; }
	void set_end_time( time_t );
	
	// Number of unimplemented instructions encountered and skipped
	void clear_error_count()        { error_count_ = 0; }
	unsigned error_count() const    { return error_count_; }
	void count_error()              { error_count_++; }
	
	// Unmapped page should be filled with this
	enum { halt_opcode = 0x22 };
	
	enum { irq_inhibit_mask = 0x04 };
	
	// Can read this many bytes past end of a page
	enum { cpu_padding = 8 };

private:
	// noncopyable
	Nes_Cpu( const Nes_Cpu& );
	Nes_Cpu& operator = ( const Nes_Cpu& );


// Implementation
public:
	Nes_Cpu() { cpu_state = &cpu_state_; }
	enum { page_count = 0x10000 >> page_bits };
	
	struct cpu_state_t {
		byte const* code_map [page_count + 1];
		time_t base;
		int time;
	};
	cpu_state_t* cpu_state; // points to cpu_state_ or a local copy
	cpu_state_t cpu_state_;
	time_t irq_time_;
	time_t end_time_;
	unsigned error_count_;
	
private:
	void set_code_page( int, void const* );
	inline void update_end_time( time_t end, time_t irq );
};

#define NES_CPU_PAGE( addr ) ((unsigned) (addr) >> Nes_Cpu::page_bits)

#if BLARGG_NONPORTABLE
	#define NES_CPU_OFFSET( addr ) (addr)
#else
	#define NES_CPU_OFFSET( addr ) ((addr) & (Nes_Cpu::page_size - 1))
#endif

inline BOOST::uint8_t const* Nes_Cpu::get_code( addr_t addr ) const
{
	return cpu_state_.code_map [NES_CPU_PAGE( addr )] + NES_CPU_OFFSET( addr );
}

inline void Nes_Cpu::update_end_time( time_t end, time_t irq )
{
	if ( end > irq && !(r.flags & irq_inhibit_mask) )
		end = irq;
	
	cpu_state->time += cpu_state->base - end;
	cpu_state->base = end;
}

inline void Nes_Cpu::set_irq_time( time_t t )
{
	irq_time_ = t;
	update_end_time( end_time_, t );
}

inline void Nes_Cpu::set_end_time( time_t t )
{
	end_time_ = t;
	update_end_time( t, irq_time_ );
}   
=======
typedef int32_t nes_time_t; // clock cycle count
typedef unsigned nes_addr_t; // 16-bit address
enum { future_nes_time = INT_MAX / 2 + 1 };

class Nes_Cpu {
public:
	// Clear registers, map low memory and its three mirrors to address 0,
	// and mirror unmapped_page in remaining memory
	void reset( void const* unmapped_page = 0 );

	// Map code memory (memory accessed via the program counter). Start and size
	// must be multiple of page_size. If mirror is true, repeats code page
	// throughout address range.
	enum { page_size = 0x800 };
	void map_code( nes_addr_t start, unsigned size, void const* code, bool mirror = false );

	// Access emulated memory as CPU does
	uint8_t const* get_code( nes_addr_t );

	// 2KB of RAM at address 0
	uint8_t low_mem [0x800];

	// NES 6502 registers. Not kept updated during a call to run().
	struct registers_t {
		uint16_t pc;
		uint8_t a;
		uint8_t x;
		uint8_t y;
		uint8_t status;
		uint8_t sp;
	};
	registers_t r;

	// Set end_time and run CPU from current time. Returns true if execution
	// stopped due to encountering bad_opcode.
	bool run( nes_time_t end_time );

	// Time of beginning of next instruction to be executed
	nes_time_t time() const             { return state->time + state->base; }
	void set_time( nes_time_t t )       { state->time = t - state->base; }
	void adjust_time( int delta )       { state->time += delta; }

	nes_time_t irq_time() const         { return irq_time_; }
	void set_irq_time( nes_time_t );

	nes_time_t end_time() const         { return end_time_; }
	void set_end_time( nes_time_t );

	// Number of undefined instructions encountered and skipped
	void clear_error_count()            { error_count_ = 0; }
	unsigned long error_count() const   { return error_count_; }

	// CPU invokes bad opcode handler if it encounters this
	enum { bad_opcode = 0xF2 };

public:
	Nes_Cpu() { state = &state_; }
	enum { page_bits = 11 };
	enum { page_count = 0x10000 >> page_bits };
	enum { irq_inhibit = 0x04 };
private:
	struct state_t {
		uint8_t const* code_map [page_count + 1];
		nes_time_t base;
		int time;
	};
	state_t* state; // points to state_ or a local copy within run()
	state_t state_;
	nes_time_t irq_time_;
	nes_time_t end_time_;
	unsigned long error_count_;

	void set_code_page( int, void const* );
	inline int update_end_time( nes_time_t end, nes_time_t irq );
};

inline uint8_t const* Nes_Cpu::get_code( nes_addr_t addr )
{
	return state->code_map [addr >> page_bits] + addr
	#if !BLARGG_NONPORTABLE
		% (unsigned) page_size
	#endif
	;
}

inline int Nes_Cpu::update_end_time( nes_time_t t, nes_time_t irq )
{
	if ( irq < t && !(r.status & irq_inhibit) ) t = irq;
	int delta = state->base - t;
	state->base = t;
	return delta;
}

inline void Nes_Cpu::set_irq_time( nes_time_t t )
{
	state->time += update_end_time( end_time_, (irq_time_ = t) );
}

inline void Nes_Cpu::set_end_time( nes_time_t t )
{
	state->time += update_end_time( (end_time_ = t), irq_time_ );
}
>>>>>>> db7344ebf (abc)

#endif
