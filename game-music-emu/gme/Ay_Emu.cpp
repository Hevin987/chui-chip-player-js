<<<<<<< HEAD
// Game_Music_Emu $vers. http://www.slack.net/~ant/
=======
// Game_Music_Emu https://bitbucket.org/mpyne/game-music-emu/
>>>>>>> db7344ebf (abc)

#include "Ay_Emu.h"

#include "blargg_endian.h"
<<<<<<< HEAD

/* Copyright (C) 2006-2009 Shay Green. This module is free software; you
=======
#include <string.h>

#include <algorithm> // min, max

/* Copyright (C) 2006 Shay Green. This module is free software; you
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
// TODO: probably don't need detailed errors as to why file is corrupt

int const spectrum_clock = 3546900; // 128K Spectrum
int const spectrum_period = 70908;

//int const spectrum_clock = 3500000; // 48K Spectrum
//int const spectrum_period = 69888;

int const cpc_clock = 2000000;

Ay_Emu::Ay_Emu()
{
	core.set_cpc_callback( enable_cpc_, this );
	set_type( gme_ay_type );
=======
static long const spectrum_clock = 3546900;
static long const cpc_clock      = 2000000;

static unsigned const ram_start = 0x4000;
static int const osc_count = Ay_Apu::osc_count + 1;

using std::min;
using std::max;

Ay_Emu::Ay_Emu()
{
	beeper_output = 0;
	set_type( gme_ay_type );

	static const char* const names [osc_count] = {
		"Wave 1", "Wave 2", "Wave 3", "Beeper"
	};
	set_voice_names( names );

	static int const types [osc_count] = {
		wave_type | 0, wave_type | 1, wave_type | 2, mixed_type | 0
	};
	set_voice_types( types );
>>>>>>> db7344ebf (abc)
	set_silence_lookahead( 6 );
}

Ay_Emu::~Ay_Emu() { }

// Track info

<<<<<<< HEAD
// Given pointer to 2-byte offset of data, returns pointer to data, or NULL if
// offset is 0 or there is less than min_size bytes of data available.
static byte const* get_data( Ay_Emu::file_t const& file, byte const ptr [], int min_size )
{
	int offset = (BOOST::int16_t) get_be16( ptr );
	int pos  = ptr - (byte const*) file.header;
	int size = file.end - (byte const*) file.header;
	assert( (unsigned) pos <= (unsigned) size - 2 );
	int limit = size - min_size;
	if ( limit < 0 || !offset || (unsigned) (pos + offset) > (unsigned) limit )
		return NULL;
	return ptr + offset;
}

static blargg_err_t parse_header( byte const in [], int size, Ay_Emu::file_t* out )
{
	typedef Ay_Emu::header_t header_t;
	if ( size < header_t::size )
		return blargg_err_file_type;
	
	out->header = (header_t const*) in;
	out->end    = in + size;
	header_t const& h = *(header_t const*) in;
	if ( memcmp( h.tag, "ZXAYEMUL", 8 ) )
		return blargg_err_file_type;
	
	out->tracks = get_data( *out, h.track_info, (h.max_track + 1) * 4 );
	if ( !out->tracks )
		return BLARGG_ERR( BLARGG_ERR_FILE_CORRUPT, "missing track data" );
	
	return blargg_ok;
=======
static byte const* get_data( Ay_Emu::file_t const& file, byte const* ptr, int min_size )
{
	long pos = ptr - (byte const*) file.header;
	long file_size = file.end - (byte const*) file.header;
	assert( (unsigned long) pos <= (unsigned long) file_size - 2 );
	int offset = (int16_t) get_be16( ptr );
	if ( !offset || uint32_t (pos + offset) > uint32_t (file_size - min_size) )
		return 0;
	return ptr + offset;
}

static blargg_err_t parse_header( byte const* in, long size, Ay_Emu::file_t* out )
{
	typedef Ay_Emu::header_t header_t;
	out->header = (header_t const*) in;
	out->end    = in + size;

	if ( size < Ay_Emu::header_size )
		return gme_wrong_file_type;

	header_t const& h = *(header_t const*) in;
	if ( memcmp( h.tag, "ZXAYEMUL", 8 ) )
		return gme_wrong_file_type;

	out->tracks = get_data( *out, h.track_info, (h.max_track + 1) * 4 );
	if ( !out->tracks )
		return "Missing track data";

	return 0;
>>>>>>> db7344ebf (abc)
}

static void copy_ay_fields( Ay_Emu::file_t const& file, track_info_t* out, int track )
{
	Gme_File::copy_field_( out->song, (char const*) get_data( file, file.tracks + track * 4, 1 ) );
	byte const* track_info = get_data( file, file.tracks + track * 4 + 2, 6 );
	if ( track_info )
<<<<<<< HEAD
		out->length = get_be16( track_info + 4 ) * (1000 / 50); // frames to msec
	
=======
		out->length = get_be16( track_info + 4 ) * (1000L / 50); // frames to msec

>>>>>>> db7344ebf (abc)
	Gme_File::copy_field_( out->author,  (char const*) get_data( file, file.header->author, 1 ) );
	Gme_File::copy_field_( out->comment, (char const*) get_data( file, file.header->comment, 1 ) );
}

<<<<<<< HEAD
static void hash_ay_file( Ay_Emu::file_t const& file, Gme_Info_::Hash_Function& out )
{
	out.hash_( &file.header->vers, sizeof(file.header->vers) );
	out.hash_( &file.header->player, sizeof(file.header->player) );
	out.hash_( &file.header->unused[0], sizeof(file.header->unused) );
	out.hash_( &file.header->max_track, sizeof(file.header->max_track) );
	out.hash_( &file.header->first_track, sizeof(file.header->first_track) );

	for ( unsigned i = 0; i <= file.header->max_track; i++ )
	{
		byte const* track_info = get_data( file, file.tracks + i * 4 + 2, 14 );
		if ( track_info )
		{
			out.hash_( track_info + 8, 2 );
			byte const* points = get_data( file, track_info + 10, 6 );
			if ( points ) out.hash_( points, 6 );

			byte const* blocks = get_data( file, track_info + 12, 8 );
			if ( blocks )
			{
				int addr = get_be16( blocks );

				while ( addr )
				{
					out.hash_( blocks, 4 );

					int len = get_be16( blocks + 2 );

					byte const* block = get_data( file, blocks + 4, len );
					if ( block ) out.hash_( block, len );

					blocks += 6;
					addr = get_be16( blocks );
				}
			}
		}
	}
}

blargg_err_t Ay_Emu::track_info_( track_info_t* out, int track ) const
{
	copy_ay_fields( file, out, track );
	return blargg_ok;
=======
blargg_err_t Ay_Emu::track_info_( track_info_t* out, int track ) const
{
	copy_ay_fields( file, out, track );
	return 0;
>>>>>>> db7344ebf (abc)
}

struct Ay_File : Gme_Info_
{
	Ay_Emu::file_t file;
<<<<<<< HEAD
	
	Ay_File() { set_type( gme_ay_type ); }
	
	blargg_err_t load_mem_( byte const begin [], int size )
	{
		RETURN_ERR( parse_header( begin, size, &file ) );
		set_track_count( file.header->max_track + 1 );
		return blargg_ok;
	}
	
	blargg_err_t track_info_( track_info_t* out, int track ) const
	{
		copy_ay_fields( file, out, track );
		return blargg_ok;
	}

	blargg_err_t hash_( Hash_Function& out ) const
	{
		hash_ay_file( file, out );
		return blargg_ok;
	}
};

static Music_Emu* new_ay_emu ()
{
	return BLARGG_NEW Ay_Emu;
}

static Music_Emu* new_ay_file()
{
	return BLARGG_NEW Ay_File;
}

gme_type_t_ const gme_ay_type [1] = {{
	"ZX Spectrum",
	0,
	&new_ay_emu,
	&new_ay_file,
	"AY",
	1
}};

// Setup

blargg_err_t Ay_Emu::load_mem_( byte const in [], int size )
{
	assert( offsetof (header_t,track_info [2]) == header_t::size );
	
	RETURN_ERR( parse_header( in, size, &file ) );
	set_track_count( file.header->max_track + 1 );
	
	if ( file.header->vers > 2 )
		set_warning( "Unknown file version" );
	
	int const osc_count = Ay_Apu::osc_count + 1; // +1 for beeper
	
	set_voice_count( osc_count );
	core.apu().volume( gain() );
	
	static const char* const names [osc_count] = {
		"Wave 1", "Wave 2", "Wave 3", "Beeper"
	};
	set_voice_names( names );
	
	static int const types [osc_count] = {
		wave_type+0, wave_type+1, wave_type+2, mixed_type+1
	};
	set_voice_types( types );
	
	return setup_buffer( spectrum_clock );
}
	
void Ay_Emu::update_eq( blip_eq_t const& eq )
{
	core.apu().treble_eq( eq );
=======

	Ay_File() { set_type( gme_ay_type ); }

	blargg_err_t load_mem_( byte const* begin, long size )
	{
		RETURN_ERR( parse_header( begin, size, &file ) );
		set_track_count( file.header->max_track + 1 );
		return 0;
	}

	blargg_err_t track_info_( track_info_t* out, int track ) const
	{
		copy_ay_fields( file, out, track );
		return 0;
	}
};

static Music_Emu* new_ay_emu () { return BLARGG_NEW Ay_Emu ; }
static Music_Emu* new_ay_file() { return BLARGG_NEW Ay_File; }

static gme_type_t_ const gme_ay_type_ = { "ZX Spectrum", 0, &new_ay_emu, &new_ay_file, "AY", 1 };
extern gme_type_t const gme_ay_type = &gme_ay_type_;

// Setup

blargg_err_t Ay_Emu::load_mem_( byte const* in, long size )
{
	blaarg_static_assert( offsetof (header_t,track_info [2]) == header_size, "AY Header layout incorrect!" );

	RETURN_ERR( parse_header( in, size, &file ) );
	set_track_count( file.header->max_track + 1 );

	if ( file.header->vers > 2 )
		set_warning( "Unknown file version" );

	set_voice_count( osc_count );
	apu.volume( gain() );

	return setup_buffer( spectrum_clock );
}

void Ay_Emu::update_eq( blip_eq_t const& eq )
{
	apu.treble_eq( eq );
>>>>>>> db7344ebf (abc)
}

void Ay_Emu::set_voice( int i, Blip_Buffer* center, Blip_Buffer*, Blip_Buffer* )
{
	if ( i >= Ay_Apu::osc_count )
<<<<<<< HEAD
		core.set_beeper_output( center );
	else
		core.apu().set_output( i, center );
}

void Ay_Emu::set_tempo_( double t )
{
	int p = spectrum_period;
	if ( clock_rate() != spectrum_clock )
		p = clock_rate() / 50;
	
	core.set_play_period( blip_time_t (p / t) );
=======
		beeper_output = center;
	else
		apu.osc_output( i, center );
}

// Emulation

void Ay_Emu::set_tempo_( double t )
{
	play_period = blip_time_t (clock_rate() / 50 / t);
>>>>>>> db7344ebf (abc)
}

blargg_err_t Ay_Emu::start_track_( int track )
{
	RETURN_ERR( Classic_Emu::start_track_( track ) );
<<<<<<< HEAD
	
	byte* const mem = core.mem();
	
	memset( mem + 0x0000, 0xC9, 0x100 ); // fill RST vectors with RET
	memset( mem + 0x0100, 0xFF, 0x4000 - 0x100 );
	memset( mem + core.ram_addr, 0x00, core.mem_size - core.ram_addr );

	// locate data blocks
	byte const* const data = get_data( file, file.tracks + track * 4 + 2, 14 );
	if ( !data )
		return BLARGG_ERR( BLARGG_ERR_FILE_CORRUPT, "file data missing" );
	
	byte const* const more_data = get_data( file, data + 10, 6 );
	if ( !more_data )
		return BLARGG_ERR( BLARGG_ERR_FILE_CORRUPT, "file data missing" );
	
	byte const* blocks = get_data( file, data + 12, 8 );
	if ( !blocks )
		return BLARGG_ERR( BLARGG_ERR_FILE_CORRUPT, "file data missing" );
	
	// initial addresses
	unsigned addr = get_be16( blocks );
	if ( !addr )
		return BLARGG_ERR( BLARGG_ERR_FILE_CORRUPT, "file data missing" );
	
	unsigned init = get_be16( more_data + 2 );
	if ( !init )
		init = addr;
	
=======

	memset( mem.ram + 0x0000, 0xC9, 0x100 ); // fill RST vectors with RET
	memset( mem.ram + 0x0100, 0xFF, 0x4000 - 0x100 );
	memset( mem.ram + ram_start, 0x00, sizeof mem.ram - ram_start );
	memset( mem.padding1, 0xFF, sizeof mem.padding1 );
	memset( mem.ram + 0x10000, 0xFF, sizeof mem.ram - 0x10000 );

	// locate data blocks
	byte const* const data = get_data( file, file.tracks + track * 4 + 2, 14 );
	if ( !data ) return "File data missing";

	byte const* const more_data = get_data( file, data + 10, 6 );
	if ( !more_data ) return "File data missing";

	byte const* blocks = get_data( file, data + 12, 8 );
	if ( !blocks ) return "File data missing";

	// initial addresses
	cpu::reset( mem.ram );
	r.sp = get_be16( more_data );
	r.b.a = r.b.b = r.b.d = r.b.h = data [8];
	r.b.flags = r.b.c = r.b.e = r.b.l = data [9];
	r.alt.w = r.w;
	r.ix = r.iy = r.w.hl;

	unsigned addr = get_be16( blocks );
	if ( !addr ) return "File data missing";

	unsigned init = get_be16( more_data + 2 );
	if ( !init )
		init = addr;

>>>>>>> db7344ebf (abc)
	// copy blocks into memory
	do
	{
		blocks += 2;
		unsigned len = get_be16( blocks ); blocks += 2;
<<<<<<< HEAD
		if ( addr + len > core.mem_size )
		{
			set_warning( "Bad data block size" );
			len = core.mem_size - addr;
		}
		check( len );
		byte const* in = get_data( file, blocks, 0 ); blocks += 2;
		if ( len > (unsigned) (file.end - in) )
		{
			set_warning( "File data missing" );
			len = file.end - in;
		}
		//dprintf( "addr: $%04X, len: $%04X\n", addr, len );
		if ( addr < core.ram_addr && addr >= 0x400 ) // several tracks use low data
			dprintf( "Block addr in ROM\n" );
		memcpy( mem + addr, in, len );
		
		if ( file.end - blocks < 8 )
		{
			set_warning( "File data missing" );
=======
		if ( addr + len > 0x10000 )
		{
			set_warning( "Bad data block size" );
			len = 0x10000 - addr;
		}
		check( len );
		byte const* in = get_data( file, blocks, 0 ); blocks += 2;
		if ( len > uint32_t (file.end - in) )
		{
			set_warning( "Missing file data" );
			len = file.end - in;
		}
		//debug_printf( "addr: $%04X, len: $%04X\n", addr, len );
		if ( addr < ram_start && addr >= 0x400 ) // several tracks use low data
			debug_printf( "Block addr in ROM\n" );
		memcpy( mem.ram + addr, in, len );

		if ( file.end - blocks < 8 )
		{
			set_warning( "Missing file data" );
>>>>>>> db7344ebf (abc)
			break;
		}
	}
	while ( (addr = get_be16( blocks )) != 0 );
<<<<<<< HEAD
	
=======

>>>>>>> db7344ebf (abc)
	// copy and configure driver
	static byte const passive [] = {
		0xF3,       // DI
		0xCD, 0, 0, // CALL init
		0xED, 0x5E, // LOOP: IM 2
		0xFB,       // EI
		0x76,       // HALT
		0x18, 0xFA  // JR LOOP
	};
	static byte const active [] = {
		0xF3,       // DI
		0xCD, 0, 0, // CALL init
		0xED, 0x56, // LOOP: IM 1
		0xFB,       // EI
		0x76,       // HALT
		0xCD, 0, 0, // CALL play
		0x18, 0xF7  // JR LOOP
	};
<<<<<<< HEAD
	memcpy( mem, passive, sizeof passive );
	int const play_addr = get_be16( more_data + 4 );
	if ( play_addr )
	{
		memcpy( mem, active, sizeof active );
		mem [ 9] = play_addr;
		mem [10] = play_addr >> 8;
	}
	mem [2] = init;
	mem [3] = init >> 8;
	
	mem [0x38] = 0xFB; // Put EI at interrupt vector (followed by RET)
	
	// start at spectrum speed
	change_clock_rate( spectrum_clock );
	set_tempo( tempo() );
	
	Ay_Core::registers_t r = { };
	r.sp = get_be16( more_data );
	r.b.a     = r.b.b = r.b.d = r.b.h = data [8];
	r.b.flags = r.b.c = r.b.e = r.b.l = data [9];
	r.alt.w = r.w;
	r.ix = r.iy = r.w.hl;
	
	core.start_track( r, play_addr );
	
	return blargg_ok;
=======
	memcpy( mem.ram, passive, sizeof passive );
	unsigned play_addr = get_be16( more_data + 4 );
	//debug_printf( "Play: $%04X\n", play_addr );
	if ( play_addr )
	{
		memcpy( mem.ram, active, sizeof active );
		mem.ram [ 9] = play_addr;
		mem.ram [10] = play_addr >> 8;
	}
	mem.ram [2] = init;
	mem.ram [3] = init >> 8;

	mem.ram [0x38] = 0xFB; // Put EI at interrupt vector (followed by RET)

	memcpy( mem.ram + 0x10000, mem.ram, 0x80 ); // some code wraps around (ugh)

	beeper_delta = int (apu.amp_range * 0.65);
	last_beeper = 0;
	apu.reset();
	next_play = play_period;

	// start at spectrum speed
	change_clock_rate( spectrum_clock );
	set_tempo( tempo() );

	spectrum_mode = false;
	cpc_mode      = false;
	cpc_latch     = 0;

	return 0;
}

// Emulation

void Ay_Emu::cpu_out_misc( cpu_time_t time, unsigned addr, int data )
{
	if ( !cpc_mode )
	{
		switch ( addr & 0xFEFF )
		{
		case 0xFEFD:
			spectrum_mode = true;
			apu_addr = data & 0x0F;
			return;

		case 0xBEFD:
			spectrum_mode = true;
			apu.write( time, apu_addr, data );
			return;
		}
	}

	if ( !spectrum_mode )
	{
		switch ( addr >> 8 )
		{
		case 0xF6:
			switch ( data & 0xC0 )
			{
			case 0xC0:
				apu_addr = cpc_latch & 0x0F;
				goto enable_cpc;

			case 0x80:
				apu.write( time, apu_addr, cpc_latch );
				goto enable_cpc;
			}
			break;

		case 0xF4:
			cpc_latch = data;
			goto enable_cpc;
		}
	}

	debug_printf( "Unmapped OUT: $%04X <- $%02X\n", addr, data );
	return;

enable_cpc:
	if ( !cpc_mode )
	{
		cpc_mode = true;
		change_clock_rate( cpc_clock );
		set_tempo( tempo() );
	}
}

void ay_cpu_out( Ay_Cpu* cpu, cpu_time_t time, unsigned addr, int data )
{
	Ay_Emu& emu = STATIC_CAST(Ay_Emu&,*cpu);

	if ( (addr & 0xFF) == 0xFE && !emu.cpc_mode )
	{
		int delta = emu.beeper_delta;
		data &= 0x10;
		if ( emu.last_beeper != data )
		{
			emu.last_beeper = data;
			emu.beeper_delta = -delta;
			emu.spectrum_mode = true;
			if ( emu.beeper_output )
				emu.apu.synth_.offset( time, delta, emu.beeper_output );
		}
	}
	else
	{
		emu.cpu_out_misc( time, addr, data );
	}
}

int ay_cpu_in( Ay_Cpu*, unsigned addr )
{
	// keyboard read and other things
	if ( (addr & 0xFF) == 0xFE )
		return 0xFF; // other values break some beeper tunes

	debug_printf( "Unmapped IN : $%04X\n", addr );
	return 0xFF;
>>>>>>> db7344ebf (abc)
}

blargg_err_t Ay_Emu::run_clocks( blip_time_t& duration, int )
{
<<<<<<< HEAD
	core.end_frame( &duration );
	return blargg_ok;
}

inline void Ay_Emu::enable_cpc()
{
	change_clock_rate( cpc_clock );
	set_tempo( tempo() );
}

void Ay_Emu::enable_cpc_( void* data )
{
	STATIC_CAST(Ay_Emu*,data)->enable_cpc();
}

blargg_err_t Ay_Emu::hash_( Hash_Function& out ) const
{
	hash_ay_file( file, out );
	return blargg_ok;
=======
	set_time( 0 );
	if ( !(spectrum_mode | cpc_mode) )
		duration /= 2; // until mode is set, leave room for halved clock rate

	while ( time() < duration )
	{
		cpu::run( min( duration, (blip_time_t) next_play ) );

		if ( time() >= next_play )
		{
			next_play += play_period;

			if ( r.iff1 )
			{
				if ( mem.ram [r.pc] == 0x76 )
					r.pc++;

				r.iff1 = r.iff2 = 0;

				mem.ram [--r.sp] = uint8_t (r.pc >> 8);
				mem.ram [--r.sp] = uint8_t (r.pc);
				r.pc = 0x38;
				cpu::adjust_time( 12 );
				if ( r.im == 2 )
				{
					cpu::adjust_time( 6 );
					unsigned addr = r.i * 0x100u + 0xFF;
					r.pc = mem.ram [(addr + 1) & 0xFFFF] * 0x100u + mem.ram [addr];
				}
			}
		}
	}
	duration = time();
	next_play -= duration;
	check( next_play >= 0 );
	adjust_time( -duration );

	apu.end_frame( duration );

	return 0;
>>>>>>> db7344ebf (abc)
}
