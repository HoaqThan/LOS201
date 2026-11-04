set terminal pngcairo size 1400,800 enhanced font "Arial,12"
set output "/home/drearlattice/embedded_lab3/logs/latency_histogram.png"

set title "LAB03 - Real-Time Latency Histogram"
set xlabel "Latency (microseconds)"
set ylabel "Number of Samples"

set grid
set key top right
set xrange [0:300]

set datafile commentschars "#"

plot \
"/home/drearlattice/embedded_lab3/logs/hist_standard.txt" \
using 1:($2+$3+$4+$5) \
with lines linewidth 2 title "Normal Load", \
"/home/drearlattice/embedded_lab3/logs/hist_stress.txt" \
using 1:($2+$3+$4+$5) \
with lines linewidth 2 title "Stress Load"
