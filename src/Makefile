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

override CFLAGS   := -O2 -Wall -Wshadow $(CFLAGS)
override CXXFLAGS := -O2 -std=c++11 -Wall -Wextra -Wshadow -Wno-sign-compare \
		     -Werror $(CXXFLAGS)

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
