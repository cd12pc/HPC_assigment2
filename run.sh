#!/bin/bash


echo "------- Running Bo0_adress ------- "
bin/Bo0_run -a results/Bo0 -g Write
bin/Bo0_run -a results/Bo0 -g Read
echo "==================================="
echo "------- Running Bo2_adress ------- "
bin/Bo2_run -a results/Bo2 -g Write
bin/Bo2_run -a results/Bo2 -g Read
echo "==================================="
echo "------- Running Bo2n_adress ------- "
bin/Bo2n_run -a results/Bo2n -g Write
bin/Bo2n_run -a results/Bo2n -g Read
echo "==================================="
