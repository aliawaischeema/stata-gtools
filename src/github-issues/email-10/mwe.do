clear
set more off
set seed 1
set obs 2
g y = 1.23
l
preserve
    gcollapse (count) cy = y (first) fy = y, freq(z)
    l
restore
    gcollapse (first) fy = y (count) cy = y, freq(z)
    l
