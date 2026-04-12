#!/bin/sh

# 1. Extract colors from Xresources and format them for i3
# This converts: *color0: #123456  --> set $color0 #123456
xrdb -query | grep -E "^\*color[0-9]" | sed 's/*//; s/:/ /; s/^/set $/' > /tmp/i3-colors

# 2. Add some custom aliases if you want
# This way you can use $bg in your config instead of $color0
echo "set \$bg \$color0" >> /tmp/i3-colors
echo "set \$fg \$color7" >> /tmp/i3-colors

# 3. Reload i3 to apply the new file
i3-msg reload
