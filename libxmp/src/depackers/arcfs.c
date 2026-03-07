<<<<<<< HEAD
/* ArcFS depacker for xmp
 * Copyright (C) 2007 Claudio Matsuoka
 *
 * Based on the nomarch .arc depacker from nomarch
 * Copyright (C) 2001-2006 Russell Marks
 *
 * This file is part of the Extended Module Player and is distributed
 * under the terms of the GNU Lesser General Public License. See COPYING.LIB
 * for more information.
 */

#include "../common.h"
#include "depacker.h"
#include "readlzw.h"


struct archived_file_header_tag {
	unsigned char method;
	unsigned char bits;
	char name[13];
	unsigned int compressed_size;
	unsigned int date, time, crc;
	unsigned int orig_size;
	unsigned int offset;
};


static int read_file_header(HIO_HANDLE *in, struct archived_file_header_tag *hdrp)
{
	int hlen, start /*, ver*/;
	int i;

	if (hio_seek(in, 8, SEEK_CUR) < 0)		/* skip magic */
		return -1;
	hlen = hio_read32l(in) / 36;
	if (hio_error(in) != 0) return -1;
	if (hlen < 1) return -1;
	start = hio_read32l(in);
	if (hio_error(in) != 0) return -1;
	/*ver =*/ hio_read32l(in);
	if (hio_error(in) != 0) return -1;

	hio_read32l(in);
	if (hio_error(in) != 0) return -1;
	/*ver =*/ hio_read32l(in);
	if (hio_error(in) != 0) return -1;

	if (hio_seek(in, 68, SEEK_CUR) < 0)	/* reserved */
		return -1;

	for (i = 0; i < hlen; i++) {
		int x = hio_read8(in);
		if (hio_error(in) != 0) return -1;

		if (x == 0)			/* end? */
			break;

		hdrp->method = x & 0x7f;
		if (hio_read(hdrp->name, 1, 11, in) != 11) {
			return -1;
		}
		hdrp->name[12] = 0;
		hdrp->orig_size = hio_read32l(in);
		if (hio_error(in) != 0) return -1;
		hio_read32l(in);
		if (hio_error(in) != 0) return -1;
		hio_read32l(in);
		if (hio_error(in) != 0) return -1;
		x = hio_read32l(in);
		if (hio_error(in) != 0) return -1;
		hdrp->compressed_size = hio_read32l(in);
		if (hio_error(in) != 0) return -1;
		hdrp->offset = hio_read32l(in);
		if (hio_error(in) != 0) return -1;

		if (x == 1)			/* deleted */
			continue;

		if (hdrp->offset & 0x80000000)		/* directory */
			continue;

		hdrp->crc = x >> 16;
		hdrp->bits = (x & 0xff00) >> 8;
		hdrp->offset &= 0x7fffffff;
		hdrp->offset += start;

		/* Max allowed compression bits value is 16 for method FFh. */
		if (hdrp->method > 2 && hdrp->bits > 16)
			return -1;

		return 0;
	}

	/* no usable files */
	return -1;
}

/* read file data, assuming header has just been read from in
 * and hdrp's data matches it. Caller is responsible for freeing
 * the memory allocated.
 * Returns NULL for file I/O error only; OOM is fatal (doesn't return).
 */
static unsigned char *read_file_data(HIO_HANDLE *in, long inlen,
				     struct archived_file_header_tag *hdrp)
{
	unsigned char *data;
	int siz = hdrp->compressed_size;

	/* Precheck: if the file can't hold this size, don't bother. */
	if (siz <= 0 || inlen < siz)
		return NULL;

	data = (unsigned char *) malloc(siz);
	if (data == NULL) {
		goto err;
	}
	if (hio_seek(in, hdrp->offset, SEEK_SET) < 0) {
		goto err2;
	}
	if (hio_read(data, 1, siz, in) != siz) {
		goto err2;
	}

	return data;

    err2:
	free(data);
    err:
	return NULL;
}

static int arcfs_extract(HIO_HANDLE *in, void **out, long inlen, long *outlen)
{
	struct archived_file_header_tag hdr;
	unsigned char *data, *orig_data;

	if (read_file_header(in, &hdr) < 0)
		return -1;

	if (hdr.method == 0)
		return -1;

	/* error reading data (hit EOF) */
	if ((data = read_file_data(in, inlen, &hdr)) == NULL)
		return -1;

	orig_data = NULL;

	/* FWIW, most common types are (by far) 8/9 and 2.
	 * (127 is the most common in Spark archives, but only those.)
	 * 3 and 4 crop up occasionally. 5 and 6 are very, very rare.
	 * And I don't think I've seen a *single* file with 1 or 7 yet.
	 */
	switch (hdr.method) {
	case 2:		/* no compression */
		if (hdr.orig_size != hdr.compressed_size) {
			free(data);
			return -1;
		}
		orig_data = data;
		break;

	case 8:		/* "Crunched" [sic]
			 * (RLE+9-to-12-bit dynamic LZW, a *bit* like GIF) */
		orig_data = libxmp_convert_lzw_dynamic(data, hdr.bits, 1,
					hdr.compressed_size, hdr.orig_size, 0);
		break;

	case 9:		/* "Squashed" (9-to-13-bit, no RLE) */
		orig_data = libxmp_convert_lzw_dynamic(data, hdr.bits, 0,
					hdr.compressed_size, hdr.orig_size, 0);
		break;

	case 127:	/* "Compress" (9-to-16-bit, no RLE) ("Spark" only) */
		orig_data = libxmp_convert_lzw_dynamic(data, hdr.bits, 0,
					hdr.compressed_size, hdr.orig_size, 0);
		break;

	default:
		free(data);
		return -1;
	}

	if (orig_data == NULL) {
		free(data);
		return -1;
	}

	if (orig_data != data)	/* don't free uncompressed stuff twice :-) */
		free(data);

	*out = orig_data;
	*outlen = hdr.orig_size;

	return 0;
}

static int test_arcfs(unsigned char *b)
{
	return !memcmp(b, "Archive\0", 8);
}

static int decrunch_arcfs(HIO_HANDLE *f, void **out, long inlen, long *outlen)
{
	return arcfs_extract(f, out, inlen, outlen);
}

struct depacker libxmp_depacker_arcfs = {
	test_arcfs,
	NULL,
	decrunch_arcfs
=======
/* Extended Module Player
 * Copyright (C) 2021-2024 Alice Rowan <petrifiedrowan@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * Simple single-file unpacker for ArcFS archives.
 * Report bugs to libxmp or to here: https://github.com/AliceLR/megazeuxtests
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arc_unpack.h"
#include "depacker.h"
#include "crc32.h"

/* Arbitrary maximum allowed output filesize. */
#define ARCFS_MAX_OUTPUT LIBXMP_DEPACK_LIMIT

/* #define ARCFS_DEBUG */

#define ARCFS_HEADER_SIZE 96
#define ARCFS_ENTRY_SIZE 36

#define ARCFS_END_OF_DIR 0
#define ARCFS_DELETED 1

#ifdef ARCFS_DEBUG
#define debug(...) do{ fprintf(stderr, "" __VA_ARGS__); fflush(stderr); }while(0)
#endif

static arc_uint16 arc_crc16(arc_uint8 *buf, size_t len)
{
  return libxmp_crc16_IBM(buf, len, 0);
}

static arc_uint16 arc_mem_u16(arc_uint8 *buf)
{
  return (buf[1] << 8) | buf[0];
}

static arc_uint32 arc_mem_u32(arc_uint8 *buf)
{
  return (buf[3] << 24UL) | (buf[2] << 16UL) | (buf[1] << 8UL) | buf[0];
}

struct arcfs_data
{
  /*  0    char magic[8]; */
  /*  8 */ arc_uint32 entries_length;
  /* 12 */ arc_uint32 data_offset;
  /* 16 */ arc_uint32 min_read_version;
  /* 20 */ arc_uint32 min_write_version;
  /* 24 */ arc_uint32 format_version;
  /* 28    Filler. */
  /* 96 */
};

struct arcfs_entry
{
  /* Unhandled fields:
   * - Permissions are stored in the low byte of attributes.
   * - A 40-bit timestamp is usually stored in load_offset/exec_offset.
   *   The timestamp counts 10ms increments from epoch 1900-01-01.
   *   This is supposed to be stored when the top 12 bits of load_offset are 0xFFF.
   * - Likewise, when the top 12 bits of load_offset are 0xFF, bits 8 through 19
   *   in load_offset are supposed to be the RISC OS filetype.
   */

  /*  0 */ arc_uint8 method;
  /*  1 */ char filename[12];
  /* 12 */ arc_uint32 uncompressed_size;
  /* 16    arc_uint32 load_offset; */ /* Low byte -> high byte of the 40-bit timestamp. */
  /* 20    arc_uint32 exec_offset; */ /* Low portion of the 40-bit timestamp. */
  /* 24    arc_uint32 attributes; */
  /* 28 */ arc_uint32 compressed_size;
  /* 32    arc_uint32 info; */
  /* 36 */

  /* Unpacked fields */
  arc_uint16 crc16;
  arc_uint8  compression_bits;
  arc_uint8  is_directory;
  arc_uint32 value_offset;
};

static int arcfs_check_magic(const unsigned char *buf)
{
  return memcmp(buf, "Archive\x00", 8) ? -1 : 0;
}

static int arcfs_read_header(struct arcfs_data *data, HIO_HANDLE *f)
{
  arc_uint8 buffer[ARCFS_HEADER_SIZE];

  if(hio_read(buffer, 1, ARCFS_HEADER_SIZE, f) < ARCFS_HEADER_SIZE)
  {
    #ifdef ARCFS_DEBUG
    debug("short read in header\n");
    #endif
    return -1;
  }

  if(arcfs_check_magic(buffer) < 0)
  {
    #ifdef ARCFS_DEBUG
    debug("bad header magic: %8.8s\n", (char *)buffer);
    #endif
    return -1;
  }

  data->entries_length    = arc_mem_u32(buffer + 8);
  data->data_offset       = arc_mem_u32(buffer + 12);
  data->min_read_version  = arc_mem_u32(buffer + 16);
  data->min_write_version = arc_mem_u32(buffer + 20);
  data->format_version    = arc_mem_u32(buffer + 24);

  if(data->entries_length % ARCFS_ENTRY_SIZE != 0)
  {
    #ifdef ARCFS_DEBUG
    debug("bad entries length: %zu\n", (size_t)data->entries_length);
    #endif
    return -1;
  }

  if(data->data_offset < ARCFS_HEADER_SIZE ||
     data->data_offset - ARCFS_HEADER_SIZE < data->entries_length)
  {
    #ifdef ARCFS_DEBUG
    debug("bad data offset: %zu\n", (size_t)data->data_offset);
    #endif
    return -1;
  }

  /* These seem to be the highest versions that exist. */
  if(data->min_read_version > 260 || data->min_write_version > 260 || data->format_version > 0x0a)
  {
    #ifdef ARCFS_DEBUG
    debug("bad versions: %zu %zu %zu\n", (size_t)data->min_read_version,
     (size_t)data->min_write_version, (size_t)data->format_version);
    #endif
    return -1;
  }

  return 0;
}

static int arcfs_read_entry(struct arcfs_entry *e, HIO_HANDLE *f)
{
  arc_uint8 buffer[ARCFS_ENTRY_SIZE];

  if(hio_read(buffer, 1, ARCFS_ENTRY_SIZE, f) < ARCFS_ENTRY_SIZE)
    return -1;

  e->method = buffer[0] & 0x7f;
  if(e->method == ARCFS_END_OF_DIR)
    return 0;

  memcpy(e->filename, buffer + 1, 11);
  e->filename[11] = '\0';

  e->uncompressed_size = arc_mem_u32(buffer + 12);
  e->compression_bits  = buffer[25]; /* attributes */
  e->crc16             = arc_mem_u16(buffer + 26); /* attributes */
  e->compressed_size   = arc_mem_u32(buffer + 28);
  e->value_offset      = arc_mem_u32(buffer + 32) & 0x7fffffffUL; /* info */
  e->is_directory      = buffer[35] >> 7; /* info */

  return 0;
}

static int arcfs_read(unsigned char **dest, size_t *dest_len, HIO_HANDLE *f, unsigned long file_len)
{
  struct arcfs_data data;
  struct arcfs_entry e;
  unsigned char *in;
  unsigned char *out;
  const char *err;
  size_t out_len;
  size_t offset;
  size_t i;

  if(arcfs_read_header(&data, f) < 0)
    return -1;

  if(data.data_offset > file_len)
    return -1;

  for(i = 0; i < data.entries_length; i += ARCFS_ENTRY_SIZE)
  {
    if(arcfs_read_entry(&e, f) < 0)
    {
      #ifdef ARCFS_DEBUG
      debug("error reading entry %zu\n", (size_t)data.entries_length / ARCFS_ENTRY_SIZE);
      #endif
      return -1;
    }

    #ifdef ARCFS_DEBUG
    debug("checking file: %s\n", e.filename);
    #endif

    /* Ignore directories, end of directory markers, deleted files. */
    if(e.method == ARCFS_END_OF_DIR || e.method == ARCFS_DELETED || e.is_directory)
      continue;

    if(e.method == ARC_M_UNPACKED)
      e.compressed_size = e.uncompressed_size;

    /* Ignore junk offset/size. */
    if(e.value_offset >= file_len - data.data_offset)
      continue;

    offset = data.data_offset + e.value_offset;
    if(e.compressed_size > file_len - offset)
      continue;

    /* Ignore sizes over the allowed limit. */
    if(e.uncompressed_size > ARCFS_MAX_OUTPUT)
      continue;

    /* Ignore unsupported methods. */
    if(arc_method_is_supported(e.method) < 0)
      continue;

    if(libxmp_exclude_match(e.filename))
      continue;

    /* Read file. */
    #ifdef ARCFS_DEBUG
    debug("unpacking file: %s\n", e.filename);
    #endif

    if(hio_seek(f, offset, SEEK_SET) < 0)
      return -1;

    in = (unsigned char *)malloc(e.compressed_size);
    if(!in)
      return -1;

    if(hio_read(in, 1, e.compressed_size, f) < e.compressed_size)
    {
      free(in);
      return -1;
    }

    if(e.method != ARC_M_UNPACKED)
    {
      out = (unsigned char *)malloc(e.uncompressed_size);
      out_len = e.uncompressed_size;
      if(!out)
      {
        free(in);
        return -1;
      }

      err = arc_unpack(out, out_len, in, e.compressed_size, e.method, e.compression_bits);
      if(err != NULL)
      {
        #ifdef ARCFS_DEBUG
        debug("error unpacking: %s\n", err);
        #endif
        free(in);
        free(out);
        return -1;
      }
      free(in);
    }
    else
    {
      out = in;
      out_len = e.uncompressed_size;
    }

    /* ArcFS CRC may sometimes just be 0, in which case, ignore it. */
    if(e.crc16)
    {
      arc_uint16 out_crc16 = arc_crc16(out, out_len);
      if(e.crc16 != out_crc16)
      {
        #ifdef ARCFS_DEBUG
        debug("crc16 mismatch: expected %u, got %u\n", e.crc16, out_crc16);
        #endif
        free(out);
        return -1;
      }
    }
    *dest = out;
    *dest_len = out_len;
    return 0;
  }
  return -1;
}


static int arcfs_test(unsigned char *data)
{
  return arcfs_check_magic(data) == 0;
}

static int arcfs_decrunch(HIO_HANDLE *in, void **out, long *outlen)
{
  unsigned char *outbuf;
  size_t size;

  int ret = arcfs_read(&outbuf, &size, in, hio_size(in));
  if(ret < 0)
    return -1;

  *out = outbuf;
  *outlen = size;
  return 0;
}

const struct depacker libxmp_depacker_arcfs =
{
  arcfs_test,
  NULL,
  arcfs_decrunch
>>>>>>> db7344ebf (abc)
};
