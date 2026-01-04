#!/bin/bash
status=$(playerctl status 2>/dev/null)

if [ "$status" = "Playing" ]; then
    echo "  $(playerctl metadata --format '{{ title }} - {{ artist }}') "
else
    echo ""
fi
