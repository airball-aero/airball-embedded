# This file strips empty lines, status lines (with a "#"), and
# lines with invalid data out of the raw log file. The log can
# contain invalid lines because the writing process gets no
# warning when the power is turned off!
cat airball.log \
    | grep -a -v '#' \
    | grep -a -v '^$' \
    | grep -a -v '[^0-9\.,-]' \
    | ../../display/build/src/airdata_plot \
    | sed 's/nan/0/g' \
    > airball_log_computed.csv
