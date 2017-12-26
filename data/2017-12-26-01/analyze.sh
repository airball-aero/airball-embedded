cat plot.in | grep -a -v '#' | grep -a -v '^$' > plot.csv
gnuplot --persist -c plot.gpl
