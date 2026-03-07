<<<<<<< HEAD
// Game_Music_Emu $vers. http://www.slack.net/~ant/

#include "Gme_File.h"

/* Copyright (C) 2003-2008 Shay Green. This module is free software; you
=======
// Game_Music_Emu https://bitbucket.org/mpyne/game-music-emu/

#include "Gme_File.h"

#include "blargg_endian.h"
#include <string.h>

/* Copyright (C) 2003-2006 Shay Green. This module is free software; you
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
void Gme_File::unload()
{
	clear_playlist(); // BEFORE clearing track count
	track_count_     = 0;
	raw_track_count_ = 0;
	Gme_Loader::unload();
}

Gme_File::Gme_File()
{
	type_           = NULL;
	user_data_      = NULL;
	user_cleanup_   = NULL;
	Gme_File::unload(); // clears fields
}

Gme_File::~Gme_File()
{
	if ( user_cleanup_ )
		user_cleanup_( user_data_ );
}

blargg_err_t Gme_File::post_load()
{
	if ( !track_count() )
		set_track_count( type()->track_count );
	return Gme_Loader::post_load();
}
=======
const char* const gme_wrong_file_type = "Wrong file type for this emulator";
>>>>>>> db7344ebf (abc)

void Gme_File::clear_playlist()
{
	playlist.clear();
	clear_playlist_();
	track_count_ = raw_track_count_;
}

<<<<<<< HEAD
void Gme_File::copy_field_( char out [], const char* in, int in_size )
{
	if ( !in || !*in )
		return;
	
=======
void Gme_File::unload()
{
	clear_playlist(); // *before* clearing track count
	track_count_     = 0;
	raw_track_count_ = 0;
	file_data.clear();
}

Gme_File::Gme_File()
{
	type_         = 0;
	user_data_    = 0;
	user_cleanup_ = 0;
	unload(); // clears fields
	blargg_verify_byte_order(); // used by most emulator types, so save them the trouble
}

Gme_File::~Gme_File()
{
	if ( user_cleanup_ )
		user_cleanup_( user_data_ );
}

blargg_err_t Gme_File::load_mem_( byte const* data, long size )
{
	require( data != file_data.begin() ); // load_mem_() or load_() must be overridden
	Mem_File_Reader in( data, size );
	return load_( in );
}

blargg_err_t Gme_File::load_( Data_Reader& in )
{
	RETURN_ERR( file_data.resize( in.remain() ) );
	RETURN_ERR( in.read( file_data.begin(), file_data.size() ) );
	if ( type()->track_count == 1 )
	{
		RETURN_ERR( tracks.resize( 2 ) );
		tracks[0] = 0, tracks[1] = file_data.size();
	}
	return load_mem_( file_data.begin(), file_data.size() );
}

// public load functions call this at beginning
void Gme_File::pre_load() { unload(); }

void Gme_File::post_load_() { }

// public load functions call this at end
blargg_err_t Gme_File::post_load( blargg_err_t err )
{
	if ( !track_count() )
		set_track_count( type()->track_count );
	if ( !err )
		post_load_();
	else
		unload();

	return err;
}

// Public load functions

blargg_err_t Gme_File::load_mem( void const* in, long size )
{
	pre_load();
	return post_load( load_mem_( (byte const*) in, size ) );
}

blargg_err_t Gme_File::load_tracks( void const* in, long* sizes, int count )
{
	pre_load();
	if ( type()->track_count != 1 )
		return "File type must have a fixed track count of 1";
	set_track_count( count );
	RETURN_ERR( tracks.resize( count + 1 ) );
	long size = 0;
	for ( int i = 0; i < count; size += sizes[i++] )
		tracks[i] = size;
	tracks[count] = size;
	RETURN_ERR( file_data.resize( size ) );
	memcpy( file_data.begin(), in, size );
	return post_load( load_mem_( file_data.begin(), tracks[1] ) );
}

blargg_err_t Gme_File::load( Data_Reader& in )
{
	pre_load();
	return post_load( load_( in ) );
}

blargg_err_t Gme_File::load_file( const char* path )
{
	pre_load();
	GME_FILE_READER in;
	RETURN_ERR( in.open( path ) );
	return post_load( load_( in ) );
}

blargg_err_t Gme_File::load_remaining_( void const* h, long s, Data_Reader& in )
{
	Remaining_Reader rem( h, s, &in );
	return load( rem );
}

// Track info

void Gme_File::copy_field_( char* out, const char* in, int in_size )
{
	if ( !in || !*in )
		return;

>>>>>>> db7344ebf (abc)
	// remove spaces/junk from beginning
	while ( in_size && unsigned (*in - 1) <= ' ' - 1 )
	{
		in++;
		in_size--;
	}
<<<<<<< HEAD
	
	// truncate
	if ( in_size > max_field_ )
		in_size = max_field_;
	
=======

	// truncate
	if ( in_size > max_field_ )
		in_size = max_field_;

>>>>>>> db7344ebf (abc)
	// find terminator
	int len = 0;
	while ( len < in_size && in [len] )
		len++;
<<<<<<< HEAD
	
	// remove spaces/junk from end
	while ( len && unsigned (in [len - 1]) <= ' ' )
		len--;
	
	// copy
	out [len] = 0;
	memcpy( out, in, len );
	
=======

	// remove spaces/junk from end
	while ( len && unsigned (in [len - 1]) <= ' ' )
		len--;

	// copy
	out [len] = 0;
	memcpy( out, in, len );

>>>>>>> db7344ebf (abc)
	// strip out stupid fields that should have been left blank
	if ( !strcmp( out, "?" ) || !strcmp( out, "<?>" ) || !strcmp( out, "< ? >" ) )
		out [0] = 0;
}

<<<<<<< HEAD
void Gme_File::copy_field_( char out [], const char* in )
=======
void Gme_File::copy_field_( char* out, const char* in )
>>>>>>> db7344ebf (abc)
{
	copy_field_( out, in, max_field_ );
}

blargg_err_t Gme_File::remap_track_( int* track_io ) const
{
	if ( (unsigned) *track_io >= (unsigned) track_count() )
<<<<<<< HEAD
		return BLARGG_ERR( BLARGG_ERR_CALLER, "invalid track" );
	
=======
		return "Invalid track";

>>>>>>> db7344ebf (abc)
	if ( (unsigned) *track_io < (unsigned) playlist.size() )
	{
		M3u_Playlist::entry_t const& e = playlist [*track_io];
		*track_io = 0;
		if ( e.track >= 0 )
		{
			*track_io = e.track;
<<<<<<< HEAD
			// TODO: really needs to be removed?
			//if ( !(type_->flags_ & 0x02) )
			//  *track_io -= e.decimal_track;
		}
		if ( *track_io >= raw_track_count_ )
			return BLARGG_ERR( BLARGG_ERR_FILE_CORRUPT, "invalid track in m3u playlist" );
=======
			if ( !(type_->flags_ & 0x02) )
				*track_io -= e.decimal_track;
		}
		if ( *track_io >= raw_track_count_ )
			return "Invalid track in m3u playlist";
>>>>>>> db7344ebf (abc)
	}
	else
	{
		check( !playlist.size() );
	}
<<<<<<< HEAD
	return blargg_ok;
=======
	return 0;
>>>>>>> db7344ebf (abc)
}

blargg_err_t Gme_File::track_info( track_info_t* out, int track ) const
{
<<<<<<< HEAD
	out->track_count   = track_count();
=======
	out->track_count = track_count();
>>>>>>> db7344ebf (abc)
	out->length        = -1;
	out->loop_length   = -1;
	out->intro_length  = -1;
	out->fade_length   = -1;
	out->play_length   = -1;
	out->repeat_count  = -1;
<<<<<<< HEAD
	out->song      [0] = 0;
	out->game      [0] = 0;
	out->author    [0] = 0;
	out->composer  [0] = 0;
	out->engineer  [0] = 0;
	out->sequencer [0] = 0;
	out->tagger    [0] = 0;
	out->copyright [0] = 0;
	out->date      [0] = 0;
	out->comment   [0] = 0;
	out->dumper    [0] = 0;
	out->system    [0] = 0;
	out->disc      [0] = 0;
	out->track     [0] = 0;
	out->ost       [0] = 0;
	
	copy_field_( out->system, type()->system );
	
	int remapped = track;
	RETURN_ERR( remap_track_( &remapped ) );
	RETURN_ERR( track_info_( out, remapped ) );
	
=======
	out->song [0]      = 0;

	out->game [0]      = 0;
	out->author [0]    = 0;
	out->composer [0]  = 0;
	out->engineer [0]  = 0;
	out->sequencer [0] = 0;
	out->tagger [0]    = 0;
	out->copyright [0] = 0;
	out->date [0]      = 0;
	out->comment [0]   = 0;
	out->dumper [0]    = 0;
	out->system [0]    = 0;
	out->disc [0]      = 0;
	out->track [0]     = 0;
	out->ost [0]       = 0;

	copy_field_( out->system, type()->system );

	int remapped = track;
	RETURN_ERR( remap_track_( &remapped ) );
	RETURN_ERR( track_info_( out, remapped ) );

>>>>>>> db7344ebf (abc)
	// override with m3u info
	if ( playlist.size() )
	{
		M3u_Playlist::info_t const& i = playlist.info();
<<<<<<< HEAD
		copy_field_( out->game     , i.title );
		copy_field_( out->author   , i.artist );
		copy_field_( out->engineer , i.engineer );
		copy_field_( out->composer , i.composer );
		copy_field_( out->sequencer, i.sequencer );
		copy_field_( out->copyright, i.copyright );
		copy_field_( out->dumper   , i.ripping );
		copy_field_( out->tagger   , i.tagging );
		copy_field_( out->date     , i.date );
		
		M3u_Playlist::entry_t const& e = playlist [track];
=======
		copy_field_( out->game  , i.title );
		copy_field_( out->author, i.artist );
		copy_field_( out->engineer, i.engineer );
		copy_field_( out->composer, i.composer );
		copy_field_( out->sequencer, i.sequencer );
		copy_field_( out->copyright, i.copyright );
		copy_field_( out->dumper, i.ripping );
		copy_field_( out->tagger, i.tagging );
		copy_field_( out->date, i.date );

		M3u_Playlist::entry_t const& e = playlist [track];
		copy_field_( out->song, e.name );
>>>>>>> db7344ebf (abc)
		if ( e.length >= 0 ) out->length       = e.length;
		if ( e.intro  >= 0 ) out->intro_length = e.intro;
		if ( e.loop   >= 0 ) out->loop_length  = e.loop;
		if ( e.fade   >= 0 ) out->fade_length  = e.fade;
		if ( e.repeat >= 0 ) out->repeat_count = e.repeat;
<<<<<<< HEAD
		copy_field_( out->song, e.name );
	}
	
	// play_length
	out->play_length = out->length;
	if ( out->play_length <= 0 )
	{
		out->play_length = out->intro_length + 2 * out->loop_length; // intro + 2 loops
		if ( out->play_length <= 0 )
			out->play_length = 150 * 1000; // 2.5 minutes
	}
	
	return blargg_ok;
=======
	}
	return 0;
>>>>>>> db7344ebf (abc)
}
