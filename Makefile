all:
	$(MAKE) -C src all

clean:
	$(MAKE) -C src clean

# SC2119: "Use foo "$@" if function's $1 should mean script's $1". False
# positives on helpers like _init_scsi_debug.
SHELLCHECK_EXCLUDE := SC2119

check:
	shellcheck -x -e $(SHELLCHECK_EXCLUDE) -f gcc check new common/* \
		tests/*/rc tests/*/[0-9]*[0-9]
	! grep TODO tests/*/rc tests/*/[0-9]*[0-9]

.PHONY: all check
