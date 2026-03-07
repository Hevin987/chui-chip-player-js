#include "test.h"

<<<<<<< HEAD
/* This input caused UMRs in the Galaxy 4.0 loader due to
 * not checking the hio_read return value for the module title.
 */
=======
>>>>>>> db7344ebf (abc)

TEST(test_fuzzer_gal4_truncated)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();
<<<<<<< HEAD
	ret = xmp_load_module(opaque, "data/f/load_gal4_truncated");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");
=======

	/* This input caused UMRs in the Galaxy 4.0 loader due to
	 * not checking the hio_read return value for the module title. */
	ret = xmp_load_module(opaque, "data/f/load_gal4_truncated");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (truncated)");

	/* This input caused UMRs in the Galaxy 4.0 loader due to
	 * not checking the hio_read return value for the volume envelope. */
	ret = xmp_load_module(opaque, "data/f/load_gal4_truncated_env");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (truncated_env)");

	/* Same, but the pan envelope instead. */
	ret = xmp_load_module(opaque, "data/f/load_gal4_truncated_env2");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (truncated_env2)");
>>>>>>> db7344ebf (abc)

	xmp_free_context(opaque);
}
END_TEST
