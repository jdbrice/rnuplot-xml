set terminal pdfcairo enhanced size 8.5,8.5 
set output "datafiles1.pdf"

#set terminal qt
#set output
set termoption enhanced
set encoding utf8
load "turbo.pal"
set logscale cb
set cbtics nomirror
set xlabel "ZDC East ADC Sum π" font "Helvetica Regular, 46" offset screen 0,-0.02
set ylabel "ZDC West ADC Sum π^+π^-  π" 
set title "β ⇒ π 〈P_⊥〉ππππ" font "Symbol Regular,36" 
set view map
set contour base
# set rmargin at screen 0.90
splot zdc u 1:2:($3 <= 0 ? NaN : $3) with pm3d 