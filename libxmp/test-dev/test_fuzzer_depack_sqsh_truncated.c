#include "test.h"

<<<<<<< HEAD
/* This input caused an out-of-bounds read in the SQSH depacker
 * due to an incorrect bounds check on an input buffer.
 */
=======
>>>>>>> db7344ebf (abc)

TEST(test_fuzzer_depack_sqsh_truncated)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();
<<<<<<< HEAD
	ret = xmp_load_module(opaque, "data/f/depack_sqsh_truncated.xpk");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking");
=======

	/* This input caused an out-of-bounds read in the XPK SQSH depacker
	 * due to an incorrect bounds check on an input buffer. */
	ret = xmp_load_module(opaque, "data/f/depack_sqsh_truncated.xpk");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (truncated)");

	/* This input caused an out-of-bounds read in the XPK SQSH depacker
	 * due to missing bounds checks when parsing the input bitstream. */
	ret = xmp_load_module(opaque, "data/f/depack_sqsh_truncated2.xpk");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (truncated2)");
>>>>>>> db7344ebf (abc)

	xmp_free_context(opaque);
}
END_TEST
