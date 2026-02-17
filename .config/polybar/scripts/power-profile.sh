#!/bin/bash

CURRENT=$(powerprofilesctl get)

if [ "$1" = "--toggle" ]; then
    if [ "$CURRENT" = "power-saver" ]; then
        powerprofilesctl set balanced
    elif [ "$CURRENT" = "balanced" ]; then
        powerprofilesctl set performance
    elif [ "$CURRENT" = "performance" ]; then
        powerprofilesctl set power-saver
    fi
    
    CURRENT=$(powerprofilesctl get)
fi

if [ "$CURRENT" = "power-saver" ]; then
    echo " Power Saver"
elif [ "$CURRENT" = "balanced" ]; then
    echo " Balanced"
elif [ "$CURRENT" = "performance" ]; then
    echo " Performance"
else
    echo "$CURRENT"
fi
