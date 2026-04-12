#!/bin/bash
# Extract colors from Xresources and write them to a temp file i3 can include
xrdb -query | grep "*color" | sed 's/*//' > /tmp/i3-colors
