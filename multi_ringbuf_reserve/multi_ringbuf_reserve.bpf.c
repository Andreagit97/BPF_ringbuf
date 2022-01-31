#include <vmlinux.h>
#include <bpf/bpf_helpers.h>

#if defined(__BYTE_512)  	// 1 KB - 4 KB
#define BYTE 512  
#elif defined(__BYTE_2048)  // 4 KB - 16 KB
#define BYTE 2048
#else
#define BYTE 1024  // 2 KB - 8 KB
#endif

#if defined(__CUSTOMIZED_1)
#define CUSTOMIZED 1
#endif

#define KB_2 2 * BYTE
#define KB_3 3 * BYTE
#define KB_4 4 * BYTE
#define KB_5 5 * BYTE
#define KB_6 6 * BYTE
#define KB_7 7 * BYTE
#define KB_8 8 * BYTE


#define MAX_CPUS 64

struct ringbuf_map
{
	__uint(type, BPF_MAP_TYPE_RINGBUF);
};

struct
{
	__uint(type, BPF_MAP_TYPE_ARRAY_OF_MAPS);
	__uint(max_entries, MAX_CPUS);
	__type(key, u32);
	__type(value, u32); /* this would be the `fd` of inner maps */
	__array(values, struct ringbuf_map);
} ringbuf_arr SEC(".maps");

char LICENSE[] SEC("license") = "Dual BSD/GPL";

int capture_enabled = 0;
long n_drops = 0;
long n_skipped = 0;
const volatile int n_cpus = 0;

static __always_inline void *get_best_buffer()
{
	int j = 0;
	void *chosen_rb = NULL;
	void *rb = NULL;
	u64 min_remained = 16 * 1024 * 1024;
	U64 rb_remained = 16 * 1024 * 1024;
	int max = n_cpus;

	for(j = 0; j < max; j++)
	{
		int i = j;
		rb = bpf_map_lookup_elem(&ringbuf_arr, &i);
		if(rb != NULL)
		{
			rb_remained = bpf_ringbuf_query(rb, BPF_RB_AVAIL_DATA);
			if(rb_remained <= min_remained)
			{
				min_remained = rb_remained;
				chosen_rb = rb;
			}
		}
	}

	return chosen_rb;
}

SEC("fentry/__x64_sys_getpgid")
int catch_fentry(void *ctx)
{
	if(!capture_enabled)
	{
		return 0;
	}

	void *rb;
#ifdef CUSTOMIZED
	rb = get_best_buffer();
#else
	uint32_t cpu = bpf_get_smp_processor_id();
	rb = bpf_map_lookup_elem(&ringbuf_arr, &cpu);
#endif
	if(!rb)
	{
		n_skipped++;
		return 0;
	}

	void *event;

	uint32_t i = bpf_get_prandom_u32() % 7;

	switch(i)
	{
	case 0:
		event = bpf_ringbuf_reserve(rb, KB_2, 0);
		break;
	case 1:
		event = bpf_ringbuf_reserve(rb, KB_3, 0);
		break;
	case 2:
		event = bpf_ringbuf_reserve(rb, KB_4, 0);
		break;
	case 3:
		event = bpf_ringbuf_reserve(rb, KB_5, 0);
		break;
	case 4:
		event = bpf_ringbuf_reserve(rb, KB_6, 0);
		break;
	case 5:
		event = bpf_ringbuf_reserve(rb, KB_7, 0);
		break;
	case 6:
		event = bpf_ringbuf_reserve(rb, KB_8, 0);
		break;
	default:
		event = bpf_ringbuf_reserve(rb, KB_8, 0);
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