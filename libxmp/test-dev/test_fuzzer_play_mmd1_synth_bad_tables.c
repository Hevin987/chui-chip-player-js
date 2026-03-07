#include "test.h"

/* These inputs caused crashes in the MED/OctaMED synth interpreter
 * due to libxmp not checking for out-of-bounds indexes. They play
 * "correctly" in OctaMED 4.
 */

TEST(test_fuzzer_play_mmd1_synth_bad_tables)
{
	static const struct playback_sequence sequence[] =
	{
<<<<<<< HEAD
		{ PLAY_FRAMES,	4, 0 },
=======
		{ PLAY_FRAMES,	6, 0 },
>>>>>>> db7344ebf (abc)
		{ PLAY_END,	0, 0 }
	};
	compare_playback("data/f/play_mmd1_synth_bad_wavtable.med", sequence, 4000, 0, 0);
	compare_playback("data/f/play_mmd1_synth_bad_voltable.med", sequence, 4000, 0, 0);
	/* libxmp could also hang on improperly terminated arpeggios. */
	compare_playback("data/f/play_mmd1_synth_bad_arpeggio.med", sequence, 4000, 0, 0);
<<<<<<< HEAD
=======
	/* ...and the arpeggio indices weren't bounded, either. */
	compare_playback("data/f/play_mmd1_synth_bad_arpeggio2.med", sequence, 4000, 0, 0);
	compare_playback("data/f/play_mmd1_synth_bad_arpeggio3.med", sequence, 4000, 0, 0);
>>>>>>> db7344ebf (abc)
}
END_TEST
