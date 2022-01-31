#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <bpf/libbpf.h>
#include <errno.h>
#include <signal.h>

#include "single_ringbuf_reserve.skel.h"

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

	/* enable capture */
	skel->bss->capture_enabled = 1;
	while(1)
	{
		/* libbpf standard API. */
		reads = ring_buffer__consume(rb);
		n_elaborated += reads;
	}

	return EXIT_SUCCESS;

cleanup:
	single_ringbuf_reserve_bpf__destroy(skel);
	return -err;
}