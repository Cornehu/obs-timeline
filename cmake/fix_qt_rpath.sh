#!/bin/bash
# Redirige les liens Qt de Homebrew (absolu) vers @rpath (Qt bundlé dans OBS)
BINARY="$1"
for LIB in QtCore QtGui QtWidgets QtNetwork; do
    OLD=$(otool -L "$BINARY" 2>/dev/null | grep "opt/homebrew.*$LIB" | awk '{print $1}')
    if [ -n "$OLD" ]; then
        install_name_tool -change "$OLD" "@rpath/$LIB.framework/Versions/A/$LIB" "$BINARY"
    fi
done
