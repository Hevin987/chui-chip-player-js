<<<<<<< HEAD
// Game_Music_Emu $vers. http://www.slack.net/~ant/
=======
// Game_Music_Emu https://bitbucket.org/mpyne/game-music-emu/
>>>>>>> db7344ebf (abc)

#include "Kss_Emu.h"

#include "blargg_endian.h"
<<<<<<< HEAD

/* Copyright (C) 2006-2009 Shay Green. This module is free software; you
=======
#include <string.h>
#include <algorithm>

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
#define IF_PTR( ptr ) if ( ptr ) (ptr)

int const clock_rate = 3579545;

#define FOR_EACH_APU( macro )\
{\
	macro( sms.psg   );\
	macro( sms.fm    );\
	macro( msx.psg   );\
	macro( msx.scc   );\
	macro( msx.music );\
	macro( msx.audio );\
}

Kss_Emu::Kss_Emu() :
	core( this )
{
	#define ACTION( apu ) { core.apu = NULL; }
	FOR_EACH_APU( ACTION );
	#undef ACTION

	set_type( gme_kss_type );
}

Kss_Emu::~Kss_Emu()
{
	unload();
}

inline void Kss_Emu::Core::unload()
{
	#define ACTION( ptr ) { delete (ptr); (ptr) = 0; }
	FOR_EACH_APU( ACTION );
	#undef ACTION
}

void Kss_Emu::unload()
{
	core.unload();
=======
static long const clock_rate = 3579545;
static int const osc_count = Ay_Apu::osc_count + Scc_Apu::osc_count;

using std::min;
using std::max;

Kss_Emu::Kss_Emu()
{
	sn = 0;
	set_type( gme_kss_type );
	set_silence_lookahead( 6 );
	static const char* const names [osc_count] = {
		"Square 1", "Square 2", "Square 3",
		"Wave 1", "Wave 2", "Wave 3", "Wave 4", "Wave 5"
	};
	set_voice_names( names );

	static int const types [osc_count] = {
		wave_type | 0, wave_type | 1, wave_type | 2,
		wave_type | 3, wave_type | 4, wave_type | 5, wave_type | 6, wave_type | 7
	};
	set_voice_types( types );

	memset( unmapped_read, 0xFF, sizeof unmapped_read );
}

Kss_Emu::~Kss_Emu() { unload(); }

void Kss_Emu::unload()
{
	delete sn;
	sn = 0;
>>>>>>> db7344ebf (abc)
	Classic_Emu::unload();
}

// Track info

<<<<<<< HEAD
static void copy_kss_fields( Kss_Core::header_t const& h, track_info_t* out )
{
	const char* system = "MSX";

=======
static void copy_kss_fields( Kss_Emu::header_t const& h, track_info_t* out )
{
	const char* system = "MSX";
>>>>>>> db7344ebf (abc)
	if ( h.device_flags & 0x02 )
	{
		system = "Sega Master System";
		if ( h.device_flags & 0x04 )
			system = "Game Gear";

		if ( h.device_flags & 0x01 )
<<<<<<< HEAD
			system = "Sega Mark III";
	}
	else
	{
		if ( h.device_flags & 0x09 )
			system = "MSX + FM Sound";
=======
			system = "Sega Mega Drive";
>>>>>>> db7344ebf (abc)
	}
	Gme_File::copy_field_( out->system, system );
}

<<<<<<< HEAD
static void hash_kss_file( Kss_Core::header_t const& h, byte const* data, int data_size, Music_Emu::Hash_Function& out )
{
	out.hash_( &h.load_addr[0], sizeof(h.load_addr) );
	out.hash_( &h.load_size[0], sizeof(h.load_size) );
	out.hash_( &h.init_addr[0], sizeof(h.init_addr) );
	out.hash_( &h.play_addr[0], sizeof(h.play_addr) );
	out.hash_( &h.first_bank, sizeof(h.first_bank) );
	out.hash_( &h.bank_mode, sizeof(h.bank_mode) );
	out.hash_( &h.extra_header, sizeof(h.extra_header) );
	out.hash_( &h.device_flags, sizeof(h.device_flags) );

	out.hash_( data, data_size );
}

blargg_err_t Kss_Emu::track_info_( track_info_t* out, int ) const
{
	copy_kss_fields( header(), out );
// TODO: remove
//if ( msx.music ) strcpy( out->system, "msxmusic" );
//if ( msx.audio ) strcpy( out->system, "msxaudio" );
//if ( sms.fm    ) strcpy( out->system, "fmunit"   );
	return blargg_ok;
=======
blargg_err_t Kss_Emu::track_info_( track_info_t* out, int ) const
{
	copy_kss_fields( header_, out );
	return 0;
>>>>>>> db7344ebf (abc)
}

static blargg_err_t check_kss_header( void const* header )
{
	if ( memcmp( header, "KSCC", 4 ) && memcmp( header, "KSSX", 4 ) )
<<<<<<< HEAD
		return blargg_err_file_type;

	return blargg_ok;
=======
		return gme_wrong_file_type;
	return 0;
>>>>>>> db7344ebf (abc)
}

struct Kss_File : Gme_Info_
{
<<<<<<< HEAD
	Kss_Emu::header_t const* header_;

	Kss_File() { set_type( gme_kss_type ); }

	blargg_err_t load_mem_( byte const begin [], int size )
	{
		header_ = ( Kss_Emu::header_t const* ) begin;

		if ( header_->tag [3] == 'X' && header_->extra_header == 0x10 )
			set_track_count( get_le16( header_->last_track ) + 1 );

		return check_kss_header( header_ );
=======
	Kss_Emu::header_t header_;

	Kss_File() { set_type( gme_kss_type ); }

	blargg_err_t load_( Data_Reader& in )
	{
		blargg_err_t err = in.read( &header_, Kss_Emu::header_size );
		if ( err )
			return (err == in.eof_error ? gme_wrong_file_type : err);
		return check_kss_header( &header_ );
>>>>>>> db7344ebf (abc)
	}

	blargg_err_t track_info_( track_info_t* out, int ) const
	{
<<<<<<< HEAD
		copy_kss_fields( *header_, out );
		return blargg_ok;
	}

	blargg_err_t hash_( Hash_Function& out ) const
	{
		hash_kss_file( *header_, file_begin() + Kss_Core::header_t::base_size, file_end() - file_begin() - Kss_Core::header_t::base_size, out );
		return blargg_ok;
=======
		copy_kss_fields( header_, out );
		return 0;
>>>>>>> db7344ebf (abc)
	}
};

static Music_Emu* new_kss_emu () { return BLARGG_NEW Kss_Emu ; }
static Music_Emu* new_kss_file() { return BLARGG_NEW Kss_File; }

<<<<<<< HEAD
gme_type_t_ const gme_kss_type [1] = {{
	"MSX",
	256,
	&new_kss_emu,
	&new_kss_file,
	"KSS",
	0x03
}};

// Setup

void Kss_Emu::Core::update_gain_()
{
	double g = emu.gain();
	if ( msx.music || msx.audio || sms.fm )
	{
		g *= 0.3;
	}
	else
	{
		g *= 1.2;
		if ( scc_accessed )
			g *= 1.4;
	}

	#define ACTION( apu ) IF_PTR( apu )->volume( g )
	FOR_EACH_APU( ACTION );
	#undef ACTION
}

static blargg_err_t new_opl_apu( Opl_Apu::type_t type, Opl_Apu** out )
{
	check( !*out );
	CHECK_ALLOC( *out = BLARGG_NEW( Opl_Apu ) );
	blip_time_t const period = 72;
	int const rate = clock_rate / period;
	return (*out)->init( rate * period, rate, period, type );
=======
static gme_type_t_ const gme_kss_type_ = { "MSX", 256, &new_kss_emu, &new_kss_file, "KSS", 0x03 };
extern gme_type_t const gme_kss_type = &gme_kss_type_;


// Setup

void Kss_Emu::update_gain()
{
	double g = gain() * 1.4;
	if ( scc_accessed )
		g *= 1.5;
	ay.volume( g );
	scc.volume( g );
	if ( sn )
		sn->volume( g );
>>>>>>> db7344ebf (abc)
}

blargg_err_t Kss_Emu::load_( Data_Reader& in )
{
<<<<<<< HEAD
	RETURN_ERR( core.load( in ) );
	set_warning( core.warning() );

	set_track_count( get_le16( header().last_track ) + 1 );

	core.scc_enabled = false;
	if ( header().device_flags & 0x02 ) // Sega Master System
	{
		int const osc_count = Sms_Apu::osc_count + Opl_Apu::osc_count;
		static const char* const names [osc_count] = {
			"Square 1", "Square 2", "Square 3", "Noise", "FM"
		};
		set_voice_names( names );

		static int const types [osc_count] = {
			wave_type+1, wave_type+3, wave_type+2, mixed_type+1, wave_type+0
		};
		set_voice_types( types );

		// sms.psg
		set_voice_count( Sms_Apu::osc_count );
		check( !core.sms.psg );
		CHECK_ALLOC( core.sms.psg = BLARGG_NEW Sms_Apu );

		// sms.fm
		if ( header().device_flags & 0x01 )
		{
			set_voice_count( osc_count );
			RETURN_ERR( new_opl_apu( Opl_Apu::type_smsfmunit, &core.sms.fm ) );
		}

	}
	else // MSX
	{
		int const osc_count = Ay_Apu::osc_count + Opl_Apu::osc_count;
		static const char* const names [osc_count] = {
			"Square 1", "Square 2", "Square 3", "FM"
		};
		set_voice_names( names );

		static int const types [osc_count] = {
			wave_type+1, wave_type+3, wave_type+2, wave_type+0
		};
		set_voice_types( types );

		// msx.psg
		set_voice_count( Ay_Apu::osc_count );
		check( !core.msx.psg );
		CHECK_ALLOC( core.msx.psg = BLARGG_NEW Ay_Apu );

		if ( header().device_flags & 0x10 )
			set_warning( "MSX stereo not supported" );

		// msx.music
		if ( header().device_flags & 0x01 )
		{
			set_voice_count( osc_count );
			RETURN_ERR( new_opl_apu( Opl_Apu::type_msxmusic, &core.msx.music ) );
		}

		// msx.audio
		if ( header().device_flags & 0x08 )
		{
			set_voice_count( osc_count );
			RETURN_ERR( new_opl_apu( Opl_Apu::type_msxaudio, &core.msx.audio ) );
		}

		if ( !(header().device_flags & 0x80) )
		{
			if ( !(header().device_flags & 0x84) )
				core.scc_enabled = core.scc_enabled_true;

			// msx.scc
			check( !core.msx.scc );
			CHECK_ALLOC( core.msx.scc = BLARGG_NEW Scc_Apu );

			int const osc_count = Ay_Apu::osc_count + Scc_Apu::osc_count;
			static const char* const names [osc_count] = {
				"Square 1", "Square 2", "Square 3",
				"Wave 1", "Wave 2", "Wave 3", "Wave 4", "Wave 5"
			};
			set_voice_names( names );

			static int const types [osc_count] = {
				wave_type+1, wave_type+3, wave_type+2,
				wave_type+0, wave_type+4, wave_type+5, wave_type+6, wave_type+7,
			};
			set_voice_types( types );

			set_voice_count( osc_count );
		}
	}

	set_silence_lookahead( 6 );
	if ( core.sms.fm || core.msx.music || core.msx.audio )
	{
		if ( !Opl_Apu::supported() )
			set_warning( "FM sound not supported" );
		else
			set_silence_lookahead( 3 ); // Opl_Apu is really slow
	}

=======
	memset( &header_, 0, sizeof header_ );
	blaarg_static_assert( offsetof (header_t,device_flags) == header_size - 1, "KSS Header layout incorrect!" );
	blaarg_static_assert( offsetof (ext_header_t,msx_audio_vol) == ext_header_size - 1, "KSS Extended Header layout incorrect!" );
	RETURN_ERR( rom.load( in, header_size, STATIC_CAST(header_t*,&header_), 0 ) );

	RETURN_ERR( check_kss_header( header_.tag ) );

	if ( header_.tag [3] == 'C' )
	{
		if ( header_.extra_header )
		{
			header_.extra_header = 0;
			set_warning( "Unknown data in header" );
		}
		if ( header_.device_flags & ~0x0F )
		{
			header_.device_flags &= 0x0F;
			set_warning( "Unknown data in header" );
		}
	}
	else
	{
		ext_header_t& ext = header_;
		memcpy( &ext, rom.begin(), min( (int) ext_header_size, (int) header_.extra_header ) );
		if ( header_.extra_header > 0x10 )
			set_warning( "Unknown data in header" );
	}

	if ( header_.device_flags & 0x09 )
		set_warning( "FM sound not supported" );

	scc_enabled = 0xC000;
	if ( header_.device_flags & 0x04 )
		scc_enabled = 0;

	if ( header_.device_flags & 0x02 && !sn )
		CHECK_ALLOC( sn = BLARGG_NEW( Sms_Apu ) );

	set_voice_count( osc_count );

>>>>>>> db7344ebf (abc)
	return setup_buffer( ::clock_rate );
}

void Kss_Emu::update_eq( blip_eq_t const& eq )
{
<<<<<<< HEAD
	#define ACTION( apu ) IF_PTR( core.apu )->treble_eq( eq )
	FOR_EACH_APU( ACTION );
	#undef ACTION
=======
	ay.treble_eq( eq );
	scc.treble_eq( eq );
	if ( sn )
		sn->treble_eq( eq );
>>>>>>> db7344ebf (abc)
}

void Kss_Emu::set_voice( int i, Blip_Buffer* center, Blip_Buffer* left, Blip_Buffer* right )
{
<<<<<<< HEAD
	if ( core.sms.psg ) // Sega Master System
	{
		i -= core.sms.psg->osc_count;
		if ( i < 0 )
		{
			core.sms.psg->set_output( i + core.sms.psg->osc_count, center, left, right );
			return;
		}

		if ( core.sms.fm && i < core.sms.fm->osc_count )
			core.sms.fm->set_output( i, center, NULL, NULL );
	}
	else if ( core.msx.psg ) // MSX
	{
		i -= core.msx.psg->osc_count;
		if ( i < 0 )
		{
			core.msx.psg->set_output( i + core.msx.psg->osc_count, center );
			return;
		}

		if ( core.msx.scc   && i < core.msx.scc->osc_count   ) core.msx.scc  ->set_output( i, center );
		if ( core.msx.music && i < core.msx.music->osc_count ) core.msx.music->set_output( i, center, NULL, NULL );
		if ( core.msx.audio && i < core.msx.audio->osc_count ) core.msx.audio->set_output( i, center, NULL, NULL );
	}
}

void Kss_Emu::set_tempo_( double t )
{
	int period = (header().device_flags & 0x40 ? ::clock_rate / 50 : ::clock_rate / 60);
	core.set_play_period( (Kss_Core::time_t) (period / t) );
=======
	int i2 = i - ay.osc_count;
	if ( i2 >= 0 )
		scc.osc_output( i2, center );
	else
		ay.osc_output( i, center );
	if ( sn && i < sn->osc_count )
		sn->osc_output( i, center, left, right );
}

// Emulation

void Kss_Emu::set_tempo_( double t )
{
	blip_time_t period =
			(header_.device_flags & 0x40 ? ::clock_rate / 50 : ::clock_rate / 60);
	play_period = blip_time_t (period / t);
>>>>>>> db7344ebf (abc)
}

blargg_err_t Kss_Emu::start_track_( int track )
{
	RETURN_ERR( Classic_Emu::start_track_( track ) );

<<<<<<< HEAD
	#define ACTION( apu ) IF_PTR( core.apu )->reset()
	FOR_EACH_APU( ACTION );
	#undef ACTION

	core.scc_accessed = false;
	core.update_gain_();

	return core.start_track( track );
}

void Kss_Emu::Core::cpu_write_( addr_t addr, int data )
{
	// TODO: SCC+ support

=======
	memset( ram, 0xC9, 0x4000 );
	memset( ram + 0x4000, 0, sizeof ram - 0x4000 );

	// copy driver code to lo RAM
	static byte const bios [] = {
		0xD3, 0xA0, 0xF5, 0x7B, 0xD3, 0xA1, 0xF1, 0xC9, // $0001: WRTPSG
		0xD3, 0xA0, 0xDB, 0xA2, 0xC9                    // $0009: RDPSG
	};
	static byte const vectors [] = {
		0xC3, 0x01, 0x00,   // $0093: WRTPSG vector
		0xC3, 0x09, 0x00,   // $0096: RDPSG vector
	};
	memcpy( ram + 0x01, bios,    sizeof bios );
	memcpy( ram + 0x93, vectors, sizeof vectors );

	// copy non-banked data into RAM
	unsigned load_addr = get_le16( header_.load_addr );
	long orig_load_size = get_le16( header_.load_size );
	long load_size = min( orig_load_size, rom.file_size() );
	load_size = min( load_size, long (mem_size - load_addr) );
	if ( load_size != orig_load_size )
		set_warning( "Excessive data size" );
	memcpy( ram + load_addr, rom.begin() + header_.extra_header, load_size );

	rom.set_addr( -load_size - header_.extra_header );

	// check available bank data
	int32_t const bank_size = this->bank_size();
	int max_banks = (rom.file_size() - load_size + bank_size - 1) / bank_size;
	bank_count = header_.bank_mode & 0x7F;
	if ( bank_count > max_banks )
	{
		bank_count = max_banks;
		set_warning( "Bank data missing" );
	}
	//debug_printf( "load_size : $%X\n", load_size );
	//debug_printf( "bank_size : $%X\n", bank_size );
	//debug_printf( "bank_count: %d (%d claimed)\n", bank_count, header_.bank_mode & 0x7F );

	ram [idle_addr] = 0xFF;
	cpu::reset( unmapped_write, unmapped_read );
	cpu::map_mem( 0, mem_size, ram, ram );

	ay.reset();
	scc.reset();
	if ( sn )
		sn->reset();
	r.sp = 0xF380;
	ram [--r.sp] = idle_addr >> 8;
	ram [--r.sp] = idle_addr & 0xFF;
	r.b.a = track;
	r.pc = get_le16( header_.init_addr );
	next_play = play_period;
	scc_accessed = false;
	gain_updated = false;
	update_gain();
	ay_latch = 0;

	return 0;
}

void Kss_Emu::set_bank( int logical, int physical )
{
	unsigned const bank_size = this->bank_size();

	unsigned addr = 0x8000;
	if ( logical && bank_size == 8 * 1024 )
		addr = 0xA000;

	physical -= header_.first_bank;
	if ( (unsigned) physical >= (unsigned) bank_count )
	{
		byte* data = ram + addr;
		cpu::map_mem( addr, bank_size, data, data );
	}
	else
	{
		long phys = physical * (int32_t) bank_size;
		for ( unsigned offset = 0; offset < bank_size; offset += page_size )
			cpu::map_mem( addr + offset, page_size,
					unmapped_write, rom.at_addr( phys + offset ) );
	}
}

void Kss_Emu::cpu_write( unsigned addr, int data )
{
>>>>>>> db7344ebf (abc)
	data &= 0xFF;
	switch ( addr )
	{
	case 0x9000:
		set_bank( 0, data );
		return;

	case 0xB000:
		set_bank( 1, data );
		return;
<<<<<<< HEAD

	case 0xBFFE: // selects between mapping areas (we just always enable both)
		if ( data == 0 || data == 0x20 )
			return;
	}

	int scc_addr = (addr & 0xDFFF) - 0x9800;
	if ( (unsigned) scc_addr < 0xB0 && msx.scc )
	{
		scc_accessed = true;
		//if ( (unsigned) (scc_addr - 0x90) < 0x10 )
		//  scc_addr -= 0x10; // 0x90-0x9F mirrors to 0x80-0x8F
		if ( scc_addr < Scc_Apu::reg_count )
			msx.scc->write( cpu.time(), addr, data );
		return;
	}

	dprintf( "LD ($%04X),$%02X\n", addr, data );
}

void Kss_Emu::Core::cpu_write( addr_t addr, int data )
{
	*cpu.write( addr ) = data;
	if ( (addr & scc_enabled) == 0x8000 )
		cpu_write_( addr, data );
}

void Kss_Emu::Core::cpu_out( time_t time, addr_t addr, int data )
{
	data &= 0xFF;
	switch ( addr & 0xFF )
	{
	case 0xA0:
		if ( msx.psg )
			msx.psg->write_addr( data );
		return;

	case 0xA1:
		if ( msx.psg )
			msx.psg->write_data( time, data );
		return;

	case 0x06:
		if ( sms.psg && (header().device_flags & 0x04) )
		{
			sms.psg->write_ggstereo( time, data );
=======
	}

	int scc_addr = (addr & 0xDFFF) ^ 0x9800;
	if ( scc_addr < scc.reg_count )
	{
		scc_accessed = true;
		scc.write( time(), scc_addr, data );
		return;
	}

	debug_printf( "LD ($%04X),$%02X\n", addr, data );
}

void kss_cpu_write( Kss_Cpu* cpu, unsigned addr, int data )
{
	*cpu->write( addr ) = data;
	if ( (addr & STATIC_CAST(Kss_Emu&,*cpu).scc_enabled) == 0x8000 )
		STATIC_CAST(Kss_Emu&,*cpu).cpu_write( addr, data );
}

void kss_cpu_out( Kss_Cpu* cpu, cpu_time_t time, unsigned addr, int data )
{
	data &= 0xFF;
	Kss_Emu& emu = STATIC_CAST(Kss_Emu&,*cpu);
	switch ( addr & 0xFF )
	{
	case 0xA0:
		emu.ay_latch = data & 0x0F;
		return;

	case 0xA1:
		GME_APU_HOOK( &emu, emu.ay_latch, data );
		emu.ay.write( time, emu.ay_latch, data );
		return;

	case 0x06:
		if ( emu.sn && (emu.header_.device_flags & 0x04) )
		{
			emu.sn->write_ggstereo( time, data );
>>>>>>> db7344ebf (abc)
			return;
		}
		break;

	case 0x7E:
	case 0x7F:
<<<<<<< HEAD
		if ( sms.psg )
		{
			sms.psg->write_data( time, data );
=======
		if ( emu.sn )
		{
			GME_APU_HOOK( &emu, 16, data );
			emu.sn->write_data( time, data );
>>>>>>> db7344ebf (abc)
			return;
		}
		break;

<<<<<<< HEAD
	#define OPL_WRITE_HANDLER( base, opl )\
		case base  : if ( opl ) { opl->write_addr(       data ); return; } break;\
		case base+1: if ( opl ) { opl->write_data( time, data ); return; } break;

	OPL_WRITE_HANDLER( 0x7C, msx.music )
	OPL_WRITE_HANDLER( 0xC0, msx.audio )
	OPL_WRITE_HANDLER( 0xF0, sms.fm    )

	case 0xFE:
		set_bank( 0, data );
		return;

	#ifndef NDEBUG
=======
	case 0xFE:
		emu.set_bank( 0, data );
		return;

	#ifndef NDEBUG
	case 0xF1: // FM data
		if ( data )
			break; // trap non-zero data
	case 0xF0: // FM addr
>>>>>>> db7344ebf (abc)
	case 0xA8: // PPI
		return;
	#endif
	}

<<<<<<< HEAD
	Kss_Core::cpu_out( time, addr, data );
}

int Kss_Emu::Core::cpu_in( time_t time, addr_t addr )
{
	switch ( addr & 0xFF )
	{
	case 0xC0:
	case 0xC1:
		if ( msx.audio )
			return msx.audio->read( time, addr & 1 );
		break;

	case 0xA2:
		if ( msx.psg )
			return msx.psg->read();
		break;

	#ifndef NDEBUG
	case 0xA8: // PPI
		return 0;
	#endif
	}

	return Kss_Core::cpu_in( time, addr );
}

void Kss_Emu::Core::update_gain()
{
	if ( scc_accessed )
	{
		dprintf( "SCC accessed\n" );
		update_gain_();
	}
}

blargg_err_t Kss_Emu::run_clocks( blip_time_t& duration, int )
{
	RETURN_ERR( core.end_frame( duration ) );

	#define ACTION( apu ) IF_PTR( core.apu )->end_frame( duration )
	FOR_EACH_APU( ACTION );
	#undef ACTION

	return blargg_ok;
}

blargg_err_t Kss_Emu::hash_( Hash_Function& out ) const
{
	hash_kss_file( header(), core.rom_().begin(), core.rom_().file_size(), out );
	return blargg_ok;
}
=======
	debug_printf( "OUT $%04X,$%02X\n", addr, data );
}

int kss_cpu_in( Kss_Cpu*, cpu_time_t, unsigned addr )
{
	//Kss_Emu& emu = STATIC_CAST(Kss_Emu&,*cpu);
	//switch ( addr & 0xFF )
	//{
	//}

	debug_printf( "IN $%04X\n", addr );
	return 0;
}

// Emulation

blargg_err_t Kss_Emu::run_clocks( blip_time_t& duration, int )
{
	while ( time() < duration )
	{
		blip_time_t end = min( duration, next_play );
		cpu::run( min( duration, next_play ) );
		if ( r.pc == idle_addr )
			set_time( end );

		if ( time() >= next_play )
		{
			next_play += play_period;
			if ( r.pc == idle_addr )
			{
				if ( !gain_updated )
				{
					gain_updated = true;
					if ( scc_accessed )
						update_gain();
				}

				ram [--r.sp] = idle_addr >> 8;
				ram [--r.sp] = idle_addr & 0xFF;
				r.pc = get_le16( header_.play_addr );
				GME_FRAME_HOOK( this );
			}
		}
	}

	duration = time();
	next_play -= duration;
	check( next_play >= 0 );
	adjust_time( -duration );
	ay.end_frame( duration );
	scc.end_frame( duration );
	if ( sn )
		sn->end_frame( duration );

	return 0;
}
>>>>>>> db7344ebf (abc)
