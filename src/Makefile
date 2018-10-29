C_TARGETS := \
	loblksize \
	loop_get_status_null \
	openclose \
	sg/dxfer-from-dev \
	sg/syzkaller1 \
	nbdsetsize \
	loop_change_fd

CXX_TARGETS := \
	discontiguous-io

TARGETS := $(C_TARGETS) $(CXX_TARGETS)

CFLAGS := -O2 -Wall -Wshadow
CXXFLAGS := -O2 -std=c++11 -Wall -Wextra -Wshadow -Wno-sign-compare -Werror

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

$(C_TARGETS): %: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

$(CXX_TARGETS): %: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^

.PHONY: all clean
