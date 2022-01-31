#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>
#include <errno.h>
#include <signal.h>
#include "multi_ringbuf_output.skel.h"
#include <stdint.h>

#define RING_BUF_DIM 8 * 1024 * 1024

/* Global variables needed to manage multiple ringbuffer. */
static unsigned long *cons_pos = NULL;
static unsigned long *prod_pos = NULL;
static struct ring_buffer *ringbuf = NULL;
static struct multi_ringbuf_output_bpf *skel = NULL;
static uint64_t n_elaborated = 0;
static int n_cpus = 0;

typedef int (*ring_buffer_sample_fn)(void *ctx, void *data, size_t size);

struct ring
{
	ring_buffer_sample_fn sample_cb;
	void *ctx;
	void *data;
	unsigned long *consumer_pos;
	unsigned long *producer_pos;
	unsigned long mask;
	int map_fd;
};

struct ring_buffer
{
	struct epoll_event *events;
	struct ring *rings;
	size_t page_size;
	int epoll_fd;
	int ring_cnt;
};

static inline int roundup_len(uint32_t len)
{
	/* clear out top 2 bits (discard and busy, if set). */
	len <<= 2;
	len >>= 2;
	/* add length prefix. */
	len += BPF_RINGBUF_HDR_SZ;
	/* round up to 8 byte alignment. */
	return (len + 7) / 8 * 8;
}

static int handle_event(void *ctx, void *data, size_t data_sz)
{
	return 0;
}

static int libbpf_print_fn(enum libbpf_print_level level, const char *format, va_list args)
{
	/* to enable libbpf logging use this code:
	 * `return vfprintf(stderr, format, args);`
	 */
	return 0;
}

static void bump_memlock_rlimit(void)
{
	struct rlimit rlim_new = {
		.rlim_cur = RLIM_INFINITY,
		.rlim_max = RLIM_INFINITY,
	};

	if(setrlimit(RLIMIT_MEMLOCK, &rlim_new))
	{
		fprintf(stderr, "userspace-loader: failed to increase RLIMIT_MEMLOCK limit!\n");
		exit(1);
	}
}

static bool allocate_consumer_producer(int ncpus)
{
	cons_pos = (unsigned long *)calloc(ncpus, sizeof(unsigned long));
	prod_pos = (unsigned long *)calloc(ncpus, sizeof(unsigned long));
	if(cons_pos == NULL || prod_pos == NULL)
	{
		fprintf(stderr, "userspace-loader: failed to alloc memory!\n");
		return false;
	}
	return true;
}

static void free_consumer_producer()
{
	if(!cons_pos)
	{
		free(cons_pos);
	}

	if(!prod_pos)
	{
		free(prod_pos);
	}
}

static bool set_cpus_number()
{
	n_cpus = libbpf_num_possible_cpus();
	if(n_cpus < 1)
	{
		fprintf(stderr, "userspace-loader: failed to get the cpus number\n");
		return false;
	}

	if(!allocate_consumer_producer(n_cpus))
	{
		return false;
	}

	skel->rodata->n_cpus = n_cpus;
	return true;
}

static void clean()
{
	free_consumer_producer();
	ring_buffer__free(ringbuf);
	multi_ringbuf_output_bpf__destroy(skel);
}

static void signal_callback(int signal)
{
	skel->bss->capture_enabled = 0;
	printf("%ld\n", skel->bss->n_drops);
	printf("%ld\n", n_elaborated);
	clean();
	exit(EXIT_SUCCESS);
}

static bool initialize_resources()
{
	libbpf_set_print(libbpf_print_fn);
	bump_memlock_rlimit();
	if(signal(SIGINT, signal_callback) == SIG_ERR)
	{
		fprintf(stderr, "userspace-loader: an error occurred while setting SIGINT signal handler.\n");
		return false;
	}
	return true;
}

static bool open_skeleton()
{

	skel = multi_ringbuf_output_bpf__open();
	if(!skel)
	{
		fprintf(stderr, "userspace-loader: failed to open BPF skeleton\n");
		return false;
	}
	return true;
}

static bool load_inner_map(int *inner_map_fd)
{

	int err = 0;
	*inner_map_fd = bpf_map_create(BPF_MAP_TYPE_RINGBUF, NULL, 0, 0, RING_BUF_DIM, NULL);
	if(*inner_map_fd < 0)
	{
		fprintf(stderr, "userspace-loader: failed to create the dummy inner map\n");
		return false;
	}

	/* set the inner map file descriptor to the outer map. */
	err = bpf_map__set_inner_map_fd(skel->maps.ringbuf_arr, *inner_map_fd);
	if(err)
	{
		fprintf(stderr, "userspace-loader: failed to set the dummy inner map fd\n");
		return false;
	}
	return true;
}

static bool load_skeleton()
{
	int err = multi_ringbuf_output_bpf__load(skel);
	if(err)
	{
		fprintf(stderr, "userspace-loader: failed to load the skeleton\n");
		return false;
	}
	return true;
}

static bool instaciate_ring_buffers()
{

	int outer_map_fd = -1;
	int ringbuffer_fd = -1;
	int err = 0;
	int i = 0;

	/* get the fd of the outer map. */
	outer_map_fd = bpf_map__fd(skel->maps.ringbuf_arr);
	if(outer_map_fd < 0)
	{
		fprintf(stderr, "userspace-loader: failed to get the outer map fd\n");
		return false;
	}

	/* add the first ring_buf and initialize the ring_buffer manager. */
	ringbuffer_fd = bpf_map_create(BPF_MAP_TYPE_RINGBUF, NULL, 0, 0, RING_BUF_DIM, NULL);
	if(ringbuffer_fd < 0)
	{
		fprintf(stderr, "userspace-loader: failed to create the ringbuf for cpu '%d':  error %d\n", i, ringbuffer_fd);
		return false;
	}
	err = bpf_map_update_elem(outer_map_fd, &i, &ringbuffer_fd, BPF_ANY);
	if(err)
	{
		fprintf(stderr, "userspace-loader: failed to update ringbuf_array with the ringbuf for cpu '%d':  error %d\n", i, err);
		return false;
	}

	ringbuf = ring_buffer__new(ringbuffer_fd, handle_event, NULL, NULL);
	if(!ringbuf)
	{
		fprintf(stderr, "userspace-loader: failed to create ring buffer manager\n");
		return false;
	}

	close(ringbuffer_fd); /* to avoid memory leakage. */
	ringbuffer_fd = -1;

	for(i = 1; i < n_cpus; i++)
	{
		ringbuffer_fd = bpf_map_create(BPF_MAP_TYPE_RINGBUF, NULL, 0, 0, RING_BUF_DIM, NULL);
		if(ringbuffer_fd < 0)
		{
			fprintf(stderr, "userspace-loader: failed to create the ringbuf for cpu %d\n", i);
			return false;
		}
		err = bpf_map_update_elem(outer_map_fd, &i, &ringbuffer_fd, BPF_ANY);
		if(err)
		{
			fprintf(stderr, "userspace-loader: failed to update ringbuf array with ringbuf '%d'\n", i);
			return false;
		}

		/* add buffer to the manager. */
		err = ring_buffer__add(ringbuf, ringbuffer_fd, handle_event, NULL);
		if(err)
		{
			fprintf(stderr, "userspace-loader: failed to create ring buffer %d\n", i);
			return false;
		}
		close(ringbuffer_fd); /* to avoid memory leakage. */
		ringbuffer_fd = -1;
	}
	close(outer_map_fd);
	return true;
}

static bool attach_skeleton()
{
	int err = multi_ringbuf_output_bpf__attach(skel);
	if(err)
	{
		fprintf(stderr, "userspace-loader: failed to attach skeleton \n");
		return false;
	}
	return true;
}

static void start_capture()
{
	skel->bss->capture_enabled = 1;
}
