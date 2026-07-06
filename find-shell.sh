#!/bin/sh

HERE="$(dirname "$0")"
LOG="$HERE/shell-scan.log"
exec >"$LOG" 2>&1

echo "=== Shell Scan Started ==="

# Common shell locations
for path in \
    /bin/bash \
    /usr/bin/bash \
    /bin/sh \
    /usr/bin/sh \
    /bin/ash \
    /usr/bin/ash \
    /bin/dash \
    /usr/bin/dash \
    /bin/zsh \
    /usr/bin/zsh \
    /bin/busybox \
    /usr/bin/busybox
do
    if [ -f "$path" ]; then
        echo "FOUND: $path"
    else
        echo "missing: $path"
    fi
done

echo "=== Shell Scan Complete ==="
