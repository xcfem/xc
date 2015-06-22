set size 1.0, 0.6
set title "Serie temporal: "
set terminal postscript portrait enhanced color dashed lw 1 "Helvetica" 14
set output "dfree.ps"
set grid
plot  "DFree.out" using 1:2 title 'x' with lines


