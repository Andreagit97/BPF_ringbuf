#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <bpf/libbpf.h>
#include <errno.h>
#include <signal.h>
#include <compiler.h>

#include "single_ringbuf_reserve.skel.h"

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
	/* clear out top 2 bits (discard and busy, if set) */
	len <<= 2;
	len >>= 2;
	/* add length prefix */
	len += BPF_RINGBUF_HDR_SZ;
	/* round up to 8 byte alignment */
	return (len + 7) / 8 * 8;
}

static unsigned long cons_pos = 0;
static unsigned long prod_pos = 0;

static void *ringbuf_process_one_from_ring(struct ring *r)
{
	int *len_ptr, len;
	void *sample = NULL;

	if(cons_pos >= prod_pos)
	{
		prod_pos = smp_load_acquire(r->producer_pos);
	}

	if(cons_pos < prod_pos)
	{
		len_ptr = r->data + (cons_pos & r->mask);
		len = smp_load_acquire(len_ptr);

		/* Check in the header if the bit is active, sample not committed yet, bail out for now */
		if(len & BPF_RINGBUF_BUSY_BIT)
		{
			goto done;
		}

		cons_pos += roundup_len(len);

		/* the sample is discarded kernel side. */
		if((len & BPF_RINGBUF_DISCARD_BIT) == 0)
		{
			sample = (void *)len_ptr + BPF_RINGBUF_HDR_SZ;
		}

		smp_store_release(r->consumer_pos, cons_pos);
	}
done:
	return sample;
}

int ring_buffer__consume_one(struct ring_buffer *rb, void **sample)
{
	struct ring *ring = &rb->rings[0];
	*sample = ringbuf_process_one_from_ring(ring);
	if(*sample != NULL)
	{
		return 1;
	}
	return 0;
}

uint64_t n_elaborated = 0;
struct single_ringbuf_reserve_bpf *skel;

static void signal_callback(int signal)
{
	skel->bss->capture_enabled = 0;
	printf("%ld\n", skel->bss->n_drops);
	printf("%lu\n", n_elaborated);
	single_ringbuf_reserve_bpf__destroy(skel);
	exit(EXIT_SUCCESS);
}

static int libbpf_print_fn(enum libbpf_print_level level, const char *format, va_list args)
{
	// return vfprintf(stderr, format, args);
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
		fprintf(stderr, "Failed to increase RLIMIT_MEMLOCK limit!\n");
		exit(1);
	}
}

static int handle_event(void *ctx, void *data, size_t data_sz)
{
	return 0;
}

int main(int argc, char **argv)
{
	int err;

	libbpf_set_print(libbpf_print_fn);

	bump_memlock_rlimit();

	if(signal(SIGINT, signal_callback) == SIG_ERR)
	{
		fprintf(stderr, "An error occurred while setting SIGINT signal handler.\n");
		return EXIT_FAILURE;
	}

	skel = single_ringbuf_reserve_bpf__open();
	if(!skel)
	{
		fprintf(stderr, "Failed to open BPF skeleton\n");
		return EXIT_FAILURE;
	}

	err = single_ringbuf_reserve_bpf__load(skel);
	if(err)
	{
		fprintf(stderr, "Failed to load and verify BPF skeleton\n");
		goto cleanup;
	}

	struct ring_buffer *rb = ring_buffer__new(bpf_map__fd(skel->maps.rb), handle_event, NULL, NULL);
	if(!rb)
	{
		fprintf(stderr, "failed to create ring buffer\n");
		goto cleanup;
	}

	err = single_ringbuf_reserve_bpf__attach(skel);
	if(err)
	{
		fprintf(stderr, "Failed to attach BPF skeleton\n");
		goto cleanup;
	}

	int reads = 0;
	void *header = NULL;

	/* enable capture */
	skel->bss->capture_enabled = 1;
	while(1)
	{
		/* our custom API consumes just one event. */
		reads = ring_buffer__consume_one(rb, &header);
		n_elaborated += reads;
	}

	return EXIT_SUCCESS;

cleanup:
	single_ringbuf_reserve_bpf__destroy(skel);
	return -err;
}