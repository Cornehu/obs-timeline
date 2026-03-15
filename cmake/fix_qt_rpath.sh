#!/bin/bash
# Redirects Qt framework links from Homebrew absolute paths to @rpath
# so the plugin uses OBS's bundled Qt instead of the system Homebrew Qt.
BINARY="$1"
[ -z "$BINARY" ] && { echo "Usage: $0 <binary>"; exit 1; }

for LIB in QtCore QtGui QtWidgets QtNetwork QtSvg; do
    OLD=$(otool -L "$BINARY" 2>/dev/null | grep "opt/homebrew.*${LIB}" | awk '{print $1}')
    if [ -n "$OLD" ]; then
        install_name_tool -change "$OLD" \
            "@rpath/${LIB}.framework/Versions/A/${LIB}" \
            "$BINARY"
    fi
done
