#!/bin/bash

# Get size of terminal.
read -r max_row max_col <<< $(stty size < /dev/tty)

# Convert stdin to array of bytes.
content=$(hexdump -ve '/1 "%02X "')

# Coordinates of current character.
col=0; row=0

# Triples of character, column and row.
triples=""

# Assign coordinate to each character that fits into terminal window.
for char in $content; do
    if [ $char = "0A" ]; then
        # Handle newline.
        if [ $row -lt $(( $max_row - 2)) ]; then
            # Advance to the next line.
            row=$(( $row + 1 ))
            col=0 
        else
            # Give up to read content if `y` coordinate exceeds terminal size.
            break
        fi
    elif [ $col -lt $max_col ]; then
        # Handle non-newline character within terminal window.
        if [ $char != "20" ]; then
            # Insert new triple for non-space characters.
            triples="$char $col $row\n$triples"
        fi
        # Advance to the next column.
        col=$(( $col + 1 ))
    else
        # Skip character if `x` coordinate exceeds terminal size.
        continue
    fi
done

# Shuffle triples.
random_triples=$(mktemp)
shuf <<< $(echo -ne $triples) > $random_triples

# Set delay.
delay=0.01
if [ $1 ]; then
    delay=$1
fi

# Print characters at specific positions.
tput clear

while read char col row; do
    tput cup $row $col ; echo -ne "\x$char"
    sleep $delay
done < $random_triples

# Remove temporary file for `shuf` output.
rm -f $random_triples

# Put cursor on the last line.
tput cup $(( $max_row - 1 )) 0
