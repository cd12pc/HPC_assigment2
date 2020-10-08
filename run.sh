#!/bin/bash


echo "------- Running 1e4 T 1 ------- "
echo "For Pass 0.26 s"
hyperfine "bin/run.a data/test_data/cell_e4 1"
echo "------- Running 1e5 T 5 ------- "
echo "For Pass 10.0 s"
hyperfine "bin/run.a data/test_data/cell_e5 5"
echo "------- Running 1e5 T 10 ------- "
echo "For Pass 5.3 s"
hyperfine "bin/run.a data/test_data/cell_e5 10"
echo "------- Running 1e5 T 20 ------- "
echo "For Pass 2.8 s"
hyperfine "bin/run.a data/test_data/cell_e5 20"
