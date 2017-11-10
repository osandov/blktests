TARGETS := sg/dxfer-from-dev sg/syzkaller1 loblksize openclose

CFLAGS := -O2

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

$(TARGETS): %: %.c
	$(CC) -Wall -o $@ $(CPPFLAGS) $(CFLAGS) $^

.PHONY: all clean
