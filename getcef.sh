#!/bin/bash

set -e

wincef="cef_binary_123.0.13+gfc703fb+chromium-123.0.6312.124_windows64_minimal"
lincef="cef_binary_123.0.13+gfc703fb+chromium-123.0.6312.124_linux64_minimal"

rm -rf "${wincef}" "${lincef}" "reincarnation/webview"

curl https://cef-builds.spotifycdn.com/${lincef}.tar.bz2 -L -\# | tar xj

mkdir -p reincarnation/webview/linux

mv ${lincef}/Release/* ${lincef}/Resources/* reincarnation/webview/linux

curl https://cef-builds.spotifycdn.com/${wincef}.tar.bz2 -L -\# | tar xj

(cd "$wincef/cmake" && patch) < cef-mingw-support.patch

mkdir -p reincarnation/webview/windows

mv ${wincef}/Release/*.{bin,dll,json} ${wincef}/Resources/* reincarnation/webview/windows
for file in reincarnation/webview/windows/*; do
    ln -s ../../$file "${wincef}/Release/$(basename "$file")"
done

for file in reincarnation/webview/linux/*; do
    ln -s ../../$file "${lincef}/Release/$(basename "$file")"
done

strip --strip-unneeded reincarnation/webview/linux/chrome-sandbox reincarnation/webview/linux/*.so reincarnation/webview/linux/*.so*

