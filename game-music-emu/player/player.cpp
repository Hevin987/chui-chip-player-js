/* How to play game music files with Music_Player (requires SDL library)

Run program with path to a game music file.

Left/Right  Change track
Space       Pause/unpause
E           Normal/slight stereo echo/more stereo echo
<<<<<<< HEAD
A			Enable/disable accurate emulation
=======
D           Toggle echo processing
A           Enable/disable accurate emulation
H           Show help message
L           Toggle track looping (infinite playback)
>>>>>>> db7344ebf (abc)
-/=         Adjust tempo
1-9         Toggle channel on/off
0           Reset tempo and turn channels back on */

<<<<<<< HEAD
int const scope_width = 512;
=======
// Make ISO C99 symbols available for snprintf, define must be set before any
// system header includes
#define _ISOC99_SOURCE 1

static int const scope_width = 1024;
static int const scope_height = 512;
>>>>>>> db7344ebf (abc)

#include "Music_Player.h"
#include "Audio_Scope.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "SDL.h"

<<<<<<< HEAD
void handle_error( const char* );
=======
static const char *usage = R"(
Left/Right  Change track
Up/Down     Seek to one second forward/backward (if possible)
Space       Pause/unpause
E           Normal/slight stereo echo/more stereo echo
D           Toggle echo processing
A           Enable/disable accurate emulation
H           Show help message
L           Toggle track looping (infinite playback)
-/=         Adjust tempo
1-9         Toggle channel on/off
0           Reset tempo and turn channels back on */
)";

static void handle_error( const char* );
>>>>>>> db7344ebf (abc)

static bool paused;
static Audio_Scope* scope;
static Music_Player* player;
static short scope_buf [scope_width * 2];

<<<<<<< HEAD
static void init()
=======
static void init( void )
>>>>>>> db7344ebf (abc)
{
	// Start SDL
	if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
		exit( EXIT_FAILURE );
	atexit( SDL_Quit );
<<<<<<< HEAD
	SDL_EnableKeyRepeat( 500, 80 );
	
	// Init scope
	scope = new Audio_Scope;
	if ( !scope )
		handle_error( "Out of memory" );
	if ( scope->init( scope_width, 256 ) )
		handle_error( "Couldn't initialize scope" );
	memset( scope_buf, 0, sizeof scope_buf );
	
	// Create player
	player = new Music_Player;
=======

	// Init scope
	scope = GME_NEW Audio_Scope;
	if ( !scope )
		handle_error( "Out of memory" );
	std::string err_msg = scope->init( scope_width, scope_height );
	if ( !err_msg.empty() )
		handle_error( err_msg.c_str() );
	memset( scope_buf, 0, sizeof scope_buf );

	// Create player
	player = GME_NEW Music_Player;
>>>>>>> db7344ebf (abc)
	if ( !player )
		handle_error( "Out of memory" );
	handle_error( player->init() );
	player->set_scope_buffer( scope_buf, scope_width * 2 );
}

static void start_track( int track, const char* path )
{
	paused = false;
	handle_error( player->start_track( track - 1 ) );
<<<<<<< HEAD
	
	// update window title with track info
	
=======

	// update window title with track info

>>>>>>> db7344ebf (abc)
	long seconds = player->track_info().length / 1000;
	const char* game = player->track_info().game;
	if ( !*game )
	{
		// extract filename
		game = strrchr( path, '\\' ); // DOS
		if ( !game )
			game = strrchr( path, '/' ); // UNIX
		if ( !game )
			game = path;
		else
			game++; // skip path separator
	}
<<<<<<< HEAD
	
	char title [512];
	sprintf( title, "%s: %d/%d %s (%ld:%02ld)",
			game, track, player->track_count(), player->track_info().song,
			seconds / 60, seconds % 60 );
	SDL_WM_SetCaption( title, title );
=======

	char title [512];
	if ( 0 < snprintf( title, sizeof title, "%s: %d/%d %s (%ld:%02ld)",
			game, track, player->track_count(), player->track_info().song,
			seconds / 60, seconds % 60 ) )
	{
		scope->set_caption( title );
	}
>>>>>>> db7344ebf (abc)
}

int main( int argc, char** argv )
{
	init();
<<<<<<< HEAD
	
	// Load file
	const char* path = (argc > 1 ? argv [argc - 1] : "test.nsf");
	handle_error( player->load_file( path ) );
	start_track( 1, path );
	
=======

	bool by_mem = false;
	const char* path = "test.nsf";

	for ( int i = 1; i < argc; ++i )
	{
		if ( SDL_strcmp( "-m", argv[i] ) == 0 )
			by_mem = true;
		else
			path = argv[i];
	}

	// Load file
	handle_error( player->load_file( path, by_mem ) );
	start_track( 1, path );

>>>>>>> db7344ebf (abc)
	// Main loop
	int track = 1;
	double tempo = 1.0;
	bool running = true;
	double stereo_depth = 0.0;
	bool accurate = false;
<<<<<<< HEAD
	int muting_mask = 0;
	while ( running )
	{
		SDL_Delay( 1000 / 100 );
		
		// Update scope
		scope->draw( scope_buf, scope_width, 2 );
		
=======
	bool echo_disabled = false;
	bool fading_out = true;
	int muting_mask = 0;
	while ( running )
	{
		// Update scope
		scope->draw( scope_buf, scope_width, 2 );

>>>>>>> db7344ebf (abc)
		// Automatically go to next track when current one ends
		if ( player->track_ended() )
		{
			if ( track < player->track_count() )
				start_track( ++track, path );
			else
				player->pause( paused = true );
		}
<<<<<<< HEAD
		
=======

>>>>>>> db7344ebf (abc)
		// Handle keyboard input
		SDL_Event e;
		while ( SDL_PollEvent( &e ) )
		{
			switch ( e.type )
			{
			case SDL_QUIT:
				running = false;
				break;
<<<<<<< HEAD
			
			case SDL_KEYDOWN:
				int key = e.key.keysym.sym;
				switch ( key )
				{
				case SDLK_q:
				case SDLK_ESCAPE: // quit
					running = false;
					break;
				
				case SDLK_LEFT: // prev track
=======

			case SDL_KEYDOWN:
				int key = e.key.keysym.scancode;
				switch ( key )
				{
				case SDL_SCANCODE_Q:
				case SDL_SCANCODE_ESCAPE: // quit
					running = false;
					break;

				case SDL_SCANCODE_LEFT: // prev track
>>>>>>> db7344ebf (abc)
					if ( !paused && !--track )
						track = 1;
					start_track( track, path );
					break;
<<<<<<< HEAD
				
				case SDLK_RIGHT: // next track
					if ( track < player->track_count() )
						start_track( ++track, path );
					break;
				
				case SDLK_MINUS: // reduce tempo
=======

				case SDL_SCANCODE_RIGHT: // next track
					if ( track < player->track_count() )
						start_track( ++track, path );
					break;

				case SDL_SCANCODE_MINUS: // reduce tempo
>>>>>>> db7344ebf (abc)
					tempo -= 0.1;
					if ( tempo < 0.1 )
						tempo = 0.1;
					player->set_tempo( tempo );
					break;
<<<<<<< HEAD
				
				case SDLK_EQUALS: // increase tempo
=======

				case SDL_SCANCODE_EQUALS: // increase tempo
>>>>>>> db7344ebf (abc)
					tempo += 0.1;
					if ( tempo > 2.0 )
						tempo = 2.0;
					player->set_tempo( tempo );
					break;
<<<<<<< HEAD
				
				case SDLK_SPACE: // toggle pause
					paused = !paused;
					player->pause( paused );
					break;
				
				case SDLK_a: // toggle accurate emulation
					accurate = !accurate;
					player->enable_accuracy( accurate );
					break;
				
				case SDLK_e: // toggle echo
=======

				case SDL_SCANCODE_SPACE: // toggle pause
					paused = !paused;
					player->pause( paused );
					break;

				case SDL_SCANCODE_A: // toggle accurate emulation
					accurate = !accurate;
					player->enable_accuracy( accurate );
					break;

				case SDL_SCANCODE_E: // toggle echo
>>>>>>> db7344ebf (abc)
					stereo_depth += 0.2;
					if ( stereo_depth > 0.5 )
						stereo_depth = 0;
					player->set_stereo_depth( stereo_depth );
					break;
<<<<<<< HEAD
				
				case SDLK_0: // reset tempo and muting
=======

				case SDL_SCANCODE_D: // toggle echo on/off
					echo_disabled = !echo_disabled;
					player->set_echo_disable(echo_disabled);
					printf( "%s\n", echo_disabled ? "SPC echo is disabled" : "SPC echo is enabled" );
					fflush( stdout );
					break;

				case SDL_SCANCODE_L: // toggle loop
					player->set_fadeout( fading_out = !fading_out );
					printf( "%s\n", fading_out ? "Will stop at track end" : "Playing forever" );
					fflush( stdout );
					break;

				case SDL_SCANCODE_0: // reset tempo and muting
>>>>>>> db7344ebf (abc)
					tempo = 1.0;
					muting_mask = 0;
					player->set_tempo( tempo );
					player->mute_voices( muting_mask );
					break;
<<<<<<< HEAD
				
				default:
					if ( SDLK_1 <= key && key <= SDLK_9 ) // toggle muting
					{
						muting_mask ^= 1 << (key - SDLK_1);
=======

				case SDL_SCANCODE_DOWN: // Seek back
					player->seek_backward();
					break;

				case SDL_SCANCODE_UP: // Seek forward
					player->seek_forward();
					break;

				case SDL_SCANCODE_H: // help
					printf( "%s\n", usage );
					fflush( stdout );
					break;

				default:
					if ( SDL_SCANCODE_1 <= key && key <= SDL_SCANCODE_9 ) // toggle muting
					{
						muting_mask ^= 1 << (key - SDL_SCANCODE_1);
>>>>>>> db7344ebf (abc)
						player->mute_voices( muting_mask );
					}
				}
			}
		}
<<<<<<< HEAD
	}
	
	// Cleanup
	delete player;
	delete scope;
	
	return 0;
}

void handle_error( const char* error )
=======

		SDL_Delay( 1000 / 100 ); // Sets 'frame rate'
	}

	// Cleanup
	delete player;
	delete scope;

	return 0;
}

static void handle_error( const char* error )
>>>>>>> db7344ebf (abc)
{
	if ( error )
	{
		// put error in window title
		char str [256];
		sprintf( str, "Error: %s", error );
		fprintf( stderr, "%s\n", str );
<<<<<<< HEAD
		SDL_WM_SetCaption( str, str );
		
=======
		scope->set_caption( str );

>>>>>>> db7344ebf (abc)
		// wait for keyboard or mouse activity
		SDL_Event e;
		do
		{
			while ( !SDL_PollEvent( &e ) ) { }
		}
		while ( e.type != SDL_QUIT && e.type != SDL_KEYDOWN && e.type != SDL_MOUSEBUTTONDOWN );

		exit( EXIT_FAILURE );
	}
}
