#include "test.h"

TEST(test_api_create_context)
{
	xmp_context ctx;
	int state;

	ctx = xmp_create_context();
<<<<<<< HEAD
	fail_unless(ctx != 0, "returned NULL");
=======
	fail_unless(ctx != NULL, "returned NULL");
>>>>>>> db7344ebf (abc)

	state = xmp_get_player(ctx, XMP_PLAYER_STATE);
	fail_unless(state == XMP_STATE_UNLOADED, "state error");

	xmp_free_context(ctx);
}
END_TEST
