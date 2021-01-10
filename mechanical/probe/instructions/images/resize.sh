#!/bin/bash

ls *.jpg | grep -v -- '-icon' | while read f ; do
    sm=$(echo $f | sed 's/\.jpg/\-icon\.jpg/g')
    convert $f -resize 320 $sm
done
