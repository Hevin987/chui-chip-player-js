#include "test.h"


TEST(test_fuzzer_depack_pp20_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

<<<<<<< HEAD
	/* This input caused invalid shifts in the PowerPacker 2.0 unpacker
	 * due to an invalid number of skip bits.
	 */
	ret = xmp_load_module(opaque, "data/f/depack_zip_truncated.zip");
=======
	/* This input caused invalid shifts in the PowerPacker 2.0
	 * depacker due to an invalid number of skip bits. */
	ret = xmp_load_module(opaque, "data/f/depack_pp20_invalid");
>>>>>>> db7344ebf (abc)
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking");

	xmp_free_context(opaque);
}
END_TEST
