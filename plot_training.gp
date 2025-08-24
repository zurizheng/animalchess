set terminal png size 1200,800
set output 'training_progress.png'
set multiplot layout 2,1
set title 'AI Training Progress - Rewards'
set xlabel 'Game Number'
set ylabel 'Average Reward'
set grid
set datafile separator ','
plot 'training_data_gnuplot.csv' using 1:2 with lines title 'Player 1' lw 2, \
     'training_data_gnuplot.csv' using 1:3 with lines title 'Player 2' lw 2
set title 'AI Training Progress - Win Rate'
set ylabel 'Win Rate (%)'
set yrange [0:100]
plot 'training_data_gnuplot.csv' using 1:($4*100) with lines title 'Player 1 Win Rate' lw 2
unset multiplot
