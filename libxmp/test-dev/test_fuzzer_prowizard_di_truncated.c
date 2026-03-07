#include "test.h"

<<<<<<< HEAD
/* This input crashed the ProWizard DI test function due to
 * bad PW_REQUEST_DATA usage.
 */
=======
>>>>>>> db7344ebf (abc)

TEST(test_fuzzer_prowizard_di_truncated)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();
<<<<<<< HEAD
	ret = xmp_load_module(opaque, "data/f/prowizard_di_truncated");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module load");
=======

	/* This input crashed the ProWizard DI test function due to
	 * bad PW_REQUEST_DATA usage and truncated sample headers. */
	ret = xmp_load_module(opaque, "data/f/prowizard_di_truncated_sample");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module load (truncated_sample)");

	/* This input crashed the ProWizard DI test function
	 * due to reading a byte past the end of the file buffer. */
	ret = xmp_load_module(opaque, "data/f/prowizard_di_truncated_pattern.xz");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module load (truncated_pattern)");
>>>>>>> db7344ebf (abc)

	xmp_free_context(opaque);
}
END_TEST
