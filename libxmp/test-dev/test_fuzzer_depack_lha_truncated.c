#include "test.h"

<<<<<<< HEAD
/* This input caused an out-of-bounds write in the LHA depacker
 * due to interpreting EOFs as valid input data.
 */
=======
>>>>>>> db7344ebf (abc)

TEST(test_fuzzer_depack_lha_truncated)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();
<<<<<<< HEAD
=======

	/* This input caused an out-of-bounds write in the LHA depacker
	 * due to interpreting EOFs as valid input data. */
>>>>>>> db7344ebf (abc)
	ret = xmp_load_module(opaque, "data/f/depack_lha_truncated.lha");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking");

	xmp_free_context(opaque);
}
END_TEST
