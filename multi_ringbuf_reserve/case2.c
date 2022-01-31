#include "shared_code.h"

int main(int argc, char **argv)
{
	int inner_map_fd = -1;

	/*
	 * Initialize signal handler and libbpf stuff.
	 */
	if(!initialize_resources())
	{
		return EXIT_FAILURE;
	}

	/*
	 * Open phase.
	 */
	if(!open_skeleton())
	{
		return EXIT_FAILURE;
	}

	/* 
	 * Initialize the inner map for our map of map.
	 */
	if(!load_inner_map(&inner_map_fd))
	{
		goto cleanup;
	}

	/*
	 * Set CPUs number.
	 */
	if(!set_cpus_number())
	{
		goto cleanup;
	}

	/* 
	 * Load phase.
	 */
	if(!load_skeleton())
	{
		goto cleanup;
	}

	/* close the inner map fd to avoid memory leakage. */
	close(inner_map_fd);
	inner_map_fd = -1;

	/* 
	 * Create a ring buffer for every available cpu.
	 */
	if(!instaciate_ring_buffers())
	{
		goto cleanup;
	}

	/* 
	 * Attach phase.
	 */
	if(!attach_skeleton())
	{
		goto cleanup;
	}

	/* 
	 * Start capture.
	 */
	start_capture();
	while(1)
	{
		n_elaborated += ring_buffer__consume(ringbuf);
	}

	return EXIT_SUCCESS;

cleanup:
	clean();
	return EXIT_FAILURE;
}