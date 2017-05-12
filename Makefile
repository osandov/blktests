all:
	@echo "Please read README.md"

shellcheck:
	shellcheck -x -f gcc check new common/* tests/*/[0-9]*[0-9]

.PHONY: all shellcheck
