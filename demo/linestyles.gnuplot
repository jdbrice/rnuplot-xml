# set terminal canvas  rounded size 600,400 enhanced fsize 10 lw 1.6 fontscale 1 name "rainbow_1" jsdir "."
# set output 'rainbow.1.js'
set terminal qt
set label 1 "set style line 1 lt rgb \"red\" lw 3" at -0.400000, -0.250000, 0.00000 left norotate back textcolor rgb "red"  nopoint
set label 2 "set style line 2 lt rgb \"orange\" lw 2" at -0.400000, -0.350000, 0.00000 left norotate back textcolor rgb "orange"  nopoint
set label 3 "set style line 3 lt rgb \"yellow\" lw 3" at -0.400000, -0.450000, 0.00000 left norotate back textcolor rgb "yellow"  nopoint
set label 4 "set style line 4 lt rgb \"green\" lw 2" at -0.400000, -0.550000, 0.00000 left norotate back textcolor rgb "green"  nopoint
set label 5 "set style line 5 lt rgb \"cyan\" lw 3" at -0.400000, -0.650000, 0.00000 left norotate back textcolor rgb "cyan"  nopoint
set label 6 "set style line 6 lt rgb \"blue\" lw 2" at -0.400000, -0.750000, 0.00000 left norotate back textcolor rgb "blue"  nopoint
set label 7 "set style line 7 lt rgb \"violet\" lw 3" at -0.400000, -0.850000, 0.00000 left norotate back textcolor rgb "violet"  nopoint
set style increment default
set style line 1  linecolor rgb "red"  linewidth 3.000 dashtype solid pointtype 1 pointsize default pointinterval 0 
set style line 2  linecolor rgb "orange"  linewidth 2.000 dashtype solid pointtype 2 pointsize default pointinterval 0 
set style line 3  linecolor rgb "yellow"  linewidth 3.000 dashtype solid pointtype 3 pointsize default pointinterval 0 
set style line 4  linecolor rgb "green"  linewidth 2.000 dashtype solid pointtype 4 pointsize default pointinterval 0 
set style line 5  linecolor rgb "cyan"  linewidth 3.000 dashtype solid pointtype 5 pointsize default pointinterval 0 
set style line 6  linecolor rgb "blue"  linewidth 2.000 dashtype solid pointtype 6 pointsize default pointinterval 0 
set style line 7  linecolor rgb "violet"  linewidth 3.000 dashtype solid pointtype 7 pointsize default pointinterval 0 

load "turbo.pal"

unset xtics
unset ytics
set title "Terminal-independent RGB colors in 2D" 
set xlabel "Implemented using built-in rgb color names\n(only works for terminals that can do full rgb color)" 
set xrange [ -0.500000 : 3.50000 ] noreverse nowriteback
set x2range [ * : * ] noreverse writeback
set yrange [ -1.00000 : 1.40000 ] noreverse nowriteback
set y2range [ * : * ] noreverse writeback
set zrange [ * : * ] noreverse writeback
set cbrange [ * : * ] noreverse writeback
set rrange [ * : * ] noreverse writeback
set bmargin  7
plot cos(x)     ls 1 title 'red',        cos(x-.2)  ls 2 title 'orange',     cos(x-.4)  ls 3 title 'yellow',     cos(x-.6)  ls 4 title 'green',      cos(x-.8)  ls 5 title 'cyan',       cos(x-1.)  ls 6 title 'blue',       cos(x-1.2) ls 7 title 'violet'