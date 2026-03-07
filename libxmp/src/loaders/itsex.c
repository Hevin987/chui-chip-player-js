<<<<<<< HEAD
#ifndef LIBXMP_CORE_DISABLE_IT

/* Public domain IT sample decompressor by Olivier Lapicque */

#include "loader.h"
#include "it.h"

static inline uint32 read_bits(HIO_HANDLE *ibuf, uint32 *bitbuf, int *bitnum, int n, int *err)
{
	uint32 retval = 0;
	int i = n;
	int bnum = *bitnum;
	uint32 bbuf = *bitbuf;

	if (n > 0 && n <= 32) {
		do {
			if (bnum == 0) {
				if (hio_eof(ibuf)) {
					*err = EOF;
					return 0;
				}
				bbuf = hio_read8(ibuf);
				bnum = 8;
			}
			retval >>= 1;
			retval |= bbuf << 31;
			bbuf >>= 1;
			bnum--;
			i--;
		} while (i != 0);

		i = n;

		*bitnum = bnum;
		*bitbuf = bbuf;
	} else {
		/* Invalid shift value. */
		*err = -2;
		return 0;
	}

	return (retval >> (32 - i));
}


int itsex_decompress8(HIO_HANDLE *src, uint8 *dst, int len, int it215)
{
	/* uint32 size = 0; */
	uint32 block_count = 0;
	uint32 bitbuf = 0;
	int bitnum = 0;
	uint8 left = 0, temp = 0, temp2 = 0;
	uint32 d, pos;
	int err = 0;
=======
/* Public domain IT sample decompressor by Olivier Lapicque */

/* Modified by Alice Rowan (2023-2024)- more or less complete
 * rewrite of the input stream to add buffering.
 */

#include "../common.h"

#ifndef LIBXMP_CORE_DISABLE_IT

#include "loader.h"
#include "it.h"

#define READ_BITS_MASK(n) ((1u << (unsigned)(n)) - 1u)

struct it_stream
{
	uint8 *pos;
	size_t left;
	uint32 bits;
	int num_bits;
	int err;
};

static inline uint32 read_bits(struct it_stream *in, int n)
{
	uint32 retval = 0;

	if (n <= 0 || n >= 32) {
		/* Invalid shift value. */
		in->err = -2;
		return 0;
	}

	retval = in->bits & READ_BITS_MASK(n);

	if (in->num_bits < n) {
		uint32 offset = in->num_bits;
		uint32 used;

		if (in->left == 0) {
			in->err = EOF;
			return 0;
		}
		/* Buffer should be zero-padded to 4-byte alignment. */
		in->bits = in->pos[0] |
			   (in->pos[1] << 8) |
			   (in->pos[2] << 16) |
			   ((uint32)in->pos[3] << 24);

		used = MIN(in->left, 4);

		in->num_bits = used * 8;
		in->pos += 4;
		in->left -= used;

		n -= offset;
		retval |= (in->bits & READ_BITS_MASK(n)) << offset;
	}

	in->bits >>= n;
	in->num_bits -= n;

	return retval;
}

static inline int init_block(struct it_stream *in, uint8 *tmp, int tmplen,
			     HIO_HANDLE *src)
{
	size_t i;
	in->pos = tmp;
	in->left = hio_read16l(src);
	in->bits = 0;
	in->num_bits = 0;
	in->err = 0;

	/* tmp should be INT16_MAX rounded up to a multiple of 4 bytes long. */
	if (tmplen < (int)((in->left + 4) & ~3))
		return -1;
	if (hio_read(tmp, 1, in->left, src) < in->left)
		return -1;

	/* Zero pad to a multiple of 4 bytes for read_bits. */
	for (i = in->left; i & 3; i++)
		tmp[i] = 0;

	return 0;
}

int itsex_decompress8(HIO_HANDLE *src, uint8 *dst, int len,
		      uint8 *tmp, int tmplen, int it215)
{
	struct it_stream in;
	uint32 block_count = 0;
	uint8 left = 0, temp = 0, temp2 = 0;
	uint32 d, pos;

	memset(&in, 0, sizeof(in)); /* bogus GCC 12 -Wmaybe-uninitialized */
>>>>>>> db7344ebf (abc)

	while (len) {
		if (!block_count) {
			block_count = 0x8000;
<<<<<<< HEAD
			/*size =*/ hio_read16l(src);
			left = 9;
			temp = temp2 = 0;
			bitbuf = bitnum = 0;
=======
			left = 9;
			temp = temp2 = 0;

			if (init_block(&in, tmp, tmplen, src) < 0)
				return -1;
>>>>>>> db7344ebf (abc)
		}

		d = block_count;
		if (d > len)
			d = len;

		/* Unpacking */
		pos = 0;
		do {
<<<<<<< HEAD
			uint16 bits = read_bits(src, &bitbuf, &bitnum, left, &err);
			if (err != 0)
=======
			uint16 bits = read_bits(&in, left);
			if (in.err)
>>>>>>> db7344ebf (abc)
				return -1;

			if (left < 7) {
				uint32 i = 1 << (left - 1);
				uint32 j = bits & 0xffff;
				if (i != j)
					goto unpack_byte;
<<<<<<< HEAD
				bits = (read_bits(src, &bitbuf, &bitnum, 3, &err)
								+ 1) & 0xff;
				if (err != 0)
=======
				bits = (read_bits(&in, 3) + 1) & 0xff;
				if (in.err)
>>>>>>> db7344ebf (abc)
					return -1;

				left = ((uint8)bits < left) ?  (uint8)bits :
						(uint8)((bits + 1) & 0xff);
				goto next;
			}

			if (left < 9) {
				uint16 i = (0xff >> (9 - left)) + 4;
				uint16 j = i - 8;

				if ((bits <= j) || (bits > i))
					goto unpack_byte;

				bits -= j;
				left = ((uint8)(bits & 0xff) < left) ?
						(uint8)(bits & 0xff) :
						(uint8)((bits + 1) & 0xff);
				goto next;
			}

			if (left >= 10)
				goto skip_byte;

			if (bits >= 256) {
				left = (uint8) (bits + 1) & 0xff;
				goto next;
			}

		    unpack_byte:
			if (left < 8) {
				uint8 shift = 8 - left;
				signed char c = (signed char)(bits << shift);
				c >>= shift;
				bits = (uint16) c;
			}
			bits += temp;
			temp = (uint8)bits;
			temp2 += temp;
			dst[pos] = it215 ? temp2 : temp;

		    skip_byte:
			pos++;

		    next:
			/* if (slen <= 0)
				return -1 */;
		} while (pos < d);

		/* Move On */
		block_count -= d;
		len -= d;
		dst += d;
	}

	return 0;
}

<<<<<<< HEAD
int itsex_decompress16(HIO_HANDLE *src, int16 *dst, int len, int it215)
{
	/* uint32 size = 0; */
	uint32 block_count = 0;
	uint32 bitbuf = 0;
	int bitnum = 0;
	uint8 left = 0;
	int16 temp = 0, temp2 = 0;
	uint32 d, pos;
	int err = 0;
=======
int itsex_decompress16(HIO_HANDLE *src, int16 *dst, int len,
		       uint8 *tmp, int tmplen, int it215)
{
	struct it_stream in;
	uint32 block_count = 0;
	uint8 left = 0;
	int16 temp = 0, temp2 = 0;
	uint32 d, pos;

	memset(&in, 0, sizeof(in)); /* bogus GCC 12 -Wmaybe-uninitialized */
>>>>>>> db7344ebf (abc)

	while (len) {
		if (!block_count) {
			block_count = 0x4000;
<<<<<<< HEAD
			/*size =*/ hio_read16l(src);
			left = 17;
			temp = temp2 = 0;
			bitbuf = bitnum = 0;
=======
			left = 17;
			temp = temp2 = 0;

			if (init_block(&in, tmp, tmplen, src) < 0)
				return -1;
>>>>>>> db7344ebf (abc)
		}

		d = block_count;
		if (d > len)
			d = len;

		/* Unpacking */
		pos = 0;
		do {
<<<<<<< HEAD
			uint32 bits = read_bits(src, &bitbuf, &bitnum, left, &err);
			if (err != 0)
=======
			uint32 bits = read_bits(&in, left);
			if (in.err)
>>>>>>> db7344ebf (abc)
				return -1;

			if (left < 7) {
				uint32 i = 1 << (left - 1);
				uint32 j = bits;

				if (i != j)
					goto unpack_byte;

<<<<<<< HEAD
				bits = read_bits(src, &bitbuf, &bitnum, 4, &err) + 1;
				if (err != 0)
=======
				bits = read_bits(&in, 4) + 1;
				if (in.err)
>>>>>>> db7344ebf (abc)
					return -1;

				left = ((uint8)(bits & 0xff) < left) ?
						(uint8)(bits & 0xff) :
						(uint8)((bits + 1) & 0xff);
				goto next;
			}

			if (left < 17) {
				uint32 i = (0xffff >> (17 - left)) + 8;
				uint32 j = (i - 16) & 0xffff;

				if ((bits <= j) || (bits > (i & 0xffff)))
					goto unpack_byte;

				bits -= j;
				left = ((uint8)(bits & 0xff) < left) ?
						(uint8)(bits & 0xff) :
						(uint8)((bits + 1) & 0xff);
				goto next;
			}

			if (left >= 18)
				goto skip_byte;

			if (bits >= 0x10000) {
				left = (uint8)(bits + 1) & 0xff;
				goto next;
			}

		    unpack_byte:
			if (left < 16) {
				uint8 shift = 16 - left;
				int16 c = (int16)(bits << shift);
				c >>= shift;
				bits = (uint32) c;
			}
			bits += temp;
			temp = (int16)bits;
			temp2 += temp;
			dst[pos] = (it215) ? temp2 : temp;

		    skip_byte:
			pos++;

		    next:
			/* if (slen <= 0)
				return -1 */;
		} while (pos < d);

		/* Move On */
		block_count -= d;
		len -= d;
		dst += d;
		if (len <= 0)
			break;
	}

	return 0;
}

#endif /* LIBXMP_CORE_DISABLE_IT */
