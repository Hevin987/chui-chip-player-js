#ifndef LIBXMP_MEMIO_H
#define LIBXMP_MEMIO_H

#include <stddef.h>
#include "common.h"

typedef struct {
	const unsigned char *start;
	ptrdiff_t pos;
	ptrdiff_t size;
<<<<<<< HEAD
	int free_after_use;
=======
	void *ptr_free;
>>>>>>> db7344ebf (abc)
} MFILE;

LIBXMP_BEGIN_DECLS

<<<<<<< HEAD
MFILE  *mopen(const void *, long, int);
=======
MFILE  *mopen(void *, long, int);
MFILE  *mcopen(const void *, long);
>>>>>>> db7344ebf (abc)
int     mgetc(MFILE *stream);
size_t  mread(void *, size_t, size_t, MFILE *);
int     mseek(MFILE *, long, int);
long    mtell(MFILE *);
int     mclose(MFILE *);
int	meof(MFILE *);

LIBXMP_END_DECLS

#endif
