TARGETS := \
	loblksize \
	loop_get_status_null \
	openclose \
	sg/dxfer-from-dev \
	sg/syzkaller1

CFLAGS := -O2

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

$(TARGETS): %: %.c
	$(CC) -Wall -o $@ $(CPPFLAGS) $(CFLAGS) $^

.PHONY: all clean
