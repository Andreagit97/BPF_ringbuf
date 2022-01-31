#/bin/sh

../tools/bpftool btf dump file ${1:-/sys/kernel/btf/vmlinux} format c > vmlinux.h
