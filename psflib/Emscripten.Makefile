# CFLAGS = -c -fPIC
CFLAGS = -c -g4 -s USE_ZLIB=1

OBJS = psflib.o psf2fs.o 


<<<<<<< HEAD
OPTS = -O2
=======
OPTS = -O2 
>>>>>>> db7344ebf (abc)

all: libpsflib.a

libpsflib.a : $(OBJS)
	$(AR) rcs $@ $^

.c.o:
	$(CC) $(CFLAGS) $(OPTS) $*.c

clean:
	rm -f $(OBJS) libpsflib.a > /dev/null

