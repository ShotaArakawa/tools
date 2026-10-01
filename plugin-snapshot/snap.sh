#!/usr/bin/env bash
# JUCEプラグインのエディタのスクリーンショットをPNGで書き出す。
#
#   snap.sh <プラグインのディレクトリ> <out.png> [--scale S] [paramID=value ...]
#   snap.sh <プラグインのディレクトリ> --list        # パラメータID・範囲の一覧
#
# 初回はプラグインごとにJUCEを含めてビルドするため数分かかる(2回目以降は差分ビルドのみ)。
# 相対パスの<out.png>は呼び出し時のカレントディレクトリ基準。
set -euo pipefail

if [[ $# -lt 2 ]]; then
    sed -n '2,8p' "$0" | sed 's/^# \{0,1\}//'
    exit 1
fi

TOOL_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PLUGIN_DIR="$(cd "$1" && pwd)"
shift

TARGET="$(grep -oP 'juce_add_plugin\s*\(\s*\K[A-Za-z0-9_]+' "$PLUGIN_DIR/CMakeLists.txt" | head -1)"
if [[ -z "$TARGET" ]]; then
    echo "juce_add_plugin が $PLUGIN_DIR/CMakeLists.txt に見つかりません" >&2
    exit 1
fi

BUILD_DIR="$TOOL_DIR/build/$TARGET"
if [[ ! -f "$BUILD_DIR/CMakeCache.txt" ]]; then
    cmake -S "$TOOL_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release \
          -DPLUGIN_DIR="$PLUGIN_DIR" -DPLUGIN_TARGET="$TARGET" > /dev/null
fi
cmake --build "$BUILD_DIR" --target plugin-snapshot -j"$(nproc)" > "$BUILD_DIR/build.log" 2>&1 \
    || { tail -30 "$BUILD_DIR/build.log" >&2; exit 1; }

"$BUILD_DIR/plugin-snapshot" "$@"
