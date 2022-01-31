OUTPUT := .output
CLANG ?= clang
LLVM_STRIP ?= llvm-strip
BPFTOOL ?= $(abspath ../tools/bpftool)
LIBBPF_SRC := $(abspath ../libbpf/src)
LIBBPF_OBJ := $(abspath $(OUTPUT)/libbpf.a)
VMLINUX := ../vmlinux/vmlinux.h

# To configure
SKEL := multi_ringbuf_output.skel
APPS = case1
SHARED := shared_code.h
ARCH := $(shell uname -m | sed 's/x86_64/x86/')
CLANG_FLAGS := -D__TARGET_ARCH_$(ARCH) -D__BYTE_$(BYTE) -D__CUSTOMIZED_$(CUSTOMIZED)
$(info -----------------------------------------------------------------------)
$(info CLANG FLAGS used [${CLANG_FLAGS}])
$(info -----------------------------------------------------------------------)
$(info )

# Use our own libbpf API headers and Linux UAPI headers distributed with
# libbpf to avoid dependency on system-wide headers, which could be missing or
# outdated. Output directory is added into the include path for C compiler to 
# find BPF skeletons and libbpf headers
INCLUDES := -I$(OUTPUT) -I../libbpf/include/uapi -I../libbpf/include/linux -I$(dir $(VMLINUX))

# All the user-space files are compiled with debug info (-g) and without any 
# optimizations to make it easier to debug them
CFLAGS := -g -Wall

# Get Clang's default includes on this system. We'll explicitly add these dirs
# to the includes list when compiling with `-target bpf` because otherwise some
# architecture-specific dirs will be "missing" on some architectures/distros -
# headers such as asm/types.h, asm/byteorder.h, asm/socket.h, asm/sockios.h,
# sys/cdefs.h etc. might be missing.
#
# Use '-idirafter': Don't interfere with include mechanics except where the
# build would have failed anyways.
CLANG_BPF_SYS_INCLUDES = $(shell $(CLANG) -v -E - </dev/null 2>&1 \
	| sed -n '/<...> search starts here:/,/End of search list./{ s| \(/.*\)|-idirafter \1|p }')


ifeq ($(V),1)
	Q =
	msg =
else
	Q = @
	msg = @printf '  %-8s %s%s\n'					\
		      "$(1)"						\
		      "$(patsubst $(abspath $(OUTPUT))/%,%,$(2))"	\
		      "$(if $(3), $(3))";
	MAKEFLAGS += --no-print-directory
endif

.PHONY: all
all: $(APPS)

.PHONY: clean
clean:
	$(call msg,CLEAN)
	$(Q)rm -rf $(OUTPUT) $(APPS) 

$(OUTPUT) $(OUTPUT)/libbpf:
	$(call msg,MKDIR,$@)
	$(Q)mkdir -p $@


# Build libbpf
# First, libbpf is built as a static library and its API headers are installed into .output/:
$(LIBBPF_OBJ): $(wildcard $(LIBBPF_SRC)/*.[ch] $(LIBBPF_SRC)/Makefile) | $(OUTPUT)/libbpf
	$(call msg,LIB,$@)
	$(Q)$(MAKE) -C $(LIBBPF_SRC) BUILD_STATIC_ONLY=1		      \
		    OBJDIR=$(dir $@)/libbpf DESTDIR=$(dir $@)		      \
		    INCLUDEDIR= LIBDIR= UAPIDIR=			      \
		    install

# Build BPF code
# builds BPF C code (*.bpf.c) into a compiled object file:
# 1. We use Clang to do this. -g is mandatory to make Clang emit BTF information.
# 2. -O2 is also necessary for BPF compilation. 
# 3. -D__TARGET_ARCH_$(ARCH) defines necessary macro for bpf_tracing.h header dealing with 
# low-level struct pt_regs macro. You can disregard that if you are not dealing with kprobes 
# and struct pt_regs.
# 4. Finally, we strip DWARF info out from the generated .o file, as it's never used and is 
# mostly just a compilation artifact of Clang. BTF is the only info necessary for 
# BPF functionality and that one is preserved during stripping. It's important to reduce 
# the size of a .bpf.o file because it will get embedded into the final application binary 
# through BPF skeleton, so there is no need to increase its size with unneeded DWARF data.
$(OUTPUT)/%.bpf.o: %.bpf.c $(LIBBPF_OBJ) $(wildcard %.h) $(VMLINUX) | $(OUTPUT)
	$(call msg,BPF,$@)
	$(Q)$(CLANG) -g -O2 -target bpf $(CLANG_FLAGS) $(INCLUDES) $(CLANG_BPF_SYS_INCLUDES) -c $(filter %.c,$^) -o $@
	$(Q)$(LLVM_STRIP) -g $@ # strip useless DWARF info

# Generate BPF skeletons
# Now that we have .bpf.o file generated, bpftool is used to generate a corresponding 
# BPF skeleton header (.skel.h) with bpftool gen skeleton command:
$(OUTPUT)/%.skel.h: $(OUTPUT)/%.bpf.o | $(OUTPUT)
	$(call msg,GEN-SKEL,$@)
	$(Q)$(BPFTOOL) gen skeleton $< > $@

# Build user-space code
# With that, we make sure that whenever BPF skeleton is updated, user-space parts of the 
# application are rebuilt as well, because they need to embed BPF skeleton during the 
# compilation. The compilation of user-space .c → .o is pretty straightforward otherwise:
$(patsubst %,$(OUTPUT)/%.o,$(APPS)): %.o: $(OUTPUT)/$(SKEL).h


$(OUTPUT)/%.o: %.c $(SHARED) | $(OUTPUT)
	$(call msg,CC,$@)
	$(Q)$(CC) $(CFLAGS) $(INCLUDES) -c $(filter %.c,$^) -o $@

# Build application binary
# Finally, using only user-space .o file (together with libbpf.a static library) the final 
# binary is generated. -lelf and -lz are dependencies of libbpf and need to be provided 
# explicitly to the compiler:
$(APPS): %: $(OUTPUT)/%.o $(LIBBPF_OBJ) | $(OUTPUT)
	$(call msg,BINARY,$@)
	$(Q)$(CC) $(CFLAGS) $^ -lelf -lz -o $@

# delete failed targets
.DELETE_ON_ERROR:

# keep intermediate (.skel.h, .bpf.o, etc) targets
.SECONDARY:

