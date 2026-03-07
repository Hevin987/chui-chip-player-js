<<<<<<< HEAD
#ifndef XMP_PLATFORM_H
#define XMP_PLATFORM_H
=======
#ifndef LIBXMP_TEMPFILE_H
#define LIBXMP_TEMPFILE_H

#include "common.h"

LIBXMP_BEGIN_DECLS
>>>>>>> db7344ebf (abc)

FILE *make_temp_file(char **);
void unlink_temp_file(char *);

<<<<<<< HEAD
=======
#ifndef HAVE_MKSTEMP
int libxmp_mkstemp(char *);
#define mkstemp libxmp_mkstemp
#endif

LIBXMP_END_DECLS

>>>>>>> db7344ebf (abc)
#endif
