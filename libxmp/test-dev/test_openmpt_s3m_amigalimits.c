#include "test.h"

/*
<<<<<<< HEAD
 If “Force Amiga Limits” is enabled, the limits should also be enfored on the
=======
 If “Force Amiga Limits” is enabled, the limits should also be enforced on the
>>>>>>> db7344ebf (abc)
 stored period, not only on the computed period.
*/

TEST(test_openmpt_s3m_amigalimits)
{
	compare_mixer_data(
		"openmpt/s3m/AmigaLimits.s3m",
		"openmpt/s3m/AmigaLimits.data");
}
END_TEST
