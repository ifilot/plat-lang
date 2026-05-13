#!/usr/bin/env bash
set -eu

platlang="$1"
case_base="$2"

build_dir="$(cd "$(dirname "$platlang")" && pwd)"
platlang_name="$(basename "$platlang")"
package_dir="$(mktemp -d)"

cleanup() {
    rm -rf "$package_dir"
}
trap cleanup EXIT

mkdir -p "$package_dir/package/bin"
mkdir -p "$package_dir/package/lib"
mkdir -p "$package_dir/outside"

cp "$platlang" "$package_dir/package/bin/$platlang_name"

if [ -d "$build_dir/capabilities" ]; then
    cp -R "$build_dir/capabilities" "$package_dir/package/capabilities"
fi

if [ -f "$build_dir/libplatlang_core.so" ]; then
    cp "$build_dir/libplatlang_core.so" "$package_dir/package/lib/"
fi

if [ -f "$build_dir/platlang_core.dll" ]; then
    cp "$build_dir/platlang_core.dll" "$package_dir/package/bin/"
fi

if [ -f "$build_dir/libplatlang_core.dylib" ]; then
    cp "$build_dir/libplatlang_core.dylib" "$package_dir/package/lib/"
fi

(
    cd "$package_dir/outside"
    "$package_dir/package/bin/$platlang_name" "$case_base.plat"
)
