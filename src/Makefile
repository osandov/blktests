TARGETS := fsstress loblksize sg/dxfer-from-dev sg/syzkaller1

CFLAGS := -O2

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

$(TARGETS): %: %.c
	$(CC) -Wall -o $@ $(CPPFLAGS) $(CFLAGS) $^

fsstress: fsstress.c
	$(CC) -Wall -o $@ $(CPPFLAGS) $(CFLAGS) $^ -D_GNU_SOURCE \
		-D_FILE_OFFSET_BITS=64 -DHAVE_ATTR_XATTR_H \
		-DHAVE_ATTR_ATTRIBUTES_H \
		-DHAVE_LINUX_FIEMAP_H -DHAVE_ATTR_LIST -DHAVE_SYS_PRCTL_H \
		-DAIO -DFALLOCATE -lattr -laio

.PHONY: all clean
