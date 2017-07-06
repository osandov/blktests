TARGETS := sg/dxfer-from-dev sg/syzkaller1

CFLAGS := -O2

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

$(TARGETS): %: %.c
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $^

.PHONY: all clean
