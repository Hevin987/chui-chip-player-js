/* Extended Module Player
<<<<<<< HEAD
 * Copyright (C) 1996-2021 Claudio Matsuoka and Hipolito Carraro Jr
=======
 * Copyright (C) 1996-2025 Claudio Matsuoka and Hipolito Carraro Jr
>>>>>>> db7344ebf (abc)
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

#include "../common.h"
<<<<<<< HEAD
#include "../list.h"
=======
>>>>>>> db7344ebf (abc)
#include "iff.h"

#include "loader.h"

<<<<<<< HEAD
struct iff_data {
	struct list_head iff_list;
=======
struct iff_info {
	char id[4];
	iff_loader loader;
	struct iff_info *next;
};

struct iff_data {
	struct iff_info *head;
	struct iff_info *tail;
>>>>>>> db7344ebf (abc)
	unsigned id_size;
	unsigned flags;
};

<<<<<<< HEAD
static int iff_process(iff_handle opaque, struct module_data *m, char *id, long size,
		HIO_HANDLE *f, void *parm)
{
	struct iff_data *data = (struct iff_data *)opaque;
	struct list_head *tmp;
	struct iff_info *i;
	int pos;

	pos = hio_tell(f);

	list_for_each(tmp, &data->iff_list) {
		i = list_entry(tmp, struct iff_info, list);
		if (id && !memcmp(id, i->id, data->id_size)) {
			D_(D_WARN "Load IFF chunk %s (%ld) @%d", id, size, pos);
=======
static void iff_append(struct iff_data *data, struct iff_info *i)
{
	if (data->head && data->tail) {
		data->tail->next = i;
		data->tail = i;
	} else {
		data->head = i;
		data->tail = i;
	}
}

static int iff_process(iff_handle opaque, struct module_data *m, char *id, uint32 size,
		HIO_HANDLE *f, void *parm)
{
	struct iff_data *data = (struct iff_data *)opaque;
	struct iff_info *i;
	long pos;

	pos = hio_tell(f);
	D_(D_INFO "chunk offset: %ld", pos);

	for (i = data->head; i; i = i->next) {
		if (id && !memcmp(id, i->id, data->id_size)) {
			D_(D_WARN "Load IFF chunk %s (%u) @%ld", id, (unsigned)size, pos);
>>>>>>> db7344ebf (abc)
			if (size > IFF_MAX_CHUNK_SIZE) {
				return -1;
			}
			if (i->loader(m, size, f, parm) < 0) {
				return -1;
			}
			break;
		}
	}

<<<<<<< HEAD
	if (hio_seek(f, pos + size, SEEK_SET) < 0) {
		return -1;
=======
#if LONG_MAX <= 2147483647L
	/* TODO: hio_seek doesn't support 64-bit values. */
	if (pos < 0 || pos + (int64)size > LONG_MAX) {
		return 1;
	}
#endif
	if (hio_seek(f, pos + size, SEEK_SET) < 0) {
		/* IFF container issue--exit without error. */
		return 1;
>>>>>>> db7344ebf (abc)
	}

	return 0;
}

static int iff_chunk(iff_handle opaque, struct module_data *m, HIO_HANDLE *f, void *parm)
{
	struct iff_data *data = (struct iff_data *)opaque;
<<<<<<< HEAD
	unsigned size;
=======
	uint32 size;
>>>>>>> db7344ebf (abc)
	char id[17] = "";

	D_(D_INFO "chunk id size: %d", data->id_size);
	if (hio_read(id, 1, data->id_size, f) != data->id_size) {
<<<<<<< HEAD
		(void)hio_error(f);	/* clear error flag */
=======
		/* End of file or IFF container issue--exit without error. */
>>>>>>> db7344ebf (abc)
		return 1;
	}
	D_(D_INFO "chunk id: [%s]", id);

	if (data->flags & IFF_SKIP_EMBEDDED) {
		/* embedded RIFF hack */
		if (!strncmp(id, "RIFF", 4)) {
			hio_read32b(f);
			hio_read32b(f);
			/* read first chunk ID instead */
			if (hio_read(id, 1, data->id_size, f) != data->id_size){
				return 1;
			}
		}
	}

	if (data->flags & IFF_LITTLE_ENDIAN) {
		size = hio_read32l(f);
	} else {
		size = hio_read32b(f);
	}
<<<<<<< HEAD
	D_(D_INFO "size: %d", size);

	if (hio_error(f)) {
		return -1;
=======
	D_(D_INFO "chunk size: %u", (unsigned)size);

	if (hio_error(f)) {
		/* IFF container issue--exit without error. */
		return 1;
>>>>>>> db7344ebf (abc)
	}

	if (data->flags & IFF_CHUNK_ALIGN2) {
		/* Sanity check */
		if (size > 0xfffffffe) {
<<<<<<< HEAD
			return -1;
=======
			/* IFF container issue--exit without error. */
			return 1;
>>>>>>> db7344ebf (abc)
		}
		size = (size + 1) & ~1;
	}

	if (data->flags & IFF_CHUNK_ALIGN4) {
		/* Sanity check */
		if (size > 0xfffffffc) {
<<<<<<< HEAD
			return -1;
=======
			/* IFF container issue--exit without error. */
			return 1;
>>>>>>> db7344ebf (abc)
		}
		size = (size + 3) & ~3;
	}

	/* PT 3.6 hack: this does not seem to ever apply to "PTDT".
	 * This broke several modules (city lights.pt36, acid phase.pt36) */
	if ((data->flags & IFF_FULL_CHUNK_SIZE) && memcmp(id, "PTDT", 4)) {
		if (size < data->id_size + 4)
			return -1;
		size -= data->id_size + 4;
	}

	return iff_process(opaque, m, id, size, f, parm);
}

<<<<<<< HEAD
iff_handle libxmp_iff_new()
=======
iff_handle libxmp_iff_new(void)
>>>>>>> db7344ebf (abc)
{
	struct iff_data *data;

	data = (struct iff_data *) malloc(sizeof(struct iff_data));
	if (data == NULL) {
		return NULL;
	}

<<<<<<< HEAD
	INIT_LIST_HEAD(&data->iff_list);
=======
	data->head = data->tail = NULL;
>>>>>>> db7344ebf (abc)
	data->id_size = 4;
	data->flags = 0;

	return (iff_handle)data;
}

int libxmp_iff_load(iff_handle opaque, struct module_data *m, HIO_HANDLE *f, void *parm)
{
	int ret;

	while (!hio_eof(f)) {
		ret = iff_chunk(opaque, m, f, parm);
		if (ret > 0)
			break;
		if (ret < 0)
			return -1;
	}

<<<<<<< HEAD
	return 0;
}

int libxmp_iff_register(iff_handle opaque, const char *id,
	int (*loader)(struct module_data *, int, HIO_HANDLE *, void *))
=======
	/* Reached end of file, or there was an IFF structural issue.
	 * Either way, clear the error flag to allow loading to continue. */
	(void)hio_error(f);
	return 0;
}

int libxmp_iff_register(iff_handle opaque, const char *id, iff_loader loader)
>>>>>>> db7344ebf (abc)
{
	struct iff_data *data = (struct iff_data *)opaque;
	struct iff_info *f;
	int i = 0;

	f = (struct iff_info *) malloc(sizeof(struct iff_info));
	if (f == NULL)
		return -1;

	/* Note: previously was an strncpy */
	for (; i < 4 && id && id[i]; i++)
		f->id[i] = id[i];
	for (; i < 4; i++)
		f->id[i] = '\0';

	f->loader = loader;
<<<<<<< HEAD

	list_add_tail(&f->list, &data->iff_list);

=======
	f->next = NULL;

	iff_append(data, f);
>>>>>>> db7344ebf (abc)
	return 0;
}

void libxmp_iff_release(iff_handle opaque)
{
	struct iff_data *data = (struct iff_data *)opaque;
<<<<<<< HEAD
	struct list_head *tmp;
	struct iff_info *i;

	/* can't use list_for_each, we free the node before incrementing */
	for (tmp = (&data->iff_list)->next; tmp != (&data->iff_list);) {
		i = list_entry(tmp, struct iff_info, list);
		list_del(&i->list);
		tmp = tmp->next;
		free(i);
=======
	struct iff_info *i;

	for (i = data->head; i; ) {
		struct iff_info *tmp = i->next;
		free(i);
		i = tmp;
>>>>>>> db7344ebf (abc)
	}

	free(data);
}

/* Functions to tune IFF mutations */

void libxmp_iff_id_size(iff_handle opaque, int n)
{
	struct iff_data *data = (struct iff_data *)opaque;

	data->id_size = n;
}

void libxmp_iff_set_quirk(iff_handle opaque, int i)
{
	struct iff_data *data = (struct iff_data *)opaque;

	data->flags |= i;
}
