<<<<<<< HEAD
#include "test.h"
#include "../src/mixer.h"
#include "../src/virtual.h"
=======
#include "read_event_common.h"
>>>>>>> db7344ebf (abc)

/*
Case 1: New note

  Instrument -> None    Same    Valid   Inval
PT1.1           Play    Play    Play    Cut
PT1.3           Play    Play    Play    Cut
PT2.3           Switch  Play    Play    Cut     <=
PT3.15          Switch  Play    Play    Cut     <= "Standard"
PT3.61          Switch  Play    Play    Cut     <=
PT4b2           Switch  Play    Play    Cut     <=
MED             Switch  Play    Play    Cut     <=
FT2             Switch  Play    Play    Cut     <=
ST3             Switch  Play    Play    Switch
IT(s)           Switch  Play    Play    ?
IT(i)           Switch  Play    Play    Cont
DT32            Play    Play    Play    Cut

Play    = Play new note with new default volume
Switch  = Play new note with current volume
NewVol  = Don't play sample, set new default volume
OldVol  = Don't play sample, set old default volume
Cont    = Continue playing sample
Cut     = Stop playing sample

*/

TEST(test_new_note_no_ins_mod)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct player_data *p;
<<<<<<< HEAD
=======
	struct channel_data *xc;
>>>>>>> db7344ebf (abc)
	struct mixer_voice *vi;
	int voc;

	opaque = xmp_create_context();
	ctx = (struct context_data *)opaque;
	p = &ctx->p;

<<<<<<< HEAD
 	create_simple_module(ctx, 2, 2);
	set_instrument_volume(ctx, 0, 0, 22);
	set_instrument_volume(ctx, 1, 0, 33);
	new_event(ctx, 0, 0, 0, 60, 1, 44, 0x0f, 2, 0, 0);
	new_event(ctx, 0, 1, 0, 50, 0,  0, 0x00, 0, 0, 0);

	xmp_start_player(opaque, 44100, 0);
=======
	create_read_event_test_module(ctx, 2);
	new_event(ctx, 0, 0, 0, KEY_C5, INS_0, 0, 0x00, 0, 0, 0);
	new_event(ctx, 0, 1, 0, 0,      0,     0, FX_VOLSET, SET_VOL,
						  FX_SETPAN, SET_PAN);
	new_event(ctx, 0, 2, 0, KEY_D4, 0,     0, 0x00, 0, 0, 0);

	xmp_start_player(opaque, XMP_MIN_SRATE, 0);
>>>>>>> db7344ebf (abc)

	/* Row 0 */
	xmp_play_frame(opaque);

<<<<<<< HEAD
=======
	xc = &p->xc_data[0];
>>>>>>> db7344ebf (abc)
	voc = map_channel(p, 0);
	fail_unless(voc >= 0, "virtual map");
	vi = &p->virt.voice_array[voc];

<<<<<<< HEAD
	fail_unless(vi->note == 59, "set note");
	fail_unless(vi->ins  ==  0, "set instrument");
	fail_unless(vi->vol  == 43 * 16, "set volume");
	fail_unless(vi->pos0 ==  0, "sample position");

	xmp_play_frame(opaque);

	/* Row 1: no instrument with new note (PT 3.15)
=======
	check_new(xc, vi, KEY_C5, INS_0,
		  INS_0_SUB_0_VOL, INS_0_SUB_0_PAN, INS_0_FADE, "row 0");

	xmp_play_frame(opaque);

	/* Row 1: set non-default volume and pan */
	xmp_play_frame(opaque);
	check_on(xc, vi, KEY_C5, INS_0,
		 SET_VOL, SET_PAN, INS_0_FADE, "row 1");

	xmp_play_frame(opaque);

	/* Row 2: no instrument with new note (PT 3.15)
>>>>>>> db7344ebf (abc)
	 *
	 * When no instrument and a new note is set, PT3.15 plays
	 * the new note with old sample and current volume.
	 */
	xmp_play_frame(opaque);
<<<<<<< HEAD
	fail_unless(vi->ins  ==  0, "not old instrument");
	fail_unless(vi->note == 49, "not new note");
	fail_unless(vi->vol  == 43 * 16, "not current volume");
	fail_unless(vi->pos0 ==  0, "sample didn't reset");
=======
	check_new(xc, vi, KEY_D4, INS_0,
		  SET_VOL, SET_PAN, INS_0_FADE, "row 2");

>>>>>>> db7344ebf (abc)
	xmp_play_frame(opaque);

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
