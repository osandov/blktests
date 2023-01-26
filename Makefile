prefix ?= /usr/local
dest = $(DESTDIR)$(prefix)/blktests

all:
	$(MAKE) -C src all

clean:
	$(MAKE) -C src clean

install:
	install -m755 -d $(dest)
	install check $(dest)
	cp -R tests common $(dest)
	$(MAKE) -C src dest=$(dest)/src install

SHELLCHECK_EXCLUDE := ""

check:
	shellcheck -x -e $(SHELLCHECK_EXCLUDE) -f gcc check new common/* \
		tests/*/rc tests/*/[0-9]*[0-9]
	! grep TODO tests/*/rc tests/*/[0-9]*[0-9]
	! find -name '*.out' -perm /u=x+g=x+o=x -printf '%p is executable\n' | grep .

.PHONY: all check install
