#include "vmlinux.h"
#include <bpf/bpf_helpers.h>

#if defined(__BYTE_10)
#define BYTE 10
#elif defined(__BYTE_100)
#define BYTE 100
#else
#define BYTE 1024
#endif

#define KB_2 2 * BYTE
#define KB_3 3 * BYTE
#define KB_4 4 * BYTE
#define KB_5 5 * BYTE
#define KB_6 6 * BYTE
#define KB_7 7 * BYTE
#define KB_8 8 * BYTE
#define KB_9 9 * BYTE
#define KB_10 10 * BYTE
#define KB_11 11 * BYTE
#define KB_12 12 * BYTE
#define KB_13 13 * BYTE
#define KB_14 14 * BYTE
#define KB_15 15 * BYTE
#define KB_16 16 * BYTE
#define KB_17 17 * BYTE
#define KB_18 18 * BYTE
#define KB_19 19 * BYTE
#define KB_20 20 * BYTE

#define MAX_BUF_DIM 64 * 1024 * 1024

struct
{
	__uint(type, BPF_MAP_TYPE_RINGBUF);
	__uint(max_entries, MAX_BUF_DIM);
} rb SEC(".maps");

char LICENSE[] SEC("license") = "Dual BSD/GPL";

int capture_enabled = 0;
long n_drops = 0;

SEC("fentry/__x64_sys_getpgid")
int catch_fentry_one(void *ctx)
{

	if(!capture_enabled)
	{
		return 0;
	}

	void *event;
	uint32_t i = bpf_get_prandom_u32() % 19;

	switch(i)
	{
	case 0:
		event = bpf_ringbuf_reserve(&rb, KB_2, 0);
		break;
	case 1:
		event = bpf_ringbuf_reserve(&rb, KB_3, 0);
		break;
	case 2:
		event = bpf_ringbuf_reserve(&rb, KB_4, 0);
		break;
	case 3:
		event = bpf_ringbuf_reserve(&rb, KB_5, 0);
		break;
	case 4:
		event = bpf_ringbuf_reserve(&rb, KB_6, 0);
		break;
	case 5:
		event = bpf_ringbuf_reserve(&rb, KB_7, 0);
		break;
	case 6:
		event = bpf_ringbuf_reserve(&rb, KB_8, 0);
		break;
	case 7:
		event = bpf_ringbuf_reserve(&rb, KB_9, 0);
		break;
	case 8:
		event = bpf_ringbuf_reserve(&rb, KB_10, 0);
		break;
	case 9:
		event = bpf_ringbuf_reserve(&rb, KB_11, 0);
		break;
	case 10:
		event = bpf_ringbuf_reserve(&rb, KB_12, 0);
		break;
	case 11:
		event = bpf_ringbuf_reserve(&rb, KB_13, 0);
		break;
	case 12:
		event = bpf_ringbuf_reserve(&rb, KB_14, 0);
		break;
	case 13:
		event = bpf_ringbuf_reserve(&rb, KB_15, 0);
		break;
	case 14:
		event = bpf_ringbuf_reserve(&rb, KB_16, 0);
		break;
	case 15:
		event = bpf_ringbuf_reserve(&rb, KB_17, 0);
		break;
	case 16:
		event = bpf_ringbuf_reserve(&rb, KB_18, 0);
		break;
	case 17:
		event = bpf_ringbuf_reserve(&rb, KB_19, 0);
		break;
	case 18:
		event = bpf_ringbuf_reserve(&rb, KB_20, 0);
		break;
	default:
		event = bpf_ringbuf_reserve(&rb, KB_20, 0);
		break;
	}

	if(!event)
	{
		n_drops++;
		return 0;
	}

	bpf_ringbuf_submit(event, BPF_RB_NO_WAKEUP);

	return 0;
}