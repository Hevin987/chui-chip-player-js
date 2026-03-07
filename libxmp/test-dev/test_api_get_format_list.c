#include "test.h"

TEST(test_api_get_format_list)
{
	const char *const *list;
	int i;

	list = xmp_get_format_list();
<<<<<<< HEAD
	fail_unless(list != 0, "returned NULL");
=======
	fail_unless(list != NULL, "returned NULL");
>>>>>>> db7344ebf (abc)

	for (i = 0; list[i] != NULL; i++) {
		fail_unless(*list[i] != 0, "empty format name");
	}

<<<<<<< HEAD
	fail_unless(i == 93, "wrong number of formats");
=======
	fail_unless(i == 95, "wrong number of formats");
>>>>>>> db7344ebf (abc)
}
END_TEST
