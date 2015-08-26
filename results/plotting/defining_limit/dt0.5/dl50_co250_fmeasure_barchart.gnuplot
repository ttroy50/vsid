set terminal svg size 500,240 fname "Gill Sans" fsize 9 rounded solid

# Line style for axes
set style line 80 lt 0
set style line 80 lt rgb "#808080"

# Line style for grid
set style line 81 lt 3  # dashed
set style line 81 lt rgb "#808080" lw 0.5  # grey

#set grid back linestyle 81
set border 3 back linestyle 80 # Remove border on top and right.

set key outside bottom left

set xtics nomirror rotate by -45

set tic scale 0
set grid ytics lt rgb "#808080" lw 0.5 


# Thinner, filled bars
set boxwidth 0.9
set style fill solid


set output "dl50_co250_fmeasure_barchart.svg"

set ylabel "Percent"

set yrange [0:100]

set style data histogram
plot "../results/plotting/defining_limit/dt0.5/dl50_co250_fmeasure_barchart.data" using 2:xtic(1) title "Precision" lt rgb "#A00000", \
    "" using 3 title "Recall" lt rgb "#00A000", \
    "" using 4 title "F-Measure" lt rgb "#5060D0", \