HAVE_C_HEADER = $(shell if echo "\#include <$(1)>" |			\
		  $(CC) -x c -c - 2>/dev/null; then echo "$(2)";	\
		  else echo "$(3)"; fi)

C_TARGETS := \
	loblksize \
	loop_get_status_null \
	openclose \
	sg/dxfer-from-dev \
	sg/syzkaller1 \
	nbdsetsize \
	loop_change_fd \
	zbdioctl

CXX_TARGETS := \
	discontiguous-io

TARGETS := $(C_TARGETS) $(CXX_TARGETS)

CONFIG_DEFS := $(call HAVE_C_HEADER,linux/blkzoned.h,-DHAVE_LINUX_BLKZONED_H)

override CFLAGS   := -O2 -Wall -Wshadow $(CFLAGS) $(CONFIG_DEFS)
override CXXFLAGS := -O2 -std=c++11 -Wall -Wextra -Wshadow -Wno-sign-compare \
		     -Werror $(CXXFLAGS) $(CONFIG_DEFS)

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

install: $(TARGETS)
	install -m755 -d $(dest)
	install $(TARGETS) $(dest)

$(C_TARGETS): %: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

$(CXX_TARGETS): %: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^

.PHONY: all clean install
