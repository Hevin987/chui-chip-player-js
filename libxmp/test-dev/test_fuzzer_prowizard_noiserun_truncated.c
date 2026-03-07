#include "test.h"

<<<<<<< HEAD
/* This input caused out-of-bounds reads in the ProWizard NoiseRunner
 * test function due to not requesting adequate pattern data.
 */
=======
>>>>>>> db7344ebf (abc)

TEST(test_fuzzer_prowizard_noiserun_truncated)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();
<<<<<<< HEAD
	ret = xmp_load_module(opaque, "data/f/prowizard_noiserun_truncated");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module load");
=======

	/* This input caused out-of-bounds reads in the ProWizard NoiseRunner
	 * test function due to not requesting adequate pattern data. */
	ret = xmp_load_module(opaque, "data/f/prowizard_noiserun_truncated_pattern");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module load (truncated_pattern)");
>>>>>>> db7344ebf (abc)

	xmp_free_context(opaque);
}
END_TEST
