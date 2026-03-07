<<<<<<< HEAD
#include "test.h"
#include "../src/mixer.h"
#include "../src/virtual.h"
=======
#include "read_event_common.h"
>>>>>>> db7344ebf (abc)

/*
Case 2: New instrument (no note)

  Instrument -> None    Same    Valid   Inval
PT1.1           -       Play    Play    Cut
PT1.3           -       NewVol  NewVol* Cut
PT2.3           -       NewVol  NewVol* Cut
PT3.15          -       NewVol  NewVol  Cut     <= "Standard"
PT3.61          -       NewVol  NewVol  Cut     <=
PT4b2           -       NewVol  NewVol  Cut     <=
MED             -       Hold    Hold    Cut%
FT2             -       OldVol  OldVol  OldVol
ST3             -       NewVol  NewVol  Cont
IT(s)           -       NewVol  NewVol  Cont
IT(i)           -       NewVol# Play    Cont
DT32            -       NewVol# NewVol# Cut

Play    = Play new note with new default volume
Switch  = Play new note with current volume
NewVol  = Don't play sample, set new default volume
OldVol  = Don't play sample, set old default volume
Cont    = Continue playing sample
Cut     = Stop playing sample

<<<<<<< HEAD
  * Protracker 1.3/2.3 switches to new sample in the line after the new
    instrument event. The new instrument is not played from start (i.e. a
    short transient sample may not be played). This behaviour is NOT
    emulated by the current version of xmp.

    00 C-2 03 A0F  <=  Play instrument 03 and slide volume down
    01 --- 02 000  <=  Set volume of instrument 02, playing instrument 03
    02 --- 00 000  <=  Switch to instrument 02 (weird!)

    00 C-2 03 000  <=  Play instrument 03
    01 A-3 02 308  <=  Start portamento with instrument 03
    02 --- 00 xxx  <=  Switch to instrument 02 (weird!)
=======
  * Protracker 1.3/2.3 queues sample changes immediately, but they don't take
    effect until the current playing sample completes its loop. This is
    supported by libxmp, as it shouldn't significantly hurt PT3 compatibility.
>>>>>>> db7344ebf (abc)

  # Don't reset envelope.

*/

TEST(test_no_note_same_ins_it)
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
	new_event(ctx, 0, 1, 0,  0, 1,  0, 0x00, 0, 0, 0);
	set_quirk(ctx, QUIRKS_IT, READ_EVENT_IT);

	xmp_start_player(opaque, 44100, 0);
=======
	create_read_event_test_module(ctx, 2);
	new_event(ctx, 0, 0, 0, KEY_C5,       INS_0, 0, 0x00, 0, 0, 0);
	new_event(ctx, 0, 1, 0, 0,            0,     0, FX_VOLSET, SET_VOL,
							FX_SETPAN, SET_PAN);
	new_event(ctx, 0, 2, 0, 0,            INS_0, 0, 0x00, 0, 0, 0);
	new_event(ctx, 0, 3, 0, KEY_B5,       INS_0, 0, 0x00, 0, 0, 0);
	new_event(ctx, 0, 4, 0, 0,            0,     0, FX_VOLSET, SET_VOL,
							FX_SETPAN, SET_PAN);
	new_event(ctx, 0, 5, 0, 0,            INS_0, 0, 0x00, 0, 0, 0);
	set_quirk(ctx, QUIRKS_IT, READ_EVENT_IT);

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

	/* Row 1: valid instrument with no note (IT)
=======
	check_new(xc, vi, KEY_C5, INS_0,
		  INS_0_SUB_0_VOL, INS_0_SUB_0_PAN, INS_0_FADE, "row 0");

	xmp_play_frame(opaque);

	/* Row 1: set non-default volume and pan */
	xmp_play_frame(opaque);
	check_on(xc, vi, KEY_C5, INS_0,
		 SET_VOL, SET_PAN, INS_0_FADE, "row 1");

	xmp_play_frame(opaque);

	/* Row 2: valid instrument with no note (IT)
>>>>>>> db7344ebf (abc)
	 *
	 * When a new valid instrument is the same as the current instrument
	 * and no note is set, IT keeps playing the current sample but
	 * sets the volume to the instrument's default volume.
	 *
	 * FIXME: check if envelope is being reset
	 */
	xmp_play_frame(opaque);
<<<<<<< HEAD
	fail_unless(vi->ins  ==  0, "not original instrument");
	fail_unless(vi->note == 59, "not same note");
	fail_unless(vi->vol  == 22 * 16, "not instrument volume");
	fail_unless(vi->pos0 !=  0, "sample reset");
	xmp_play_frame(opaque);

=======
	check_on(xc, vi, KEY_C5, INS_0,
		 INS_0_SUB_0_VOL, -1 /* FIXME: INS_0_SUB_0_PAN */, INS_0_FADE, "row 2");

	xmp_play_frame(opaque);

	/* Row 3: same, except subinstrument 1 (IT) */
	xmp_play_frame(opaque);
	check_new(xc, vi, KEY_B5, INS_0,
		  INS_0_SUB_1_VOL, INS_0_SUB_1_PAN, INS_0_FADE, "row 3");

	xmp_play_frame(opaque);

	/* Row 4: set non-default volume and pan */
	xmp_play_frame(opaque);
	check_on(xc, vi, KEY_B5, INS_0,
		 SET_VOL, SET_PAN, INS_0_FADE, "row 4");

	xmp_play_frame(opaque);

	/* Row 5 */
	xmp_play_frame(opaque);
	check_on(xc, vi, KEY_B5, INS_0,
		 INS_0_SUB_1_VOL, -1 /* FIXME: INS_0_SUB_1_PAN */, INS_0_FADE, "row 5");

	xmp_play_frame(opaque);


>>>>>>> db7344ebf (abc)
	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
