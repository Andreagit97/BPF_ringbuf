#include <vmlinux.h>
#include <bpf/bpf_helpers.h>

#if defined(__BYTE_512)  	// 1 KB - 4 KB
#define BYTE 512  
#elif defined(__BYTE_2048)  // 4 KB - 16 KB
#define BYTE 2048
#else
#define BYTE 200
#endif

struct event
{
	char path[BYTE];
};


#define MAX_CPUS 32

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


struct {
	__uint(type, BPF_MAP_TYPE_PERCPU_ARRAY);
	__uint(max_entries, MAX_CPUS);
	__type(key, int);
	__type(value, struct event);
} heap SEC(".maps");

char LICENSE[] SEC("license") = "Dual BSD/GPL";

int capture_enabled = 0;
long n_drops = 0;
long n_skipped = 0;
const volatile int n_cpus = 0;


SEC("fentry/__x64_sys_getpgid")
int catch_fentry(void *ctx)
{

	struct event *e;

	if(!capture_enabled)
	{
		return 0;
	}

	void *rb;
	uint32_t cpu = bpf_get_smp_processor_id();
	rb = bpf_map_lookup_elem(&ringbuf_arr, &cpu);
	
	// use a temporary map
	e = bpf_map_lookup_elem(&heap, &cpu);
	if(!rb || !e)
	{
		n_skipped++;
		return 0;
	}

	long err = 0;
	err = bpf_ringbuf_output(rb, e, sizeof(*e),  BPF_RB_NO_WAKEUP);
	if(err!=0){
		n_skipped++;
	}
	return 0;
}