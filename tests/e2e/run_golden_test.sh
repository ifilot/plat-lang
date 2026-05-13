#!/usr/bin/env bash
set -u

platlang="$1"
case_base="$2"

actual_out="$(mktemp)"
actual_err="$(mktemp)"
temp_source=""
temp_input=""

cleanup() {
    rm -f "$actual_out" "$actual_err"
    if [ -n "$temp_source" ]; then
        rm -f "$temp_source"
    fi
    if [ -n "$temp_input" ]; then
        rm -f "$temp_input"
    fi
}
trap cleanup EXIT

if [ -f "$case_base.args" ]; then
    # shellcheck disable=SC2207
    args=($(cat "$case_base.args"))
else
    args=()
fi

source_file="$case_base.plat"

if [ -f "$case_base.newlines" ]; then
    temp_source="$(mktemp)"
    case "$(cat "$case_base.newlines")" in
        crlf)
            perl -0pe 's/\n/\r\n/g' "$case_base.plat" >"$temp_source"
            ;;
        cr)
            perl -0pe 's/\n/\r/g' "$case_base.plat" >"$temp_source"
            ;;
        *)
            echo "unknown newline mode in $case_base.newlines" >&2
            exit 1
            ;;
    esac
    source_file="$temp_source"
fi

input_file="$case_base.in"
if [ -f "$case_base.input_newlines" ]; then
    temp_input="$(mktemp)"
    case "$(cat "$case_base.input_newlines")" in
        crlf)
            perl -0pe 's/\n/\r\n/g' "$case_base.in" >"$temp_input"
            ;;
        cr)
            perl -0pe 's/\n/\r/g' "$case_base.in" >"$temp_input"
            ;;
        *)
            echo "unknown newline mode in $case_base.input_newlines" >&2
            exit 1
            ;;
    esac
    input_file="$temp_input"
fi

if [ -f "$case_base.in" ]; then
    "$platlang" "${args[@]}" "$source_file" <"$input_file" >"$actual_out" 2>"$actual_err"
else
    "$platlang" "${args[@]}" "$source_file" >"$actual_out" 2>"$actual_err"
fi
actual_exit=$?

expected_exit="$(cat "$case_base.exit")"
if [ "$actual_exit" != "$expected_exit" ]; then
    echo "expected exit $expected_exit, got $actual_exit" >&2
    exit 1
fi

expected_out=/dev/null
if [ -f "$case_base.out" ]; then
    expected_out="$case_base.out"
fi

if ! diff -u --strip-trailing-cr "$expected_out" "$actual_out"; then
    exit 1
fi

expected_err=/dev/null
if [ -f "$case_base.err" ]; then
    expected_err="$case_base.err"
fi

if ! diff -u --strip-trailing-cr "$expected_err" "$actual_err"; then
    exit 1
fi
