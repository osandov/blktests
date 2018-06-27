C_TARGETS := \
	loblksize \
	loop_get_status_null \
	openclose \
	sg/dxfer-from-dev \
	sg/syzkaller1 \
	nbdsetsize

TARGETS := $(C_TARGETS)

CFLAGS := -O2

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

$(C_TARGETS): %: %.c
	$(CC) -Wall -o $@ $(CPPFLAGS) $(CFLAGS) $^

.PHONY: all clean
