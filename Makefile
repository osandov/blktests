all:
	$(MAKE) -C src all

clean:
	$(MAKE) -C src clean

# SC2034: "VARIABLE appears unused". All test scripts use this for the test
# metadata, and many helper functions define global variables.
# SC2119: "Use foo "$@" if function's $1 should mean script's $1". False
# positives on helpers like _init_scsi_debug.
SHELLCHECK_EXCLUDE := SC2034,SC2119

check:
	shellcheck -x -e $(SHELLCHECK_EXCLUDE) -f gcc check new common/* tests/*/[0-9]*[0-9]
	! grep TODO tests/*/[0-9]*[0-9]

.PHONY: all check
