#include "shared_code.h"
#include <compiler.h>

int ringbuf_pos = 0;

void *ringbuf_process_one_from_ring(struct ring *r)
{
	int *len_ptr, len;
	void *sample = NULL;

	if(cons_pos[ringbuf_pos] >= prod_pos[ringbuf_pos])
	{
		prod_pos[ringbuf_pos] = smp_load_acquire(r->producer_pos);
	}

	if(cons_pos[ringbuf_pos] < prod_pos[ringbuf_pos])
	{
		len_ptr = r->data + (cons_pos[ringbuf_pos] & r->mask);
		/* this `len` is the size required by the bpf side, without 8 bytes of header. */
		len = smp_load_acquire(len_ptr);

		/* sample not committed yet, bail out for now */
		if(len & BPF_RINGBUF_BUSY_BIT)
		{
			goto done;
		}

		/* here we add `len + 8 bytes + roundup`. */
		cons_pos[ringbuf_pos] += roundup_len(len);

		/* the sample is discarded kernel side. */
		if((len & BPF_RINGBUF_DISCARD_BIT) == 0)
		{
			sample = (void *)len_ptr + BPF_RINGBUF_HDR_SZ;
		}

		smp_store_release(r->consumer_pos, cons_pos[ringbuf_pos]);
	}
done:
	return sample;
}

int ring_buffer__consume_one(struct ring_buffer *rb, void **sample)
{
	while(true)
	{
		struct ring *ring = &rb->rings[ringbuf_pos];
		*sample = ringbuf_process_one_from_ring(ring);
		ringbuf_pos = (ringbuf_pos + 1) % rb->ring_cnt;
		if(*sample != NULL)
		{
			return 1;
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	int inner_map_fd = -1;
	void *header = NULL;

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
	 * Initialize the inner map of the outer map.
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
		/* this custom API consumes just one record each time is called. */
		n_elaborated += ring_buffer__consume_one(ringbuf, &header);
	}

	return EXIT_SUCCESS;

cleanup:
	clean();
	return EXIT_FAILURE;
}