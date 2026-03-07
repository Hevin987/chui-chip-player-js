// M3U playlist file parser, with support for subtrack information

<<<<<<< HEAD
// Game_Music_Emu $vers
=======
// Game_Music_Emu https://bitbucket.org/mpyne/game-music-emu/
>>>>>>> db7344ebf (abc)
#ifndef M3U_PLAYLIST_H
#define M3U_PLAYLIST_H

#include "blargg_common.h"
#include "Data_Reader.h"

class M3u_Playlist {
public:
	// Load playlist data
	blargg_err_t load( const char* path );
	blargg_err_t load( Data_Reader& in );
	blargg_err_t load( void const* data, long size );
<<<<<<< HEAD
	
	// Line number of first parse error, 0 if no error. Any lines with parse
	// errors are ignored.
	int first_error() const { return first_error_; }
	
	// All string pointers point to valid string, or "" if not available
=======

	// Line number of first parse error, 0 if no error. Any lines with parse
	// errors are ignored.
	int first_error() const { return first_error_; }

>>>>>>> db7344ebf (abc)
	struct info_t
	{
		const char* title;
		const char* artist;
		const char* date;
		const char* composer;
		const char* sequencer;
		const char* engineer;
		const char* ripping;
		const char* tagging;
		const char* copyright;
	};
	info_t const& info() const { return info_; }
<<<<<<< HEAD
	
	struct entry_t
	{
		const char* file; // filename without stupid ::TYPE suffix
		const char* type; // if filename has ::TYPE suffix, this is "TYPE", otherwise ""
		const char* name;
		bool decimal_track; // true if track was specified in decimal
		// integers are -1 if not present
		int track;
=======

	struct entry_t
	{
		const char* file; // filename without stupid ::TYPE suffix
		const char* type; // if filename has ::TYPE suffix, this will be "TYPE". "" if none.
		const char* name;
		bool decimal_track; // true if track was specified in hex
		// integers are -1 if not present
		int track;  // 1-based
>>>>>>> db7344ebf (abc)
		int length; // milliseconds
		int intro;
		int loop;
		int fade;
		int repeat; // count
	};
	entry_t const& operator [] ( int i ) const { return entries [i]; }
	int size() const { return entries.size(); }
<<<<<<< HEAD
	
	void clear();
	
=======

	void clear();

>>>>>>> db7344ebf (abc)
private:
	blargg_vector<entry_t> entries;
	blargg_vector<char> data;
	int first_error_;
	info_t info_;
<<<<<<< HEAD
	
	blargg_err_t parse();
	blargg_err_t parse_();
	void clear_();
};

inline void M3u_Playlist::clear_()
{
	info_.title     = "";
	info_.artist    = "";
	info_.date      = "";
	info_.composer  = "";
	info_.sequencer = "";
	info_.engineer  = "";
	info_.ripping   = "";
	info_.tagging   = "";
	info_.copyright = "";
	entries.clear();
	data.clear();
}

inline void M3u_Playlist::clear()
{
	first_error_ = 0;
	clear_();
=======

	blargg_err_t parse();
	blargg_err_t parse_();
};

inline void M3u_Playlist::clear()
{
	first_error_ = 0;
	entries.clear();
	data.clear();
>>>>>>> db7344ebf (abc)
}

#endif
