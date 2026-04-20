#!/bin/sh

for i in *; do
    if [ "$i" != "tests" ] && [ "$i" != "src" ] && [ "$i" != "Makefile" ]; then
        rm -rf "$i"
    fi
done

