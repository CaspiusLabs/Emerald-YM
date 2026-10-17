#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# The video's clips (tests/film/*.amb) from the host simulator, each as three MP4s in build/film/: NAME.mp4
# (1920x1080: the panel, the screen large beside it), NAME-panel.mp4 (1920x1080: the panel alone, large) and
# NAME-screen.mp4 (1080x1080: the screen alone). The same audio in each, and in NAME.wav.
#   tools/make_film.sh [NAME...]        (default: every clip; after host/build_host.sh)
set -e
cd "$(dirname "$0")/.."
PY="${PYTHON:-.venv/bin/python3}"
mkdir -p build/film
[ $# -gt 0 ] || set -- $(cd tests/film && ls *.amb | sed 's/\.amb$//')
for n in "$@"; do
    build/host/amb_host "tests/film/$n.amb" build/film | grep -v '^said' | tail -1
    "$PY" tools/film.py "build/film/$n.film" "build/film/$n.wav" "build/film/$n.mp4" --layout split
    "$PY" tools/film.py "build/film/$n.film" "build/film/$n.wav" "build/film/$n-panel.mp4" --layout panel
    "$PY" tools/film.py "build/film/$n.film" "build/film/$n.wav" "build/film/$n-screen.mp4" --layout screen
    rm -f "build/film/$n.film"
done
