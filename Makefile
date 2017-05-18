all:
	$(MAKE) -C src all

clean:
	$(MAKE) -C src clean

shellcheck:
	shellcheck -x -f gcc check new common/* tests/*/[0-9]*[0-9]

.PHONY: all shellcheck
