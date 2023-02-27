H := \#

HAVE_C_HEADER = $(shell if echo "$(H)include <$(1)>" |		\
		$(CC) -E - > /dev/null 2>&1; then echo "$(2)";	\
		else echo "$(3)"; fi)

HAVE_C_MACRO = $(shell if echo "$(H)include <$(1)>" |	\
		$(CC) -E - 2>&1 /dev/null | grep $(2) > /dev/null 2>&1; \
		then echo 1;else echo 0; fi)

C_TARGETS := \
	loblksize \
	loop_change_fd \
	loop_get_status_null \
	mount_clear_sock \
	nbdsetsize \
	openclose \
	sg/dxfer-from-dev \
	sg/syzkaller1 \
	zbdioctl

C_MINIUBLK := miniublk

HAVE_LIBURING := $(call HAVE_C_MACRO,liburing.h,IORING_OP_URING_CMD)
HAVE_UBLK_HEADER := $(call HAVE_C_HEADER,linux/ublk_cmd.h,1)

CXX_TARGETS := \
	discontiguous-io

ifeq ($(HAVE_LIBURING)$(HAVE_UBLK_HEADER), 11)
TARGETS := $(C_TARGETS) $(CXX_TARGETS) $(C_MINIUBLK)
else
$(info Skip $(C_MINIUBLK) build due to missing kernel header(v6.0+) or liburing(2.2+))
TARGETS := $(C_TARGETS) $(CXX_TARGETS)
endif

CONFIG_DEFS := $(call HAVE_C_HEADER,linux/blkzoned.h,-DHAVE_LINUX_BLKZONED_H)

override CFLAGS   := -O2 -Wall -Wshadow $(CFLAGS) $(CONFIG_DEFS)
override CXXFLAGS := -O2 -std=c++11 -Wall -Wextra -Wshadow -Wno-sign-compare \
		     -Werror $(CXXFLAGS) $(CONFIG_DEFS)
MINIUBLK_FLAGS :=  -D_GNU_SOURCE -lpthread -luring

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

$(C_MINIUBLK): %: miniublk.c
	$(CC) $(CFLAGS) $(MINIUBLK_FLAGS) -o $@ miniublk.c

.PHONY: all clean install
