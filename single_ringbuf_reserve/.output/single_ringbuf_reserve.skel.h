/* SPDX-License-Identifier: (LGPL-2.1 OR BSD-2-Clause) */

/* THIS FILE IS AUTOGENERATED! */
#ifndef __SINGLE_RINGBUF_RESERVE_BPF_SKEL_H__
#define __SINGLE_RINGBUF_RESERVE_BPF_SKEL_H__

#include <stdlib.h>
#include <bpf/libbpf.h>

struct single_ringbuf_reserve_bpf {
	struct bpf_object_skeleton *skeleton;
	struct bpf_object *obj;
	struct {
		struct bpf_map *rb;
		struct bpf_map *bss;
	} maps;
	struct {
		struct bpf_program *catch_fentry_one;
	} progs;
	struct {
		struct bpf_link *catch_fentry_one;
	} links;
	struct single_ringbuf_reserve_bpf__bss {
		int capture_enabled;
		char __pad0[4];
		long n_drops;
	} *bss;
};

static void
single_ringbuf_reserve_bpf__destroy(struct single_ringbuf_reserve_bpf *obj)
{
	if (!obj)
		return;
	if (obj->skeleton)
		bpf_object__destroy_skeleton(obj->skeleton);
	free(obj);
}

static inline int
single_ringbuf_reserve_bpf__create_skeleton(struct single_ringbuf_reserve_bpf *obj);

static inline struct single_ringbuf_reserve_bpf *
single_ringbuf_reserve_bpf__open_opts(const struct bpf_object_open_opts *opts)
{
	struct single_ringbuf_reserve_bpf *obj;

	obj = (struct single_ringbuf_reserve_bpf *)calloc(1, sizeof(*obj));
	if (!obj)
		return NULL;
	if (single_ringbuf_reserve_bpf__create_skeleton(obj))
		goto err;
	if (bpf_object__open_skeleton(obj->skeleton, opts))
		goto err;

	return obj;
err:
	single_ringbuf_reserve_bpf__destroy(obj);
	return NULL;
}

static inline struct single_ringbuf_reserve_bpf *
single_ringbuf_reserve_bpf__open(void)
{
	return single_ringbuf_reserve_bpf__open_opts(NULL);
}

static inline int
single_ringbuf_reserve_bpf__load(struct single_ringbuf_reserve_bpf *obj)
{
	return bpf_object__load_skeleton(obj->skeleton);
}

static inline struct single_ringbuf_reserve_bpf *
single_ringbuf_reserve_bpf__open_and_load(void)
{
	struct single_ringbuf_reserve_bpf *obj;

	obj = single_ringbuf_reserve_bpf__open();
	if (!obj)
		return NULL;
	if (single_ringbuf_reserve_bpf__load(obj)) {
		single_ringbuf_reserve_bpf__destroy(obj);
		return NULL;
	}
	return obj;
}

static inline int
single_ringbuf_reserve_bpf__attach(struct single_ringbuf_reserve_bpf *obj)
{
	return bpf_object__attach_skeleton(obj->skeleton);
}

static inline void
single_ringbuf_reserve_bpf__detach(struct single_ringbuf_reserve_bpf *obj)
{
	return bpf_object__detach_skeleton(obj->skeleton);
}

static inline int
single_ringbuf_reserve_bpf__create_skeleton(struct single_ringbuf_reserve_bpf *obj)
{
	struct bpf_object_skeleton *s;

	s = (struct bpf_object_skeleton *)calloc(1, sizeof(*s));
	if (!s)
		return -1;
	obj->skeleton = s;

	s->sz = sizeof(*s);
	s->name = "single_ringbuf_reserve_bpf";
	s->obj = &obj->obj;

	/* maps */
	s->map_cnt = 2;
	s->map_skel_sz = sizeof(*s->maps);
	s->maps = (struct bpf_map_skeleton *)calloc(s->map_cnt, s->map_skel_sz);
	if (!s->maps)
		goto err;

	s->maps[0].name = "rb";
	s->maps[0].map = &obj->maps.rb;

	s->maps[1].name = "single_r.bss";
	s->maps[1].map = &obj->maps.bss;
	s->maps[1].mmaped = (void **)&obj->bss;

	/* programs */
	s->prog_cnt = 1;
	s->prog_skel_sz = sizeof(*s->progs);
	s->progs = (struct bpf_prog_skeleton *)calloc(s->prog_cnt, s->prog_skel_sz);
	if (!s->progs)
		goto err;

	s->progs[0].name = "catch_fentry_one";
	s->progs[0].prog = &obj->progs.catch_fentry_one;
	s->progs[0].link = &obj->links.catch_fentry_one;

	s->data_sz = 4304;
	s->data = (void *)"\
\x7f\x45\x4c\x46\x02\x01\x01\0\0\0\0\0\0\0\0\0\x01\0\xf7\0\x01\0\0\0\0\0\0\0\0\
\0\0\0\0\0\0\0\0\0\0\0\x50\x0d\0\0\0\0\0\0\0\0\0\0\x40\0\0\0\0\0\x40\0\x0e\0\
\x01\0\x18\x01\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x61\x11\0\0\0\0\0\0\x15\x01\x5a\0\0\
\0\0\0\x85\0\0\0\x07\0\0\0\x67\0\0\0\x20\0\0\0\x77\0\0\0\x20\0\0\0\xbf\x01\0\0\
\0\0\0\0\x37\x01\0\0\x13\0\0\0\x27\x01\0\0\x13\0\0\0\x1f\x10\0\0\0\0\0\0\x65\0\
\x08\0\x08\0\0\0\x65\0\x0e\0\x03\0\0\0\x65\0\x2a\0\x01\0\0\0\xb7\x02\0\0\0\x08\
\0\0\x15\0\x40\0\0\0\0\0\x15\0\x01\0\x01\0\0\0\x05\0\x24\0\0\0\0\0\xb7\x02\0\0\
\0\x0c\0\0\x05\0\x3c\0\0\0\0\0\x65\0\x0c\0\x0d\0\0\0\x65\0\x11\0\x0a\0\0\0\x15\
\0\x26\0\x09\0\0\0\x15\0\x01\0\x0a\0\0\0\x05\0\x1d\0\0\0\0\0\xb7\x02\0\0\0\x30\
\0\0\x05\0\x35\0\0\0\0\0\x65\0\x11\0\x05\0\0\0\x15\0\x22\0\x04\0\0\0\x15\0\x01\
\0\x05\0\0\0\x05\0\x17\0\0\0\0\0\xb7\x02\0\0\0\x1c\0\0\x05\0\x2f\0\0\0\0\0\x65\
\0\x11\0\x0f\0\0\0\x15\0\x1e\0\x0e\0\0\0\x15\0\x01\0\x0f\0\0\0\x05\0\x11\0\0\0\
\0\0\xb7\x02\0\0\0\x44\0\0\x05\0\x29\0\0\0\0\0\x15\0\x1b\0\x0b\0\0\0\x15\0\x1c\
\0\x0c\0\0\0\x15\0\x01\0\x0d\0\0\0\x05\0\x0b\0\0\0\0\0\xb7\x02\0\0\0\x3c\0\0\
\x05\0\x23\0\0\0\0\0\x15\0\x19\0\x06\0\0\0\x15\0\x1a\0\x07\0\0\0\x15\0\x01\0\
\x08\0\0\0\x05\0\x05\0\0\0\0\0\xb7\x02\0\0\0\x28\0\0\x05\0\x1d\0\0\0\0\0\x15\0\
\x17\0\x10\0\0\0\x15\0\x18\0\x11\0\0\0\x15\0\0\0\x12\0\0\0\xb7\x02\0\0\0\x50\0\
\0\x05\0\x18\0\0\0\0\0\x15\0\x16\0\x02\0\0\0\x15\0\x01\0\x03\0\0\0\x05\0\xfb\
\xff\0\0\0\0\xb7\x02\0\0\0\x14\0\0\x05\0\x13\0\0\0\0\0\xb7\x02\0\0\0\x2c\0\0\
\x05\0\x11\0\0\0\0\0\xb7\x02\0\0\0\x18\0\0\x05\0\x0f\0\0\0\0\0\xb7\x02\0\0\0\
\x40\0\0\x05\0\x0d\0\0\0\0\0\xb7\x02\0\0\0\x34\0\0\x05\0\x0b\0\0\0\0\0\xb7\x02\
\0\0\0\x38\0\0\x05\0\x09\0\0\0\0\0\xb7\x02\0\0\0\x20\0\0\x05\0\x07\0\0\0\0\0\
\xb7\x02\0\0\0\x24\0\0\x05\0\x05\0\0\0\0\0\xb7\x02\0\0\0\x48\0\0\x05\0\x03\0\0\
\0\0\0\xb7\x02\0\0\0\x4c\0\0\x05\0\x01\0\0\0\0\0\xb7\x02\0\0\0\x10\0\0\x18\x01\
\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xb7\x03\0\0\0\0\0\0\x85\0\0\0\x83\0\0\0\x55\0\x06\
\0\0\0\0\0\x18\x01\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x79\x12\0\0\0\0\0\0\x07\x02\0\0\
\x01\0\0\0\x7b\x21\0\0\0\0\0\0\x05\0\x03\0\0\0\0\0\xbf\x01\0\0\0\0\0\0\xb7\x02\
\0\0\x01\0\0\0\x85\0\0\0\x84\0\0\0\xb7\0\0\0\0\0\0\0\x95\0\0\0\0\0\0\0\x44\x75\
\x61\x6c\x20\x42\x53\x44\x2f\x47\x50\x4c\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\
\0\x9f\xeb\x01\0\x18\0\0\0\0\0\0\0\x84\x01\0\0\x84\x01\0\0\x88\x01\0\0\0\0\0\0\
\0\0\0\x02\x03\0\0\0\x01\0\0\0\0\0\0\x01\x04\0\0\0\x20\0\0\x01\0\0\0\0\0\0\0\
\x03\0\0\0\0\x02\0\0\0\x04\0\0\0\x1b\0\0\0\x05\0\0\0\0\0\0\x01\x04\0\0\0\x20\0\
\0\0\0\0\0\0\0\0\0\x02\x06\0\0\0\0\0\0\0\0\0\0\x03\0\0\0\0\x02\0\0\0\x04\0\0\0\
\0\0\0\x04\0\0\0\0\x02\0\0\x04\x10\0\0\0\x19\0\0\0\x01\0\0\0\0\0\0\0\x1e\0\0\0\
\x05\0\0\0\x40\0\0\0\x2a\0\0\0\0\0\0\x0e\x07\0\0\0\x01\0\0\0\0\0\0\0\0\0\0\x02\
\0\0\0\0\0\0\0\0\x01\0\0\x0d\x02\0\0\0\x2d\0\0\0\x09\0\0\0\x31\0\0\0\x01\0\0\
\x0c\x0a\0\0\0\x4b\x01\0\0\0\0\0\x01\x01\0\0\0\x08\0\0\x01\0\0\0\0\0\0\0\x03\0\
\0\0\0\x0c\0\0\0\x04\0\0\0\x0d\0\0\0\x50\x01\0\0\0\0\0\x0e\x0d\0\0\0\x01\0\0\0\
\x58\x01\0\0\0\0\0\x0e\x02\0\0\0\x01\0\0\0\x68\x01\0\0\0\0\0\x01\x08\0\0\0\x40\
\0\0\x01\x6d\x01\0\0\0\0\0\x0e\x10\0\0\0\x01\0\0\0\x75\x01\0\0\x02\0\0\x0f\0\0\
\0\0\x0f\0\0\0\0\0\0\0\x04\0\0\0\x11\0\0\0\0\0\0\0\x08\0\0\0\x7a\x01\0\0\x01\0\
\0\x0f\0\0\0\0\x08\0\0\0\0\0\0\0\x10\0\0\0\x80\x01\0\0\x01\0\0\x0f\0\0\0\0\x0e\
\0\0\0\0\0\0\0\x0d\0\0\0\0\x69\x6e\x74\0\x5f\x5f\x41\x52\x52\x41\x59\x5f\x53\
\x49\x5a\x45\x5f\x54\x59\x50\x45\x5f\x5f\0\x74\x79\x70\x65\0\x6d\x61\x78\x5f\
\x65\x6e\x74\x72\x69\x65\x73\0\x72\x62\0\x63\x74\x78\0\x63\x61\x74\x63\x68\x5f\
\x66\x65\x6e\x74\x72\x79\x5f\x6f\x6e\x65\0\x66\x65\x6e\x74\x72\x79\x2f\x5f\x5f\
\x78\x36\x34\x5f\x73\x79\x73\x5f\x67\x65\x74\x70\x67\x69\x64\0\x2f\x68\x6f\x6d\
\x65\x2f\x61\x6e\x64\x72\x65\x61\x2f\x53\x63\x72\x69\x76\x61\x6e\x69\x61\x2f\
\x74\x6d\x70\x5f\x74\x65\x73\x74\x5f\x72\x69\x6e\x67\x62\x75\x66\x2f\x73\x69\
\x6e\x67\x6c\x65\x5f\x72\x69\x6e\x67\x62\x75\x66\x5f\x72\x65\x73\x65\x72\x76\
\x65\x2f\x73\x69\x6e\x67\x6c\x65\x5f\x72\x69\x6e\x67\x62\x75\x66\x5f\x72\x65\
\x73\x65\x72\x76\x65\x2e\x62\x70\x66\x2e\x63\0\x09\x69\x66\x28\x21\x63\x61\x70\
\x74\x75\x72\x65\x5f\x65\x6e\x61\x62\x6c\x65\x64\x29\0\x09\x75\x69\x6e\x74\x33\
\x32\x5f\x74\x20\x69\x20\x3d\x20\x62\x70\x66\x5f\x67\x65\x74\x5f\x70\x72\x61\
\x6e\x64\x6f\x6d\x5f\x75\x33\x32\x28\x29\x20\x25\x20\x31\x39\x3b\0\x09\x73\x77\
\x69\x74\x63\x68\x28\x69\x29\0\x09\x69\x66\x28\x21\x65\x76\x65\x6e\x74\x29\0\
\x09\x09\x6e\x5f\x64\x72\x6f\x70\x73\x2b\x2b\x3b\0\x09\x62\x70\x66\x5f\x72\x69\
\x6e\x67\x62\x75\x66\x5f\x73\x75\x62\x6d\x69\x74\x28\x65\x76\x65\x6e\x74\x2c\
\x20\x42\x50\x46\x5f\x52\x42\x5f\x4e\x4f\x5f\x57\x41\x4b\x45\x55\x50\x29\x3b\0\
\x7d\0\x63\x68\x61\x72\0\x4c\x49\x43\x45\x4e\x53\x45\0\x63\x61\x70\x74\x75\x72\
\x65\x5f\x65\x6e\x61\x62\x6c\x65\x64\0\x6c\x6f\x6e\x67\0\x6e\x5f\x64\x72\x6f\
\x70\x73\0\x2e\x62\x73\x73\0\x2e\x6d\x61\x70\x73\0\x6c\x69\x63\x65\x6e\x73\x65\
\0\x9f\xeb\x01\0\x20\0\0\0\0\0\0\0\x14\0\0\0\x14\0\0\0\xac\0\0\0\xc0\0\0\0\0\0\
\0\0\x08\0\0\0\x42\0\0\0\x01\0\0\0\0\0\0\0\x0b\0\0\0\x10\0\0\0\x42\0\0\0\x0a\0\
\0\0\0\0\0\0\x5b\0\0\0\xb7\0\0\0\x06\xc4\0\0\x18\0\0\0\x5b\0\0\0\xb7\0\0\0\x05\
\xc4\0\0\x20\0\0\0\x5b\0\0\0\xcd\0\0\0\x0f\xdc\0\0\x38\0\0\0\x5b\0\0\0\xcd\0\0\
\0\x25\xdc\0\0\x58\0\0\0\x5b\0\0\0\xf7\0\0\0\x02\xe4\0\0\x80\x02\0\0\x5b\0\0\0\
\0\0\0\0\0\0\0\0\xa0\x02\0\0\x5b\0\0\0\x02\x01\0\0\x05\xe4\x01\0\xa8\x02\0\0\
\x5b\0\0\0\x0e\x01\0\0\x0a\xec\x01\0\xd8\x02\0\0\x5b\0\0\0\x1b\x01\0\0\x02\xfc\
\x01\0\xf0\x02\0\0\x5b\0\0\0\x49\x01\0\0\x01\x08\x02\0\0\0\0\0\0\0\0\0\0\0\0\0\
\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x03\0\x03\0\0\0\0\0\0\0\0\0\0\0\0\0\0\
\0\0\0\xb6\0\0\0\0\0\x03\0\xf0\x02\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x25\x01\0\0\0\0\
\x03\0\xa0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xaf\0\0\0\0\0\x03\0\xd8\0\0\0\0\0\0\0\
\0\0\0\0\0\0\0\0\xee\0\0\0\0\0\x03\0\xc0\x01\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xfd\0\
\0\0\0\0\x03\0\x80\x02\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe6\0\0\0\0\0\x03\0\x90\0\0\
\0\0\0\0\0\0\0\0\0\0\0\0\0\x0d\x01\0\0\0\0\x03\0\xb0\x01\0\0\0\0\0\0\0\0\0\0\0\
\0\0\0\x5d\x01\0\0\0\0\x03\0\x08\x01\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xce\0\0\0\0\0\
\x03\0\x38\x01\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x05\x01\0\0\0\0\x03\0\xe8\x01\0\0\0\
\0\0\0\0\0\0\0\0\0\0\0\xf5\0\0\0\0\0\x03\0\xc8\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\
\x65\x01\0\0\0\0\x03\0\x68\x01\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x75\x01\0\0\0\0\x03\
\0\xf8\x01\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x55\x01\0\0\0\0\x03\0\xf8\0\0\0\0\0\0\0\
\0\0\0\0\0\0\0\0\x1d\x01\0\0\0\0\x03\0\x98\x01\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x6d\
\x01\0\0\0\0\x03\0\x08\x02\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x4d\x01\0\0\0\0\x03\0\
\x28\x01\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xde\0\0\0\0\0\x03\0\x18\x02\0\0\0\0\0\0\0\
\0\0\0\0\0\0\0\xbe\0\0\0\0\0\x03\0\x28\x02\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x9f\0\0\
\0\0\0\x03\0\x58\x01\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x45\x01\0\0\0\0\x03\0\x38\x02\
\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x35\x01\0\0\0\0\x03\0\x48\x02\0\0\0\0\0\0\0\0\0\0\
\0\0\0\0\x15\x01\0\0\0\0\x03\0\x88\x01\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x3d\x01\0\0\
\0\0\x03\0\x58\x02\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x2d\x01\0\0\0\0\x03\0\x68\x02\0\
\0\0\0\0\0\0\0\0\0\0\0\0\0\xc6\0\0\0\0\0\x03\0\x78\x02\0\0\0\0\0\0\0\0\0\0\0\0\
\0\0\xa7\0\0\0\0\0\x03\0\xd8\x01\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xd6\0\0\0\0\0\x03\
\0\xd8\x02\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x3d\0\0\0\x12\0\x03\0\0\0\0\0\0\0\0\0\0\
\x03\0\0\0\0\0\0\x6b\0\0\0\x11\0\x06\0\0\0\0\0\0\0\0\0\x04\0\0\0\0\0\0\0\x7b\0\
\0\0\x11\0\x07\0\0\0\0\0\0\0\0\0\x10\0\0\0\0\0\0\0\x19\0\0\0\x11\0\x06\0\x08\0\
\0\0\0\0\0\0\x08\0\0\0\0\0\0\0\x97\0\0\0\x11\0\x05\0\0\0\0\0\0\0\0\0\x0d\0\0\0\
\0\0\0\0\0\0\0\0\0\0\0\0\x01\0\0\0\x1f\0\0\0\x80\x02\0\0\0\0\0\0\x01\0\0\0\x20\
\0\0\0\xa8\x02\0\0\0\0\0\0\x01\0\0\0\x21\0\0\0\x58\x01\0\0\0\0\0\0\x04\0\0\0\
\x1f\0\0\0\x64\x01\0\0\0\0\0\0\x04\0\0\0\x21\0\0\0\x7c\x01\0\0\0\0\0\0\x04\0\0\
\0\x20\0\0\0\x94\x01\0\0\0\0\0\0\x04\0\0\0\x22\0\0\0\x2c\0\0\0\0\0\0\0\x04\0\0\
\0\x01\0\0\0\x40\0\0\0\0\0\0\0\x04\0\0\0\x01\0\0\0\x50\0\0\0\0\0\0\0\x04\0\0\0\
\x01\0\0\0\x60\0\0\0\0\0\0\0\x04\0\0\0\x01\0\0\0\x70\0\0\0\0\0\0\0\x04\0\0\0\
\x01\0\0\0\x80\0\0\0\0\0\0\0\x04\0\0\0\x01\0\0\0\x90\0\0\0\0\0\0\0\x04\0\0\0\
\x01\0\0\0\xa0\0\0\0\0\0\0\0\x04\0\0\0\x01\0\0\0\xb0\0\0\0\0\0\0\0\x04\0\0\0\
\x01\0\0\0\xc0\0\0\0\0\0\0\0\x04\0\0\0\x01\0\0\0\xd0\0\0\0\0\0\0\0\x04\0\0\0\
\x01\0\0\0\x27\x2b\x29\0\x2e\x74\x65\x78\x74\0\x2e\x72\x65\x6c\x2e\x42\x54\x46\
\x2e\x65\x78\x74\0\x2e\x62\x73\x73\0\x6e\x5f\x64\x72\x6f\x70\x73\0\x2e\x6d\x61\
\x70\x73\0\x2e\x6c\x6c\x76\x6d\x5f\x61\x64\x64\x72\x73\x69\x67\0\x6c\x69\x63\
\x65\x6e\x73\x65\0\x63\x61\x74\x63\x68\x5f\x66\x65\x6e\x74\x72\x79\x5f\x6f\x6e\
\x65\0\x2e\x72\x65\x6c\x66\x65\x6e\x74\x72\x79\x2f\x5f\x5f\x78\x36\x34\x5f\x73\
\x79\x73\x5f\x67\x65\x74\x70\x67\x69\x64\0\x63\x61\x70\x74\x75\x72\x65\x5f\x65\
\x6e\x61\x62\x6c\x65\x64\0\x72\x62\0\x2e\x73\x74\x72\x74\x61\x62\0\x2e\x73\x79\
\x6d\x74\x61\x62\0\x2e\x72\x65\x6c\x2e\x42\x54\x46\0\x4c\x49\x43\x45\x4e\x53\
\x45\0\x4c\x42\x42\x30\x5f\x33\x39\0\x4c\x42\x42\x30\x5f\x32\x39\0\x4c\x42\x42\
\x30\x5f\x38\0\x4c\x42\x42\x30\x5f\x34\x38\0\x4c\x42\x42\x30\x5f\x33\x38\0\x4c\
\x42\x42\x30\x5f\x32\x38\0\x4c\x42\x42\x30\x5f\x31\x38\0\x4c\x42\x42\x30\x5f\
\x34\x37\0\x4c\x42\x42\x30\x5f\x33\x37\0\x4c\x42\x42\x30\x5f\x32\x37\0\x4c\x42\
\x42\x30\x5f\x36\0\x4c\x42\x42\x30\x5f\x33\x36\0\x4c\x42\x42\x30\x5f\x34\x35\0\
\x4c\x42\x42\x30\x5f\x33\x35\0\x4c\x42\x42\x30\x5f\x34\x34\0\x4c\x42\x42\x30\
\x5f\x33\x34\0\x4c\x42\x42\x30\x5f\x32\x34\0\x4c\x42\x42\x30\x5f\x31\x34\0\x4c\
\x42\x42\x30\x5f\x34\x33\0\x4c\x42\x42\x30\x5f\x33\x33\0\x4c\x42\x42\x30\x5f\
\x34\x32\0\x4c\x42\x42\x30\x5f\x33\x32\0\x4c\x42\x42\x30\x5f\x34\x31\0\x4c\x42\
\x42\x30\x5f\x33\x31\0\x4c\x42\x42\x30\x5f\x32\x31\0\x4c\x42\x42\x30\x5f\x31\
\x31\0\x4c\x42\x42\x30\x5f\x34\x30\0\x4c\x42\x42\x30\x5f\x33\x30\0\0\0\0\0\0\0\
\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\
\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x7e\0\0\0\x03\0\0\0\0\0\0\0\0\0\0\0\0\0\
\0\0\0\0\0\0\xd3\x0b\0\0\0\0\0\0\x7d\x01\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x01\0\0\0\
\0\0\0\0\0\0\0\0\0\0\0\0\x01\0\0\0\x01\0\0\0\x06\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\
\x40\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x04\0\0\0\0\0\0\0\0\0\0\0\0\
\0\0\0\x52\0\0\0\x01\0\0\0\x06\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x40\0\0\0\0\0\0\0\
\0\x03\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x08\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x4e\0\0\0\
\x09\0\0\0\x40\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xb0\x0a\0\0\0\0\0\0\x30\0\0\0\0\0\
\0\0\x0d\0\0\0\x03\0\0\0\x08\0\0\0\0\0\0\0\x10\0\0\0\0\0\0\0\x35\0\0\0\x01\0\0\
\0\x03\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x40\x03\0\0\0\0\0\0\x0d\0\0\0\0\0\0\0\0\0\
\0\0\0\0\0\0\x01\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x14\0\0\0\x08\0\0\0\x03\0\0\0\0\
\0\0\0\0\0\0\0\0\0\0\0\x50\x03\0\0\0\0\0\0\x10\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\
\x08\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x21\0\0\0\x01\0\0\0\x03\0\0\0\0\0\0\0\0\0\0\
\0\0\0\0\0\x50\x03\0\0\0\0\0\0\x10\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x08\0\0\0\0\0\
\0\0\0\0\0\0\0\0\0\0\x92\0\0\0\x01\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x60\
\x03\0\0\0\0\0\0\x24\x03\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x04\0\0\0\0\0\0\0\0\0\0\0\
\0\0\0\0\x8e\0\0\0\x09\0\0\0\x40\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\xe0\x0a\0\0\0\0\
\0\0\x40\0\0\0\0\0\0\0\x0d\0\0\0\x08\0\0\0\x08\0\0\0\0\0\0\0\x10\0\0\0\0\0\0\0\
\x0b\0\0\0\x01\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x84\x06\0\0\0\0\0\0\xe0\0\
\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x04\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x07\0\0\0\x09\0\
\0\0\x40\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x20\x0b\0\0\0\0\0\0\xb0\0\0\0\0\0\0\0\
\x0d\0\0\0\x0a\0\0\0\x08\0\0\0\0\0\0\0\x10\0\0\0\0\0\0\0\x27\0\0\0\x03\x4c\xff\
\x6f\0\0\0\x80\0\0\0\0\0\0\0\0\0\0\0\0\xd0\x0b\0\0\0\0\0\0\x03\0\0\0\0\0\0\0\0\
\0\0\0\0\0\0\0\x01\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x86\0\0\0\x02\0\0\0\0\0\0\0\0\
\0\0\0\0\0\0\0\0\0\0\0\x68\x07\0\0\0\0\0\0\x48\x03\0\0\0\0\0\0\x01\0\0\0\x1e\0\
\0\0\x08\0\0\0\0\0\0\0\x18\0\0\0\0\0\0\0";

	return 0;
err:
	bpf_object__destroy_skeleton(s);
	return -1;
}

#endif /* __SINGLE_RINGBUF_RESERVE_BPF_SKEL_H__ */