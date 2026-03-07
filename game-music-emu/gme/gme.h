/* Game music emulator library C interface (also usable from C++) */

<<<<<<< HEAD
/* Game_Music_Emu 0.6.2 - forked by kode54 - merged with bitbucket repo by mmontag */
=======
/* Game_Music_Emu 0.6.4 */
>>>>>>> db7344ebf (abc)
#ifndef GME_H
#define GME_H

#ifdef __cplusplus
	extern "C" {
#endif

<<<<<<< HEAD
/* Pointer to error, or NULL if function was successful. See Errors below. */
#ifndef gme_err_t /* (#ifndef allows better testing of library) */
	typedef const char* gme_err_t;
#endif

/* First parameter of most functions is gme_t*, or const gme_t* if nothing is
changed. */
typedef struct gme_t gme_t;

/* Boolean; false = 0, true = 1 */
typedef int gme_bool;


/******** Basic operations ********/

/* Opens game music file and points *out at it. If error, sets *out to NULL. */
gme_err_t gme_open_file( const char path [], gme_t** out, int sample_rate );

/* Number of tracks */
int gme_track_count( const gme_t* );

/* Starts a track, where 0 is the first track. Requires that 0 <= index < gme_track_count(). */
gme_err_t gme_start_track( gme_t*, int index );

/* Generates 'count' 16-bit signed samples info 'out'. Output is in stereo, so count
must be even. */
gme_err_t gme_play( gme_t*, int count, short out [] );

/* Closes file and frees memory. OK to pass NULL. */
void gme_delete( gme_t* );
=======
#define GME_VERSION 0x000604 /* 1 byte major, 1 byte minor, 1 byte patch-level */

/* Error string returned by library functions, or NULL if no error (success) */
typedef const char* gme_err_t;

/* First parameter of most gme_ functions is a pointer to the Music_Emu */
typedef struct Music_Emu Music_Emu;


/* Setup compiler defines useful for exporting required public API symbols in gme.cpp */
#ifndef BLARGG_EXPORT
    #if defined (_WIN32)
        #if defined(BLARGG_BUILD_DLL)
            #define BLARGG_EXPORT __declspec(dllexport)
        #else
            #define BLARGG_EXPORT /* Leave blank: friendly with both static and shared linking. */
        #endif
    #elif defined (LIBGME_VISIBILITY)
        #define BLARGG_EXPORT __attribute__((visibility ("default")))
    #else
        #define BLARGG_EXPORT
    #endif
#endif


/******** Basic operations ********/

/* Create emulator and load game music file/data into it. Sets *out to new emulator. */
BLARGG_EXPORT gme_err_t gme_open_file( const char path [], Music_Emu** out, int sample_rate );

/* Number of tracks available */
BLARGG_EXPORT int gme_track_count( Music_Emu const* );

/* Start a track, where 0 is the first track */
BLARGG_EXPORT gme_err_t gme_start_track( Music_Emu*, int index );

/* Generate 'count' 16-bit signed samples info 'out'. Output is in stereo. */
BLARGG_EXPORT gme_err_t gme_play( Music_Emu*, int count, short out [] );

/* Finish using emulator and free memory */
BLARGG_EXPORT void gme_delete( Music_Emu* );
>>>>>>> db7344ebf (abc)


/******** Track position/length ********/

<<<<<<< HEAD
/* Sets time to start fading track out. Once fade ends track_ended() returns true.
Fade time can be changed while track is playing. */
void gme_set_fade( gme_t*, int start_msec, int length_msec );

/* True if a track has reached its end */
gme_bool gme_track_ended( const gme_t* );

/* Number of milliseconds played since beginning of track (1000 = one second) */
int gme_tell( const gme_t* );

/* Number of milliseconds played since beginning of track (scaled with tempo) */
int gme_tell_scaled( const gme_t* );

/* Seeks to new time in track. Seeking backwards or far forward can take a while. */
gme_err_t gme_seek( gme_t*, int msec );

/* Seeks to new time in track (scaled with current tempo) */
gme_err_t gme_seek_scaled( gme_t*, int msec );

/* Skips the specified number of samples. */
gme_err_t gme_skip( gme_t*, int samples );
=======
/* Set time to start fading track out. Once fade ends track_ended() returns true.
Fade time can be changed while track is playing. */
BLARGG_EXPORT void gme_set_fade( Music_Emu*, int start_msec );

/** See gme_set_fade.
 * @since 0.6.4
 */
BLARGG_EXPORT void gme_set_fade_msecs( Music_Emu*, int start_msec, int length_msecs );

/**
 * If do_autoload_limit is nonzero, then automatically load track length
 * metadata (if present) and terminate playback once the track length has been
 * reached. Otherwise playback will continue for an arbitrary period of time
 * until a prolonged period of silence is detected.
 *
 * Not all individual emulators support this setting.
 *
 * By default, playback limits are loaded and applied.
 *
 * @since 0.6.3
 */
BLARGG_EXPORT void gme_set_autoload_playback_limit( Music_Emu *, int do_autoload_limit );

/** See gme_set_autoload_playback_limit.
 * (This was actually added in 0.6.3, but wasn't exported because of a typo.)
 * @since 0.6.4
 */
BLARGG_EXPORT int gme_autoload_playback_limit( Music_Emu const* );

/* True if a track has reached its end */
BLARGG_EXPORT int gme_track_ended( Music_Emu const* );

/* Number of milliseconds (1000 = one second) played since beginning of track */
BLARGG_EXPORT int gme_tell( Music_Emu const* );

/* Number of samples generated since beginning of track */
BLARGG_EXPORT int gme_tell_samples( Music_Emu const* );

/* Number of milliseconds played since beginning of track (scaled with tempo).
 * @since 0.6.5 */
BLARGG_EXPORT int gme_tell_scaled( Music_Emu const* );

/* Seek to new time in track. Seeking backwards or far forward can take a while. */
BLARGG_EXPORT gme_err_t gme_seek( Music_Emu*, int msec );

/* Equivalent to restarting track then skipping n samples */
BLARGG_EXPORT gme_err_t gme_seek_samples( Music_Emu*, int n );

/* Seek to new time in track (scaled with tempo).
 * @since 0.6.5 */
BLARGG_EXPORT gme_err_t gme_seek_scaled( Music_Emu*, int msec );
>>>>>>> db7344ebf (abc)


/******** Informational ********/

<<<<<<< HEAD
/* Use in place of sample rate for open/load if you only need to get track
information from a music file */
=======
/* If you only need track information from a music file, pass gme_info_only for
sample_rate to open/load. */
>>>>>>> db7344ebf (abc)
enum { gme_info_only = -1 };

/* Most recent warning string, or NULL if none. Clears current warning after returning.
Warning is also cleared when loading a file and starting a track. */
<<<<<<< HEAD
const char* gme_warning( gme_t* );

/* Loads m3u playlist file (must be done after loading music) */
gme_err_t gme_load_m3u( gme_t*, const char path [] );

/* Clears any loaded m3u playlist and any internal playlist that the music format
supports (NSFE for example). */
void gme_clear_playlist( gme_t* );

/* Passes back pointer to information for a particular track (length, name, author, etc.).
Must be freed after use. */
typedef struct gme_info_t gme_info_t;
gme_err_t gme_track_info( const gme_t*, gme_info_t** out, int track );

gme_err_t gme_set_track_info( gme_t*, const gme_info_t* in, int track );
        
/* Frees track information */
void gme_free_info( gme_info_t* );
=======
BLARGG_EXPORT const char* gme_warning( Music_Emu* );

/* Load m3u playlist file (must be done after loading music) */
BLARGG_EXPORT gme_err_t gme_load_m3u( Music_Emu*, const char path [] );

/* Clear any loaded m3u playlist and any internal playlist that the music format
supports (NSFE for example). */
BLARGG_EXPORT void gme_clear_playlist( Music_Emu* );

/* Gets information for a particular track (length, name, author, etc.).
Must be freed after use. */
typedef struct gme_info_t gme_info_t;
BLARGG_EXPORT gme_err_t gme_track_info( Music_Emu const*, gme_info_t** out, int track );

/* Frees track information */
BLARGG_EXPORT void gme_free_info( gme_info_t* );
>>>>>>> db7344ebf (abc)

struct gme_info_t
{
	/* times in milliseconds; -1 if unknown */
<<<<<<< HEAD
	int length;         /* total length, if file specifies it */
	int intro_length;   /* length of song up to looping section */
	int loop_length;    /* length of looping section */
	
	/* Length if available, otherwise intro_length+loop_length*2 if available,
	otherwise a default of 150000 (2.5 minutes). */
	int play_length;
	
	int i4,i5,i6,i7,i8,i9,i10,i11,i12,i13,i14,i15; /* reserved */
	
=======
	int length;			/* total length, if file specifies it */
	int intro_length;	/* length of song up to looping section */
	int loop_length;	/* length of looping section */

	/* Length if available, otherwise intro_length+loop_length*2 if available,
	otherwise a default of 150000 (2.5 minutes). */
	int play_length;

	/* fade length in milliseconds; -1 if unknown */
	int fade_length;

	int i5,i6,i7,i8,i9,i10,i11,i12,i13,i14,i15; /* reserved */

>>>>>>> db7344ebf (abc)
	/* empty string ("") if not available */
	const char* system;
	const char* game;
	const char* song;
	const char* author;
	const char* copyright;
	const char* comment;
	const char* dumper;
<<<<<<< HEAD
	
=======

>>>>>>> db7344ebf (abc)
	const char *s7,*s8,*s9,*s10,*s11,*s12,*s13,*s14,*s15; /* reserved */
};


/******** Advanced playback ********/

<<<<<<< HEAD
/* Disables automatic end-of-track detection and skipping of silence at beginning
if ignore is true */
void gme_ignore_silence( gme_t*, gme_bool ignore );

/* Adjusts song tempo, where 1.0 = normal, 0.5 = half speed, 2.0 = double speed, etc.
Track length as returned by track_info() ignores tempo (assumes it's 1.0). */
void gme_set_tempo( gme_t*, double tempo );

/* Number of voices used by currently loaded file */
int gme_voice_count( const gme_t* );

/* Name of voice i, from 0 to gme_voice_count() - 1 */
const char* gme_voice_name( const gme_t*, int i );

/* Mutes/unmutes single voice i, where voice 0 is first voice */
void gme_mute_voice( gme_t*, int index, gme_bool mute );

/* Sets muting state of ALL voices at once using a bit mask, where -1 mutes all
voices, 0 unmutes them all, 0x01 mutes just the first voice, etc. */
void gme_mute_voices( gme_t*, int muting_mask );

/* Frequency equalizer parameters (see gme.txt) */
=======
/* Adjust stereo echo depth, where 0.0 = off and 1.0 = maximum. Has no effect for
GYM, SPC, and Sega Genesis VGM music */
BLARGG_EXPORT void gme_set_stereo_depth( Music_Emu*, double depth );

/* Disable automatic end-of-track detection and skipping of silence at beginning
if ignore is true */
BLARGG_EXPORT void gme_ignore_silence( Music_Emu*, int ignore );

/* Adjust song tempo, where 1.0 = normal, 0.5 = half speed, 2.0 = double speed.
Track length as returned by track_info() assumes a tempo of 1.0. */
BLARGG_EXPORT void gme_set_tempo( Music_Emu*, double tempo );

/* Number of voices used by currently loaded file */
BLARGG_EXPORT int gme_voice_count( Music_Emu const* );

/* Name of voice i, from 0 to gme_voice_count() - 1 */
BLARGG_EXPORT const char* gme_voice_name( Music_Emu const*, int i );

/* Mute/unmute voice i, where voice 0 is first voice */
BLARGG_EXPORT void gme_mute_voice( Music_Emu*, int index, int mute );

/* Set muting state of all voices at once using a bit mask, where -1 mutes all
voices, 0 unmutes them all, 0x01 mutes just the first voice, etc. */
BLARGG_EXPORT void gme_mute_voices( Music_Emu*, int muting_mask );

/* Disable/Enable echo effect for SPC files */
/* Available since 0.6.4 */
BLARGG_EXPORT void gme_disable_echo( Music_Emu*, int disable );

/* Frequency equalizer parameters (see gme.txt) */
/* Implementers: If modified, also adjust Music_Emu::make_equalizer as needed */
>>>>>>> db7344ebf (abc)
typedef struct gme_equalizer_t
{
	double treble; /* -50.0 = muffled, 0 = flat, +5.0 = extra-crisp */
	double bass;   /* 1 = full bass, 90 = average, 16000 = almost no bass */
<<<<<<< HEAD
	
	double d2,d3,d4,d5,d6,d7,d8,d9; /* reserved */
} gme_equalizer_t;

/* Gets current frequency equalizer parameters */
void gme_equalizer( const gme_t*, gme_equalizer_t* out );

/* Changes frequency equalizer parameters */
void gme_set_equalizer( gme_t*, gme_equalizer_t const* eq );

/* stub to avoid ABI breakage, I think --Wyatt */
void gme_enable_accuracy( gme_t*, int enabled );

/******** Effects processor ********/

/* Adds stereo surround and echo to music that's usually mono or has little
stereo. Has no effect on GYM, SPC, and Sega Genesis VGM music. */

/* Simplified control using a single value, where 0.0 = off and 1.0 = maximum */
void gme_set_stereo_depth( gme_t*, double depth );

struct gme_effects_t
{
	double echo;    /* Amount of echo, where 0.0 = none, 1.0 = lots */
	double stereo;  /* Separation, where 0.0 = mono, 1.0 = hard left and right */
	
	double d2,d3,d4,d5,d6,d7; /* reserved */
	
	gme_bool enabled; /* If 0, no effects are added */
	gme_bool surround;/* If 1, some channels are put in "back", using phase inversion */
	
	int i1,i3,i4,i5,i6,i7; /* reserved */
};
typedef struct gme_effects_t gme_effects_t;

/* Sets effects configuration, or disables effects if NULL */
void gme_set_effects( gme_t*, gme_effects_t const* );

/* Passes back current effects configuration */
void gme_effects( const gme_t*, gme_effects_t* out );
=======

	double d2,d3,d4,d5,d6,d7,d8,d9; /* reserved */
} gme_equalizer_t;

/* Get current frequency equalizater parameters */
BLARGG_EXPORT void gme_equalizer( Music_Emu const*, gme_equalizer_t* out );

/* Change frequency equalizer parameters */
BLARGG_EXPORT void gme_set_equalizer( Music_Emu*, gme_equalizer_t const* eq );

/* Enables/disables most accurate sound emulation options */
BLARGG_EXPORT void gme_enable_accuracy( Music_Emu*, int enabled );
>>>>>>> db7344ebf (abc)


/******** Game music types ********/

/* Music file type identifier. Can also hold NULL. */
typedef const struct gme_type_t_* gme_type_t;

<<<<<<< HEAD
/* Type of this emulator */
gme_type_t gme_type( const gme_t* );

/* Pointer to array of all music types, with NULL entry at end. Allows a player linked
to this library to support new music types without having to be updated. */
gme_type_t const* gme_type_list();

/* Name of game system for this music file type */
const char* gme_type_system( gme_type_t );

/* True if this music file type supports multiple tracks */
gme_bool gme_type_multitrack( gme_type_t );


/******** Advanced file loading ********/

/* Same as gme_open_file(), but uses file data already in memory. Makes copy of data. */
gme_err_t gme_open_data( void const* data, long size, gme_t** emu_out, int sample_rate );

/* Determines likely game music type based on first four bytes of file. Returns
string containing proper file suffix ("NSF", "SPC", etc.) or "" if file header
is not recognized. */
const char* gme_identify_header( void const* header );

/* Gets corresponding music type for file path or extension passed in. */
gme_type_t gme_identify_extension( const char path_or_extension [] );

/* Determines file type based on file's extension or header (if extension isn't recognized).
Sets *type_out to type, or 0 if unrecognized or error. */
gme_err_t gme_identify_file( const char path [], gme_type_t* type_out );

/* Creates new emulator and sets sample rate. Returns NULL if out of memory. If you only need
track information, pass gme_info_only for sample_rate. */
gme_t* gme_new_emu( gme_type_t, int sample_rate );

/* Loads music file into emulator */
gme_err_t gme_load_file( gme_t*, const char path [] );

/* Loads music file from memory into emulator. Makes a copy of data passed. */
gme_err_t gme_load_data( gme_t*, void const* data, long size );

/* Loads music file using custom data reader function that will be called to
read file data. Most emulators load the entire file in one read call. */
typedef gme_err_t (*gme_reader_t)( void* your_data, void* out, long count );
gme_err_t gme_load_custom( gme_t*, gme_reader_t, long file_size, void* your_data );

/* Loads m3u playlist file from memory (must be done after loading music) */
gme_err_t gme_load_m3u_data( gme_t*, void const* data, long size );

        
/******** Saving ********/
typedef gme_err_t (*gme_writer_t)( void* your_data, void const* in, long count );
gme_err_t gme_save( gme_t const*, gme_writer_t, void* your_data );

/******** User data ********/

/* Sets/gets pointer to data you want to associate with this emulator.
You can use this for whatever you want. */
void  gme_set_user_data( gme_t*, void* new_user_data );
void* gme_user_data( const gme_t* );

/* Registers cleanup function to be called when deleting emulator, or NULL to
clear it. Passes user_data when calling cleanup function. */
typedef void (*gme_user_cleanup_t)( void* user_data );
void gme_set_user_cleanup( gme_t*, gme_user_cleanup_t func );


/******** Errors ********/

/* Internally, a gme_err_t is a const char* that points to a normal C string.
This means that other strings can be passed to the following functions. In the
descriptions below, these other strings are referred to as being not gme_err_t
strings. */

/* Error string associated with err. Returns "" if err is NULL. Returns err
unchanged if it isn't a gme_err_t string. */
const char* gme_err_str( gme_err_t err );

/* Details of error beyond main cause, or "" if none or err is NULL. Returns
err unchanged if it isn't a gme_err_t string. */
const char* gme_err_details( gme_err_t err );

/* Numeric code corresponding to err. Returns gme_ok if err is NULL. Returns
gme_err_generic if err isn't a gme_err_t string. */
int gme_err_code( gme_err_t err );

enum {
	gme_ok               =    0,/* Successful call. Guaranteed to be zero. */
	gme_err_generic      = 0x01,/* Error of unspecified type */
	gme_err_memory       = 0x02,/* Out of memory */
	gme_err_caller       = 0x03,/* Caller violated requirements of function */
	gme_err_internal     = 0x04,/* Internal problem, corruption, etc. */
	gme_err_limitation   = 0x05,/* Exceeded program limit */
	
	gme_err_file_missing = 0x20,/* File not found at specified path */
	gme_err_file_read    = 0x21,/* Couldn't open file for reading */
	gme_err_file_io      = 0x23,/* Read/write error */
	gme_err_file_eof     = 0x25,/* Tried to read past end of file */
	
	gme_err_file_type    = 0x30,/* File is of wrong type */
	gme_err_file_feature = 0x32,/* File requires unsupported feature */
	gme_err_file_corrupt = 0x33 /* File is corrupt */
};

/* gme_err_t corresponding to numeric code. Note that this might not recover
the original gme_err_t before it was converted to a numeric code; in
particular, gme_err_details(gme_code_to_err(code)) will be "" in most cases. */
gme_err_t gme_code_to_err( int code );



/* Deprecated */
typedef gme_t Music_Emu;

=======
/* Emulator type constants for each supported file type */
extern BLARGG_EXPORT const gme_type_t
	gme_ay_type,
	gme_gbs_type,
	gme_gym_type,
	gme_hes_type,
	gme_kss_type,
	gme_nsf_type,
	gme_nsfe_type,
	gme_sap_type,
	gme_spc_type,
	gme_vgm_type,
	gme_vgz_type;

/* Type of this emulator */
BLARGG_EXPORT gme_type_t gme_type( Music_Emu const* );

/* Pointer to array of all music types, with NULL entry at end. Allows a player linked
to this library to support new music types without having to be updated. */
BLARGG_EXPORT gme_type_t const* gme_type_list( void );

/* Name of game system for this music file type */
BLARGG_EXPORT const char* gme_type_system( gme_type_t );

/* True if this music file type supports multiple tracks */
BLARGG_EXPORT int gme_type_multitrack( gme_type_t );

/* whether the pcm output retrieved by gme_play() will have all 8 voices rendered to their
 * individual stereo channel or (if false) these voices get mixed into one single stereo channel
 * @since 0.6.3 */
BLARGG_EXPORT int gme_multi_channel( Music_Emu const* );

/******** Advanced file loading ********/

/* Error returned if file type is not supported */
extern BLARGG_EXPORT const char* const gme_wrong_file_type;

/* Same as gme_open_file(), but uses file data already in memory. Makes copy of data.
 * The resulting Music_Emu object will be set to single channel mode. */
BLARGG_EXPORT gme_err_t gme_open_data( void const* data, long size, Music_Emu** out, int sample_rate );

/* Determine likely game music type based on first four bytes of file. Returns
string containing proper file suffix (i.e. "NSF", "SPC", etc.) or "" if
file header is not recognized. */
BLARGG_EXPORT const char* gme_identify_header( void const* header );

/* Get corresponding music type for file path or extension passed in. */
BLARGG_EXPORT gme_type_t gme_identify_extension( const char path_or_extension [] );

/**
 * Get typical file extension for a given music type.  This is not a replacement
 * for a file content identification library (but see gme_identify_header).
 *
 * @since 0.6.3
 */
BLARGG_EXPORT const char* gme_type_extension( gme_type_t music_type );

/* Determine file type based on file's extension or header (if extension isn't recognized).
Sets *type_out to type, or 0 if unrecognized or error. */
BLARGG_EXPORT gme_err_t gme_identify_file( const char path [], gme_type_t* type_out );

/* Create new emulator and set sample rate. Returns NULL if out of memory. If you only need
track information, pass gme_info_only for sample_rate. */
BLARGG_EXPORT Music_Emu* gme_new_emu( gme_type_t, int sample_rate );

/* Create new multichannel emulator and set sample rate. Returns NULL if out of memory.
 * If you only need track information, pass gme_info_only for sample_rate.
 * (see gme_multi_channel for more information on multichannel support)
 * @since 0.6.3
 */
BLARGG_EXPORT Music_Emu* gme_new_emu_multi_channel( gme_type_t, int sample_rate );

/* Load music file into emulator */
BLARGG_EXPORT gme_err_t gme_load_file( Music_Emu*, const char path [] );

/* Load music file from memory into emulator. Makes a copy of data passed. */
BLARGG_EXPORT gme_err_t gme_load_data( Music_Emu*, void const* data, long size );

/* Load multiple single-track music files from memory into emulator.
 * @since 0.6.4
 */
BLARGG_EXPORT gme_err_t gme_load_tracks( Music_Emu* me,
                                         void const* data, long* sizes, int count );

/* Return the fixed track count of an emu file type
 * @since 0.6.4
 */
BLARGG_EXPORT int gme_fixed_track_count( gme_type_t );

/* Load music file using custom data reader function that will be called to
read file data. Most emulators load the entire file in one read call. */
typedef gme_err_t (*gme_reader_t)( void* your_data, void* out, int count );
BLARGG_EXPORT gme_err_t gme_load_custom( Music_Emu*, gme_reader_t, long file_size, void* your_data );

/* Load m3u playlist file from memory (must be done after loading music) */
BLARGG_EXPORT gme_err_t gme_load_m3u_data( Music_Emu*, void const* data, long size );


/******** User data ********/

/* Set/get pointer to data you want to associate with this emulator.
You can use this for whatever you want. */
BLARGG_EXPORT void  gme_set_user_data( Music_Emu*, void* new_user_data );
BLARGG_EXPORT void* gme_user_data( Music_Emu const* );

/* Register cleanup function to be called when deleting emulator, or NULL to
clear it. Passes user_data to cleanup function. */
typedef void (*gme_user_cleanup_t)( void* user_data );
BLARGG_EXPORT void gme_set_user_cleanup( Music_Emu*, gme_user_cleanup_t func );


>>>>>>> db7344ebf (abc)
#ifdef __cplusplus
	}
#endif

#endif
