# Writing Tests

The `./new` script creates new test cases and categories from a template. The
template includes `TODO` comments where the test should be filled in.

## Categories

Each category has a top-level bash script, `category`, which defines the
requirements for that category.

The `category` script should define a `prepare()` function which returns zero
if tests in that category can be run and non-zero otherwise, in which case it
should set the `$SKIP_REASON` variable. Usually, `prepare()` should just call
any relevant `_have_foo` helpers to check that necessary programs and kernel
features are available. For example,

```sh
prepare() {
	_have_loop && _have_wbt
}
```

Additionally, the `category` script should define a `prepare_device()`
function. `prepare_device()` will be called with the `$TEST_DEV` variable
defined and should return zero if tests in that category can be run on
`$TEST_DEV` and non-zero otherwise. Usually, `prepare_device()` should just
call any relevant `_test_dev_foo` helpers to check, for example, that the
device is the right kind of hardware.

```sh
prepare_device() {
	_test_dev_is_nvme
}
```

If the category has no special requirements, `prepare()` and/or
`prepare_device()` can just `return 0`.

Functions (other than `prepare()` and `prepare_device()`) and variables defined
in the `category` script will be available in the test cases themselves.

## Test Cases

Tests are bash scripts defining a few functions and variables as described
below.

Tests are executed with a few basic shell variables defined. The `$TEST_NAME`
variable is the full name of the test. The `$FULL` variable is a file path in
the `results` directory where you can log verbose output to. The `$TMPDIR`
variable is a temporary directory created before the test and removed after it
runs.

`test()` is the main test function; its output (stdout and stderr) is compared
to `tests/${TEST_NAME}.out`. If the output does not match, the test is
considered a failure. Additionally, if `test()` returns non-zero, it is
considered a failure. You should prefer letting the test fail because of broken
output over, say, checking the exit status of every command you run.

### Tests Not Requiring a Device

Many tests do not require a configured block device, usually because they set
up a pseudo-device like null-blk or scsi-debug. These test should define a
`test()` function. They can also define a `prepare()` function which works the
same way as it does in the `category` script. The `test()` function will be
called iff the `prepare()` function returned zero.

### Device Tests

Tests that require a device should define `test_device()` instead of `test()`.
`test_device()` is called with the `$TEST_DEV` variable defined for each device
the test should run on. It is not called for any devices for which the
`category` script's `prepare_device()` returned non-zero. Additionally, the
test can define its own `prepare()` or `prepare_device()` functions.

### Test Groups

The `GROUPS` array variable defines the groups that the test belongs to. The
test is automatically added to a group for the test category and the "auto"
group. If the test shouldn't be run by default (e.g., because it is flaky or
dangerous to run), remove the "auto" group. If the test runs quickly (i.e., ~30
seconds or less on reasonable hardware), add the "quick" group.

Add any other groups describing what functionality is exercised by the test,
like "discard" or "hotplug". Feel free to add new groups if it makes sense.

### Helpers

`common` contains several helper libraries which you can source in your test.
Functionality shared between tests should be factored out into these helper
scripts.

In addition to the `_have_foo` and `_test_dev_foo` helpers mentioned, the
`_filter_foo`-style helpers are commonly used for removing test-specific output
so it matches the golden output.

### Dmesg

By default, the testing framework checks `dmesg` for warnings, oopses, etc.,
after a test runs. You can suppress this with

```sh
CHECK_DMESG=0
```

If there are specific oopses or warnings you want to suppress, you can define a
filter:

```sh
DMESG_FILTER="grep -v sysfs"
```

### Regression Tests

If the test you are writing is a regression test, make sure you indicate this
in the test description. For a patch, use the patch title:

``` sh
# Regression test for patch "blk-stat: fix blk_stat_sum() if all samples are
# batched".
```

For a commit, use the first 12 characters of the commit hash and the one-line
commit summary:

``` sh
# Regression test for commit efd4b81abbe1 ("blk-stat: fix blk_stat_sum() if all
# samples are batched").
```

## Coding Style and General Guidelines

- Indent with tabs.
- Don't add a space before the parentheses or a newline before the curly brace
  in function definitions.
- Variables set and used by the testing framework are in caps with underscores.
  E.g., `TEST_NAME` and `GROUPS`. Variables local to the test are lowercase
  with underscores.
- Functions defined by the testing framework, including helpers, have a leading
  underscore. E.g., `_have_scsi_debug`. Functions local to the test or category
  should not have a leading underscore.
- Use the bash `[[ ]]` form of tests instead of `[ ]`.
- Always quote variable expansions unless the variable is a number or inside of
  a `[[ ]]` test.
- Use the `$()` form of command substitution instead of backticks.
- Use bash for loops instead of `seq`. E.g., `for ((i = 0; i < 10; i++))`, not
  `for i in $(seq 0 9)`.
