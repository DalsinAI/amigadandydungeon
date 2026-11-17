#!/bin/sh
# The upstream source and data, ready to build: package/dd.lock's commit of
# jackpal/Dandy-Dungeon is fetched into DIR/upstream (once), then
#   DIR/src   dandy-c++/Dandy.cpp as src/dandy.cpp, DOS's CRs taken out,
#             amiga/patches applied;
#   DIR/data  the game's files: Dandy.bmp (the tiles) and levels/level.a
#             to level.z.
#   sh package/prepare-source.sh DIR
# Apache-2.0, like Dandy Dungeon. Copyright (c) 2026 AmiCradle Team.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
ROOT=$(CDPATH= cd -- "$HERE/.." && pwd)
. "$HERE/dd.lock"
D=${1:?DIR}
mkdir -p "$D"
U="$D/upstream"
if [ "$(git -C "$U" rev-parse HEAD 2>/dev/null)" != "$DD_COMMIT" ]; then
    rm -rf "$U"; mkdir -p "$U"
    git -C "$U" init -q
    git -C "$U" fetch -q --depth 1 "$DD_GIT" "$DD_COMMIT"
    git -C "$U" checkout -q FETCH_HEAD
fi
[ "$(git -C "$U" rev-parse HEAD)" = "$DD_COMMIT" ] || { echo "dandy: commit mismatch" >&2; exit 1; }

rm -rf "$D/src" "$D/data"
mkdir -p "$D/src" "$D/data/levels"
tr -d '\r' < "$U/dandy-c++/Dandy.cpp" > "$D/src/dandy.cpp"
for p in "$ROOT"/amiga/patches/*.patch; do
    patch -s -d "$D/src" -p1 < "$p"
done
cp "$U/dandy-c++/dandy.bmp" "$D/data/Dandy.bmp"
for l in a b c d e f g h i j k l m n o p q r s t u v w x y z; do
    cp "$U/dandy-c++/levels/level.$l" "$D/data/levels/level.$l"
done
cp "$U/LICENSE" "$D/LICENSE"
