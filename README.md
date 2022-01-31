# BPF ringbuf performance.

## Introduction :book: 

I created this repository with the idea of testing the performance of the new BPF `ringbuf` API in different scenarios. More precisely, we have `3` main scenarios:

1. `multi_ringbuf_output`: test the `bpf_ringbuf_output()` API with one ringbuf for every cpu.
2. `multi_ringbuf_reserve`: test the `bpf_ringbuf_reserve()` API with one ringbuf for every cpu.
3. `single_ringbuf_reserve`: test the `bpf_ringbuf_reserve()` API with one ringbuf shared between all cpus.

Let's see what tests are available and how we can run them.

## Tests available :boxing_glove:

### :red_circle: Multi_ringbuf_output

**BPF side**

The bpf side of the application is shared between all tests:

- One ring buffer for every cpu available.
- One auxiliary map for every cpu available.
- Bpf programs save data in an auxiliary map before pushing them in the ring buffer with the `bpf_ringbuf_output()` API.
- Every bpf program sends `n` Bytes to the userspace side. We will see in the build section how to set this `n` variable. 

**Userspace side**

What changes is the userspace side:

* `case1`:
    1. consume all events from all the ring buffers each time the "consume" API is called;
    2. increment the consumer position at every record consumed;
    3. switch the ring buffer to consume when no more records are available. 

### :red_circle: Multi_ringbuf_reserve

**BPF side**

The bpf side of the application is shared between all tests:

- One ring buffer for every cpu available.
- Bpf programs save data directly in the ring buffer with the `bpf_ringbuf_reserve()` API.
- Every bpf program sends `n` Bytes to the userspace side. We will see in the build section how to set this `n` variable. 

**Userspace side**

What changes is the userspace side:

* `case1`: 
    1. consume one event from one of the ring buffers each time the "consume" API is called;
    2. increment the consumer position at every record consumed;
    3. switch the ring buffer to consume at every record consumed.  
    
* `case2`: 
    1. consume all events from all the ring buffers each time the "consume" API is called;
    2. increment the consumer position at every record consumed;
    3. switch the ring buffer to consume when no more records are available. 

* `case3`: 
    1. consume one event from one of the ring buffers each time the "consume" API is called;
    2. increment the consumer position when it reaches the producer position;
    3. switch the ring buffer when it reaches the producer position.

* `case4`: 
    1. consume one event from one of the ring buffers each time the "consume" API is called; 
    2. increment the consumer position at every record consumed;
    3. switch the ring buffer when it reaches the producer position.


### :red_circle: Single-ringbuf_reserve

**BPF side**

The bpf side of the application is shared between all tests:

- One ringbuf shared between all cpus.
- Bpf programs save data directly in the ring buffer with the `bpf_ringbuf_reserve()` API.
- Every bpf program sends `n` Bytes to the userspace side. We will see in the build section how to set this `n` variable. 

**Userspace side**

What changes is the userspace side:

* `case1`: 
    1. consume one event from the ring buffer each time the "consume" API is called;
    2. increment the consumer position at every record consumed;

* `case2`: 
    1. consume all events from the ring buffer each time the "consume" API is called;
    2. increment the consumer position at every record consumed;


## Requirements :eye_speech_bubble:

1. At least kernel 5.8, support for `ringbuf` API is required. 
2. Kernel with BTF enabled (check if the `vmlinux` file is here `/sys/kernel/btf/vmlinux`).
3. At least `clang-10`.
4. A recent version of `gcc`.
5. `make` installed on your system.
6. `libelf` installed on your system.

## Configure the environment :bulb:

1. Clone repository:

```bash
git clone https://github.com/Andreagit97/BPF_ringbuf.git
```

2. Configure the libbpf submodule:

```bash 
git submodule init
git submodule update
```

## Build a test case  :building_construction:

First of all, we have to choose a test case among those proposed above. Here I choose a `multi_ringbuf_reserve` example, more precisely the `case3`:

```bash
cd multi_ringbuf_reserve
make case3
```

You can build all tests in the folder by simply typing `make`.

If you want to send a specific number of bytes from the bpf side, you have to compile in that way:

```bash
make -B BYTE=2048
```
>__NOTE__: the `-B` option is necessary to recompile from scratch all programs with the new number of bytes.

Please look at the code to see what byte sizes are allowed...

## Run a test case :racing_car:

After the test case building, we can run it in the `/test` folder. The `/test` folder has the same structure as the source code:

1. `multi_ringbuf_output`.
2. `multi_ringbuf_reserve`.
3. `single_ringbuf_reserve`.

Since we first built an example for `multi_ringbuf_reserve`, let's move inside the corresponding directory:

```bash
cd multi_ringbuf_reserve
```

Here we have to use the `average` script. These are the parameters that the `average` script requires:

```
$1 = program to call `case1`, `case2`, `case3`, ...
$2 = number of parallel stress instances.
$3 = stress test duration.
$4 = use dedicated cpu for each process (no, yes).
$5 = number of times we repeat the test (to have a better average).
```

We have built the `case3` test so we can launch it by typing:

```bash
sudo ./average.sh case3 5 10 no 3
```

- We have `5` parallel producers that push data in the ring buffer for `10` seconds. 
- Every producer can use any cpu: there is no reservation.
- We repeat the test 3 times.

The result will be something like this:

```
- Drops: 0          # number of events dropped (the ring buffer is full). 
- Elaborated: 0     # number of events correctly sent to userspace.
```

You can repeat this procedure with every test you want to try. Make sure you've built the corresponding test before running the `average` script.