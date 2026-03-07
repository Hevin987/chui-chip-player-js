// Nintendo NES/Famicom NSFE music file emulator

<<<<<<< HEAD
// Game_Music_Emu $vers
=======
// Game_Music_Emu https://bitbucket.org/mpyne/game-music-emu/
>>>>>>> db7344ebf (abc)
#ifndef NSFE_EMU_H
#define NSFE_EMU_H

#include "blargg_common.h"
#include "Nsf_Emu.h"
<<<<<<< HEAD
class Nsfe_Emu;
=======
>>>>>>> db7344ebf (abc)

// Allows reading info from NSFE file without creating emulator
class Nsfe_Info {
public:
<<<<<<< HEAD
	blargg_err_t load( Data_Reader&, Nsfe_Emu* );
	
=======
	blargg_err_t load( Data_Reader&, Nsf_Emu* );

>>>>>>> db7344ebf (abc)
	struct info_t : Nsf_Emu::header_t
	{
		char game      [256];
		char author    [256];
		char copyright [256];
		char dumper    [256];
	} info;

<<<<<<< HEAD
	blargg_vector<unsigned char> data;

	void disable_playlist( bool = true );
	
	blargg_err_t track_info_( track_info_t* out, int track ) const;
	
	int remap_track( int i ) const;
	
	void unload();
	
// Implementation
public:
	Nsfe_Info();
	~Nsfe_Info();
	BLARGG_DISABLE_NOTHROW
=======
	void disable_playlist( bool = true );

	blargg_err_t track_info_( track_info_t* out, int track ) const;

	int remap_track( int i ) const;

	void unload();

	Nsfe_Info();
	~Nsfe_Info();
>>>>>>> db7344ebf (abc)
private:
	blargg_vector<char> track_name_data;
	blargg_vector<const char*> track_names;
	blargg_vector<unsigned char> playlist;
	blargg_vector<char [4]> track_times;
	int actual_track_count_;
	bool playlist_disabled;
};

class Nsfe_Emu : public Nsf_Emu {
public:
	static gme_type_t static_type() { return gme_nsfe_type; }
<<<<<<< HEAD
	
	struct header_t { char tag [4]; };


// Implementation
public:
	Nsfe_Emu();
	~Nsfe_Emu();
	virtual void unload();

protected:
	virtual blargg_err_t load_( Data_Reader& );
	virtual blargg_err_t track_info_( track_info_t*, int track ) const;
	virtual blargg_err_t start_track_( int );
	virtual void clear_playlist_();

private:
	Nsfe_Info info;
	
	void disable_playlist_( bool b );
	friend class Nsfe_Info;
=======

public:
	// deprecated
	struct header_t { char tag [4]; };
	using Music_Emu::load;
	blargg_err_t load( header_t const& h, Data_Reader& in ) // use Remaining_Reader
			{ return load_remaining_( &h, sizeof h, in ); }
	void disable_playlist( bool = true ); // use clear_playlist()

public:
	Nsfe_Emu();
	~Nsfe_Emu();
protected:
	blargg_err_t load_( Data_Reader& );
	blargg_err_t track_info_( track_info_t*, int track ) const;
	blargg_err_t start_track_( int );
	void unload();
	void clear_playlist_();
private:
	Nsfe_Info info;
	bool loading;
>>>>>>> db7344ebf (abc)
};

#endif
