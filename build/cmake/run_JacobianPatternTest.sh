#!/bin/bash

# Run test suite in data/tests directory, all project files with .nandrad file extension
(cd ../../data/tests; OMP_NUM_THREADS=1 ../../scripts/TestSuite/run_JacobiMatrixPatternTest.py -p ./ -s ../../bin/release/NandradSolver -e nandrad)

