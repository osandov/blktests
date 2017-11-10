all:
	$(MAKE) -C src all

clean:
	$(MAKE) -C src clean

# SC1090: "Can't follow non-constant source". We use variable sources all over
# the place.
# SC2034: "VARIABLE appears unused". All test scripts use this for the test
# metadata, and many helper functions define global variables.
# SC2119: "Use foo "$@" if function's $1 should mean script's $1". False
# positives on helpers like _init_scsi_debug.
# SC2154: "VARIABLE is referenced but not assigned". False positives on
# TEST_RUN[foo]=bar.
SHELLCHECK_EXCLUDE := SC1090,SC2034,SC2119,SC2154

shellcheck:
	shellcheck -x -e $(SHELLCHECK_EXCLUDE) -f gcc check new common/* tests/*/[0-9]*[0-9]

.PHONY: all shellcheck
