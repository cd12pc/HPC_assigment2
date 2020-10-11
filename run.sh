#!/bin/bash



echo "------- Running 1e4 T 1 ------- "
echo "For Pass 0.26 s (260 ms)"
hyperfine  --warmup 10 "bin/run.a data/test_data/cell_e4 -t1 > results/cell_e4"
echo "------- Running 1e5 T 5 ------- "
echo "For Pass 10.0 s (10 000 ms)" 
hyperfine --warmup 1 "bin/run.a data/test_data/cell_e5 -t5 > results/cell_e5_t5"
echo "------- Running 1e5 T 10 ------- "
echo "For Pass 5.3 s (5 300 ms)"
hyperfine --warmup 1 "bin/run.a data/test_data/cell_e5 -t10 > results/cell_e5_t5"
echo "------- Running 1e5 T 20 ------- "
echo "For Pass 2.8 s (2 800 ms)"
hyperfine --warmup 1 "bin/run.a data/test_data/cell_e5 -t20 > results/cell_e5_t20"
