#!/usr/bin/env bash
set -u

platlang="$1"
case_base="$2"

actual_out="$(mktemp)"
actual_err="$(mktemp)"

cleanup() {
    rm -f "$actual_out" "$actual_err"
}
trap cleanup EXIT

if [ -f "$case_base.args" ]; then
    # shellcheck disable=SC2207
    args=($(cat "$case_base.args"))
else
    args=()
fi

"$platlang" "${args[@]}" "$case_base.plat" >"$actual_out" 2>"$actual_err"
actual_exit=$?

expected_exit="$(cat "$case_base.exit")"
if [ "$actual_exit" != "$expected_exit" ]; then
    echo "expected exit $expected_exit, got $actual_exit" >&2
    exit 1
fi

if ! diff -u "$case_base.out" "$actual_out"; then
    exit 1
fi

if ! diff -u "$case_base.err" "$actual_err"; then
    exit 1
fi
