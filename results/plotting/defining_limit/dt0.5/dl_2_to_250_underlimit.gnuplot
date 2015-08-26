set terminal svg size 480,240 fname "Gill Sans" fsize 9 rounded dashed

# Line style for axes
set style line 80 lt 0
set style line 80 lt rgb "#808080"

# Line style for grid
set style line 81 lt 3  # dashed
set style line 81 lt rgb "#808080" lw 0.5  # grey

set grid back linestyle 81
set border 3 back linestyle 80 # Remove border on top and right.  These
             # borders are useless and make it harder
             # to see plotted lines near the border.
    # Also, put it in grey; no need for so much emphasis on a border.
set xtics nomirror
set ytics nomirror

#set log x
#set mxtics 10    # Makes logscale look good.

# Line styles: try to pick pleasing colors, rather
# than strictly primary colors or hard-to-see colors
# like gnuplot's default yellow.  Make the lines thick
# so they're easy to see in small plots in papers.
set style line 1 lt 1
set style line 2 lt 1
set style line 3 lt 1
set style line 4 lt 1
set style line 5 lt 1
set style line 6 lt 1
set style line 7 lt 1
set style line 8 lt 1
set style line 9 lt 1
set style line 1 lt rgb "#A00000" lw 2 pt 7
set style line 2 lt rgb "#00A000" lw 2 pt 9
set style line 3 lt rgb "#5060D0" lw 2 pt 5
set style line 4 lt rgb "#F25900" lw 2 pt 13
set style line 5 lt rgb "#000033" lw 2 pt 16
set style line 6 lt rgb "#330000" lw 2 pt 19
set style line 7 lt rgb "#009999" lw 2 pt 21
set style line 8 lt rgb "#606060" lw 2 pt 28
set style line 9 lt rgb "#00000A" lw 2 pt 27
set style line 10 lt rgb "#0000AA" lw 2 pt 25

set output "dl_2_to_250_underlimit.svg"

set xlabel "Defining Limit" 
set ylabel "% Under Limit" offset 2.75,2


set key outside bottom left box


set xrange [0:250]
set yrange [0:100]

plot "../results/plotting/defining_limit/dt0.5/dl_2_to_250_underlimit.data" \
   using 1:2 title "Youtube" w lp ls 1, \
"" using 1:3 title "HTTP-Progressive" w lp ls 2, \
"" using 1:4 title "HLS" w lp ls 3, \
"" using 1:5 title "RTMP" w lp ls 4, \
"" using 1:6 title "RTP" w lp ls 5, \
"" using 1:7 title "RTCP" w lp ls 6, \
"" using 1:8 title "RTSP" w lp ls 7, \
"" using 1:9 title "HTTP" w lp ls 8, \
"" using 1:10 title "HTTPS" w lp ls 9, \
"" using 1:11 title "DNS" w lp ls 10